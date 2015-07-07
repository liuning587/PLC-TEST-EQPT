#include "includes.h"


// ---------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * 标志    控制字  消息内容    时间    情况
 *
 * 发送→  13H FE 68 AA AA AA AA AA AA 68 13 00 DF 16  2014-11-28 10:38:15 
 * 接收←  93H 68 00 36 19 00 00 00 68 93 06 33 69 4C 33 33 33 39 16   2014-11-28 10:38:17 成功
 * 发送→  11H FE 68 00 36 19 00 00 00 68 11 04 33 32 34 33 00 16  2014-11-28 10:39:07 
 * 接收←  91H 68 00 36 19 00 00 00 68 91 18 33 32 34 33 63 69 33 33 89 35 33 33 4C 33 33 33 6B 33 33 33 4A 66 33 33 E9 16     2014-11-28 10:39:09 成功
 */

const INT8U cPLC_READ_ENERGY[] = {0xFE, 0x68, 0x62, 0x33, 0x19, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x32, 0x34, 0x33, 0x5F, 0x16};

const INT8U mPLC_READ_ADDR[] = {0xFE, 0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x13, 0x00, 0xDF, 0x16};
const INT8U mPLC_REPLY_ADDR[] = {0xFE, 0x68, 0x62, 0x33, 0x19, 0x00, 0x00, 0x00, 0x68, 0x93, 0x06, 0x95, 0x66, 0x4C, 0x33, 0x33, 0x33, 0xF7, 0x16};

const INT8U mPLC_REPLY_ENERGY[] = {0x68, 0x62, 0x33, 0x19, 0x00, 0x00, 0x00, 0x68, 0x91, 0x18, 0x33, 0x32, 0x34, 0x33, 0x63, 0x69, 0x33, 0x33, 0x89, 0x35, 0x33, 0x33, 0x4C, 0x33, 0x33, 0x33, 0x6B, 0x33, 0x33, 0x33, 0x4A, 0x66, 0x33, 0x33, 0x48, 0x16};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------

OS_EVENT *g_sem_plc;
INT8U g_cur_freq, g_cur_mplc;
INT8U g_plc_num, g_tmp_plc_num;
bool g_sta_level_flag;
INT8U g_plc_index;

bool g_mplc_state[mPLC_NUM][PLC_FREQ_NUM] = {
	/* {PLC_FREQ_270KHz, PLC_FREQ_421KHz} */
	{FALSE, FALSE}, 
	{FALSE, FALSE},
	{FALSE, FALSE},
	{FALSE, FALSE}		
}; 

bool g_sta_level[mPLC_NUM][PLC_FREQ_NUM] = {
	/* {PLC_FREQ_270KHz, PLC_FREQ_421KHz} */
	{FALSE, FALSE}, 
	{FALSE, FALSE},
	{FALSE, FALSE},
	{FALSE, FALSE}		
}; 

DL645_Frame_C plc_frame_send;
DL645_Frame_C plc_frame_recv;
DL645_Frame_Stat_C plc_frame_stat;

INT8U plc_send_buf[256];
INT16U plc_send_len;

const INT8U mPLC_ADDR[6] = {0x62, 0x33, 0x19, 0x00, 0x00, 0x00};
const INT8U mPLC_ENERGY[20] = {0x30, 0x36, 0x00, 0x00, 0x56, 0x02, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x17, 0x33, 0x00, 0x00};

void PLC_Init(void)
{
    INT32U  temp;

    
    PLC_Config();

    mPLC_PWR_ON();
    mPLC_RESET_HIGH();
    mPLC_EVENT_LOW();
    mPLC_SELECT_EN();

    if(TRUE == plc_read_para(&temp))
    {
        temp &= 0xff;

        if((temp >= PLC_NUM_1) && (temp <= PLC_NUM_8))
        {
            g_plc_num = temp;
        }
        else
        {
            g_plc_num = PLC_NUM_1;
        }
    }
    else
    {
        g_plc_num = PLC_NUM_1;
    }

    g_plc_index = get_plc_index(g_plc_num);

    g_cur_mplc = mPLC_NUM_1;
    g_cur_freq = PLC_FREQ_270KHz;    
}

void cplc_read_energy_proc(void)
{
    INT8U  ctrl_code, data_len, dev_addr[6];
    INT32U  plc_data_item;


    memcpy(dev_addr, mPLC_ADDR, sizeof(mPLC_ADDR));

    dev_addr[PLC_NUM_INDEX] = g_plc_index;
    dev_addr[mPLC_NUM_INDEX] = g_cur_mplc;

    ctrl_code = PLC_READ_DATA;

    plc_data_item = PLC_DATA_BLOCK_ITEM;
    memcpy(plc_frame_send.Data, &plc_data_item, DL645_07_DATA_ITEM_LEN);
    data_len = DL645_07_DATA_ITEM_LEN;

    Create_DL645_Frame(dev_addr, ctrl_code, data_len, &plc_frame_send);

    plc_send_len = data_len + DL645_FIX_LEN;
    
    memcpy(&plc_send_buf[PLC_DL645_INDEX], &plc_frame_send, plc_send_len);
                        
    plc_send_buf[PLC_FREQ_INDEX] = PLC_DEFAULT_PREAMBLE;
    plc_send_len += PLC_FREQ_LEN;
}

INT16U cplc_read_energy(void)
{
    P_MSG_INFO  pMsg = NULL;
    OS_CPU_SR  cpu_sr;     
    

    if(!(pMsg = (P_MSG_INFO)alloc_send_buffer(MSG_SHORT)))
    {
        return (FALSE);
    }

    OS_ENTER_CRITICAL();
    cplc_read_energy_proc();
    memcpy(pMsg->msg_buffer, plc_send_buf, plc_send_len);
    pMsg->msg_header.msg_len = plc_send_len;

    if(PLC_FREQ_270KHz == g_cur_freq)
    {
        plc_send_buf[PLC_FREQ_INDEX] = PLC_FREQ_270KHz_PREAMBLE;
    }
    else
    {
        plc_send_buf[PLC_FREQ_INDEX] = PLC_FREQ_421KHz_PREAMBLE;
    }
    OS_EXIT_CRITICAL();
    
    memcpy(pMsg->msg_buffer, plc_send_buf, plc_send_len);

    pMsg->msg_header.msg_len = plc_send_len;

    pMsg->msg_header.end_id = cPLC_COM_PORT;

    pMsg->msg_header.need_buffer_free = TRUE;

    return (End_send(pMsg));
}

void mplc_assign_addr_proc(void)
{
    INT8U  ctrl_code, data_len, dev_addr[6];


    memcpy(dev_addr, mPLC_ADDR, sizeof(mPLC_ADDR));

    dev_addr[PLC_NUM_INDEX] = g_plc_index;
    dev_addr[mPLC_NUM_INDEX] = g_cur_mplc;

    ctrl_code = PLC_BROAD_REPLY_ADDR;

    memcpy(&plc_frame_send.Data, dev_addr, sizeof(dev_addr));
    data_len = sizeof(dev_addr);

    Create_DL645_Frame(dev_addr, ctrl_code, data_len, &plc_frame_send);

    plc_send_len = data_len + DL645_FIX_LEN;
    
    memcpy(&plc_send_buf[PLC_DL645_INDEX], &plc_frame_send, plc_send_len);
                        
    plc_send_buf[PLC_FREQ_INDEX] = PLC_DEFAULT_PREAMBLE;
    plc_send_len += PLC_FREQ_LEN;
}

INT16U mplc_assign_addr(void)
{
    P_MSG_INFO  pMsg = NULL;
    OS_CPU_SR  cpu_sr;      


    if(!(pMsg = (P_MSG_INFO)alloc_send_buffer(MSG_SHORT)))
    {
        return (FALSE);
    }

    OS_ENTER_CRITICAL();
    mplc_assign_addr_proc();
    memcpy(pMsg->msg_buffer, plc_send_buf, plc_send_len);
    pMsg->msg_header.msg_len = plc_send_len;
    OS_EXIT_CRITICAL();

    pMsg->msg_header.end_id = mPLC_COM_PORT;

    pMsg->msg_header.need_buffer_free = TRUE;

    return (End_send(pMsg));
}

void mplc_reply_energy_proc(void)
{
    INT8U  ctrl_code, data_len, dev_addr[6];
    INT32U  plc_data_item;


    memcpy(dev_addr, mPLC_ADDR, sizeof(mPLC_ADDR));

    dev_addr[PLC_NUM_INDEX] = g_plc_index;
    dev_addr[mPLC_NUM_INDEX] = g_cur_mplc;

    ctrl_code = PLC_REPLY_DATA;

    plc_data_item = PLC_DATA_BLOCK_ITEM;
    memcpy(plc_frame_send.Data, &plc_data_item, DL645_07_DATA_ITEM_LEN);
    memcpy(&plc_frame_send.Data[DL645_07_DATA_ITEM_LEN], mPLC_ENERGY, sizeof(mPLC_ENERGY));
    data_len = DL645_07_DATA_ITEM_LEN + sizeof(mPLC_ENERGY);

    Create_DL645_Frame(dev_addr, ctrl_code, data_len, &plc_frame_send);

    plc_send_len = data_len + DL645_FIX_LEN;

    memcpy(plc_send_buf, &plc_frame_send, plc_send_len);
}

INT16U mplc_reply_energy(void)
{
    P_MSG_INFO  pMsg = NULL;
    OS_CPU_SR  cpu_sr;      


    if(!(pMsg = (P_MSG_INFO)alloc_send_buffer(MSG_SHORT)))
    {
        return (FALSE);
    }

    OS_ENTER_CRITICAL();
    mplc_reply_energy_proc();
    memcpy(pMsg->msg_buffer, plc_send_buf, plc_send_len);
    pMsg->msg_header.msg_len = plc_send_len;
    OS_EXIT_CRITICAL();

    pMsg->msg_header.end_id = mPLC_COM_PORT;

    pMsg->msg_header.need_buffer_free = TRUE;

    return (End_send(pMsg));
}

INT8U proto_check_frame(void)
{
    INT8U  result;
    
    
    if(DL645_FRAME_ERROR == Analysis_DL645_Frame((INT8U *)mPLC_ADDR, 
                                                 (INT8U *)&plc_frame_recv,
                                                  &plc_frame_stat))
    {
         result = RECV_RES_INVALID;                                                 
    }
    else
    {                
        if((0 == plc_frame_stat.Status) || (0 == plc_frame_stat.C))
        {
            result = RECV_RES_INVALID;                            
        }
        else
        {
            result = RECV_RES_SUCC;
        }
    }
    
    return (result);
}

void cplc_recv_msg_proc(INT8U *pBuf, INT16U mLen)
{
    INT32U  plc_data_item;
        
    
    memcpy(&plc_frame_recv, pBuf, mLen);

    if(RECV_RES_SUCC == proto_check_frame())
    { 
        switch(plc_frame_stat.C)
        {
        case PLC_REPLY_DATA:
            plc_data_item = ((INT32U)plc_frame_recv.Data[3] << 24) | ((INT32U)plc_frame_recv.Data[2] << 16) | ((INT32U)plc_frame_recv.Data[1] << 8) | ((INT32U)plc_frame_recv.Data[0] << 0);

            if(PLC_DATA_BLOCK_ITEM == plc_data_item)
            {
                LED_PLC_TOGGLE();
                
                g_mplc_state[g_cur_mplc][g_cur_freq] = TRUE;
            }
            else
            {
                g_mplc_state[g_cur_mplc][g_cur_freq] = FALSE;
            }
            break;

        default:
            g_mplc_state[g_cur_mplc][g_cur_freq] = FALSE;
            break;
        }
    }
}

INT16U cPLC_postProcess(pvoid h)
{
    P_MSG_INFO  pMsg = (P_MSG_INFO)h;
    INT8U  *pBuf = (UCHAR *)(pMsg->msg_buffer);
    INT16U  mLen = pMsg->msg_header.msg_len, index;
    OS_CPU_SR  cpu_sr;    


    OS_ENTER_CRITICAL();

    for(index = 0; PLC_DEFAULT_PREAMBLE == pBuf[index]; index++);

    cplc_recv_msg_proc(&pBuf[index], mLen - index);

    OS_EXIT_CRITICAL();

    OSSemPost(g_sem_plc);

    return (TRUE);
}

void mplc_recv_msg_proc(INT8U *pBuf, INT16U mLen)
{
    INT32U  plc_data_item;
        
    
    memcpy(&plc_frame_recv, pBuf, mLen);

    if(RECV_RES_SUCC == proto_check_frame())
    { 
        switch(plc_frame_stat.C)
        {
        case PLC_BROAD_READ_ADDR:
            mplc_assign_addr();
            break;

        case PLC_READ_DATA:
            plc_data_item = ((INT32U)plc_frame_recv.Data[3] << 24) | ((INT32U)plc_frame_recv.Data[2] << 16) | ((INT32U)plc_frame_recv.Data[1] << 8) | ((INT32U)plc_frame_recv.Data[0] << 0);

            if(PLC_DATA_BLOCK_ITEM == plc_data_item)
            {
                mplc_reply_energy();
            }
            break;

        default:
            break;
        }
    }
}

INT16U mPLC_postProcess(pvoid h)
{
    P_MSG_INFO  pMsg = (P_MSG_INFO)h;
    INT8U  *pBuf = (UCHAR *)(pMsg->msg_buffer);
    INT16U  mLen = pMsg->msg_header.msg_len, index;
    OS_CPU_SR  cpu_sr;    


    OS_ENTER_CRITICAL();

    for(index = 0; PLC_DEFAULT_PREAMBLE == pBuf[index]; index++);

    mplc_recv_msg_proc(&pBuf[index], mLen - index);
    
    OS_EXIT_CRITICAL();

    return (TRUE);
}

