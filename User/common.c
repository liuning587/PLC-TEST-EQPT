#include "includes.h"


INT8U g_msg_buf[MAX_MSG_LEN];
INT16U g_msg_len;

INT8U plc_read_para(INT32U *pBuf)
{
    INT8U  i;
    INT32U  *ptr = (INT32U *)PLC_PARA_ADDR;
    INT32U  temp;
    FLASH_Status  stat;
    OS_CPU_SR  cpu_sr; 


    for(i = 0; i < 0xff; i++)
    {
        if(0xffffffff != *ptr)
        {
            temp = *ptr;
            ptr++;
        }
        else
        {
            if(0 == i)
            {
                temp = PLC_NUM_1;

                FLASH_Unlock();

                OS_ENTER_CRITICAL();
                stat = FLASH_ProgramWord((uint32_t)ptr, temp);
                OS_EXIT_CRITICAL();

                if(FLASH_COMPLETE != stat)
                {
                    FLASH_Lock();

                    return (FALSE);
                }            

                FLASH_Lock();     
            }
            else
            {
                *pBuf = temp;
            }

            break;
        }
    }
    
    return (TRUE);
}

INT8U plc_write_para(INT32U *pBuf)
{
    INT8U  i;
    INT32U  *ptr = (INT32U *)PLC_PARA_ADDR;
    INT32U  temp;
    FLASH_Status  stat;
    OS_CPU_SR  cpu_sr; 


    for(i = 0; i < 0xff; i++)
    {
        if(0xfe == i)
        {
            FLASH_Unlock();

            OS_ENTER_CRITICAL();
            stat = FLASH_ErasePage(PLC_PARA_ADDR);
            OS_EXIT_CRITICAL();

            if(FLASH_COMPLETE != stat)
            {
                FLASH_Lock();
            
                return (FALSE);
            }            

            temp = *pBuf;

            OS_ENTER_CRITICAL();
            stat = FLASH_ProgramWord((uint32_t)PLC_PARA_ADDR, temp);  
            OS_EXIT_CRITICAL();

            if(FLASH_COMPLETE != stat)
            {
                FLASH_Lock();
            
                return (FALSE);
            }            
            
            FLASH_Lock();                        
            
            break;
        }

        if(0xffffffff != *ptr)
        {
            ptr++;
        }
        else
        {
            temp = *pBuf;
            
            FLASH_Unlock();

            OS_ENTER_CRITICAL();
            stat = FLASH_ProgramWord((uint32_t)ptr, temp);
            OS_EXIT_CRITICAL();
            
            if(FLASH_COMPLETE != stat)
            {
                FLASH_Lock();
            
                return (FALSE);
            }            
            
            FLASH_Lock(); 

            break;
        }
    }
    
    return (TRUE);
}

INT8U get_plc_index(INT8U plc_num)
{
    INT8U  index;

    
    switch(plc_num)
    {
    case PLC_NUM_1:
        index = 0;
        break;
    
    case PLC_NUM_2:
        index = 1;
        break;
    
    case PLC_NUM_3:
        index = 2;
        break;
    
    case PLC_NUM_4:
        index = 3;
        break;
    
    case PLC_NUM_5:
        index = 4;
        break;
    
    case PLC_NUM_6:
        index = 5;
        break;
    
    case PLC_NUM_7:
        index = 6;
        break;
    
    case PLC_NUM_8:
        index = 7;
        break;             
    
    default:
        index = 0;
        break;
    }

    return (index);
}

bool get_mplc_state(void)
{
    bool  *ptr;
    INT8U  i;


    ptr = (bool *)g_mplc_state;

    for(i = 0; i < MAX_mPLC_STATE_EDIT_NUM; i++)
    {
        if(FALSE == *ptr)
        {
            return (FALSE);
        }

        ptr++;
    }

    return (TRUE);
}

int nstrcmp(const char *str1, const char *str2, int num)
{
    if((NULL == str1) ||
       (NULL == str2))
    {
        return (-2);
    }

    if(0 == num)
    {
        return (2);
    }

    if(str1 == str2)
    {
        return (0);
    }
    
    while(num)
    {
        if((*str1) == (*str2))
        {
            str1++;
            str2++;

            num--;
        }
        else if((*str1) > (*str2))
        {
            return (1);
        }
        else
        {
            return (-1);
        }
    }

    return (0);
}

