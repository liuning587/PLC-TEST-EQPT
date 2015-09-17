#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define MAX_MSG_LEN               128

extern INT8U g_msg_buf[MAX_MSG_LEN];
extern INT16U g_msg_len;

bool get_mplc_state(void);
int nstrcmp(const char *str1, const char *str2, int num);


#ifdef __cplusplus
}
#endif

#endif
