#pragma once

#include <HalWrapper/IDigitalOutput.hpp>
#include <HalWrapper/PwmOutput.hpp>
#include "stm32g4xx_hal.h"

class PwmOutput : public IDigitalOutput
{
public:
    PwmOutput(GPIO_TypeDef* port, uint8_t pin, TIM_TypeDef* tim, uint16_t resolution = 100);
    virtual ~PwmOutput() {}

    virtual void init() override;
    virtual void deinit() override;

    void set_resolution(uint16_t resolution);
    void set_prescaler(uint32_t prescaler);
    void set(uint8_t value) override;
    uint8_t get() const override;
    void toggle() override;
    void invert(bool invert) override;

    void start();
    void stop();

protected:
    TIM_TypeDef* timer_instance;
    TIM_HandleTypeDef htim;
    TIM_OC_InitTypeDef config_oc;

    GPIO_TypeDef* port;
    uint8_t pin;
    bool inverted;

    uint16_t duty_cycle;
    uint16_t resolution;
    uint32_t prescaler;
    
};