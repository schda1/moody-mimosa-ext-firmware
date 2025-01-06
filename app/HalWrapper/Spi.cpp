#include <HalWrapper/Spi.hpp>

#define SPI_MAX_DELAY (1000)


Spi::Spi(SPI_TypeDef* instance, IDigitalOutput* cs) : 
    instance(instance), 
    cs(cs), 
    mode(SpiMode::MODE_0), 
    frequency(1000000)
{}

void Spi::init()
{
    cs->init();

    if (instance == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();
        /* Even minimalistic implementation missing */

    } else if (instance == SPI2) {
        __HAL_RCC_SPI2_CLK_ENABLE();

        /* By default Pins PB13-15 for SPI2. Of course, in a professional
         * environment, you would have to figure out how to handle all the
         * different mux configurations and think carefully about portability,
         * e.g. between different MCU families. For this simple application,
         * I keep it as simple as possible. However, think about the benefits of
         * using a RTOS such as Zephyr which provides you with mighty constructs
         * such as the devicetree allowing you to define your peripherals and
         * connectivity on a high level and without compiling unnecessary code. */
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        hspi.Instance = SPI2;
        hspi.Init.Mode = SPI_MODE_MASTER;
        hspi.Init.Direction = SPI_DIRECTION_2LINES;
        hspi.Init.DataSize = SPI_DATASIZE_8BIT;
        hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
        hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
        hspi.Init.TIMode = SPI_TIMODE_DISABLE;
        hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        hspi.Init.CRCPolynomial = 7;
        hspi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
        hspi.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

        /* Handling software-controlled CS */
        if (cs != nullptr) {
            hspi.Init.NSS = SPI_NSS_SOFT;
        } else {
            hspi.Init.NSS = SPI_NSS_HARD_OUTPUT;
        }

        /* Handling different SPI modes */
        if (mode == SpiMode::MODE_0) {
            hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
            hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
        } else if (mode == SpiMode::MODE_1) {
            hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
            hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
        } else if (mode == SpiMode::MODE_2) {
            hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
            hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
        } else if (mode == SpiMode::MODE_3) {
            hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
            hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
        }

        /* Handling different frequencies not implemented */

        if (HAL_SPI_Init(&hspi) != HAL_OK) {
            /* Error handling */
        }

    } else if (instance == SPI3) {
        __HAL_RCC_SPI3_CLK_ENABLE();
        /* Even minimalistic implementation missing */
    }
}

void Spi::deinit()
{
    HAL_SPI_DeInit(&hspi);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    cs->deinit();
}

void Spi::add_cs(IDigitalOutput* custom_cs)
{
    cs = custom_cs;
    cs->init();
    cs->set(1);

    init();
}

void Spi::set_frequency(uint32_t frequency)
{
    /* Not implemented */
}

void Spi::set_mode(SpiMode mode)
{
    mode = mode;
    init();
}

void Spi::set_cs(uint8_t state)
{
    if (cs != nullptr) {
        cs->set(state);
    }
}

void Spi::write(const uint8_t* data_tx, uint16_t len)
{
    set_cs(0);
    HAL_SPI_Transmit(&hspi, data_tx, len, SPI_MAX_DELAY);
    set_cs(1);
}

void Spi::read(uint8_t* data_rx, uint16_t len)
{
    set_cs(0);
    HAL_SPI_Receive(&hspi, data_rx, len, SPI_MAX_DELAY);
    set_cs(1);
}

void Spi::write_and_read(const uint8_t* data_tx, uint8_t* data_rx, uint16_t len)
{
    set_cs(0);
    HAL_SPI_TransmitReceive(&hspi, data_tx, data_rx, len, SPI_MAX_DELAY);
    set_cs(1);
}

void Spi::write_then_read(const uint8_t* data_tx, uint16_t len_tx, uint8_t* data_rx, uint16_t len_rx)
{
    set_cs(0);
    HAL_SPI_Transmit(&hspi, data_tx, len_tx, SPI_MAX_DELAY);
    HAL_SPI_Receive(&hspi, data_rx, len_rx, SPI_MAX_DELAY);
    set_cs(1);
}

void Spi::write_then_write(const uint8_t* data_tx1, uint16_t len_tx1, const uint8_t* data_tx2, uint16_t len_tx2)
{
    set_cs(0);
    HAL_SPI_Transmit(&hspi, data_tx1, len_tx1, SPI_MAX_DELAY);
    HAL_SPI_Transmit(&hspi, data_tx2, len_tx2, SPI_MAX_DELAY);
    set_cs(1);
}