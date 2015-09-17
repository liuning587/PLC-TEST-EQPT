#ifndef __MEMORY_H__
#define __MEMORY_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define MEM_PARA_DATE                   0x20150917

#define MEM_PARA_TAG                    0x19890924
#define MAX_PARA_NUM                    16 //16 * 4B = 64B
#define MEM_PARA_SIZE                  (sizeof(MEM_PARA)) //64B

#define PLC_GROUP_DEFAULT               PLC_GROUP_NONE

#define MEM_PARA_PAGE_ADDR             (0x08018C00ul)
#define MEM_PARA_PAGE_SIZE             (1 * 1024) //1KB
#define MEM_PARA_PROGRAM_TIMES         (MEM_PARA_PAGE_SIZE / MEM_PARA_SIZE) //1024 / 64 = 16

typedef struct
{
    unsigned int tag; //存储标签
    unsigned int crc; //校验和
    unsigned int date; //日期

    unsigned int plc_group;

    unsigned int param_reserve[MAX_PARA_NUM - 4];
} MEM_PARA, *P_MEM_PARA;

extern MEM_PARA g_mem_para;

unsigned int mem_param_write(void);
void mem_param_read(void);
void MEM_Init(void);


#ifdef __cplusplus
}
#endif

#endif
