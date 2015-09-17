#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define LCD_DISP_TIME            (20) //ms

#define LCD_COLOR_WHITE           0xffff
#define LCD_COLOR_RED             0xf800

#define LCD_AUTO_UPLOAD_LEN       9

#define MAX_mPLC_STATE_EDIT_NUM   8
#define MAX_ERR_EDIT_NUM          8
#define MAX_OS_INFO_EDIT_NUM      2
#define MAX_PLC_GROUP_EDIT_NUM      2


#define mPLC_STATE_EDIT1_ADDR     0x0000 //������ַ�����֣�21
#define mPLC_STATE_EDIT1_DP       0x0100 //����ָ��

#define mPLC_STATE_EDIT2_ADDR     0x0008 //������ַ�����֣�21
#define mPLC_STATE_EDIT2_DP       0x0120 //����ָ��

#define mPLC_STATE_EDIT3_ADDR     0x0010 //������ַ�����֣�21
#define mPLC_STATE_EDIT3_DP       0x0140 //����ָ��

#define mPLC_STATE_EDIT4_ADDR     0x0018 //������ַ�����֣�21
#define mPLC_STATE_EDIT4_DP       0x0160 //����ָ��

#define mPLC_STATE_EDIT5_ADDR     0x0020 //������ַ�����֣�21
#define mPLC_STATE_EDIT5_DP       0x0180 //����ָ��

#define mPLC_STATE_EDIT6_ADDR     0x0028 //������ַ�����֣�21
#define mPLC_STATE_EDIT6_DP       0x01a0 //����ָ��

#define mPLC_STATE_EDIT7_ADDR     0x0030 //������ַ�����֣�21
#define mPLC_STATE_EDIT7_DP       0x01c0 //����ָ��

#define mPLC_STATE_EDIT8_ADDR     0x0038 //������ַ�����֣�21
#define mPLC_STATE_EDIT8_DP       0x01e0 //����ָ��


#define USG_EDIT_ADDR             0x0040 //������ַ��ASCII��9
#define USG_EDIT_DP               0x0200 //����ָ��

#define VER_EDIT_ADDR             0x0048 //������ַ��ASCII��9
#define VER_EDIT_DP               0x0220 //����ָ��


#define ERR_EDIT1_ADDR            0x0300 //������ַ��ASCII��10
#define ERR_EDIT1_DP              0x0400 //����ָ��

#define ERR_EDIT2_ADDR            0x0308 //������ַ��ASCII��10
#define ERR_EDIT2_DP              0x0420 //����ָ��

#define ERR_EDIT3_ADDR            0x0310 //������ַ��ASCII��10
#define ERR_EDIT3_DP              0x0440 //����ָ��

#define ERR_EDIT4_ADDR            0x0318 //������ַ��ASCII��10
#define ERR_EDIT4_DP              0x0460 //����ָ��

#define ERR_EDIT5_ADDR            0x0320 //������ַ��ASCII��10
#define ERR_EDIT5_DP              0x0480 //����ָ��

#define ERR_EDIT6_ADDR            0x0328 //������ַ��ASCII��10
#define ERR_EDIT6_DP              0x04a0 //����ָ��

#define ERR_EDIT7_ADDR            0x0330 //������ַ��ASCII��10
#define ERR_EDIT7_DP              0x04c0 //����ָ��

#define ERR_EDIT8_ADDR            0x0338 //������ַ��ASCII��10
#define ERR_EDIT8_DP              0x04e0 //����ָ��


#define PLC_GROUP_EDIT1_ADDR      0x0500 //������ַ�����֣�21
#define PLC_GROUP_EDIT1_DP        0x0600 //����ָ��        

#define PLC_GROUP_EDIT2_ADDR      0x0508 //������ַ��ASCII��10
#define PLC_GROUP_EDIT2_DP        0x0620 //����ָ��  


#define EDIT_ADDR_HIGH_INDEX      4
#define EDIT_ADDR_LOW_INDEX       5

#define EDIT_DP_HIGH_INDEX        4
#define EDIT_DP_LOW_INDEX         5

#define EDIT_DATA_INDEX           6

extern const INT16U g_mplc_state_edit_addr[MAX_mPLC_STATE_EDIT_NUM];
extern const INT16U g_mplc_state_edit_dp[MAX_mPLC_STATE_EDIT_NUM];
extern const INT16U g_err_edit_addr[MAX_ERR_EDIT_NUM];
extern const INT16U g_err_edit_dp[MAX_ERR_EDIT_NUM];
extern const INT16U g_os_info_edit_addr[MAX_OS_INFO_EDIT_NUM];
extern const INT16U g_os_info_edit_dp[MAX_OS_INFO_EDIT_NUM];
extern const INT16U g_plc_group_edit_addr[MAX_PLC_GROUP_EDIT_NUM];
extern const INT16U g_plc_group_edit_dp[MAX_PLC_GROUP_EDIT_NUM];
extern OS_EVENT *g_sem_disp;
extern const INT8U g_color_code[8];
extern const INT8U g_font_code[10];
extern INT8U g_color_buf[8];
extern INT8U g_font_buf[10];

INT16U lcd_read_id(void);
INT16U lcd_disp(INT8U *buf, INT8U len);
INT16U LCD_postProcess(pvoid h);


#ifdef __cplusplus
}
#endif

#endif
