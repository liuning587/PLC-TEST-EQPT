#include "includes.h"


void LED_On(INT8U led_id)
{
    switch (led_id) {
        case LED_RUN:
             GPIO_ResetBits(GPIOC, GPIO_Pin_15);
             break;
             
        case LED_DISP:
             GPIO_ResetBits(GPIOC, GPIO_Pin_14);
             break;       

        case LED_PLC:
             GPIO_ResetBits(GPIOC, GPIO_Pin_13);
             break;      

        case LED_UART:
             GPIO_ResetBits(GPIOB, GPIO_Pin_9);
             break;     

        case LED_mPLC_1:
             GPIO_ResetBits(GPIOA, GPIO_Pin_6);
             break;    

        case LED_mPLC_2:
             GPIO_ResetBits(GPIOA, GPIO_Pin_7);
             break;     
             
        case LED_mPLC_3:
             GPIO_ResetBits(GPIOB, GPIO_Pin_0);
             break;  
             
        case LED_mPLC_4:
             GPIO_ResetBits(GPIOB, GPIO_Pin_1);
             break; 
             
        default:
             break;
    }
}

void LED_Off(INT8U led_id)
{
    switch (led_id) {
        case LED_RUN:
             GPIO_SetBits(GPIOC, GPIO_Pin_15);
             break;   

        case LED_DISP:
             GPIO_SetBits(GPIOC, GPIO_Pin_14);
             break;         

        case LED_PLC:
             GPIO_SetBits(GPIOC, GPIO_Pin_13);
             break;     

        case LED_UART:
             GPIO_SetBits(GPIOB, GPIO_Pin_9);
             break;    
             
        case LED_mPLC_1:
             GPIO_SetBits(GPIOA, GPIO_Pin_6);
             break;    

        case LED_mPLC_2:
             GPIO_SetBits(GPIOA, GPIO_Pin_7);
             break;     
             
        case LED_mPLC_3:
             GPIO_SetBits(GPIOB, GPIO_Pin_0);
             break;  
             
        case LED_mPLC_4:
             GPIO_SetBits(GPIOB, GPIO_Pin_1);
             break;    
             
        default:
             break;
    }    
}

void LED_Toggle(INT8U led_id)
{
    switch (led_id) {
        case LED_RUN:
             GPIO_ToggleBits(GPIOC, GPIO_Pin_15);
             break;   

        case LED_DISP:
             GPIO_ToggleBits(GPIOC, GPIO_Pin_14);
             break;    

        case LED_PLC:
             GPIO_ToggleBits(GPIOC, GPIO_Pin_13);   
             break;    

        case LED_UART:
             GPIO_ToggleBits(GPIOB, GPIO_Pin_9);
          break;            

        case LED_mPLC_1:
             GPIO_ToggleBits(GPIOA, GPIO_Pin_6);
             break;    

        case LED_mPLC_2:
             GPIO_ToggleBits(GPIOA, GPIO_Pin_7);
             break;     
             
        case LED_mPLC_3:
             GPIO_ToggleBits(GPIOB, GPIO_Pin_0);
             break;  
             
        case LED_mPLC_4:
             GPIO_ToggleBits(GPIOB, GPIO_Pin_1);
             break; 

        default:
             break;
    }     
}

void LED_Init(void)
{
    LED_Config();

    LED_Off(LED_RUN);
    LED_Off(LED_DISP);
    LED_Off(LED_PLC);
    LED_Off(LED_UART);

    LED_Off(LED_mPLC_1);
    LED_Off(LED_mPLC_2);
    LED_Off(LED_mPLC_3);
    LED_Off(LED_mPLC_4);
}

