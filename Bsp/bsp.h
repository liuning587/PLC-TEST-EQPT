#ifndef __BSP_H__
#define __BSP_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define DEBUG_INFO_PRINT_EN          0u

#if (DEBUG_INFO_PRINT_EN > 0u)
#define DEBUG_PRINT(s)         printf s
#else
#define DEBUG_PRINT(s)             
#endif

#ifndef CFG_USE_STM32F101X
 #define CFG_USE_STM32F103X
#endif

#define WDT_EN                       1u

#if (WDT_EN > 0u)
#define clr_wdt()         { IWDG_ReloadCounter(); }
#else
#define clr_wdt() 
#endif

#define SYS_TICK_PREEMPT_PRIO         0
#define SYS_TICK_SUB_PRIO             0

#define USART1_PREEMPT_PRIO           1
#define USART1_SUB_PRIO               0

#define USART2_PREEMPT_PRIO           1
#define USART2_SUB_PRIO               1

#define USART3_PREEMPT_PRIO           1
#define USART3_SUB_PRIO               2

#define EXTI8_PREEMPT_PRIO            0
#define EXTI8_SUB_PRIO                1

#define EXTI11_PREEMPT_PRIO           0
#define EXTI11_SUB_PRIO               2

void STM32_SoftReset(void);
void LED_Config(void);
void PLC_Config(void);
void BEEP_Config(void);
void BSP_Init(void);


#ifdef __cplusplus
}
#endif

#endif
