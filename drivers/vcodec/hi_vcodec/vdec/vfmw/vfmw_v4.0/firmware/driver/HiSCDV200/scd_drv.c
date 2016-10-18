/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7omB5iYtHEGPqMDW7TpB3q4yvDckVri1GoBL4gKI3k0M86wMC5hMD5fQLRyjt
hU2r3xTDeWuhAOfofKROCEE150YZfDeEJejM53xK+RBEPLvUN0rFoH1R6Rw0Da4E4jfclGDI
O11TkffEXQzBps2DXCRt0H5wZIoBYgMZZQOm/lg9CXbIuo3T+4lRt6c2SIIcHg==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/************************************************************************/
/*                                                                      */
/************************************************************************/

#include "public.h"
#include "scd_drv.h"
#include "vfmw_ctrl.h"
#include "dsp_vfmw_avsp.h"
#include "dsp_vfmw_h264.h"
#ifdef HIVDEC_SMMU_SUPPORT
#include "smmu.h"
#endif

#define SM_DOWN_CLIP_ZERO(a) do{(a) = (((a) < 0)? 0: (a));}while(0)

static SINT32 s_SCDInISR = 0;
static SCDDRV_SLEEP_STAGE_E  s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_NONE;

#if defined(VFMW_AVSPLUS_SUPPORT) || (VFMW_SCD_LOWDLY_SUPPORT)
SINT32 *g_hex_base_vir_addr;
UADDR   g_hex_base_addr;
UINT32  g_hex_len;
#endif

SM_IIS_S  s_SmIIS;   //��̬�����SM��Ϣ��
SM_IIS_S *g_pSmIIS = &s_SmIIS;

extern OSAL_FILE  *vfmw_save_str_file;
extern UINT32      save_str_chan_num;


/*======================================================================*/
/*  ����ʵ��                                                            */
/*======================================================================*/

#ifdef VFMW_SCD_LOWDLY_SUPPORT
//д�����
extern UINT32 g_CabacMN[160][8];
VOID WriteCabacTabDsp(UADDR mn_phy_addr)
{
    UINT32 *p;

    p = (UINT32 *)MEM_Phy2Vir(mn_phy_addr);

    if (NULL == p)
    {
        dprint(PRN_DBG, "line: %d, p = NULL is not expected value!\n", __LINE__);
        return;
    }

    memcpy(p, g_CabacMN, 160 * 8 * sizeof(UINT32));

    return;
}
#endif

#if defined(VFMW_AVSPLUS_SUPPORT) || (VFMW_SCD_LOWDLY_SUPPORT)
VOID WriteHexFile(SINT32 eVidStd)
{
    UINT32 i = 0;
    UINT32 data32 = 0;
    UINT8* pVirAddr = NULL;
    UINT32 u32VfmwBinLen = 0;
    UINT32 ReadyFlag = 0;

    pVirAddr = (UINT8*)MEM_Phy2Vir(g_hex_base_addr);
    if (NULL == pVirAddr)
    {
        dprint(PRN_FATAL,"Fatal: get g_hex_base_addr(0x%x) vir addr failed!\n", g_hex_base_addr);
        return;
    }

    if (VFMW_AVS == eVidStd)
    {
        WR_SCDREG(REG_AVS_FLAG, 0x00000001);
        u32VfmwBinLen = sizeof(g_avsp_firmware);
        memcpy(pVirAddr, (UINT8*)g_avsp_firmware, sizeof(g_avsp_firmware));
        u32VfmwBinLen = u32VfmwBinLen/4;
    }
    else if (VFMW_H264 == eVidStd)
    {
        data32 = RD_SCDREG(REG_SCD_PROTOCOL);
        WR_SCDREG(REG_SCD_PROTOCOL, (data32|(1 << 8)));
        u32VfmwBinLen = sizeof(g_h264_firmware);
        memcpy(pVirAddr, (UINT8*)g_h264_firmware, sizeof(g_h264_firmware));
        u32VfmwBinLen = u32VfmwBinLen/4;
    }
    else
    {
        dprint(PRN_FATAL,"Warn: eVidStd = %d have no DSP vfmw bin file!!\n");
        return;
    }
	
    WR_SCDREG(REG_DSP_PROG_WLEN,     u32VfmwBinLen);
    WR_SCDREG(REG_DSP_PROG_STADDR, g_hex_base_addr);
    
    data32 = 0;
#ifdef HIVDEC_SMMU_SUPPORT
    // mmu enable, secure disable
    data32 = (1<<2) | (0<<1);
#endif

    WR_SCDREG(REG_DSP_PROG_ST, data32);
    WR_SCDREG(REG_DSP_PROG_ST, (data32|1));

    for (i=0; i<100; i++)
    {
        ReadyFlag = (RD_SCDREG(REG_DSP_PROG_OVER)) & 0x1;
        if(1 == ReadyFlag)
        {
            break;
        }
	 else
	 {
	     VFMW_OSAL_mSleep(5);
	 }
    }
    if (i >= 100)
    {
        dprint(PRN_FATAL, "eVidStd %d load dsp code failed!\n", eVidStd);
    }

    return;
}
#endif
    
/*********************************************************************************
  ����ԭ��  SINT32 GetInstIDByRawArray( RAW_ARRAY_S *pRawArray)
  ��������  ����RawArray����Scdͨ��ID
  ����˵��  ��
  ����ֵ    ��
  �㷨����  ��
 *********************************************************************************/
SINT32 GetInstIDByRawArray( RAW_ARRAY_S *pRawArray)
{
    SINT32 i;

    for ( i=0; i<MAX_CHAN_NUM; i++ )
    {
        if (s_SmIIS.pSmInstArray[i] != NULL && pRawArray == &s_SmIIS.pSmInstArray[i]->RawPacketArray )
        {
            return i;
        }
    }

    return -1;
}


/*========================================================================
  part1.    raw stream management module
  ========================================================================*/

/*********************************************************************************
  ����ԭ��
  SINT32 ResetSCD(VOID)
  ��������
  ��λSCDӲ��
  ����˵��
  ��
  ����ֵ
  ��
  �㷨����
  ��
 *********************************************************************************/
SINT32 ResetSCD(VOID)
{
    UINT32 tmp;
    UINT32 i;
    UINT32 reg_rst_ok;
    UINT32 reg;
    UINT32 *pScdResetReg = NULL;
    UINT32 *pScdResetOkReg = NULL;
	
    pScdResetReg   = (UINT32*)MEM_Phy2Vir(gSOFTRST_REQ_Addr);
    pScdResetOkReg = (UINT32*)MEM_Phy2Vir(gSOFTRST_OK_ADDR);

    if( NULL == pScdResetReg || NULL == pScdResetOkReg)
    {
        dprint(PRN_FATAL,"scd reset register map fail!\n");
        return VF_ERR_SYS;
    }

    tmp = RD_SCDREG( REG_SCD_INT_MASK );

    /* require scd reset */
    reg= *(volatile UINT32 *)pScdResetReg;
    *(volatile UINT32 *)pScdResetReg = reg | (UINT32)(1<<SCD_RESET_CTRL_BIT);

    /*wait for rest ok */
    for(i=0; i<100; i++)
    {
        reg_rst_ok =*(volatile UINT32 *)pScdResetOkReg;
        if( reg_rst_ok & (1 <<SCD_RESET_OK_BIT))
        {
            break;
        }	
        VFMW_OSAL_uDelay(10);
    }
    
    if (i >= 100)
    {
        dprint(PRN_FATAL, "%s reset failed!\n", __func__);
    }
    else
    {
        //dprint(PRN_ALWS,  "%s reset success!\n", __func__);
    }

    /* clear reset require */
    *(volatile UINT32 *)pScdResetReg = reg & (UINT32)(~(1<<SCD_RESET_CTRL_BIT));

    WR_SCDREG(REG_SCD_INT_MASK, tmp);
	
    return(VF_OK);
}

/*********************************************************************************
  ����ԭ��
  VOID SCDDRV_ResetSCD(SINT32 ChanID)
  ��������
  ��λΪ��ͨ��������SCDӲ��
  ����˵��
  ChanID ͨ����
  ����ֵ
  ��
  �㷨����
  ��
 *********************************************************************************/
VOID SCDDRV_ResetSCD(SINT32 ChanID)
{
    if (1 == s_SmIIS.SCDState && ChanID == s_SmIIS.ThisInstID)
    {
        ResetSCD();
        s_SmIIS.SCDState = 0;
    }
    
    return;
}

/*����SCD�ж�*/
VOID SCDDRV_MaskInt(VOID)
{
    WR_SCDREG(REG_SCD_INT_MASK, 1);

    return;
}

/*ʹ��SCD�ж�*/
VOID SCDDRV_EnableInt(VOID)
{
#ifndef SCD_BUSY_WAITTING
    WR_SCDREG(REG_SCD_INT_MASK, 0);
#endif

    return;
}

/************************************************************************
  ԭ��    VOID ResetRawStreamArray( RAW_ARRAY_S *pRawStreamArray )
  ����    ��һ��ԭʼ�������ϸ�λ������������м�¼
  ����    pRawStreamArray ָ��ԭʼ��������
  ����ֵ  ��
 ************************************************************************/
VOID ResetRawStreamArray(RAW_ARRAY_S *pRawStreamArray)
{
    FMW_ASSERT( NULL != pRawStreamArray );

    memset( pRawStreamArray, 0, sizeof(RAW_ARRAY_S) );
    return;
}

/************************************************************************
  ԭ��    SINT32 InsertRawPacket( RAW_ARRAY_S *pRawStreamArray, RAW_PACKET_S *pRawPacket )
  ����    ��һ��ԭʼ���������뵽ָ������������
  ����    pRawStreamArray ָ��ԭʼ��������
  pRawPacket  ָ��ԭʼ������
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 InsertRawPacket( RAW_ARRAY_S *pRawStreamArray, STREAM_DATA_S *pRawPacket )
{
    SINT32 Index;

    /* ���Ե�GetRawState */
    FMW_ASSERT_RET( (0 != pRawStreamArray) && (0 != pRawPacket), FMW_ERR_PARAM );

    //# ��������ֹ����. �����ʩ��������

    /* �ж��Ƿ����� */
    if ( ((pRawStreamArray->Tail+1) % MAX_STREAM_RAW_NUM) == pRawStreamArray->History )
    {
        return FMW_ERR_BUSY;
    }

    /* ����� */
    Index = pRawStreamArray->Tail;
    memcpy( &(pRawStreamArray->RawPacket[Index]), pRawPacket, sizeof(STREAM_DATA_S) );
    pRawStreamArray->Tail = (pRawStreamArray->Tail+1) % MAX_STREAM_RAW_NUM;

    //# �������

    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 GetRawIsFull(SINT32 SmID)
  ����    �õ�Raw Buffer�ܷ�����״̬
  ����    SmID    ��������ģ���ʵ���ı�ʶ
  ����ֵ  ���Բ���Raw Packet�ͷ���FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 GetRawState(SINT32 SmID)
{

    SINT32 Ret;
    RAW_ARRAY_S *pRawStreamArray;

    Ret = FMW_ERR_BUSY;
    FMW_ASSERT_RET((SmID >= 0) && (SmID < SM_MAX_SMID),Ret);
    FMW_ASSERT_RET(s_SmIIS.pSmInstArray[SmID]!=NULL,Ret);

    pRawStreamArray = &s_SmIIS.pSmInstArray[SmID]->RawPacketArray;    

    if( ((pRawStreamArray->Tail+1) % MAX_STREAM_RAW_NUM) == pRawStreamArray->History )
    {
        return Ret;
    }

    return FMW_OK;

}

/************************************************************************
  ԭ��    SINT32 ShowFirstRawPacket( RAW_ARRAY_S *pRawStreamArray, RAW_PACKET_S *pRawPacket )
  ����    �鿴ָ�������������еĵ�һ����������Ϣ��
  ����    pRawStreamArray ָ��ԭʼ��������
  pRawPacket  ָ��ԭʼ�������Ľṹ�����ڽ�����������Ϣ
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 ShowFirstRawPacket( RAW_ARRAY_S *pRawStreamArray, STREAM_DATA_S *pRawPacket )
{
    SINT32 Index;
    FMW_ASSERT_RET( (0 != pRawStreamArray) && (0 != pRawPacket), FMW_ERR_PARAM );

    if ( pRawStreamArray->Head == pRawStreamArray->Tail )
    {
        return FMW_ERR_NOTRDY;
    }
    else
    {
        Index = pRawStreamArray->Head;
        pRawStreamArray->CurShowIndex = Index;
        memcpy( pRawPacket, &pRawStreamArray->RawPacket[Index], sizeof(STREAM_DATA_S) );
    }

    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 ShowNextRawPacket( RAW_ARRAY_S *pRawStreamArray, RAW_PACKET_S *pRawPacket )
  ����    "Next"���������һ�ε���Show���������Եġ���������ShowFirstRawPacket()��Ͽɲ鿴��������������Ϣ��
  ����    pRawStreamArray ָ��ԭʼ��������
  pRawPacket  ָ��ԭʼ�������Ľṹ�����ڽ�����������Ϣ
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 ShowNextRawPacket( RAW_ARRAY_S *pRawStreamArray, STREAM_DATA_S *pRawPacket )
{
    SINT32 Index;
    FMW_ASSERT_RET( (0 != pRawStreamArray) && (0 != pRawPacket), FMW_ERR_PARAM );

    /* �ж��Ƿ��п�show�������� */
    Index = (pRawStreamArray->CurShowIndex + 1) % MAX_STREAM_RAW_NUM;
    if( Index == pRawStreamArray->Tail )
    {
        return FMW_ERR_NOTRDY;
    }
    else
    {
        pRawStreamArray->CurShowIndex = Index;
        memcpy( pRawPacket, &pRawStreamArray->RawPacket[Index], sizeof(STREAM_DATA_S) );
    }

    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 SetFirstOffset( RAW_ARRAY_S *pRawStreamArray, SINT32 Offset )
  ����    ���õ�һ������������Ч�ֽ�ƫ��
  ����    pRawStreamArray ָ��ԭʼ��������
  Offset  ��һ�����ݰ�����Ч�ֽ�ƫ����
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 SetFirstOffset( RAW_ARRAY_S *pRawStreamArray, SINT32 Offset )
{
    SINT32 Index;
    SINT32 TargetOffset;
    FMW_ASSERT_RET( 0 != pRawStreamArray, FMW_ERR_PARAM );

    /* ���ԭʼ�������գ�����ʧ�� */
    if( pRawStreamArray->Head == pRawStreamArray->Tail )
    {
        return FMW_ERR_NOTRDY;
    }

    TargetOffset = pRawStreamArray->FirstPacketOffset + Offset;
    Index = pRawStreamArray->Head;
    if( TargetOffset >= pRawStreamArray->RawPacket[Index].Length )  
    {
        return FMW_ERR_PARAM;
    }

    pRawStreamArray->FirstPacketOffset += Offset;

    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 GetFirstOffset( RAW_ARRAY_S *pRawStreamArray, SINT32 *pOffset )
  ����    ��ȡ��һ������������Ч�ֽ�ƫ��
  ����    pRawStreamArray ָ��ԭʼ��������
  pOffset ��һ�����ݰ�����Ч�ֽ�ƫ����
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 GetFirstOffset( RAW_ARRAY_S *pRawStreamArray, SINT32 *pOffset )
{
    FMW_ASSERT_RET( (0 != pRawStreamArray) && (0 != pOffset), FMW_ERR_PARAM );

    /* ���ԭʼ�������գ�����ʧ�� */
    if( pRawStreamArray->Head == pRawStreamArray->Tail )
    {
        return FMW_ERR_NOTRDY;
    }

    *pOffset = pRawStreamArray->FirstPacketOffset;

    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 DeleteRawPacket( RAW_ARRAY_S *pRawStreamArray, UINT32 DelNum )
  ����    �ӵ�һ��������ʼ������ɾ��DelNum����������
  ����������Ƭ�λ������Ĵ��ڣ�ԭʼ�������������ͷţ�����ԭʼ����
  ����ģ�����ֻ֧�ְ�����˳���ͷš���������ԭʼ��������ɾ����ͬʱ��
  Ҳ�����MPP�Ľӿ�֪ͨ�ⲿ�齨�ͷŶ�Ӧ�������ռ䡣
  ����    pRawStreamArray ָ��ԭʼ��������
  pRawPacket  ָ��ԭʼ�������Ľṹ�����ڽ�����������Ϣ
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 DeleteRawPacket( RAW_ARRAY_S *pRawStreamArray, UINT32 DelNum )
{
    UINT32 PacketNum = 0;
    UINT32 i = 0, j = 0;
    SINT32 SmID = 0;

    FMW_ASSERT_RET( (0 != pRawStreamArray) && (0 != DelNum), FMW_ERR_PARAM );
    SmID = GetInstIDByRawArray( pRawStreamArray );

    if( SmID < 0 )
    {
        dprint(PRN_FATAL,"line: %d Get SmID fail!", __LINE__);
        return FMW_ERR_PARAM;
    }

    /* ԭʼ���������ж��ٰ���*/
    PacketNum = (pRawStreamArray->Head <= pRawStreamArray->Tail)?
        (pRawStreamArray->Tail - pRawStreamArray->Head):
        (pRawStreamArray->Tail + MAX_STREAM_RAW_NUM - pRawStreamArray->Head);

    if( DelNum > PacketNum )
    {
        dprint(PRN_SCD_INFO, "DelRaw err PacketNum=%d,DelNum=%d\n",PacketNum,DelNum);
        return FMW_ERR_PARAM;
    }
    else
    {
        j = pRawStreamArray->Head;
        for( i=0; i < DelNum; i++ )
        {
            dprint(PRN_SCD_INFO, "Release RawPhyAddr=0x%x,Len=%d\n",pRawStreamArray->RawPacket[j].PhyAddr,pRawStreamArray->RawPacket[j].Length);
            //ReleaseRawStreamData(SmID, &pRawStreamArray->RawPacket[j]);
            s_SmIIS.pSmInstArray[SmID]->RawPacketArray.RawTotalSize -= pRawStreamArray->RawPacket[j].Length;
            SM_DOWN_CLIP_ZERO(s_SmIIS.pSmInstArray[SmID]->RawPacketArray.RawTotalSize);
            j = (j+1) % MAX_STREAM_RAW_NUM;
        }

        pRawStreamArray->Head += DelNum;
        if(pRawStreamArray->Head >= MAX_STREAM_RAW_NUM)
        {
            pRawStreamArray->Head -= MAX_STREAM_RAW_NUM;
        }
        if( DelNum > 0 )
        {
            pRawStreamArray->FirstPacketOffset = 0;
        }
    }

    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 DeleteRawPacketInBuffer(SINT32 SmID, SINT32 ResetFlag)
  ����    ��history��ʼ������ɾ����head
  ����    SmID ͨ����
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 DeleteRawPacketInBuffer(SINT32 SmID, SINT32 ResetFlag)
{
    UINT32 i, j;
    SINT32 RawHead;
    SINT32 DelNum;
    RAW_ARRAY_S *pRawStreamArray;
    SM_INSTANCE_S *pSmInstArray = NULL;

    if ((SmID < 0) || (SmID > (SM_MAX_SMID - 1)))
    {
        dprint(PRN_FATAL, "SmID = %d is not expected!\n", SmID);
        return FMW_ERR_PARAM;
    }

    pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    if (pSmInstArray == NULL)
    {
        return FMW_ERR_PARAM;
    }

    if (ResetFlag == 0)
    {
        FMW_ASSERT_RET( (SM_INST_MODE_IDLE != pSmInstArray->Mode), FMW_ERR_PARAM );
    }
    pRawStreamArray = &(pSmInstArray->RawPacketArray);
    FMW_ASSERT_RET( (0 != pRawStreamArray), FMW_ERR_PARAM );

    RawHead = pRawStreamArray->Head;
    /* ������Ҫ�ͷŵ�ԭʼ�����ж��ٰ� */
    DelNum = (pRawStreamArray->History <= RawHead)?
        (RawHead - pRawStreamArray->History):
        (RawHead + MAX_STREAM_RAW_NUM - pRawStreamArray->History);

    if(DelNum == 0)
    {
        return FMW_ERR_PARAM;
    }
    else
    {
        j = pRawStreamArray->History;
        for( i =0; i < DelNum; i++ )
        {
            dprint(PRN_SCD_INFO, "Release RawPhyAddr=0x%x,Len=%d\n",pRawStreamArray->RawPacket[j].PhyAddr,pRawStreamArray->RawPacket[j].Length);
            ReleaseRawStreamData(SmID, &pRawStreamArray->RawPacket[j]);
            j = (j+1) % MAX_STREAM_RAW_NUM;
        }

        pRawStreamArray->History += DelNum;
        if(pRawStreamArray->History >= MAX_STREAM_RAW_NUM)
        {
            pRawStreamArray->History -= MAX_STREAM_RAW_NUM;
        }
    }

    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 GetRawNumOffsert( RAW_ARRAY_S *pRawStreamArray, SINT32 Len, SINT32 *pNum, SINT32 Offset )
  ����    �ӳ���Len����ӵ�һ��������ʼ��������Ҫ�ж�����������Ĵ�С����Len������а���򳤶�����ΪOffset�� 
  ����    pRawStreamArray ָ��ԭʼ��������
  Len             ����
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 GetRawNumOffsert( RAW_ARRAY_S *pRawStreamArray, SINT32 Len, SINT32 *pNum, SINT32 *pOffset, UINT64 *pPts, UINT64 *pUsertag, UINT64 *pDisptime, UINT32 *pDispEnableFlag, UINT32 *pDispFrameDistance, UINT32 *pDistanceBeforeFirstFrame, UINT32 *pGopNum, UINT64 *pRawPts)
{
    SINT32 i, Index;
    SINT32 TotalLen;
    SINT32 RawNum = 0;

    FMW_ASSERT_RET( (0 != pRawStreamArray) && (0 <= Len), FMW_ERR_PARAM );
    FMW_ASSERT_RET( (0 != pNum) && (0 != pOffset), FMW_ERR_PARAM );

    TotalLen = pRawStreamArray->RawPacket[pRawStreamArray->Head].Length - pRawStreamArray->FirstPacketOffset;

    if(FMW_OK !=  GetRawStreamNum(pRawStreamArray, &RawNum))
    {
        dprint(PRN_DBG, "%s %d FMW_OK !=  GetRawStreamNum!!\n",__FUNCTION__,__LINE__);
    }
    RawNum = RawNum + pRawStreamArray->Head;

    Index = pRawStreamArray->Head;
    *pPts = pRawStreamArray->RawPacket[Index].Pts;
    *pUsertag = pRawStreamArray->RawPacket[Index].UserTag;
    *pRawPts = pRawStreamArray->RawPacket[Index].Pts;
    *pDisptime = pRawStreamArray->RawPacket[Index].DispTime;
    *pDispEnableFlag = pRawStreamArray->RawPacket[Index].DispEnableFlag;
    *pDispFrameDistance = pRawStreamArray->RawPacket[Index].DispFrameDistance;
    *pDistanceBeforeFirstFrame = pRawStreamArray->RawPacket[Index].DistanceBeforeFirstFrame;
    *pGopNum = pRawStreamArray->RawPacket[Index].GopNum;

    for( i = pRawStreamArray->Head; i < RawNum;)
    {
        if( Len < TotalLen )
        {            
            break;
        }        
        i++;
        Index = i % MAX_STREAM_RAW_NUM;
        TotalLen += pRawStreamArray->RawPacket[Index].Length;
        *pPts = (pRawStreamArray->RawPacket[Index].Pts == -1)? (*pPts) : pRawStreamArray->RawPacket[Index].Pts;
        *pUsertag = pRawStreamArray->RawPacket[Index].UserTag;
        *pRawPts = pRawStreamArray->RawPacket[Index].Pts;
        *pDisptime = pRawStreamArray->RawPacket[Index].DispTime;
        *pDispEnableFlag = pRawStreamArray->RawPacket[Index].DispEnableFlag;
        *pDispFrameDistance = pRawStreamArray->RawPacket[Index].DispFrameDistance;
        *pDistanceBeforeFirstFrame = pRawStreamArray->RawPacket[Index].DistanceBeforeFirstFrame;
        *pGopNum = pRawStreamArray->RawPacket[Index].GopNum;

    }

    if (pRawStreamArray->Head == i)
    {
        *pNum = 0;
        *pOffset = Len;
        return FMW_OK;
    }
    else
    {
        TotalLen = TotalLen - pRawStreamArray->RawPacket[Index].Length;
        *pNum = i - pRawStreamArray->Head;
        *pOffset = Len - TotalLen;
        return FMW_OK;
    }
}

/************************************************************************
  ԭ��    SINT32 DeleteRawLen( RAW_ARRAY_S *pRawStreamArray, SINT32 DelLen )
  ����    �ӵ�һ��������ʼ������ɾ��DelLen�����������ɾ���ĳ��Ȳ�������Ҫ��ƫ�ơ�            
  ����    pRawStreamArray ָ��ԭʼ��������
  DelLen  �ܹ���Ҫɾ���ĳ���
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 DeleteRawLen( RAW_ARRAY_S *pRawStreamArray, SINT32 DelLen )
{
    SINT32 TotallLen;
    SINT32 Ret;
    SINT32 DelNum;
    SINT32 RawPacketOffSet;
    UINT64 Pts;
    UINT64 Usertag;
    UINT64 Disptime;
    UINT64 RawPts;
    UINT32 DispEnableFlag;
    UINT32 DispFrameDistance;
    UINT32 DistanceBeforeFirstFrame;
    UINT32 GopNum;

    FMW_ASSERT_RET( (0 != pRawStreamArray) && (0 <= DelLen), FMW_ERR_PARAM );

    DelNum = 0;
    RawPacketOffSet = 0;

    /*����ɾ������Ϊ0������*/
    if (0 == DelLen)
    {
        //return FMW_OK;  //lenth == 0ʱ�������ᵼ��ʵ���������û�б�del��������VP6 NVOP(��ʱ�ð���������Ϊ0)ʱ����
    }
    RawPacketOffSet = 0;

    /* ԭʼ���������ж��ٰ���*/
    Ret = GetRawStreamSize( pRawStreamArray, &TotallLen);
    if (FMW_OK == Ret)
    {
        if( DelLen > TotallLen )
        {
            return FMW_ERR_PARAM;
        }
        else
        {
            /*�õ�Raw���ĸ���������ƫ��*/
            GetRawNumOffsert(pRawStreamArray, DelLen, &DelNum, &RawPacketOffSet, &Pts, &Usertag, &Disptime, &DispEnableFlag, &DispFrameDistance, &DistanceBeforeFirstFrame, &GopNum, &RawPts);
            /*ɾ��EatenRawNum������,��Ϊǰ���а���������,����EatenRawNum����Խ��,
              ��EatenRawNum��Ϊ��,��������Բ��ù�,��Ϊ����ɾ����*/
            DeleteRawPacket(pRawStreamArray, DelNum);
            Ret = SetFirstOffset(pRawStreamArray, RawPacketOffSet);
            if (FMW_OK != Ret)
            { 
                return Ret;
            }
        } 
    }
    else
    {
        return Ret;
    }

    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 DeleteLastSendRaw( UINT32 SmID  )
  ����    ɾ��SmID��ָ���ͨ�����ϴ�������SCD������
  ����    SmIDͨ����
  ����ֵ  ��
 ************************************************************************/
VOID DeleteLastSendRaw( UINT32 SmID )
{
    SINT32 LastPushRawNum;
    SM_INSTANCE_S *pSmInstArray = NULL;

    SM_CHECK_VALUE((SINT32)SmID, 0, SM_MAX_SMID-1, return);

    pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    if (pSmInstArray == NULL)
    {
        dprint(PRN_FATAL, "line: %d pSmInstArray is NULL!\n", __LINE__);
        return;
    }

    LastPushRawNum = pSmInstArray->SmPushRaw.PushRawNum;

    if (LastPushRawNum > 0)
    {
        DeleteRawPacket(&(pSmInstArray->RawPacketArray), (UINT32)LastPushRawNum);
        pSmInstArray->SmPushRaw.PushRawNum = 0;
    }

    return;

}



/************************************************************************
  ԭ��    SINT32 GetRawStreamSize( RAW_ARRAY_S *pRawStreamArray, SINT32 *pStreamSize)
  ����    ��ԭʼ�������������а��ĳ����ۼ�������Ϊ����������Ŀ���ǻ��Ŀǰ������ռ�������
  ����    pRawStreamArray ָ��ԭʼ��������
  pRawPacket  ָ��ԭʼ�������Ľṹ�����ڽ�����������Ϣ
  ����ֵ  �ɹ������������ȣ����򷵻ش����루��������
 ************************************************************************/
SINT32 GetRawStreamSize( RAW_ARRAY_S *pRawStreamArray, SINT32 *pStreamSize)
{

    FMW_ASSERT_RET( 0 != pRawStreamArray, FMW_ERR_PARAM );

    *pStreamSize  = pRawStreamArray->RawTotalSize;
    *pStreamSize -= pRawStreamArray->FirstPacketOffset;
    FMW_ASSERT_RET( (*pStreamSize) >= 0, FMW_ERR_PARAM );
    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 GetRawStreamNum( RAW_ARRAY_S *pRawStreamArray, SINT32 *pStreamNum )
  ����    ��ԭʼ�����������ѱ����͵���δ���и������������Ŀ���ǻ��Ŀǰ������ռ�������
  ����    pRawStreamArray ָ��ԭʼ��������
  ����ֵ  �ɹ�����pStreamNum����������������FMW_OK,ʧ���򷵻ش����루��������
 ************************************************************************/
SINT32 GetRawStreamNum(RAW_ARRAY_S *pRawStreamArray, SINT32 *pStreamNum)
{
    FMW_ASSERT_RET( 0 != pRawStreamArray, FMW_ERR_PARAM );
    
    *pStreamNum = pRawStreamArray->Tail - pRawStreamArray->Head;

    if (*pStreamNum < 0)
    {
        *pStreamNum += MAX_STREAM_RAW_NUM;
    }

    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 GetRawStreamNumInBuffer( RAW_ARRAY_S *pRawStreamArray, SINT32 *pStreamNum )
  ����    ��ԭʼ�����������ѱ����͵���δ���и���������������иδ����buffer���ͷŵ�����������
  Ŀ���ǻ��Ŀǰ������ռ�������
  ����    pRawStreamArray ָ��ԭʼ��������
  ����ֵ  �ɹ�����pStreamNum����������������FMW_OK,ʧ���򷵻ش����루��������
 ************************************************************************/
SINT32 GetRawStreamNumInBuffer(RAW_ARRAY_S *pRawStreamArray, SINT32 *pStreamNum)
{
    FMW_ASSERT_RET( 0 != pRawStreamArray, FMW_ERR_PARAM );

    *pStreamNum = pRawStreamArray->Tail - pRawStreamArray->History;

    if (*pStreamNum < 0)
    {
        *pStreamNum += MAX_STREAM_RAW_NUM;
    }

    return FMW_OK;
}



/*========================================================================
  part2.    stream segment management module
  ========================================================================*/
/************************************************************************
  ԭ��    SINT32 ConfigStreamSegArray( SEG_ARRAY_S *pStreamSegArray, UADDR  BufPhyAddr, UINT8 *pBufVirAddr, UINT32 BufSize )
  ����    Ϊ����Ƭ�μ������ñ�Ҫ����Ϣ��������Ƭ�λ��������׵�ַ�����ȵȡ�
  ����    pStreamSegArray ָ���и�����Ƭ�μ���
  BufPhyAddr  ����Ƭ�λ����������ַ
  BufSize ����Ƭ�λ�������С����λ���ֽ�
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 ConfigStreamSegArray( SEG_ARRAY_S *pStreamSegArray, UADDR  BufPhyAddr, UINT8 *pBufVirAddr, UINT32 BufSize )
{
    FMW_ASSERT_RET( (NULL!=pStreamSegArray && 0!=BufPhyAddr && NULL!=pBufVirAddr &&
                BufSize>=SM_MIN_SEG_BUF_SIZE), FMW_ERR_PARAM );

    memset( pStreamSegArray, 0, sizeof(SEG_ARRAY_S) );

    pStreamSegArray->SegBufPhyAddr   = BufPhyAddr;
    pStreamSegArray->pSegBufVirAddr  = pBufVirAddr;
    pStreamSegArray->SegBufReadAddr  = BufPhyAddr;
    pStreamSegArray->SegBufWriteAddr = BufPhyAddr;
    pStreamSegArray->SegBufSize      = BufSize;

    return FMW_OK;
}

/************************************************************************
  ԭ��    VOID ResetStreamSegArray( SEG_ARRAY_S *pStreamSegArray )
  ����    ��һ���и�����Ƭ�μ��ϸ�λ������������м�¼���ͷ�ȫ���и���������ռ䡣
  ����    pStreamSegArray ָ���и�����Ƭ�μ���
  ����ֵ  ��
 ************************************************************************/
VOID ResetStreamSegArray( SEG_ARRAY_S *pStreamSegArray )
{
    FMW_ASSERT( NULL!=pStreamSegArray );

    memset( pStreamSegArray->StreamSeg, 0, sizeof(pStreamSegArray->StreamSeg) );

    pStreamSegArray->Head            = 0;
    pStreamSegArray->Tail            = 0;
    pStreamSegArray->History         = 0;	
    pStreamSegArray->SegBufReadAddr  = pStreamSegArray->SegBufPhyAddr;
    pStreamSegArray->SegBufWriteAddr = pStreamSegArray->SegBufPhyAddr;

    return;
}

/************************************************************************
  ԭ��    VOID GetSegBuf( SEG_ARRAY_S *pStreamSegArray, UINT32 *pBufAddr, UINT32 *pBufLength )
  ����    ��ȡ����Ƭ�λ������ĵ�ַ�ͳ��ȡ�
  SCD�и�����ʱ��Ҫ���û������Ķ��׵�ַ��
  ����    pStreamSegArray ָ���и�����Ƭ�μ���
  pAddr   ��������Ƭ�λ������׵�ַ�������ַ��������ָ��
  pLength ��������Ƭ�λ��������ȱ�����ָ��
  ����ֵ  ��
 ************************************************************************/
VOID GetSegBuf( SEG_ARRAY_S* pStreamSegArray, UADDR* pBufAddr, UINT32* pBufLength )
{
    FMW_ASSERT( NULL != pStreamSegArray && NULL != pBufAddr && NULL != pBufLength );

// *pBufAddr   = ((pStreamSegArray->SegBufPhyAddr + 0x3) & (~0x3));
    *pBufAddr   = pStreamSegArray->SegBufPhyAddr;
    *pBufLength = pStreamSegArray->SegBufSize;

    return;
}

/************************************************************************
  ԭ��    VOID GetFreeSegBuf( SEG_ARRAY_S *pStreamSegArray, UINT32 *pAddr, UINT32 *pLength )
  ����    ��ȡ���е�����Ƭ�λ���ռ䣬���н���д��ַ�Ͷ���ַֻ���Ŀռ䶼����Ϊ���пռ䡣
  ��������Ƭ�λ�������һ��ѭ��buffer�����Կ���������ܾ��ƣ��Ӷ�ʹ�õ�ַ��������
  �жϵ�ַ�Ƿ���Ƶ�������pAddr+pLength>buffer�ײ���
  ����    pStreamSegArray ָ���и�����Ƭ�μ���
  pAddr   ���տ��пռ��׵�ַ�������ַ��������ָ��
  pLength ���տ��пռ䳤�ȱ�����ָ��
  ����ֵ  ��
 ************************************************************************/
VOID GetFreeSegBuf( SEG_ARRAY_S *pStreamSegArray, SINT32 *pAddr, SINT32 *pLength )
{
    SINT32 Len;

    FMW_ASSERT( NULL != pStreamSegArray && NULL != pAddr && NULL != pLength );

    *pAddr   = pStreamSegArray->SegBufWriteAddr;
    *pLength = 0;

    if (pStreamSegArray->SegBufWriteAddr > pStreamSegArray->SegBufReadAddr)
    {
        Len = pStreamSegArray->SegBufSize - 1 - 
            (pStreamSegArray->SegBufWriteAddr - pStreamSegArray->SegBufReadAddr);
    }
    else if(pStreamSegArray->SegBufWriteAddr < pStreamSegArray->SegBufReadAddr)
    {
        Len = pStreamSegArray->SegBufReadAddr - pStreamSegArray->SegBufWriteAddr - 1;
    }
    else
    {
        Len = pStreamSegArray->SegBufSize - 1;
    }

    FMW_ASSERT(Len <= pStreamSegArray->SegBufSize);

    *pLength = Len;

    return;
}

/************************************************************************
  ԭ��    SINT32 InsertStreamSeg( SEG_ARRAY_S *pStreamSegArray, STREAM_SEG_S *pStreamSeg )
  ����    ���ض�������Ƭ�μ��в���һ������Ƭ�Ρ�
  ����������������������һ�ǽ�����������Ƭ�ε�������Ϣ���뵽�����У��ڶ��Ǹ�������Ƭ�λ�������д��ַ��
  ����    pStreamSegArray ָ���и�����Ƭ�μ���
  pStreamSeg  ����������Ƭ�ε�������Ϣ
  ����ֵ  FMW_OK --- �ɹ�
  FMW_ERR_PARAM  --- ��������
  FMW_ERR_BUSY   --- ��Ϣ���������ܲ���
 ************************************************************************/
SINT32 InsertStreamSeg(SEG_ARRAY_S *pStreamSegArray, STREAM_SEG_S *pStreamSeg)
{
    STREAM_SEG_S *pTail;
    SINT32  Ret = FMW_OK;

    FMW_ASSERT_RET( NULL!=pStreamSegArray && NULL!=pStreamSeg , FMW_ERR_PARAM );

    /* ����������Ƭ�εĵ�ַ�ͳ��Ȳ���飬Ĭ���ɱ������ĵ����߱�֤������� */

    if( (pStreamSegArray->Tail+1) % MAX_STREAM_SEG_NUM != pStreamSegArray->Head )  /* ����Ƭ��Ϣ�鹻��ֱ�Ӳ��� */
    {        
        pTail = &pStreamSegArray->StreamSeg[pStreamSegArray->Tail];
        memcpy( pTail, pStreamSeg, sizeof(STREAM_SEG_S) );
        SMSEG_SET_ISFRESH(pStreamSegArray->StreamSeg[pStreamSegArray->Tail]);
        pTail->StreamID = pStreamSegArray->Tail;

        pStreamSegArray->Tail = (pStreamSegArray->Tail+1) % MAX_STREAM_SEG_NUM;
        pStreamSegArray->SegBufWriteAddr = pStreamSeg->PhyAddr + pStreamSeg->LenInByte;
        pStreamSegArray->SegTotalSize += pStreamSeg->LenInByte;
        pStreamSegArray->SegFreshSize += pStreamSeg->LenInByte;
        pStreamSegArray->SegFreshNum++;
    }
    else  /* ����Ƭ��Ϣ�������������ǿ�Ʋ����򷵻ش����룬����ǿ���ͷ�һ���ٲ��� */
    {
        Ret = FMW_ERR_BUSY;
    }

    return Ret;
}

/************************************************************************
  ԭ��    SINT32 GetStreamSeg( SEG_ARRAY_S *pStreamSegArray, STREAM_SEG_S *pStreamSeg )
  ����    ���ض�������Ƭ�μ���ȡ��һ������Ƭ�Σ������ڶ���ͷ������Ƭ����Ϣȡ����
  ����    pStreamSegArray ָ����Ƭ�μ���
  pStreamSeg  �洢����Ƭ�ε�������Ϣ�Ľṹָ��
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 GetStreamSeg( SEG_ARRAY_S *pStreamSegArray, STREAM_SEG_S *pStreamSeg )
{
    SINT32 i, Ret = FMW_ERR_NOTRDY;
    STREAM_SEG_S *pSeg;

    FMW_ASSERT_RET( NULL!=pStreamSegArray && NULL!=pStreamSeg , FMW_ERR_PARAM );

    pSeg = NULL;
    for(i=pStreamSegArray->Head;;)
    {
        if(i==pStreamSegArray->Tail) /*��ͷ�ˣ�������Ƭ�ο�ȡ*/
        {
            break;
        }
        else if( SMSEG_IS_FRESH(pStreamSegArray->StreamSeg[i]) ) /* �ҵ�һ����û�з��ʹ��ģ������� */
        {
            pSeg = &pStreamSegArray->StreamSeg[i];
            break;
        }
        else    /* ����Ѿ���ȡ���ˣ�����һ�� */
        {
            i = (i+1) % MAX_STREAM_SEG_NUM;
        }
    }

    if(NULL != pSeg)
    {
        memcpy(pStreamSeg, pSeg, sizeof(STREAM_SEG_S));
        SMSEG_SET_ISREAD(*pSeg);    /* �����������ѱ����� */

        pStreamSegArray->SegFreshSize -= pSeg->LenInByte;
        pStreamSegArray->SegFreshNum--;
        SM_DOWN_CLIP_ZERO(pStreamSegArray->SegFreshSize);
        SM_DOWN_CLIP_ZERO(pStreamSegArray->SegFreshNum);

        dprint(PRN_SCD_INFO, "Read_seg SegPacket.PhyAddr=0x%x, LenByte=%d, IsLastSeg=%d\n", pSeg->PhyAddr, pSeg->LenInByte, pSeg->IsLastSeg);
        Ret = FMW_OK;
    }
    else
    {
        Ret = FMW_ERR_NOTRDY;
    }

    return Ret;
}

/************************************************************************
  ԭ��    SINT32 ReleaseStreamSeg( SEG_ARRAY_S *pStreamSegArray, UINT32 StreamID)
  ����    ��IDΪStreamID������Ƭ���ͷţ�����������Ƭ������ʷ���Ѷ�ȡ��δ�ͷŵĵ�һ����������ͬ����������Ƭ�λ������Ķ���ַ��
  ����    pStreamSegArray ָ����Ƭ�μ���
  StreamID    ���ͷ�����Ƭ�ε�ID
  ���StreamIDȡֵ��0~ MAX_STREAM_SEG_NUM-1֮�����ʾҪ�ͷ�������ΪStreamID������Ƭ�Σ�
  ���StreamID = 0xffffffff�����ʾ�ͷŵ�һ������Ƭ�Ρ����������ʹ�ó����ǣ��������
  ����ģ�鳤ʱ�䲻�ͷ��������������ݴ���SMģ����Ҫǿ���ͷŵ�һ�������ڳ��ռ��������и���
  ����������������������£����۵�һ������Ƭ���Ƿ񱻶��ߣ���ǿ���ͷš�
  ����ֵ  �ɹ�����FMW_OK�����򷵻ش�����
 ************************************************************************/
SINT32 ReleaseStreamSeg(SEG_ARRAY_S *pStreamSegArray, UINT32 StreamID)
{
    SINT32 Ret = FMW_ERR_NOTRDY;
    UINT32 i = 0;
    STREAM_SEG_S *pSeg = NULL;

    FMW_ASSERT_RET( (NULL!=pStreamSegArray) &&
            ((StreamID<MAX_STREAM_SEG_NUM) || (StreamID==0xffffffff)), FMW_ERR_PARAM  );

    if( 0xffffffff == StreamID )  /* Ҫǿ���ͷŶ�ͷ�� */
    {
        if( pStreamSegArray->Head != pStreamSegArray->Tail ) /* ֻҪ���в��������ͷ� */
        {
            pSeg     = &pStreamSegArray->StreamSeg[pStreamSegArray->Head];
            StreamID = pStreamSegArray->Head;
            dprint(PRN_FATAL, "-------- forece release seg[%d] -----------\n", StreamID);
            for (i = 0; i < pSeg->LenInByte && i < 16; i++)
            {
                dprint(PRN_FATAL,"0x%02x,", pSeg->VirAddr[i]);
            }
            dprint(PRN_FATAL,"\n");
        }
    }
    else
    {
        for( i=pStreamSegArray->Head;; )
        {
            if( i == pStreamSegArray->Tail ) /* ɨ�������δ�ҵ�ָID������ */
            {
                break;
            }
            else if( i == (SINT32)StreamID ) /* �ҵ�ָ��ID����������֪�����ͷŷ�... */
            {
                if( 1 == SMSEG_IS_READ(pStreamSegArray->StreamSeg[i]))
                {
                    pSeg = &pStreamSegArray->StreamSeg[i];
                }
                break;
            }

            i = (i+1)%MAX_STREAM_SEG_NUM;
        }
    }

    /* ���ҵ����ͷŵ����������ͷ�֮ */
    if( NULL != pSeg )
    {
        SMSEG_SET_ISRELEASED(*pSeg); /* ��־��������Ѿ����ͷ��� */

        dprint(PRN_SCD_INFO, "Release SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d StreamID=%d\n", pSeg->PhyAddr, pSeg->LenInByte, pSeg->IsLastSeg, StreamID);
        if( StreamID == (UINT32)pStreamSegArray->Head )  /* ���ͷŵ��ǵ�һ��������Ҫͬ�����¶�ָ�룬�ͷſռ乩��һ��ʹ�� */
        {
            for( i=pStreamSegArray->Head;; )
            {
                if(i >= MAX_STREAM_SEG_NUM)
                {
                    dprint(PRN_ERROR, "Array out of bound in function %s\n",__FUNCTION__);
                    Ret = FMW_ERR_PARAM;

                }
                if( i == pStreamSegArray->Tail ) /* ���������������ͷŵ��� */
                {
                    pStreamSegArray->Head = pStreamSegArray->Tail;
                    pStreamSegArray->SegBufReadAddr = pStreamSegArray->SegBufWriteAddr;
                    break;
                }
                else if( (i<(MAX_STREAM_SEG_NUM)) && (0 == SMSEG_IS_RELEASED(pStreamSegArray->StreamSeg[i]))) /* ����Ϊֹ */
                {
                    pStreamSegArray->Head = i;
                    pStreamSegArray->SegBufReadAddr = pStreamSegArray->StreamSeg[i].PhyAddr;
                    break;
                }
                else  /* ������ܻ��� */
                {
                    i = (i+1)%MAX_STREAM_SEG_NUM;
                }
            }
        }

        pStreamSegArray->SegTotalSize -= pSeg->LenInByte;
        SM_DOWN_CLIP_ZERO(pStreamSegArray->SegTotalSize);

        Ret = FMW_OK;
    }
    else
    {
        //dprint(PRN_ERROR, "Release Err StreamID %d in function %s\n", StreamID,__FUNCTION__);
        Ret = FMW_ERR_NOTRDY;
    }

    return Ret;
}


/************************************************************************
  ԭ��    SINT32 GetSegStreamSize( SEG_ARRAY_S *pSegStreamArray, SINT32 *pStreamSize)
  ����    ������Ƭ�ϼ��������а��ĳ����ۼ�������Ϊ����������Ŀ���ǻ��Ŀǰ������ռ�������
  ����    pSegStreamArray ָ������Ƭ�ϼ���
  ����ֵ  �ɹ������������ȣ����򷵻ش����루��������

 ************************************************************************/
SINT32 GetSegStreamSize( SEG_ARRAY_S *pSegStreamArray, SINT32 *pStreamSize)
{

    FMW_ASSERT_RET( 0 != pSegStreamArray, FMW_ERR_PARAM );
    *pStreamSize = pSegStreamArray->SegTotalSize;
    //FMW_ASSERT_RET( pSegStreamArray->SegBufSize < *pStreamSize, FMW_ERR_PARAM );

    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 GetSegStreamNum( SEG_ ARRAY_S *pSegStreamArray, SINT32 *pStreamNum )
  ����    ������Ƭ�ϼ������ѱ����͵���δ���ͷŵ�����������Ŀ���ǻ��Ŀǰ������ռ�������
  ����    pSegStreamArray ָ������Ƭ�ϼ���
  ����ֵ  �ɹ������������������򷵻ش����루��������
 ************************************************************************/
SINT32 GetSegStreamNum( SEG_ARRAY_S *pSegStreamArray, SINT32 *pStreamNum)
{   
    //SINT32 i, idx;
    //SINT32 StreamCount;

    FMW_ASSERT_RET( 0 != pSegStreamArray, FMW_ERR_PARAM );
    
    *pStreamNum = pSegStreamArray->Tail - pSegStreamArray->Head;

    if (*pStreamNum < 0)
    {
        *pStreamNum += MAX_STREAM_SEG_NUM;
    }
#if 0

    idx = pSegStreamArray->Head;
    *pStreamNum = 0;

    for( i = 0; i < StreamCount; i++ )
    {  
        if ( 0 == SMSEG_IS_RELEASED(pSegStreamArray->StreamSeg[idx]))
        {
            (*pStreamNum)++;
        }

        idx++;
        if( idx >= MAX_STREAM_SEG_NUM )
        {
            idx -= MAX_STREAM_SEG_NUM;
        }
    }
#endif
    return FMW_OK;
}

/************************************************************************
  ԭ��    SINT32 GetSegStreamNum( SEG_ ARRAY_S *pSegStreamArray, SINT32 *pStreamNum )
  ����    ������Ƭ�ϼ������ѱ����͵���δ���ͷŵ�����������Ŀ���ǻ��Ŀǰ������ռ�������
  ����    pSegStreamArray ָ������Ƭ�ϼ���
  ����ֵ  �ɹ������������������򷵻ش����루��������
 ************************************************************************/
VOID GetFreshSegStream( SEG_ARRAY_S *pSegStreamArray, SINT32 *pFreshNum, SINT32 *pFreshSize)
{
    FMW_ASSERT( 0 != pSegStreamArray );

    *pFreshNum = pSegStreamArray->SegFreshNum;
    *pFreshSize = pSegStreamArray->SegFreshSize;

    return;
}

/* ׼������: �յ��������ѵ�ǰ������ɣ�������Ѱ����һ������ͨ�� */
SINT32 SCDDRV_PrepareSleep(VOID)
{
    SINT32 ret = SCDDRV_OK;

    if (s_eScdDrvSleepStage == SCDDRV_SLEEP_STAGE_NONE)
    {
        if (0 == s_SmIIS.SCDState)
        {
            s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_SLEEP;
        }
        else
        {
            s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_PREPARE;
        }

        ret = SCDDRV_OK;
    }
    else
    {
        ret = SCDDRV_ERR;
    }

    return ret;
}

/* ��ѯ����: �������߽��е����ֽ׶� */
SCDDRV_SLEEP_STAGE_E SCDDRV_GetSleepStage(VOID)
{
    return s_eScdDrvSleepStage;
}

/* ǿ������: �޷���Ȼ���ߣ�ǿ�������sleep */
VOID SCDDRV_ForceSleep(VOID)
{
    if (s_eScdDrvSleepStage != SCDDRV_SLEEP_STAGE_SLEEP)
    {
        s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_SLEEP;
    }

    return;
}

/* �˳�����: �ָ��������� */
VOID SCDDRV_ExitSleep(VOID)
{
    SCDDRV_OpenHardware();
    s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_NONE;
    return;
}


/************************************************************************
  ԭ��    SINT32 SM_Reset ( SINT32 SmID )
  ����    ���������Ը�λһ����������ģ���ʵ���� 
  ������ʹ��ID��ΪSmID����������ģ��ʵ��������״̬���ص���ʼֵ��
  ����    SmID    ��ʶ��������ģ�����������Ϣ���ڶ�·�����У�ÿһ·����������һ����������ģ���
  ʵ����SmIDָ��ĳ���ض���ʵ����
  ����ֵ  �ɹ�����FMW_OK�����򷵻��ض��Ĵ�����
 ************************************************************************/
SINT32 SM_Reset (SINT32 SmID)
{   
    SINT32 RawNum = 0;
    SM_CFG_S TmpCfg;
    SINT32   TmpMode;
    UINT32   TmpHeaderLen;    
    UINT8    TmpHeaderBuf[HEADER_BUFFER_SIZE];
    SM_INSTANCE_S *pSmInstArray = NULL;

    SM_CHECK_VALUE(SmID, 0, SM_MAX_SMID-1, return FMW_ERR_PARAM);

    pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    if (pSmInstArray == NULL)
    {
        dprint(PRN_FATAL, "line: %d, pSmInstArray is NULL!\n", __LINE__);
        return FMW_ERR_PARAM;
    }

    /* ���ݱ�Ҫ��Ϣ */
    memcpy( &TmpCfg, &pSmInstArray->Config, sizeof(SM_CFG_S) );
    TmpMode = pSmInstArray->Mode;

    /* VC1 REAL8��Э����seek����£�OMXͨ·����������config���ݣ������Ҫ���� */
    TmpHeaderLen = pSmInstArray->HeaderLen;
    memcpy(TmpHeaderBuf, pSmInstArray->HeaderBuf, sizeof(pSmInstArray->HeaderBuf));

    /* ͣ�����ͨ�� */
    SM_Stop(SmID);

    /* �ͷ������е�ȫ��raw���ݣ���history��ʼ */
    if(FMW_OK != GetRawStreamNum(&pSmInstArray->RawPacketArray, &RawNum))
    {
        dprint(PRN_DBG, "%s %d FMW_OK !=  GetRawStreamNum !!\n",__FUNCTION__,__LINE__);
    }    
    DeleteRawPacket(&(pSmInstArray->RawPacketArray), (UINT32)RawNum);
    DeleteRawPacketInBuffer(SmID, 1);

    /* ��ȫ���ʵ���ڲ���Ϣ */
    memset(pSmInstArray, 0, sizeof(SM_INSTANCE_S) );
    pSmInstArray->LastPts = (UINT64)(-1);
    pSmInstArray->UpMsgStepNum = SM_SCD_UP_INFO_NUM;

    /* �ָ�ʵ�������ã�ģʽ����Ϣ */
    SM_Config(SmID, &TmpCfg);
    if( SM_INST_MODE_WORK == TmpMode || SM_INST_MODE_WAIT == TmpMode )
    {
        pSmInstArray->Mode = SM_INST_MODE_WORK;  /* ʹ֮��������״̬ */
    }
    else
    {
        pSmInstArray->Mode = SM_INST_MODE_IDLE;  /* ʹ֮���ڷ�����״̬ */
    }
    
    pSmInstArray->HeaderLen = TmpHeaderLen;
    memcpy(pSmInstArray->HeaderBuf, TmpHeaderBuf, sizeof(pSmInstArray->HeaderBuf));
    
    return FMW_OK;
}


SINT32 SM_ClearInst (SINT32 SmID)
{
    SM_CHECK_VALUE(SmID, 0, SM_MAX_SMID-1, return FMW_ERR_PARAM);

    /* ��ȫ���ʵ���ڲ���Ϣ */
    memset( s_SmIIS.pSmInstArray[SmID], 0, sizeof(SM_INSTANCE_S) );
    s_SmIIS.pSmInstArray[SmID]->LastPts = (UINT64)(-1);

    return FMW_OK;
}

/* 
   ����Ӧͨ��������ӳ������Ŀռ�
*/
SINT32 SM_AllocDSPCtxMem(SINT32 SmID)
{
    SINT32 Size    = 0;
    SINT32 Offset  = 0;
    UADDR  MemAddr = 0;
    MEM_RECORD_S stTempMem;
    MEM_RECORD_S  *pstMem    = NULL;
    DSP_CTX_MEM_S *pstDSPMem = NULL;

    Offset = 0;
    Size = DSP_SPS_MSG_SIZE + DSP_PPS_MSG_SIZE + 32;
    pstDSPMem = &s_SmIIS.DspCtxMem[SmID];
    pstMem = &stTempMem;

    memset(pstMem, 0, sizeof(MEM_RECORD_S));

    if( MEM_MAN_OK == MEM_AllocMemBlock("DSP_CTX", Size, pstMem, 0) )
    {
        MEM_AddMemRecord(pstMem->PhyAddr, pstMem->VirAddr, pstMem->Length);
    }
    else
    {
        dprint(PRN_FATAL,"%s Alloc mem for DSP (SPS PPS) failed!\n", __func__);
        return FMW_ERR_SCD;
    }

    memcpy(&(pstDSPMem->stBaseMemInfo), pstMem, sizeof(MEM_RECORD_S));
    MemAddr = pstMem->PhyAddr;
    Offset = ((MemAddr + 15) & (~15)) - MemAddr;

    pstDSPMem->DspSpsMsgMemAddr = MemAddr + Offset;
    pstDSPMem->pDspSpsMsgMemVirAddr = MEM_Phy2Vir(pstDSPMem->DspSpsMsgMemAddr);
    if (NULL == pstDSPMem->pDspSpsMsgMemVirAddr)
    {
        dprint(PRN_FATAL,"%s pDspSpsMsgMemVirAddr is NULL!\n", __func__);
        return FMW_ERR_SCD;
    }
    pstDSPMem->DspSpsMsgMemSize = DSP_SPS_MSG_SIZE;
    memset(pstDSPMem->pDspSpsMsgMemVirAddr, 0, DSP_SPS_MSG_SIZE);
    Offset += DSP_SPS_MSG_SIZE;

    pstDSPMem->DspPpsMsgMemAddr = MemAddr + Offset;
    pstDSPMem->pDspPpsMsgMemVirAddr = MEM_Phy2Vir(pstDSPMem->DspPpsMsgMemAddr);
    if (NULL == pstDSPMem->pDspPpsMsgMemVirAddr)
    {
        dprint(PRN_FATAL,"%s pDspPpsMsgMemVirAddr is NULL!\n", __func__);
        return FMW_ERR_SCD;
    }
    pstDSPMem->DspPpsMsgMemSize = DSP_PPS_MSG_SIZE;
    memset(pstDSPMem->pDspPpsMsgMemVirAddr, 0, DSP_PPS_MSG_SIZE);
    Offset += DSP_PPS_MSG_SIZE;

    return FMW_OK;

}


/*
   Ϊ��Ӧͨ���ͷŵ��ӳ������Ŀռ�
   */
VOID SM_DeletDSPCtxMem(MEM_RECORD_S *pstDSPMem)
{
    /* �ͷ���Դ ctx */
    if ( 0 != pstDSPMem->PhyAddr )
    {
        MEM_ReleaseMemBlock(pstDSPMem->PhyAddr, (UINT8 *)pstDSPMem->VirAddr);
    }
    else
    {
        dprint(PRN_FATAL, "pstDSPMem->PhyAddr = 0 is not right address\n");
    }

    MEM_DelMemRecord(pstDSPMem->PhyAddr, pstDSPMem->VirAddr, pstDSPMem->Length);

    return ;
}


/* SCD����ȫ�ִ� */
SINT32 SM_OpenSCDDrv(SCD_OPEN_PARAM_S *pOpenParam)
{
    SINT32 Offset;
    UADDR MemAddr;

    if(NULL == pOpenParam)
    {
        dprint(PRN_FATAL, "%s: pOpenParam = NULL\n",__func__);
        return FMW_ERR_SCD;
    }
    MemAddr = pOpenParam->MemAddr;
    
    SCDDRV_OpenHardware();

    if( 1 == s_SmIIS.IsScdDrvOpen )
    {
        dprint(PRN_FATAL, "s_SmIIS.IsScdDrvOpen %d is not opened.\n");
        return FMW_ERR_SCD;
    }

    /* ��λSCDӲ�� */
    ResetSCD();

    memset( &s_SmIIS, 0, sizeof(SM_IIS_S) );
    s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_NONE;

    /* ���Ի����ϣ�������Ϣ�� */
    Offset = ((MemAddr+15) & (~15)) - MemAddr;

    s_SmIIS.ScdDrvMem.HwMemAddr = MemAddr;
    s_SmIIS.ScdDrvMem.HwMemSize = pOpenParam->MemSize;

    s_SmIIS.ScdDrvMem.DownMsgMemAddr = MemAddr + Offset;
    s_SmIIS.ScdDrvMem.pDownMsgMemVirAddr = (UINT32 *)MEM_Phy2Vir(s_SmIIS.ScdDrvMem.DownMsgMemAddr);
    s_SmIIS.ScdDrvMem.DownMsgMemSize = SM_MAX_DOWNMSG_SIZE;
    Offset += (SM_MAX_DOWNMSG_SIZE+15) & (~15);

    s_SmIIS.ScdDrvMem.UpMsgMemAddr = MemAddr + Offset;
    s_SmIIS.ScdDrvMem.pUpMsgMemVirAddr = (SINT32 *)MEM_Phy2Vir(s_SmIIS.ScdDrvMem.UpMsgMemAddr);
    s_SmIIS.ScdDrvMem.UpMsgMemSize = SM_MAX_UPMSG_SIZE;
    Offset += (SM_MAX_UPMSG_SIZE+15) & (~15);
    
#ifdef VFMW_SCD_LOWDLY_SUPPORT
    s_SmIIS.ScdDrvMem.DvmMemAddr = MemAddr + Offset;
    s_SmIIS.ScdDrvMem.pDvmMemVirAddr = MEM_Phy2Vir(s_SmIIS.ScdDrvMem.DvmMemAddr);
    if(NULL == s_SmIIS.ScdDrvMem.pDvmMemVirAddr)
    {
        dprint(PRN_FATAL, "%s: s_SmIIS.ScdDrvMem.pDvmMemVirAddr = NULL\n",__func__);
        return VDMHAL_ERR;
    }
    memset(s_SmIIS.ScdDrvMem.pDvmMemVirAddr,0,DVM_SIZE);
    s_SmIIS.ScdDrvMem.DvmMemSize = DVM_SIZE;
    Offset += (DVM_SIZE+15) & (~15);

    s_SmIIS.ScdDrvMem.DspSedTopMemAddr = MemAddr + Offset;  
    s_SmIIS.ScdDrvMem.pDspSedTopMemVirAddr = MEM_Phy2Vir(s_SmIIS.ScdDrvMem.DspSedTopMemAddr);
    if(NULL == s_SmIIS.ScdDrvMem.pDspSedTopMemVirAddr)
    {
        dprint(PRN_FATAL, "%s: s_SmIIS.ScdDrvMem.pDspSedTopMemVirAddr = NULL\n",__func__);
        return VDMHAL_ERR;
    }
    s_SmIIS.ScdDrvMem.DspSedTopMemSize = DSP_SED_TOP_SIZE;
    memset(s_SmIIS.ScdDrvMem.pDspSedTopMemVirAddr,0,DSP_SED_TOP_SIZE);
    Offset += (DSP_SED_TOP_SIZE+15) & (~15);

    s_SmIIS.ScdDrvMem.DspCaMnMemAddr = MemAddr + Offset;
    s_SmIIS.ScdDrvMem.pDspCaMnMemVirAddr = MEM_Phy2Vir(s_SmIIS.ScdDrvMem.DspCaMnMemAddr);
    if(NULL == s_SmIIS.ScdDrvMem.pDspCaMnMemVirAddr)
    {
        dprint(PRN_FATAL, "%s: s_SmIIS.ScdDrvMem.pDspCaMnMemVirAddr = NULL\n",__func__);
        return VDMHAL_ERR;
    }
    memset(s_SmIIS.ScdDrvMem.pDspCaMnMemVirAddr,0,CA_MN_SIZE);
    s_SmIIS.ScdDrvMem.DspCaMnMemSize = CA_MN_SIZE;
    Offset += (CA_MN_SIZE+15) & (~15);
    //д���
    WriteCabacTabDsp(s_SmIIS.ScdDrvMem.DspCaMnMemAddr);
#endif

#if defined(VFMW_AVSPLUS_SUPPORT) || (VFMW_SCD_LOWDLY_SUPPORT)
    g_hex_base_addr= MemAddr + Offset;
    g_hex_base_addr = (g_hex_base_addr+127)/128*128;
    g_hex_base_vir_addr = MEM_Phy2Vir(g_hex_base_addr);
    if(NULL == g_hex_base_vir_addr)
    {
        dprint(PRN_FATAL, "%s: g_hex_base_vir_addr = NULL\n",__func__);
        return VDMHAL_ERR;
    }
    g_hex_len = 0x1180*4;
    Offset += g_hex_len;
#endif

    /* ���MemSize�Ƿ��ã��Լ���ַ�Ƿ���Ч */
    if( (Offset > pOpenParam->MemSize) || NULL == s_SmIIS.ScdDrvMem.pDownMsgMemVirAddr
            || NULL == s_SmIIS.ScdDrvMem.pUpMsgMemVirAddr )
    {
       dprint(PRN_FATAL, "Invalid: Offset %d > MemSize %d, pDownMsgMemVirAddr = %p, pUpMsgMemVirAddr = %p\n", Offset, pOpenParam->MemSize, s_SmIIS.ScdDrvMem.pDownMsgMemVirAddr, s_SmIIS.ScdDrvMem.pUpMsgMemVirAddr);
        return FMW_ERR_SCD;
    }
    else
    {
        s_SmIIS.IsScdDrvOpen = 1;  /* ����SCD�򿪳ɹ� */
    }

    return FMW_OK;
}

/* SCD����ȫ�ֹر� */
SINT32 SM_CloseSCDDrv(VOID)
{
    SINT32 i;

    /* ����ʵ�����ͣ�� */
    for( i = 0; i < MAX_CHAN_NUM; i++ )
    {
        (VOID)SM_Stop(i);
    }

    /* ��λSCDӲ�� */
    ResetSCD();

    /* ��������� */
    memset( &s_SmIIS, 0, sizeof(SM_IIS_S) );

    SCDDRV_CloseHardware();
    
    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 SetMaxSegSize(SINT32 ChanID)
  ����    ��ӡSeg���е���ϸ���
  ����    ͨ��ID
  ����ֵ  ��Ӧͨ��SCD Buf �������ֵ
 ********************************** *************************************/
SINT32 SetMaxSegSize(SINT32 ChanID, SM_CFG_S *pSmCfg)
{
    SINT32 MaxSegSize;
    SINT32 ChanWidth, ChanHeight;
    VDEC_CHAN_CAP_LEVEL_E eCapLevel;

    eCapLevel = VCTRL_GetChanCapLevel(ChanID);

    if (CAP_LEVEL_BUTT <= eCapLevel)
    {
        dprint(PRN_FATAL, "SetMaxSegSize get eCapLevel failed!\n"); 
        return FMW_ERR_PARAM;
    }

    if (CAP_LEVEL_USER_DEFINE_WITH_OPTION == eCapLevel)
    {
        ChanWidth = VCTRL_GetChanWidth(ChanID);
        ChanHeight = VCTRL_GetChanHeight(ChanID);
        if(VCTRL_ERR == ChanWidth || VCTRL_ERR == ChanHeight )
        {
            dprint(PRN_FATAL, "SetMaxSegSize get ChanWidth/ChanHeight failed!\n"); 
            return FMW_ERR_PARAM;
        }
        else
        {
            if((ChanWidth*ChanHeight) <= (1920*1088)) 
            {
                MaxSegSize = 3*1024*1024;
            }
            else
            {
                MaxSegSize = 20*1024*1024;
            }
        }
   }
   else
   {
        if((VFMW_H264 != pSmCfg->VidStd) && (VFMW_HEVC != pSmCfg->VidStd))
        {
            eCapLevel = CAP_LEVEL_H264_FHD;
        }
        switch(eCapLevel)
        {
            case CAP_LEVEL_HEVC_UHD:
                MaxSegSize = 20*1024*1024;
                break;

            case CAP_LEVEL_1488x1280:
            case CAP_LEVEL_1280x1488:
            case CAP_LEVEL_2160x1280:
            case CAP_LEVEL_1280x2160:
            case CAP_LEVEL_2160x2160:
                MaxSegSize = 4*1024*1024;
                break;

            case CAP_LEVEL_4096x2160:
            case CAP_LEVEL_2160x4096:
                MaxSegSize = 20*1024*1024;
                break;

            case CAP_LEVEL_4096x4096:
            case CAP_LEVEL_8192x4096:
            case CAP_LEVEL_4096x8192:
                MaxSegSize = 20*1024*1024;
                break;

            case CAP_LEVEL_8192x8192:
                MaxSegSize = 20*1024*1024;
                break;

            default:
                MaxSegSize = 3*1024*1024;
                break;
        }
    }


    return MaxSegSize;
}


/************************************************************************
  ԭ��    SINT32 SM_Config ( SINT32 SmID, SM_CFG_S *pSmCfg )
  ����    ��������������һ����������ģ���ʵ���������ʵ����ʼ����֮ǰ��������ô˺����������ñ�Ҫ����Ϣ��
  ����    SmID    ��ʶ��������ģ�����������Ϣ���ڶ�·�����У�ÿһ·����������һ����������ģ���ʵ����
  SmIDָ��ĳ���ض���ʵ����
  pSmCfg  SMʵ����������Ϣ
  ����ֵ  �ɹ�����FMW_OK�����򷵻��ض��Ĵ�����
 ************************************************************************/
SINT32 SM_Config (SINT32 SmID, SM_CFG_S *pSmCfg)
{
    SINT32 Offset, ByteOffsetInWord;
    SM_BUFFERCFG_S *pBufAddrCfg = NULL;
    UADDR TmpPhyAddr;
    UINT8* pTmpVirAddr;
    SINT32 SmMaxSegBufSize;
    SM_INSTANCE_S *pSmInstArray = NULL;
	SINT32 s32ScdBlankSegLen=0;

    if ((SmID < 0) || (SmID >= SM_MAX_SMID) || (NULL == pSmCfg))
    {
        dprint(PRN_FATAL, "line: %d, SmID = %d, pSmCfg = %p is not expected!\n", __LINE__, SmID, pSmCfg);        
        return FMW_ERR_PARAM;
    }

    pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    if (NULL == pSmInstArray)
    {
        return FMW_ERR_PARAM;	
    }

    /* ֻ�ڵ�һ��SM_Config()ʱ�������³�ʼ��SegArray���˺��SM_Config()��ֻ���޸����ã����ؽ�SegArray */
    if( pSmInstArray->Config.BufPhyAddr != pSmCfg->BufPhyAddr )
    {
        FMW_ASSERT_RET((0 != pSmCfg->BufPhyAddr), FMW_ERR_PARAM);
        FMW_ASSERT_RET((0 != pSmCfg->pBufVirAddr), FMW_ERR_PARAM);
        //SM_CHECK_VALUE(pSmCfg->VidStd, VFMW_START_RESERVED, VFMW_END_RESERVED - 1, return FMW_ERR_PARAM);

        pBufAddrCfg = &pSmInstArray->BufAddrCfg;

        pBufAddrCfg->DownMsgPhyAddr = s_SmIIS.ScdDrvMem.DownMsgMemAddr;
        pBufAddrCfg->pDownMsgVirAddr = s_SmIIS.ScdDrvMem.pDownMsgMemVirAddr;
        pBufAddrCfg->DownMsgSize = s_SmIIS.ScdDrvMem.DownMsgMemSize;

        pBufAddrCfg->UpMsgPhyAddr = s_SmIIS.ScdDrvMem.UpMsgMemAddr;
        pBufAddrCfg->pUpMsgVirAddr = s_SmIIS.ScdDrvMem.pUpMsgMemVirAddr;
        pBufAddrCfg->UpMsgSize = s_SmIIS.ScdDrvMem.UpMsgMemSize;
        pBufAddrCfg->UpMsgNum = MAX_STREAM_SEG_NUM;
#ifdef VFMW_SCD_LOWDLY_SUPPORT
        pBufAddrCfg->DspSpsMsgMemAddr         = s_SmIIS.DspCtxMem[SmID].DspSpsMsgMemAddr;
        pBufAddrCfg->pDspSpsMsgMemVirAddr     = s_SmIIS.DspCtxMem[SmID].pDspSpsMsgMemVirAddr;
        pBufAddrCfg->DspSpsMsgMemSize         = s_SmIIS.DspCtxMem[SmID].DspSpsMsgMemSize;

        pBufAddrCfg->DspPpsMsgMemAddr         = s_SmIIS.DspCtxMem[SmID].DspPpsMsgMemAddr;
        pBufAddrCfg->pDspPpsMsgMemVirAddr     = s_SmIIS.DspCtxMem[SmID].pDspPpsMsgMemVirAddr;
        pBufAddrCfg->DspPpsMsgMemSize         = s_SmIIS.DspCtxMem[SmID].DspPpsMsgMemSize;

        pBufAddrCfg->DvmMemAddr               = s_SmIIS.ScdDrvMem.DvmMemAddr;
        pBufAddrCfg->pDvmMemVirAddr           = s_SmIIS.ScdDrvMem.pDvmMemVirAddr;
        pBufAddrCfg->DvmMemSize               = s_SmIIS.ScdDrvMem.DvmMemSize;

        pBufAddrCfg->DspSedTopMemAddr         = s_SmIIS.ScdDrvMem.DspSedTopMemAddr;
        pBufAddrCfg->pDspSedTopMemVirAddr     = s_SmIIS.ScdDrvMem.pDspSedTopMemVirAddr;
        pBufAddrCfg->DspSedTopMemSize         = s_SmIIS.ScdDrvMem.DspSedTopMemSize;

        pBufAddrCfg->DspCaMnMemAddr           = s_SmIIS.ScdDrvMem.DspCaMnMemAddr;
        pBufAddrCfg->pDspCaMnMemVirAddr       = s_SmIIS.ScdDrvMem.pDspCaMnMemVirAddr;
        pBufAddrCfg->DspCaMnMemSize           = s_SmIIS.ScdDrvMem.DspCaMnMemSize;
#endif
        if(VFMW_AVS == pSmCfg->VidStd)
        {
            s32ScdBlankSegLen = SCD_AVS_SEG_BLANK_LEN;
        }
		else
		{
		   s32ScdBlankSegLen = SCD_SEG_BLANK_LEN;
		}
        TmpPhyAddr = (pSmCfg->BufPhyAddr + 15) & 0xfffffff0;
        ByteOffsetInWord = TmpPhyAddr - pSmCfg->BufPhyAddr;
        pTmpVirAddr = pSmCfg->pBufVirAddr + ByteOffsetInWord;
        Offset = s32ScdBlankSegLen;

        pBufAddrCfg->SegBufPhyAddr = TmpPhyAddr + s32ScdBlankSegLen;
        pBufAddrCfg->pSegBufVirAddr = pTmpVirAddr + s32ScdBlankSegLen;

        /* ����ͨ���������������� SmMaxSegBufSize ֵ*/   
        SmMaxSegBufSize = SetMaxSegSize(SmID, pSmCfg);
        if (FMW_ERR_PARAM == SmMaxSegBufSize)  // ��ȡʧ��ʱ������Ĭ��ֵ
        {
            SmMaxSegBufSize = 2*1024*1024;
        }

        if  ( (pSmCfg->BufSize - ByteOffsetInWord) > SmMaxSegBufSize)
        {
            pBufAddrCfg->SegBufSize =  (SmMaxSegBufSize - s32ScdBlankSegLen - 1024); // y00226912  (SM_MAX_SEG_BUF_SIZE - SCD_SEG_BLANK_LEN - 1024);    // ��������xk,�ײ�����1K
        }
        else
        {
            pBufAddrCfg->SegBufSize = pSmCfg->BufSize - ByteOffsetInWord - s32ScdBlankSegLen - 1024;    // ��������xk,�ײ�����1K
            if( pBufAddrCfg->SegBufSize < SM_MIN_SEG_BUF_SIZE )
            {
                return FMW_ERR_PARAM;
            }
        }

        ConfigStreamSegArray(&pSmInstArray->StreamSegArray, pBufAddrCfg->SegBufPhyAddr, pBufAddrCfg->pSegBufVirAddr, pBufAddrCfg->SegBufSize);        

        /*Check input pSmCfg param*/
        memcpy(&pSmInstArray->Config, pSmCfg, sizeof(SM_CFG_S));
    }

    /* �洢������Ϣ */
    memcpy( &pSmInstArray->Config, pSmCfg, sizeof(SM_CFG_S) );

    pSmInstArray->UpMsgStepNum = SM_SCD_UP_INFO_NUM;
    if (VFMW_MPEG4 == pSmCfg->VidStd)
    { 
#ifdef SCD_MP4_SLICE_ENABLE
        pSmInstArray->UpMsgStepNum = MAX_SM_SCD_UP_INFO_NUM;
#endif
    }

    pSmInstArray->InstID = SmID;
    return FMW_OK;
}


SINT32 SM_Start (SINT32 SmID)
{
    SINT32 ret = FMW_OK;

    if(s_SmIIS.pSmInstArray[SmID] != NULL
            && s_SmIIS.pSmInstArray[SmID]->Config.BufSize != 0  
            && SM_INST_MODE_IDLE == s_SmIIS.pSmInstArray[SmID]->Mode  )
    {
        s_SmIIS.pSmInstArray[SmID]->Mode = SM_INST_MODE_WORK;
        ret = FMW_OK;
    }
    else
    {
        ret = FMW_ERR_SCD;
    }

    return FMW_ERR_SCD;
}

SINT32 SM_Stop (SINT32 SmID)
{
    SINT32 i;


    if( s_SmIIS.pSmInstArray[SmID] == NULL
        || SM_INST_MODE_IDLE == s_SmIIS.pSmInstArray[SmID]->Mode )
    {
        return FMW_OK;
    }

    s_SmIIS.pSmInstArray[SmID]->Mode = SM_INST_MODE_IDLE;

    /* ȷ����ʵ���˺������������� */
    for( i = 0; i < 50; i++ )
    {
        if( s_SmIIS.SCDState == 0 )
        {
            return FMW_OK;
        }
        else
        {
            //OSAL_MSLEEP(10);
        }
    }

    return FMW_ERR_SCD;
}




/************************************************************************
  ԭ��    SINT32 SM_GetInfo( SINT32 SmID, SM_INST_INFO_S *pSmInstInfo )
  ����    ��������ѯ��������ģ����ԭʼ�������ۼ��˶��ٰ������и������������ж��ٶΡ��ڶ�·���뻷���£��ϲ���ȳ��������Ҫ����ÿһ·����Щ��Ϣ��������SCD�������һ·ȥ�и�������
  ����    SmID    ��������ģ���ʵ���ı�ʶ
  pSmInstInfo  ���ʵ����Ϣ�Ľṹ
  ����ֵ  FMW_ERR_PARAM or FMW_OK

 ************************************************************************/
//SINT32 SM_GetInfo(SINT32 SmID, SINT32 *pRawNum, SINT32 *pRawSize, SINT32 *pSegNum, SINT32 *pSegSize)
SINT32 SM_GetInfo( SINT32 SmID, SM_INST_INFO_S *pSmInstInfo )
{
    SINT32 Ret;
    SM_INSTANCE_S *pSmInstArray = NULL;

    SM_CHECK_VALUE(SmID, 0, SM_MAX_SMID-1, return FMW_ERR_PARAM);

    pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    if (NULL == pSmInstArray)
    {
        if (NULL != pSmInstInfo)
        {
            pSmInstInfo->InstMode = SM_INST_MODE_IDLE;
        }
        return FMW_ERR_PARAM;
    }

    pSmInstInfo->InstMode = pSmInstArray->Mode;

    pSmInstInfo->numReadSegFail = pSmInstArray->numReadSegFail;

    Ret = GetRawStreamNum(&pSmInstArray->RawPacketArray, &pSmInstInfo->RawNum);
    if (FMW_OK != Ret)
    {
        //return FMW_OK;
    }
    
    Ret = GetRawStreamSize(&pSmInstArray->RawPacketArray, &pSmInstInfo->TotalRawSize);
    if (FMW_OK != Ret)
    {
        //return FMW_OK;
    }
    
    Ret = GetSegStreamNum(&pSmInstArray->StreamSegArray, &pSmInstInfo->SegNum);
    if (FMW_OK != Ret)
    {
        //return FMW_OK;
    }
    
    Ret = GetSegStreamSize(&pSmInstArray->StreamSegArray, &pSmInstInfo->TotalSegSize);
    if (FMW_OK != Ret)
    {
        //return FMW_OK;
    }

    return FMW_OK;   
}

/************************************************************************
  ԭ��    SINT32 SM_PushRaw(SINT32 SmID, RAW_PACKET_S *pRawPacket)
  ����    �ⲿ������ñ���������������ģ������һ�����������͹���ֻ�ǽ������������Ϣ��¼��SMģ����ڲ��ṹ�У�����һ������ִ���������зֹ�����
  ����    SmID    ��������ģ���ʵ���ı�ʶ
  PhyAddr ԭʼ�����������ַ
  VirAddr ԭʼ�����������ַ
  Len ԭʼ�����ĳ��ȣ����ֽ�Ϊ��λ
  Pts ԭʼ�����Ľ�Ŀʱ���(PTS)
  ����ֵ  �ɹ�����FMW_OK�����򷵻��ض��Ĵ�����
 ************************************************************************/
SINT32 SM_PushRaw(SINT32 SmID, STREAM_DATA_S *pRawPacket)
{    
    SINT32 Ret;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];

    FMW_ASSERT_RET( 0 != pRawPacket->PhyAddr, FMW_ERR_PARAM );
    FMW_ASSERT_RET( 0 != pRawPacket->VirAddr, FMW_ERR_PARAM );
    FMW_ASSERT_RET( 0 != pSmInstArray, FMW_ERR_PARAM );

    Ret = InsertRawPacket(&pSmInstArray->RawPacketArray, pRawPacket);

    return Ret;

}


SINT32 SM_CopyRawToSeg(SINT32 SmID, STREAM_SEG_S *pStreamSeg)
{
    STREAM_DATA_S *pRawPacket;
    RAW_ARRAY_S *pRawPacketArray;
    SEG_ARRAY_S *pStreamSegArray;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    //STREAM_SEG_S *pTail;
    UADDR TmpPhyAddr;
    UINT8 *TmpVirAddr;
    UINT8 *SrcVirAddr = 0;
    SINT32 SrcLength = 0, TotalSrcLength = 0, SegFreeBufLen=0;
    UINT32 index, rawnum, i, rawnum1;

    index = 0;
    rawnum = 0;
    FMW_ASSERT_RET( NULL!=pSmInstArray, FMW_ERR_PARAM );

    pRawPacketArray = &pSmInstArray->RawPacketArray;
    pStreamSegArray = &pSmInstArray->StreamSegArray;

    FMW_ASSERT_RET( NULL!=pRawPacketArray && NULL!=pStreamSegArray && NULL!=pStreamSeg, FMW_ERR_PARAM );


    //Find a filled raw packet
    if (pRawPacketArray->Tail != pRawPacketArray->Head)
    {
        pRawPacket = &pRawPacketArray->RawPacket[pRawPacketArray->Head];
    }
    else
    {
        return FMW_ERR_BUSY;
    }

    index = pRawPacketArray->Head;
    TotalSrcLength = pRawPacket->Length;
    rawnum = 1;
    rawnum1 = 1;
    while (pRawPacketArray->RawPacket[index].is_not_last_packet_flag)
    {
        rawnum++;
        index = (index+1) % MAX_STREAM_RAW_NUM;
        TotalSrcLength += pRawPacketArray->RawPacket[index].Length;
        if (index == pRawPacketArray->Tail)
        {
            return FMW_ERR_BUSY ;  
        }
        if (rawnum > MAX_STREAM_RAW_NUM)
        {
            return FMW_ERR_BUSY;  
        }
    }

    ResetStreamSegArray(pStreamSegArray);

    if (TotalSrcLength > pStreamSegArray->SegBufSize - SM_MIN_SEG_BUF_SIZE)
    {
        dprint(PRN_FATAL, "Raw packet TOO BIG! can't copy to seg! Len: %#x\n", TotalSrcLength); 
        if(FMW_OK != DeleteRawLen(pRawPacketArray, TotalSrcLength))
        {
            dprint(PRN_FATAL,"line: %d, delet raw failed!\n",__LINE__);            
        }

        return FMW_OK;
    }

    TmpPhyAddr = pStreamSegArray->SegBufPhyAddr;
    SMSEG_PHY2VIR(pStreamSegArray, TmpPhyAddr, TmpVirAddr);

    SegFreeBufLen = pStreamSegArray->SegBufSize;

    if (((VFMW_VP6 == pSmInstArray->Config.VidStd) || (VFMW_VP6A == pSmInstArray->Config.VidStd))  && (0 == (g_VdmCharacter & VDM_SUPPORT_VP6)))
    {
        *TmpVirAddr++ = 0;
    }

    SrcVirAddr = pRawPacket->VirAddr; 
    SrcLength = pRawPacket->Length;
    index = pRawPacketArray->Head;

    if ((VFMW_VP6A == pSmInstArray->Config.VidStd) && (0 != (g_VdmCharacter & VDM_SUPPORT_VP6)) && (SrcLength != 0))
    {
// *TmpVirAddr++ = 0;

        if (SrcLength >= 3)
        {
            SrcVirAddr += 3;
            SrcLength  -= 3;
        }
        else
        {
            index = (index+1) % MAX_STREAM_RAW_NUM;     
            rawnum1++;
            if (rawnum1 > rawnum)
            {
                return FMW_ERR_BUSY;
            }
            SrcLength += pRawPacketArray->RawPacket[index].Length;
            SrcVirAddr  = pRawPacketArray->RawPacket[index].VirAddr;
            if (SrcLength >= 3)
            {
                SrcVirAddr += (3 -pRawPacketArray->RawPacket[((index+MAX_STREAM_RAW_NUM)-1) % MAX_STREAM_RAW_NUM].Length) ;
                SrcLength -=3;
                rawnum --;
            }
            else
            {	          
                index = (index+1) % MAX_STREAM_RAW_NUM;
                rawnum1++;
                if (rawnum1 > rawnum)
                {
                    return FMW_ERR_BUSY;
                }
                SrcLength += pRawPacketArray->RawPacket[index].Length;
                SrcVirAddr  = pRawPacketArray->RawPacket[index].VirAddr;
                if (SrcLength >= 3)
                {
                    SrcVirAddr += (3 - pRawPacketArray->RawPacket[((index+MAX_STREAM_RAW_NUM)-1) % MAX_STREAM_RAW_NUM].Length 
                            - pRawPacketArray->RawPacket[((index+MAX_STREAM_RAW_NUM)-2) % MAX_STREAM_RAW_NUM].Length);
                    SrcLength -= 3;
                    rawnum -= 2;
                }
                else
                {
                    return FMW_ERR_BUSY;
                }
            }
        }

    }

    //set seg para
    pStreamSeg->PhyAddr = TmpPhyAddr;
    pStreamSeg->VirAddr = (UINT8 *)MEM_Phy2Vir(TmpPhyAddr);
    pStreamSeg->LenInByte = TmpVirAddr - pStreamSeg->VirAddr;
    pStreamSeg->Pts = pRawPacket->Pts; /* һ֡������raw����ptsӦһ�� */
    pStreamSeg->RawPts = pRawPacket->Pts;
    pStreamSeg->Usertag = pRawPacket->UserTag;
    pStreamSeg->IsLastSeg = 1;

    for (i=0; i<rawnum; i++)
    {
        if ( (0!=TmpVirAddr) && (0!=SrcVirAddr) )
        {
            memcpy(TmpVirAddr, SrcVirAddr, SrcLength);
        }
        else
        {
            return FMW_ERR_BUSY;
        }

        //set seg para 
        pStreamSeg->IsStreamEnd              = pRawPacketArray->RawPacket[index].is_stream_end_flag;
        pStreamSeg->LenInByte               += SrcLength;

        TmpVirAddr                           = TmpVirAddr + SrcLength;
        index                                = (index+1) % MAX_STREAM_RAW_NUM;
        SrcVirAddr                           = pRawPacketArray->RawPacket[index].VirAddr;
        SrcLength                            = pRawPacketArray->RawPacket[index].Length;

        pStreamSeg->DispTime                 = pRawPacket->DispTime;
        pStreamSeg->DispEnableFlag           = pRawPacket->DispEnableFlag;
        pStreamSeg->DispFrameDistance        = pRawPacket->DispFrameDistance;
        pStreamSeg->DistanceBeforeFirstFrame = pRawPacket->DistanceBeforeFirstFrame;  
        pStreamSeg->GopNum                   = pRawPacket->GopNum;

    } 

    if ( FMW_OK == InsertStreamSeg(&pSmInstArray->StreamSegArray, pStreamSeg) )
    {
        //pStreamSegArray->SegBufWriteAddr += LenOfNULL;
    }

    //Delete raw packet
    if (FMW_OK != DeleteRawLen(pRawPacketArray, TotalSrcLength) )
    {
        //return FMW_ERR_SCD;
    }

    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 SM_ReadStreamSeg(SINT32 SmID, STREAM_SEG_S *pStreamSeg )
  ����    ����������������ģ���ȡһ���ѷָ���ϵ�����
  ����    SmID    ��������ģ���ʵ���ı�ʶ
  pStreamSeg  ������������Ϣ�Ľṹָ��
  ����ֵ  �����ȡ�ɹ�����FMW_OK�����򷵻���Ӧ�Ĵ�����
 ************************************************************************/
SINT32 SM_ReadStreamSeg(SINT32 SmID, STREAM_SEG_S *pStreamSeg)
{
    SINT32 ret = FMW_OK;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    FMW_ASSERT_RET( 0 != pStreamSeg, FMW_ERR_PARAM );
    FMW_ASSERT_RET( 0 != pSmInstArray, FMW_ERR_PARAM );

    if ( ( VFMW_REAL8    == pSmInstArray->Config.VidStd ) ||
         ( VFMW_REAL9    == pSmInstArray->Config.VidStd ) ||
         ( VFMW_DIVX3    == pSmInstArray->Config.VidStd ) ||
         ( VFMW_H263     == pSmInstArray->Config.VidStd ) ||
         ( VFMW_VP6      == pSmInstArray->Config.VidStd ) ||
         ( VFMW_SORENSON == pSmInstArray->Config.VidStd ) ||
         ( VFMW_VP6F     == pSmInstArray->Config.VidStd ) ||
         ( VFMW_VP6A     == pSmInstArray->Config.VidStd ) ||
         ( VFMW_VP8      == pSmInstArray->Config.VidStd ) ||
         ((VFMW_VC1      == pSmInstArray->Config.VidStd ) && (pSmInstArray->Config.StdExt.Vc1Ext.IsAdvProfile == 0)))
    {
        if ( FMW_OK != SM_CopyRawToSeg(SmID, pStreamSeg) )
        {
            dprint(PRN_SCD_INFO, "copy raw to seg fail\n");
            ret = FMW_ERR_SCD;
            goto SM_ReadStreamSeg_exit;
        }
    }

    ret = GetStreamSeg(&pSmInstArray->StreamSegArray, pStreamSeg);

#ifdef ENV_ARMLINUX_KERNEL
    if(ret == FMW_OK)
    {
        extern OSAL_FILE *vfmw_save_seg_file;
        extern UINT32 save_seg_chan_num;

        if( NULL != vfmw_save_seg_file && SmID == save_seg_chan_num)
        {
            VFMW_OSAL_SpinUnLock(G_SPINLOCK_THREAD);
            OSAL_FileWrite(pStreamSeg->VirAddr, pStreamSeg->LenInByte, vfmw_save_seg_file);
            VFMW_OSAL_SpinLock(G_SPINLOCK_THREAD);
        }
    }
#endif

SM_ReadStreamSeg_exit:
    if (FMW_OK != ret)
    {
        pSmInstArray->numReadSegFail++;
    }
    else
    {
        pSmInstArray->numReadSegFail = 0;
    }

    return ret;

}


/************************************************************************
  ԭ��    SINT32 SM_ReleaseStreamSeg( SINT32 SmID, SINT32 StreamID )
  ����    ��������Э����Ѿ�������ϵ�����Ƭ�黹����������ģ��
  ����    SmID    ��������ģ���ʵ���ı�ʶ
  StreamID    ����Ƭ��ID
  ����ֵ  ��
 ************************************************************************/
SINT32 SM_ReleaseStreamSeg(SINT32 SmID, SINT32 StreamID)
{
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    SINT32 ret;
    FMW_ASSERT_RET( 0 != pSmInstArray, FMW_ERR_PARAM );    

    ret =  ReleaseStreamSeg(&pSmInstArray->StreamSegArray, (UINT32)StreamID);

    return ret;
}


/***********************************************************************
  SM_GiveThreadEvent(): 
  SCD�жϺ���Ҫ���ݵ�ǰ���������ͨ��decparam�������ʹ�����̱߳�����
 ***********************************************************************/
VOID SM_GiveThreadEvent(SINT32 SmID)
{
    static UINT32 LastEventTime = 0;
    UINT32 CurTime = VFMW_OSAL_GetTimeInMs();
    UINT32  Period;
    SINT32 flag = 0;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];

    if (NULL == pSmInstArray)
    {
        return;
    }

    /*������decparam�ж�*/
    if ((pSmInstArray->InsertNewSegFlag == 1)&&
            (NULL == VCTRL_GetDecParam(SmID))&&(0 == VCTRL_IsChanSegEnough(SmID)))
    {
        flag = 1;
    }
    if (CurTime > LastEventTime)
    {
        Period = CurTime - LastEventTime;
    }
    else
    {
        Period = 2;
        LastEventTime = CurTime;
    }
    if ((Period >= 2)&&(flag == 1))
    {
        VFMW_OSAL_GiveEvent(G_INTEVENT);
        LastEventTime = CurTime;
    }

    return;
}

/************************************************************************
  ԭ��    SINT32 CalcInstBlockTime( SINT32 SmID )
  ����    ����ָ��ʵ����������ʱ�䣬���ϴ�����ʱ���뵱ǰʱ��֮���ۼӵ�
  ������ʱ����ȥ�����Ҹ���"�ϴ�����ʱ��"
  ����    SmID    ��������ģ���ʵ���ı�ʶ
  FirstCalc  1: ��һ�μ��㣬����¼"�ϴ�����ʱ��"
0: �ۼƼ��㣬����������֮�⻹Ҫ�ۼ�����ʱ��
����ֵ  �ۼ�����ʱ��
 ************************************************************************/
SINT32 CalcInstBlockTime( SINT32 SmID, SINT32 FirstCalc )
{
    UINT32  CurTime = 0;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];

    if (NULL == pSmInstArray)
    {
        return 0;
    }

    CurTime = VFMW_OSAL_GetTimeInMs();     /* ��ϵͳʱ���ȿ��ţ�����ʹ��OSALʵ�� */

    if( FirstCalc )
    {
        pSmInstArray->BlockTimeInMs = 0;
        pSmInstArray->LastBlockTime = CurTime;
    }
    else
    {
        if(CurTime > pSmInstArray->LastBlockTime)
        {
            pSmInstArray->BlockTimeInMs = CurTime - pSmInstArray->LastBlockTime;
        }
        else
        {
            pSmInstArray->BlockTimeInMs = 0;
            pSmInstArray->LastBlockTime = CurTime;
        }
    }

    return pSmInstArray->BlockTimeInMs;
}

/************************************************************************
  ԭ��    SINT32 SetInstMode( SINT32 SmID, SINT32 Mode )
  ����    ����ָ��ʵ����������ʱ�䣬���ϴ�����ʱ���뵱ǰʱ��֮���ۼӵ�
  ������ʱ����ȥ�����Ҹ���"�ϴ�����ʱ��"
  ����    SmID    ��������ģ���ʵ���ı�ʶ
  Mode    SM_INST_MODE_WORK  ����ģʽ
  SM_INST_MODE_WAIT  ����ģʽ
  ����ֵ  �ɹ�����FMW_OK�� ���򷵻ش�����
 ************************************************************************/
SINT32 SetInstMode( SINT32 SmID, SINT32 Mode )
{
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];
    FMW_ASSERT_RET( Mode==SM_INST_MODE_WORK || Mode==SM_INST_MODE_WAIT, FMW_ERR_PARAM );
    FMW_ASSERT_RET( NULL!=pSmInstArray, FMW_ERR_PARAM );

    pSmInstArray->Mode = Mode;
    return FMW_OK;
}



/*========================================================================
  part4.   SCD level �����и�Ϳ�����ģ�� 
  ========================================================================*/

/************************************************************************
  ԭ��    SINT32 CutStreamWithSCD(RAW_ARRAY_S *pRawStreamArray, SEG_ARRAY_S *pStreamSegArray, UINT32 StdType, SINT32 *pCutRawNum)
  ����    ����SCD�и�������
  ��������ԭʼ��������ȡ�����ɸ����������ø�SCD��Ȼ������SCD�иһ���и��ԭʼ����������ȡ��������������
  1.  ����Ƭ�λ������Ŀ��пռ�
  2.  ԭʼ������PTS�ֲ�
  �༴�����и���������ܳ��Ȳ��ô�������Ƭ�λ������Ŀ��пռ�������һ���и��������PTS���ܱ仯������MPEGϵ�п��������ܴ����ƣ���
  ����������ԭʼ����������ģ���ShowFirstRawPacket() / ShowNextRawPacket()����������ɨ��ԭʼ�������õ����и������������
  ����    pRawStreamArray ԭʼ������
  pStreamSegArray �и�����Ƭ�μ�
  StdType ������Э�����͡�
  SCDͬ��ͷ���������Э�����͡�
  pSmPushRaw ���и������������Ϣ
  ����ֵ  FMW_OK�������

 ************************************************************************/
SINT32 CutStreamWithSCD(SM_INSTANCE_S *pScdInstance)
{
    /*ʣ��İ���������*/
    SINT32 StreamNum, RawStreamSize;
    SINT32 SegStreamSize;

    /*һ������SCD���ٰ�raw packet*/
    SINT32 PushRawNum;
    SM_CTRLREG_S ScdCtrlReg;
    SINT32 Ret;
    UADDR FreeSegBufAddr = 0;
    UADDR SegBufPhyAddr = 0;
    SINT32 SegBufSize    = 0;
    SINT32 RawTotalLen;

    RAW_ARRAY_S *pRawStreamArray;
    SEG_ARRAY_S *pStreamSegArray;

    UINT32 StdType;
    UINT32 ScdLowdlyEnable;

    /*��ʼ������*/    
    pRawStreamArray = &(pScdInstance->RawPacketArray);
    pStreamSegArray = &(pScdInstance->StreamSegArray);
    
    StdType = pScdInstance->Config.VidStd;
    ScdLowdlyEnable = pScdInstance->Config.ScdLowdlyEnable;

    FMW_ASSERT_RET( 0 != pRawStreamArray, FMW_ERR_PARAM );
    FMW_ASSERT_RET( 0 != pStreamSegArray, FMW_ERR_PARAM );

    PushRawNum     = 0;
    StreamNum      = 0;
    RawStreamSize  = 0;
    SegStreamSize  = 0;
    FreeSegBufAddr = 0;
    RawTotalLen    = 0;
    memset(&(pScdInstance->SmPushRaw), 0, sizeof(SM_PUSHRAW_S));

    /*�õ�����������*/
    Ret = GetRawStreamNum(pRawStreamArray, &StreamNum);
    if (FMW_OK != Ret)
    {
        return Ret;
    }
    dprint(PRN_SCD_INFO, "RawStreamNum=%d\n", StreamNum);

    Ret = GetRawStreamSize(pRawStreamArray, &RawStreamSize);    
    if (FMW_OK != Ret)
    {
        return Ret;
    }
    dprint(PRN_SCD_INFO, "RawStreamSize=%d\n",RawStreamSize);

    /*����RawStreamSize���ж�������ǰ��<=0�����������������ּ�ʹ�в������ɹ������seg��ȴʹ����ͨ�������ò�����ʱ
      ��������⣬��RAW������֡Ϊ��λ�Ҽ����˽������ʱ���а��Ľ������RAW�����ʣ3��bytes(00 00 01)����ʱ��ͨ����Ȼ
      �ᱻSCD��������ΪSCD���ò�ѯģʽ������ÿ����һ�μ����߳�����һ�ε�ʱ�䣬����һ��ͨ���ͻ���ʱ��ô��ʱ��ŵõ�����
      ��ˣ��˴��ж�������Ϊ (RawStreamSize <= 3)�����Ա�����������ķ�����*/
    if ((StreamNum <= 0) || (RawStreamSize <= 3))
    {
        return FMW_ERR_RAWNULL;
    }

    Ret = GetSegStreamNum(pStreamSegArray, &StreamNum);
    if (FMW_OK != Ret)
    {
        return Ret;
    }

    /* StreamNum��ʾ���е�SegStreamNum���� */
    StreamNum = MAX_STREAM_SEG_NUM - StreamNum;
    dprint(PRN_SCD_INFO, "SegStreamNum=%d\n", StreamNum);
    FMW_ASSERT_RET((StreamNum > MAX_STREAM_SEG_NUM / 4), FMW_ERR_SEGFULL);

    /* SegStreamSize��ʾ���е�SegStreamSize��С */
    GetFreeSegBuf(pStreamSegArray, &FreeSegBufAddr, &SegStreamSize);

    /* FreeSegBufAddr��ʾSegBuf��дָ�룬��16�ֽڶ��� */
    FreeSegBufAddr = (FreeSegBufAddr + 15) & 0xfffffff0;
    SegStreamSize  = SegStreamSize - SM_SEGWASTE_BUF_SIZE - 15;
    dprint(PRN_SCD_INFO, "SegStreamSize=%d\n", SegStreamSize);

    /*û�п��пռ�*/
    //FMW_ASSERT_RET((SegStreamSize > 1024), FMW_ERR_SEGFULL);
    FMW_ASSERT_RET((SegStreamSize > (pStreamSegArray->SegBufSize >> 3)), FMW_ERR_SEGFULL);
    
    GetSegBuf(pStreamSegArray, &SegBufPhyAddr, &SegBufSize);

    pScdInstance->BufAddrCfg.UpMsgNum  = StreamNum;
    pScdInstance->BufAddrCfg.UpMsgSize = (pScdInstance->UpMsgStepNum) * pScdInstance->BufAddrCfg.UpMsgNum * sizeof(SINT32); //TODO
    /*���üĴ���*/
    ScdCtrlReg.ScdStart = 1;
    ScdCtrlReg.DownMsgPhyAddr  = pScdInstance->BufAddrCfg.DownMsgPhyAddr;
    ScdCtrlReg.pDownMsgVirAddr = pScdInstance->BufAddrCfg.pDownMsgVirAddr;
    ScdCtrlReg.UpMsgPhyAddr    = pScdInstance->BufAddrCfg.UpMsgPhyAddr;
 
    // UpLen �� word Ϊ��λ
    ScdCtrlReg.UpLen       = (pScdInstance->UpMsgStepNum) * pScdInstance->BufAddrCfg.UpMsgNum;//MAX_STREAM_SEG_NUM;//pScdInstance->BufAddrCfg.UpMsgSize/sizeof(SINT32);//pScdInstance->BufAddrCfg.UpMsgNum;
    ScdCtrlReg.BufferFirst = SegBufPhyAddr;
    ScdCtrlReg.BufferLast  = SegBufPhyAddr + SegBufSize;
    ScdCtrlReg.BufferIni   = FreeSegBufAddr;
    ScdCtrlReg.ScdIntMask  = 1;

    ScdCtrlReg.ScdLowdlyEnable = ScdLowdlyEnable;

    /* SCD ���ӳ�ʹ�ܱ�־ */
#ifdef VFMW_SCD_LOWDLY_SUPPORT
    ScdCtrlReg.DspSpsMsgMemAddr = pScdInstance->BufAddrCfg.DspSpsMsgMemAddr;
    ScdCtrlReg.DspPpsMsgMemAddr = pScdInstance->BufAddrCfg.DspPpsMsgMemAddr;
    ScdCtrlReg.DvmMemAddr       = pScdInstance->BufAddrCfg.DvmMemAddr;
    ScdCtrlReg.DspSedTopMemAddr = pScdInstance->BufAddrCfg.DspSedTopMemAddr;
    ScdCtrlReg.DspCaMnMemAddr   = pScdInstance->BufAddrCfg.DspCaMnMemAddr;
#endif
    ScdCtrlReg.reg_avs_flag = (VFMW_AVS == StdType)? 1: 0;

    /*1"��ʾҲ����slice��ʼ���⡣
      "0"��ʾ������slice��ʼ���⡣
      MPEG-4��ʱ���ܴ򿪽���slice��ʼ����
      ����*/
    if (VFMW_VC1 == StdType || VFMW_MVC == StdType || VFMW_HEVC == StdType)
    {
        StdType = VFMW_H264;
    }

    if (StdType == VFMW_AVS)
    {
        ScdCtrlReg.SliceCheckFlag = 0x01;
    }
#ifdef SCD_MP4_SLICE_ENABLE
    else if (StdType == VFMW_MPEG2 || StdType == VFMW_MPEG4)
    {
        ScdCtrlReg.SliceCheckFlag = 0x01;
    }
#endif
    else
    {
        ScdCtrlReg.SliceCheckFlag = 0;
    }    
    ScdCtrlReg.ScdProtocol = StdType;
    ScdCtrlReg.ScdIniClr   = 0x01;
    
    Ret = ScdDriver(&ScdCtrlReg, &(pScdInstance->SmPushRaw),SegStreamSize, pRawStreamArray, pScdInstance->BufAddrCfg.pDownMsgVirAddr, pScdInstance->BufAddrCfg.DownMsgPhyAddr);
    if (Ret != FMW_OK)
    {
        return Ret;
    }
    pScdInstance->IsWaitSCD = 1;
    pScdInstance->PrevCfgRawSize = RawStreamSize;
    pScdInstance->PrevCfgSegSize = SegStreamSize;

    if (RawStreamSize <= SegStreamSize)
    {
        /*�˴�SCD�������е�RAW�����������˴����������û���µ�RAW�����򲻱ؼ��ڼ��������ͨ������*/
        pScdInstance->LastCfgAllRawFlag = 1;
    }
    else
    {
        pScdInstance->LastCfgAllRawFlag = 0;
    }
    pScdInstance->FillNewRawFlag   = 0;
    pScdInstance->LastSCDStartTime = VFMW_OSAL_GetTimeInMs();
    pScdInstance->InsertNewSegFlag = 0;

    return FMW_OK;
}

/************************************************************************
  ԭ��    VOID WriteScdVtrlReg()
  ����    ����Scd���ƼĴ�������
  ����    
  ����ֵ  

 ************************************************************************/
VOID WriteScdVtrlReg(SM_CTRLREG_S *pSmCtrlReg)
{
    // LIST_ADDRESS
    WR_SCDREG(REG_LIST_ADDRESS, (unsigned int)pSmCtrlReg->DownMsgPhyAddr);

    // UP_ADDRESS
    WR_SCDREG(REG_UP_ADDRESS, (UINT32)pSmCtrlReg->UpMsgPhyAddr);

    // UP_LEN
    WR_SCDREG(REG_UP_LEN,(UINT32) pSmCtrlReg->UpLen);

    // BUFFER_FIRST
    WR_SCDREG(REG_BUFFER_FIRST, (UINT32)pSmCtrlReg->BufferFirst);

    // BUFFER_LAST
    WR_SCDREG(REG_BUFFER_LAST, (UINT32)pSmCtrlReg->BufferLast);

    // BUFFER_INI
    WR_SCDREG(REG_BUFFER_INI, (UINT32)pSmCtrlReg->BufferIni); 

    // AVS_FLAG    
    WR_SCDREG(REG_AVS_FLAG, (UINT32)pSmCtrlReg->reg_avs_flag);

#ifdef VFMW_SCD_LOWDLY_SUPPORT
    // SCD LOW DELAY
    WR_SCDREG(REG_DSP_SPS_MSG_ADDRESS, (UINT32)pSmCtrlReg->DspSpsMsgMemAddr);
    WR_SCDREG(REG_DSP_PPS_MSG_ADDRESS, (UINT32)pSmCtrlReg->DspPpsMsgMemAddr);
    WR_SCDREG(REG_DVM_MSG_ADDRESS,     (UINT32)pSmCtrlReg->DvmMemAddr);
    WR_SCDREG(REG_SED_TOP_ADDRESS,     (UINT32)pSmCtrlReg->DspSedTopMemAddr);
    WR_SCDREG(REG_CA_MN_ADDRESS,       (UINT32)pSmCtrlReg->DspCaMnMemAddr);
#endif     

    // SCD_PROTOCOL   
#ifdef ENV_SOS_KERNEL
    WR_SCDREG(REG_SCD_PROTOCOL, (UINT32)((pSmCtrlReg->ScdLowdlyEnable << 8) 
                                      | (1<<7) /* sec mode */
                                      | ((pSmCtrlReg->SliceCheckFlag << 4) & 0x10)
                                      | (pSmCtrlReg->ScdProtocol& 0x0f)));
#ifdef HIVDEC_SMMU_SUPPORT
    SMMU_SetMasterReg(SCD, SECURE_ON,  SMMU_OFF);
#endif

#else
    WR_SCDREG(REG_SCD_PROTOCOL, (UINT32)((pSmCtrlReg->ScdLowdlyEnable << 8) 
                                      | ((pSmCtrlReg->SliceCheckFlag << 4) & 0x10)
                                      | (pSmCtrlReg->ScdProtocol& 0x0f)));
#ifdef HIVDEC_SMMU_SUPPORT
    SMMU_SetMasterReg(SCD, SECURE_OFF, SMMU_ON);
#endif

#endif 

    PrintScdVtrlReg();
	
    // SCD_START
    WR_SCDREG(REG_SCD_START, 0);   
    WR_SCDREG(REG_SCD_START, (UINT32)(pSmCtrlReg->ScdStart & 0x01));
    
}

/************************************************************************
  ԭ��  VOID WriteScdMsg(SM_PUSHRAW_S *pSmPushRaw, SINT32 *pDownMsgVirAddr, UADDR DownMsgPhyAddr)
  ����  ����Scd������Ϣ��
  ����  
  ����ֵ    

 ************************************************************************/
SINT32 WriteScdMsg(SM_PUSHRAW_S *pSmPushRaw, SINT32 SegStreamSize, RAW_ARRAY_S *pRawStreamArray,
        SINT32 *pDownMsgVirAddr, UADDR DownMsgPhyAddr, SINT32 LowdlyFlag)
{
    /*Ϊʣ��İ���������*/
    /*һ������SCD���ٰ�raw packet*/
    SINT32 PushRawNum;
    STREAM_DATA_S RawPacket = {0};
    SINT32 RawShowTotalLen;
    SINT32 FirstRawOffset = 0;
    SINT32 j;

    /*****����Ͱ��Ĺ���*******/
    GetFirstOffset(pRawStreamArray, &FirstRawOffset);
    ShowFirstRawPacket(pRawStreamArray, &RawPacket);

    RawPacket.PhyAddr += FirstRawOffset;
    RawPacket.VirAddr += FirstRawOffset;
    RawPacket.Length  -= FirstRawOffset;

    RawShowTotalLen = RawPacket.Length;

    pDownMsgVirAddr[0] = 0;
    pDownMsgVirAddr[1] = 0;
    pDownMsgVirAddr[2] = 0;
    pSmPushRaw->PushRawTotalLen = 0;

    for (PushRawNum = 0, j = 0; (SegStreamSize >= RawShowTotalLen) && (PushRawNum < MAX_STREAM_RAW_NUM);)
    {
        /*****���һ��ScdҪ������,�����Ͱ��Ĺ���*******/
        if (RawPacket.Length > 0)
        {
#ifdef SM_RUN_ON_SOFTSCD
            pDownMsgVirAddr[j++] = RawPacket.VirAddr;
            pDownMsgVirAddr[j++] = RawPacket.VirAddr + RawPacket.Length - 1;
            pDownMsgVirAddr[j]   = &pDownMsgVirAddr[j + 1];
#else
            pDownMsgVirAddr[j++] = RawPacket.PhyAddr;
            pDownMsgVirAddr[j++] = RawPacket.PhyAddr + RawPacket.Length - 1;
            pDownMsgVirAddr[j]   = DownMsgPhyAddr + (j + 1) * (sizeof(SINT32));
#endif  
            j++;
            RawPacket.Length = 0;
        }
        
        pSmPushRaw->PushRawTotalLen = RawShowTotalLen;
        PushRawNum++;
        if ( FMW_OK != ShowNextRawPacket(pRawStreamArray, &RawPacket) )
        {
            break;
        }
        RawShowTotalLen += RawPacket.Length;
        /*���⣺���raw��Ϊ0��,���Կ��ǽ�PushRawNum++������,����if����,��Pts��Ҫ�࿼��һ��*/
    }

    // �Ͱ��(��һ���Ͳ���ȥ���N���Ͳ���ȥ)
    // ����һ����(0 == LowdlyFlag)�������ڲ����ӳ�ʱ�����ܽ�������ķ�֧
    // ԭ��������Ĵ����֧�Ὣһ��RAW PACKET �ֳ������͸�SCDȥ�У�������
    // ����SCD��ⲻ��һ֡�Ľ������Ӷ����ӹ����޷�ʵ��,
    // LowdlyFlag: 0 -- ���ӳ�δʹ�ܣ�1 -- ���ӳ�ʹ��
    if ((0 == LowdlyFlag) && (RawPacket.Length != 0) &&
            (pSmPushRaw->PushRawTotalLen < (SegStreamSize - 16)) &&
            (SegStreamSize < RawShowTotalLen) && (PushRawNum < MAX_STREAM_RAW_NUM))
    {
        if (RawPacket.Length >= SegStreamSize - pSmPushRaw->PushRawTotalLen)
        {
            RawPacket.Length = SegStreamSize - pSmPushRaw->PushRawTotalLen;
#ifdef SM_RUN_ON_SOFTSCD
            pDownMsgVirAddr[j++] = RawPacket.VirAddr;
            pDownMsgVirAddr[j++] = RawPacket.VirAddr + RawPacket.Length - 1;
            pDownMsgVirAddr[j] = (&pDownMsgVirAddr[j + 1]);
#else
            pDownMsgVirAddr[j++] = RawPacket.PhyAddr;
            pDownMsgVirAddr[j++] = RawPacket.PhyAddr + RawPacket.Length - 1;
            pDownMsgVirAddr[j] = DownMsgPhyAddr + (j + 1) * (sizeof(SINT32));
#endif  
            j++;  

            pSmPushRaw->PushRawTotalLen += RawPacket.Length; 
            PushRawNum++;            
            RawPacket.Length = 0;


        } 
    }

    if (0 == PushRawNum)
    {
        return FMW_ERR_SEGFULL;
    }
    pDownMsgVirAddr[j-1] = 0;

    pSmPushRaw->PushRawNum  = PushRawNum;
    pSmPushRaw->PushRawOver = 1;
    PrintDownMsg(DownMsgPhyAddr, pDownMsgVirAddr, PushRawNum);

    return FMW_OK;
}


/************************************************************************
  ԭ��  VOID ScdDriver(SM_CTRLREG_S *pSmCtrlReg, UINT32 *pRegUpAddress)
  ����  ���üĴ�������
  ����  
  ����ֵ    

 ************************************************************************/
SINT32 ScdDriver(SM_CTRLREG_S *pSmCtrlReg, SM_PUSHRAW_S *pSmPushRaw, SINT32 SegStreamSize,RAW_ARRAY_S *pRawStreamArray,
        SINT32 *pDownMsgVirAddr, UADDR DownMsgPhyAddr)
{
    SINT32 Ret;
    ResetSCD();
    Ret = WriteScdMsg(pSmPushRaw, SegStreamSize, pRawStreamArray, pDownMsgVirAddr, 
            DownMsgPhyAddr, pSmCtrlReg->ScdLowdlyEnable);
    if (Ret != FMW_OK)
    {
        return Ret;
    }

    WriteScdVtrlReg(pSmCtrlReg);

    return FMW_OK;
}


/************************************************************************
  ԭ��  VOID ReadScdStateReg(SM_STATEREG_S *pSmStateReg)
  ����  ��ȡScd״̬�Ĵ�������
  ����  
  ����ֵ    

 ************************************************************************/
VOID ReadScdStateReg(SM_STATEREG_S *pSmStateReg)
{
    /* state registers */
    pSmStateReg->Scdover     = RD_SCDREG(REG_SCD_OVER) & 0x01;
    pSmStateReg->ScdInt      = RD_SCDREG(REG_SCD_INT) & 0x01;
    pSmStateReg->ShortScdNum = (RD_SCDREG(REG_SCD_NUM) >> 22) & 0x3ff;
    pSmStateReg->ScdNum      = RD_SCDREG(REG_SCD_NUM) & 0x3fffff;
    pSmStateReg->ScdRollAddr = RD_SCDREG(REG_ROLL_ADDR);
    pSmStateReg->SrcEaten    = RD_SCDREG(REG_SRC_EATEN);

    PrintScdStateReg(pSmStateReg);
}

/************************************************************************
  ԭ��  SINT32 CheckScdStateReg(SM_STATEREG_S *pSmStateReg, SINT32 StdType)
  ����  ���Scd״̬�Ĵ�������
  ����  
  ����ֵ    

 ************************************************************************/
SINT32 CheckScdStateReg(SM_STATEREG_S *pSmStateReg, SINT32 StdType)
{
    //SINT32 ScdSegNum;
#if 0 
    /* state registers */
    FMW_ASSERT_RET((0 != pSmStateReg->Scdover), FMW_ERR_SCD);
    //FMW_ASSERT_RET((1 != pSmStateReg->ScdNum) || (0 == RD_SCDREG(REG_SRC_EATEN)), FMW_ERR_SCD);
    FMW_ASSERT_RET((0 != pSmStateReg->ScdRollAddr), FMW_ERR_SCD);
    //FMW_ASSERT_RET((0 != pSmStateReg->SrcEaten), FMW_ERR_SCD);
#else
    FMW_ASSERT_RET((0 != pSmStateReg->ScdRollAddr), FMW_ERR_SCD);

#endif

    return FMW_OK;
}

SINT32 ScdParseUpMsgInfo(SM_SEGSTREAMFILTER_S *pSegFilter, STREAM_SEG_S *ParseUpMsgInfo, UINT32 SegNum, UINT32 UpMsgStepNum)
{
    if (NULL == ParseUpMsgInfo)
    {
        return FMW_ERR_PARAM;
    }

    if (SM_MPEG4_NOTH263 == pSegFilter->ModeFlag)
    {
        ParseUpMsgInfo->stMp4UpMsg.IsShStreamFlag = 0;
    }
    else
    {
        ParseUpMsgInfo->stMp4UpMsg.IsShStreamFlag = 1;
    }

#ifdef SCD_MP4_SLICE_ENABLE
    #ifndef REP_SCD_ADDR_BUG
    ParseUpMsgInfo->stMp4UpMsg.SliceFlag = (pSegFilter->pScdUpMsg[SegNum*UpMsgStepNum + 0]>> 31) & 0x1; 
    #else
    ParseUpMsgInfo->stMp4UpMsg.SliceFlag = (pSegFilter->pScdUpMsg[SegNum*UpMsgStepNum + 1]>> 30) & 0x1; 
    #endif
    ParseUpMsgInfo->stMp4UpMsg.IsShortHeader = (pSegFilter->pScdUpMsg[SegNum*UpMsgStepNum + 2] >> 16) & 0x1;
    ParseUpMsgInfo->stMp4UpMsg.StartCodeBitOffset = (pSegFilter->pScdUpMsg[SegNum*UpMsgStepNum + 2]>> 4) & 0x7;
    ParseUpMsgInfo->stMp4UpMsg.StartCodeLength = pSegFilter->pScdUpMsg[SegNum*UpMsgStepNum + 2]& 0x7;
#else
    ParseUpMsgInfo->stMp4UpMsg.SliceFlag = 0;
    ParseUpMsgInfo->stMp4UpMsg.IsShortHeader = 0;
    ParseUpMsgInfo->stMp4UpMsg.StartCodeBitOffset = 0;
    ParseUpMsgInfo->stMp4UpMsg.StartCodeLength = 0;
#endif


    return FMW_OK;
}

/************************************************************************
  ԭ��  SINT32 ScdReturnSegFilter(SM_SEGSTREAMFILTER_S *pSegStreamFilter, SINT32 *pTransFormSeg, SINT32 *pSegNum, UINT32 UpMsgStepNum)
  ����  
  ����    pSegNum(����/���)
  ����ֵ    FMW_OK�������

 ************************************************************************/
SINT32 ScdReturnSegFilter(SM_SEGSTREAMFILTER_S *pSegStreamFilter, SINT32 *pTransFormSeg, SINT32 *pExtraSrcBack, SINT32 *pSegNum, UINT32 UpMsgStepNum)
{
    SINT32 i;
    SINT32 j;
    SINT32 SegNum;
    UINT8 *pTmpVirAddr;
    UADDR TmpPhyAddr;
    UINT32 Data32;
    /*��Ϊ������Ϣһ����SegNum + 1 ��,�����п������һ���ڵ�û�и�ֵ */
    SINT32 LastPacketOffset;
    //	  UINT32 BitOffset;

    SegNum = pSegStreamFilter->SegNum;
    LastPacketOffset = 0;
    *pExtraSrcBack = 0;    
    dprint(PRN_SCD_REGMSG,"***Print Up Msg Now\n");

    if (VFMW_MPEG4 != pSegStreamFilter->ProtocolType)
    {
        for (i = 0; i < SegNum ; i++)
        {
            #ifndef  REP_SCD_ADDR_BUG
            TmpPhyAddr = (pSegStreamFilter->pScdUpMsg[i * UpMsgStepNum] & 0x7fffffff);
            if (TmpPhyAddr < pSegStreamFilter->SegBufPhyAddr)
            {
                TmpPhyAddr |= 0x80000000;
            }
            pTransFormSeg[i * UpMsgStepNum]  = TmpPhyAddr;
            #else
            pTransFormSeg[i*UpMsgStepNum]  = pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum];
            #endif
            pTransFormSeg[i*UpMsgStepNum + 1]  = pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum + 1];
            //dprint(PRN_SCD_REGMSG,"ScdUpMsg[%d] = 0x%x\n",i,pSegStreamFilter->pScdUpMsg[i]);

        }
        pSegStreamFilter->ModeFlag = SM_NOT_MPEG4;
        pSegStreamFilter->LastH263Num = 0;
        *pSegNum = SegNum;
    }
    else
    {
        /*���ܴ���00 00 01����,������Ҫ����һ��word��Buffer�ײ�*/
        TmpPhyAddr = pSegStreamFilter->SegBufPhyAddr;
        pTmpVirAddr = (UINT8 *)MEM_Phy2Vir(TmpPhyAddr);
        FMW_ASSERT_RET((pTmpVirAddr != 0), FMW_ERR_SCD);

        Data32 =    (((UINT32)pTmpVirAddr[0] & 0xff) << 24)
                    |  (((UINT32)pTmpVirAddr[1] & 0xff) << 16)
                    |  (((UINT32)pTmpVirAddr[2] & 0xff) << 8)
                    |  (((UINT32)pTmpVirAddr[3] & 0xff));

        TmpPhyAddr = pSegStreamFilter->SegBufRollAddr;// + 1;//�����ַ�����Ѿ�����Ч���ݵĵ�ַ
        pTmpVirAddr = (UINT8 *)MEM_Phy2Vir(TmpPhyAddr);
        FMW_ASSERT_RET((pTmpVirAddr != 0), FMW_ERR_SCD);

        pTmpVirAddr[0] = (Data32 >> 24) & 0xff;
        pTmpVirAddr[1] = (Data32 >> 16) & 0xff;
        pTmpVirAddr[2] = (Data32 >> 8) & 0xff;
        pTmpVirAddr[3] = Data32 & 0xff;
        for ( i = 0, j = 0; i < SegNum; i++)
        {
            #ifndef  REP_SCD_ADDR_BUG
            TmpPhyAddr = (pSegStreamFilter->pScdUpMsg[i * UpMsgStepNum] & 0x7fffffff);
            if (TmpPhyAddr < pSegStreamFilter->SegBufPhyAddr)
            {
                TmpPhyAddr |= 0x80000000;
            }
            #else
            TmpPhyAddr = (SINT32)( pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum] );
            #endif
            FMW_ASSERT_RET((TmpPhyAddr != 0), FMW_ERR_SCD);

            pTmpVirAddr = (UINT8 *)MEM_Phy2Vir(TmpPhyAddr);

            FMW_ASSERT_RET((pTmpVirAddr != 0), FMW_ERR_SCD);

            Data32 =    (((UINT32)pTmpVirAddr[0] & 0xff) << 24)
                        |  (((UINT32)pTmpVirAddr[1] & 0xff) << 16)
                        |  (((UINT32)pTmpVirAddr[2] & 0xff) << 8)
                        |  (((UINT32)pTmpVirAddr[3] & 0xff));

            if((SM_VOP_START_CODE == Data32)
                    ||(SM_VIDOBJLAY_START_CODE == Data32))
            {            
                /* ��ǰ������ʽ���Ϊ"�Ƕ�ͷģʽ" */
                pSegStreamFilter->ModeFlag = SM_MPEG4_NOTH263;
                /* ��ShortHeader Picture ������0 */
                pSegStreamFilter->LastH263Num = 0;
                pTransFormSeg[j*UpMsgStepNum]  = TmpPhyAddr;
                pTransFormSeg[j*UpMsgStepNum + 1] = pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum + 1];
                j++;
                LastPacketOffset = 0;
            }
            else if (SM_SHORT_VIDEO_START_CODE == (Data32 & 0xfffffc00))
            {
                if (SM_MPEG4_NOTH263 != pSegStreamFilter->ModeFlag)
                {
                    /*��ǰ������δ���Ϊ�Ƕ�ͷ���ͷģʽ,����ǰ������ʽ���Ϊ"��ͷ"ģʽ����ԭ��Ϊ��Mpeg4ģʽ */
                    pSegStreamFilter->ModeFlag = SM_MPEG4_H263;
                    pTransFormSeg[j*UpMsgStepNum]  = TmpPhyAddr;
                    pTransFormSeg[j*UpMsgStepNum + 1] = pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum + 1];
                    j++;
                    LastPacketOffset= 0;
                }
                else 
                {
                    /* �����Ѿ����Ϊ"�Ƕ�ͷ"ģʽ,Picture ���� */
                    pSegStreamFilter->LastH263Num++;
                    LastPacketOffset = 1;
                    if(pSegStreamFilter->LastH263Num > SM_H263_THRESHOLD_NUM)
                    {
                        REPORT_UNSUPPORT(s_SmIIS.ThisInstID);
                        /* ����ǰ������ʽ���Ϊ"��ͷ"ģʽ */
                        pSegStreamFilter->ModeFlag = SM_MPEG4_H263;
                        pTransFormSeg[j*UpMsgStepNum]  = TmpPhyAddr;
                        pTransFormSeg[j*UpMsgStepNum + 1] = pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum + 1];
                        j++;
                        LastPacketOffset = 0;
                    }
                    else
                    {
#ifdef SCD_MP4_SLICE_ENABLE  					  
                        //�Ƕ�ͷ�����У��յ�byte����Ķ�ͷ�����ܹ��˵�����Ҫ����slice����
                        pTransFormSeg[j*UpMsgStepNum]   = TmpPhyAddr;
                        pTransFormSeg[j*UpMsgStepNum+1] = pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum+1];
                        j++;
#endif					
                    }
                }
            }
            else
            {
#if 0   /* �����Ĺ��������� */
                BitOffset = (pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum + 2] >> 4) & 0x7;
                /*�ڷǶ�ͷ�����У��յ��˷�byte����Ķ�ͷ��ʼ�룬���˵�*/
                if ((BitOffset != 0) && (pSegStreamFilter->ModeFlag == SM_MPEG4_NOTH263))
                {
                    continue;
                }
#endif                  
                /*mode���䣬��ǰͷ��Ч*/
                pTransFormSeg[j*UpMsgStepNum]  = TmpPhyAddr;//pSegStreamFilter->pScdUpMsg[i*2];
                pTransFormSeg[j*UpMsgStepNum + 1] = pSegStreamFilter->pScdUpMsg[i*UpMsgStepNum + 1];
                j++;
                LastPacketOffset = 0;
            }
        }
        *pSegNum = j;  // zhl

        for (i = 0; i < j; i++)
        {
            //dprint(PRN_SCD_REGMSG,"ScdUpMsg[%d] = 0x%x\n",i,pTransFormSeg[i*UpMsgStepNum]);
        }
    }        

    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 CheckSegValid(STREAM_SEG_S *pSegPacket, SINT32 SendSegTotalSize, SINT32 SegFreeBufLen)
  ����    
  ����    pPushRawPacket:�����Ľṹ��
PreSegTotalSize:����֮ǰ��������ܳ���
SegFreeBufLen:Seg Stream Buffer�л�ʣ����ٳ���
����ֵ  FMW_OK�������

 ************************************************************************/
SINT32 CheckSegValid(STREAM_SEG_S *pSegPacket,SINT32 SegFreeBufLen)
{
    FMW_ASSERT_RET((NULL != pSegPacket), FMW_ERR_PARAM);

    FMW_ASSERT_RET((0 != pSegPacket->PhyAddr), FMW_ERR_SCD);
    FMW_ASSERT_RET((NULL != pSegPacket->VirAddr), FMW_ERR_SCD);
    //FMW_ASSERT_RET((pSegPacket->LenInByte >= 4) && (pSegPacket->LenInByte <= SegFreeBufLen), FMW_ERR_SCD);
    FMW_ASSERT_RET((pSegPacket->LenInByte <= SegFreeBufLen), FMW_ERR_SCD);

    return FMW_OK;
}


/************************************************************************
  ԭ��  SM_PUSHRAW_S *pPushRaw, SINT32 CurrSegTotalSize, SINT32 *pCurrRawId,SINT64 *pPts)
  ����  
  ����    
  ����ֵ    Pts

 ************************************************************************/

/*����һ�����ȣ��Ϳ��Եõ������������Ӧ��Pts*/
VOID GetSegPts(RAW_ARRAY_S *pRawStreamArray, SINT32 Len, UINT64 *pLastPts, STREAM_SEG_S* pSegPacket)
{
    SINT32 Num;
    SINT32 Offset;
    UINT64*  pPts      = &(pSegPacket->Pts);
    UINT64*  pUsertag  = &(pSegPacket->Usertag);
    UINT64*  pRawPts   = &(pSegPacket->RawPts);
    UINT64*  pDisptime = &(pSegPacket->DispTime);
    UINT32*  pDispEnableFlag = &(pSegPacket->DispEnableFlag);     
    UINT32*  pDispFrameDistance = &(pSegPacket->DispFrameDistance);     
    UINT32*  pDistanceBeforeFirstFrame = &(pSegPacket->DistanceBeforeFirstFrame);     
    UINT32*  pGopNum = &(pSegPacket->GopNum);     

    Num = 0;
    Offset = 0;
    *pPts      = (UINT64)(-1);
    *pUsertag  = (UINT64)(-1);
    *pRawPts   = (UINT64)(-1); 
    *pDisptime = (UINT64)(-1);
    *pDispEnableFlag = (UINT32)(-1);
    *pDispFrameDistance = (UINT32)(-1);
    *pDistanceBeforeFirstFrame = (UINT32)(-1);
    *pGopNum = (UINT32)(-1);

    GetRawNumOffsert(pRawStreamArray, Len, &Num, &Offset, pPts, pUsertag, pDisptime, pDispEnableFlag, pDispFrameDistance, pDistanceBeforeFirstFrame, pGopNum, pRawPts);


    if (*pLastPts == *pPts)
    {
        *pPts = (UINT64)(-1);   
    }
    else
    {
        *pLastPts = *pPts;
    }

    return;   
}


SINT32 IsVopHeader(SINT32 ModeFlag, UINT32 StartCode)
{
    SINT32 flag = 0;
    
    /*�Ƕ�ͷvop��0x000001B6��ǣ���ͷvop��0x000080xx ~ 0x000083xx���*/
    /*���ڴ�С�˲�һ�����⣬ʹ��0xB6010000��Ϊ�Ƕ�ͷ�ж���ʼ��������0xxx800000~0xxx830000��Ϊ��ͷ��ʼ���ж����������Ч��*/
    if (SM_MPEG4_NOTH263 == ModeFlag)
    {
        if (0xB6010000 == StartCode)
        {
            flag = 1;
        } 
    }
    else if (SM_MPEG4_H263 == ModeFlag)
    {
        if ((((StartCode & 0xFF0000) >> 16)  >= 0x80)
        && ((((StartCode & 0xFF0000) >> 16)) <= 0x83))
        {
            flag = 1;
        }
    }
	
    if (0xFE010000 == StartCode) // ���һ֡�Զ���ͷ
    {
        flag = 1;
    }
	
    return flag;
}

SINT32 GetNextVop(SM_INSTANCE_S *pScdInstance, SINT32 *pUsedSeg, SEG_ARRAY_S *pStreamSegArray, SINT32 Seg_num)
{
    SM_STATEREG_S SmStateReg;
    SINT32 StdType;
    SINT32 ScdSegNum = 0;
    SINT32 Ret;
    SINT32 i;
    UADDR  CurrSegPhyAddr;
    UINT32 start_code;
    UINT8  *VirAddr = NULL;
    SINT32 count = 0;
    UINT32 SegNum = 0;

    StdType = (RD_SCDREG(REG_SCD_PROTOCOL)&0x0f);

    /*��ȡScd״̬�Ĵ�������*/
    ReadScdStateReg(&SmStateReg);
    Ret = CheckScdStateReg(&SmStateReg, StdType);
    if(FMW_OK !=  GetSegStreamNum(pStreamSegArray, &SegNum))
    {
        dprint(PRN_DBG, "%s %d FMW_OK !=  GetSegStreamNum!!\n",__FUNCTION__,__LINE__);
    }

    SegNum = MAX_STREAM_SEG_NUM - SegNum;

    if (FMW_OK != Ret)
    {
        return Ret;
    }
    ScdSegNum = Seg_num;

    for(i = ScdSegNum - 1; i >= 0; i--)
    {
        CurrSegPhyAddr  = pUsedSeg[i*(pScdInstance->UpMsgStepNum)];
        SMSEG_PHY2VIR(pStreamSegArray, CurrSegPhyAddr, VirAddr);
        /* �ڰ�ȫ�⣬����ֱ�Ӷ�ȡһ��word��ֻ�ܶ�ȡһ���ֽ� */
        start_code = *((UINT8 *)VirAddr) + (*(((UINT8 *)VirAddr)+1) << 8) + (*(((UINT8 *)VirAddr)+2) << 16) + (*(((UINT8 *)VirAddr)+3) << 24);

        if (1 == IsVopHeader(pScdInstance->SegFilter.ModeFlag, start_code))
        {
            count++;
            if(count >= 1)
            {
                ScdSegNum--;
                /* �ɲ����seg����Ӧ�ô���ʵ��Ҫ����ĸ��� */
                if(SegNum < ScdSegNum)
                {
                    continue;
                }
                break;
            }
        }
        ScdSegNum--;
    }

    return ScdSegNum;
}

/************************************************************************
  ԭ��  SINT32 ProcessSCDReturn(RAW_ ARRAY_S *pRawStreamArray, SEG_ARRAY_S *pStreamSegArray, SINT32 *pCutRawNum)
  ����  ����SCD�ķ�����Ϣ��
  ��������SCD�ļĴ������ϡ�������Ϣ���ж�ȡ��һ�ε������и���Ϣ������������ֲ�����
  1.    ����������Ϣ����֡����NAL��������STREAM_SEG_S��ʽ��֯���Ҳ��뵽����Ƭ�μ�pStreamSegArray�й�������
  2.    �����˴��и������˶���ԭʼ��������������Щ�������ͷš�
  ����
  pRawStreamArray   ԭʼ������
  pStreamSegArray   �и�����Ƭ�μ�
  pSmPushRaw   ��һ������Scd��ԭʼ����
  pSmPushRaw ���и������������Ϣ
  ����ֵ    FMW_OK�������

 ************************************************************************/
SINT32 ProcessScdReturn(SM_INSTANCE_S *pScdInstance)
{
    SINT32 Ret;
    SINT32 i = 0;
    SM_STATEREG_S SmStateReg;
    STREAM_SEG_S SegPacket;
    SINT32 RawStreamSize;
    SINT32 SrcEaten = 0;
    SINT32 PushRawNum;
    SINT32 ScdSegNum;//Scd�ͳ���Seg��ʵ�ʰ���;
    SINT32 StreamNum=0;
    UADDR  CurrSegPhyAddr;
    UADDR  NextSegPhyAddr;
    SINT32 SegPacketLen;
    UADDR  SegBufAddr = 0;
    SINT32 SegBufLength = 0;
#ifdef SCD_MP4_SLICE_ENABLE	  
    UINT32 Data32;
    UINT8 *pTmpVirAddr = NULL;
#endif
    RAW_ARRAY_S *pRawStreamArray;
    SEG_ARRAY_S *pStreamSegArray;
    SM_PUSHRAW_S  *pSmPushRaw;
    SINT32 *pUsedSeg = NULL;
    SINT32 StdType;
    /*��Ptsʱ*/
    SINT32 CurrRawId;
    SINT32 SegFreeBufLen = 0;
    UADDR  TmpAddr = 0;
    SINT32 ScdErrFlag = 0;
    SINT32 ExtraSrcBack;
    SINT32 LastSegTail;
    UINT32 UpMsgStepNum;
    STREAM_SEG_S LastSegPacket;
    UINT32 UpNum = 0;
    UINT32 ReadScdSegNum;
    UINT32 FilterScdSegNum;

    if( 0 == pScdInstance->IsWaitSCD )
    {
        return FMW_ERR_SCD;
    }
    if(pScdInstance->SmPushRaw.PushRawOver == 3)
    {
        pScdInstance->SmPushRaw.PushRawOver = 2;
        return FMW_ERR_SCD;
    }
    /*��ʼ������*/    
    pRawStreamArray = &(pScdInstance->RawPacketArray);
    pStreamSegArray = &(pScdInstance->StreamSegArray);
    StdType = (RD_SCDREG(REG_SCD_PROTOCOL)&0x0f);
    pSmPushRaw = &(pScdInstance->SmPushRaw);
    pSmPushRaw->PushRawOver = 2;

    memset(&SegPacket, 0, sizeof(STREAM_SEG_S));
    memset(&LastSegPacket, 0, sizeof(STREAM_SEG_S));      
    memset(&SmStateReg, 0 , sizeof(SmStateReg));
    RawStreamSize = 0;
    SegPacketLen = 0;
    PushRawNum = pSmPushRaw->PushRawNum;
    pUsedSeg = NULL;
    CurrRawId = 0;
    SegFreeBufLen = 0;
    TmpAddr = 0;
    ScdErrFlag = 0;
    SrcEaten = 0;
    UpMsgStepNum = pScdInstance->UpMsgStepNum;


    /*��ȡScd״̬�Ĵ�������*/
    ReadScdStateReg(&SmStateReg);
    Ret = CheckScdStateReg(&SmStateReg, StdType);    
    if (FMW_OK != Ret)
    {
        return Ret;
    }

    ScdSegNum = SmStateReg.ScdNum;  // zhl.
    ReadScdSegNum = ScdSegNum;
    /* l00232354: ��� SCD �г�������ʼ�볬���� MAX_STREAM_SEG_NUM*10 ���������Ϊ���Ǹ�������������ǿ�ж�
      һ��RAW �����ԡ��޸ı�����һ��H264 4K�������м�ĳЩ��֡�е�����ȫ�ǣ�00 00 01 xx����������������
      �ŵ����﷨����ȥ���Ļ����Ῠס��Ｘ���ӵ�ʱ�䣬���ǲ��ɽ��ܡ�          
     */
    if((ReadScdSegNum > MAX_STREAM_SEG_ALLOW) && (VFMW_H264 == pScdInstance->Config.VidStd))
    {
        dprint(PRN_FATAL,"line:%d ReadScdSegNum = %d, FilterScdSegNum = 0 release raw packet anyway!\n", __LINE__, ReadScdSegNum);
        DeleteRawPacket(pRawStreamArray, 1);
        return FMW_ERR_SCD;
    }

    GetSegBuf(pStreamSegArray, &SegBufAddr, &SegBufLength );
    UpNum = ScdSegNum;

    /*���˰�,��Ҫ����Mpeg4*/
    pScdInstance->SegFilter.ProtocolType = (RD_SCDREG(REG_SCD_PROTOCOL)&0x0f);    
	pScdInstance->SegFilter.SegNum = MIN(MIN(ScdSegNum,MAX_STREAM_SEG_NUM),(((SINT32)RD_SCDREG(REG_UP_LEN))/UpMsgStepNum));

    /* �ݴ�:  �����������������ȫ������������Ϊ�ռ䲻�㣬���޷��������seg��Ϣ�ϱ���
       ��ʱ����Ҫ��LastCfgAllRawFlag��־�������������е���scd�ӳ���������ɽ��뿨�� */
    if (pScdInstance->SegFilter.SegNum < ScdSegNum && 1 == pScdInstance->LastCfgAllRawFlag)
    {
        pScdInstance->LastCfgAllRawFlag = 0;
    }

    pScdInstance->SegFilter.pScdUpMsg = pScdInstance->BufAddrCfg.pUpMsgVirAddr;
    pScdInstance->SegFilter.SegBufPhyAddr = SegBufAddr;
    SMSEG_PHY2VIR(pStreamSegArray, SegBufAddr, pScdInstance->SegFilter.pSegBufVirAddr);
    pScdInstance->SegFilter.SegBufSize = SegBufLength;
    pScdInstance->SegFilter.SegBufRollAddr = SmStateReg.ScdRollAddr;

    Ret = ScdReturnSegFilter(&(pScdInstance->SegFilter), pScdInstance->TransFormSeg, &ExtraSrcBack, &ScdSegNum, UpMsgStepNum);
    if (FMW_OK != Ret)
    {
        return FMW_ERR_SCD;
    }
    /*�������SegNum < 2����Ϊ��ʱ����Filter֮��SegNum��һ���ˣ���ô��Ҫ����һ��*/
    /*��ֻ������0����һ��ͷ*/
    if (ScdSegNum < 2)
    {
        /* �в�����Ч��seg!
           ���raw���㹻��(����seg buf�Ŀ��ÿռ�)�����в�����ԭ�������seg buf�Ŀ��ÿռ�̫С. 
           ��������£�seg buf�����Ž������ͷ�seg�������죬��Ȼ����в��������´ο��ܾ������ˣ����Դ�
           ʱ����óȻ�����������ǣ���һ����̫��(����ÿ��slice������1M)�������������󣬻���vfmw������δ��
           �ֵĴ���seg buf���ܲ��������죬scd��Զ�޷��г��µ������������ɽ�����ֹͣ����.
           Ӧ�Է���: ��������ɹ鲢��seg buf full�������Դ˴���SegFullFlag��־��������DoNextInstance()
           ��⵽�˱�־���������ʱ��һ��ʱ���㹻������ǿ���ͷ�seg buf��ʹ���и���Լ���. */
        if ( pScdInstance->PrevCfgRawSize > pScdInstance->PrevCfgSegSize )
        {
            pScdInstance->SegFullFlag = 1;
        }
        else
        {
            pScdInstance->SegFullFlag = 0;
        }

        if (ScdSegNum == 0)
        {
            /*����Ҫ��4����λ���ͣ�����ֻ��0��1��ͷ�ǲ��������߽�������*/
            /*����İ�ȫ��ɾ��*/
            SrcEaten = pScdInstance->SmPushRaw.PushRawTotalLen;
            /*��Ϊ������00 00 01 xx�����������Ҫ�ټ�3���ֽ�*/
            SrcEaten = (SrcEaten >= 4)? (SrcEaten - 3) : 0;
        }
        else if (ScdSegNum == 1)
        {
            /*��һ��ͷǰ��ȫ��ɾ������Ϊ�͵�������00 00 01 xx ��ʼ�͵�
              ��ʵֻ�е�һ�βŻ�ɾ��������������ɾ����*/
            //��һ���ض�����Ч��ͷ��������pScdInstance->BufAddrCfg.pUpMsgVirAddr[1]Ҳ��һ����
            SINT32 DiscardThr;
            DiscardThr = pScdInstance->StreamSegArray.SegBufSize * 3 / 4;
            #ifndef REP_SCD_ADDR_BUG
            SrcEaten = pScdInstance->TransFormSeg[1];//pScdInstance->BufAddrCfg.pUpMsgVirAddr[1];
            #else
            SrcEaten = pScdInstance->TransFormSeg[1] & 0x3FFFFFFF;
            #endif
            //ʣ�µĳ�����Ȼ����SM_RAW_DISCARD_SIZE,������İ�ȫ��ɾ�����������������ֽ�
            //if ((pScdInstance->SmPushRaw.PushRawTotalLen - SrcEaten) > SM_RAW_DISCARD_SIZE)
            if ((pScdInstance->SmPushRaw.PushRawTotalLen - SrcEaten) > DiscardThr)
            {
                dprint(PRN_FATAL, "Error! (PushRawTotalLen - SrcEaten) > SM_RAW_DISCARD_SIZE, packet discard...\n");
                SrcEaten = pScdInstance->SmPushRaw.PushRawTotalLen;
            }
        }
        Ret = DeleteRawLen( pRawStreamArray, SrcEaten );
        if (FMW_OK != Ret)
        {
            return FMW_ERR_SCD;
        }
        return FMW_OK;
    }
    else
    {
        pScdInstance->SegFullFlag = 0;
    }

    pUsedSeg = pScdInstance->TransFormSeg;
    /*�ϱ�ͷ�ĸ�������С��������Ϣ�صĸ���*/
    if (ScdSegNum < (((SINT32)RD_SCDREG(REG_UP_LEN))/UpMsgStepNum))
    {
        ScdSegNum = ScdSegNum - 1;
        //SrcEaten = pUsedSeg[ScdSegNum*UpMsgStepNum + 1];   // zhl, 20081230        
        #ifndef REP_SCD_ADDR_BUG
        SrcEaten = pUsedSeg[ScdSegNum*UpMsgStepNum + 1] & 0x7FFFFFFF;
        #else
        SrcEaten = pUsedSeg[ScdSegNum*UpMsgStepNum + 1] & 0x3FFFFFFF;
        #endif
    }
    else
    {
        /*����Ϊ�˹��Ӳ����һ������,����Ϣ����ʱ,���һ����Ϣ�ز�����*/
        ScdSegNum = ((RD_SCDREG(REG_UP_LEN) - UpMsgStepNum)/UpMsgStepNum) - 1;
        if (ScdSegNum < 0)
        {
            ScdSegNum = 0;
        }
        //SrcEaten = pScdInstance->SegFilter.pScdUpMsg[ScdSegNum*UpMsgStepNum + 1];
        #ifndef REP_SCD_ADDR_BUG
        SrcEaten = (pScdInstance->SegFilter.pScdUpMsg[ScdSegNum*UpMsgStepNum + 1]) & 0x7FFFFFFF;
        #else
        SrcEaten = (pScdInstance->SegFilter.pScdUpMsg[ScdSegNum*UpMsgStepNum + 1]) & 0x3FFFFFFF;
        #endif
    }

    /* ֻ���յ�����vop���Ż��ǰһ֡������seg����������� */
#ifdef SCD_MP4_SLICE_ENABLE	  
    if(VFMW_MPEG4 == pScdInstance->Config.VidStd)
    {
        ScdSegNum = GetNextVop(pScdInstance, pUsedSeg, pStreamSegArray, ScdSegNum);
    }
#endif
    LastSegTail = pStreamSegArray->Tail;
    FilterScdSegNum = ScdSegNum;
      
    for (i = 0; i < ScdSegNum; i++)
    {
        CurrSegPhyAddr  = pUsedSeg[i*UpMsgStepNum];
        NextSegPhyAddr  = pUsedSeg[(i+1)*UpMsgStepNum];
        SegPacket.DspCheckPicOverFlag = ((pUsedSeg[i*UpMsgStepNum+1]) & 0x80000000)>>31; 
        if (SegBufAddr == NextSegPhyAddr)//����ĳ�ƫ�ƵĻ�,��һ�д���Ҫ�ĳ�if (0 == NextSegPhyAddr)
        {
            SegPacketLen = SmStateReg.ScdRollAddr - CurrSegPhyAddr;
        }
        else
        {
            SegPacketLen = NextSegPhyAddr - CurrSegPhyAddr;
        }

        /*��С����3�İ��Ų���*/
        if (SegPacketLen > 3 )
        {
            if(LastSegPacket.PhyAddr != 0)
            {
#ifdef SCD_MP4_SLICE_ENABLE                    	            
                LastSegPacket.LenInByte += SegPacketLen;
                GetFreeSegBuf(pStreamSegArray, &TmpAddr, &SegFreeBufLen);	
                if (FMW_OK == CheckSegValid(&LastSegPacket, SegFreeBufLen))
                {
                    Ret = InsertStreamSeg(pStreamSegArray, &LastSegPacket);
                    memset(&LastSegPacket, 0, sizeof(STREAM_SEG_S));

                    if (FMW_OK != Ret)
                    {
                        //���������λ���룬ֱ��break��û������
                        dprint(PRN_SCD_INFO, "Insert err Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", LastSegPacket.PhyAddr, LastSegPacket.LenInByte, LastSegPacket.IsLastSeg);
                        break;
                    }
                    #ifndef REP_SCD_ADDR_BUG
                    dprint(PRN_SCD_INFO, "RawLen = %d, Pts = %ld, \n", pUsedSeg[i*UpMsgStepNum+1] & 0x7FFFFFFF, LastSegPacket.Pts);
                    #else
                    dprint(PRN_SCD_INFO, "RawLen = %d, Pts = %ld, \n", pUsedSeg[i*UpMsgStepNum+1] & 0x3FFFFFFF, LastSegPacket.Pts);
                    #endif
                    dprint(PRN_SCD_INFO, "Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", LastSegPacket.PhyAddr, LastSegPacket.LenInByte, LastSegPacket.IsLastSeg);
                }			  
                else
                {
                    dprint(PRN_ERROR, "line %d CheckSeg err\n",__LINE__);
                    dprint(PRN_ERROR, "line %d SegFreeBufLen=%d,SrcEaten=%d\n",__LINE__, SegFreeBufLen,SrcEaten);
                    dprint(PRN_ERROR, "line %d Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", __LINE__, LastSegPacket.PhyAddr, LastSegPacket.LenInByte, LastSegPacket.IsLastSeg);
                    ScdErrFlag = 1;
                    break;
                }
#endif			  

            }
            else
            {
                if(VFMW_MPEG4 == pScdInstance->Config.VidStd)
                {
                    ScdParseUpMsgInfo(&pScdInstance->SegFilter, &SegPacket, i,UpMsgStepNum);
                }
                SegPacket.IsLastSeg = 1;
                SegPacket.PhyAddr = CurrSegPhyAddr;//pStreamSegArray->SegBufPhyAddr + CurrSegPhyAddr;        
                SMSEG_PHY2VIR(pStreamSegArray, SegPacket.PhyAddr, SegPacket.VirAddr);
                SegPacket.LenInByte = SegPacketLen;             
                GetFreeSegBuf(pStreamSegArray, &TmpAddr, &SegFreeBufLen);            

                if (FMW_OK == CheckSegValid(&SegPacket, SegFreeBufLen))
                {
                    #ifndef REP_SCD_ADDR_BUG
                    GetSegPts(pRawStreamArray, pUsedSeg[i*UpMsgStepNum+1] & 0x7FFFFFFF,&pScdInstance->LastPts,&SegPacket);
                    #else
                    GetSegPts(pRawStreamArray, pUsedSeg[i*UpMsgStepNum+1] & 0x3FFFFFFF,&pScdInstance->LastPts,&SegPacket);
                    #endif
                    Ret =InsertStreamSeg(pStreamSegArray, &SegPacket);
                    if (FMW_OK != Ret)
                    {
                        //���������λ���룬ֱ��break��û������
                        dprint(PRN_SCD_INFO, "Insert err Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);
                        break;
                    }
                    #ifndef REP_SCD_ADDR_BUG
                    dprint(PRN_SCD_INFO, "RawLen = %d, Pts =  %lld, \n",pUsedSeg[i*UpMsgStepNum+1] & 0x7FFFFFFF, SegPacket.Pts);
                    #else
                    dprint(PRN_SCD_INFO, "RawLen = %d, Pts =  %lld, \n",pUsedSeg[i*UpMsgStepNum+1] & 0x3FFFFFFF, SegPacket.Pts);
                    #endif
                    dprint(PRN_SCD_INFO, "Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);
                }            
                else
                {
                    dprint(PRN_FATAL, "CheckSeg err\n");
                    dprint(PRN_FATAL, "SegFreeBufLen=%d,SrcEaten=%d\n",SegFreeBufLen,SrcEaten);
                    dprint(PRN_FATAL, "Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);
                    ScdErrFlag = 1;
                    break;
                }
            }//������    
        }
        else if ((0 < SegPacketLen) && (SegPacketLen <= 3))
        {	

#ifdef SCD_MP4_SLICE_ENABLE                    	            

            if(VFMW_MPEG4 == pScdInstance->Config.VidStd)
            {
                pTmpVirAddr = (UINT8 *)MEM_Phy2Vir(NextSegPhyAddr);
                FMW_ASSERT_RET((pTmpVirAddr != NULL), FMW_ERR_SCD);

                Data32 = (((SINT32)pTmpVirAddr[0] & 0xff) << 24)
                      |  (((SINT32)pTmpVirAddr[1] & 0xff) << 16)
                      |  (((SINT32)pTmpVirAddr[2] & 0xff) << 8)
                      |  (((SINT32)pTmpVirAddr[3] & 0xff));

                if ((Data32&0xffffff00) != 0x100)
                {
                    ScdParseUpMsgInfo(&pScdInstance->SegFilter, &LastSegPacket, i, UpMsgStepNum);
                    LastSegPacket.IsLastSeg = 1;
                    LastSegPacket.PhyAddr = CurrSegPhyAddr;//pStreamSegArray->SegBufPhyAddr + CurrSegPhyAddr;        
                    SMSEG_PHY2VIR(pStreamSegArray, LastSegPacket.PhyAddr, LastSegPacket.VirAddr);
                    LastSegPacket.LenInByte = SegPacketLen;
                    GetFreeSegBuf(pStreamSegArray, &TmpAddr, &SegFreeBufLen);
                    #ifndef REP_SCD_ADDR_BUG
                    GetSegPts(pRawStreamArray, pUsedSeg[i*UpMsgStepNum+1] & 0x7FFFFFFF,&pScdInstance->LastPts,&LastSegPacket);
                    #else
                    GetSegPts(pRawStreamArray, pUsedSeg[i*UpMsgStepNum+1] & 0x3FFFFFFF,&pScdInstance->LastPts,&LastSegPacket);
                    #endif
                }
            }
#endif			
        }
        else if (SegPacketLen < 0)
        {
            /*ֻҪ���ƣ���Ҫ��ȥ��Buffer��û������ĵ�ַ*/ 
            if(VFMW_MPEG4 == pScdInstance->Config.VidStd)
            {
                ScdParseUpMsgInfo(&pScdInstance->SegFilter, &SegPacket, i, UpMsgStepNum);
            }
            SegPacket.IsLastSeg = 0;
            SegPacket.PhyAddr = CurrSegPhyAddr;//SegBufAddr + CurrSegPhyAddr;   
            SMSEG_PHY2VIR(pStreamSegArray, SegPacket.PhyAddr, SegPacket.VirAddr);
            SegPacket.LenInByte = SmStateReg.ScdRollAddr - SegPacket.PhyAddr;
            //��������������
            SegPacket.LenInByte = (NextSegPhyAddr - SegBufAddr) + SegPacket.LenInByte;
            GetFreeSegBuf(pStreamSegArray, &TmpAddr, &SegFreeBufLen);             
            /*����������Ƿ�ϸ�*/
            if (SegPacket.LenInByte > 3)
            {
                if(FMW_OK != GetSegStreamNum(pStreamSegArray, &StreamNum))
                {
                    dprint(PRN_DBG, "%s %d FMW_OK != GetSegStreamNum!!\n",__FUNCTION__,__LINE__);
                }
                StreamNum = MAX_STREAM_SEG_NUM - StreamNum;       
                if ((FMW_OK == CheckSegValid(&SegPacket, SegFreeBufLen))
                        && (StreamNum >= 2))
                {
                    /*�����һ��Seg,��Ҫ����һ�³��Ȳ��ܲ���*/
                    SegPacket.LenInByte = SmStateReg.ScdRollAddr - SegPacket.PhyAddr;
                    #ifndef REP_SCD_ADDR_BUG
                    GetSegPts(pRawStreamArray, pUsedSeg[i*UpMsgStepNum+1] & 0x7FFFFFFF, &pScdInstance->LastPts, &SegPacket);
                    #else
                    GetSegPts(pRawStreamArray, pUsedSeg[i*UpMsgStepNum+1] & 0x3FFFFFFF, &pScdInstance->LastPts, &SegPacket);
                    #endif
                    Ret = InsertStreamSeg(pStreamSegArray, &SegPacket);   
                    if (FMW_OK != Ret)
                    {
                        //���������λ���룬ֱ��break��û������
                        dprint(PRN_SCD_INFO, "Insert err Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);
                        break;
                    }
                    #ifndef REP_SCD_ADDR_BUG
                    dprint(PRN_SCD_INFO,"RawLen = %d, Pts =  %lld, \n",pUsedSeg[i*UpMsgStepNum+1] & 0x7FFFFFFF, SegPacket.Pts);
                    #else
                    dprint(PRN_SCD_INFO,"RawLen = %d, Pts =  %lld, \n",pUsedSeg[i*UpMsgStepNum+1] & 0x3FFFFFFF, SegPacket.Pts);
                    #endif
                    dprint(PRN_SCD_INFO, "Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);

                    SegPacket.IsLastSeg = 1;
                    SegPacket.PhyAddr = SegBufAddr;
                    SMSEG_PHY2VIR(pStreamSegArray, SegPacket.PhyAddr, SegPacket.VirAddr);
                    SegPacket.LenInByte = NextSegPhyAddr - SegPacket.PhyAddr;//���ƿ϶��Ǵ�ͷ����һ���Ŀ�ʼλ��
                    //SegPacket.Pts ���ø�ֵ��ֱ������һ����Pts
                    Ret = InsertStreamSeg(pStreamSegArray, &SegPacket);
                    if (FMW_OK != Ret)
                    {
                        //���������λ���룬ֱ��break��û������
                        dprint(PRN_SCD_INFO, "Insert err Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);
                        break;
                    }
                    dprint(PRN_SCD_INFO, "Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);
                }
                else
                {
                    //���������λ���룬ֱ��break��û������
                    if (StreamNum < 2)
                    {                     
                        dprint(PRN_SCD_INFO, "Insert 2 err Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", SegPacket.PhyAddr, SegPacket.LenInByte,SegPacket.IsLastSeg);
                    }
                    else
                    {
                        dprint(PRN_ERROR, "line %d CheckSeg err\n",__LINE__);
                        dprint(PRN_ERROR, "line %d SegFreeBufLen=%d,SrcEaten=%d",__LINE__, SegFreeBufLen,SrcEaten);
                        dprint(PRN_ERROR, "line %d Scd SegPacket.PhyAddr=0x%x,LenByte=%d,IsLastSeg=%d\n", __LINE__, LastSegPacket.PhyAddr, LastSegPacket.LenInByte, LastSegPacket.IsLastSeg);
                        ScdErrFlag = 1;
                    }
                    break;
                }
            }
        }
    }

    if (pStreamSegArray->Tail != LastSegTail)
    {
        pScdInstance->InsertNewSegFlag = 1;
    }
    else
    {
        pScdInstance->InsertNewSegFlag = 0;
    }

    if (i > 0)
    {
        #ifndef REP_SCD_ADDR_BUG
        SrcEaten = pUsedSeg[i * UpMsgStepNum + 1] & 0x7FFFFFFF;
        #else
        SrcEaten = pUsedSeg[i * UpMsgStepNum + 1] & 0x3FFFFFFF;
        #endif
        Ret = DeleteRawLen( pRawStreamArray, SrcEaten );

        if (FMW_OK != Ret)
        {
            return FMW_ERR_SCD;
        }
    }     
    else
    {
#ifdef SCD_MP4_SLICE_ENABLE
          if((ReadScdSegNum > MAX_STREAM_SEG_NUM) 
		  && (FilterScdSegNum == 0) 
		  && (VFMW_MPEG4 == pScdInstance->Config.VidStd))
          {
              dprint(PRN_FATAL,"ReadScdSegNum = %d, FilterScdSegNum = 0 release raw packet anyway!\n",
                ReadScdSegNum);
              DeleteRawPacket(pRawStreamArray, 1);
          }

#endif

    }

    return ((1 == ScdErrFlag) ? FMW_ERR_SCD : FMW_OK); 
}

/************************************************************************
  ԭ��    VOID FillRawData(SINT32 InstID)
  ����    ��ȡ�����������������
  ����    InstID    ͨ��ʵ��ID
  ����ֵ  ��
 ************************************************************************/
VOID FillRawData(SINT32 InstID)
{
    STREAM_DATA_S  RawPacket;
    UINT32  ThisTime, TimePeriod;
    static UINT32 last_rec_pos_time[MAX_CHAN_NUM] = {0};
    SINT32 rec_pos_print_flag = 0;
    UINT32 dat = 0;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[InstID];

    memset(&RawPacket, 0, sizeof(STREAM_DATA_S));
    if (NULL == pSmInstArray)
    {
        return;
    }

    while (1)
    {
        if (1 == pSmInstArray->Config.IsOmxPath) // For Omx Low Frequency Get Stream
        {
            /* OMXͨ·����seg�����������²�ȡ���������� y00226912 */
            if (1 == VCTRL_IsChanSegEnough(InstID))
            {
                break;
            }
        }
        if ( FMW_OK == GetRawState(InstID) )
        {
            /* ���Խ����µ�raw���� */
            if (FMW_OK != ReadRawStreamData( InstID, &RawPacket))
            {
                break;
            }

            /* ͳ������ */
            pSmInstArray->TotalRawSize    += RawPacket.Length;
            pSmInstArray->u32RawNumCount++;
            pSmInstArray->u32RawSizeCount += RawPacket.Length;
            ThisTime = VFMW_OSAL_GetTimeInMs();
            if (pSmInstArray->LastStatTime == 0)
            {
                pSmInstArray->LastStatTime = ThisTime;
            }
            else
            {
                TimePeriod = ThisTime - pSmInstArray->LastStatTime;
                if (TimePeriod > 2000)
                {
                    pSmInstArray->BitRate = pSmInstArray->TotalRawSize * 8 / TimePeriod * 1000 / 1024;
                    pSmInstArray->TotalRawSize = 0;
                    pSmInstArray->LastStatTime = ThisTime;
                }
            }

            /* ����raw�����뵽����raw���� */
            dprint(PRN_PTS,"raw_pts: %lld\n", RawPacket.Pts);
            dprint(PRN_SCD_INFO, "Get RawPhyAddr=0x%x,Len=%d\n", RawPacket.PhyAddr, RawPacket.Length);
            if (RawPacket.Pts == (UINT32)(-1))
            {
                RawPacket.Pts = (UINT64)(-1);
            }

            if (FMW_OK != SM_PushRaw(InstID, &RawPacket))
            {
                dprint(PRN_FATAL, "!!!push raw err!!!\n");
                break;
            }
            else
            {
                pSmInstArray->FillNewRawFlag = 1;
            #ifdef VFMW_SCD_LOWDLY_SUPPORT
                DBG_CountTimeInfo(InstID, STATE_RCV_RAW, 0);
            #endif
                if (CHECK_REC_POS_ENABLE(STATE_RCV_RAW))
                {
                    ThisTime = VFMW_OSAL_GetTimeInMs();	
                    if ((ThisTime - last_rec_pos_time[InstID]) > g_TraceBsPeriod)
                    {
                        rec_pos_print_flag = 1;
                        last_rec_pos_time[InstID] = ThisTime;
                    }
                    else
                    {
                        rec_pos_print_flag = 0;
                    }

                    if (rec_pos_print_flag == 1)
                    {
                        dat = (InstID<<24) + (STATE_RCV_RAW<<16);
                        REC_POS(dat);
                    }
                }
            }

        #ifdef ENV_ARMLINUX_KERNEL
            if (InstID == save_str_chan_num && NULL != vfmw_save_str_file)
            {
                VID_STD_E eStd = pSmInstArray->Config.VidStd;
                VFMW_OSAL_SpinUnLock(G_SPINLOCK_THREAD);
                /* ĳЩЭ��������֡���棬֡ǰ�ӳ��� */
                if (eStd == VFMW_VP6 || eStd == VFMW_VP6F || eStd == VFMW_VP6A || eStd == VFMW_VP8 
                        || eStd == VFMW_H263 || eStd == VFMW_SORENSON || eStd == VFMW_RAW
                        || eStd == VFMW_REAL8 || eStd == VFMW_REAL9 || eStd == VFMW_DIVX3
                        || (eStd == VFMW_VC1 && 1 != pSmInstArray->Config.StdExt.Vc1Ext.IsAdvProfile) )
                {
                    OSAL_FileWrite((char *)&RawPacket.Length, 4, vfmw_save_str_file);
                }                
                OSAL_FileWrite((char *)RawPacket.VirAddr, RawPacket.Length, vfmw_save_str_file);
                VFMW_OSAL_SpinLock(G_SPINLOCK_THREAD);
            }
        #endif

        }
        else
        {
            /* ��ʵ��raw�������� */
            dprint(PRN_BLOCK, "Chan%d raw full\n", InstID);
            PrintScdRawState(s_SmIIS.ThisInstID, 0);
            break;
        }
    }

    return;
}

SINT32 FillEosRawStream(SINT32 InstID, STREAM_DATA_S *pRawPacket)
{
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[InstID];
    
    UINT8 au8EndFlag[5][20] = 
    {
        /* H264 */
        {
            0x00, 0x00, 0x01, 0x0b, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45,
            0x4e, 0x44, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        /* VC1ap,AVS */
        {
            0x00, 0x00, 0x01, 0xfe, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45,
            0x4e, 0x44, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        /* MPEG4 */
        {
            0x00, 0x00, 0x01, 0xfe, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45,
            0x4e, 0x44, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x01, 0x00
        },
        /* MPEG2 */
        {
            0x00, 0x00, 0x01, 0xb7, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45,
            0x4e, 0x44, 0x00, 0x00, 0x01, 0xb7, 0x00, 0x00, 0x00, 0x00
        },
        /* H265 */
        {
            0x00, 0x00, 0x01, 0x62, 0x00, 0x48, 0x53, 0x50, 0x49, 0x43,
            0x45, 0x4e, 0x44, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00
        },
    };
    UINT8* pu8Flag = NULL;
    UINT32 u32FlagLen = 0;

    if (NULL == pRawPacket)
    {
        dprint(PRN_FATAL, "%s: pRawPacket = NULL!\n", __func__);
        return FMW_ERR_SCD;
    }

    /* CodecType Relative */
    switch (pSmInstArray->Config.VidStd)
    {
        case STD_HEVC:
            pu8Flag = au8EndFlag[4];
            u32FlagLen = 16;
            break;
            
        case STD_H264:
            pu8Flag = au8EndFlag[0];
            u32FlagLen = 15;
            break;
            
        case STD_MVC:
            pu8Flag = au8EndFlag[0];
            u32FlagLen = 15;
            break;

        case STD_AVS:
            pu8Flag = au8EndFlag[1];
            u32FlagLen = 15;
            break;

        case STD_MPEG4:
            pu8Flag = au8EndFlag[2];
            u32FlagLen = 19;
            break;

        case STD_MPEG2:
            pu8Flag = au8EndFlag[3];
            u32FlagLen = 16;
            break;

        case STD_VC1:
            if (1 == pSmInstArray->Config.StdExt.Vc1Ext.IsAdvProfile)
            {
                pu8Flag = au8EndFlag[1];
                u32FlagLen = 15;
            }
            break;

        case STD_H263:
        case STD_DIVX3:
        case STD_REAL8:
        case STD_REAL9:
        case STD_VP6:
        case STD_VP6F:
        case STD_VP6A:
        case STD_VP8:
        case STD_SORENSON:
            break;

        default:
            dprint(PRN_FATAL, "%s: unkown standard type = %d!\n", __func__, pSmInstArray->Config.VidStd);
            return FMW_ERR_SCD;
    }
    
    if (u32FlagLen > pRawPacket->Length)
    {
        dprint(PRN_FATAL, "%s: FlagLen(%d) > RawPacketLength(%d)\n", __func__, u32FlagLen, pRawPacket->Length);
        return FMW_ERR_SCD;
    }

    if (u32FlagLen != 0)
    {
        memcpy(pRawPacket->VirAddr, pu8Flag, u32FlagLen);
        pRawPacket->Length = u32FlagLen;
    }

    return FMW_OK;
}

SINT32 FillExtraData(SINT32 InstID, STREAM_DATA_S *pRawPacket)
{
    VID_STD_E     VidStd;
    SM_INSTANCE_S *pSmInstArray = NULL; 
    
    FMW_ASSERT_RET((InstID >= 0) && (InstID < SM_MAX_SMID), FMW_ERR_PARAM);
    FMW_ASSERT_RET(pRawPacket != NULL, FMW_ERR_PARAM);
    
    if (pRawPacket->Length <= 0)
    {
        dprint(PRN_ERROR, "%s: RawPacket Length %d <= 0\n", __func__, pRawPacket->Length);
        return FMW_ERR_PARAM;
    }
    
    pSmInstArray = s_SmIIS.pSmInstArray[InstID];
    VidStd = pSmInstArray->Config.VidStd;
    
    switch(VidStd)
    {
        case STD_VC1:
            if (1 == pSmInstArray->Config.StdExt.Vc1Ext.IsAdvProfile)
            {
                if (pRawPacket->RawExt.Flags & VDEC_BUFFERFLAG_CODECCONFIG)
                {
                    if (pRawPacket->Length > HEADER_BUFFER_SIZE)
                    {
                        dprint(PRN_ERROR, "%s(): Std(%d) buffer has no enough space,(act:%d, header_buf:%d)\n", 
                                          __func__, VidStd, pRawPacket->Length, HEADER_BUFFER_SIZE);
                        return FMW_ERR_NOMEM;
                    }
                    else
                    {
                        /*����ǰ��������*/
                        pSmInstArray->HeaderLen = pRawPacket->Length;
                        memcpy(pSmInstArray->HeaderBuf, pRawPacket->VirAddr, pRawPacket->Length);
                    }
                }
                else if (pRawPacket->RawExt.IsSeekPending)
                {
                    if (pRawPacket->Length+pSmInstArray->HeaderLen > pRawPacket->RawExt.BufLen)
                    {
                        dprint(PRN_ERROR, "%s(): Std(%d) buffer has no enough space,(act:%d, str_buf:%d)\n", 
                                          __func__, VidStd, pRawPacket->Length+pSmInstArray->HeaderLen, pRawPacket->RawExt.BufLen);
                        return FMW_ERR_NOMEM;
                    }
                    else
                    {
                        /*����ǰ��������*/
                        memmove((pRawPacket->VirAddr+pSmInstArray->HeaderLen), pRawPacket->VirAddr, pRawPacket->Length);
                        memcpy(pRawPacket->VirAddr, pSmInstArray->HeaderBuf, pSmInstArray->HeaderLen);
                        pRawPacket->Length += pSmInstArray->HeaderLen;
                    }
                }
            }
            else
            {
                if (pRawPacket->RawExt.Flags & VDEC_BUFFERFLAG_CODECCONFIG)
                {
                    if (pRawPacket->Length+12 > pRawPacket->RawExt.BufLen || pRawPacket->Length+12 > HEADER_BUFFER_SIZE)
                    {
                        dprint(PRN_ERROR, "%s(): Std(%d) buffer has no enough space,(act:%d, str_buf:%d, header_buf:%d)\n", 
                                          __func__, VidStd, pRawPacket->Length+12, pRawPacket->RawExt.BufLen, HEADER_BUFFER_SIZE);
                        return FMW_ERR_NOMEM;
                    }
                    else
                    {
                        /*���Ͽ��*/
                        memmove((pRawPacket->VirAddr+8), pRawPacket->VirAddr, pRawPacket->Length);
                        memcpy(pRawPacket->VirAddr, &pRawPacket->RawExt.CfgWidth, 4);
                        memcpy((pRawPacket->VirAddr+4), &pRawPacket->RawExt.CfgHeight, 4);
                        /*��׺����*/
                        memset((pRawPacket->VirAddr+12), 0, 4);

                        pRawPacket->Length = 16;
                        /*����ǰ��������*/
                        pSmInstArray->HeaderLen = pRawPacket->Length;
                        memcpy(pSmInstArray->HeaderBuf, pRawPacket->VirAddr, pRawPacket->Length);
                    }
                }
                else
                {
                    if (pRawPacket->Length+pSmInstArray->HeaderLen > pRawPacket->RawExt.BufLen)
                    {
                        dprint(PRN_ERROR, "%s(): Std(%d) buffer has no enough space,(act:%d, str_buf:%d)\n", 
                                          __func__, VidStd, pRawPacket->Length+pSmInstArray->HeaderLen, pRawPacket->RawExt.BufLen);
                        return FMW_ERR_NOMEM;
                    }
                    else
                    {
                        /*����ǰ��������*/
                        memmove((pRawPacket->VirAddr+pSmInstArray->HeaderLen), pRawPacket->VirAddr, pRawPacket->Length);
                        memcpy(pRawPacket->VirAddr, pSmInstArray->HeaderBuf, pSmInstArray->HeaderLen);
                        pRawPacket->Length += pSmInstArray->HeaderLen;
                    }
                }
            }
            break;

        case STD_DIVX3: 
            if (pRawPacket->Length+8 > pRawPacket->RawExt.BufLen)
            {
                dprint(PRN_ERROR, "%s(): Std(%d) buffer has no enough space, (act:%d, str_buf:%d)\n", 
                                  __func__, VidStd, pRawPacket->Length+8, pRawPacket->RawExt.BufLen);
                return FMW_ERR_NOMEM;
            }
            else
            {
                //���Ͽ��
                memmove((pRawPacket->VirAddr+8), pRawPacket->VirAddr, pRawPacket->Length);
                memcpy(pRawPacket->VirAddr, &pRawPacket->RawExt.CfgWidth, 4);
                memcpy((pRawPacket->VirAddr+4), &pRawPacket->RawExt.CfgHeight, 4);
                pRawPacket->Length += 8;
            }
            break;

        case STD_REAL8:
            if (pRawPacket->RawExt.Flags & VDEC_BUFFERFLAG_CODECCONFIG)
            {
                if (pRawPacket->Length > HEADER_BUFFER_SIZE)
                {
                    dprint(PRN_ERROR, "%s(): Std(%d) buffer has no enough space,(act:%d, header_buf:%d)\n", 
                                      __func__, VidStd, pRawPacket->Length, HEADER_BUFFER_SIZE);
                    return FMW_ERR_NOMEM;
                }
                else
                {
                    /*����ǰ��������*/
                    pSmInstArray->HeaderLen = pRawPacket->Length;
                    memcpy(pSmInstArray->HeaderBuf, pRawPacket->VirAddr, pRawPacket->Length);
                }
            }
            else
            {
                if (pRawPacket->Length+pSmInstArray->HeaderLen > pRawPacket->RawExt.BufLen)
                {
                    dprint(PRN_ERROR, "%s(): Std(%d) buffer has no enough space,(act:%d, str_buf:%d)\n", 
                                      __func__, VidStd, pRawPacket->Length+pSmInstArray->HeaderLen, pRawPacket->RawExt.BufLen);
                    return FMW_ERR_NOMEM;
                }
                else
                {
                    /*����ǰ��������*/
                    memmove((pRawPacket->VirAddr+pSmInstArray->HeaderLen), pRawPacket->VirAddr, pRawPacket->Length);
                    memcpy((pRawPacket->VirAddr), pSmInstArray->HeaderBuf, pSmInstArray->HeaderLen);
                    pRawPacket->Length += pSmInstArray->HeaderLen;
                }
            }
            break;

        default:
            break;
    }

    return FMW_OK;
}

SINT32 ReadRawStreamData(SINT32 InstID, STREAM_DATA_S * pRawPacket)
{
    STREAM_INTF_S *pStreamIntf;
    STREAM_INTF_S  StreamIntf;
    SINT32 ret = FMW_ERR_SCD;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[InstID];

    memset(&StreamIntf, 0, sizeof(STREAM_INTF_S));
    if (NULL == pSmInstArray)
    {
        return FMW_ERR_SCD;    
    }

    pStreamIntf = VCTRL_GetStreamInterface(InstID);
    if ((NULL == pStreamIntf))
    {
        dprint(PRN_FATAL, "Chan %d has been destroyed or the function pointer -- ReadRawStreamData pStreamIntf is NULL!", InstID);
        return ret;
    }

    memcpy(&StreamIntf, pStreamIntf, sizeof(STREAM_INTF_S));
    if (NULL != pStreamIntf->read_stream)
    {
        ret = StreamIntf.read_stream( StreamIntf.stream_provider_inst_id, pRawPacket);
        ret = (0==ret)? FMW_OK: FMW_ERR_SCD;
    }

    if (ret == FMW_OK)
    {
        if (1 == pRawPacket->is_stream_end_flag)
        {
            if (FillEosRawStream(InstID, pRawPacket) != FMW_OK)
            {
                dprint(PRN_FATAL, "Chan %d FillEosRawStream failed!\n", InstID);
                return FMW_ERR_SCD;    
            }
            VCTRL_SetLastFrameState(InstID, LAST_FRAME_RECEIVE);
        }
        else
        {
            /* Fill extra data in omx path & normal dec mode */
            if (1 == s_SmIIS.pSmInstArray[InstID]->Config.IsOmxPath && 1 != s_SmIIS.pSmInstArray[InstID]->Config.SpecMode)
            {
                /* OMXͨ·����Э�������Ҫ���Ӷ������Ϣ */
                if (FillExtraData(InstID, pRawPacket) != FMW_OK)
                {
                    dprint(PRN_FATAL, "Chan %d FillExtraData failed!\n", InstID);
                    ret = ReleaseRawStreamData( StreamIntf.stream_provider_inst_id, pRawPacket);
                    if (ret != FMW_OK)
                    {
                        dprint(PRN_FATAL, "Release this packet failed!\n");
                    }
                    return FMW_ERR_SCD;    
                }
            }
        }
        pSmInstArray->RawPacketArray.RawTotalSize += pRawPacket->Length;
    }

    return ret;
}

SINT32 ReleaseRawStreamData(SINT32 InstID, STREAM_DATA_S * pRawPacket)
{
    STREAM_INTF_S *pStreamIntf;
    STREAM_INTF_S StreamIntf;
    SINT32 ret = FMW_ERR_SCD;
    pStreamIntf = VCTRL_GetStreamInterface(InstID);

    memset(&StreamIntf, 0, sizeof(STREAM_INTF_S));
    if (pStreamIntf == NULL)
    {
        dprint(PRN_FATAL, "Chan %d has been destroyed or the function pointer -- ReleaseRawStreamData pStreamIntf is NULL!", InstID);
        return FMW_ERR_SCD;
    }

    memcpy(&StreamIntf, pStreamIntf, sizeof(STREAM_INTF_S));
    if( NULL != pStreamIntf->release_stream )
    {
        ret = StreamIntf.release_stream( StreamIntf.stream_provider_inst_id, pRawPacket);
        ret = (0==ret)? FMW_OK: FMW_ERR_SCD;
    }

    if (NULL == s_SmIIS.pSmInstArray[InstID])
    {
        dprint(PRN_FATAL, "line: %d s_SmIIS.pSmInstArray[%d] is NULL\n", __LINE__,InstID);
        return FMW_ERR_SCD;
    }

    return ret;
}

SINT32 SM_MoveRawData(SM_INSTANCE_S *pSmInstArray)
{
    SINT32 LastPos=-1, PrePos=-1;
    STREAM_DATA_S *pLastRawPacket=NULL, *pPreRawPacket=NULL;

    LastPos = pSmInstArray->RawPacketArray.Tail-1;
    LastPos = (LastPos < 0)? LastPos+MAX_STREAM_RAW_NUM: LastPos;
    PrePos  = pSmInstArray->RawPacketArray.Tail-2;
    PrePos  = (PrePos < 0)? PrePos+MAX_STREAM_RAW_NUM: PrePos;

    pLastRawPacket = &pSmInstArray->RawPacketArray.RawPacket[LastPos];
    pPreRawPacket  = &pSmInstArray->RawPacketArray.RawPacket[PrePos];

    if (pPreRawPacket->Length+pLastRawPacket->Length <= pSmInstArray->Config.MaxRawPacketSize)
    {
        memcpy(pPreRawPacket->VirAddr+pPreRawPacket->Length, pLastRawPacket->VirAddr, pLastRawPacket->Length);
        pPreRawPacket->Length += pLastRawPacket->Length;

        ReleaseRawStreamData(pSmInstArray->InstID, pLastRawPacket);
        pSmInstArray->RawPacketArray.Tail = LastPos;
    }
    else
    {
        dprint(PRN_ERROR,"No space for raw move, total:%d+%d > max:%d!\n", pPreRawPacket->Length,pLastRawPacket->Length,pSmInstArray->Config.MaxRawPacketSize);
        return FMW_ERR_SCD;
    }

    return FMW_OK;
}


/************************************************************************
  ԭ��    SINT32 SM_DoNextInstance ( SINT32 *pNextInstId )
  ����    ����������SM������ʵ������һ�����Է����ʵ����������SCDΪ���и�����
  ����    ��  
  ����ֵ  ����ҵ����Է����ʵ��������FMW_OK
  ���򷵻ش�����
 ************************************************************************/
SINT32 SM_DoNextInstance( SINT32 *pNextInstId )
{
    /* ����ɾ��CurInstID���� */
    SINT32 i, CurInstID, NextInstId;
    SINT32 Ret;
    SINT32 WaitSegTime;
    /* stInstInfoӦ�ó�ʼ�� */
    SM_INST_INFO_S  stInstInfo;
    /* memset(&stInstInfo, 0, sizeof(SM_INST_INFO_S)); */
    SINT32 FreshNum=0, FreshSize=0, SegBufTotalSize = 0;
    UADDR SegBufAddr = 0 ;
    SINT32 CurTimeInMs, WaitTime;
    static UINT32 last_rec_pos_time[MAX_CHAN_NUM] = {0};
    UINT32 dat = 0;
    SM_INSTANCE_S *pSmInstArray = NULL;

    /* ����SCD���� */
    if (s_eScdDrvSleepStage != SCDDRV_SLEEP_STAGE_NONE)
    {
        if (s_eScdDrvSleepStage == SCDDRV_SLEEP_STAGE_PREPARE)
        {
            s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_SLEEP;
        }
        dprint(PRN_ERROR, "s_eScdDrvSleepStage = SCDDRV_SLEEP_STAGE_SLEEP!\n");
        return FMW_ERR_NOTRDY;
    }

    /* ѡ��һ��ͨ����׼����һ���и� */
    CurInstID  = s_SmIIS.ThisInstID;
    NextInstId = CurInstID;
    for ( i = 0; i < MAX_CHAN_NUM; i++ )
    {
        NextInstId = (NextInstId+1) % MAX_CHAN_NUM;

        /* �����ʵ��δ���������һ�� */
        pSmInstArray = s_SmIIS.pSmInstArray[NextInstId];

        (VOID)SM_GetInfo( NextInstId, &stInstInfo );
        if ( SM_INST_MODE_IDLE == stInstInfo.InstMode )
        {
            continue;
        }

        /* �����ʵ���Ѽ��� */
        if ( (VFMW_REAL8 == pSmInstArray->Config.VidStd)    ||
             (VFMW_REAL9 == pSmInstArray->Config.VidStd)    ||
             (VFMW_DIVX3 == pSmInstArray->Config.VidStd)    ||
             (VFMW_VC1   == pSmInstArray->Config.VidStd && pSmInstArray->Config.StdExt.Vc1Ext.IsAdvProfile == 0) ||
             (VFMW_VP6F  == pSmInstArray->Config.VidStd)    ||
             (VFMW_VP6A  == pSmInstArray->Config.VidStd)    ||
             (VFMW_VP6   == pSmInstArray->Config.VidStd)    ||
             (VFMW_VP8   == pSmInstArray->Config.VidStd))
        {
            continue;
        }

        PrintScdRawState(NextInstId, 0);
        PrintScdSegState(NextInstId, 0);

        CurTimeInMs = VFMW_OSAL_GetTimeInMs();
        WaitTime =  CurTimeInMs - pSmInstArray->LastSCDStartTime;		
        if (WaitTime < 0)
        {
            WaitTime = 200;
            pSmInstArray->LastSCDStartTime = CurTimeInMs;
        }

        if ((pSmInstArray->LastCfgAllRawFlag == 1)
		 && (pSmInstArray->FillNewRawFlag == 0)   
	     && (WaitTime < 500))
        {
            continue;
        }
        
        pSmInstArray->LastSCDStartTime = CurTimeInMs;

        if ( 1 == pSmInstArray->SegFullFlag )   // seg�ͷűȽ�����Ҫ�����˶���ʱ��
        {
            PrintScdRawState(NextInstId, 0);
            PrintScdSegState(NextInstId, 0);

            GetFreshSegStream(&pSmInstArray->StreamSegArray, &FreshNum, &FreshSize);
            GetSegBuf(&pSmInstArray->StreamSegArray, &SegBufAddr, &SegBufTotalSize);

            if ( 0 == pSmInstArray->IsCounting )
            {  
                if (FreshSize < (SegBufTotalSize / 2) && FreshNum < 128)
                {
                    // ֮ǰ��û�еȴ����ո�������
                    pSmInstArray->IsCounting = 1;
                    CalcInstBlockTime( NextInstId, 1 );
                }
            }
            else
            {  
                // ��ǰ��������״̬��Ҫ�������˶�ã��Լ�seg buf�Ƿ��Ѿ��γ�����
                WaitSegTime = CalcInstBlockTime( NextInstId, 0 );
                #ifdef RELEASE_LONG_TIME_SEG
                if ( (WAIT_SEG_REL_MAX_TIME < WaitSegTime) && (FreshSize < (SegBufTotalSize / 4)) && (FreshNum < 64))
                {
                    dprint(PRN_FATAL,"long time(%d ms) not release stream seg, release anyway!\n", WaitSegTime );
                    // �Ѿ���ʱ��,ǿ���ͷ�һ������...
                    ReleaseStreamSeg( &pSmInstArray->StreamSegArray, 0xffffffff );
                    pSmInstArray->IsCounting  = 0;
                    pSmInstArray->SegFullFlag = 0;
                }
                #endif
            }
        }
        else
        {
            pSmInstArray->IsCounting = 0;
        }

        /* �����и� */
        Ret = CutStreamWithSCD( pSmInstArray );
        if( FMW_ERR_SEGFULL == Ret )
        {
            pSmInstArray->SegFullFlag = 1;
        }
        else
        {
            pSmInstArray->SegFullFlag = 0;
        }

        if( FMW_OK == Ret )
        {    
            // �Ѿ���SCDͶ�뵽һ��ʵ����
            *pNextInstId = NextInstId;
            SetInstMode(NextInstId, SM_INST_MODE_WORK);
        #ifdef VFMW_SCD_LOWDLY_SUPPORT
            DBG_CountTimeInfo(NextInstId, STATE_SCD_START, pSmInstArray->Config.ScdLowdlyEnable);
        #endif
            if (CHECK_REC_POS_ENABLE(STATE_SCD_START))
            {
                CurTimeInMs = VFMW_OSAL_GetTimeInMs();	
                if ((CurTimeInMs - last_rec_pos_time[NextInstId]) > g_TraceBsPeriod)
                {
                    last_rec_pos_time[NextInstId] = CurTimeInMs;
                    dat = (NextInstId<<24) + (STATE_SCD_START<<16);
                    REC_POS(dat);
                }
            }

            return FMW_OK;
        }
    }

    return FMW_ERR_NOTRDY;
}

/************************************************************************
  ԭ��    VOID SM_SCDIntServeProc ( VOID )
  ����    ������ΪSCD���жϷ��������ӦSCD���ж��źŲ������䷵�ص���Ϣ��
  ����    ��  
  ����ֵ  ��
 ************************************************************************/
VOID SM_SCDIntServeProc ( VOID )
{
    SINT32 NextInstID;
    SINT32 SmID;
    SINT32 dat = 0;
    UINT32 CurTimeInMs = 0;
    SM_INSTANCE_S *pSmInstArray;
    static UINT32 last_rec_pos_time[MAX_CHAN_NUM] = {0};
    SINT32 Ret;
    SINT32 CurRawNum = 0;

#ifndef SCD_BUSY_WAITTING   
    VFMW_OSAL_SpinLock(G_SPINLOCK_THREAD); 
    dat = RD_SCDREG(REG_SCD_OVER) & 0x01;
    if( (dat & 1) == 0 )
    {
        VFMW_OSAL_SpinUnLock(G_SPINLOCK_THREAD);
        return;
    }
#endif

    /* ���SCD�ж� */
    WR_SCDREG(REG_SCD_INI_CLR, 1);

    s_SCDInISR = 1;

    if (CHECK_REC_POS_ENABLE(STATE_SCD_INTERRUPT))
    {
        CurTimeInMs = VFMW_OSAL_GetTimeInMs();	
        if ((CurTimeInMs - last_rec_pos_time[s_SmIIS.ThisInstID]) > g_TraceBsPeriod)
        {
            last_rec_pos_time[s_SmIIS.ThisInstID] = CurTimeInMs;
            dat = (s_SmIIS.ThisInstID<<24) + (STATE_SCD_INTERRUPT<<16);
            REC_POS(dat);
        }
    }
    SmID = s_SmIIS.ThisInstID;
    pSmInstArray = s_SmIIS.pSmInstArray[SmID];

    /* ������һ */
    if( 0 == s_SmIIS.IsScdDrvOpen || NULL == pSmInstArray)
    {
        s_SCDInISR = 0;
        s_SmIIS.SCDState = 0;
#ifndef SCD_BUSY_WAITTING 
        VFMW_OSAL_SpinUnLock(G_SPINLOCK_THREAD);
#endif
        return;
    }

    /* ��������µ�SCD�ж�ʱ��s_SmIIS.SCDStateһ������1 */
    if( 1 == s_SmIIS.SCDState )
    {
        /* ������ͨ����ͣ�����򲻴���SCD��������Ϣ */
        if( pSmInstArray->Mode == SM_INST_MODE_WORK )
        {
            PrintScdRawState(s_SmIIS.ThisInstID, 0);
            PrintScdSegState(s_SmIIS.ThisInstID, 0);
#ifdef VFMW_SCD_LOWDLY_SUPPORT
            DBG_CountTimeInfo(s_SmIIS.ThisInstID, STATE_SCD_INTERRUPT, 0);
#endif
            (VOID)ProcessScdReturn( pSmInstArray );

            /* ����: ��ΪOMXͨ·�����������̶��������еİ�������������VFMW���ͷţ��ϲ�Ҳ���ܼ������������½���ֹͣ��
               �ж�: �ô��и��������������������������и�������ͬ����������δ���ͷš�
               �ݴ�: ���һ�����ݿ��������ڶ����У��ͷ����һ�������޷�������ǿ�Ƹ�λSCD */
            if (1 == pSmInstArray->Config.IsOmxPath && 0 == gIsFPGA)
            {
                Ret  = GetRawStreamNum(&pSmInstArray->RawPacketArray, &CurRawNum);
                if(Ret !=FMW_OK)
                {
                    dprint(PRN_ERROR,"SCD GetRawStreamNum Fail.\n");
                }
                if (1 == pSmInstArray->LastCfgAllRawFlag
                 && CurRawNum == pSmInstArray->Config.MaxRawPacketNum)
                {
                    if (SM_MoveRawData(pSmInstArray) != FMW_OK)
                    {
                        dprint(PRN_ERROR,"Move raw data failed, force to reset scd.\n");
                        SM_Reset(pSmInstArray->InstID);
                    }
                }
            }
            
            PrintScdRawState(s_SmIIS.ThisInstID, 0);
            PrintScdSegState(s_SmIIS.ThisInstID, 0);
        }
        s_SmIIS.SCDState = 0;  // ��־SCD����ֵ�Ѿ��������
        if( FMW_OK == SM_DoNextInstance( &NextInstID ) )
        {
            s_SmIIS.ThisInstID = NextInstID;
            s_SmIIS.SCDState   = 1;  // ���SCD���ڴ���
            s_SmIIS.LastProcessTime = VFMW_OSAL_GetTimeInMs();
        }
    }

    s_SCDInISR = 0;
    SM_GiveThreadEvent(SmID);
#ifndef SCD_BUSY_WAITTING 
    VFMW_OSAL_SpinUnLock(G_SPINLOCK_THREAD);
#endif

    return;
}


SINT32 WaitSCDFinish(VOID)
{
    SINT32 i;

    if ( 1 == s_SmIIS.SCDState )
    {
        for ( i=0; i<SCD_TIME_OUT_COUNT; i++ )
        {
            if ( (RD_SCDREG( REG_SCD_OVER ) & 1) )
            {
                return FMW_OK;
            }
            else
            {
                //OSAL_MSLEEP(10);
            }
        }

        return FMW_ERR_SCD;
    }
    else
    {
        return FMW_OK;
    }
}


/************************************************************************
  ԭ��    VOID SM_Wakeup ( VOID )
  ����    ���������ڻ���SMģ�飬����SMͣ�����޷��ָ����С�
  ��������̼߳��SCD��������߳���ÿ����һ֡���ô˺�����
  ������ö�ʱ�жϼ��SCD�����ڶ�ʱ�ж��е��ô˺�����
  ����    ��  
  ����ֵ  ��
 ************************************************************************/
VOID SM_Wakeup(VOID)
{
    SINT32 NextInstID;
    UINT32 CurTime, DelayTime;
    UINT32 MaxTimeOut = (0 == gIsFPGA)? SCD_TIMEOUT: SCD_FPGA_TIMEOUT; //FPGA wait longer
    
    /* ��ͼ����ͨ����raw���� */
    for ( NextInstID=0; NextInstID < MAX_CHAN_NUM; NextInstID++ )
    {
        if( NULL != s_SmIIS.pSmInstArray[NextInstID] 
                && SM_INST_MODE_WORK == s_SmIIS.pSmInstArray[NextInstID]->Mode )
        {
            /* ���NextInstID��raw���� */
            FillRawData(NextInstID);
        }
    }

    /* ���s_SmIIS.SCDState����1����ʾSCD��δͣ�� */
    if ( 0 == s_SmIIS.SCDState )
    {
        if ( FMW_OK == SM_DoNextInstance( &NextInstID ) )
        {
            s_SmIIS.ThisInstID = NextInstID;
            s_SmIIS.SCDState   = 1;  // ���SCD���ڴ���
            s_SmIIS.LastProcessTime = VFMW_OSAL_GetTimeInMs();
        }
    }
    else
    {
#ifdef SCD_BUSY_WAITTING
        if ( FMW_OK == WaitSCDFinish() )
        {
            SM_SCDIntServeProc();  // Ӳ���Ѿ���ɣ������жϷ����������״̬
            return;
        }
#endif
		
        /* �õȴ�SCD���������Ȳ�����������, ���SCDû�з��أ����������������˳����������� */
        CurTime = VFMW_OSAL_GetTimeInMs();
        DelayTime = CurTime - s_SmIIS.LastProcessTime;
        if (CurTime < s_SmIIS.LastProcessTime)
        {
            DelayTime = 0;
            s_SmIIS.LastProcessTime = CurTime;
        }

        if (DelayTime > MaxTimeOut)
        {
            dprint(PRN_ERROR, "SCD Time Out(%d/%d ms), Reset SCD!\n", DelayTime, MaxTimeOut);
			
            ResetSCD();
            s_SmIIS.SCDState = 0;  //Ӳ��Scd���Թ���
            DeleteLastSendRaw(s_SmIIS.ThisInstID);
            return;
        }
    }

    return;
}


/************************************************************************
  ԭ��    VOID PrintScdVtrlReg()
  ����    ��ӡScd���ƼĴ�������
  ����    
  ����ֵ  ��
 ************************************************************************/
VOID PrintScdVtrlReg()
{

    SM_CTRLREG_S SmCtrlReg = {0};

    SmCtrlReg.DownMsgPhyAddr = RD_SCDREG(REG_LIST_ADDRESS);
    // UP_ADDRESS
    SmCtrlReg.UpMsgPhyAddr = RD_SCDREG(REG_UP_ADDRESS);

    // UP_LEN
    SmCtrlReg.UpLen = RD_SCDREG(REG_UP_LEN);

    // BUFFER_FIRST
    SmCtrlReg.BufferFirst = RD_SCDREG(REG_BUFFER_FIRST);

    // BUFFER_LAST
    SmCtrlReg.BufferLast = RD_SCDREG(REG_BUFFER_LAST);

    // BUFFER_INI
    SmCtrlReg.BufferIni= RD_SCDREG(REG_BUFFER_INI); 

    // SCD_PROTOCOL    
    SmCtrlReg.ScdProtocol = RD_SCDREG(REG_SCD_PROTOCOL);

    // SCD_START
    SmCtrlReg.ScdStart = RD_SCDREG(REG_SCD_START); 

    dprint(PRN_SCD_REGMSG,"***Print Scd Vtrl Reg Now\n");
    dprint(PRN_SCD_REGMSG,"DownMsgPhyAddr = %x\n", SmCtrlReg.DownMsgPhyAddr);
    dprint(PRN_SCD_REGMSG,"UpMsgPhyAddr = %x\n", SmCtrlReg.UpMsgPhyAddr);
    dprint(PRN_SCD_REGMSG,"UpLen = %x\n", SmCtrlReg.UpLen);
    dprint(PRN_SCD_REGMSG,"BufferFirst = %x\n", SmCtrlReg.BufferFirst);
    dprint(PRN_SCD_REGMSG,"BufferLast = %x\n", SmCtrlReg.BufferLast);
    dprint(PRN_SCD_REGMSG,"BufferIni = %x\n", SmCtrlReg.BufferIni);
    dprint(PRN_SCD_REGMSG,"ScdProtocol = %x\n", SmCtrlReg.ScdProtocol);
    dprint(PRN_SCD_REGMSG,"ScdStart = %x\n", SmCtrlReg.ScdStart);

}

/************************************************************************
  ԭ��    VOID PrintDownMsg()
  ����    ��ӡ������Ϣ��
  ����    
  ����ֵ  ��
 ************************************************************************/
VOID PrintDownMsg(UADDR DownMsgPhyAddr, SINT32 * pDownMsgVirAddr, SINT32 PushRawNum)
{
    SINT32 i, j;

    dprint(PRN_SCD_REGMSG,"***Print Down Msg Now\n");
    dprint(PRN_SCD_REGMSG,"DownMsgVirAddr = %p   \n",pDownMsgVirAddr);
    dprint(PRN_SCD_REGMSG,"DownMsgPhyAddr = %x   \n",DownMsgPhyAddr);
    for (i = 0, j = 0; i < PushRawNum; i++)
    {
        dprint(PRN_SCD_REGMSG,"DownMsg[%d] = %x   ",j,pDownMsgVirAddr[j]);
        j++;
        dprint(PRN_SCD_REGMSG,"DownMsg[%d] = %x   ",j,pDownMsgVirAddr[j]);
        j++;
        dprint(PRN_SCD_REGMSG,"DownMsg[%d] = %x   ",j,pDownMsgVirAddr[j]);
        j++;
        dprint(PRN_SCD_REGMSG,"\n");
    }

}

/************************************************************************
  ԭ��    VOID PrintScdStateReg()
  ����    ��ӡScd״̬�Ĵ�������
  ����    
  ����ֵ  ��
 ************************************************************************/
VOID PrintScdStateReg(SM_STATEREG_S *pSmStateReg)
{

    dprint(PRN_SCD_REGMSG, "***Print Scd State Reg\n");
    dprint(PRN_SCD_REGMSG, "Scdover = %d \n",pSmStateReg->Scdover);
    dprint(PRN_SCD_REGMSG, "ScdInt = %d \n",pSmStateReg->ScdInt);
    dprint(PRN_SCD_REGMSG, "ShortScdNum = %d \n",pSmStateReg->ShortScdNum);
    dprint(PRN_SCD_REGMSG, "ScdNum = %d \n",pSmStateReg->ScdNum);
    dprint(PRN_SCD_REGMSG, "ScdRollAddr = %0x \n",pSmStateReg->ScdRollAddr);
    dprint(PRN_SCD_REGMSG, "SrcEaten = %d \n",pSmStateReg->SrcEaten);

}

/************************************************************************
  ԭ��    VOID PrintScdRawState()
  ����    ��ӡRaw���е���ϸ���
  ����    
  ����ֵ  ��
 ************************************************************************/
VOID PrintScdRawState(SINT32 SmID, SINT32 PrintDetail)
{
    SINT32 i,RawNum,RawIndex;
    RAW_ARRAY_S *pRawStreamArray = NULL;

    i = 0;
    RawNum = 0;
    RawIndex = 0;

    if (IsDprintTypeEnable(PRN_SCD_INFO))
    {
        pRawStreamArray = &s_SmIIS.pSmInstArray[SmID]->RawPacketArray;

        dprint(PRN_SCD_INFO, "RawHead=%d\n",pRawStreamArray->Head);
        dprint(PRN_SCD_INFO, "RawTail=%d\n",pRawStreamArray->Tail);
        dprint(PRN_SCD_INFO, "RawHistory=%d\n",pRawStreamArray->History);		
        dprint(PRN_SCD_INFO, "RawFirstPacketOffset=%d\n",pRawStreamArray->FirstPacketOffset);
        dprint(PRN_SCD_INFO, "CurShowIndex=%d\n",pRawStreamArray->CurShowIndex);

        if (1 == PrintDetail)
        {
            if(FMW_OK !=  GetRawStreamNum(pRawStreamArray,&RawNum))
            {
                dprint(PRN_DBG, "%s %d FMW_OK !=  GetRawStreamNum!!\n",__FUNCTION__,__LINE__);
            }
            RawIndex = pRawStreamArray->Head;
            for (i = 0; i < RawNum; i++, RawIndex++)
            {
                dprint(PRN_SCD_INFO, "RawPhyAddr=0x%x\n",pRawStreamArray->RawPacket[RawIndex].PhyAddr);
                dprint(PRN_SCD_INFO, "RawVirAddr=%p\n",pRawStreamArray->RawPacket[RawIndex].VirAddr);
                dprint(PRN_SCD_INFO, "RawLength=%d\n",pRawStreamArray->RawPacket[RawIndex].Length);
                dprint(PRN_SCD_INFO, "RawPts=%lld\n",pRawStreamArray->RawPacket[RawIndex].Pts);      
            }
        }
    }
}


/************************************************************************
  ԭ��    VOID PrintScdRawState()
  ����    ��ӡRaw���е���ϸ���
  ����    
  ����ֵ  ��
 ************************************************************************/
VOID PrintSmInfo(SINT32 SmID)
{
    SINT32 i, idx;
    SM_INST_INFO_S SmInstInfo = {0};
    RAW_ARRAY_S *pstRaw;
    SM_INSTANCE_S *pSmInstArray = s_SmIIS.pSmInstArray[SmID];

    SM_GetInfo(SmID, &SmInstInfo);
    if (NULL == pSmInstArray)
    {
        return;
    }
    dprint(PRN_ALWS, "------------------ s_SmIIS.SmInstArray[%d] --------------------\n",SmID);

    dprint(PRN_ALWS, "%-25s :%d\n", "Mode",pSmInstArray->Mode);
    dprint(PRN_ALWS, "%-25s :%d\n", "Priority",pSmInstArray->Config.Priority);
    dprint(PRN_ALWS, "%-25s :%d\n", "VidStd",pSmInstArray->Config.VidStd);
    dprint(PRN_ALWS, "%-25s :0x%x\n", "BufPhyAddr",pSmInstArray->Config.BufPhyAddr);
    dprint(PRN_ALWS, "%-25s :%p\n", "pBufVirAddr",pSmInstArray->Config.pBufVirAddr);
    dprint(PRN_ALWS, "%-25s :%d\n", "BufSize",pSmInstArray->Config.BufSize);

    dprint(PRN_ALWS, "%-25s :0x%x\n", "ScdRegBaseAddr",   gScdRegBaseAddr);
    dprint(PRN_ALWS, "%-25s :%d\n", "SmPushRaw.PushRawNum",pSmInstArray->SmPushRaw.PushRawNum);
    dprint(PRN_ALWS, "%-25s :%d\n", "SmPushRaw.PushRawTotalLen",pSmInstArray->SmPushRaw.PushRawTotalLen);
    dprint(PRN_ALWS, "%-25s :0x%x\n", "DownMsgPhyAddr",pSmInstArray->BufAddrCfg.DownMsgPhyAddr);
    dprint(PRN_ALWS, "%-25s :%d\n", "DownMsgSize",pSmInstArray->BufAddrCfg.DownMsgSize);
    dprint(PRN_ALWS, "%-25s :0x%x\n", "UpMsgPhyAddr",pSmInstArray->BufAddrCfg.UpMsgPhyAddr);
    dprint(PRN_ALWS, "%-25s :%d\n", "UpMsgNum",pSmInstArray->BufAddrCfg.UpMsgNum);
    dprint(PRN_ALWS, "%-25s :%d\n", "UpMsgSize",pSmInstArray->BufAddrCfg.UpMsgSize);

    dprint(PRN_ALWS, "%-25s :0x%x\n", "seg BufPhyAddr",pSmInstArray->BufAddrCfg.SegBufPhyAddr);
    dprint(PRN_ALWS, "%-25s :%d\n", "seg BufSize",pSmInstArray->BufAddrCfg.SegBufSize);
    dprint(PRN_ALWS, "%-25s :%d\n", "seg BufUsed", SmInstInfo.TotalSegSize );
    dprint(PRN_ALWS, "%-25s :%d\n", "seg BufItemNum", SmInstInfo.SegNum);

    dprint(PRN_ALWS, "%-25s :%d\n", "raw StreamHold", SmInstInfo.TotalRawSize);
    dprint(PRN_ALWS, "%-25s :%d\n", "raw StreamPacketNum", SmInstInfo.RawNum);

    dprint(PRN_ALWS, "%-25s :%d\n", "filter ProtocolType",pSmInstArray->SegFilter.ProtocolType);
    dprint(PRN_ALWS, "%-25s :%d\n", "filter ModeFlag",pSmInstArray->SegFilter.ModeFlag);
    dprint(PRN_ALWS, "%-25s :%d\n", "filter LastH263Num",pSmInstArray->SegFilter.LastH263Num);
    dprint(PRN_ALWS, "%-25s :0x%x\n", "filter SegBufPhyAddr",pSmInstArray->SegFilter.SegBufPhyAddr);
    dprint(PRN_ALWS, "%-25s :%d\n", "filter SegBufSize",pSmInstArray->SegFilter.SegBufSize);
    dprint(PRN_ALWS, "%-25s :0x%x\n", "filter SegBufRollAddr",pSmInstArray->SegFilter.SegBufRollAddr);
    dprint(PRN_ALWS, "%-25s :0x%x\n", "filter pScdUpMsg",pSmInstArray->SegFilter.pScdUpMsg);
    dprint(PRN_ALWS, "%-25s :%d\n", "filter SegNum",pSmInstArray->SegFilter.SegNum);
    dprint(PRN_ALWS, "%-25s :%d\n", "BlockTimeInMs",pSmInstArray->BlockTimeInMs);
    dprint(PRN_ALWS, "%-25s :%d\n", "LastBlockTime",pSmInstArray->LastBlockTime);
    dprint(PRN_ALWS, "%-25s :%lld\n", "LastPts",pSmInstArray->LastPts);
    dprint(PRN_ALWS, "%-25s :%d\n", "ThisInstID",s_SmIIS.ThisInstID);
    dprint(PRN_ALWS, "%-25s :%d\n", "SCDState",s_SmIIS.SCDState);

    pstRaw = &pSmInstArray->RawPacketArray;

    dprint(PRN_ALWS, "\nraw packets Head : %d, Histroy : %d\n", "SCDState", pstRaw->Head, pstRaw->History);
    dprint(PRN_ALWS, "\nraw packets length is(first packet %d bytes used):\n", pstRaw->FirstPacketOffset);
    idx = pstRaw->History;
    for( i = 0; i < MAX_STREAM_RAW_NUM; i++ )
    {
        if ((i!=0) && ((i%10)==0) )
        {
            dprint(PRN_ALWS,"\n");
        }
        if( idx == pstRaw->Tail )
        {
            break;
        }
        else
        {
            dprint(PRN_ALWS,"%5d ", pstRaw->RawPacket[idx].Length);
        }

        idx++;
        if( idx >= MAX_STREAM_RAW_NUM )
        {
            idx -= MAX_STREAM_RAW_NUM;
        }
    }
    dprint(PRN_ALWS, "\n");

}


/************************************************************************
  ԭ��    VOID PrintScdSegState()
  ����    ��ӡSeg���е���ϸ���
  ����    
  ����ֵ  ��
 ************************************************************************/
VOID PrintScdSegState(SINT32 SmID, SINT32 PrintDetail)
{
    SINT32 i,SegNum,SegIndex;
    SEG_ARRAY_S *pSegStreamArray = NULL;

    i = 0;
    SegNum = 0;
    SegIndex = 0;

    if (IsDprintTypeEnable(PRN_SCD_INFO))
    {
        pSegStreamArray = &s_SmIIS.pSmInstArray[SmID]->StreamSegArray;

        dprint(PRN_SCD_INFO, "SegHead=%d\n",pSegStreamArray->Head);
        dprint(PRN_SCD_INFO, "SegTail=%d\n",pSegStreamArray->Tail);
        dprint(PRN_SCD_INFO, "SegCurrent=%d\n",pSegStreamArray->Current);
        dprint(PRN_SCD_INFO, "SegBufPhyAddr=%x\n",pSegStreamArray->SegBufPhyAddr);
        dprint(PRN_SCD_INFO, "SegBufVirAddr=%p\n",pSegStreamArray->pSegBufVirAddr);
        dprint(PRN_SCD_INFO, "SegBufSize=%d\n",pSegStreamArray->SegBufSize);
        dprint(PRN_SCD_INFO, "SegBufReadAddr=%x\n",pSegStreamArray->SegBufReadAddr);
        dprint(PRN_SCD_INFO, "SegBufWriteAddr=%x\n",pSegStreamArray->SegBufWriteAddr);

        if (1 == PrintDetail)
        {
            if(FMW_OK !=  GetSegStreamNum(pSegStreamArray,&SegNum))
            {
                dprint(PRN_DBG, "%s %d FMW_OK !=  GetSegStreamNum\n",__FUNCTION__,__LINE__);
            }            
            SegIndex = pSegStreamArray->Head;
            for (i = 0; i < SegNum; i++, SegIndex++)
            {
                dprint(PRN_SCD_INFO, "SegPhyAddr=0x%x\n",pSegStreamArray->StreamSeg[SegIndex].PhyAddr);
                dprint(PRN_SCD_INFO, "SegVirAddr=%p\n",pSegStreamArray->StreamSeg[SegIndex].VirAddr);
                dprint(PRN_SCD_INFO, "SegLength=%d\n",pSegStreamArray->StreamSeg[SegIndex].LenInByte);
                dprint(PRN_SCD_INFO, "SegStreamID=%d\n",pSegStreamArray->StreamSeg[SegIndex].StreamID);
                dprint(PRN_SCD_INFO, "SegState=%d\n",pSegStreamArray->StreamSeg[SegIndex].SegState);
                dprint(PRN_SCD_INFO, "SegIsLastSeg=%d\n",pSegStreamArray->StreamSeg[SegIndex].IsLastSeg);
                dprint(PRN_SCD_INFO, "SegPts=%lld\n",pSegStreamArray->StreamSeg[SegIndex].Pts);              
            }
        }   
    }
}

