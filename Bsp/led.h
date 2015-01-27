#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define      LED_RUN                      0
#define      LED_DISP                     1
#define      LED_PLC                      2
#define      LED_UART                     3
#define      LED_mPLC_1                   4
#define      LED_mPLC_2                   5
#define      LED_mPLC_3                   6
#define      LED_mPLC_4                   7

void         LED_On                      (INT8U     led_id);
void         LED_Off                     (INT8U     led_id);
void         LED_Toggle                  (INT8U     led_id);
void         LED_Init                    (void            );


#ifdef __cplusplus
}
#endif

#endif
