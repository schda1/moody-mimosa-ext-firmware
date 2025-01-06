#include <HalWrapper/PwmOutput.hpp>

PwmOutput::PwmOutput(GPIO_TypeDef* port, uint8_t pin, TIM_TypeDef* timer_instance, uint16_t resolution) :
    timer_instance(timer_instance),
    port(port),
    pin(pin),
    prescaler(0), 
    resolution(resolution)
{ 
    duty_cycle = resolution / 2;

    htim.Instance = timer_instance;
    htim.Init.Prescaler = prescaler;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = resolution-1;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
}

void PwmOutput::init()
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    if (timer_instance != TIM4) {
        /* Not implemented */
        printf("not implemented \n");
    }
    else
    {
        __HAL_RCC_TIM4_CLK_ENABLE();

        if (HAL_TIM_OC_Init(&htim) != HAL_OK) {
            /* Error Handling*/
        }

        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

        if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK) {
            /* Error Handling*/
        }

        config_oc.OCMode = TIM_OCMODE_TOGGLE;
        config_oc.Pulse = duty_cycle-1;
        config_oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        config_oc.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_TIM_OC_ConfigChannel(&htim, &config_oc, TIM_CHANNEL_4) != HAL_OK) {
            /* Error Handling*/
        }

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void PwmOutput::deinit()
{
    HAL_TIM_OC_Stop(&htim, TIM_CHANNEL_4);
    HAL_TIM_OC_DeInit(&htim);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
}

void PwmOutput::set_prescaler(uint32_t prescaler)
{
    this->prescaler = prescaler;
    htim.Init.Prescaler = prescaler;
}

void PwmOutput::set_resolution(uint16_t resolution)
{
    uint32_t old_resolution = this->resolution;

    this->resolution = resolution;

    duty_cycle = (duty_cycle * resolution) / old_resolution;
    duty_cycle = (duty_cycle > resolution) ? resolution : duty_cycle;
}

void PwmOutput::set(uint8_t value)
{
    uint32_t duty_cycle = value;
    duty_cycle = (duty_cycle > resolution) ? resolution : duty_cycle;
}

uint8_t PwmOutput::get() const
{
    return duty_cycle;
}

void PwmOutput::toggle()
{
    /* Not implemented */
}

void PwmOutput::invert(bool invert)
{
    /* Not implemented */
}

void PwmOutput::start()
{
    if (timer_instance == TIM4) {
        HAL_TIM_OC_Start(&htim, TIM_CHANNEL_4);
    } else {
        /* Not implemented */
    }
}


void PwmOutput::stop()
{
    if (timer_instance == TIM4) {
        HAL_TIM_OC_Stop(&htim, TIM_CHANNEL_4);
    } else {
        /* Not implemented */
    }
}