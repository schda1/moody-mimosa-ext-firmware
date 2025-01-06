#include <HalWrapper/TimerInterruptHandler.hpp>
#include <HalWrapper/ITimer.hpp>


TimerInterruptHandler* TimerInterruptHandler::handler = nullptr;

TimerInterruptHandler::TimerInterruptHandler()
{
    for (int i = 0; i < N_TIMER; i++) {
        timers[i] = nullptr;
    }
}

void TimerInterruptHandler::attach(ITimer* timer)
{
    if (timer == nullptr) {
        return;
    }

    int8_t id = get_timer_id(timer->get_instance());

    if (id >= 0) {
        timers[id] = timer;
    }
}

void TimerInterruptHandler::detach(ITimer* timer)
{
    if (timer == nullptr) {
        return;
    }

    int8_t id = get_timer_id(timer->get_instance());

    if (id >= 0) {
        timers[id] = nullptr;
    }
}

ITimer* TimerInterruptHandler::get_timer(TIM_TypeDef* instance)
{
    int8_t id = get_timer_id(instance);

    if (id >= 0) {
        return timers[id];
    }

    return nullptr;
}

int8_t TimerInterruptHandler::get_timer_id(TIM_TypeDef* instance)
{
    if (instance == TIM1) {
        return 0;
    } else if (instance == TIM2) {
        return 1;
    } else if (instance == TIM3) {
        return 2;
    } else if (instance == TIM4) {
        return 3;
    } else if (instance == TIM5) {
        return 4;
    } else if (instance == TIM6) {
        return 5;
    } else if (instance == TIM7) {
        return 6;
    } else if (instance == TIM8) {
        return 7;
    } else if (instance == TIM15) {
        return 8;
    } else if (instance == TIM16) {
        return 9;
    } else if (instance == TIM17) {
        return 10;
    } else if (instance == TIM20) {
        return 11;
    } else {
        /* Default None */
        return -1;
    }
}

/*
 * This function overwrites the weak implementation of HAL_TIM_PeriodElapsedCallback
 * defined in the STM32 HAL library. Since the HAL library is written in C,
 * we use extern "C" clause to ensure this function uses C linkage, avoiding name
 * mangling by the C++ compiler. In this way the linker can correctly replace the weak
 * implementation with this one.
 */
extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    ITimer* timer = TimerInterruptHandler::getInstance()->get_timer(htim->Instance);

    if (timer != nullptr) {
        timer->callback();
    }
}

/**
 * This is the basic Timer2 IRQ handler, called by the NVIC when the
 * timer interrupt is triggered. Note that it eventually calls the
 * HAL_TIM_IRQHandler which contains the rather complex flag handling
 * logic.
 */
extern "C" void TIM2_IRQHandler(void)
{
    ITimer* timer = TimerInterruptHandler::getInstance()->get_timer(TIM2);

    if (timer != nullptr) {
        HAL_TIM_IRQHandler(timer->get_handle());
    }
}

extern "C" void TIM3_IRQHandler(void)
{
    ITimer* timer = TimerInterruptHandler::getInstance()->get_timer(TIM3);

    if (timer != nullptr) {
        HAL_TIM_IRQHandler(timer->get_handle());
    }
}

extern "C" void TIM4_IRQHandler(void)
{
    ITimer* timer = TimerInterruptHandler::getInstance()->get_timer(TIM4);

    if (timer != nullptr) {
        HAL_TIM_IRQHandler(timer->get_handle());
    }
}