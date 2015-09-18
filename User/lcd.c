#include "includes.h"


const INT16U g_mplc_state_edit_addr[MAX_mPLC_STATE_EDIT_NUM] = {
    mPLC_STATE_EDIT1_ADDR,
    mPLC_STATE_EDIT2_ADDR,
    mPLC_STATE_EDIT3_ADDR,
    mPLC_STATE_EDIT4_ADDR,
    mPLC_STATE_EDIT5_ADDR,
    mPLC_STATE_EDIT6_ADDR,
    mPLC_STATE_EDIT7_ADDR,
    mPLC_STATE_EDIT8_ADDR,
};

const INT16U g_mplc_state_edit_dp[MAX_mPLC_STATE_EDIT_NUM] = {
    mPLC_STATE_EDIT1_DP,
    mPLC_STATE_EDIT2_DP,
    mPLC_STATE_EDIT3_DP,
    mPLC_STATE_EDIT4_DP,
    mPLC_STATE_EDIT5_DP,
    mPLC_STATE_EDIT6_DP,
    mPLC_STATE_EDIT7_DP,
    mPLC_STATE_EDIT8_DP,
};

const INT16U g_err_edit_addr[MAX_ERR_EDIT_NUM] = {
    ERR_EDIT1_ADDR,
    ERR_EDIT2_ADDR,
    ERR_EDIT3_ADDR,
    ERR_EDIT4_ADDR,
    ERR_EDIT5_ADDR,
    ERR_EDIT6_ADDR,
    ERR_EDIT7_ADDR,
    ERR_EDIT8_ADDR,
};

const INT16U g_err_edit_dp[MAX_ERR_EDIT_NUM] = {
    ERR_EDIT1_DP,
    ERR_EDIT2_DP,
    ERR_EDIT3_DP,
    ERR_EDIT4_DP,
    ERR_EDIT5_DP,
    ERR_EDIT6_DP,
    ERR_EDIT7_DP,
    ERR_EDIT8_DP,
};

const INT16U g_os_info_edit_addr[MAX_OS_INFO_EDIT_NUM] = {
    USG_EDIT_ADDR,
    VER_EDIT_ADDR,
};

const INT16U g_os_info_edit_dp[MAX_OS_INFO_EDIT_NUM] = {
    USG_EDIT_DP,
    VER_EDIT_DP,
};

const INT16U g_plc_group_edit_addr[MAX_PLC_GROUP_EDIT_NUM] = {
    PLC_GROUP_EDIT1_ADDR,
    PLC_GROUP_EDIT2_ADDR,
};

const INT16U g_plc_group_edit_dp[MAX_PLC_GROUP_EDIT_NUM] = {
    PLC_GROUP_EDIT1_DP,
    PLC_GROUP_EDIT2_DP,
};

const INT8U LCD_READ_ID[] = {0x5a, 0xa5, 0x03, 0x81, 0x00, 0x01};
const INT8U LCD_PARA_UPLOAD[] = {0x5a, 0xa5, 0x06, 0x83, 0x05, 0x08};
const INT8U LCD_OK_UPLOAD[] = {0x5a, 0xa5, 0x06, 0x83, 0x07, 0xff};

OS_EVENT *g_sem_disp;

const INT8U g_color_code[8] = {0x5a, 0xa5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x00};
const INT8U g_font_code[10] = {0x5a, 0xa5, 0x07, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

INT8U g_color_buf[8];
INT8U g_font_buf[10];

INT16U lcd_read_id(void)
{
    P_MSG_INFO  pMsg = NULL;
    

    if(!(pMsg = (P_MSG_INFO)alloc_send_buffer(MSG_SHORT)))
    {
        return (FALSE);
    }

    memcpy(pMsg->msg_buffer, LCD_READ_ID, sizeof(LCD_READ_ID));

    pMsg->msg_header.msg_len = sizeof(LCD_READ_ID);

    pMsg->msg_header.end_id = LCD_COM_PORT;

    pMsg->msg_header.need_buffer_free = TRUE;
    
    return (End_send(pMsg));
}

INT16U lcd_disp(INT8U *buf, INT8U len)
{
    P_MSG_INFO  pMsg = NULL;
    

    if(!(pMsg = (P_MSG_INFO)alloc_send_buffer(MSG_SHORT)))
    {
        return (FALSE);
    }

    memcpy(pMsg->msg_buffer, buf, len);

    pMsg->msg_header.msg_len = len;

    pMsg->msg_header.end_id = LCD_COM_PORT;

    pMsg->msg_header.need_buffer_free = TRUE;
    
    return (End_send(pMsg));
}

INT16U LCD_postProcess(pvoid h)
{
    P_MSG_INFO  pMsg = (P_MSG_INFO)h;
    static  INT32U  plc_group_new = PLC_GROUP_NONE;
    INT8U  *pBuf = (UCHAR *)(pMsg->msg_buffer);
    INT16U  mLen = pMsg->msg_header.msg_len;
    OS_CPU_SR_ALLOC();   


    OS_ENTER_CRITICAL();
    g_msg_len = mLen;
    memcpy(g_msg_buf, pBuf, mLen);

    if((LCD_AUTO_UPLOAD_LEN == mLen) && !(nstrcmp((char *)g_msg_buf, (char *)LCD_PARA_UPLOAD, sizeof(LCD_PARA_UPLOAD))))
    {
        plc_group_new = g_msg_buf[LCD_AUTO_UPLOAD_LEN - 1];
    }

    if((LCD_AUTO_UPLOAD_LEN == mLen) && !(nstrcmp((char *)g_msg_buf, (char *)LCD_OK_UPLOAD, sizeof(LCD_OK_UPLOAD))))
    {
        if((plc_group_new >= PLC_GROUP_1) && (plc_group_new <= PLC_GROUP_9))
        {
            if(g_mem_para.plc_group != plc_group_new)
            {
                g_mem_para.plc_group = plc_group_new;

                mem_para_write();

                STM32_SoftReset();
            }
        }
    }    
    OS_EXIT_CRITICAL();
    
    OSSemPost(g_sem_disp);

    return (TRUE);
}
