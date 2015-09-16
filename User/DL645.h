#ifndef __DL645_H__
#define __DL645_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define DL645_1997	                            0
#define DL645_2007	                            1
#define	DL645_NONE	                            2

#define CCTT_CONTROL_CODE_MASK               0x1F

#define FRM_CTRW_97_READ_SLVS_DATA           0x01 //�����������������
#define FRM_CTRW_97_READ_SLVS_FOLLOW_DATA    0x02 //���������������������
#define FRM_CTRW_97_WRITE_SLVS_DATA          0x04 //������������д����

#define FRM_CTRW_07_READ_SLVS_DATA           0x11 //�����������������
#define FRM_CTRW_07_READ_SLVS_FOLLOW_DATA    0x12 //���������������������
#define FRM_CTRW_07_WRITE_SLVS_DATA          0x14 //������������д����

#define FRM_CTRW_07_BROAD_READ_ADDR          0x13 //�㲥����ַ

#define DL645_97_DATA_ITEM_LEN                  2
#define DL645_07_DATA_ITEM_LEN                  4

#define DL645_ADDR_LEN				            6

#define DL645_REPLY_STAT_MASK                0x40

#define DL645_FRAME_MAX_DATA_LEN              257

#define DL645_FRAME_OK                          0
#define DL645_FRAME_ERROR                     127

typedef struct
{
	u8 Status; //֡״̬: 0�Ƿ���1�Ϸ���2��ַ���ϡ�3��ַ������
	u8 Protocol; //Э��
	u8 Ctrl; //������
	u8 ID_Length; //��ʶ�볤��
} DL645_FRAME_STAT, *P_DL645_FRAME_STAT;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    u8 Start1;
    u8 Addr[6];
    u8 Start2;
    u8 Ctrl;
    u8 Len;
    u8 Data[DL645_FRAME_MAX_DATA_LEN];
} DL645_FRAME, *P_DL645_FRAME;
#pragma pack(pop)

u16 Create_DL645_Frame(u8 *Addr, u8 Ctrl, u8 Len, P_DL645_FRAME P_DL645_Frame);
u16 Create_DL645_Relay_Frame(u8 *Relay_Addr, u8 Level, u8 *Addr, u8 Ctrl, u8 Len, u8 *Data, P_DL645_FRAME P_DL645_Frame);
u16 Analysis_DL645_Frame(u8 *Addr, u8 *Buf, P_DL645_FRAME_STAT P_DL645_Frame_Stat);


#ifdef __cplusplus
}
#endif

#endif
