#pragma once

#include <common.hpp>
#include <HalWrapper/IDigitalOutput.hpp>

class DigitalOutput : public IDigitalOutput
{
public:
    DigitalOutput(GPIO_TypeDef* port, uint8_t pin);
    virtual ~DigitalOutput() {}

    virtual void init() override;
    virtual void deinit() override;

    virtual void set(uint8_t value) override;
    virtual uint8_t get() const override;
    virtual void toggle() override;
    virtual void invert(bool invert) override;

protected:
    GPIO_TypeDef* port;
    uint8_t pin;
    bool inverted;
};