#include <HalWrapper/UartInterruptHandler.hpp>
#include <HalWrapper/IUart.hpp>

UartInterruptHandler* UartInterruptHandler::handler = nullptr;

UartInterruptHandler::UartInterruptHandler()
{
    for (int i = 0; i < N_UART; i++) {
        uarts[i] = nullptr;
    }
}

void UartInterruptHandler::attach(IUart* uart)
{
    if (uart == nullptr) {
        return;
    }

    int8_t id = get_uart_id(uart->get_instance());

    if (id >= 0) {
        uarts[id] = uart;
    }
}

void UartInterruptHandler::detach(IUart* uart)
{
    if (uart == nullptr) {
        return;
    }

    int8_t id = get_uart_id(uart->get_instance());

    if (id >= 0) {
        uarts[id] = nullptr;
    }
}

int8_t UartInterruptHandler::get_uart_id(USART_TypeDef* instance)
{
    if (instance == LPUART1) {
        return 0;
    } else if (instance == USART1) {
        return 1;
    } else if (instance == USART2) {
        return 2;
    } else if (instance == USART3) {
        return 3;
    } else if (instance == UART4) {
        return 4;
    } else if (instance == UART5) {
        return 5;
    } else {
        /* Default None */
        return -1;
    }
}

IUart* UartInterruptHandler::get_uart(USART_TypeDef* instance)
{
    int8_t id = get_uart_id(instance);

    if (id >= 0) {
        return uarts[id];
    }

    return nullptr;
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    IUart* uart = UartInterruptHandler::getInstance()->get_uart(huart->Instance);

    if (uart != nullptr) {
        uart->rx_callback();
    }
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    IUart* uart = UartInterruptHandler::getInstance()->get_uart(huart->Instance);

    if (uart != nullptr) {
        uart->rx_callback();
    }
}

extern "C" void LPUART1_IRQHandler(void)
{
    IUart* uart = UartInterruptHandler::getInstance()->get_uart(LPUART1);

    if (uart != nullptr) {
        HAL_UART_IRQHandler(uart->get_handle());
    }
}

extern "C" void USART1_IRQHandler(void)
{
    IUart* uart = UartInterruptHandler::getInstance()->get_uart(USART1);

    if (uart != nullptr) {
        HAL_UART_IRQHandler(uart->get_handle());
    }
}

extern "C" void USART2_IRQHandler(void)
{
    IUart* uart = UartInterruptHandler::getInstance()->get_uart(USART2);

    if (uart != nullptr) {
        HAL_UART_IRQHandler(uart->get_handle());
    }
}

extern "C" void USART3_IRQHandler(void)
{
    IUart* uart = UartInterruptHandler::getInstance()->get_uart(USART3);

    if (uart != nullptr) {
        HAL_UART_IRQHandler(uart->get_handle());
    }
}
