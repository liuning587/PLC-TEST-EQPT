#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define FLASH_START_ADDR          0x08000000
#define FLASH_PAGE_SIZE           1024
#define PLC_PARA_PAGE             63

#define PLC_PARA_ADDR            (FLASH_START_ADDR + PLC_PARA_PAGE * FLASH_PAGE_SIZE)

#define MAX_MSG_LEN               128

extern INT8U g_msg_buf[MAX_MSG_LEN];
extern INT16U g_msg_len;

INT8U plc_read_para(INT32U *pBuf);
INT8U plc_write_para(INT32U *pBuf);
INT8U get_plc_index(INT8U plc_num);
bool get_mplc_state(void);
int nstrcmp(const char *str1, const char *str2, int num);


#ifdef __cplusplus
}
#endif

#endif
