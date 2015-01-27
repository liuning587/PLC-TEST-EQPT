#ifndef __BEEP_H__
#define __BEEP_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define BUZZER_PIN            GPIO_Pin_3
#define BUZZER_PORT           GPIOB

#define BUZZER_ON()           GPIO_SetBits(BUZZER_PORT, BUZZER_PIN)
#define BUZZER_OFF()          GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN) 
#define BUZZER_TOGGLE()       GPIO_ToggleBits(BUZZER_PORT, BUZZER_PIN)

#define BEEP_TYPE_NONE        0
#define BEEP_TYPE_NORMAL      1
#define BEEP_TYPE_WARNING     2

#define BEEP_TYPE_NONE_PERIOD     (0) //ms
#define BEEP_TYPE_NORMAL_PERIOD   (400 / 50) //ms
#define BEEP_TYPE_WARNING_PERIOD  (400 / 50) //ms

extern INT8U g_beep_type;
extern INT8U g_beep_cnt;

void BEEP_Init(void);
void beep(void);
void beep_on(void); 
void beep_off(void);


#ifdef __cplusplus
}
#endif

#endif
