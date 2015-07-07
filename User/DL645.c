#include "includes.h"


/********************************************************
*                  ���ͽ���DL645֡
*Input:buff����֡
*Output���Ƿ�֡���ش���100�����ݣ��Ϸ�����ƫ����
*********************************************************/
u8 CheckDL645_Frame(u8 *buff)
{
    u8 len,i,CS,offset;
    
    for(offset=0; offset<12; offset++)
    {
        if(buff[offset]==0x68)
            break; //����֡ͷ68H
    }
    if(offset>10)
        return (DL645_FRAME_ERROR);

    if(buff[7+offset]!=0x68)
        return (DL645_FRAME_ERROR);
    
    len=buff[offset+9];
    if(buff[len+11+offset]!=0x16)
        return (DL645_FRAME_ERROR);
    CS=0;
    len+=10;
    for(i=0; i<len; i++)
        CS+=buff[offset+i];
    if(CS!=buff[len+offset])
        return (DL645_FRAME_ERROR);
    return (offset);
}

/*************************************************************/
void Frame_Sub_33(u8 *buff,u8 len)
{
    u8 i;

    for(i=0; i<len; i++) buff[i]-=0x33;
}

void Frame_Add_33(u8 *buff,u8 len)
{
    u8 i;

    for(i=0; i<len; i++) buff[i]+=0x33;
}

/********************************************************
*                ����645֡
*
*********************************************************/
void CS_Pack(u8 *buff)
{
    u8 len,i,CS;
    
    len=buff[9];
    CS=0;
    for(i=0; i<len; i++) buff[10+i]+=0x33;
    len+=10;
    for(i=0; i<len; i++) CS+=buff[i];
    buff[len]=CS;
    buff[len+1]=0x16;
}

//------------------------------------����DL645����----------------------------------
void Create_DL645_Frame(u8 *Addr, u8 C, u8 len, DL645_Frame_C *DL645_Frame)
{
    DL645_Frame->Start=0x68;
    memcpy(DL645_Frame->Addr,Addr,6);
    DL645_Frame->Start1=0x68;
    DL645_Frame->C=C;
    DL645_Frame->L=len;
    CS_Pack(&DL645_Frame->Start);
}

//--------------------------�м�645��֡-------------------------------------------------------------------
//����֡����
//leve :�м̼���
//Addr :Ŀ���ַ
//
u8 Create_DL645_LeveFrame(u8 *leve_Addr, u8 leve, u8 *Addr, u8 C, u8 len, u8 *data, u8 *Send_buf)
{
	DL645_Frame_C *DL645_Frame;
	u8 Point,i;
	
	DL645_Frame=(DL645_Frame_C *)Send_buf;
	DL645_Frame->C=C;
	Point=0;
    
	if(leve)
	{
		for(i=0;i<(leve);i++)
		{
			memcpy(DL645_Frame->Data+Point,leve_Addr+Point+6,6);
			Point+=6;
			DL645_Frame->C+=0x20;
		}
        
		//memcpy(DL645_Frame->Data+Point,Addr,6); //HRK
		//Point+=6; //HRK
		memcpy(DL645_Frame->Addr,leve_Addr,6);
		//memcpy(DL645_Frame->Data+Point,data,len); /*add on 03.23*/ //HRK
		//Point+=len; //HRK
	}
	else
	{
		memcpy(DL645_Frame->Addr,Addr,6);
	}
    
	memcpy(DL645_Frame->Data+Point,data,len);
	Point+=len;
	DL645_Frame->Start=0x68;
	DL645_Frame->Start1=0x68;
	DL645_Frame->L=Point;
    CS_Pack(&DL645_Frame->Start);
    
	return (Point+12);	
}

u32 Analysis_DL645_Frame(u8 *Addr, u8 *buff, DL645_Frame_Stat_C *pFreame_st)
{
	DL645_Frame_C *DL645_Frame;
	u8 i;
	pFreame_st->protocol=DL645_NONE;
	pFreame_st->ID_Leng=0;
	pFreame_st->Status=0;
    
	i = CheckDL645_Frame(buff);
    
	if(i != DL645_FRAME_ERROR)
	{
		pFreame_st->Status=1; //�Ϸ�645����
		
		DL645_Frame=(DL645_Frame_C *)(buff+i);
        
		if(!memcmp(Addr,DL645_Frame->Addr,6))
		{
			pFreame_st->Status=2; //��ַ��ͬ
		}
        else
        {
			pFreame_st->Status=3; //��ַ����ͬ
        }
        
		Frame_Sub_33(DL645_Frame->Data,DL645_Frame->L);
        
		pFreame_st->C = DL645_Frame->C;
        
		if((DL645_Frame->C&0x10)||(DL645_Frame->C==0x83))
		{
			pFreame_st->protocol=DL645_2007;
			if(DL645_Frame->C==0x91) pFreame_st->ID_Leng=4;
		}
		else
		{
			pFreame_st->protocol=DL645_1997;
			if(DL645_Frame->C==0x81) pFreame_st->ID_Leng=2;
		}     

        return (DL645_FRAME_OK);
	}
    
	return (DL645_FRAME_ERROR); //����
}

