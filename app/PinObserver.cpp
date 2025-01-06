#include <PinObserver.hpp>


PinObserver::PinObserver(InputConfigFct config_fct) : config_fct(config_fct), pins({0, 0, 0, 0, 0}), log_on(false)
{
    memset(&config, 0, sizeof(config));
    memset(&pins, 0, sizeof(pins));
}

void PinObserver::configure(PortType type, GPIO_TypeDef* port, uint16_t mask)
{
    uint8_t id = (uint8_t)type;

    if (id < 4) {
        config[id].port = port;
        config[id].mask = mask;

        if (config_fct == nullptr) {
            return;
        }

        for (uint8_t i = 0; i < 16; i++) {
            if (mask & (1 << i)) {
                config_fct(port, i);
            }
        }
    }
}

void PinObserver::reset()
{
    pins.cycle = 0;
}

PinObservation PinObserver::get_pins() const
{
    return pins;
}

PortConfig PinObserver::get_config(PortType type) const
{
    return config[(uint8_t)type];
}

void PinObserver::update()
{
    pins.cycle++;
    pins.ui_in = read_bits(&config[0]);
    pins.uo_out = read_bits(&config[1]);
    pins.uoi = read_bits(&config[2]);
    pins.ctrl = read_bits(&config[3]);
}

void PinObserver::enable_log(bool enable)
{
    log_on = enable;
}

void PinObserver::print() const
{
    if (log_on) {
        uint8_t cs = calc_check_sum();

        printf("%lu,%u,%u,%u,%u,%u\n", pins.cycle, pins.ui_in, pins.uo_out, pins.uoi, pins.ctrl, cs);
    }
}

uint8_t PinObserver::read_bits(PortConfig* config) const
{
    if (config->port == nullptr) {
        return 0;
    }

    uint16_t result = 0;
    uint16_t value = config->port->IDR;
    uint16_t mask = config->mask;
    uint8_t position = 0;

    for (uint8_t i = 0; i < 16; i++) {
        if (mask & (1 << i)) {
            if (value & (1 << i)) {
                result |= (1 << position);
            }
            position++;
        }
    }

    return result;
}

uint8_t PinObserver::calc_check_sum() const
{
    uint8_t* data = (uint8_t*)&pins;
    uint8_t cs = 0;

    for (uint32_t i = 0; i < sizeof(PinObservation) - 1; i++) {
        cs += data[i];
    }

    return cs;
}
