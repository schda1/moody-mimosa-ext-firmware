#ifndef STM32G4XX_HAL_UTEST_H
#define STM32G4XX_HAL_UTEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* GPIO */
typedef struct {
    uint32_t IDR;
} GPIO_TypeDef;

volatile static GPIO_TypeDef gpioa;
volatile static GPIO_TypeDef gpiob;
volatile static GPIO_TypeDef gpioc;

#define GPIOA ((GPIO_TypeDef*)&gpioa)
#define GPIOB ((GPIO_TypeDef*)&gpiob)
#define GPIOC ((GPIO_TypeDef*)&gpioc)

/* Timer */
typedef uint32_t* TIM_TypeDef;  
typedef uint32_t TIM_HandleTypeDef;

volatile static uint32_t timer1;
volatile static uint32_t timer2;
volatile static uint32_t timer3;

#define TIM1 ((TIM_TypeDef*)&timer1)
#define TIM2 ((TIM_TypeDef*)&timer2)
#define TIM3 ((TIM_TypeDef*)&timer3)

/* SPI */


#ifdef __cplusplus
}   
#endif

#endif /* STM32G4XX_HAL_UTEST_H */