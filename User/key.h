#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define KEY_PORT_NULL           NULL
#define KEY_MSG_NONE           (!(GPIO_Pin_All))

#define KEY_RESET_PORT          GPIOB
#define KEY_MSG_RESET           GPIO_Pin_8

typedef struct {
    GPIO_TypeDef *GPIOx;
    INT16U GPIO_Pin;
} KEY_MSG, *P_KEY_MSG;

typedef struct {
    KEY_MSG msg;
    OS_EVENT *sem;
} KEY_PARA, *P_KEY_PARA;

extern KEY_PARA g_key_para;

void GUI_X_Init(void);
int GUI_X_GetKey(void);
int GUI_X_WaitKey(void); 
void GUI_X_StoreKey(GPIO_TypeDef *GPIOx, INT16U GPIO_Pin); 
void GUI_X_KeyProc(int k);
    

#ifdef __cplusplus
}
#endif

#endif
