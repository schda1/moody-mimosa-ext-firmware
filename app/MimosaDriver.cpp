
#include <MimosaDriver.hpp>
#include <Parameter.hpp>
#include <functional>

#define MASK_IS_PAUSED (1 << 0)
#define MASK_IS_STOPPED (1 << 1)
#define MASK_NOT_RUNNING (MASK_IS_PAUSED | MASK_IS_STOPPED)
#define MASK_HALF_CYCLE (1 << 2)
#define MASK_FULL_CYCLE (1 << 3)

MimosaDriver::MimosaDriver(PwmOutput* clk, IExtiInput *exti, IDigitalOutput* rst_n, PinObserver* observer, ParameterHandler* handler) :
    clk(clk),
    model_clk_exti(exti),
    rst_n(rst_n),
    pin_observer(observer),
    param_handler(handler)
{ }

void MimosaDriver::init()
{
    /* Initialize external interrupt for model clock */
    model_clk_exti->set_input_mode(DigitalInputMode::INPUT_PULL_DOWN);
    model_clk_exti->set_edge(InterruptEdge::BOTH);
    model_clk_exti->set_callback([this]() { this->callback_exti(); });
    model_clk_exti->init();
    
    /* Initialize timer for creating mimosa input clock */
    clk->set_prescaler(130);
    clk->init();

    rst_n->init();

    period_ms = param_handler->get_or_create<uint32_t>(PARAM_MIMOSA_PERIOD, PARAM_MIMOSA_PERIOD_DEFAULT);
    printf("Mimosa Period: %ld\n", *period_ms);
}

void MimosaDriver::start()
{
    clk->start();
    rst_n->set(1);
    status &= ~MASK_NOT_RUNNING;
    printf("Mimosa start \n");
}

void MimosaDriver::stop()
{
    clk->stop();
    rst_n->set(0);
    counter = 0;
    status |= MASK_IS_STOPPED;
}

void MimosaDriver::pause()
{
    clk->stop();
    status |= MASK_IS_PAUSED;
}

void MimosaDriver::update()
{
    if ((status & MASK_NOT_RUNNING) > 0) {
        return;
    }

    if ((status & MASK_HALF_CYCLE) > 0) {
        pin_observer->update();
        pin_observer->print();
        status &= ~MASK_HALF_CYCLE;
    } else if ((status & MASK_FULL_CYCLE) > 0) {
        status &= ~MASK_FULL_CYCLE;
    }
}

void MimosaDriver::set_period_ms(uint32_t period_ms)
{
    *this->period_ms = period_ms;
    set_period();
}

void MimosaDriver::enable_log(bool enable)
{
    pin_observer->enable_log(enable);
}

void MimosaDriver::set_period()
{
    // 
}

void MimosaDriver::callback_exti()
{
    if ((status & MASK_NOT_RUNNING) == 0) {
        
        counter++;

        if (clk->get()) {
            status |= MASK_HALF_CYCLE;
        } else {
            status |= MASK_FULL_CYCLE;
        }
    }
}