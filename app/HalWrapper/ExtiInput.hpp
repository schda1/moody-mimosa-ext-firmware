#pragma once

#include <functional>
#include <common.hpp>
#include <HalWrapper/IExtiInput.hpp>

class ExtiInput : public IExtiInput
{
public:
    ExtiInput(GPIO_TypeDef* port, uint8_t pin, InterruptEdge edge = InterruptEdge::RISING, DigitalInputMode mode = DigitalInputMode::INPUT);
    virtual ~ExtiInput() {}

    virtual void init() override;
    virtual void deinit() override;
    
    virtual void invert(bool invert) override;
    virtual uint8_t get() const override;

    virtual uint8_t get_instance() override;

protected: 
    DigitalInputMode mode;
    InterruptEdge edge;

    GPIO_TypeDef* port;
    uint8_t pin;
    bool inverted;
};