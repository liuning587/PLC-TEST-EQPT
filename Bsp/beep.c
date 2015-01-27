#include "includes.h"


INT8U g_beep_type;
INT8U g_beep_cnt;

void BEEP_Init(void)
{
    BEEP_Config();
    
    beep_off();
}

void beep(void)
{
    switch(g_beep_type)
    {
    case BEEP_TYPE_NONE:
        BUZZER_OFF();
        PLC_TEST_RESET();
        break;
        
    case BEEP_TYPE_NORMAL:
        if(g_beep_cnt)
        {
            g_beep_cnt--;

            if(g_beep_cnt)
            {
                BUZZER_ON();
                
                PLC_TEST_ONCE();
            }
            else
            {
                BUZZER_OFF();

                PLC_TEST_RESET();
            }
        }        
        break;

    case BEEP_TYPE_WARNING:
        if(g_beep_cnt)
        {
            g_beep_cnt--;
        
            if(g_beep_cnt)
            {
                BUZZER_TOGGLE();

                PLC_TEST_ONCE();
            }
            else
            {
                BUZZER_OFF();

                PLC_TEST_RESET();
            }
        } 
        break;

    default:
        beep_off();       
        break;
    }
}

void beep_on(void)
{
    if(TRUE == get_mplc_state())
    {
        g_beep_type = BEEP_TYPE_NORMAL;
        g_beep_cnt = BEEP_TYPE_NORMAL_PERIOD;
    }
    else
    {
        g_beep_type = BEEP_TYPE_WARNING;
        g_beep_cnt = BEEP_TYPE_WARNING_PERIOD;
    }
}

void beep_off(void)
{
    g_beep_type = BEEP_TYPE_NONE;
    g_beep_cnt = BEEP_TYPE_NONE_PERIOD;

    BUZZER_OFF();

    PLC_TEST_RESET();
}

