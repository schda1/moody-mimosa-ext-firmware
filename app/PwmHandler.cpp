#include <PwmHandler.hpp>
#include <functional>

PwmHandler::PwmHandler(ITimer* pwm_timer)
{
    timer = pwm_timer;
    n_channels = 0;
    enabled = false;

    timer->set_callback([this]() { this->callback(); });
}

void PwmHandler::attach(IDigitalOutput* output, uint16_t duty_cycle)
{
    /* Same channel cannot be attached twice */
    if (check_if_present(output)) {
        return;
    }

    if (n_channels < N_PWM_CHANNELS) {
        channels[n_channels].output = output;
        channels[n_channels].duty_cycle = duty_cycle;
        channels[n_channels].cycle = 0;
        n_channels++;
    }
}

void PwmHandler::init()
{
    for (uint8_t i = 0; i < n_channels; i++) {
        channels[i].output->init();
    }

    timer->init();
}

void PwmHandler::set_duty_cycle(uint8_t channel_nr, uint16_t duty_cycle)
{
    if (channel_nr < n_channels) {
        channels[channel_nr].duty_cycle = duty_cycle;
        channels[channel_nr].cycle = 0;
    }
}

void PwmHandler::enable(bool enable)
{
    if (enable && !enabled) {
        timer->start();
    } else if (!enable && enabled) {
        timer->stop();
    }

    enabled = enable;
}

void PwmHandler::callback()
{
    if (!enabled) {
        for (uint8_t i = 0; i < n_channels; i++) {
            channels[i].output->set(false);
        }
        return;
    }

    for (uint8_t i = 0; i < n_channels; i++) {
        channels[i].output->set(channels[i].cycle < channels[i].duty_cycle);
        channels[i].cycle = (channels[i].cycle + 1) % 100;
    }
}

bool PwmHandler::check_if_present(IDigitalOutput* output) const
{
    for (uint8_t i = 0; i < n_channels; i++) {
        if (channels[i].output == output) {
            return true;
        }
    }

    return false;
}
