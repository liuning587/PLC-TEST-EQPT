#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define KEY_RESET_PORT          GPIOB
#define KEY_MSG_RESET           EXTI_Line8

extern OS_EVENT *g_sem_key;
extern INT32U g_key_msg;

void GUI_X_Init(void);
int GUI_X_GetKey(void);
int GUI_X_WaitKey(void); 
void GUI_X_StoreKey(int k); 
void GUI_X_KeyProc(int k);
    

#ifdef __cplusplus
}
#endif

#endif
