#include "includes.h"


OS_EVENT *g_sem_key;
INT32U g_key_msg;

void  GUI_X_Init (void) 
{
    g_key_msg = 0;
}

int  GUI_X_GetKey (void) 
{
    int  r;


    r         = g_key_msg;
    g_key_msg = 0;
    
    return (r);
}

int  GUI_X_WaitKey (void) 
{
    int    r;
    INT8U  err;


    r = 0;
    
    if (0 == g_key_msg) {
        OSSemPend(g_sem_key, 0, &err);
    }
    
    if(RESET == GPIO_ReadInputDataBit(KEY_RESET_PORT, g_key_msg))
    {
        OSTimeDlyHMSM(0, 0, 0, 30);

        if(RESET == GPIO_ReadInputDataBit(KEY_RESET_PORT, g_key_msg))
        {
            r = g_key_msg;
        }
    }
    
    g_key_msg = 0;
    
    return (r);
}

void  GUI_X_StoreKey (int k) 
{
    g_key_msg = k;
    
    OSSemPost(g_sem_key);
}

void  GUI_X_KeyProc (int k)
{
    INT32U  key_msg;


    key_msg = k;
    
    switch(key_msg)
    {
    case KEY_MSG_RESET:
        stm32_soft_reset();
        break;

    default:
        break;
    }
}

