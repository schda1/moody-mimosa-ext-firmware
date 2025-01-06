#pragma once

#include <common.hpp>
#include <HalWrapper/IDigitalInput.hpp>

class DigitalInput : public IDigitalInput
{
public:
    DigitalInput(GPIO_TypeDef* port, uint8_t pin, DigitalInputMode mode = DigitalInputMode::INPUT);
    virtual ~DigitalInput() {}
    
    virtual void init() override;
    virtual void deinit() override;
    
    virtual uint8_t get() const override;
    virtual void invert(bool invert) override;

protected:
    DigitalInputMode mode;
    GPIO_TypeDef* port;
    uint8_t pin;
    bool inverted;
};