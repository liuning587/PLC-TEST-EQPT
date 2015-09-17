#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define LED_RUN                         0
#define LED_DISP                        1
#define LED_PLC                         2
#define LED_UART                        3
#define LED_mPLC_1                      4
#define LED_mPLC_2                      5
#define LED_mPLC_3                      6
#define LED_mPLC_4                      7

#define LED_RUN_TOGGLE_PERIOD      (500 / 50) //ms

#define LED_RUN_TOGGLE()       LED_Toggle(LED_RUN)

#define LED_DISP_ON()          LED_On(LED_DISP)
#define LED_DISP_OFF()         LED_Off(LED_DISP)

#define LED_PLC_TOGGLE()       LED_Toggle(LED_PLC)

#define LED_UART_ON()          LED_On(LED_UART)
#define LED_UART_OFF()         LED_Off(LED_UART)

#define LED_mPLC_ON() {                     \
    LED_On(LED_mPLC_1);                     \
    LED_On(LED_mPLC_2);                     \
    LED_On(LED_mPLC_3);                     \
    LED_On(LED_mPLC_4);                     \
}

#define LED_mPLC_OFF() {                    \
    LED_Off(LED_mPLC_1);                    \
    LED_Off(LED_mPLC_2);                    \
    LED_Off(LED_mPLC_3);                    \
    LED_Off(LED_mPLC_4);                    \
}

void LED_On(INT8U led_id);
void LED_Off(INT8U led_id);
void LED_Toggle(INT8U led_id);
void LED_Init(void);


#ifdef __cplusplus
}
#endif

#endif
