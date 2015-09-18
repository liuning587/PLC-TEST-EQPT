#include "includes.h"


KEY_PARA g_key_para;

void  GUI_X_Init (void) 
{
    g_key_para.msg.GPIOx    = KEY_PORT_NULL;
    g_key_para.msg.GPIO_Pin = KEY_MSG_NONE;
}

int  GUI_X_GetKey (void) 
{
    int r;


    r                       = g_key_para.msg.GPIO_Pin;
    g_key_para.msg.GPIOx    = KEY_PORT_NULL;
    g_key_para.msg.GPIO_Pin = KEY_MSG_NONE;
    
    return (r);
}

int  GUI_X_WaitKey (void) 
{
    int   r;
    INT8U err;


    r = KEY_MSG_NONE;
    
    if((KEY_PORT_NULL == g_key_para.msg.GPIOx) || 
       (KEY_MSG_NONE == g_key_para.msg.GPIO_Pin)) 
    {
        OSSemPend(g_key_para.sem, 0, &err);
    }

    if(OS_ERR_NONE == err)
    {
        if(RESET == GPIO_ReadInputDataBit(g_key_para.msg.GPIOx, g_key_para.msg.GPIO_Pin))
        {
            OSTimeDlyHMSM(0, 0, 0, 30);

            if(RESET == GPIO_ReadInputDataBit(g_key_para.msg.GPIOx, g_key_para.msg.GPIO_Pin))
            {
                r = g_key_para.msg.GPIO_Pin;
            }
        }
    }

    g_key_para.msg.GPIOx    = KEY_PORT_NULL;
    g_key_para.msg.GPIO_Pin = KEY_MSG_NONE;
    
    return (r);
}

void  GUI_X_StoreKey (GPIO_TypeDef *GPIOx, INT16U GPIO_Pin) 
{
    g_key_para.msg.GPIOx = GPIOx;
    g_key_para.msg.GPIO_Pin = GPIO_Pin;

    OSSemPost(g_key_para.sem);
}

void  GUI_X_KeyProc (int k)
{
    INT32U key_msg;


    key_msg = k;
    
    switch(key_msg)
    {
    case KEY_MSG_RESET:
        STM32_SoftReset();
        break;

    default:
        break;
    }
}

