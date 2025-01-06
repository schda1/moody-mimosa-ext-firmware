#include <ComparatorDriver.hpp>

#include <Parameter.hpp>
#include <HalWrapper/DigitalOutput.hpp>

ComparatorDriver::ComparatorDriver(PwmHandler* pwm_handler, ParameterHandler* parameter_handler) : pwm_handler(pwm_handler), parameter_handler(parameter_handler)
{
    outputs[0] = new DigitalOutput(PORT_CMP, PIN_CMP_COLD);
    outputs[1] = new DigitalOutput(PORT_CMP, PIN_CMP_HOT);
    outputs[2] = new DigitalOutput(PORT_CMP, PIN_CMP_CALM);
    outputs[3] = new DigitalOutput(PORT_CMP, PIN_CMP_LOUD);
    outputs[4] = new DigitalOutput(PORT_CMP, PIN_CMP_DARK);
    outputs[5] = new DigitalOutput(PORT_CMP, PIN_CMP_BRIGHT);
}

ComparatorDriver::~ComparatorDriver()
{
    for (uint8_t i = 0; i < 6; i++) {
        delete outputs[i];
    }
}

void ComparatorDriver::set_duty_cycle(uint8_t channel_nr, uint16_t duty_cycle)
{
    if (channel_nr < 6) {
        *duty_cycles[channel_nr] = duty_cycle;
    }
}

void ComparatorDriver::init()
{
    duty_cycles[0] = parameter_handler->get_or_create<uint16_t>(PARAM_COLD, PARAM_COLD_DEFAULT);
    duty_cycles[1] = parameter_handler->get_or_create<uint16_t>(PARAM_HOT, PARAM_HOT_DEFAULT);
    duty_cycles[2] = parameter_handler->get_or_create<uint16_t>(PARAM_CALM, PARAM_CALM_DEFAULT);
    duty_cycles[3] = parameter_handler->get_or_create<uint16_t>(PARAM_LOUD, PARAM_LOUD_DEFAULT);
    duty_cycles[4] = parameter_handler->get_or_create<uint16_t>(PARAM_DARK, PARAM_DARK_DEFAULT);
    duty_cycles[5] = parameter_handler->get_or_create<uint16_t>(PARAM_BRIGHT, PARAM_BRIGHT_DEFAULT);

    for (uint8_t i = 0; i < 6; i++) {
        pwm_handler->attach(outputs[i], *duty_cycles[i]);
    }

    for (uint8_t i = 0; i < 6; i++) {
        outputs[i]->init();
        pwm_handler->set_duty_cycle(i, *duty_cycles[i]);
    }

    pwm_handler->init();
}

void ComparatorDriver::enable(bool enable)
{
    pwm_handler->enable(enable);
}

void ComparatorDriver::print()
{
    printf("ComparatorDriver:\n");
    for (uint8_t i = 0; i < 6; i++) {
        printf("Channel %d: %d\n", i, *duty_cycles[i]);
    }
}

void ComparatorDriver::set_cold_thresh(uint16_t value)
{
    *duty_cycles[0] = value;
    pwm_handler->set_duty_cycle(0, value);
}

void ComparatorDriver::set_hot_thresh(uint16_t value)
{
    *duty_cycles[1] = value;
    pwm_handler->set_duty_cycle(1, value);
}

void ComparatorDriver::set_quiet_thresh(uint16_t value)
{
    *duty_cycles[2] = value;
    pwm_handler->set_duty_cycle(2, value);
}

void ComparatorDriver::set_loud_thresh(uint16_t value)
{
    *duty_cycles[3] = value;
    pwm_handler->set_duty_cycle(3, value);
}

void ComparatorDriver::set_dark_thresh(uint16_t value)
{
    *duty_cycles[4] = value;
    pwm_handler->set_duty_cycle(4, value);
}

void ComparatorDriver::set_bright_thresh(uint16_t value)
{
    *duty_cycles[5] = value;
    pwm_handler->set_duty_cycle(5, value);
}