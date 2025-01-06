#pragma once

#include <common.hpp>
#include <HalWrapper/IExtiInput.hpp>

#define N_EXTI_INTERRUPTS (16)

class ExtiInterruptHandler
{
public:
    static ExtiInterruptHandler* getInstance()
    {
        if (handler == nullptr) {
            handler = new ExtiInterruptHandler();
        }
        return handler;
    }

    void attach(IExtiInput* input);
    void detach(IExtiInput* input);

    IExtiInput* get_exti_handle(uint8_t instance);

private:
    ExtiInterruptHandler();
    ExtiInterruptHandler(ExtiInterruptHandler const&);
    void operator=(ExtiInterruptHandler const&);

    static ExtiInterruptHandler* handler;
    IExtiInput* exti_interrupts[N_EXTI_INTERRUPTS];
};
