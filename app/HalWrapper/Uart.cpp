#include <HalWrapper/Uart.hpp>
#include <HalWrapper/UartInterruptHandler.hpp>

Uart::Uart(USART_TypeDef* instance) : IUart(instance)
{
    huart.Instance = instance;
    huart.Init.BaudRate = 115200;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    async_read_callback = nullptr;
    async_transmit_callback = nullptr;
    async_read = false;
}

void Uart::init()
{
    init_uart();
}

void Uart::deinit()
{
    HAL_UART_Abort(&huart);

    if (instance == LPUART1) HAL_NVIC_DisableIRQ(LPUART1_IRQn);
    if (instance == USART3)  HAL_NVIC_DisableIRQ(USART3_IRQn);

    HAL_UART_DeInit(&huart);
}

void Uart::set_baudrate(uint32_t baudrate)
{
    huart.Init.BaudRate = baudrate;
}

void Uart::write(uint8_t data) 
{
    HAL_UART_Transmit(&huart, &data, 1, HAL_MAX_DELAY);
}

void Uart::write(const uint8_t* data, uint16_t len)
{
    HAL_UART_Transmit(&huart, (uint8_t*)data, len, HAL_MAX_DELAY);
}

uint16_t Uart::read(uint8_t* data, uint16_t len)
{
    return HAL_UART_Receive(&huart, data, len, HAL_MAX_DELAY);
}

void Uart::start_read_async()
{
    async_read = true;
    HAL_UART_Receive_IT(&huart, &rx_data, 1);
}

void Uart::stop_read_async()
{
    async_read = false;
    HAL_UART_AbortReceive(&huart);
}

void Uart::rx_callback()
{
    if (async_read_callback) {
        async_read_callback(rx_data);
    }

    /* Retrigger */
    if (async_read) {
        HAL_UART_Receive_IT(&huart, &rx_data, 1);
    }
}

void Uart::tx_callback()
{
    if (async_transmit_callback) {
        async_transmit_callback();
    }
}

void Uart::init_uart()
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /* Attach uart to uart interrupt handler */
    UartInterruptHandler::getInstance()->attach(this);

    /* Default settings for LPUART1 and USART3. For a real project,
     * you would have to implement a proper logic for the different
     * pin assignment possibilities (and alternate functions). And if
     * you want ot implement it for different families (G4, G0, F4),
     * and subvariants, your code will get quite verbose and with lots
     * of compiler switches. */
    if (instance == LPUART1) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
        PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;

        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            /* Error handling */
        }

        __HAL_RCC_LPUART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* Configure pins for UART */
        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF12_LPUART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Enable interrupts */
        HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(LPUART1_IRQn);
    } else if (instance == USART3) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
        PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;

        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
           /* Error handling */
        }

        __HAL_RCC_USART3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* Configure pins for UART */
        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Enable interrupts */
        HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }

    if (HAL_UART_Init(&huart) != HAL_OK) {
        /* Error handling */
    }

    if (HAL_UARTEx_SetTxFifoThreshold(&huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        /* Error handling */
    }

    if (HAL_UARTEx_SetRxFifoThreshold(&huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        /* Error handling */
    }

    if (HAL_UARTEx_DisableFifoMode(&huart) != HAL_OK) {
        /* Error handling */
    }
}