#include "includes.h"


UCHAR FreeMsgCnt[MAX_MSG_ITEM] =
{
    MAX_MSG_SHORT,
    MAX_MSG_LONG,
    MAX_MSG_LARGE
};

const UCHAR MAX_MSG_CNT[MAX_MSG_ITEM] =
{
    MAX_MSG_SHORT,
    MAX_MSG_LONG,
    MAX_MSG_LARGE
};

MSG_SHORT_INFO gShortMsgPool[MAX_MSG_SHORT];
P_SHORT_MSG_INFO pShortMsgPool[MAX_MSG_SHORT] = {NULL};

/**********************************************************

��������:      ��'from -> to'  ����������memory ��ʼ�����.

���뺯��:    �ڴ���ʼ��ַ-> �ڴ������ַ

**********************************************************/
void mem_zeroinit(pvoid from, pvoid to)
{
    unsigned char* p_start =(unsigned char*)from;
    unsigned char* p_end =(unsigned char*)to;
    unsigned char* tmp;
    unsigned long size = 0;


    if( p_start > p_end )
    {
        tmp = p_end;
        p_end = p_start;
        p_start = tmp;
    }

    size = (unsigned long)p_end - (unsigned long)p_start;
    memset((unsigned char*)p_start, 0, size);
}

void mem_msg_buffer_init(MSG_INFO * MsgPool, P_MSG_INFO * MsgArray, unsigned short MsgNum, U16 msg_size)
{
    U32 size;
    U32 i;


    size = msg_size * (U32)MsgNum;

    mem_zeroinit((pvoid)(MsgPool),  (pvoid)((unsigned long)(MsgPool) + size) );

    i = 0;
    while (i < MsgNum)
    {
        // ��ʼ���ָ�ײ���������buffer queue (10 memory blocks).
        MsgArray[i] = (P_MSG_INFO)(((U32)MsgPool) + msg_size*i);

        // initial ..
        MsgArray[i]->msg_header.block_state = FREE;    /* ��ʼ��ΪFREE  */

        i++;
    }
}

/*******************************************************

����˵��:  ��pMsgTxPool[i]  ������һ֡����buffer

*******************************************************/
P_MSG_INFO alloc_send_buffer(unsigned char type)
{
    P_MSG_INFO pmsg = NULL;
    P_MSG_INFO *pool = NULL;
    unsigned short i;
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif


    if( type == MSG_LARGE)
    {
        //pool = (P_MSG_INFO *)pLargeMsgPool;
        return NULL;
    }
    else if( type == MSG_LONG)
    {
        //pool = (P_MSG_INFO *)pLongMsgPool;
        return NULL;
    }
    else if( type == MSG_SHORT)
    {
        pool = (P_MSG_INFO *)pShortMsgPool;
    }
    else
        return NULL;

    /* �������п���buffer , �ҵ�һ�����ÿ���buffer */
    for(i =0; i<GET_MAX_MSG(type); i++)
    {
        pmsg = (P_MSG_INFO)(*(pool + i));

        OS_ENTER_CRITICAL();
        if(pmsg->msg_header.block_state == FREE)
        {
            pmsg->msg_header.block_state = ALLOC;
            OS_EXIT_CRITICAL();
            pmsg->msg_header.msg_len = 0;
            pmsg->msg_header.time_stamp = OSTimeGet();
            break;
        }
        OS_EXIT_CRITICAL();
    }

    if( i >= GET_MAX_MSG(type) )
        return NULL;
   
    memset(pmsg->msg_buffer, 0xff, UART_RECEIVE_BUF_SIZE);

    /* ͳ�Ƽ����ݼ�*/
    //FreeMSGTxCnt--;

    OS_ENTER_CRITICAL();
    FreeMsgCnt[type]--;
    OS_EXIT_CRITICAL();

    //if( FreeMSGTxCnt <= 3 )
    if( FreeMsgCnt[type] == 0 )
    {
        //ALERT(" left msg number �ݼ����ٽ�ֵ ");
        //Alert((type), ALERT_RESET_DEVICE, __FILE__, __LINE__);
    }

    return pmsg;
}

/*******************************************************

����˵��:  ��pMsgTxPool[i]  ���ͷ�һ֡����buffer (   *pmsg
)

*******************************************************/
unsigned char free_send_buffer(pvoid pmsg )
{
    unsigned char i = 0, type;
    P_MSG_INFO	pfree = (P_MSG_INFO)pmsg;
    P_MSG_INFO	*pool = NULL;
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif


    if(pfree->msg_header.block_state == FREE)
        return FALSE;

    pool = (P_MSG_INFO *)pShortMsgPool;

    type = MSG_SHORT;

//next_find_msg:
    /*-------------------------------------------*/
    for(i = 0; i < GET_MAX_MSG(type); i++)
    {
        OS_ENTER_CRITICAL();
        if((*(pool + i ) == pfree))
        {
            /* �ͷ��ڴ��ΪFREE */
            if(pfree->msg_header.block_state != FREE)
            {
                pfree->msg_header.block_state = FREE;
            }
            else
            {
                OS_EXIT_CRITICAL();
                //LIST_ADD_STR_WITHOUT_T("Free Failed 1");
                return FALSE;
            }

            OS_EXIT_CRITICAL();
            break;
        }
        OS_EXIT_CRITICAL();
    }

#if 0
    if(i >= GET_MAX_MSGITEM(type))
    {
        if(type == MSG_SHORT)
        {
            type = MSG_LONG;
            pool = (P_MSG_INFO *)pLongMsgPool;
            goto next_find_msg;
        }
        if(type == MSG_LONG)
        {
            type = MSG_LARGE;
            pool = (P_MSG_INFO *)pLargeMsgPool;
            goto next_find_msg;
        }
        LIST_ADD_STR_WITHOUT_T("Free Failed 2");
        return FALSE;
    }
#endif

    /* ͳ�Ƽ����ݼ�*/
    OS_ENTER_CRITICAL();
    FreeMsgCnt[type]++;
    OS_EXIT_CRITICAL();

    //if( FreeMSGTxCnt > GET_MAX_MSG(type) )
    if( FreeMsgCnt[type] > GET_MAX_MSG(type) )
    {
        //ALERT(" mail queue �������� ");
        //Alert(ALERT_NO_MEMORY, ALERT_RESET_DEVICE, __FILE__, __LINE__);
    }

    return TRUE;
}

