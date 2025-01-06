#include <HalWrapper/DigitalInput.hpp>

DigitalInput::DigitalInput(GPIO_TypeDef* port, uint8_t pin, DigitalInputMode mode) : 
    IDigitalInput(mode),
    port(port), 
    pin(pin), 
    inverted(false) 
{ }

void DigitalInput::init()
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
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

    /* Pullup settings */
    if (mode == DigitalInputMode::INPUT) {
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    } else if (mode == DigitalInputMode::INPUT_PULL_UP) {
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    } else if (mode == DigitalInputMode::INPUT_PULL_DOWN) {
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    }

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void DigitalInput::deinit()
{
    HAL_GPIO_DeInit(port, (1L << pin));
}

uint8_t DigitalInput::get() const
{
    uint8_t state = HAL_GPIO_ReadPin(port, 1 << pin);
    return (inverted) ? !state : state;
}

void DigitalInput::invert(bool invert)
{
    inverted = invert;
}
