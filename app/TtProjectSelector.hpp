#pragma once

#include <HalWrapper/IDigitalOutput.hpp>

class TtProjectSelector {

public:
    TtProjectSelector(IDigitalOutput* sel_rst_n, IDigitalOutput* sel_inc, IDigitalOutput* ena, uint16_t max_number = 512, uint16_t delay_ms = 10);

    void init();

    void load_project(uint16_t project_number);

    uint16_t get_loaded_project();

private:

    void send_pulse_pattern();

    IDigitalOutput* sel_rst_n;
    IDigitalOutput* sel_inc;
    IDigitalOutput* ena;

    uint16_t project_number;
    uint16_t max_number;
    uint16_t delay_ms;
};