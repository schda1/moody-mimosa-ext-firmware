#pragma once

#include <common.hpp>

struct PinObservation
{
    uint32_t cycle; /**< Cycle number of the current observation     */
    uint8_t ui_in;  /**< Pin states ui_in, according to tt notation  */
    uint8_t uo_out; /**< Pin states uo_out, according to tt notation */
    uint8_t uoi;    /**< Pin states uoi, according to tt notation    */
    uint8_t ctrl;   /**< Rst_n (bit 0) and clk (bit 1) signal        */
};

struct PortConfig
{
    GPIO_TypeDef* port;
    uint16_t mask;
};

enum class PortType
{
    UI_IN,
    UO_OUT,
    UIO,
    CTRL
};

typedef void (*InputConfigFct)(GPIO_TypeDef* port, uint8_t pin);

class PinObserver
{
public:
    PinObserver(InputConfigFct config_fct);
    virtual ~PinObserver() {}

    void configure(PortType type, GPIO_TypeDef* port, uint16_t mask);
    void update();
    void reset();

    PinObservation get_pins() const;
    PortConfig get_config(PortType type) const;
    void enable_log(bool enable);
    void print() const;

private:
    uint8_t calc_check_sum() const;
    uint8_t read_bits(PortConfig* config) const;

    InputConfigFct config_fct;
    PortConfig config[4];
    PinObservation pins;
    bool log_on;
};