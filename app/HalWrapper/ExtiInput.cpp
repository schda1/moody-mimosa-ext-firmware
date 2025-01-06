#include <HalWrapper/ExtiInput.hpp>
#include <HalWrapper/ExtiInterruptHandler.hpp>

ExtiInput::ExtiInput(GPIO_TypeDef* port, uint8_t pin, InterruptEdge edge, DigitalInputMode mode) : 
    IExtiInput(edge, mode), 
    port(port), 
    pin(pin), 
    inverted(false) 
{ 
    this->mode = mode;
    this->edge = edge;
}

void ExtiInput::init()
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
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

    /* Define pullup settings */
    if (mode == DigitalInputMode::INPUT) {
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    } else if (mode == DigitalInputMode::INPUT_PULL_UP) {
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    } else if (mode == DigitalInputMode::INPUT_PULL_DOWN) {
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    }

    /* Define interrupt edge */
    if (edge == InterruptEdge::NONE) {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    } else if (edge == InterruptEdge::RISING) {
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    } else if (edge == InterruptEdge::FALLING) {
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    } else if (edge == InterruptEdge::BOTH) {
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    }

    HAL_GPIO_Init(port, &GPIO_InitStruct);

    ExtiInterruptHandler::getInstance()->attach(this);

    if (pin == 0) {
        HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    } else if (pin == 1) {
        HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    } else if (pin == 2) {
        HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    } else if (pin == 3) {
        HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    } else if (pin == 4) {
        HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    } else if (pin >= 5 && pin <= 9) {
        HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    } else if (pin >= 10 && pin <= 15) {
        HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
}

void ExtiInput::deinit()
{
    if (pin == 0) HAL_NVIC_DisableIRQ(EXTI0_IRQn);
    if (pin == 1) HAL_NVIC_DisableIRQ(EXTI1_IRQn);
    if (pin == 2) HAL_NVIC_DisableIRQ(EXTI2_IRQn);
    if (pin == 3) HAL_NVIC_DisableIRQ(EXTI3_IRQn);
    if (pin == 4) HAL_NVIC_DisableIRQ(EXTI4_IRQn);
    
    HAL_GPIO_DeInit(port, (1L << pin));
}


uint8_t ExtiInput::get() const
{
    uint8_t state = HAL_GPIO_ReadPin(port, 1 << pin);
    return (inverted) ? !state : state;
}

void ExtiInput::invert(bool invert)
{
    inverted = invert;
}

uint8_t ExtiInput::get_instance()
{
    return pin;
}