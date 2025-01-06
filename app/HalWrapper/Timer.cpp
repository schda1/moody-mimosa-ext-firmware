#include <HalWrapper/Timer.hpp>
#include <HalWrapper/TimerInterruptHandler.hpp>

Timer::Timer(TIM_TypeDef* instance) : ITimer(instance)
{
    this->instance = instance;
    htim.Instance = instance;
    htim.Init.Prescaler = 8000;
    htim.Init.Period = 10000;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    callback_fct = nullptr;
}

void Timer::set_prescaler(uint32_t prescaler)
{
    htim.Init.Prescaler = prescaler;
}

void Timer::set_period(uint32_t period)
{
    htim.Init.Period = period;
}

void Timer::init()
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* Attach timer to timer interrupt handler */
    TimerInterruptHandler::getInstance()->attach(this);

    if (htim.Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
        irq_number = TIM2_IRQn;
    } else if (htim.Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
        irq_number = TIM3_IRQn;
    } else if (htim.Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
        irq_number = TIM4_IRQn;
    } else if (htim.Instance == TIM5) {
        __HAL_RCC_TIM5_CLK_ENABLE();
        irq_number = TIM5_IRQn;
    } else {
        /* Error handling */
        return;
    }

    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&htim) != HAL_OK) {
        /* Error handling */
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK) {
        /* Error handling */
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK) {
        /* Error handling */
    }

    __disable_irq();
    HAL_NVIC_SetPriority(irq_number, 0, 0);
    HAL_NVIC_EnableIRQ(irq_number);
    __enable_irq();
}

void Timer::deinit()
{
    HAL_TIM_Base_Stop_IT(&htim);

    if (irq_number > 0) {
        HAL_NVIC_DisableIRQ(irq_number);
    }

    HAL_TIM_Base_DeInit(&htim);
}

void Timer::start()
{
    if (instance == TIM4) {
        // HAL_TIM_OC_Start(&htim, TIM_CHANNEL_4);
    }
    else  {
        if (HAL_TIM_Base_Start_IT(&htim) != HAL_OK) {
            /* Error handling */
        }
    }
}

void Timer::stop()
{
    if (HAL_TIM_Base_Stop_IT(&htim) != HAL_OK) {
        /* Error handling */
    }
}

void Timer::reset()
{
    if (HAL_TIM_Base_Stop_IT(&htim) != HAL_OK) {
        /* Error handling */
    }

    init();
}

void Timer::callback()
{
    if (callback_fct != nullptr) {
        callback_fct();
    }
}