#pragma once

#include <common.hpp>
#include <HalWrapper/IUart.hpp>

#define N_UART (6)

class UartInterruptHandler
{
public:
    static UartInterruptHandler* getInstance()
    {
        if (handler == nullptr) {
            handler = new UartInterruptHandler();
        }
        return handler;
    }

    void attach(IUart* uart);
    void detach(IUart* uart);

    IUart* get_uart(USART_TypeDef* instance);

private:
    UartInterruptHandler();
    UartInterruptHandler(UartInterruptHandler const&);
    void operator=(UartInterruptHandler const&);

    int8_t get_uart_id(USART_TypeDef* instance);

    static UartInterruptHandler* handler;
    IUart* uarts[N_UART];
};
