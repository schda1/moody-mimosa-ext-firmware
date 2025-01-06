#include <HalWrapper/DigitalOutput.hpp>

DigitalOutput::DigitalOutput(GPIO_TypeDef* port, uint8_t pin)
{
    this->port = port;
    this->pin = pin;
    this->inverted = false;
}

void DigitalOutput::init()
{
    if (port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (port == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = (1L << pin);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void DigitalOutput::deinit()
{
   HAL_GPIO_DeInit(port, (1L << pin));
}

void DigitalOutput::set(uint8_t state)
{
    if (inverted) {
        state = !state;
    }

    if (state) {
        HAL_GPIO_WritePin(port, 1L << pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(port, 1L << pin, GPIO_PIN_RESET);
    }
}

uint8_t DigitalOutput::get() const
{
    uint8_t state = HAL_GPIO_ReadPin(port, 1L << pin);

    if (inverted) {
        state = !state;
    }

    return state;
}

void DigitalOutput::toggle()
{
    HAL_GPIO_TogglePin(port, 1L << pin);
}

void DigitalOutput::invert(bool invert)
{
    inverted = invert;
}
