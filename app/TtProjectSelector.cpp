#include <TtProjectSelector.hpp>

TtProjectSelector::TtProjectSelector(IDigitalOutput* sel_rst_n, IDigitalOutput* sel_inc, IDigitalOutput* ena, uint16_t max_number, uint16_t delay_ms) :
    sel_rst_n(sel_rst_n),
    sel_inc(sel_inc),
    ena(ena),
    max_number(max_number),
    delay_ms(delay_ms)
{ }

void TtProjectSelector::init() 
{
    sel_rst_n->init();
    sel_inc->init();
    ena->init();
}

void TtProjectSelector::load_project(uint16_t project_number) 
{

    if (project_number > this->max_number) {
        return;
    }

    this->project_number = project_number;
    
    send_pulse_pattern();
}

uint16_t TtProjectSelector::get_loaded_project() 
{
    return project_number;
}

void TtProjectSelector::send_pulse_pattern() 
{
    /* Disable and reset */
    ena->set(0);
    sel_inc->set(0);
    sel_rst_n->set(0);
    HAL_Delay(delay_ms);

    /* De-reset*/
    sel_rst_n->set(1);
    HAL_Delay(delay_ms);

    /* Send N pulses */
    for (int i = 0; i < project_number; i++) {
        sel_inc->set(1);
        HAL_Delay(delay_ms);
        sel_inc->set(0);
        HAL_Delay(delay_ms);
    }

    /* Enable */
    ena->set(1);
}
