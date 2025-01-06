#pragma once

#include <functional>
#include <common.hpp>
#include <HalWrapper/IDigitalInput.hpp>

enum class InterruptEdge
{
    NONE,
    RISING,
    FALLING,
    BOTH
};

class IExtiInput : public IDigitalInput 
{
    public: 
        using ExtiCallback = std::function<void(void)>;
        
        IExtiInput(InterruptEdge edge, DigitalInputMode mode) : IDigitalInput(mode) {this->edge = edge;}
        virtual ~IExtiInput() {}

        virtual void init() = 0;

        virtual void callback() { if (exti_callback != nullptr) { exti_callback(); } }
        virtual void set_edge(InterruptEdge edge) { this->edge = edge; }
        virtual void set_callback(ExtiCallback callback) { exti_callback = callback; }
        virtual uint8_t get_instance() = 0;

    protected:
        ExtiCallback exti_callback;
        InterruptEdge edge;
};