#include "includes.h"


INT8U g_msg_buf[MAX_MSG_LEN];
INT16U g_msg_len;

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

