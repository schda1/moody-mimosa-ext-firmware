#include <HalWrapper/ExtiInterruptHandler.hpp>

ExtiInterruptHandler* ExtiInterruptHandler::handler = nullptr;

ExtiInterruptHandler::ExtiInterruptHandler()
{
    for (int i = 0; i < N_EXTI_INTERRUPTS; i++) {
        exti_interrupts[i] = nullptr;
    }
}

void ExtiInterruptHandler::attach(IExtiInput* input)
{
    int8_t exti_id = input->get_instance();
    exti_interrupts[exti_id] = input;
}

void ExtiInterruptHandler::detach(IExtiInput* input)
{
    int8_t exti_id = input->get_instance();
    exti_interrupts[exti_id] = nullptr;
}

IExtiInput* ExtiInterruptHandler::get_exti_handle(uint8_t instance)
{
    if (instance < N_EXTI_INTERRUPTS) {
        return exti_interrupts[instance];
    }

    return nullptr;
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint16_t pos = 0; 
    for (pos = 0; pos < 16; pos++) {
        if (GPIO_Pin & (1 << pos)) {
            break;
        }
    }

    IExtiInput* input = ExtiInterruptHandler::getInstance()->get_exti_handle(pos);

    if (input != nullptr) {
        input->callback();
    }
}

extern "C" void EXTI0_IRQHandler(void)  { HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0); }
extern "C" void EXTI1_IRQHandler(void)  { HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1); }
extern "C" void EXTI2_IRQHandler(void)  { HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2); }
extern "C" void EXTI3_IRQHandler(void)  { HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3); }
extern "C" void EXTI4_IRQHandler(void)  { HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4); }

extern "C" void EXTI9_5_IRQHandler(void)  
{ 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

extern "C" void EXTI15_10_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15); 
}
