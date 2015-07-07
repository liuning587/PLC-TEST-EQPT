#ifndef __PLC_H__
#define __PLC_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define LED_PLC_TOGGLE()       LED_Toggle(LED_PLC)

#define PLC_TEST_ONCE()        GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define PLC_TEST_RESET()       GPIO_SetBits(GPIOA, GPIO_Pin_15)

#define mPLC_PWR_ON()          GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define mPLC_PWR_OFF()         GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define mPLC_RESET_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define mPLC_RESET_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_14)

#define mPLC_EVENT_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define mPLC_EVENT_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_15)

#define mPLC_SELECT_EN()       GPIO_ResetBits(GPIOA, GPIO_Pin_8)

#define cPLC_READ_ENERGY_LEN   sizeof(cPLC_READ_ENERGY)
#define mPLC_READ_ADDR_LEN     sizeof(mPLC_READ_ADDR)
#define mPLC_REPLY_ADDR_LEN    sizeof(mPLC_REPLY_ADDR)
#define mPLC_REPLY_ENERGY_LEN  sizeof(mPLC_REPLY_ENERGY)

#define mPLC_NUM               4

#define PLC_FREQ_NUM           2

#define mPLC_RESET_NUM         1

#define mPLC_NUM_1             0
#define mPLC_NUM_2             1
#define mPLC_NUM_3             2
#define mPLC_NUM_4             3

#define PLC_FREQ_270KHz        0
#define PLC_FREQ_421KHz        1

#define PLC_FREQ_270KHz_PREAMBLE    0xFC
#define PLC_FREQ_421KHz_PREAMBLE    0xFA

#define MAX_PLC_NUM            8

#define PLC_NUM_1              0x31
#define PLC_NUM_2              0x32
#define PLC_NUM_3              0x33
#define PLC_NUM_4              0x34
#define PLC_NUM_5              0x35
#define PLC_NUM_6              0x36
#define PLC_NUM_7              0x37
#define PLC_NUM_8              0x38

#define PLC_DEFAULT_PREAMBLE   0xFE

#define PLC_BROAD_READ_ADDR    0x13
#define PLC_BROAD_REPLY_ADDR   0X93

#define PLC_READ_DATA          0x11
#define PLC_REPLY_DATA         0x91

#define PLC_DATA_BLOCK_ITEM    0x0001FF00

#define PLC_FREQ_INDEX         0
#define PLC_FREQ_LEN           1

#define PLC_DL645_INDEX       (PLC_FREQ_INDEX + PLC_FREQ_LEN)

#define PLC_NUM_INDEX          4
#define mPLC_NUM_INDEX         5

typedef enum
{
    RECV_RES_NONE = 0,
    RECV_RES_SUCC,
    RECV_RES_INVALID,
    RECV_RES_TIMEOUT
} RECV_RES;

#define mPLC_SELECT(n) {                    \
    if(0 == n)                              \
    {                                       \
        GPIO_ResetBits(GPIOB, GPIO_Pin_12); \
        GPIO_ResetBits(GPIOB, GPIO_Pin_13); \
        LED_On(LED_mPLC_1);                 \
        LED_Off(LED_mPLC_2);                \
        LED_Off(LED_mPLC_3);                \
        LED_Off(LED_mPLC_4);                \
    }                                       \
    else if(1 == n)                         \
    {                                       \
        GPIO_SetBits(GPIOB, GPIO_Pin_12);   \
        GPIO_ResetBits(GPIOB, GPIO_Pin_13); \
        LED_Off(LED_mPLC_1);                \
        LED_On(LED_mPLC_2);                 \
        LED_Off(LED_mPLC_3);                \
        LED_Off(LED_mPLC_4);                \
    }                                       \
    else if(2 == n)                         \
    {                                       \
        GPIO_ResetBits(GPIOB, GPIO_Pin_12); \
        GPIO_SetBits(GPIOB, GPIO_Pin_13);   \
        LED_Off(LED_mPLC_1);                \
        LED_Off(LED_mPLC_2);                \
        LED_On(LED_mPLC_3);                 \
        LED_Off(LED_mPLC_4);                \
    }                                       \
    else if(3 == n)                         \
    {                                       \
        GPIO_SetBits(GPIOB, GPIO_Pin_12);   \
        GPIO_SetBits(GPIOB, GPIO_Pin_13);   \
        LED_Off(LED_mPLC_1);                \
        LED_Off(LED_mPLC_2);                \
        LED_Off(LED_mPLC_3);                \
        LED_On(LED_mPLC_4);                 \
    }                                       \
}

#define FREQ_SELECT(n) {                    \
    if(0 == n)                              \
    {                                       \
        GPIO_ResetBits(GPIOA, GPIO_Pin_12); \
    }                                       \
    else if(1 == n)                         \
    {                                       \
        GPIO_SetBits(GPIOA, GPIO_Pin_12);   \
    }                                       \
}

#define LED_mPLC_ON() {                     \
    LED_On(LED_mPLC_1);                     \
    LED_On(LED_mPLC_2);                     \
    LED_On(LED_mPLC_3);                     \
    LED_On(LED_mPLC_4);                     \
}

#define LED_mPLC_OFF() {                    \
    LED_Off(LED_mPLC_1);                    \
    LED_Off(LED_mPLC_2);                    \
    LED_Off(LED_mPLC_3);                    \
    LED_Off(LED_mPLC_4);                    \
}

extern OS_EVENT *g_sem_plc;
extern INT8U g_cur_freq, g_cur_mplc;
extern INT8U g_plc_num, g_tmp_plc_num;
extern bool g_sta_level_flag;
extern INT8U g_plc_index;
extern bool g_mplc_state[mPLC_NUM][PLC_FREQ_NUM];
extern bool g_sta_level[mPLC_NUM][PLC_FREQ_NUM];

void PLC_Init(void);
INT16U cplc_read_energy(void);
INT16U cPLC_postProcess(pvoid h);
INT16U mPLC_postProcess(pvoid h);


#ifdef __cplusplus
}
#endif

#endif
