#include "includes.h"


END_OBJ g_EndObjectPool[MAX_COM_PORT] =
{
    {LCD_COM_PORT, END_STATUS_IDLE, 0, 0, 0, NULL, NULL, NULL, NULL},
    {cPLC_COM_PORT, END_STATUS_IDLE, 0, 0, 0, NULL, NULL, NULL, NULL},
    {mPLC_COM_PORT, END_STATUS_IDLE, 0, 0, 0, NULL, NULL, NULL, NULL},    
};

const pvoid g_EndObject[END_OBJECT_NUM] =
{
	LCD_UART,
    cPLC_UART,
    mPLC_UART,
};

queue *g_EndTxQueue[MAX_COM_PORT] = {NULL};
queue *g_EndRxQueue[MAX_COM_PORT] = {NULL};

unsigned char g_LcdTxEndQueueBuf[((END_TX_QUEUE_SIZE+2) * 4)];
unsigned char g_LcdRxEndQueueBuf[((END_RX_QUEUE_SIZE+2) * 4)];

unsigned char g_cPLCTxEndQueueBuf[((END_TX_QUEUE_SIZE+2) * 4)];
unsigned char g_cPLCRxEndQueueBuf[((END_RX_QUEUE_SIZE+2) * 4)];

unsigned char g_mPLCTxEndQueueBuf[((END_TX_QUEUE_SIZE+2) * 4)];
unsigned char g_mPLCRxEndQueueBuf[((END_RX_QUEUE_SIZE+2) * 4)];

unsigned char *pTxEndQueueBuf[] = {
    g_LcdTxEndQueueBuf,
    g_cPLCTxEndQueueBuf,
    g_mPLCTxEndQueueBuf,
};

unsigned char *pRxEndQueueBuf[] = {
    g_LcdRxEndQueueBuf,
    g_cPLCRxEndQueueBuf,
    g_mPLCRxEndQueueBuf, 
};

/***********************************************************
END���ʼ��< // ��Ҫ�ṩһ��reset �ӿڹ��ϲ����>
************************************************************/

OS_EVENT *g_sem_end;

UART_CCB g_uart_ccb[MAX_COM_PORT];

U8 g_UartLcdRxBuf[UART_RECV_BUF_SIZE];
U8 g_UartcPLCRxBuf[UART_RECV_BUF_SIZE];
U8 g_UartmPLCRxBuf[UART_RECV_BUF_SIZE];

U8 *pUartRxBuf[] = {
    g_UartLcdRxBuf,
    g_UartcPLCRxBuf,
    g_UartmPLCRxBuf,
};

U32 UART_ReceiveData(U8 end_id, UCHAR* rxbuf, USHORT rxnum)
{
    OS_CPU_SR  cpu_sr;
    P_UART_CCB p_uc = &g_uart_ccb[end_id];

    if( (rxnum < 1) || (end_id >= MAX_COM_PORT) )
    {
        return FALSE;
    }
    OS_ENTER_CRITICAL();
    //p_uc->gUartRxCnt = 0;
    //p_uc->gUartRxLen = rxnum;
    p_uc->gpUartRxAddress = rxbuf;
    p_uc->gpUartRxStartAddress = rxbuf;
    p_uc->gpUartRxReadAddress = rxbuf;
    p_uc->gpUartRxEndAddress = (UCHAR*)((ULONG)rxbuf + rxnum);
    OS_EXIT_CRITICAL()
    return TRUE;
}


void End_Init(void)
{
    P_END_OBJ pEndObj = NULL;
    unsigned char i;
    

    mem_msg_buffer_init((MSG_INFO *)gShortMsgPool, (P_MSG_INFO *)pShortMsgPool, MAX_MSG_SHORT, sizeof(MSG_SHORT_INFO));

    //alan test  ��Ҫ��ʱע�͵�, ��֪ΪɶIIC Start һ����, MCU �ͷ���.
    for( i = START_COM_PORT; i < MAX_COM_PORT; i++)
    {
        // �ҵ���ǰEnd Object
        pEndObj = g_EndObjectPool + i;

        /* end queue[x] initialize */ /* each end object define '50 block' queue */
        g_EndTxQueue[i] = define_new_queue((queue *)pTxEndQueueBuf[i], END_TX_QUEUE_SIZE);
        g_EndRxQueue[i] = define_new_queue((queue *)pRxEndQueueBuf[i], END_RX_QUEUE_SIZE);
       
        pEndObj->end_recv_buffer = (unsigned char *)pUartRxBuf[i];

        pEndObj->last_receive_len = 0;
        pEndObj->receive_len = 0;

        pEndObj->recv_timeout = 0;

        UART_ReceiveData(i, pEndObj->end_recv_buffer, UART_RECV_BUF_SIZE);

        // ���д���״̬ת��REVC STATUS
        pEndObj->end_send_status = END_STATUS_IDLE;

        USART_ITConfig((USART_TypeDef *)g_EndObject[i], USART_IT_RXNE, ENABLE);
    }
}


/***********************************************************
Tick������ã����ÿ��END�ӿ��Ƿ����µ�frame����
************************************************************/
unsigned short End_tick_check(void)
{
    OS_CPU_SR  cpu_sr;
    unsigned char i;
    U16 cp_len, msg_len;

    P_END_OBJ     pEndObj = NULL;
    P_MSG_INFO     pnewmsg = NULL;

    P_UART_CCB p_uc;

    for(i = START_COM_PORT; i < MAX_COM_PORT; i++ )
    {
        /////////////////////////////
        pEndObj = g_EndObjectPool + i;

        p_uc = &g_uart_ccb[i];

        if(pEndObj->end_send_status == END_STATUS_SENDING)
        {
            /* ��鵱ǰ�����Ƿ������. ��ϵĻ�, ת����,   ����ת����*/
            if( End_check_send(pEndObj->end_id) == TRUE )
            {

                //Send  �Ѿ������� ת��receive status ��ʼ״̬
                pEndObj->end_send_status = END_STATUS_IDLE;

                pnewmsg = (P_MSG_INFO)pEndObj->pMsgInfo;

                pnewmsg->msg_header.block_state = SENDED;

                // alan dynamic sending buffer.
                // ��ǰend sending ��������������ֱ���ͷ�sending buffer
                if( pnewmsg->msg_header.need_buffer_free == TRUE)    /* ����Ҫ�������ϲ�application �����ͷ�*/
                {
                    free_send_buffer(pnewmsg);
                }
            }
        }
        else
        {
            if( NULL != (pnewmsg = dequeue( g_EndTxQueue[pEndObj->end_id])) )
            {
                End_send(pnewmsg);
            }
        }

        if( End_check_recv(pEndObj) == TRUE ) /* û�������ݵ���, ��ʼ����*/
        {
#if 0            
            // ���ڴ�����copy  ����, ѹ��queue ���к�������
            if(pEndObj->receive_len >= MSA_MESSAGAE_MAX_SIZE)
                pnewmsg = alloc_send_buffer(MSG_LARGE);
            else if(pEndObj->receive_len >= PLC_BUFFER_UART_SIZE)
                pnewmsg = alloc_send_buffer(MSG_LONG);
            else
                pnewmsg = alloc_send_buffer(MSG_SHORT);
#else
            pnewmsg = alloc_send_buffer(MSG_SHORT);
#endif

            if(pnewmsg == NULL)
            {               
                continue;
            }

            msg_len = pEndObj->receive_len;

            pnewmsg->msg_header.msg_len = msg_len;
            pnewmsg->msg_header.end_id = i;

            OS_ENTER_CRITICAL();
            cp_len = (U16)(p_uc->gpUartRxEndAddress - p_uc->gpUartRxReadAddress);
            

            if(cp_len >= msg_len)
            {
                //OS_ENTER_CRITICAL();
                memcpy(pnewmsg->msg_buffer, p_uc->gpUartRxReadAddress, msg_len );                
                p_uc->gpUartRxReadAddress += msg_len;
                if(p_uc->gpUartRxReadAddress == p_uc->gpUartRxEndAddress)
                    p_uc->gpUartRxReadAddress = p_uc->gpUartRxStartAddress;
                //OS_EXIT_CRITICAL();
            }
            else
            {
                //OS_ENTER_CRITICAL();
                memcpy(pnewmsg->msg_buffer, p_uc->gpUartRxReadAddress, cp_len);
                memcpy((pnewmsg->msg_buffer + cp_len), p_uc->gpUartRxStartAddress, (msg_len-cp_len));                
                p_uc->gpUartRxReadAddress = p_uc->gpUartRxStartAddress + (msg_len-cp_len);
                //OS_EXIT_CRITICAL();
            }
            OS_EXIT_CRITICAL();

            pEndObj->last_receive_len = 0;
            pEndObj->receive_len = 0;

            pEndObj->endStatistics.rxPacketCount++;

            /* �����ݹ���queue ��*/
            if(enqueue(g_EndRxQueue[pEndObj->end_id], (P_MSG_INFO)(pnewmsg)))
            {			
                // ����UART Rx �����¼���
                OSSemPost(g_sem_end);
            }
            else
            {
                free_send_buffer(pnewmsg);
            }       
        }
    }

    return TRUE;
}


P_END_OBJ End_get_end_obj(UCHAR end_id)
{
    unsigned char i = 0;
    while( i < MAX_COM_PORT)
    {
        if( g_EndObjectPool[i].end_id == end_id)
            return &g_EndObjectPool[i];
        i++;
    }

    return NULL;
}

U32 End_uart_send(UCHAR end_id, UCHAR *txbuf, USHORT txnum)
{
    USART_TypeDef *UARTx;
    P_UART_CCB p_uc;
    UCHAR send_byte = 0;
    

    if(txnum < 1)
    {
        return FALSE;
    }

    switch(end_id)
    {    
    case LCD_COM_PORT:      
        UARTx = LCD_UART;
        break; 
        
    case cPLC_COM_PORT:      
        UARTx = cPLC_UART;
        break; 
        
    case mPLC_COM_PORT:      
        UARTx = mPLC_UART;
        break; 
        
    default:
        return FALSE;
    }

    p_uc = &g_uart_ccb[end_id];

    p_uc->gpUartTxAddress = txbuf;
    p_uc->gUartTxCnt = txnum;

    send_byte = *(p_uc->gpUartTxAddress);

    p_uc->gpUartTxAddress++;
    p_uc->gUartTxCnt--;

    USART_SendData(UARTx, send_byte);

    if(p_uc->gUartTxCnt)
        USART_ITConfig(UARTx, USART_IT_TXE, ENABLE);

    return TRUE;
}

/***********************************************************
pEndObj:  ���ͽӿ�
pMsgInfo: ������Ϣ����ͨ����·�㷢����Ϣ
************************************************************/
unsigned short End_send( P_MSG_INFO pMsgInfo)
{
    P_END_OBJ pEndObj;

    if( (( pEndObj = End_get_end_obj(pMsgInfo->msg_header.end_id)) == NULL) ||
            ( pMsgInfo->msg_header.msg_len == 0) )
    {
        // alan dynamic sending buffer.
        // ��Щ����ʧ��ʱ�����ﲻֱ���ͷ�sending buffer����end send �����ͷš�
        if( pMsgInfo->msg_header.need_buffer_free == TRUE)
        {
            free_send_buffer(pMsgInfo);
        }

        return FALSE;
    }


    if( END_IDLE != End_IsIdle(pEndObj) )
    {
        /* ����IDLE State, �����ݹ���queue ��*/
        enqueue(g_EndTxQueue[pEndObj->end_id], pMsgInfo);

        pMsgInfo->msg_header.time_stamp = OSTimeGet();

        return TRUE;
    }


    if( TRUE == End_uart_send( pEndObj->end_id, pMsgInfo->msg_buffer, pMsgInfo->msg_header.msg_len) )
    {

        // ����buffer ֱ�ӹ���end object �� ��������ֱ���ͷ�!
        pEndObj->pMsgInfo = pMsgInfo;

        // �ײ�״̬Ǩ�Ƶ�SENDING
        pEndObj->end_send_status = END_STATUS_SENDING;

        pMsgInfo->msg_header.block_state = SENDING;

        pEndObj->endStatistics.txPacketCount++;

        pMsgInfo->msg_header.time_stamp = OSTimeGet();

        
        return TRUE;
    }
    else
    {
        if( pMsgInfo->msg_header.need_buffer_free == TRUE)
        {
            free_send_buffer(pMsgInfo);
        }

        return FALSE;
    }

}

unsigned char End_check_recv(P_END_OBJ pEndObj)
{
    P_UART_CCB p_uc = &g_uart_ccb[pEndObj->end_id];

    // ʵʱ��¼��ǰUART Rx ���ݳ���
    pEndObj->last_receive_len = pEndObj->receive_len;

    //pEndObj->receive_len = gUart1RxCnt;
    if(p_uc->gpUartRxReadAddress <= p_uc->gpUartRxAddress)
        pEndObj->receive_len = p_uc->gpUartRxAddress - p_uc->gpUartRxReadAddress;//gIic0RxCnt;
    else
        pEndObj->receive_len = (USHORT)((ULONG)p_uc->gpUartRxAddress + UART_RECV_BUF_SIZE - (ULONG)p_uc->gpUartRxReadAddress);

    if(pEndObj->receive_len > (220))//if(pEndObj->receive_len > (UART_RECV_BUF_SIZE/2))
    {
        pEndObj->recv_timeout = 0;
        return TRUE;
    }


    if((pEndObj->receive_len != 0) && (pEndObj->receive_len == pEndObj->last_receive_len))
    {
        /* û���µĽ������ݵ���,ֱ�ӷ���FALSE �����ϲ���Դ�����*/
        pEndObj->recv_timeout++;
        if(pEndObj->recv_timeout > 1)
        {
            pEndObj->recv_timeout = 0;
            return TRUE;
        }
        else
        {
            return FALSE;
        }

    }
    else
    {
        /* �����µĽ�������*/
        pEndObj->recv_timeout = 0;
        return FALSE;
    }

}

/**********************************************

�жϵ�ǰEnd Send ���ݷ�����û��. �����귵��TRUE,  û�����귵��FALSE

**********************************************/
unsigned char End_check_send(UCHAR end_id)
{

    if( g_uart_ccb[end_id].gUartTxCnt == 0)
        /* �����������, ����TRUE �����ϲ�״̬Ǩ��*/
        return TRUE;
    else
        /* ��������û�������, ��������*/
        return FALSE;

}


/**************************************************************************************

��������:       �ײ㳭�����ݻ����ص�����. �����ｫ����д����ڴ�, ������һ�����д.

�������:      �س����ݽ��.

**************************************************************************************/
unsigned char End_postProcess(unsigned char end_type,  pvoid h)
{
    P_MSG_INFO pMsg = (P_MSG_INFO)h;
    USHORT iRet = FALSE;
    

    switch(end_type)
    {
    case LCD_COM_PORT:
        iRet = LCD_postProcess(pMsg);
        break;

    case cPLC_COM_PORT:
        iRet = cPLC_postProcess(pMsg);
        break;    

    case mPLC_COM_PORT:
        iRet = mPLC_postProcess(pMsg);
        break;        

    default:
        break;  
    }

    free_send_buffer(pMsg);

    return iRet;
}


// ��ѯ�ײ�end ��ǰ״̬�Ƿ����??
unsigned char End_IsIdle(P_END_OBJ pEndObj)
{

    if( pEndObj->end_send_status == END_STATUS_IDLE)
        return END_IDLE;
    else
        return END_BUSY;
    

}

void UART_IRQProc(UART_CCB *uccb, USART_TypeDef *UARTx)
{
  if(USART_GetITStatus(UARTx, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */    
    *(uccb->gpUartRxAddress) = USART_ReceiveData(UARTx);
    uccb->gpUartRxAddress++;

    if(uccb->gpUartRxAddress == uccb->gpUartRxEndAddress)
    {
        //��ͷ
        uccb->gpUartRxAddress = uccb->gpUartRxStartAddress;
    }
	
    if(uccb->gpUartRxReadAddress == uccb->gpUartRxAddress)
    {
        //���Կ��ǼӴ���ͳ��
    } 
    
#if (LED_UART_EN > 0u)
    LED_UART_ON();
#endif
  }

  if(USART_GetITStatus(UARTx, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    if( uccb->gUartTxCnt > 0 )
    {
        USART_SendData(UARTx, *(uccb->gpUartTxAddress));
    	uccb->gpUartTxAddress++;   	
    	uccb->gUartTxCnt--;
	}
    else
    {        
        USART_ITConfig(UARTx, USART_IT_TXE, DISABLE);
    }

#if (LED_UART_EN > 0u)
    LED_UART_ON();
#endif	
  }
}


