#include <HalWrapper/I2C.hpp>

I2C::I2C(I2C_TypeDef* instance) : 
    instance(instance), 
    reg_address_len(1), 
    frequency(100000) 
{}

void I2C::init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    if (instance == I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();

        /* Even minimalistic implementation missing */
    }
    else if (instance == I2C2) {
        __HAL_RCC_I2C2_CLK_ENABLE();

        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
        PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;

        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            /* Error handling*/
        }

        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        printf("I2C2 initialized\n");
    } 
    else if (instance == I2C3) {
        __HAL_RCC_I2C3_CLK_ENABLE();

        /* Even minimalistic implementation missing */
    }

    hi2c.Instance = instance;
    hi2c.Init.Timing = calc_timing_register();
    hi2c.Init.OwnAddress1 = 0;
    hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c.Init.OwnAddress2 = 0;
    hi2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c) != HAL_OK) {
        /* Error handling */
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        /* Error handling */
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c, 0) != HAL_OK) {
        /* Error handling */
    }
}

void I2C::deinit()
{
    HAL_I2C_DeInit(&hi2c);
}

void I2C::set_frequency(uint32_t frequency)
{
    this->frequency = frequency;
}

void I2C::set_reg_address_len(uint8_t length)
{
    if (length == 1 || length == 2) {
        reg_address_len = length;
    }
}

void I2C::write(uint8_t dev_address, const uint8_t* data_tx, uint16_t len)
{
    int ret = HAL_I2C_Master_Transmit(&hi2c, dev_address, (uint8_t*)data_tx, len, HAL_MAX_DELAY);

    if (ret != 0) {
        printf("Error: %d\n", ret);
    }
}

void I2C::read(uint8_t dev_address, uint8_t* data_rx, uint16_t len)
{
    HAL_I2C_Master_Receive(&hi2c, dev_address, data_rx, len, HAL_MAX_DELAY);
}

void I2C::write_reg(uint8_t dev_address, uint16_t reg_address, const uint8_t* data_tx, uint16_t len)
{
    HAL_I2C_Mem_Write(&hi2c, dev_address, reg_address, reg_address_len, (uint8_t*)data_tx, len, HAL_MAX_DELAY);
}

void I2C::read_reg(uint8_t dev_address, uint16_t reg_address, uint8_t* data_rx, uint16_t len)
{
    HAL_I2C_Mem_Read(&hi2c, dev_address, reg_address, reg_address_len, data_rx, len, HAL_MAX_DELAY);
}

/* Pretty complex */
uint32_t I2C::calc_timing_register()
{
    /* Default value for 100 kHz mode with 80 MHz Clock */
    return 0x10909CEC;
}