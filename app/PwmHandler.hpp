#pragma once

#include <common.hpp>
#include <HalWrapper/ITimer.hpp>
#include <HalWrapper/IDigitalOutput.hpp>

#define N_PWM_CHANNELS (6)

struct PwmChannel
{
    IDigitalOutput* output; /**< Digital output object  */
    uint8_t duty_cycle;     /**< Duty cycle in %        */
    uint8_t cycle;          /**< Current cycle          */
};

class PwmHandler
{
public:
    PwmHandler(ITimer* timer);
    virtual ~PwmHandler() {}

    void attach(IDigitalOutput* output, uint16_t duty_cycle);
    void init();

    void set_duty_cycle(uint8_t channel_nr, uint16_t duty_cycle);
    void enable(bool enable);
    void callback();

    uint8_t attached_channels() const{ return n_channels; }

private:
    bool check_if_present(IDigitalOutput* output) const;

    ITimer* timer;
    PwmChannel channels[N_PWM_CHANNELS];
    uint8_t n_channels;
    bool enabled;
};