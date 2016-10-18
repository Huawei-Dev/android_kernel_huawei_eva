/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7omB5iYtHEGPqMDW7TpB3q4yvDckVri1GoBL4gKI3k0M86wMC5hMD5fQLRyjt
hU2r3+zDC3Be1gNB5sgdQYoVeOn00PEAD8+QuxD9BZaN1DpZgEgYq7gL5UYd40xXnzlV8O+p
06rgzw8z38U2KKT+9JrHVv55T8YUhMX+h9mrMoExQjRk/aGq4sOI115pnJpFeA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************************
*
* Copyright (c) 2007 HUAWEI - All Rights Reserved
*
* File     : $h264.c$
* Date     : $2007/11/05$
* Revision : $v1.1$
* Purpose  : H.264 decoder file.
*
* Change History:
*
* Date                       Author                          Change
* ====                       ======                          ======
* 2006/12/01                 p56607                          Original.
* 2007/11/05                 l57648,p56607,q45134            Changed to serial processing.
*
* Dependencies:
* Linux OS
*
************************************************************************/

#include    "syntax.h"
#include    "h264.h"
#include    "public.h"
#include    "bitstream.h"
#include	"vdm_hal.h"

#include    "postprocess.h"
#include    "vfmw_ctrl.h"
#include    "fsp.h"


/* H.264ģ����뿪�أ����û�д��򲻱��� */
#ifdef H264_ENABLE

extern UINT8  g_allow_B_before_P;
extern UINT8  g_apple_airplay_mirrorinig_flag;
extern UINT8  g_allow_ClearDPB_after_RepairListErr_flag;

UINT8 g_NalTypeEOPIC[] = {0x00, 0x00, 0x01, 0x1E, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45, 0x4E, 0x44};

UINT32 g_AspecRatioIdc[14][2] = { { 1,  1}, { 1,  1}, {12, 11}, {10, 11}, {16, 11}, {40, 33}, { 24, 11},
    {20, 11}, {32, 11}, {80, 33}, {18, 11}, {15, 11}, {64, 33}, {160, 99}
};

UINT8 CalcZeroNum[256] = { 8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const UINT8 g_ZZ_SCAN[16] =
{
    0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

const UINT8 g_ZZ_SCAN8[64] =
{
     0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

UINT32 quant_intra_default[4] = { 0x1c140d06, 0x201c140d, 0x25201c14, 0x2a25201c };

UINT32 quant_inter_default[4] = { 0x18140e0a,  0x1b18140e, 0x1e1b1814, 0x221e1b18 };

UINT32 quant8_intra_default[16] = { 0x100d0a06, 0x1b191712, 0x12100b0a, 0x1d1b1917,
0x1712100d, 0x1f1d1b19, 0x19171210, 0x211f1d1b,
0x1b191712, 0x24211f1d, 0x1d1b1917, 0x2624211f,
0x1f1d1b19, 0x28262421, 0x211f1d1b, 0x2a282624
};

UINT32 quant8_inter_default[64] = { 0x110f0d09, 0x18161513, 0x13110d0d, 0x19181615,
0x1513110f, 0x1b191816, 0x16151311, 0x1c1b1918,
0x18161513, 0x1e1c1b19, 0x19181615, 0x201e1c1b,
0x1b191816, 0x21201e1c, 0x1c1b1918, 0x2321201e
};

UINT32 quant_org[4] = { 0x10101010, 0x10101010, 0x10101010, 0x10101010 };

UINT32 quant8_org[16] = { 0x10101010, 0x10101010, 0x10101010, 0x10101010,
0x10101010, 0x10101010, 0x10101010, 0x10101010,
0x10101010, 0x10101010, 0x10101010, 0x10101010,
0x10101010, 0x10101010, 0x10101010, 0x10101010
};



/*!
************************************************************************
*   ����ԭ�� :  SINT32 ue_v(H264_CTX_S *pH264Ctx)
*   �������� :  �޷��ű䳤�����,ָ��ӳ�亯���͵��ú�����ȡ�﷨Ԫ�ص�������Ϣ
�ͽ����﷨Ԫ��ֵ
*   ����˵�� :  ������Ϣ���﷨Ԫ��SE_TRACEstring��Ϣ
*   ����ֵ   �� ���﷨Ԫ��ֵ
************************************************************************
*/
UINT32 ue_v(H264_CTX_S *pH264Ctx,UINT8 *SE_TRACEstring)
{
    UINT32 NumBits, tmpbits;
    UINT32 INFO;
    UINT32 zeros;

    // ======== mb_type ==========
    tmpbits = BsShow(&(pH264Ctx->BS), 32);
    zeros = ZerosMS_32(tmpbits);
    if (zeros < 16)
    {
        ue_vld(tmpbits, NumBits, INFO, zeros);
        BsSkip(&(pH264Ctx->BS), NumBits);
    }
    else
    {
        if (zeros < 32)
        {
            // ��Ϊ������Ϊ2��31�η�������zeros+1��������Ϊ32
            BsSkip(&(pH264Ctx->BS), zeros);
            INFO = BsShow(&(pH264Ctx->BS), (zeros+1)) - 1;
            BsSkip(&(pH264Ctx->BS), (zeros+1));
            NumBits = (zeros<<1) + 1;
        }
        else
        {
            // ��ʱ��32���㣬˵���д�����
            pH264Ctx->SeErrFlg = 1;
            INFO = 0xffffffff;
            NumBits = 32;
        }
    }
    // NumBits = (zeros<<1) + 1;
    // INFO = ((tmpbits)>>(32-NumBits)) - 1;

    pH264Ctx->pCurrNal->nal_bitoffset += NumBits;

    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);

    return INFO;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 se_v(H264_CTX_S *pH264Ctx)
*   �������� :  �з��ű䳤�����,ָ��ӳ�亯���͵��ú�����ȡ�﷨Ԫ�ص�������Ϣ
�ͽ����﷨Ԫ��ֵ
*   ����˵�� :  ������Ϣ���﷨Ԫ��SE_TRACEstring��Ϣ
*   ����ֵ   �� ���﷨Ԫ��ֵ
************************************************************************
*/
SINT32 se_v(H264_CTX_S *pH264Ctx,UINT8 *SE_TRACEstring)
{
    UINT32 NumBits, tmpbits;
    SINT32 INFO;
    UINT32 zeros;
    UINT32 INFOLastBit;

    // ======== mb_type ==========
    tmpbits = BsShow(&(pH264Ctx->BS), 32);
    zeros   = ZerosMS_32(tmpbits);

    if (zeros < 16)
    {
        se_vld(tmpbits, NumBits, INFO, zeros);
        BsSkip(&(pH264Ctx->BS), NumBits);
    }
    else
    {
        // ��Ϊ������Ϊ2��31�η�������zeros��������Ϊ32�������������������ҪBsShow����
        if (zeros < 32)
        {
            BsSkip(&(pH264Ctx->BS), zeros);
            INFO = BsGet(&(pH264Ctx->BS), zeros);
            INFOLastBit = BsGet(&(pH264Ctx->BS), 1);
            INFO = ((INFOLastBit&1) ? -1*INFO : INFO);
            NumBits = (zeros<<1) + 1;
        }
        else
        {
            // ��ʱ��32���㣬˵���д�����
            pH264Ctx->SeErrFlg = 1;
            INFO = 0x7fffffff;
            NumBits = 32;
        }
    }

    pH264Ctx->pCurrNal->nal_bitoffset += NumBits;
    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);

    return INFO;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 u_v(H264_CTX_S *pH264Ctx)
*   �������� :  �����﷨Ԫ�ؽ���
*   ����˵�� :  �﷨Ԫ�س��Ⱥ�SE_TRACEstring��������Ϣ
*   ����ֵ   �� �﷨Ԫ��ֵ
************************************************************************
*/
SINT32 u_v(H264_CTX_S *pH264Ctx,UINT32 LenInBits, UINT8 *SE_TRACEstring)
{
    SINT32 INFO = 0;

    INFO = BsGet(&(pH264Ctx->BS), LenInBits);
    pH264Ctx->pCurrNal->nal_bitoffset += LenInBits;

    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);

    return INFO;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 u_1(H264_CTX_S *pH264Ctx)
*   �������� :  �����﷨Ԫ�ؽ��룬����1bit
*   ����˵�� :  �﷨Ԫ�س��Ⱥ�SE_TRACEstring��������Ϣ
*   ����ֵ   �� �﷨Ԫ��ֵ
************************************************************************
*/
UINT32 u_1(H264_CTX_S *pH264Ctx,UINT8 *SE_TRACEstring)
{
    SINT32 INFO = 0;
    INFO = BsGet(&(pH264Ctx->BS), 1);
    pH264Ctx->pCurrNal->nal_bitoffset += 1;

    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);

    return INFO;
}


SINT32 ClearCurrNal(H264_CTX_S *pH264Ctx)
{
    if (NULL != pH264Ctx->pCurrNal)
    {
        ReleaseNAL(pH264Ctx->ChanID, pH264Ctx->pCurrNal);
        pH264Ctx->pCurrNal = NULL;
    }

    return H264_OK;
}


SINT32 ClearAllNal(H264_CTX_S *pH264Ctx)
{
    UINT32 i;

    for (i=0; i < MAX_SLICE_SLOT_NUM+1; i++)
    {
        if (1 == (pH264Ctx->NalArray[i].is_valid))
        {
            ReleaseNAL(pH264Ctx->ChanID, &(pH264Ctx->NalArray[i]));
        }
    }

    return H264_OK;
}


SINT32 ClearCurrSlice(H264_CTX_S *pH264Ctx)
{
    dprint(PRN_ERROR, "clear curr slice.\n");

    ClearCurrNal(pH264Ctx);
    pH264Ctx->TotalSlice++;

    return H264_OK;
}


SINT32 ClearAllSlice(H264_CTX_S *pH264Ctx)
{
    UINT32 i;

    for (i = 0; i < MAX_SLICE_SLOT_NUM; i++)
    {
        if (NULL != pH264Ctx->SlcInfo[i].pSliceNal)
        {
            ReleaseNAL(pH264Ctx->ChanID, pH264Ctx->SlcInfo[i].pSliceNal);
            pH264Ctx->SlcInfo[i].pSliceNal = NULL;
        }
    }
    pH264Ctx->SliceParaNum = 0;
    pH264Ctx->MaxBytesReceived = 0;

    return H264_OK;
}


SINT32 H264_ClearCurrPic(H264_CTX_S *pH264Ctx)
{
    ClearAllSlice(pH264Ctx);
    InitOldSlice(pH264Ctx);

    return H264_OK;
}


SINT32 ClearDPB(H264_CTX_S *pH264Ctx)
{
   	SINT32 ret;
    ret = FlushDPB(pH264Ctx);
    if (H264_OK != ret)
    {
        dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
    }
    InitDPB(pH264Ctx);

    pH264Ctx->IDRFound = 0;
    pH264Ctx->FirstFrameDisplayed = FIRST_DISP_INIT;

    return H264_OK;
}


SINT32 ClearAll(H264_CTX_S *pH264Ctx,SINT32 flag)
{
    UINT32 i;
    UINT32 usrdat_idx;

    dprint(PRN_DBG, "clear all dec para\n");

    H264_ClearCurrPic(pH264Ctx);
    ClearAllNal(pH264Ctx);

    InitDPB(pH264Ctx);
    if(flag)
    {
        FSP_ClearNotInVoQueue(pH264Ctx->ChanID,&pH264Ctx->ImageQue);
    }
    else
    {
        ResetVoQueue(&pH264Ctx->ImageQue);
        FSP_EmptyInstance(pH264Ctx->ChanID);
    }
    for (i=0; i<MAX_FRAME_STORE; i++)
    {
        pH264Ctx->FrameStore[i].non_existing = 0;                 //a real pic;
        pH264Ctx->FrameStore[i].is_used = 0;                      //empty;
        pH264Ctx->FrameStore[i].is_reference = 0;                 //not pic used for ref;
        pH264Ctx->FrameStore[i].long_term_frame_idx = 0;
        pH264Ctx->FrameStore[i].frame_num = 0;
        pH264Ctx->FrameStore[i].frame_num_wrap = -1;
        pH264Ctx->FrameStore[i].poc = 0;
        pH264Ctx->FrameStore[i].is_in_dpb = 0;                    //already out from g_DPB
        pH264Ctx->FrameStore[i].is_displayed = 1;                 //already out from VO
        pH264Ctx->FrameStore[i].pmv_address_idc = MAX_PMV_STORE;
        pH264Ctx->FrameStore[i].apc_idc = 16;
        pH264Ctx->FrameStore[i].fs_image.image_id = i;
        pH264Ctx->FrameStore[i].frame.frame_store = pH264Ctx->FrameStore[i].top_field.frame_store = pH264Ctx->FrameStore[i].bottom_field.frame_store = & pH264Ctx->FrameStore[i];
        pH264Ctx->FrameStore[i].frame.structure = 0;              //g_frame is frame, will be change when field paired
        pH264Ctx->FrameStore[i].top_field.structure = 1;          //g_top is top field;
        pH264Ctx->FrameStore[i].bottom_field.structure = 2;       //g_bottom is bottom field
        for (usrdat_idx=0; usrdat_idx <4; usrdat_idx++)
        {
            if (NULL != pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx])
            {
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx]);
                pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx] = NULL;
            }
        }
    }

    // release usrdat of currpic
    for (i=0; i<4; i++)
    {
        if (NULL != pH264Ctx->CurrPic.fs_image.p_usrdat[i] )
        {
            FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->CurrPic.fs_image.p_usrdat[i]);
            pH264Ctx->CurrPic.fs_image.p_usrdat[i] = NULL;
        }
    }

    for (i = 0; i <pH264Ctx->TotalUsrDatNum; i++ )
    {
        if (NULL !=pH264Ctx->pUsrDatArray[i])
        {
            FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[i]);
            pH264Ctx->pUsrDatArray[i] = NULL;
        }
    }
    pH264Ctx->TotalUsrDatNum = 0;

    // refresh some statistic variable
    pH264Ctx->IDRPicOfCurrPic = 0;
    pH264Ctx->AllowStartDec = 0;
    pH264Ctx->SliceParaNum = 0;
    pH264Ctx->MaxBytesReceived = 0;
    pH264Ctx->IModeOutFlag = 0;
    pH264Ctx->IDRFound = 0;
    pH264Ctx->FirstFrameDisplayed = FIRST_DISP_INIT;
    pH264Ctx->PPicFound = 0;

    // init some global var
    pH264Ctx->pCurrNal = NULL;
    pH264Ctx->CurrMarkIdx = 0;
    pH264Ctx->Mark[0].is_valid = pH264Ctx->Mark[1].is_valid = 0;
    pH264Ctx->Mark[0].no_output_of_prior_pics_flag = pH264Ctx->Mark[1].no_output_of_prior_pics_flag = 0;

    // init pH264Ctx->CurrSlice
    memset(&pH264Ctx->CurrSlice, 0, sizeof(H264_SLICE_S));
    pH264Ctx->CurrSlice.slice_type = I_SLICE;
    pH264Ctx->CurrSlice.new_pic_flag = 1;
    pH264Ctx->CurrPic.state = EMPTY;

    pH264Ctx->PrevPic.structure = FRAME;
    pH264Ctx->PrevPic.frame_num = 256;
    pH264Ctx->PrevPic.nal_ref_idc = 0;

    return H264_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 GetMinPOC(H264_CTX_S *pH264Ctx,SINT32 *poc, SINT32 *pos)
*   �������� :  �ҳ���Сpoc ����Сpocͼ��λ��
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
VOID GetMinPOC(H264_CTX_S *pH264Ctx, SINT32 *poc, UINT32 *pos)
{
    UINT32 i;

    *pos = 0;
    *poc = 0x7fffffff;

    if (pH264Ctx->DPB.used_size > pH264Ctx->DPB.size)
    {
        pH264Ctx->DPB.used_size = pH264Ctx->DPB.size; // for error resilence
    }
    for (i=0; i<pH264Ctx->DPB.used_size; i++)
    {
        if ((*poc>pH264Ctx->DPB.fs[i]->poc) && (pH264Ctx->DPB.fs[i]->is_in_dpb))
        {
            *poc = pH264Ctx->DPB.fs[i]->poc;
            *pos = i;
        }
    }

    return;
}


/*
************************************************************************
*   ����ԭ�� :  UINT32 FrameStoreRefFlg(H264_FRAMESTORE_S *pFS)
*   �������� :  ָʾ��framestore�Ƿ����ڲο�
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
UINT32 FrameStoreRefFlg(const H264_FRAMESTORE_S *pFS)
{
    return (pFS->is_reference > 0);
}


/*!
************************************************************************
*   ����ԭ�� :  UINT32 ShortTermRefFlg(H264_FRAMESTORE_S *pFS)
*   �������� :  �жϸ�֡���Ƿ�Ϊ���ڲο�
*   ����˵�� :  ֡��ָ��
*   ����ֵ   �� �жϽ��
************************************************************************
*/
UINT32 ShortTermRefFlg(const H264_FRAMESTORE_S *pFS)
{
    if (3 == pFS->is_reference)
    {
        if (1 == pFS->frame.is_short_term && 0 == pFS->frame.is_long_term)
        {
            return 1;
        }
    }

    if (pFS->is_reference & 1)
    {
        if (1 == pFS->top_field.is_short_term && 0 == pFS->top_field.is_long_term)
        {
            return 1;
        }
    }

    if (pFS->is_reference & 2)
    {
        if (1 == pFS->bottom_field.is_short_term && 0 == pFS->bottom_field.is_long_term)
        {
            return 1;
        }
    }

    return 0;
}


/*!
************************************************************************
*   ����ԭ�� :  UINT32 LongTermRefFlg(H264_FRAMESTORE_S *pFS)
*   �������� :  �жϸ�֡���Ƿ�Ϊ���ڲο�
*   ����˵�� :  ֡��ָ��
*   ����ֵ   �� �жϽ��
************************************************************************
*/
UINT32 LongTermRefFlg(const H264_FRAMESTORE_S *pFS)
{
    if (3 == pFS->is_reference)
    {
        if (0 == pFS->frame.is_short_term && 1 == pFS->frame.is_long_term)
        {
            return 1;
        }
    }

    if (pFS->is_reference & 1)
    {
        if (0 == pFS->top_field.is_short_term && 1 == pFS->top_field.is_long_term)
        {
            return 1;
        }
    }

    if (pFS->is_reference & 2)
    {
        if (0 == pFS->bottom_field.is_short_term && 1 == pFS->bottom_field.is_long_term)
        {
            return 1;
        }
    }

    return 0;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UpdateLTReflist(H264_CTX_S *pH264Ctx)
*      �������� :  ����DPB�г��ڲο��б�
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID UpdateLTReflist(H264_CTX_S *pH264Ctx)
{
    UINT32 i, j;

    for (i=0, j=0; i<pH264Ctx->DPB.used_size; i++)
    {
        if (LongTermRefFlg(pH264Ctx->DPB.fs[i]))
        {
            pH264Ctx->DPB.fs_ltref[j++] = pH264Ctx->DPB.fs[i];
        }
    }
    pH264Ctx->DPB.ltref_frames_in_buffer = j;
    while (j<pH264Ctx->DPB.size)
    {
        pH264Ctx->DPB.fs_ltref[j++] = (H264_FRAMESTORE_S *) NULL;
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UpdateReflist(H264_CTX_S *pH264Ctx)
*      �������� :  ����DPB�ж��ڲο��б�
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID UpdateReflist(H264_CTX_S *pH264Ctx)
{
    UINT32 i, j;

    for (i=0, j=0; i<pH264Ctx->DPB.used_size; i++)
    {
        if (ShortTermRefFlg(pH264Ctx->DPB.fs[i]))
        {
            pH264Ctx->DPB.fs_ref[j++] = pH264Ctx->DPB.fs[i];
        }
    }
    pH264Ctx->DPB.ref_frames_in_buffer = j;
    while (j<pH264Ctx->DPB.size)
    {
        pH264Ctx->DPB.fs_ref[j++] = (H264_FRAMESTORE_S *)NULL;
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 GetPicNumX(H264_CURRPIC_S *pCurrPic, UINT32 difference_of_pic_nums_minus1)
*      �������� :  ����difference_of_pic_nums_minus1����pic_num,
*      ����˵�� :  difference_of_pic_nums_minus1�����ڼ���pic_num��pCurrPic��ǰͼ��ṹ��ָ��
*      ����ֵ   �� ��
************************************************************************
*/
SINT32 GetPicNumX(const H264_CURRPIC_S *pCurrPic, UINT32 difference_of_pic_nums_minus1)
{
    UINT32 CurrPicNum;
    SINT32 result;

    if (FRAME == pCurrPic->structure)
    {
        CurrPicNum = pCurrPic->frame_num;
    }
    else
    {
        CurrPicNum = 2 * pCurrPic->frame_num + 1;
    }

    result = (SINT32)(CurrPicNum - (difference_of_pic_nums_minus1 + 1));

    return result;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UnMarkFrameStoreRef(H264_CTX_S *pH264Ctx,H264_FRAMESTORE_S *pFrameStore)
*      �������� :  ��pFrameStore����֡��������ͼ����Ϊ���ο���ͬʱ�ͷ�pmv��λ
*      ����˵�� :  pFrameStore֡��ָ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID UnMarkFrameStoreRef(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore)
{
    if (pFrameStore->is_used & 1) // inlcude top field
    {
        //unmark top field and don't modify bottom field reference attribute
        pFrameStore->is_reference &= 2;
        pFrameStore->top_field.is_long_term = 0;
        pFrameStore->top_field.is_short_term = 0;
    }

    if (pFrameStore->is_used & 2) // include bottom field
    {
        //unmark top field and don't modify bottom field reference attribute
        pFrameStore->is_reference &= 1;
        pFrameStore->bottom_field.is_long_term = 0;
        pFrameStore->bottom_field.is_short_term = 0;
    }

    if (3 == pFrameStore->is_used) //frame, both field pair
    {
        pFrameStore->frame.is_short_term = 0;
        pFrameStore->frame.is_long_term = 0;
        pFrameStore->is_reference = 0;
    }

    //�������ϲ��費һ���Ͱ�is_reference����0���������ǿ��һ��
    pFrameStore->is_reference = 0;

    /* ���߼�֡����Ϊ���ο� */
    FSP_SetRef(pH264Ctx->ChanID, pFrameStore->logic_fs_id, 0);

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UnMarkLTFrmByFrmIdx(UINT32 long_term_frame_idx)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID UnMarkLTFrmByFrmIdx(H264_CTX_S *pH264Ctx,UINT32 long_term_frame_idx)
{
    UINT32 i;

    for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
    {
        if (long_term_frame_idx == pH264Ctx->DPB.fs_ltref[i]->long_term_frame_idx)
        {
            UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs_ltref[i]);
            return;
        }
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UnMarkLTFldByFrmIdx(H264_CTX_S *pH264Ctx,H264_FRAMESTORE_S *pFS, UINT32 structure, long_term_frame_idx)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID UnMarkLTFldByFrmIdx(H264_CTX_S *pH264Ctx,const H264_FRAMESTORE_S *pFS, UINT32 structure, UINT32 long_term_frame_idx)
{
    UINT32 i;

    for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
    {
        if (long_term_frame_idx == pH264Ctx->DPB.fs_ltref[i]->long_term_frame_idx)
        {
            if (TOP_FIELD == structure)
            {
                if ((0 == pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_long_term) || (pFS != pH264Ctx->DPB.fs_ltref[i])) // not long term field pair
                {
                    UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs_ltref[i]);
                    break;  //!!!
                }
            }
            if (BOTTOM_FIELD == structure)
            {
                if ((0 == pH264Ctx->DPB.fs_ltref[i]->top_field.is_long_term) || (pFS != pH264Ctx->DPB.fs_ltref[i])) // not long term field pair
                {
                    UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs_ltref[i]);
                    break;  //!!!
                }
            }
        }
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UnMarkSTRef(H264_CTX_S *pH264Ctx,H264_CURRPIC_S  *pCurrPic, UINT32 difference_of_pic_nums_minus1)
*      �������� :  ����difference_of_pic_nums_minus1����pic_num,
*                  ��pic_num��Ӧ�Ķ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  difference_of_pic_nums_minus1�����ڼ���pic_num��pCurrPic��ǰͼ�����ݽṹ��ָ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID  UnMarkSTRef(H264_CTX_S *pH264Ctx,const H264_CURRPIC_S  *pCurrPic, SINT32 difference_of_pic_nums_minus1)
{
    UINT32 i;
    SINT32 picNumX;

    /* Get picNumX */
    picNumX = GetPicNumX(pCurrPic, difference_of_pic_nums_minus1);

    if (FRAME == pCurrPic->structure)
    {
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if (pH264Ctx->DPB.fs_ref[i]->frame.pic_num == picNumX)
            {
                if (3==pH264Ctx->DPB.fs_ref[i]->is_reference)
                {
                    if (0==pH264Ctx->DPB.fs_ref[i]->frame.is_long_term && 1==pH264Ctx->DPB.fs_ref[i]->frame.is_short_term)
                    {
                        UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs_ref[i]);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if ((pH264Ctx->DPB.fs_ref[i]->is_reference & 1) &&
                0==pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term && 1==pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term)
            {
                if (pH264Ctx->DPB.fs_ref[i]->top_field.pic_num == picNumX)                     //include top field
                {
                    //mark top field unused
                    pH264Ctx->DPB.fs_ref[i]->is_reference &= 2;
                    pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term =0;

                    //one field reference attribute modified and frame reference attribute need to modify
                    if (FIELD_PAIR == pH264Ctx->DPB.fs_ref[i]->frame.structure || FRAME == pH264Ctx->DPB.fs_ref[i]->frame.structure)
                    {
                        pH264Ctx->DPB.fs_ref[i]->frame.is_short_term = 0;
                        pH264Ctx->DPB.fs_ref[i]->frame.is_long_term = 0;
                    }

                    if (0 == pH264Ctx->DPB.fs_ref[i]->is_reference)
                    {
                        pH264Ctx->PmvStoreUsedFlag[pH264Ctx->DPB.fs_ref[i]->pmv_address_idc] = 0;          //pmv store
                        FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->DPB.fs_ref[i]->logic_fs_id, 0);   //z56361
                    }

                    return;
                }
            }

            if ((pH264Ctx->DPB.fs_ref[i]->is_reference & 2) &&
                 0 == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term && 1 == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term)
            {
                 if (pH264Ctx->DPB.fs_ref[i]->bottom_field.pic_num == picNumX)
                 {
                    //mark bottom field unused
                    pH264Ctx->DPB.fs_ref[i]->is_reference &= 1;
                    pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term = 0;

                    //one field reference attribute modified and frame reference attribute need to modify
                    if (FIELD_PAIR == pH264Ctx->DPB.fs_ref[i]->frame.structure || FRAME == pH264Ctx->DPB.fs_ref[i]->frame.structure)
                    {
                        pH264Ctx->DPB.fs_ref[i]->frame.is_short_term = 0;
                        pH264Ctx->DPB.fs_ref[i]->frame.is_long_term = 0;
                    }

                    if (0 == pH264Ctx->DPB.fs_ref[i]->is_reference)
                    {
                        pH264Ctx->PmvStoreUsedFlag[pH264Ctx->DPB.fs_ref[i]->pmv_address_idc] = 0;          //pmv store
                        FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->DPB.fs_ref[i]->logic_fs_id, 0);   //z56361
                    }
                    return;
                }
            }
        }
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UnMarkLTRef(H264_CTX_S *pH264Ctx,H264_CURRPIC_S  *pCurrPic, UINT32 long_term_pic_num)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID UnMarkLTRef(H264_CTX_S *pH264Ctx,const H264_CURRPIC_S *pCurrPic, SINT32 long_term_pic_num)
{
    UINT32 i;

    if (FRAME == pCurrPic->structure)
    {
        for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
        {
            if (long_term_pic_num == pH264Ctx->DPB.fs_ltref[i]->frame.long_term_pic_num)
            {
                if (3 == pH264Ctx->DPB.fs_ltref[i]->is_reference)
                {
                    if (1==pH264Ctx->DPB.fs_ltref[i]->frame.is_long_term && 0==pH264Ctx->DPB.fs_ltref[i]->frame.is_short_term)
                    {
                        UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs_ltref[i]);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
        {
            if ((pH264Ctx->DPB.fs_ltref[i]->is_reference & 1) &&
                1==pH264Ctx->DPB.fs_ltref[i]->top_field.is_long_term && 0==pH264Ctx->DPB.fs_ltref[i]->top_field.is_short_term)
            {
                if (long_term_pic_num == pH264Ctx->DPB.fs_ltref[i]->top_field.long_term_pic_num)                     //include top field
                {
                    //mark top field unused
                    pH264Ctx->DPB.fs_ltref[i]->is_reference &= 2;
                    pH264Ctx->DPB.fs_ltref[i]->top_field.is_long_term = 0;

                    //one field reference attribute modified and frame reference attribute need to modify
                    if (FIELD_PAIR == pH264Ctx->DPB.fs_ltref[i]->frame.structure || FRAME == pH264Ctx->DPB.fs_ltref[i]->frame.structure)
                    {
                        pH264Ctx->DPB.fs_ltref[i]->frame.is_short_term = 0;
                        pH264Ctx->DPB.fs_ltref[i]->frame.is_long_term = 0;
                    }
                    if (0 == pH264Ctx->DPB.fs_ltref[i]->is_reference)
                    {
                        pH264Ctx->PmvStoreUsedFlag[pH264Ctx->DPB.fs_ltref[i]->pmv_address_idc] = 0;          //pmv store
                    }
                    return;
                }
            }

            if ((pH264Ctx->DPB.fs_ltref[i]->is_reference & 2) &&
                 1==pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_long_term && 0==pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_short_term)
            {
                 if (long_term_pic_num == pH264Ctx->DPB.fs_ltref[i]->bottom_field.long_term_pic_num)
                 {
                    //mark bottom field unused
                    pH264Ctx->DPB.fs_ltref[i]->is_reference &= 1;
                    pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_long_term = 0;

                    //one field reference attribute modified and frame reference attribute need to modify
                    if (FIELD_PAIR == pH264Ctx->DPB.fs_ltref[i]->frame.structure || FRAME == pH264Ctx->DPB.fs_ltref[i]->frame.structure)
                    {
                        pH264Ctx->DPB.fs_ltref[i]->frame.is_short_term = 0;
                        pH264Ctx->DPB.fs_ltref[i]->frame.is_long_term = 0;
                    }
                    if (0 == pH264Ctx->DPB.fs_ltref[i]->is_reference)
                    {
                        pH264Ctx->PmvStoreUsedFlag[pH264Ctx->DPB.fs_ltref[i]->pmv_address_idc] = 0;          //pmv store
                    }
                    return;
                }
            }
        }
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MarkPicLTRef(UINT32 long_term_pic_num, SINT32 picNumX, H264_CURRPIC_S *pCurrPic)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID MarkPicLTRef(H264_CTX_S *pH264Ctx, UINT32 long_term_frame_idx, SINT32 picNumX, const H264_CURRPIC_S *pCurrPic)
{
    UINT32 i;
    UINT32 add_top = (TOP_FIELD == pCurrPic->structure) ? 1 : 0;
    UINT32 add_bottom = (BOTTOM_FIELD == pCurrPic->structure) ? 1 : 0;

    if (FRAME == pCurrPic->structure)
    {
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if ((3 ==pH264Ctx->DPB.fs_ref[i]->is_reference) && (0 == pH264Ctx->DPB.fs_ref[i]->frame.is_long_term && 1 == pH264Ctx->DPB.fs_ref[i]->frame.is_short_term)
                && (pH264Ctx->DPB.fs_ref[i]->frame.pic_num == picNumX))
            {
                pH264Ctx->DPB.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
                pH264Ctx->DPB.fs_ref[i]->frame.long_term_pic_num = (SINT32)long_term_frame_idx;
                // mark to long term
                pH264Ctx->DPB.fs_ref[i]->frame.is_long_term = 1;
                pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term = 1;
                pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term = 1;
                // not short term anymore
                pH264Ctx->DPB.fs_ref[i]->frame.is_short_term = 0;
                pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term = 0;
                pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term = 0;
                break;
            }
        }
    }
    else
    {
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if (pH264Ctx->DPB.fs_ref[i]->is_reference & 1)
            {
                if ((1==pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term && 0==pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term)
                    && (picNumX==pH264Ctx->DPB.fs_ref[i]->top_field.pic_num))
                {
                    // top field is short term and not long term and picNum equals picNumX
                    if ((1 == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term) &&
                        (pH264Ctx->DPB.fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
                    {
                        // check bottom field is long term but long_term_frame_idx is diff
                        continue;
                    }

                    pH264Ctx->DPB.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
                    pH264Ctx->DPB.fs_ref[i]->top_field.long_term_pic_num = (SINT32)(2 * long_term_frame_idx + add_top);
                    pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term = 1;  // mark long term
                    pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term = 0; // mark not short term

                    // modify frame reference attribute
                    if (FIELD_PAIR == pH264Ctx->DPB.fs_ref[i]->frame.structure || \
                        FRAME == pH264Ctx->DPB.fs_ref[i]->frame.structure)
                    {
                        if (pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term)
                        {
                            pH264Ctx->DPB.fs_ref[i]->frame.is_long_term = pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term;
                            pH264Ctx->DPB.fs_ref[i]->frame.is_short_term = pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term;
                        }
                    }

                    break;
                }
            }
            if (pH264Ctx->DPB.fs_ref[i]->is_reference & 2) // include bottom field
            {
                if ((0 == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term && 1 == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term) &&
                    (pH264Ctx->DPB.fs_ref[i]->bottom_field.pic_num == picNumX))
                {
                    if ((1 == pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term) && (pH264Ctx->DPB.fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
                    {
                        continue;
                    }

                    pH264Ctx->DPB.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
                    pH264Ctx->DPB.fs_ref[i]->bottom_field.long_term_pic_num = (SINT32)(2 * long_term_frame_idx + add_bottom);
                    pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term = 1;  // mark long term
                    pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term = 0; // mark not short term

                    // modify frame reference attribute
                    if (FIELD_PAIR == pH264Ctx->DPB.fs_ref[i]->frame.structure ||FRAME == pH264Ctx->DPB.fs_ref[i]->frame.structure)
                    {
                        if (pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term)
                        {
                            pH264Ctx->DPB.fs_ref[i]->frame.is_long_term = pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term;
                            pH264Ctx->DPB.fs_ref[i]->frame.is_short_term = pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term;
                        }
                    }
                    break;
                }
            }
        }
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UnMarkLTRef(H264_CTX_S *pH264Ctx, H264_CURRPIC_S *pCurrPic, UINT32 long_term_pic_num)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID MarkSTToLTRef(H264_CTX_S *pH264Ctx, const H264_CURRPIC_S *pCurrPic, UINT32 difference_of_pic_nums_minus1, UINT32 long_term_frame_idx)
{
    SINT32 picNumX;
    UINT32 i;

    /* Get picNumX */
    picNumX = GetPicNumX(pCurrPic, difference_of_pic_nums_minus1);

    /*then mark long term pic to unused which have this long_term_frame_idx */
    if (FRAME == pCurrPic->structure)// frame , both field pair
    {
        UnMarkLTFrmByFrmIdx(pH264Ctx, long_term_frame_idx);
    }
    else
    {
        //just find structure which will mark to long term
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if ((pH264Ctx->DPB.fs_ref[i]->is_reference & 1) && \
                1 == pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term && 0 == pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term) // include top field
            {
                if (pH264Ctx->DPB.fs_ref[i]->top_field.pic_num == picNumX)
                {
                    UnMarkLTFldByFrmIdx(pH264Ctx, pH264Ctx->DPB.fs_ref[i], TOP_FIELD, long_term_frame_idx);
                    break;
                }
            }
            if ((pH264Ctx->DPB.fs_ref[i]->is_reference & 2) &&
                1 == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term && 0 == pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term) // include bottom field
            {
                if (pH264Ctx->DPB.fs_ref[i]->bottom_field.pic_num == picNumX)
                {
                    UnMarkLTFldByFrmIdx(pH264Ctx, pH264Ctx->DPB.fs_ref[i], BOTTOM_FIELD, long_term_frame_idx);
                    break;
                }
            }
        }
    }

    /*finally mark short pic to long term*/
    MarkPicLTRef(pH264Ctx, long_term_frame_idx, picNumX, pCurrPic);

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UpdateMaxLTFrmIdx(H264_CTX_S *pH264Ctx, UINT32 max_long_term_frame_idx_plus1)
*      �������� :  ����DPB������ڲο�����
*      ����˵�� :  ���º������ڲο�֡����max_long_term_frame_idx_plus1
*      ����ֵ   �� ��
************************************************************************
*/
VOID UpdateMaxLTFrmIdx(H264_CTX_S *pH264Ctx, UINT32 max_long_term_frame_idx_plus1)
{
    UINT32 i;

    pH264Ctx->DPB.max_long_term_pic_idx = max_long_term_frame_idx_plus1;

    // check for invalid frames
    for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
    {
        // ���������ת��һ��Ҫ����Ȼ��max_long_term_frame_idx_plus1����0��ʱ��ͳ�����
        if (pH264Ctx->DPB.fs_ltref[i]->long_term_frame_idx >= pH264Ctx->DPB.max_long_term_pic_idx)
        {
            UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs_ltref[i]);
        }
    }
}


/*!
************************************************************************
*      ����ԭ�� :  VOID UnMarkAllSTRef(H264_CTX_S *pH264Ctx)
*      �������� :  �����ж��ڲο�֡���Ϊ���ο�
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID UnMarkAllSTRef(H264_CTX_S *pH264Ctx)
{
    UINT32 i;

    for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
    {
        UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs_ref[i]);
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MarkCurrPicLT (H264_CTX_S *pH264Ctx, H264_CURRPIC_S *pCurrPic, UINT32 long_term_frame_idx)
*      �������� :  �ѵ�ǰ����ͼ����Ϊ���ڲο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬
*                  long_term_frame_idx�������ǰ����ͼ��ĳ��ڲο�֡����
*      ����ֵ   �� ��
************************************************************************
*/
VOID MarkCurrPicLT(H264_CTX_S *pH264Ctx, H264_CURRPIC_S *pCurrPic, UINT32 long_term_frame_idx)
{
    // remove long term pictures with same long_term_frame_idx
    if (FRAME == pCurrPic->structure)//frame ,field pair
    {
        // mark pic with long_term_frame_idx
        UnMarkLTFrmByFrmIdx(pH264Ctx, long_term_frame_idx);
    }
    else
    {
        UnMarkLTFldByFrmIdx(pH264Ctx, pCurrPic->frame_store, pCurrPic->structure, long_term_frame_idx);
    }

    // mark current pic
    pCurrPic->is_long_term = 1;
    pCurrPic->is_short_term = 0;
    pCurrPic->long_term_frame_idx = long_term_frame_idx;

    return;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID RemoveFrameStoreOutDPB(H264_CTX_S *pH264Ctx, UINT32 pos)
*   �������� :  ��framestoreָ���Ƴ�dpb
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
VOID RemoveFrameStoreOutDPB(H264_CTX_S *pH264Ctx, UINT32 pos)
{
    UINT32 i;
    FSP_LOGIC_FS_S *pstLogic;

    //release APC
    /* �ݴ������õ�ָ��ʱ��������ʲôʱ��Ӧ���ж��Ƿ�Ϊ�� */
    if (NULL == pH264Ctx->DPB.fs[pos])
    {
        return;
    }

    pH264Ctx->APC.is_used[pH264Ctx->DPB.fs[pos]->apc_idc] = 0;
    pH264Ctx->APC.used_size = (0 < pH264Ctx->APC.used_size) ? (pH264Ctx->APC.used_size-1) : 0;

    pstLogic = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->DPB.fs[pos]->logic_fs_id);
    if (NULL != pstLogic)
    {
        if (FS_DISP_STATE_WAIT_DISP != pstLogic->s32DispState)
        {
            FSP_SetDisplay(pH264Ctx->ChanID, pH264Ctx->DPB.fs[pos]->logic_fs_id, 0);
        }

        /* ���߼�֡����Ϊ���ο� */
        FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->DPB.fs[pos]->logic_fs_id, 0);
    }

    pH264Ctx->DPB.fs[pos]->is_used = 0;
    pH264Ctx->DPB.fs[pos]->is_in_dpb = 0;
    pH264Ctx->DPB.fs[pos]->is_reference = 0;

    for (i=pos; i<pH264Ctx->DPB.used_size-1; i++)
    {
        pH264Ctx->DPB.fs[i] = pH264Ctx->DPB.fs[i+1];
    }
    pH264Ctx->DPB.fs[pH264Ctx->DPB.used_size-1] = (H264_FRAMESTORE_S *)NULL;
    pH264Ctx->DPB.used_size--;

    return;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 RemoveUnUsedFrameStore(H264_CTX_S *pH264Ctx)
*   �������� :  ����Ѿ���g_DPB����ķǲο�֡������Ѿ�vo��ʾ������������frame store ������ֵ
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
UINT32 RemoveUnUsedFrameStore(H264_CTX_S *pH264Ctx)
{
    UINT32 i, j, k;
    FSP_LOGIC_FS_S  *pstLogicFs = NULL;

    // check for frames that were already output and no longer used for reference
    /* z56361, 20111107, pH264Ctx->DPB.used_size����ֱ����Ϊ����ѭ���Ŀ��Ʊ�����������ΪRemove���������ʹused_size
       ��1����������ѭ�����Ʊ�������ʹ��DPB�е�ĳЩ֡ʵ���ϲ�δ���������Ӷ����±��ô�DPBɾ����֡��Ȼ����DPB��. ������
       DPB��FSP����Ϣ�ǳ�ͻ�ģ����������ܶ�����.
       ͬ�ݿ����������ɴ�����ԭ����FSP��DPB��ͻ���GetAPC()ʧ�ܣ����������DPB�����¶�֡. */
    k = pH264Ctx->DPB.used_size;
    for (i=0,j=0; i<k; i++)
    {
        if((pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->DPB.fs[j]->logic_fs_id)) != NULL)
        {
            /* ���fsp֡���Ѿ����У�Ҫȷ����֡������DPB��ɾ�� */
            if (pstLogicFs->s32IsRef == 0 && pstLogicFs->s32DispState == FS_DISP_STATE_NOT_USED)
            {
                pH264Ctx->DPB.fs[j]->logic_fs_id = -1;
                pH264Ctx->DPB.fs[j]->is_reference= 0;
                /* �����֡�ǵ�һ֡���ձ������������δ�ֵ������������ʱ��Ҫ�����޳���DPB.
                   ���������Ŀ�����õ�һ֡���л��ᾭ������������������ھ�ȷ����open gop��ͷ2��B֡ */
                pH264Ctx->DPB.fs[j]->is_in_dpb = (pH264Ctx->DPB.fs[j]->eFoState != FO_STATE_FAST_OUTPUT)? 0: 1;
            }
        }
        else
        {
            pH264Ctx->DPB.fs[j]->is_in_dpb = (pH264Ctx->DPB.fs[j]->eFoState != FO_STATE_FAST_OUTPUT)? 0: 1;
        }

        if (!FrameStoreRefFlg(pH264Ctx->DPB.fs[j]) && (!pH264Ctx->DPB.fs[j]->is_in_dpb)
            && (pH264Ctx->DPB.fs[j]->eFoState != FO_STATE_FAST_OUTPUT))
        {
            pH264Ctx->DPB.fs[j]->is_used = 0;
            RemoveFrameStoreOutDPB(pH264Ctx, j);
        }
        else
        {
            j++;
        }
    }

    return 0;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 Check_FrameStore(H264DEC_FrameStore *pFrameStore)
*   �������� :  ����ͼ���Ƿ��ܹ������VO����
*   ����˵�� :
*   ����ֵ   �� 0:�������;-1�������
************************************************************************
*/
SINT32 CheckFrameStore(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore)
{
    FSP_LOGIC_FS_S *pstLogicFs;
    SINT32  already_output_flag = 0;

    /* CRCУ��ʱ��I֡ǰB֡Ҳ��� */
    if (pH264Ctx->pstExtraData->s8SpecMode != 1)
    {
        if (0 == pH264Ctx->IDRFound)
        {
            /* z56361, 20111206,
            �ú����ǰ���ʾ����õģ�֮ǰ��ͨ������"��һ����B֡"�ж�open gop��ǰ����B֡�Ѿ�������ϣ����ڴ����ʱ�������⡣
            ����ȡ�������������֡����"��B֡"��һ��I��+B����ɵ�֡��Ӧ��ΪB֡������264Э���У�������֡�п��ܾ��ǿɶ��������֡����I֡.
            ���ԣ���֡�����ж�open gop��ǰ����B֡�Ƿ��꣬�߼��ϴ���ȱ��.
            �޸ķ���: ��Ϊ"��ʾ���ڵ�һ������֮֡ǰ��֡������". �����һ֡��I��+B����ɣ�ֻҪ���ܽ⣬�����Ͼ͵�ͬ��I֡. */
            if (pFrameStore->eFoState == FO_STATE_FAST_OUTPUT || pH264Ctx->OldDecMode != IPB_MODE ||
                pH264Ctx->pstExtraData->s32DecOrderOutput != NOT_DEC_ORDER)
            {
            	pH264Ctx->IDRFound = 1;
            }
            else
            {
            	return H264_ERR;
            }
        }
    }

    pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pFrameStore->logic_fs_id);
    if (pstLogicFs != NULL)
    {
        if ((pstLogicFs->s32DispState == FS_DISP_STATE_WAIT_DISP) || (pstLogicFs->s32DispState == FS_DISP_STATE_NOT_USED))
        {
            already_output_flag = 1;
            dprint(PRN_ERROR, "frame %d, state=%d, is already output\n", pFrameStore->logic_fs_id, pstLogicFs->s32DispState);
        }
    }
    else
    {
        dprint(PRN_ERROR,"plfs(fs=%p, eFoState=%d) is null, logic_fs_id = %d\n",pFrameStore, pFrameStore->eFoState, pFrameStore->logic_fs_id);
        already_output_flag = 1;
    }

    if(already_output_flag == 1)
    {
        /* �����֡�ǿ������֡�������Ѿ���������������򣬺�������ǿ������Ȼ����DPB��. */
        if (pFrameStore->eFoState == FO_STATE_FAST_OUTPUT)
        {
            pFrameStore->eFoState = FO_STATE_NORMAL_ORDER_OUTPUT;
        }
        return H264_ERR_FS_OUT_AGAIN;
    }

    // the control condition would added
    if ((0==pFrameStore->is_used) || (1==pFrameStore->non_existing))
    {
        dprint(PRN_ERROR, "not used frame: (%d,%d)\n",pFrameStore->is_used, pFrameStore->non_existing);
        return H264_ERR;
    }

    if ((3 == pFrameStore->is_used) && (3 == pFrameStore->mode_skip_flag))
    {
        dprint(PRN_ERROR, "frame skip: is_used=%d,skip=%d\n", pFrameStore->is_used, pFrameStore->mode_skip_flag);
        return H264_ERR;
    }

    if ((1 == pFrameStore->is_used) && (1 == (pFrameStore->mode_skip_flag & 1)))
    {
        dprint(PRN_ERROR, "topfield skip: is_used=%d,skip=%d\n", pFrameStore->is_used, pFrameStore->mode_skip_flag);
        return H264_ERR;
    }

    if ((2 == pFrameStore->is_used) && (2 == (pFrameStore->mode_skip_flag & 2)))
    {
        dprint(PRN_ERROR, "bottom field skip: is_used=%d,skip=%d\n", pFrameStore->is_used, pFrameStore->mode_skip_flag);
        return H264_ERR;
    }

    if (pstLogicFs->stDispImg.error_level > pH264Ctx->pstExtraData->s32OutErrThr)
    {
        dprint(PRN_ERROR, "err(%d) > out_thr(%d)\n", pstLogicFs->stDispImg.error_level, pH264Ctx->pstExtraData->s32OutErrThr);
        return H264_ERR;
    }

    return H264_OK;
}


VOID ExchangePts(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore)
{
    UINT64 MinPts = (UINT64)(-2); //0xfffffffffffffffe;
    SINT32 i, MinPtsPos = -1;
    FSP_LOGIC_FS_S *pstLogicFs;

    /* �����ǰ֡��PTSΪ-1���򲻽��� */
    if (pFrameStore->fs_image.PTS == (UINT64)(-1))
    {
        return;
    }

    /* ��DPB����С��PTS */
    for (i = 0; i < pH264Ctx->DPB.used_size; i++)
    {
        if (pH264Ctx->DPB.fs[i] != NULL)
        {
            if (NULL != (pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->DPB.fs[i]->logic_fs_id)))
            {
                if (pstLogicFs->s32DispState == FS_DISP_STATE_WAIT_DISP || pstLogicFs->s32DispState == FS_DISP_STATE_NOT_USED)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            if (pH264Ctx->DPB.fs[i]->fs_image.PTS < MinPts)
            {
                MinPtsPos = i;
                MinPts = pH264Ctx->DPB.fs[i]->fs_image.PTS;
            }
        }
    }

    /* �����СPTS֡���ǵ�ǰ֡����ǰ֡���以��PTS */
    if (MinPtsPos != -1)
    {
        if (pFrameStore->fs_image.PTS > MinPts)
        {
            dprint(PRN_PTS,"exg_pts: %lld <-> %lld\n", pFrameStore->fs_image.PTS, MinPts);
            pH264Ctx->DPB.fs[MinPtsPos]->fs_image.PTS = pFrameStore->fs_image.PTS;
            pFrameStore->fs_image.PTS = MinPts;
        }
    }
    return;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID GetImagePara(H264_CTX_S *pH264Ctx, H264DEC_FrameStore *pFrameStore)
*   �������� :  ��ֵҪ�����framestore��fs_image�ṹ��
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
VOID GetImagePara(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore)
{
    UINT32 pic_type = pFrameStore->pic_type;
    UINT32 source_format;
    UINT32 valid_field;
    UINT8  top_field_first = 2;
    IMAGE  *pstLogicFsImage;
    H264_SPS_S *sps;
    H264_PPS_S *pps;

    pps = &pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id];
    sps = &pH264Ctx->SPS[pps->seq_parameter_set_id];

    //clean format
    //pFrameStore->fs_image.format &= CLEAN_VIDEO_FORMAT;
    pFrameStore->fs_image.format &= CLEAN_SOURCE_FORMAT;
    pFrameStore->fs_image.format &= CLEAN_FRAME_TYPE;

    pFrameStore->fs_image.frame_rate = pH264Ctx->frame_rate * 1024;

    switch (pFrameStore->is_used)
    {
    /* ���ڵ�һ֡�����������ʹ��һ֡���(���ô˺���)ʱis_used��δ��������ͨ��Ϊ0��Ϊ����format������ֶγ���
       ����Ԥ�ϵ���ֵ������Ҳ��is_used = 3�ķ�֧ */
    case 0:
    case 3:
        if (0 == pFrameStore->frame.structure)
        {
            pFrameStore->fs_image.error_level = pFrameStore->err_level;
            valid_field = 3;
            pic_type = pFrameStore->pic_type;
            if (0 == pFrameStore->MbaffFrameFlag)
            {
                //progresive frame
                source_format = 0x2;
            }
            else
            {
                //mbaff frame
                source_format = 0x3;
            }
        }
        else
        {
            //field pair
            source_format = 0x3;

            //according  to err_level, modify valid_field and output level
            if (((pFrameStore->top_field.err_level > pH264Ctx->out_error_thr) &&
                (pFrameStore->bottom_field.err_level > pH264Ctx->out_error_thr)) ||
                ((pFrameStore->top_field.err_level <= pH264Ctx->out_error_thr) &&
                (pFrameStore->bottom_field.err_level <= pH264Ctx->out_error_thr))
                )
            {
                //all field err_level smaller than out_error_thr or bigger than out_error_thr,  don't modify
                pFrameStore->fs_image.error_level =  pFrameStore->err_level;
                valid_field = 3;
                pic_type = pFrameStore->pic_type;
            }
            else if ((pFrameStore->top_field.err_level > pH264Ctx->out_error_thr) &&
                (pFrameStore->bottom_field.err_level <= pH264Ctx->out_error_thr))
            {
                //top field err don't dispaly, modify that bottom field valid only
                pFrameStore->fs_image.error_level =  pFrameStore->bottom_field.err_level;
                valid_field = 2;
                pic_type = pFrameStore->bottom_field.pic_type;
            }
            else
            {
                //bottom field err don't dispaly, modify that top field valid only
                pFrameStore->fs_image.error_level =  pFrameStore->top_field.err_level;
                valid_field = 1;
                pic_type = pFrameStore->top_field.pic_type;
            }

            if (1 == pFrameStore->mode_skip_flag)
            {
                //top field not dec and display bottom field only
                pFrameStore->fs_image.error_level =  pFrameStore->bottom_field.err_level;
                valid_field = 2;
                pic_type = pFrameStore->bottom_field.pic_type;
            }
            else if (2 == pFrameStore->mode_skip_flag)
            {
                //bottom field not dec and display top field only
                pFrameStore->fs_image.error_level =  pFrameStore->top_field.err_level;
                valid_field = 1;
                pic_type = pFrameStore->top_field.pic_type;
            }
    		pFrameStore->fs_image.top_fld_type = pFrameStore->top_field.pic_type;
    		pFrameStore->fs_image.bottom_fld_type = pFrameStore->bottom_field.pic_type;
        }

        //although some field cann't display some time becouse of err_level, its poc is right for judge display order
        if (pFrameStore->top_field.poc == pFrameStore->bottom_field.poc)
        {
            top_field_first = ((pFrameStore->fs_image.image_width==720)&&(pFrameStore->fs_image.image_height==480)) ? 0 : 1;
        }
        else if (pFrameStore->top_field.poc > pFrameStore->bottom_field.poc)
        {
            top_field_first = 0;
        }
        else
        {
            //TopPOC smaller than BottomPOC, so the field display first  is top field
            top_field_first = 1;
        }

        /* ���SEI���Ѿ����˳�����SEI����Ϣ */
        if (pH264Ctx->PicTimingSEI.pic_struct == 3)
        {
            top_field_first = 1;
        }
        else if (pH264Ctx->PicTimingSEI.pic_struct == 4)
        {
            top_field_first = 0;
        }

        pFrameStore->fs_image.format &= CLEAN_TOP_FILED_FIRST;
        pFrameStore->fs_image.format = pFrameStore->fs_image.format |
            (top_field_first & 0x3 ) << 12 |
            ( valid_field & 0x3) << 10 |
            (source_format & 0x3) << 8 |
            (pic_type & 0x3);
        break;
    case  2:
        source_format = 0x3;
        pic_type = pFrameStore->pic_type;
        pFrameStore->fs_image.format = pFrameStore->fs_image.format |
            0x2 << 10 |
            (source_format & 0x3) << 8 |
            (pic_type & 0x3);
        pFrameStore->fs_image.error_level =  pFrameStore->err_level;
        break;
    case  1:
        source_format = 0x3;
        pic_type = pFrameStore->pic_type;
        pFrameStore->fs_image.format = pFrameStore->fs_image.format |
            0x1 << 10 |
            (source_format & 0x3) << 8 |
            (pic_type & 0x3);
        pFrameStore->fs_image.error_level =  pFrameStore->err_level;
        break;
    default:
        break;
    }

    /* �������������������ǿ��ӵ绰Ӧ�ã���ʱ��Դ��ʽӦ�������� */
    if (pH264Ctx->pstExtraData->s32DecOrderOutput > NOT_DEC_ORDER &&
        0 == pFrameStore->frame.structure && pFrameStore->is_used == 3)
    {
        pFrameStore->fs_image.format &= CLEAN_SOURCE_FORMAT;  // ����
    }

    ExchangePts(pH264Ctx, pFrameStore);

    /* if it is a 3D frame, assign its packing type */
    if (pH264Ctx->FramePackSEI.frame_packing_dec_ok)
    {
        if (pH264Ctx->FramePackSEI.frame_packing_arrangement_type == 3)
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_SIDE_BY_SIDE;
        }
        else if (pH264Ctx->FramePackSEI.frame_packing_arrangement_type == 4)
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_TOP_BOTTOM;
        }
        else if (pH264Ctx->FramePackSEI.frame_packing_arrangement_type == 5)
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_TIME_INTERLACED;
        }
        else
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_NONE;
            dprint(PRN_FATAL,"stream frame packing type is %d! what can we do?\n", pH264Ctx->FramePackSEI.frame_packing_arrangement_type);
        }
    }

    pstLogicFsImage = FSP_GetFsImagePtr(pH264Ctx->ChanID, pFrameStore->logic_fs_id);
    if (NULL != pstLogicFsImage)
    {
        memcpy(pstLogicFsImage, &pFrameStore->fs_image, sizeof(IMAGE));
    }

    return;
}


VOID SetFrmRepeatCount(H264_CTX_S *pH264Ctx, IMAGE *p_image)
{
    if (!((pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed > -2048)&&(pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed <= 2048)))
    {
        if (pH264Ctx->NewPicCounter/2 >= (pH264Ctx->PicOutCounter/2 + 1 + pH264Ctx->DPB.size))
        {
            if ((pH264Ctx->NewPicCounter/2 - (pH264Ctx->PicOutCounter/2 + 1 + pH264Ctx->DPB.size)) >= 2)
            {
                p_image->u32RepeatCnt = 2;
                pH264Ctx->PicOutCounter+=4;
            }
			else
            {
                p_image->u32RepeatCnt = 1;
                pH264Ctx->PicOutCounter+=2;
            }
        }
		else
        {
            p_image->u32RepeatCnt = 0;
        }
    }
	else
    {
        p_image->u32RepeatCnt = 0;
    }

    return;
}

/*!
************************************************************************
*   ����ԭ�� :  SINT32 OutputFrmToVO(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore, UINT32 is_last_frame)
*   �������� :  ���һ֡ͼ��
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
SINT32 OutputFrmToVO(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore, UINT32 is_last_frame)
{
    SINT32 ret;
    IMAGE *pstLogicFsImage;

    if ((NULL == pH264Ctx) || (NULL == pFrameStore))
    {
        dprint(PRN_FATAL, "pH264Ctx is %p, pFrameStore is %p\n", pH264Ctx, pFrameStore);
        return H264_OUTPUT_FAIL;
    }

    /* ��������������ʾ��������������������˴����ñ�Ҫ�ı�־�������������ͼ��.
       ע��: I֡ģʽ�²��������·�֧����ΪI֡ģʽ��direct ouput��Ч���ϵ�ͬ�ڽ�����
             ���; ����direct output���������������ͼ����������ķ�֧���ܽ�.*/
	if (pH264Ctx->pstExtraData->s32DecOrderOutput > NOT_DEC_ORDER && (pH264Ctx->OldDecMode != 2))
    {
        // ������ �ͻ�������ȫ�Ƕ���, �����򡣵��µ�����: ��ʱ��᲻���ͼ��
        // �����һ����ʱ��, ����ֻ��һ������, ����û�����, ��һ���������Ƕ���, ���԰�ǰһ�����,
		//�������е������ʱ��, �����ǽ�����, �߽������֧, �����Ѿ����, ������������Ͷ���

        if (NULL != pH264Ctx->pDirectOutBuf)
        {
            pH264Ctx->pDirectOutBuf->is_used = 0;
            pH264Ctx->pDirectOutBuf = (H264_FRAMESTORE_S *)NULL;
        }
        return H264_ALREADY_OUT;
    }

    /* pFrameStore�����Ѿ�����������������뼰ʱ��ֹ���֡�Ĳ���.
       �������֡���ߵ������ԭ���ǣ�ĳЩ֡��������������ο������Ի���DPB��п����ٴα����г������. */
    ret = CheckFrameStore(pH264Ctx, pFrameStore);
    if (H264_ERR_FS_OUT_AGAIN == ret)
    {
        return H264_ALREADY_OUT;
    }

    if ((H264_FRAMESTORE_S *)NULL != pH264Ctx->pDirectOutBuf)
    {
        GetImagePara(pH264Ctx, pH264Ctx->pDirectOutBuf);
        ret = CheckFrameStore(pH264Ctx, pH264Ctx->pDirectOutBuf);
        if (H264_OK == ret)
        {
            FSP_SetDisplay(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id, 1);
            pstLogicFsImage = FSP_GetFsImagePtr(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id);

            if (NULL == pstLogicFsImage)
            {
                dprint(PRN_FATAL, "line; %d pstLogicFsImage is NULL!\n", __LINE__);
                return H264_ERR;
            }

            if (pFrameStore == pH264Ctx->pDirectOutBuf && 1 == is_last_frame)
            {
                pstLogicFsImage->last_frame = 1;
            }
            
            ret = InsertImgToVoQueue(pH264Ctx->ChanID, VFMW_H264, pH264Ctx, &pH264Ctx->ImageQue, pstLogicFsImage);
            if (VF_OK != ret)
            {
                dprint(PRN_FATAL, "InsertVO err:%d, ClearAll\n", ret);

                //!! ������г�ʱ����Ҫ�˳�
                pH264Ctx->pDirectOutBuf->is_used = 0;
                FSP_SetDisplay(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id, 0);
                pH264Ctx->pDirectOutBuf = (H264_FRAMESTORE_S *)NULL;
                ClearAll(pH264Ctx,1);
                return H264_ERR;  //OUT_FLAG
            }
            SetFrmRepeatCount(pH264Ctx, pstLogicFsImage);
            pH264Ctx->numOutputFrame++;
            pH264Ctx->PicOutCounter += 2;
        }
        else if (H264_ERR_FS_OUT_AGAIN != ret)
        {
            dprint(PRN_ERROR, "H264.c line %d: frame para err(ret=%d), recycle image self\n", __LINE__, ret);
            H264DEC_RecycleImage(pH264Ctx, pH264Ctx->pDirectOutBuf->fs_image.image_id);
        }

        pH264Ctx->pDirectOutBuf->is_used = 0;
        if (pFrameStore == pH264Ctx->pDirectOutBuf)
        {
            pH264Ctx->pDirectOutBuf = (H264_FRAMESTORE_S *)NULL;
            return H264_OK;
        }
        else
        {
            pH264Ctx->pDirectOutBuf = (H264_FRAMESTORE_S *)NULL;
        }
    }

    GetImagePara(pH264Ctx, pFrameStore);

    ret = CheckFrameStore(pH264Ctx, pFrameStore);
    if (H264_OK == ret)
    {
        FSP_SetDisplay(pH264Ctx->ChanID, pFrameStore->logic_fs_id, 1);
        pstLogicFsImage = FSP_GetFsImagePtr(pH264Ctx->ChanID, pFrameStore->logic_fs_id);

        if (NULL == pstLogicFsImage)
        {
            dprint(PRN_FATAL,"line: %d pstLogicFsImage is NULL!\n", __LINE__);
            return H264_ERR;
        }

        if (1 == is_last_frame)
		{
	        pstLogicFsImage->last_frame = 1;
		}
        
        ret = InsertImgToVoQueue(pH264Ctx->ChanID, VFMW_H264, pH264Ctx, &pH264Ctx->ImageQue, pstLogicFsImage);
        if (VF_OK != ret)
        {
            //!! ������г�ʱ����Ҫ�˳�
            dprint(PRN_FATAL, "InsertVO err:%d, ClearAll\n", ret);
            FSP_SetDisplay(pH264Ctx->ChanID, pFrameStore->logic_fs_id, 0);
            ClearAll(pH264Ctx,1);
            return H264_ERR;  //OUT_FLAG
        }
        SetFrmRepeatCount(pH264Ctx, pstLogicFsImage);
        pH264Ctx->numOutputFrame++;
        pH264Ctx->PicOutCounter += 2;
    }
    else if (H264_ERR_FS_OUT_AGAIN == ret)
    {
        return H264_ALREADY_OUT;
    }
    else
    {
        dprint(PRN_ERROR, "H264.c line %d: frame para err(ret=%d), recycle image self\n", __LINE__, ret);
        H264DEC_RecycleImage(pH264Ctx, pFrameStore->fs_image.image_id);
        return H264_OUTPUT_FAIL;  //OUT_FLAG
    }

    return H264_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 OutputFrmFromDPB(H264_CTX_S *pH264Ctx,, UINT32 pos)
*   �������� :  ��g_DPB�����һ֡��ͬʱ��һ֡��DPB��ɾ��
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
SINT32 OutputFrmFromDPB(H264_CTX_S *pH264Ctx, UINT32 pos)
{
    SINT32 ret;

    pH264Ctx->DPB.fs[pos]->is_in_dpb = 0;  // here, output g_DPB to display
    ret = OutputFrmToVO(pH264Ctx, pH264Ctx->DPB.fs[pos], 0);

    // free frame store and move empty store to end of buffer
    if (!FrameStoreRefFlg(pH264Ctx->DPB.fs[pos]))
    {
        RemoveFrameStoreOutDPB(pH264Ctx, pos);
    }

    return ret;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID SplitFrmToFlds(H264_FRAMESTORE_S *pFrameStore)
*      �������� :  ��һ֡�ֳ�����
*      ����˵�� :  pFrameStore:��������֡��
*      ����ֵ   �� ��
*
************************************************************************
*/
VOID SplitFrmToFlds(H264_FRAMESTORE_S *pFrameStore)
{
    pFrameStore->top_field.is_long_term = pFrameStore->frame.is_long_term;
    pFrameStore->top_field.is_short_term = pFrameStore->frame.is_short_term;
    pFrameStore->top_field.structure = TOP_FIELD;
    pFrameStore->top_field.err_level = pFrameStore->frame.err_level;
    pFrameStore->top_field.pic_type = pFrameStore->frame.pic_type;
    pFrameStore->top_field.frame_store = pFrameStore->frame.frame_store;

    pFrameStore->bottom_field.is_long_term = pFrameStore->frame.is_long_term;
    pFrameStore->bottom_field.is_short_term = pFrameStore->frame.is_short_term;
    pFrameStore->bottom_field.structure = BOTTOM_FIELD;
    pFrameStore->bottom_field.err_level = pFrameStore->frame.err_level;
    pFrameStore->bottom_field.pic_type = pFrameStore->frame.pic_type;
    pFrameStore->bottom_field.frame_store = pFrameStore->frame.frame_store;

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID CombineFldsToFrm(H264_FRAMESTORE_S *pFrameStore)
*      �������� :  �ɶԵ���������֡��ƴ��һ֡
*      ����˵�� :  pFrameStore:��������֡��
*      ����ֵ   �� ��
*
************************************************************************
*/
VOID CombineFldsToFrm(H264_FRAMESTORE_S *pFrameStore)
{
    pFrameStore->poc = pFrameStore->frame.poc = MIN(pFrameStore->top_field.poc, pFrameStore->bottom_field.poc);
    pFrameStore->frame.structure = FIELD_PAIR; // field pair more important
    pFrameStore->frame.err_level = (pFrameStore->top_field.err_level + pFrameStore->bottom_field.err_level) / 2;
    pFrameStore->frame.frame_store = pFrameStore->top_field.frame_store;

    pFrameStore->frame.is_long_term = pFrameStore->bottom_field.is_long_term && pFrameStore->top_field.is_long_term;
    pFrameStore->frame.is_short_term =( pFrameStore->bottom_field.is_short_term  || pFrameStore->bottom_field.is_long_term ) &&
        ( pFrameStore->top_field.is_short_term || pFrameStore->top_field.is_long_term) &&
        (!pFrameStore->frame.is_long_term);

    //pic type combine
    /* z56361, 20111205, ԭ����֡��pic_type���ɹ�����:ֻҪ��һ����I������Ϊ��֡��I֡. �˹��򲻺ϳ����޸����� */
#if 0
    pFrameStore->frame.pic_type =
                ( (0==pFrameStore->top_field.pic_type) || (0==pFrameStore->bottom_field.pic_type) )?
                0:
                MAX(pFrameStore->top_field.pic_type, pFrameStore->bottom_field.pic_type);
    pFrameStore->pic_type = pFrameStore->frame.pic_type;
#else
    pFrameStore->pic_type = pFrameStore->frame.pic_type =
        MAX(pFrameStore->top_field.pic_type, pFrameStore->bottom_field.pic_type);
#endif
    pFrameStore->err_level = pFrameStore->frame.err_level;

    return;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32    GetAPC(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore)
*   �������� :  ����apc��û�ֵ�ʱ���ش���
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
SINT32 GetAPC(H264_CTX_S *pH264Ctx, H264_FRAMESTORE_S *pFrameStore, UINT32 structure)
{
    UINT32 i;
    SINT32 toppoc, bottompoc;
    FSP_LOGIC_FS_S *pstLogicFs;

    CHECK_NULL_PTR_ReturnValue(pFrameStore, H264_ERR);

    switch (structure)
    {
    case TOP_FIELD:
        toppoc = pFrameStore->top_field.poc;
        bottompoc = pFrameStore->top_field.poc;
        break;
    case BOTTOM_FIELD:
        toppoc = pFrameStore->bottom_field.poc;
        bottompoc = pFrameStore->bottom_field.poc;
        break;
    case FRAME:
    default:
        toppoc = pFrameStore->top_field.poc;
        bottompoc = pFrameStore->bottom_field.poc;
    }

    for (i = 0; i < pH264Ctx->APC.size; i++)
    {
        if (0 == pH264Ctx->APC.is_used[i])
        {
            //apc is not used, allocate
            pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pFrameStore->logic_fs_id);
            if (NULL != pstLogicFs && NULL != pstLogicFs->pstDecodeFs)
            {
                pH264Ctx->APC.is_used[i] = 1;
                pH264Ctx->APC.idc[i] = pstLogicFs->pstDecodeFs->PhyFsID;
                pH264Ctx->APC.poc[0][i] = toppoc;
                pH264Ctx->APC.poc[1][i] = bottompoc;
                pH264Ctx->APC.used_size = (16 <= pH264Ctx->APC.used_size) ? 16 : (pH264Ctx->APC.used_size+1);
                dprint(PRN_REF, "framestore %d allocate apc %d\n", pFrameStore->fs_image.image_id, i);

                pFrameStore->apc_idc = i;
                return H264_OK;
            }
            else
            {
                dprint(PRN_FATAL,"find APC, but logic_fs_id %d abnormal(ref=%d,plfs=%p, pdfs=%p)\n", pFrameStore->logic_fs_id,
                    pFrameStore->is_reference, pstLogicFs, (pstLogicFs)? pstLogicFs->pstDecodeFs: NULL);
                return H264_ERR;
            }
        }
    }

    return H264_ERR;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID InsertFrmInDPB(H264_CTX_S pH264Ctx, UINT32 pos , H264_CURRPIC_S *pCurrPic)
*   �������� :  ����һ��ͼ��g_DPB
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
SINT32 InsertFrmInDPB(H264_CTX_S *pH264Ctx, UINT32 pos, H264_CURRPIC_S *pCurrPic)
{
    SINT32 ret;
    UINT64 OtherFieldPts = 0;
    UINT64 OtherFieldUsertag = 0;
    UINT64 OtherFieldDispTime = 0;
    UINT32 OtherFieldDispEnableFlag = 0;
    UINT32 OtherFieldDispFrameDistance = 0;
    UINT32 OtherFieldDistanceBeforeFirstFrame = 0;
    UINT32 OtherFieldGopNum = 0;

    // PTS ��ֵȱ
    switch (pCurrPic->structure)
    {
    case FRAME:
        pH264Ctx->DPB.fs[pos] = pCurrPic->frame_store;
        pH264Ctx->DPB.fs[pos]->non_existing = pCurrPic->non_existing;
        pH264Ctx->DPB.fs[pos]->is_used = 3;
        pH264Ctx->DPB.fs[pos]->is_in_dpb = pCurrPic->frame_store->is_in_dpb;
        pH264Ctx->DPB.fs[pos]->is_displayed = pCurrPic->frame_store->is_displayed;
        pH264Ctx->DPB.fs[pos]->is_reference = (pCurrPic->is_long_term ? 3 : (pCurrPic->is_short_term?3:0));
        pH264Ctx->DPB.fs[pos]->pmv_address_idc = pCurrPic->pmv_address_idc;
        pH264Ctx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;
        pH264Ctx->DPB.fs[pos]->err_level = pH264Ctx->DPB.fs[pos]->frame.err_level = pCurrPic->err_level;
        pH264Ctx->DPB.fs[pos]->frame.frame_store = pH264Ctx->DPB.fs[pos]->top_field.frame_store \
            = pH264Ctx->DPB.fs[pos]->bottom_field.frame_store = pCurrPic->frame_store;
        pH264Ctx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx; // ������Ч��Ϊ��ʡ�ж�ֱ�Ӹ�ֵ
        pH264Ctx->DPB.fs[pos]->frame_num = pCurrPic->frame_num;
        pH264Ctx->DPB.fs[pos]->frame.structure = FRAME;
        pH264Ctx->DPB.fs[pos]->frame.is_long_term = pCurrPic->is_long_term;
        pH264Ctx->DPB.fs[pos]->frame.is_short_term = pCurrPic->is_short_term;
        pH264Ctx->DPB.fs[pos]->pic_type = pH264Ctx->DPB.fs[pos]->frame.pic_type = pCurrPic->pic_type;
        pH264Ctx->DPB.fs[pos]->MbaffFrameFlag = pCurrPic->MbaffFrameFlag;

        pH264Ctx->DPB.fs[pos]->poc = pCurrPic->thispoc;
        pH264Ctx->DPB.fs[pos]->frame.poc = pCurrPic->framepoc;
        pH264Ctx->DPB.fs[pos]->top_field.poc = pCurrPic->toppoc;
        pH264Ctx->DPB.fs[pos]->bottom_field.poc = pCurrPic->bottompoc;

        pH264Ctx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ? 3 : 0;

        SplitFrmToFlds(pH264Ctx->DPB.fs[pos]);
        ret = GetAPC(pH264Ctx, pCurrPic->frame_store, FRAME);
        if (H264_OK != ret)
        {
            dprint(PRN_FATAL,"line %d, get APC error, ret %d\n", __LINE__, ret);
            ClearDPB(pH264Ctx);  //APC_ERR
            return H264_ERR;
        }
        break;
    case TOP_FIELD:
        pCurrPic->frame_store->frame.structure = FIELD_PAIR;
        if (0 == pCurrPic->field_pair_flag || (H264_FRAMESTORE_S *)NULL == pH264Ctx->DPB.fs[pos])
        {
            // δ�ɶ�
            pH264Ctx->DPB.fs[pos] = pCurrPic->frame_store;

            pH264Ctx->DPB.fs[pos]->top_field.structure = TOP_FIELD;
            pH264Ctx->DPB.fs[pos]->top_field.frame_store = pCurrPic->frame_store;
            pH264Ctx->DPB.fs[pos]->top_field.is_long_term = pCurrPic->is_long_term;
            pH264Ctx->DPB.fs[pos]->top_field.is_short_term = pCurrPic->is_short_term;
            pH264Ctx->DPB.fs[pos]->top_field.poc = pCurrPic->toppoc;
            pH264Ctx->DPB.fs[pos]->top_field.pic_type = pCurrPic->pic_type;

            pH264Ctx->DPB.fs[pos]->top_field.err_level = pCurrPic->err_level;
            pH264Ctx->DPB.fs[pos]->err_level = pH264Ctx->DPB.fs[pos]->frame.err_level = pH264Ctx->DPB.fs[pos]->top_field.err_level;

            pH264Ctx->DPB.fs[pos]->non_existing = pCurrPic->non_existing; // must be 0
            pH264Ctx->DPB.fs[pos]->is_used = 1;
            pH264Ctx->DPB.fs[pos]->is_in_dpb = pCurrPic->frame_store->is_in_dpb;
            pH264Ctx->DPB.fs[pos]->is_displayed = pCurrPic->frame_store->is_displayed;
            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pH264Ctx->DPB.fs[pos]->is_reference = 1;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pH264Ctx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pH264Ctx->DPB.fs[pos]->MbaffFrameFlag = 0;
            pH264Ctx->DPB.fs[pos]->fs_image = pCurrPic->fs_image; // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            pH264Ctx->DPB.fs[pos]->pmv_address_idc = pCurrPic->pmv_address_idc;
            pH264Ctx->DPB.fs[pos]->frame_num = pCurrPic->frame_num;
            pH264Ctx->DPB.fs[pos]->poc = pCurrPic->thispoc;
            pH264Ctx->DPB.fs[pos]->pic_type = pCurrPic->pic_type;

            //g_DPB.fs[pos]->mode_skip_flag = pCurrPic->mode_skip_flag;
            pH264Ctx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ? 1 : 0;
            ret = GetAPC(pH264Ctx, pCurrPic->frame_store, TOP_FIELD);
            if (H264_OK != ret)
            {
                dprint(PRN_FATAL,"line %d, get APC error, ret %d\n", __LINE__, ret);
                ClearDPB(pH264Ctx);  //APC_ERR
                return H264_ERR;
            }
        }
        else
        {
            pH264Ctx->DPB.fs[pos]->top_field.structure = TOP_FIELD;
            pH264Ctx->DPB.fs[pos]->top_field.pic_type = pCurrPic->pic_type;
            pH264Ctx->DPB.fs[pos]->top_field.is_long_term = pCurrPic->is_long_term;
            pH264Ctx->DPB.fs[pos]->top_field.is_short_term = pCurrPic->is_short_term;
            pH264Ctx->DPB.fs[pos]->top_field.poc = pCurrPic->toppoc;
            pH264Ctx->DPB.fs[pos]->top_field.pic_type = pCurrPic->pic_type;

            pH264Ctx->DPB.fs[pos]->top_field.err_level = pCurrPic->err_level;
            pH264Ctx->DPB.fs[pos]->err_level = pH264Ctx->DPB.fs[pos]->frame.err_level =
                pH264Ctx->DPB.fs[pos]->top_field.err_level + pH264Ctx->DPB.fs[pos]->bottom_field.err_level;

            // �ɶԣ�frame_store�еĹ����������Բ��ٸ�ֵ��ֻ��ֵ��������ı��
            pH264Ctx->DPB.fs[pos]->non_existing = pH264Ctx->DPB.fs[pos]->non_existing ? pCurrPic->non_existing : 0;
            pH264Ctx->DPB.fs[pos]->is_used |= 1;
            //g_DPB.fs[pos]->mode_skip_flag |= pCurrPic->mode_skip_flag;
            pH264Ctx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ?
                (pH264Ctx->DPB.fs[pos]->mode_skip_flag | 1) : pH264Ctx->DPB.fs[pos]->mode_skip_flag;
            OtherFieldPts = pH264Ctx->DPB.fs[pos]->fs_image.PTS;
            OtherFieldUsertag = pH264Ctx->DPB.fs[pos]->fs_image.Usertag;
            OtherFieldDispTime = pH264Ctx->DPB.fs[pos]->fs_image.DispTime;
            OtherFieldDispEnableFlag = pH264Ctx->DPB.fs[pos]->fs_image.DispEnableFlag;
            OtherFieldDispFrameDistance = pH264Ctx->DPB.fs[pos]->fs_image.DispFrameDistance;
            OtherFieldDistanceBeforeFirstFrame = pH264Ctx->DPB.fs[pos]->fs_image.DistanceBeforeFirstFrame;
            OtherFieldGopNum = pH264Ctx->DPB.fs[pos]->fs_image.GopNum;

            pH264Ctx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����

            if(pCurrPic->fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pH264Ctx->DPB.fs[pos]->fs_image.PTS = OtherFieldPts;
            }
            pH264Ctx->DPB.fs[pos]->fs_image.Usertag = (OtherFieldUsertag < pCurrPic->fs_image.Usertag)?
                OtherFieldUsertag : pCurrPic->fs_image.Usertag;
            pH264Ctx->DPB.fs[pos]->fs_image.DispTime= (OtherFieldDispTime < pCurrPic->fs_image.DispTime)?
                OtherFieldDispTime : pCurrPic->fs_image.DispTime;
            pH264Ctx->DPB.fs[pos]->fs_image.DispEnableFlag = (OtherFieldDispEnableFlag < pCurrPic->fs_image.DispEnableFlag)?
                OtherFieldDispEnableFlag : pCurrPic->fs_image.DispEnableFlag;
            pH264Ctx->DPB.fs[pos]->fs_image.DispFrameDistance= (OtherFieldDispFrameDistance < pCurrPic->fs_image.DispFrameDistance)?
                OtherFieldDispFrameDistance : pCurrPic->fs_image.DispFrameDistance;
            pH264Ctx->DPB.fs[pos]->fs_image.DistanceBeforeFirstFrame= (OtherFieldDistanceBeforeFirstFrame < pCurrPic->fs_image.DistanceBeforeFirstFrame)?
                OtherFieldDistanceBeforeFirstFrame : pCurrPic->fs_image.DistanceBeforeFirstFrame;
            pH264Ctx->DPB.fs[pos]->fs_image.GopNum = (OtherFieldGopNum < pCurrPic->fs_image.GopNum)?
                OtherFieldGopNum : pCurrPic->fs_image.GopNum;

            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pH264Ctx->DPB.fs[pos]->is_reference |= 1;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pH264Ctx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pH264Ctx->DPB.fs[pos]->pic_type = MAX(pH264Ctx->DPB.fs[pos]->top_field.pic_type, pH264Ctx->DPB.fs[pos]->bottom_field.pic_type);
            pH264Ctx->DPB.fs[pos]->poc = pCurrPic->thispoc;

            CombineFldsToFrm(pH264Ctx->DPB.fs[pos]);

            //modify apc poc
            pH264Ctx->APC.poc[0][pH264Ctx->DPB.fs[pos]->apc_idc] = pH264Ctx->DPB.fs[pos]->top_field.poc;
        }
        break;
    case BOTTOM_FIELD:
        pCurrPic->frame_store->frame.structure = FIELD_PAIR;
        if (0 == pCurrPic->field_pair_flag || (H264_FRAMESTORE_S *)NULL == pH264Ctx->DPB.fs[pos])
        {
            // δ�ɶ�
            pH264Ctx->DPB.fs[pos] = pCurrPic->frame_store;

            pH264Ctx->DPB.fs[pos]->bottom_field.structure = BOTTOM_FIELD;
            pH264Ctx->DPB.fs[pos]->bottom_field.frame_store = pCurrPic->frame_store;
            pH264Ctx->DPB.fs[pos]->bottom_field.is_long_term = pCurrPic->is_long_term;
            pH264Ctx->DPB.fs[pos]->bottom_field.is_short_term = pCurrPic->is_short_term;
            pH264Ctx->DPB.fs[pos]->bottom_field.poc = pCurrPic->bottompoc;
            pH264Ctx->DPB.fs[pos]->bottom_field.pic_type = pCurrPic->pic_type;

            pH264Ctx->DPB.fs[pos]->bottom_field.err_level = pCurrPic->err_level;
            pH264Ctx->DPB.fs[pos]->err_level = pH264Ctx->DPB.fs[pos]->frame.err_level = pH264Ctx->DPB.fs[pos]->top_field.err_level;

            pH264Ctx->DPB.fs[pos]->non_existing = pCurrPic->non_existing;  //must be 0
            pH264Ctx->DPB.fs[pos]->is_used = 2;
            pH264Ctx->DPB.fs[pos]->is_in_dpb = pCurrPic->frame_store->is_in_dpb;
            pH264Ctx->DPB.fs[pos]->is_displayed = pCurrPic->frame_store->is_displayed;
            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pH264Ctx->DPB.fs[pos]->is_reference = 2;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pH264Ctx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pH264Ctx->DPB.fs[pos]->MbaffFrameFlag = 0;
            pH264Ctx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            pH264Ctx->DPB.fs[pos]->pmv_address_idc = pCurrPic->pmv_address_idc;
            pH264Ctx->DPB.fs[pos]->frame_num = pCurrPic->frame_num;
            pH264Ctx->DPB.fs[pos]->poc = pCurrPic->thispoc;
            pH264Ctx->DPB.fs[pos]->pic_type = pH264Ctx->DPB.fs[pos]->bottom_field.pic_type = pCurrPic->pic_type;

            //g_DPB.fs[pos]->mode_skip_flag = pCurrPic->mode_skip_flag;
            pH264Ctx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ? 2 : 0;

            ret = GetAPC(pH264Ctx, pCurrPic->frame_store, BOTTOM_FIELD);
            if (H264_OK != ret)
            {
                dprint(PRN_FATAL,"line %d, get APC error, ret %d\n", __LINE__, ret);
                ClearDPB(pH264Ctx);  //APC_ERR
                return H264_ERR;
            }
        }
        else
        {
            // �ɶԣ�frame_store�еĹ����������Բ��ٸ�ֵ��ֻ��ֵ��������ı��
            pH264Ctx->DPB.fs[pos]->bottom_field.structure = BOTTOM_FIELD;
            pH264Ctx->DPB.fs[pos]->bottom_field.frame_store = pCurrPic->frame_store;
            pH264Ctx->DPB.fs[pos]->bottom_field.is_long_term = pCurrPic->is_long_term;
            pH264Ctx->DPB.fs[pos]->bottom_field.is_short_term = pCurrPic->is_short_term;
            pH264Ctx->DPB.fs[pos]->bottom_field.poc = pCurrPic->bottompoc;
            pH264Ctx->DPB.fs[pos]->bottom_field.pic_type = pCurrPic->pic_type;

            pH264Ctx->DPB.fs[pos]->bottom_field.err_level = pCurrPic->err_level;
            pH264Ctx->DPB.fs[pos]->err_level = pH264Ctx->DPB.fs[pos]->frame.err_level =
                pH264Ctx->DPB.fs[pos]->top_field.err_level + pH264Ctx->DPB.fs[pos]->bottom_field.err_level;

            pH264Ctx->DPB.fs[pos]->non_existing = pH264Ctx->DPB.fs[pos]->non_existing ? pCurrPic->non_existing : 0;
            pH264Ctx->DPB.fs[pos]->is_used |= 2;
            //g_DPB.fs[pos]->mode_skip_flag |= pCurrPic->mode_skip_flag;
            pH264Ctx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ?
                (pH264Ctx->DPB.fs[pos]->mode_skip_flag | 2) : pH264Ctx->DPB.fs[pos]->mode_skip_flag;
            OtherFieldPts = pH264Ctx->DPB.fs[pos]->fs_image.PTS;
            OtherFieldUsertag = pH264Ctx->DPB.fs[pos]->fs_image.Usertag;
            OtherFieldDispTime = pH264Ctx->DPB.fs[pos]->fs_image.DispTime;
            OtherFieldDispEnableFlag = pH264Ctx->DPB.fs[pos]->fs_image.DispEnableFlag;
            OtherFieldDispFrameDistance = pH264Ctx->DPB.fs[pos]->fs_image.DispFrameDistance;
            OtherFieldDistanceBeforeFirstFrame = pH264Ctx->DPB.fs[pos]->fs_image.DistanceBeforeFirstFrame;
            OtherFieldGopNum = pH264Ctx->DPB.fs[pos]->fs_image.GopNum;

            pH264Ctx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����

            if(pCurrPic->fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pH264Ctx->DPB.fs[pos]->fs_image.PTS = OtherFieldPts;
            }
            pH264Ctx->DPB.fs[pos]->fs_image.Usertag = (OtherFieldUsertag < pCurrPic->fs_image.Usertag)?
                OtherFieldUsertag : pCurrPic->fs_image.Usertag;
            pH264Ctx->DPB.fs[pos]->fs_image.DispTime = (OtherFieldDispTime < pCurrPic->fs_image.DispTime)?
                OtherFieldDispTime : pCurrPic->fs_image.DispTime;
            pH264Ctx->DPB.fs[pos]->fs_image.DispEnableFlag = (OtherFieldDispEnableFlag < pCurrPic->fs_image.DispEnableFlag)?
                OtherFieldDispEnableFlag : pCurrPic->fs_image.DispEnableFlag;
            pH264Ctx->DPB.fs[pos]->fs_image.DispFrameDistance= (OtherFieldDispFrameDistance < pCurrPic->fs_image.DispFrameDistance)?
                OtherFieldDispFrameDistance : pCurrPic->fs_image.DispFrameDistance;
            pH264Ctx->DPB.fs[pos]->fs_image.DistanceBeforeFirstFrame= (OtherFieldDistanceBeforeFirstFrame < pCurrPic->fs_image.DistanceBeforeFirstFrame)?
                OtherFieldDistanceBeforeFirstFrame : pCurrPic->fs_image.DistanceBeforeFirstFrame;
            pH264Ctx->DPB.fs[pos]->fs_image.GopNum = (OtherFieldGopNum < pCurrPic->fs_image.GopNum)?
                OtherFieldGopNum : pCurrPic->fs_image.GopNum;

            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pH264Ctx->DPB.fs[pos]->is_reference |= 2;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pH264Ctx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pH264Ctx->DPB.fs[pos]->pic_type = MAX(pH264Ctx->DPB.fs[pos]->top_field.pic_type, pH264Ctx->DPB.fs[pos]->bottom_field.pic_type);
            pH264Ctx->DPB.fs[pos]->poc = pCurrPic->thispoc;
            CombineFldsToFrm(pH264Ctx->DPB.fs[pos]);

            //modify apc poc
            pH264Ctx->APC.poc[1][pH264Ctx->DPB.fs[pos]->apc_idc] = pH264Ctx->DPB.fs[pos]->bottom_field.poc;
        }
        break;
    default:
        // �ڳ��������£�Ϊ�˲��ı�DPB״����ֻ����ǰͼ����Ϊ�գ�
        dprint(PRN_FATAL,"InserFrmInDPB: cur pic struct = %d!\n", pCurrPic->structure);
        ClearAll(pH264Ctx,1);
        return H264_OK;
    }

    // ��ǰͼ���ÿ�
    //pCurrPic->state = EMPTY;

    /* �߼�֡���Ϊ���ο� */
    if (0 != pH264Ctx->DPB.fs[pos]->is_reference)
    {
        FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->DPB.fs[pos]->logic_fs_id, 1);
    }

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 DirectOutput(H264_CTX_S *pH264Ctx)
*      �������� :  ��ǰ����ͼ�����ο�������poc��DPB������û�������ͼ��
*                  ��poc��С
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL:��������
*                  OUT_FLAG:�²㺯�����ܷ��ظ�ֵ��ϵͳҪ���˳���
*
************************************************************************
*/
SINT32 DirectOutput(H264_CTX_S *pH264Ctx)
{
    SINT32 ret = H264_OK;
    UINT64 OtherFieldPts = 0;
    UINT64 OtherFieldUsertag = 0;
    UINT64 OtherFieldDisptime = 0;
    UINT32 OtherFieldDispEnableFlag = 0;
    UINT32 OtherFieldDispFrameDistance = 0;
    UINT32 OtherFieldDistanceBeforeFirstFrame = 0;
    UINT32 OtherFieldGopNum = 0;

    switch (pH264Ctx->CurrPic.structure)
    {
    case FRAME:
        pH264Ctx->CurrPic.frame_store->frame.structure = FRAME;
        // �����directbuf���ٰ����ڽ����ͼ�����directbuf
        if ((H264_FRAMESTORE_S *)NULL != pH264Ctx->pDirectOutBuf)
        {
            ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
            if(pH264Ctx->pDirectOutBuf != NULL)
            {
                FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
            }
            pH264Ctx->pDirectOutBuf = NULL;
        }
        pH264Ctx->pDirectOutBuf = pH264Ctx->CurrPic.frame_store;
        pH264Ctx->pDirectOutBuf->frame.frame_store = pH264Ctx->CurrPic.frame_store;
        pH264Ctx->pDirectOutBuf->top_field.frame_store = pH264Ctx->CurrPic.frame_store;
        pH264Ctx->pDirectOutBuf->bottom_field.frame_store = pH264Ctx->CurrPic.frame_store;

        pH264Ctx->pDirectOutBuf->non_existing = pH264Ctx->CurrPic.non_existing;
        pH264Ctx->pDirectOutBuf->is_used = 3;
        pH264Ctx->pDirectOutBuf->is_reference = 0;
        pH264Ctx->pDirectOutBuf->is_in_dpb = 0;
        pH264Ctx->pDirectOutBuf->is_displayed = 0;
        pH264Ctx->pDirectOutBuf->fs_image = pH264Ctx->CurrPic.fs_image;
        pH264Ctx->pDirectOutBuf->err_level = pH264Ctx->pDirectOutBuf->frame.err_level = pH264Ctx->CurrPic.err_level;
        pH264Ctx->pDirectOutBuf->frame.structure = FRAME;
        pH264Ctx->pDirectOutBuf->pic_type = pH264Ctx->CurrPic.pic_type;
        pH264Ctx->pDirectOutBuf->MbaffFrameFlag = pH264Ctx->CurrPic.MbaffFrameFlag;
        pH264Ctx->pDirectOutBuf->mode_skip_flag = (pH264Ctx->CurrPic.mode_skip_flag == 1) ? 3 : 0;

        FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
        ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
        if(pH264Ctx->pDirectOutBuf != NULL)
        {
            FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
        }
        pH264Ctx->pDirectOutBuf = NULL;
        break;
    case TOP_FIELD:
        pH264Ctx->CurrPic.frame_store->frame.structure = FIELD_PAIR;
        if (0 == pH264Ctx->CurrPic.field_pair_flag)
        {
            // ���ɶ�
            if ((H264_FRAMESTORE_S *)NULL != pH264Ctx->pDirectOutBuf)
            {
                // ֮ǰ����һ������Ҫ���
                ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
                pH264Ctx->pDirectOutBuf = pH264Ctx->CurrPic.frame_store;
                pH264Ctx->pDirectOutBuf->top_field.frame_store = pH264Ctx->CurrPic.frame_store;
            }
            else
            {
                pH264Ctx->pDirectOutBuf = pH264Ctx->CurrPic.frame_store;
                pH264Ctx->pDirectOutBuf->top_field.frame_store = pH264Ctx->CurrPic.frame_store;
            }
            pH264Ctx->pDirectOutBuf->non_existing = pH264Ctx->CurrPic.non_existing;  //must be 0
            pH264Ctx->pDirectOutBuf->is_used = 1;
            pH264Ctx->pDirectOutBuf->is_in_dpb = 0;
            pH264Ctx->pDirectOutBuf->is_displayed = 0;
            pH264Ctx->pDirectOutBuf->is_reference = 0;
            pH264Ctx->pDirectOutBuf->pic_type = pH264Ctx->CurrPic.pic_type;
            pH264Ctx->pDirectOutBuf->MbaffFrameFlag = 0;
            pH264Ctx->pDirectOutBuf->fs_image = pH264Ctx->CurrPic.fs_image;  // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            pH264Ctx->pDirectOutBuf->top_field.err_level = pH264Ctx->CurrPic.err_level;
            pH264Ctx->pDirectOutBuf->err_level = pH264Ctx->pDirectOutBuf->frame.err_level = pH264Ctx->pDirectOutBuf->top_field.err_level;
            pH264Ctx->pDirectOutBuf->top_field.structure = TOP_FIELD;
            pH264Ctx->pDirectOutBuf->mode_skip_flag = (pH264Ctx->CurrPic.mode_skip_flag == 1) ? 1 : 0;

            if (1 == pH264Ctx->IModeOutFlag)
            {
                FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
                ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
                if(pH264Ctx->pDirectOutBuf != NULL)
                {
                    FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
                }
                pH264Ctx->pDirectOutBuf = NULL;
            }
        }
        else
        {
            // �ɶԣ�����pDirectOutBufһ������
            if ((H264_FRAMESTORE_S *)NULL == pH264Ctx->pDirectOutBuf)
            {
                pH264Ctx->pDirectOutBuf = pH264Ctx->CurrPic.frame_store;
                pH264Ctx->pDirectOutBuf->top_field.frame_store = pH264Ctx->CurrPic.frame_store;
            }
            /* 20111212: �����ɶ�ʱ���轫������Ч�ĳ� pts ���� �ɶԵ� frame */
            OtherFieldPts = pH264Ctx->pDirectOutBuf->fs_image.PTS;
            OtherFieldUsertag = pH264Ctx->pDirectOutBuf->fs_image.Usertag;
            OtherFieldDisptime = pH264Ctx->pDirectOutBuf->fs_image.DispTime;
            OtherFieldDispEnableFlag = pH264Ctx->pDirectOutBuf->fs_image.DispEnableFlag;
            OtherFieldDispFrameDistance = pH264Ctx->pDirectOutBuf->fs_image.DispFrameDistance;
            OtherFieldDistanceBeforeFirstFrame = pH264Ctx->pDirectOutBuf->fs_image.DistanceBeforeFirstFrame;
            OtherFieldGopNum = pH264Ctx->pDirectOutBuf->fs_image.GopNum;

            pH264Ctx->pDirectOutBuf->fs_image = pH264Ctx->CurrPic.fs_image;  // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����

            if(pH264Ctx->CurrPic.fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pH264Ctx->pDirectOutBuf->fs_image.PTS = OtherFieldPts;
            }
            pH264Ctx->pDirectOutBuf->fs_image.Usertag = (OtherFieldUsertag < pH264Ctx->CurrPic.fs_image.Usertag)?
                OtherFieldUsertag : pH264Ctx->CurrPic.fs_image.Usertag;
            pH264Ctx->pDirectOutBuf->fs_image.DispTime= (OtherFieldDisptime < pH264Ctx->CurrPic.fs_image.DispTime)?
                OtherFieldDisptime : pH264Ctx->CurrPic.fs_image.DispTime;
            pH264Ctx->pDirectOutBuf->fs_image.DispEnableFlag = (OtherFieldDispEnableFlag < pH264Ctx->CurrPic.fs_image.DispEnableFlag)?
                OtherFieldDispEnableFlag : pH264Ctx->CurrPic.fs_image.DispEnableFlag;
            pH264Ctx->pDirectOutBuf->fs_image.DispFrameDistance= (OtherFieldDispFrameDistance < pH264Ctx->CurrPic.fs_image.DispFrameDistance)?
                OtherFieldDispFrameDistance : pH264Ctx->CurrPic.fs_image.DispFrameDistance;
            pH264Ctx->pDirectOutBuf->fs_image.DistanceBeforeFirstFrame= (OtherFieldDistanceBeforeFirstFrame < pH264Ctx->CurrPic.fs_image.DistanceBeforeFirstFrame)?
                OtherFieldDistanceBeforeFirstFrame : pH264Ctx->CurrPic.fs_image.DistanceBeforeFirstFrame;
            pH264Ctx->pDirectOutBuf->fs_image.GopNum = (OtherFieldGopNum < pH264Ctx->CurrPic.fs_image.GopNum)?
                OtherFieldGopNum : pH264Ctx->CurrPic.fs_image.GopNum;

            pH264Ctx->pDirectOutBuf->non_existing = pH264Ctx->pDirectOutBuf->non_existing ? pH264Ctx->CurrPic.non_existing : 0;
            pH264Ctx->pDirectOutBuf->is_used |= 1;
            pH264Ctx->pDirectOutBuf->pic_type = MAX(pH264Ctx->CurrPic.pic_type, pH264Ctx->pDirectOutBuf->pic_type);
            pH264Ctx->pDirectOutBuf->top_field.structure = TOP_FIELD;
            pH264Ctx->pDirectOutBuf->frame.structure = FIELD_PAIR;  // field pair more important

            pH264Ctx->pDirectOutBuf->top_field.err_level = pH264Ctx->CurrPic.err_level;
            pH264Ctx->pDirectOutBuf->frame.err_level = (pH264Ctx->pDirectOutBuf->top_field.err_level + pH264Ctx->pDirectOutBuf->bottom_field.err_level)/2;
            pH264Ctx->pDirectOutBuf->err_level = pH264Ctx->pDirectOutBuf->frame.err_level;

            pH264Ctx->pDirectOutBuf->frame.frame_store = pH264Ctx->pDirectOutBuf->top_field.frame_store;
            pH264Ctx->pDirectOutBuf->mode_skip_flag = (pH264Ctx->CurrPic.mode_skip_flag == 1) ?
                (pH264Ctx->pDirectOutBuf->mode_skip_flag | 1) : pH264Ctx->pDirectOutBuf->mode_skip_flag;
            FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
            ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
            if(pH264Ctx->pDirectOutBuf != NULL)
            {
                FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
            }
            pH264Ctx->pDirectOutBuf = NULL;
        }

        //pH264Ctx->CurrPic.state = EMPTY;
        break;
    case BOTTOM_FIELD:
        pH264Ctx->CurrPic.frame_store->frame.structure = FIELD_PAIR;
        if (0 == pH264Ctx->CurrPic.field_pair_flag)
        {
            // ���ɶ�
            if ((H264_FRAMESTORE_S *)NULL != pH264Ctx->pDirectOutBuf)
            {
                // ֮ǰ����һ������Ҫ���
                ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
                pH264Ctx->pDirectOutBuf = pH264Ctx->CurrPic.frame_store;
                pH264Ctx->pDirectOutBuf->bottom_field.frame_store = pH264Ctx->CurrPic.frame_store;
            }
            else
            {
                pH264Ctx->pDirectOutBuf = pH264Ctx->CurrPic.frame_store;
                pH264Ctx->pDirectOutBuf->bottom_field.frame_store = pH264Ctx->CurrPic.frame_store;
            }
            pH264Ctx->pDirectOutBuf->non_existing = pH264Ctx->CurrPic.non_existing;  //must be 0
            pH264Ctx->pDirectOutBuf->is_used = 2;
            pH264Ctx->pDirectOutBuf->is_in_dpb = 0;
            pH264Ctx->pDirectOutBuf->is_displayed = 0;
            pH264Ctx->pDirectOutBuf->is_reference = 0;
            pH264Ctx->pDirectOutBuf->pic_type = pH264Ctx->CurrPic.pic_type;
            pH264Ctx->pDirectOutBuf->MbaffFrameFlag = 0;
            pH264Ctx->pDirectOutBuf->fs_image = pH264Ctx->CurrPic.fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����

            pH264Ctx->pDirectOutBuf->bottom_field.err_level = pH264Ctx->CurrPic.err_level;
            pH264Ctx->pDirectOutBuf->err_level = pH264Ctx->pDirectOutBuf->frame.err_level = pH264Ctx->pDirectOutBuf->bottom_field.err_level;

            pH264Ctx->pDirectOutBuf->bottom_field.structure = BOTTOM_FIELD;
            pH264Ctx->pDirectOutBuf->mode_skip_flag = (pH264Ctx->CurrPic.mode_skip_flag == 1) ? 2 : 0;

            if (1 == pH264Ctx->IModeOutFlag)
            {
                FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
                ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
                if(pH264Ctx->pDirectOutBuf != NULL)
                {
                    FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
                }
                pH264Ctx->pDirectOutBuf = NULL;
            }
        }
        else
        {
            // �ɶԣ�����pDirectOutBufһ������
            if ((H264_FRAMESTORE_S *)NULL == pH264Ctx->pDirectOutBuf)
            {
                pH264Ctx->pDirectOutBuf = pH264Ctx->CurrPic.frame_store;
                pH264Ctx->pDirectOutBuf->bottom_field.frame_store = pH264Ctx->CurrPic.frame_store;
            }
            /* 20111212: �����ɶ�ʱ���轫������Ч�ĳ� pts ���� �ɶԵ� frame */
            OtherFieldPts = pH264Ctx->pDirectOutBuf->fs_image.PTS;
            OtherFieldUsertag = pH264Ctx->pDirectOutBuf->fs_image.Usertag;
            OtherFieldDisptime = pH264Ctx->pDirectOutBuf->fs_image.DispTime;
            OtherFieldDispEnableFlag = pH264Ctx->pDirectOutBuf->fs_image.DispEnableFlag;
            OtherFieldDispFrameDistance = pH264Ctx->pDirectOutBuf->fs_image.DispFrameDistance;
            OtherFieldDistanceBeforeFirstFrame = pH264Ctx->pDirectOutBuf->fs_image.DistanceBeforeFirstFrame;
            OtherFieldGopNum = pH264Ctx->pDirectOutBuf->fs_image.GopNum;

            pH264Ctx->pDirectOutBuf->fs_image = pH264Ctx->CurrPic.fs_image; // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����

            if(pH264Ctx->CurrPic.fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pH264Ctx->pDirectOutBuf->fs_image.PTS = OtherFieldPts;
            }
            pH264Ctx->pDirectOutBuf->fs_image.Usertag = (OtherFieldUsertag < pH264Ctx->CurrPic.fs_image.Usertag)?
                OtherFieldUsertag : pH264Ctx->CurrPic.fs_image.Usertag;
            pH264Ctx->pDirectOutBuf->fs_image.DispTime = (OtherFieldDisptime < pH264Ctx->CurrPic.fs_image.DispTime)?
                OtherFieldDisptime : pH264Ctx->CurrPic.fs_image.DispTime;
            pH264Ctx->pDirectOutBuf->fs_image.DispEnableFlag = (OtherFieldDispEnableFlag < pH264Ctx->CurrPic.fs_image.DispEnableFlag)?
                OtherFieldDispEnableFlag : pH264Ctx->CurrPic.fs_image.DispEnableFlag;
            pH264Ctx->pDirectOutBuf->fs_image.DispFrameDistance= (OtherFieldDispFrameDistance < pH264Ctx->CurrPic.fs_image.DispFrameDistance)?
                OtherFieldDispFrameDistance : pH264Ctx->CurrPic.fs_image.DispFrameDistance;
            pH264Ctx->pDirectOutBuf->fs_image.DistanceBeforeFirstFrame= (OtherFieldDistanceBeforeFirstFrame < pH264Ctx->CurrPic.fs_image.DistanceBeforeFirstFrame)?
                OtherFieldDistanceBeforeFirstFrame : pH264Ctx->CurrPic.fs_image.DistanceBeforeFirstFrame;
            pH264Ctx->pDirectOutBuf->fs_image.GopNum = (OtherFieldGopNum < pH264Ctx->CurrPic.fs_image.GopNum)?
                OtherFieldGopNum : pH264Ctx->CurrPic.fs_image.GopNum;

            pH264Ctx->pDirectOutBuf->non_existing = pH264Ctx->pDirectOutBuf->non_existing ? pH264Ctx->CurrPic.non_existing : 0;
            pH264Ctx->pDirectOutBuf->is_used |= 2;
            pH264Ctx->pDirectOutBuf->pic_type = MAX(pH264Ctx->CurrPic.pic_type,pH264Ctx->pDirectOutBuf->pic_type);
            pH264Ctx->pDirectOutBuf->bottom_field.structure = BOTTOM_FIELD;
            pH264Ctx->pDirectOutBuf->frame.structure = FIELD_PAIR; // field pair more important

            pH264Ctx->pDirectOutBuf->bottom_field.err_level = pH264Ctx->CurrPic.err_level;
            pH264Ctx->pDirectOutBuf->frame.err_level = (pH264Ctx->pDirectOutBuf->top_field.err_level + pH264Ctx->pDirectOutBuf->bottom_field.err_level)/2;
            pH264Ctx->pDirectOutBuf->err_level = pH264Ctx->pDirectOutBuf->frame.err_level;

            pH264Ctx->pDirectOutBuf->frame.frame_store = pH264Ctx->pDirectOutBuf->bottom_field.frame_store;
            pH264Ctx->pDirectOutBuf->mode_skip_flag = (pH264Ctx->CurrPic.mode_skip_flag == 1) ?
                (pH264Ctx->pDirectOutBuf->mode_skip_flag | 2) : pH264Ctx->pDirectOutBuf->mode_skip_flag;
            FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
            ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
            if (pH264Ctx->pDirectOutBuf != NULL)
            {
                FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
            }
            pH264Ctx->pDirectOutBuf = NULL;
        }
        //pH264Ctx->CurrPic.state = EMPTY;
        break;
    default:
        //pH264Ctx->CurrPic.state = EMPTY;
        break;
    }

    return ret;
}


/*!
************************************************************************
*      ����ԭ�� :      SINT32 H264DEC_FlushDPB(H264_CTX_S *pH264Ctx)
*      �������� :      ȫ�����g_DPB����ǰͼ��
*      ����˵�� :
*      ����ֵ   ��
???????�����ǰͼ��ʹ�ʱDPB��ͼ��ɶԣ���ô����
************************************************************************
*/
SINT32 FlushDPB(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    SINT32 ret;
    UINT32 pos;
    SINT32 min_poc;

    // mark all frames unused
    for (i=0; i<pH264Ctx->DPB.used_size; i++)
    {
        if (0 != pH264Ctx->DPB.fs[i]->is_reference)
        {
            UnMarkFrameStoreRef (pH264Ctx, pH264Ctx->DPB.fs[i]);
        }
    }

    while ( RemoveUnUsedFrameStore(pH264Ctx) );

    // output frames in POC order
    while (pH264Ctx->DPB.used_size)
    {
        GetMinPOC(pH264Ctx, &min_poc, &pos);
        ret = OutputFrmFromDPB(pH264Ctx, pos);
        if (H264_ERR == ret)
        {
            return H264_ERR;
        }
    }

    return H264_OK;
}

/*!
************************************************************************
*      ����ԭ�� :      SINT32 H264DEC_FlushDPB(H264_CTX_S *pH264Ctx)
*      �������� :      ȫ�����g_DPB����ǰͼ��, ����������һ֡
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
SINT32 H264DEC_GetRemainImg(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    SINT32 ret = LAST_OUTPUT_FAIL;
    UINT32 pos;
    SINT32 min_poc;

    // mark all frames unused
    for (i=0; i<pH264Ctx->DPB.used_size; i++)
    {
        if (0 != pH264Ctx->DPB.fs[i]->is_reference)
        {
            UnMarkFrameStoreRef (pH264Ctx, pH264Ctx->DPB.fs[i]);
        }
    }

    while ( RemoveUnUsedFrameStore(pH264Ctx) );

    // output frames in POC order
    while (pH264Ctx->DPB.used_size)
    {
        GetMinPOC(pH264Ctx, &min_poc, &pos);

        pH264Ctx->DPB.fs[pos]->is_in_dpb = 0;  // here, output fs to display
        ret = OutputFrmToVO(pH264Ctx, pH264Ctx->DPB.fs[pos], pH264Ctx->DPB.used_size);

        // free frame store and move empty store to end of buffer
        if (!FrameStoreRefFlg(pH264Ctx->DPB.fs[pos]))
        {
            RemoveFrameStoreOutDPB(pH264Ctx, pos);
        }
    }

    if (LAST_OUTPUT_OK != ret)
    {
        ret = GetVoLastImageID(&pH264Ctx->ImageQue);
    }

    return ret;
}

/*
************************************************************************
*   ����ԭ�� :  VOID SlidingWinMark(VOID)
*   �������� :  ���������
*   ����˵�� :  ��
*   ����ֵ   �� ��
************************************************************************
*/
VOID SlidingWinMark(H264_CTX_S *pH264Ctx)
{
    UINT32 i;

    // if this is a reference pic with sliding sliding window, unmark first ref frame
    if (pH264Ctx->DPB.ref_frames_in_buffer == pH264Ctx->CurrSPS.num_ref_frames - pH264Ctx->DPB.ltref_frames_in_buffer)
    {
        for (i=0; i<pH264Ctx->DPB.used_size; i++)
        {
            if (0 != pH264Ctx->DPB.fs[i]->is_reference && ShortTermRefFlg(pH264Ctx->DPB.fs[i])) // short term reference
            {
                UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs[i]);
                UpdateReflist(pH264Ctx);
                break;
            }
        }
    }

    return;
}


VOID DumpDPB(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    UINT32 type;

    /* ��Ϣ���͹��� */
    type = 1 << PRN_DPB;
    if( 0 != (g_PrintEnable & type) ) /* ������ӡû�д� */
    {
        // if (20 == pH264Ctx->CurrSlice.frame_num)
        {
            for (i=0; i<pH264Ctx->DPB.used_size; i++)
            {
                dprint(PRN_DPB, "pH264Ctx->DPB.fs[%d]: frame_num=%d poc=%d is_reference=%d\n", i, pH264Ctx->DPB.fs[i]->frame_num, pH264Ctx->DPB.fs[i]->poc, pH264Ctx->DPB.fs[i]->is_reference);
                //_LOG("fs_idc = %d;   pmv_idc=%d\n",g_DPB.fs[i]->fs_image.image_id,g_DPB.fs[i]->pmv_address_idc);
                //_LOG("fram_store = 0x%x\n",g_DPB.fs[i]);
            }
            dprint(PRN_DPB, "\n");
        }

        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            dprint(PRN_DPB, "pH264Ctx->DPB.fs_ref[%d]: frame_num=%d poc=%d is_reference=%d\n", i, pH264Ctx->DPB.fs_ref[i]->frame_num, pH264Ctx->DPB.fs_ref[i]->poc, pH264Ctx->DPB.fs_ref[i]->is_reference);
            dprint(PRN_DPB, "pH264Ctx->DPB.fs_ref[%d]: frame.poc=%d \n", i, pH264Ctx->DPB.fs_ref[i]->frame.poc);
        }
        dprint(PRN_DPB, "\n");
    }

    return;
}


/*
************************************************************************
*      ����ԭ�� :  SINT32 AdaptiveMemMark(H264_CTX_S *pH264Ctx)
*      �������� :  ���ݵ�ǰͼ���﷨Ԫ�ؽ���mmco����
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
SINT32 AdaptiveMemMark(H264_CTX_S *pH264Ctx)
{
    SINT32 ret;
    UINT32 mmco_end_flag = 0;
    UINT32 i;
    H264_MMCO_S *mmco_para = pH264Ctx->CurrPic.pMarkPara->MMCO;

    /* mmco operate*/
    for (i=0; i<MAX_MMCO_LEN; i++)
    {
        switch (mmco_para[i].memory_management_control_operation)
        {
        case 0:
            mmco_end_flag = 1;
            break;
        case 1:
            UnMarkSTRef(pH264Ctx, &pH264Ctx->CurrPic, mmco_para[i].difference_of_pic_nums_minus1);
            UpdateReflist(pH264Ctx);
            break;
        case 2:
            UnMarkLTRef(pH264Ctx, &pH264Ctx->CurrPic, mmco_para[i].long_term_pic_num);
            UpdateLTReflist(pH264Ctx);
            break;
        case 3:
            MarkSTToLTRef(pH264Ctx, &pH264Ctx->CurrPic, mmco_para[i].difference_of_pic_nums_minus1, mmco_para[i].long_term_frame_idx);
            UpdateReflist(pH264Ctx);
            UpdateLTReflist(pH264Ctx);
            break;
        case 4:
            UpdateMaxLTFrmIdx(pH264Ctx, mmco_para[i].max_long_term_frame_idx_plus1);
            UpdateLTReflist(pH264Ctx);
            break;
        case 5:
            UnMarkAllSTRef(pH264Ctx);
            UpdateReflist(pH264Ctx);
            UpdateMaxLTFrmIdx (pH264Ctx, 0);
            UpdateLTReflist(pH264Ctx);
            pH264Ctx->CurrPOC.last_has_mmco_5 = 1;
            break;
        case 6:
            MarkCurrPicLT(pH264Ctx, &pH264Ctx->CurrPic, mmco_para[i].long_term_frame_idx);
            break;
        default :
            break;
        }

        if (1 == mmco_end_flag)
        {
            break;
        }
    }

    /*post process of last_has_mmco_5*/
    if (1 == pH264Ctx->CurrPOC.last_has_mmco_5)
    {
        // updata frame_num and pic_num of CurrPic
        pH264Ctx->CurrPic.frame_num = 0;
        switch (pH264Ctx->CurrPic.structure)
        {
        case TOP_FIELD:
            pH264Ctx->CurrPic.thispoc = pH264Ctx->CurrPic.toppoc = 0;
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.toppoc = 0;
            break;
        case BOTTOM_FIELD:
            pH264Ctx->CurrPic.thispoc = pH264Ctx->CurrPic.bottompoc = 0;
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.bottompoc = 0;
            break;
        case FRAME:
            pH264Ctx->CurrPic.toppoc    -= pH264Ctx->CurrPic.thispoc;
            pH264Ctx->CurrPic.bottompoc -= pH264Ctx->CurrPic.thispoc;
            pH264Ctx->CurrPic.thispoc    = pH264Ctx->CurrPic.framepoc = MIN(pH264Ctx->CurrPic.toppoc, pH264Ctx->CurrPic.bottompoc);
            pH264Ctx->CurrPOC.toppoc    -= pH264Ctx->CurrPOC.ThisPOC;
            pH264Ctx->CurrPOC.bottompoc -= pH264Ctx->CurrPOC.ThisPOC;
            pH264Ctx->CurrPOC.ThisPOC    = pH264Ctx->CurrPOC.framepoc = MIN(pH264Ctx->CurrPOC.toppoc, pH264Ctx->CurrPOC.bottompoc);
            break;
        default:
            break;
        }

        ret = FlushDPB(pH264Ctx);
        if (H264_OK != ret)
        {
            return H264_ERR;
        }
    }

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 IDRMemMarking(H264_CTX_S *pH264Ctx)
*      �������� :  ��ǰͼ��ΪIDRͼ�񣬸����﷨��DPB��ͼ����б��
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
SINT32 IDRMemMarking(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    SINT32 ret;

    // ���ж�֮ǰͼ��Ĵ�������У����IDRͼ���ǵڶ���ʱ����ô�����ͻ��
    if (pH264Ctx->CurrPic.pMarkPara->no_output_of_prior_pics_flag)
    {
        for (i=0; i<pH264Ctx->DPB.used_size; i++)
        {
            // mark Ϊ������,����vo��ʾ�Ѿ�������vo buffer�е�ͼ��ʱ�����ͷ�ͼ���λ
            pH264Ctx->DPB.fs[i]->frame.is_long_term = pH264Ctx->DPB.fs[i]->frame.is_short_term = 0;
            pH264Ctx->DPB.fs[i]->top_field.is_long_term = pH264Ctx->DPB.fs[i]->top_field.is_short_term = 0;
            pH264Ctx->DPB.fs[i]->bottom_field.is_long_term = pH264Ctx->DPB.fs[i]->bottom_field.is_short_term = 0;
            pH264Ctx->DPB.fs[i]->is_reference = 0;
            FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->DPB.fs[i]->logic_fs_id, 0);

            //if (pH264Ctx->DPB.fs[i]->is_in_dpb || (!pH264Ctx->DPB.fs[i]->is_in_dpb && pH264Ctx->DPB.fs[i]->is_displayed))
            {
                // ��ʱ�������:
                // 1: ͼ��δ���g_DPB,���������,�ҿ��Էָ���������
                // 2: ͼ�������vo,���Ѿ���ʾ,ֻ������Ϊ��
                // 3: ͼ�������vo,����û��ʾ,��ͼ����ͷ���vo���
                pH264Ctx->DPB.fs[i]->is_used = 0;
                pH264Ctx->DPB.fs[i]->is_in_dpb = 0;
                pH264Ctx->DPB.fs[i]->is_displayed = 1;
                /* z56361, 2111223, ���֡�涼�ͷŵ��ˣ�����Ӧ��APCҲҪ�ͷŵ�  */
                pH264Ctx->APC.is_used[pH264Ctx->DPB.fs[i]->apc_idc] = 0;

                if (FSP_GetDisplay(pH264Ctx->ChanID, pH264Ctx->DPB.fs[i]->logic_fs_id) != FS_DISP_STATE_WAIT_DISP)
                {
                    FSP_SetDisplay(pH264Ctx->ChanID, pH264Ctx->DPB.fs[i]->logic_fs_id, 0);
                }
            }

            pH264Ctx->DPB.fs[i] = (H264_FRAMESTORE_S *)NULL;
            pH264Ctx->DPB.fs_ref[i] = (H264_FRAMESTORE_S *)NULL;
            pH264Ctx->DPB.fs_ltref[i] = (H264_FRAMESTORE_S *)NULL;
        }
        pH264Ctx->DPB.used_size=0;

        for (i=0; i<pH264Ctx->TotalPmvNum; i++)
        {
            pH264Ctx->PmvStoreUsedFlag[i] = 0; // �ر�ע�⣬��Ҫ�ƻ��˵�ǰͼ��Ĳ�λռ�ù�ϵ
        }
    }
    else
    {
        ret = FlushDPB(pH264Ctx);
        if (H264_OK != ret)
        {
            return H264_ERR;
        }
    }

    UpdateReflist(pH264Ctx);
    UpdateLTReflist(pH264Ctx);

    if (pH264Ctx->CurrPic.pMarkPara->long_term_reference_flag)
    {
        pH264Ctx->DPB.max_long_term_pic_idx = 1;
        pH264Ctx->CurrPic.long_term_frame_idx = 0;
        pH264Ctx->CurrPic.is_long_term = 1;
        pH264Ctx->CurrPic.is_short_term = 0;
    }
    else
    {
        pH264Ctx->DPB.max_long_term_pic_idx = 0; // �������·����ͬ���˴���ʵ������
        pH264Ctx->CurrPic.is_long_term  = 0;
        pH264Ctx->CurrPic.is_short_term = 1;
    }

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 Marking(H264_CTX_S *pH264Ctx)
*      �������� :  ���ݵ�ǰͼ���mark������DPB��ͼ����б�ǣ�
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
SINT32 Marking(H264_CTX_S *pH264Ctx)
{
    SINT32 ret;

    pH264Ctx->CurrPOC.last_has_mmco_5 = 0;
    pH264Ctx->CurrPOC.last_pic_bottom_field = (BOTTOM_FIELD == pH264Ctx->CurrPic.structure);

    /*IDR mark and adaptive mmco mark*/
    if ((NALU_TYPE_IDR == pH264Ctx->CurrPic.nal_unit_type)
		||((pH264Ctx->CurrPic.pic_type==0)
		     &&(((pH264Ctx->pstExtraData->stPvrInfo.u32BFrmRefFlag==1)
		     &&(pH264Ctx->pstExtraData->stPvrInfo.u32IDRFlag==0)
		     &&(pH264Ctx->pstExtraData->stPvrInfo.u32ContinuousFlag==0)
		     &&(!((pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed >= -1024)
		     &&(pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed <= 1024)))))))
    {
        ret = IDRMemMarking(pH264Ctx);
        if (H264_OK != ret)
        {
            return H264_ERR;
        }
    }
    else if (pH264Ctx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag && (0 != pH264Ctx->CurrPic.nal_ref_idc))
    {
        // adaptive memory management
        ret = AdaptiveMemMark(pH264Ctx);
        if (H264_OK != ret)
        {
            return H264_ERR;
        }
    }

    /*sliding win mark*/
    if ((NALU_TYPE_IDR != pH264Ctx->CurrPic.nal_unit_type) && (0 != pH264Ctx->CurrPic.nal_ref_idc) &&
        (!pH264Ctx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag) && (!pH264Ctx->CurrPic.field_pair_flag))
    {
        SlidingWinMark(pH264Ctx);
        pH264Ctx->CurrPic.is_long_term = 0;
    }

    /*mark CurrPic*/
    if (0 != pH264Ctx->CurrPic.nal_ref_idc)
    {
        pH264Ctx->CurrPic.is_short_term = !pH264Ctx->CurrPic.is_long_term;
    }

    return H264_OK;
}

/** ����֡����Ϣ.
    ��һ��picture����DPBʱ�������֡����Ϣ���ɽ�ԭ�����зֲ��ڶദ����ͬ����ϲ����������������Ĵ���.
*/
VOID  UpdateCurrFrameInfo(H264_CTX_S *pH264Ctx)
{
    H264_CURRPIC_S *pCurrPic;
    H264_FRAMESTORE_S *pCurrFrame;

    pCurrPic = &pH264Ctx->CurrPic;
    pCurrFrame = pCurrPic->frame_store;

    /* ��pCurrFrame��ֵ�������ʹ��pCurrFrame��ֵ���ɿأ��˴��ĸ�ֵ��������Ӧ�ķ�֮���������� */
    pCurrFrame->non_existing        = pCurrPic->non_existing;
    pCurrFrame->is_reference        = (pCurrPic->is_long_term ? 3 : (pCurrPic->is_short_term?3:0));
    pCurrFrame->frame_num           = pCurrPic->frame_num;
    pCurrFrame->err_level           = pCurrPic->err_level;
    pCurrFrame->pic_type            = pCurrPic->pic_type;
    pCurrFrame->poc                 = pCurrPic->thispoc;
    pCurrFrame->pmv_address_idc     = pCurrPic->pmv_address_idc;
    pCurrFrame->long_term_frame_idx = pCurrPic->long_term_frame_idx; // ������Ч��Ϊ��ʡ�ж�ֱ�Ӹ�ֵ
    pCurrFrame->MbaffFrameFlag      = pCurrPic->MbaffFrameFlag;
    pCurrFrame->mode_skip_flag      = (pCurrPic->mode_skip_flag == 1) ? 3 : 0;

    /* ��CurrPic��֡�����Է������� */
    switch (pCurrPic->structure)
    {
    case FRAME:
        pCurrFrame->is_used             = 3;

        pCurrFrame->frame.structure     = FRAME;
        pCurrFrame->frame.frame_store   = pCurrFrame;
        pCurrFrame->frame.is_long_term  = pCurrPic->is_long_term;
        pCurrFrame->frame.is_short_term = pCurrPic->is_short_term;
        pCurrFrame->frame.poc           = pCurrPic->framepoc;
        pCurrFrame->frame.pic_type      = pCurrPic->pic_type;
        pCurrFrame->frame.err_level     = pCurrPic->err_level;

        pCurrFrame->top_field.poc       = pCurrPic->toppoc;
        pCurrFrame->bottom_field.poc    = pCurrPic->bottompoc;

        pCurrFrame->fs_image            = pCurrPic->fs_image;

        SplitFrmToFlds(pCurrFrame);
        break;

    case TOP_FIELD:
        pCurrFrame->top_field.structure     = TOP_FIELD;
        pCurrFrame->top_field.frame_store   = pCurrFrame;
        pCurrFrame->top_field.is_long_term  = pCurrPic->is_long_term;
        pCurrFrame->top_field.is_short_term = pCurrPic->is_short_term;
        pCurrFrame->top_field.poc           = pCurrPic->toppoc;
        pCurrFrame->top_field.pic_type      = pCurrPic->pic_type;
        pCurrFrame->top_field.err_level     = pCurrPic->err_level;
        pCurrFrame->MbaffFrameFlag = 0;
        if (0 == pCurrPic->field_pair_flag)
        {
            // δ�ɶ�
            pCurrFrame->is_used         = 1;
            pCurrFrame->err_level       = pCurrFrame->top_field.err_level;
            pCurrFrame->frame.err_level = pCurrFrame->top_field.err_level;
            pCurrFrame->mode_skip_flag  = (pCurrPic->mode_skip_flag == 1) ? 1 : 0;
            pCurrFrame->fs_image        = pCurrPic->fs_image;
        }
        else
        {
            // �����ѽ�, �ɶ�
            pCurrFrame->is_used         = 3;
            pCurrFrame->err_level       = pCurrFrame->top_field.err_level + pCurrFrame->bottom_field.err_level;
            pCurrFrame->frame.err_level = pCurrFrame->err_level;
            pCurrFrame->mode_skip_flag  = (pCurrPic->mode_skip_flag == 1) ? (pCurrFrame->mode_skip_flag | 1) : 0;

            if(pCurrPic->fs_image.PTS != (UINT64)(-1) && pCurrFrame->fs_image.PTS == (UINT64)(-1))
            {
                pCurrFrame->fs_image.PTS = pCurrPic->fs_image.PTS;
            }

            pCurrFrame->pic_type = MAX(pCurrFrame->top_field.pic_type, pCurrFrame->bottom_field.pic_type);
            pCurrFrame->poc = pCurrPic->thispoc;

            CombineFldsToFrm(pCurrFrame);
        }
        break;

    case BOTTOM_FIELD:
        pCurrFrame->bottom_field.structure     = TOP_FIELD;
        pCurrFrame->bottom_field.frame_store   = pCurrFrame;
        pCurrFrame->bottom_field.is_long_term  = pCurrPic->is_long_term;
        pCurrFrame->bottom_field.is_short_term = pCurrPic->is_short_term;
        pCurrFrame->bottom_field.poc           = pCurrPic->toppoc;
        pCurrFrame->bottom_field.pic_type      = pCurrPic->pic_type;
        pCurrFrame->bottom_field.err_level     = pCurrPic->err_level;
        pCurrFrame->MbaffFrameFlag = 0;
        if (0 == pCurrPic->field_pair_flag)
        {
            // δ�ɶ�
            pCurrFrame->is_used         = 2;
            pCurrFrame->err_level       = pCurrFrame->bottom_field.err_level;
            pCurrFrame->frame.err_level = pCurrFrame->bottom_field.err_level;
            pCurrFrame->mode_skip_flag  = (pCurrPic->mode_skip_flag == 1) ? 2 : 0;
            pCurrFrame->fs_image        = pCurrPic->fs_image;
        }
        else
        {
            // �����ѽ�, �ɶ�
            pCurrFrame->is_used         = 3;
            pCurrFrame->err_level       = pCurrFrame->top_field.err_level + pCurrFrame->bottom_field.err_level;
            pCurrFrame->frame.err_level = pCurrFrame->err_level;
            pCurrFrame->mode_skip_flag  = (pCurrPic->mode_skip_flag == 1) ? (pCurrFrame->mode_skip_flag | 2) : 0;

            if(pCurrPic->fs_image.PTS != (UINT64)(-1) && pCurrFrame->fs_image.PTS == (UINT64)(-1))
            {
                pCurrFrame->fs_image.PTS = pCurrPic->fs_image.PTS;
            }

            pCurrFrame->pic_type = MAX(pCurrFrame->top_field.pic_type, pCurrFrame->bottom_field.pic_type);
            pCurrFrame->poc = pCurrPic->thispoc;

            CombineFldsToFrm(pCurrFrame);
        }
        break;
    }
}

/*****************************************************************************
    �򻯵Ļ���dpb�������.
        ��s32DecOrderOutput == DEC_ORDER_SIMPLE_DPBʱʹ�ô˲���.
        ����Ƶ�绰��������;����I֡. ����������������룬��������������ο�ͼ.
    ������Ϊͨ�ý����������ǳ�Σ�գ�����������Ӧ�����Ȱ�DPB������ƽ��м򻯣�
    ��ͨ�������Ĵ��������ñ�׼���ԡ�
 *****************************************************************************/
VOID  SimpleSlideDPB(H264_CTX_S *pH264Ctx)
{
    if(pH264Ctx->DPB.used_size >= pH264Ctx->DPB.size)
    {
        /* �ѵ�0��֡���ʶΪ���ο�������DPB��ɾ�����֡ */
        UnMarkFrameStoreRef(pH264Ctx,pH264Ctx->DPB.fs[0]);
        RemoveFrameStoreOutDPB(pH264Ctx, 0);
    }

    pH264Ctx->CurrPic.is_long_term = 0;

    /*mark CurrPic*/
    if (0 != pH264Ctx->CurrPic.nal_ref_idc)
    {
        pH264Ctx->CurrPic.is_short_term = !pH264Ctx->CurrPic.is_long_term;
    }
}

/*!
************************************************************************
*      ����ԭ�� :  SINT32 StorePicInDPB(H264_CTX_S *pH264Ctx,)
*      �������� :  ���ݵ�ǰͼ���mark������DPB��ͼ����б�ǣ�
*                  �ѵ�ǰͼ�����DPB
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  DPB_ERR: �ڴ���DPBʱ����DPB�����ش������Ҫfirmware����
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
#define STORE_PIC_RET(ret_val) \
do { \
    ret = ret_val;\
    dprint(PRN_DPB,"StorePicInDPB return(%d) from L%d\n",ret_val, __LINE__); \
    goto store_pic_exit;\
} while(0)

SINT32 StorePicInDPB(H264_CTX_S *pH264Ctx, LUMA_INFO_S *pLumaInfo)
{
    SINT32 ret = H264_OK;
    UINT32 i;
    SINT32 min_poc;
    UINT32 pos;
    UINT32 pre_pos       = 0xffffffff;
    UINT32 pre_used_size = 0xffffffff;

    pH264Ctx->CurrPOC.last_has_mmco_5 = 0;
    pH264Ctx->CurrPOC.last_pic_bottom_field = (BOTTOM_FIELD == pH264Ctx->CurrPic.structure);

    /*�����������ǰͼ��Ϊ�գ���Ҫ�˳�*/
    if (EMPTY == pH264Ctx->CurrPic.state)
    {
        //return H264_OK;  //���Կ��Ǳ�һ�������ϲ㴦��
        dprint(PRN_FATAL,"CurrPic.state is 'EMPTY'\n");
        STORE_PIC_RET(H264_ERR);
    }
    pH264Ctx->PrevStorePicCounter = pH264Ctx->PicCounter;

    /******************** ����1: I֡ģʽ�½���directoutput�Լ����� ************************/
    if (2 == pH264Ctx->OldDecMode)
    {
        dprint(PRN_CTRL, "directoutput mode\n");
        ret = DirectOutput(pH264Ctx);

        //pmv��λ���ó�ȱʡֵ
        if (pH264Ctx->CurrPic.pmv_address_idc != pH264Ctx->TotalPmvNum)
        {
            pH264Ctx->PmvStoreUsedFlag[pH264Ctx->CurrPic.pmv_address_idc] = 0;          //pmv store
            pH264Ctx->CurrPic.pmv_address_idc = pH264Ctx->TotalPmvNum;
        }
		if (H264_ALREADY_OUT == ret)
		{
			pH264Ctx->CurrPic.frame_store->is_in_dpb = 0;
			pH264Ctx->CurrPic.frame_store->is_used	 = 0;
		}

        //return ret;
        STORE_PIC_RET(ret);
    }

    /******************** ����2: ��������������ߵ�һ֡������������⴦�� ************************/
    /* ������������: ���CurrPic��֡ͼ��ֱ�����������ǳ�ͼ��ƴ���Ժ������CRC У�鲻����һ֡������� */
    if ((pH264Ctx->pstExtraData->s32DecOrderOutput > NOT_DEC_ORDER || FIRST_DISP_OUTPUT != pH264Ctx->FirstFrameDisplayed)
     && (pH264Ctx->CurrPic.non_existing == 0) && (pH264Ctx->pstExtraData->s8SpecMode != 1))
    {
        H264_FRAMESTORE_S *pFrameStore = pH264Ctx->CurrPic.frame_store;
        IMAGE *pImg;
        UINT32 one_fld_flg = 0;

        one_fld_flg = (TOP_FIELD==pH264Ctx->CurrPic.structure) || (BOTTOM_FIELD==pH264Ctx->CurrPic.structure);

        if ( (pH264Ctx->CurrPic.structure == FRAME) ||
             (pFrameStore->is_used > 0) ||
             one_fld_flg
           )
        {
            if ( one_fld_flg && (FIRST_DISP_INIT == pH264Ctx->FirstFrameDisplayed) )
            {
                pH264Ctx->FirstFrameDisplayed = FIRST_DISP_UNPAIRED;
                pH264Ctx->LastPic = pH264Ctx->CurrPic;
            }
            else if ( (one_fld_flg && (FIRST_DISP_UNPAIRED == pH264Ctx->FirstFrameDisplayed)) ||
                      (pH264Ctx->CurrPic.structure == FRAME) ||
                      (pFrameStore->is_used > 0)
                    )
            {
                H264_CURRPIC_S *pOutPic;
                H264_FRAMESTORE_S *pOutFrameStore;

                if (one_fld_flg && (FIRST_DISP_UNPAIRED == pH264Ctx->FirstFrameDisplayed))
                {
                    //pOutPic = &pH264Ctx->LastPic;
                    pOutPic = &pH264Ctx->CurrPic;
                    pOutFrameStore = pH264Ctx->LastPic.frame_store;
                }
                else
                {
                    pOutPic = &pH264Ctx->CurrPic;
                    pOutFrameStore = pFrameStore;
                }

                /** ����error�Ĵ����ں���������������������ʱ����һ��.
                    ������ĳ�UpdateCurrFrameInfo()�ķ�ʽ������ʱ����if(){}��Ҫɾ�� */
                if (pOutPic->structure != FRAME && pOutPic->field_pair_flag == 1)
                {
                    pOutFrameStore->is_used = 3;
                    pOutFrameStore->err_level += pOutPic->err_level;
                }
                else
                {
                    pOutFrameStore->err_level = pOutPic->err_level;
                }

                GetImagePara(pH264Ctx, pOutFrameStore);
                pImg = FSP_GetFsImagePtr(pH264Ctx->ChanID, pOutFrameStore->logic_fs_id);

                if (NULL == pImg)
                {
                    dprint(PRN_FATAL,"line: %d pImg is NULL!\n", __LINE__);
                    STORE_PIC_RET(H264_ERR);
                }

                if (pOutPic->structure != FRAME && pOutPic->field_pair_flag == 1)
                {
                    if(pOutPic->fs_image.PTS == (UINT64)(-1) && pH264Ctx->LastPic.fs_image.PTS != (UINT64)(-1))
                    {
                        pImg->PTS = pH264Ctx->LastPic.fs_image.PTS;
                    }
                }
                else
                {
                    pImg->PTS = pOutPic->fs_image.PTS;          
                }
                
                //pImg->PTS = pOutPic->fs_image.PTS;
                pImg->Usertag = pOutPic->fs_image.Usertag;
                pImg->DispTime = pOutPic->fs_image.DispTime;
                pImg->DispEnableFlag = pOutPic->fs_image.DispEnableFlag;
                pImg->DispFrameDistance = pOutPic->fs_image.DispFrameDistance;
                pImg->DistanceBeforeFirstFrame = pOutPic->fs_image.DistanceBeforeFirstFrame;
                pImg->GopNum = pOutPic->fs_image.GopNum;

                pImg->error_level = pOutFrameStore->err_level;
                if (pImg->error_level > 0 && ((pImg->format&3)==0)) /* ���I֡�д����ϱ�һ���¼�֪ͨӦ�ò� */
                {
                    dprint(PRN_ERROR, "line %d, REPORT_IFRAME_ERR\n", __LINE__);
                    REPORT_IFRAME_ERR(pH264Ctx->ChanID);
                }
                if (pImg->error_level == 0 || pImg->error_level <= pH264Ctx->pstExtraData->s32OutErrThr)
                {
                    FSP_SetDisplay(pH264Ctx->ChanID, pOutFrameStore->logic_fs_id, 1);
                    if (VF_OK != InsertImgToVoQueue(pH264Ctx->ChanID, VFMW_H264, pH264Ctx, &pH264Ctx->ImageQue, pImg))
                    {
                        FSP_SetDisplay(pH264Ctx->ChanID, pOutFrameStore->logic_fs_id, 0);
                    }
                   else
                   {
                        pH264Ctx->numOutputFrame++;
                   }
                    SetFrmRepeatCount(pH264Ctx, pImg);
                    pH264Ctx->PicOutCounter += 2;
                }
                else
                {
					REPORT_OVER_REFTHR(pH264Ctx->ChanID, pImg->error_level, pH264Ctx->pstExtraData->s32OutErrThr);
                    FSP_SetDisplay(pH264Ctx->ChanID, pOutFrameStore->logic_fs_id, 0);
                    STORE_PIC_RET(H264_ERR);
                }

                if (FIRST_DISP_OUTPUT != pH264Ctx->FirstFrameDisplayed)
                {
                    /* ����ǿ������֡������֡�䱻��������ھ���������������ܴ�DPB��ɾ�����Ƚ����� */
                    pOutFrameStore->eFoState = FO_STATE_FAST_OUTPUT;
                }
                pH264Ctx->FirstFrameDisplayed = FIRST_DISP_OUTPUT;
            }
			else
			{
			    //��¼��һ�������Ⱥ�������ֱ��ͼ����Ϣ������һ������һ�����
			}
        }
    }

    /******************** ����3: Ϊ���ӶԽ����Ƶļ�DPB����Ŀ������������ʱ����I֡Ҳ�ܽ��� ************************/
    /* ���s32DecOrderOutput == DEC_ORDER_SIMPLE_DPB, ����·ԭDPB������ƣ����û�����ʽ����ο�֡.
       z56361. 20110803 */
    /* z56361. 20120302, ԭ������û�������Ƿ�ο�ȫ��slide DPB, ʵ���Ϸǲο�֡���ܲ��뻬�������򼷵��������Ĳο�֡.
       ����ϷSVC-T������������. ��������Ǽ���(0 != pH264Ctx->CurrPic.nal_ref_idc)���жϡ� */
    if ((pH264Ctx->pstExtraData->s32DecOrderOutput == DEC_ORDER_SIMPLE_DPB) && (0 != pH264Ctx->CurrPic.nal_ref_idc))
    {
        if (NALU_TYPE_IDR == pH264Ctx->CurrPic.nal_unit_type)
        {
            ret = IDRMemMarking(pH264Ctx);
            if (H264_OK != ret)
            {
                return H264_ERR;
            }
        }
        SimpleSlideDPB(pH264Ctx);

        /*�ѵ�ǰͼ�����DPB*/
        ret = InsertFrmInDPB(pH264Ctx,pH264Ctx->DPB.used_size, &pH264Ctx->CurrPic);
        if (H264_OK != ret)
        {
            //return H264_ERR;
            dprint(PRN_FATAL,"line %d, return %d\n",__LINE__, ret);
            STORE_PIC_RET(H264_ERR);
        }
        pH264Ctx->DPB.used_size++;

        UpdateReflist(pH264Ctx);
        UpdateLTReflist(pH264Ctx);

        STORE_PIC_RET(H264_OK);
    }

    /******************** ��ʼ��׼��DPB�������� ************************/
    /* ����ǲο�֡��marking����*/
    if (0 != pH264Ctx->CurrPic.nal_ref_idc)
    {
        //all pic dec down, add ref_err_thr; if this pic will display, the code return IDR_LOST
        if ((pH264Ctx->ref_error_thr < pH264Ctx->CurrPic.err_level) && (pH264Ctx->CurrPic.non_existing == 0))
        {
            dprint(PRN_ERROR, "err(%d) > ref_thr(%d)\n", pH264Ctx->CurrPic.err_level, pH264Ctx->ref_error_thr);
            REPORT_OVER_REFTHR(pH264Ctx->ChanID, pH264Ctx->CurrPic.err_level, pH264Ctx->ref_error_thr);

            H264_ClearCurrPic(pH264Ctx);
            ClearDPB(pH264Ctx);
            //return H264_ERR;  //IDR_LOST
            STORE_PIC_RET(H264_ERR);
        }

        ret = Marking(pH264Ctx);
        if (H264_OK != ret)
        {
            //return H264_ERR;
            dprint(PRN_ERROR,"marking return %d\n", ret);
            STORE_PIC_RET(H264_ERR);
        }

        /* z56361,2011-11-23, marking֮��DPB�еĲο�֡�����ᷢ���ı䣬������Ҫ��ʱ����ref_frames_in_buffer��ltref_frames_in_buffer. ����
           ������������marking֮ǰ��ֵ��������������������(���紥���Ժ��"DPB�������ж�")*/
        UpdateReflist(pH264Ctx);
        UpdateLTReflist(pH264Ctx);

        /* z56361, 20111213, marking֮����ܻ����'un-used'֡�棬��Щ֡��Ҫ�����DPB�������ȥ����Ϊ��ʱ�ܿ���fsp�Ѿ��������Ӧ��¼��
          ������Щ֡�����DPB���Ż����� */
	   if (0 != RemoveUnUsedFrameStore(pH264Ctx))
	   {
	       dprint(PRN_DBG, "return value is unexpeced!\n");
	   }
    }

    /*��ǰͼ������*/
    if (1 == pH264Ctx->CurrPic.field_pair_flag)
    {
        if (pH264Ctx->CurrPic.frame_store == pH264Ctx->pDirectOutBuf && (H264_FRAMESTORE_S *)NULL != pH264Ctx->pDirectOutBuf)
        {
            // first field in directoutput buffer
            ret = DirectOutput(pH264Ctx);
            if (H264_ERR == ret)
            {
                dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
            }
			if (H264_ALREADY_OUT == ret)
			{
				pH264Ctx->CurrPic.frame_store->is_in_dpb = 0;
				pH264Ctx->CurrPic.frame_store->is_used	 = 0;
			}

            //return ret;
            STORE_PIC_RET(ret);
        }

        for (i=0; i<pH264Ctx->DPB.used_size; i++)
        {
            if (pH264Ctx->CurrPic.frame_store == pH264Ctx->DPB.fs[i])
            {
                // first field already in g_DPB
                ret = InsertFrmInDPB(pH264Ctx, i, &(pH264Ctx->CurrPic));
                if (H264_ERR == ret)
                {
                    //return H264_ERR;
                    dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
                    STORE_PIC_RET(H264_ERR);
                }
                UpdateReflist(pH264Ctx);
                UpdateLTReflist(pH264Ctx);
                //return H264_OK;
                STORE_PIC_RET(H264_OK);
            }
        }

        //����ɶԳ������������ܵ����˵��֮ǰ����ʲô���⣬Ӧ�ö�����ǰͼ�񣬺������
    }

    /*DPB�������ж�*/
    if ((0 != pH264Ctx->CurrPic.nal_ref_idc) &&
        (pH264Ctx->DPB.size <= (pH264Ctx->DPB.ref_frames_in_buffer + pH264Ctx->DPB.ltref_frames_in_buffer)))
    {
        if((DEC_ORDER_SKYPLAY == pH264Ctx->pstExtraData->s32DecOrderOutput) ||
		   (1 == g_apple_airplay_mirrorinig_flag))
        {
            GetMinPOC(pH264Ctx, &min_poc, &pos);
			RemoveFrameStoreOutDPB(pH264Ctx, pos);
	        UpdateReflist(pH264Ctx);
	        UpdateLTReflist(pH264Ctx);
        }
		else
		{
	        // ������������ݴ������簴��������markһ��
            dprint(PRN_ERROR, "line %d: pH264Ctx->DPB.size = %d, ref %d, ltref %d\n", __LINE__, pH264Ctx->DPB.size,
                   pH264Ctx->DPB.ref_frames_in_buffer, pH264Ctx->DPB.ltref_frames_in_buffer);
            ClearDPB(pH264Ctx);
            STORE_PIC_RET(H264_ERR);
        }
    }

    /*����ͼ�����*/
    while (pH264Ctx->DPB.used_size >= pH264Ctx->DPB.size)
    {
        GetMinPOC(pH264Ctx, &min_poc, &pos); //������ӶԲ��ҵ���Сpos��λ�õĴ�������ж�

        if ((0==pH264Ctx->CurrPic.nal_ref_idc) && (pH264Ctx->CurrPic.thispoc<min_poc))
        {
            if ((2 == pH264Ctx->OldDecMode) && (0 != pH264Ctx->CurrPic.structure))
            {
                pH264Ctx->pLastFieldBuf = pH264Ctx->CurrPic.frame_store;
            }
            ret = DirectOutput(pH264Ctx);
            //return ret;
            if (H264_ERR == ret)
            {
                dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
            }
            if (H264_ALREADY_OUT == ret)
            {
                pH264Ctx->CurrPic.frame_store->is_in_dpb = 0;
                pH264Ctx->CurrPic.frame_store->is_used	 = 0;
            }

            STORE_PIC_RET(ret);
        }

         //�ݴ���: ��used_size����ǰ���£����ظ���ȡͬһ��pos���ᵼ����ѭ������ǿ���˳��� y00226912 2014.04.19
        if (pH264Ctx->DPB.used_size == pre_used_size && pos == pre_pos)
        {
            dprint(PRN_ERROR,"%s: pos(%d) = pre_pos, force return.\n",__func__, pos);
            STORE_PIC_RET(H264_ERR);
        }
        else
        {
            pre_pos = pos;
            pre_used_size = pH264Ctx->DPB.used_size;
        }

        ret = OutputFrmFromDPB(pH264Ctx, pos);
        if (H264_ERR == ret)
        {
            //return H264_ERR;
            dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
            STORE_PIC_RET(H264_ERR);
        }
    }

    /*����DPBǰ�Ĵ����ж�*/
    if (0 != pH264Ctx->CurrPic.nal_ref_idc && 0 == pH264Ctx->CurrPic.is_long_term)
    {
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if (pH264Ctx->DPB.fs_ref[i]->frame_num == pH264Ctx->CurrPic.frame_num)
            {
                pH264Ctx->DPB.fs_ref[i]->is_reference = 0;
                if (1 == pH264Ctx->DPB.fs_ref[i]->is_in_dpb)
                {
                    pH264Ctx->DPB.fs_ref[i]->is_displayed = 1;
                    pH264Ctx->DPB.fs_ref[i]->is_in_dpb = 0;
                }

                //pmv��λ���ó�ȱʡֵ
                if (pH264Ctx->DPB.fs_ref[i]->pmv_address_idc != pH264Ctx->TotalPmvNum)
                {
                    pH264Ctx->PmvStoreUsedFlag[pH264Ctx->DPB.fs_ref[i]->pmv_address_idc] = 0;          //pmv store
                    pH264Ctx->DPB.fs_ref[i]->pmv_address_idc = pH264Ctx->TotalPmvNum;
                }
            }
        }
    }

    /*�ѵ�ǰͼ�����DPB*/
    ret = InsertFrmInDPB(pH264Ctx,pH264Ctx->DPB.used_size, &pH264Ctx->CurrPic);
    pH264Ctx->PrevPic.Dpb_pos = pH264Ctx->DPB.used_size;
    if (H264_OK != ret)
    {
        //return H264_ERR;
        dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
        STORE_PIC_RET(H264_ERR);
    }
    pH264Ctx->DPB.used_size++;

    UpdateReflist(pH264Ctx);
    UpdateLTReflist(pH264Ctx);

store_pic_exit:

    if (ret == H264_ERR)
    {
		FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
        dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
        /* z56361, 20111223, ��ȻStoreʧ�ܣ���֡��֡��Ӧ���ͷŵ��������������֡��й© */
        pH264Ctx->CurrPic.frame_store->is_in_dpb = 0;
        pH264Ctx->CurrPic.frame_store->is_used   = 0;
    }
    pH264Ctx->CurrPic.state = EMPTY;

    return ret;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID ReleaseNAL(H264DEC_NALU *p_nal)
*   �������� :  �ͷ�һ��nal
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
VOID ReleaseNAL(SINT32 ChanID, H264_NALU_S *pNalu)
{
    SINT32 i;

    CHECK_NULL_PTR_Return(pNalu);

    pNalu->is_valid = 0;
    pNalu->nal_integ = 0;

    for (i=0; i<2; i++)
    {
        // add release buf;
        if (pNalu->stream[i].streamBuffer != NULL)
        {
            SM_ReleaseStreamSeg(ChanID, pNalu->stream[i].StreamID);
            // ReleaseBufCheck(p_nal->stream[i].stream_phy_addr);
            // write log
            dprint(PRN_STREAM, "\n VFMW ** release streambuff=%p   bitstream_length=%d\n", pNalu->stream[i].streamBuffer, pNalu->stream[i].bitstream_length);
            pNalu->stream[i].streamBuffer = NULL;
            pNalu->stream[i].bitsoffset = 0;
            pNalu->stream[i].bitstream_length = 0;
        }
    }

    pNalu->nal_used_segment = 0;
    pNalu->nal_segment = 0;

    return;
}


/*********************************************************************************
*����ԭ��
*SINT32 H264_ArrangeVahbMem(H264_CTX_S *pH264Ctx, UINT32 ImgWidth, UINT32 ImgHeight, UINT32 ImgNum, UINT32 PmvBlkNum)
*��������
*H264Э����VAHB��ַ�滮��ͬʱ����VAHB�洢ռ������
*
*  ����˵��
*  ImgWidth�� ͼ���ȣ�������Ϊ��λ
*  ImgHeight��ͼ��߶ȣ� ������Ϊ��λ
*  ImgNum��   ��Ҫ��VAHB�ռ��п��ٵ�ͼ��洢�������
*  PmvBlkNum����Ҫ��VAHB�ռ��п��ٵ�MV��Ϣ�洢�����
*  ����ֵ
*  ����滮�ɹ�����1�����򷵻ش�����
*  ��������
*  g_VfmwCfgInfo��VFMW��������Ϣ��������public.c
*  g_Memory��     VFMW�Ĵ洢������Ϣ��������public.c
*  g_VfmwState��  VFMW��״̬��Ϣ��������public.c
*  g_H264MemInfo��H.264ģ���ַ�滮��Ϣ��������h264.c
*  �㷨����
*  ��g_VfmwCfgInfo�л�ȡÿ��ͼ����Ҫ֧�ֶ��ٸ�slice����Ϣ��
*  ��g_Memory�л�ȡVAHB��ʼ��ַ�����ҽ�H264���ĵ�VAHB�ռ�����g_Memory.VAHB_access_size
*  ˢ��g_VfmwState����ص�״̬��Ϣ��
*  H.264ģ��ĵ�ַ��ϸ����g_H264MemInfo����H.264��������ʹ��
*********************************************************************************/
SINT32 H264_ArrangeVahbMem(H264_CTX_S *pH264Ctx, UINT32 ImgWidth, UINT32 ImgHeight, UINT32 *pImgNum, UINT32 PmvBlkNum)
{
	SINT32 Ret, UsedMemSize;
    FSP_INST_CFG_S FspInstCfg;

    memset(&FspInstCfg, 0, sizeof(FSP_INST_CFG_S));
    FspInstCfg.s32DecFsWidth        = ImgWidth;
    FspInstCfg.s32DecFsHeight       = ImgHeight;
    FspInstCfg.s32DispFsWidth       = ImgWidth;
    FspInstCfg.s32DispFsHeight      = ImgHeight;
    FspInstCfg.s32ExpectedDecFsNum  = (pH264Ctx->pstExtraData->eCapLevel == CAP_LEVEL_SINGLE_IFRAME_FHD)? 0: (pH264Ctx->DPB.size+1);
    FspInstCfg.s32ExpectedDispFsNum = 5;
    FspInstCfg.s32ExpectedPmvNum    = (pH264Ctx->pstExtraData->eCapLevel == CAP_LEVEL_SINGLE_IFRAME_FHD)? 1: PmvBlkNum;

    /* ����FSPʵ�������ָ�֡�� */
    if (FSP_OK == FSP_ConfigInstance(pH264Ctx->ChanID, &FspInstCfg))
    {
        Ret = FSP_PartitionFsMemory(pH264Ctx->ChanID, pH264Ctx->pstExtraData->s32SyntaxMemAddr,
                                    pH264Ctx->pstExtraData->s32SyntaxMemSize, &UsedMemSize);
        if (Ret != FSP_OK)
        {
            // �ָ�֡��ʧ�ܣ�����
            return VF_ERR_SYS;
        }
    }
	else
	{
	    dprint(PRN_FATAL, "ERROR: FSP_ConfigInstance fail!\n");
	    return VF_ERR_SYS;
	}

    return VF_OK;
}


/*!
************************************************************************
* ����ԭ�� : SINT32 RepairList(H264_CTX_S *pH264Ctx,)
* �������� :  �޲�list,ֻ����ȥ��list�е�NULL
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 RepairList(H264_CTX_S *pH264Ctx)
{
    UINT8  found;
    UINT32 i, i_list, i_list_cnt;
    H264_STORABLEPIC_S *pRepairPic = (H264_STORABLEPIC_S *)NULL;

    switch (pH264Ctx->CurrSlice.slice_type)
    {
    case P_SLICE:
        i_list_cnt = 1;
        break;
    case B_SLICE:
        i_list_cnt = 2;
        break;
    case I_SLICE:
        i_list_cnt = 0;
        break;
    default:
        dprint(PRN_ERROR, "slice_type %d invalid, clear current slice.\n", pH264Ctx->CurrSlice.slice_type);
        ClearCurrSlice(pH264Ctx);
        return H264_ERR;  //NULL_LIST
    }

    for (i_list=0; i_list<i_list_cnt; i_list++)
    {
        if (0 < pH264Ctx->CurrSlice.listXsize[i_list])
        {
            found = 0;
            for (i = 0; i < pH264Ctx->CurrSlice.listXsize[i_list]; i++)
            {
                //!! the condition must add
                if ((H264_STORABLEPIC_S *)NULL != pH264Ctx->pListX[i_list][i])
                {
                    /*
                    if (g_VfmwCfg.ref_error_thr < g_pListX[i_list][i]->err_level)
                    {
                        //Ϊ�˺õ�Ч�����裬���������֮������idr�޹�
                        continue;
                    }
                    */
                    found = 1;
                    pRepairPic = pH264Ctx->pListX[i_list][i];
                    break;
                }
            }

            if ((0==found) || (NULL==pRepairPic))
            {
                if (pH264Ctx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB)
                {
                    //!!! ����listԪ��Ϊ�գ����԰�list��Ϊ����Ӳ��ȥ�ݴ�
                    //!!! Ҳ���԰�slice������
                    //!!! ǰ��Ϊʹ��p bslice��ȫi���ʱ�ɽ⣬����Ӳ���ݴ�
                    //!!! ���߶�Ӳ����Ϊ��ȫ������Ҫ�޲��ܶ� slice
                    dprint(PRN_ERROR, "pRepairPic(%p)/found(%d) = 0, clear current slice.\n", pRepairPic, found);
                    ClearCurrSlice(pH264Ctx);
                    return H264_ERR;  //NULL_LIST
                }
                else
                {
                    /* ���������DEC_ORDER_SIMPLE_DPB, ���õ�ǰpic�������list�ڵ� */
                    pRepairPic = &pH264Ctx->CurrPic.frame_store->frame;
                }
            }

            for (i = 0; i < pH264Ctx->CurrSlice.listXsize[i_list]; i++)
            {
                //!! the condition must add
                if ((H264_STORABLEPIC_S *)NULL == pH264Ctx->pListX[i_list][i])
                {
                    pH264Ctx->pListX[i_list][i] = pRepairPic;
                }
            }
        }
        else
        {
            if (pH264Ctx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB)
            {
                dprint(PRN_ERROR, "listXsize[%d] = %d, clear current slice.\n", i_list, pH264Ctx->CurrSlice.listXsize[i_list]);
                ClearCurrSlice(pH264Ctx);
                return H264_ERR;  //NULL_LIST
            }
            else
            {
                /* ���������DEC_ORDER_SIMPLE_DPB, ���õ�ǰpic�������list�ڵ� */
                pH264Ctx->CurrSlice.listXsize[i_list] = 1;
                pH264Ctx->pListX[i_list][0] = &pH264Ctx->CurrPic.frame_store->frame;
            }

        }
    }

    return H264_OK;
}


/*!
************************************************************************
* ����ԭ�� :  H264DEC_StorablePicture*  GetShortTermPicPoint(H264_CTX_S *pH264Ctx,,SINT32 picNum)
* �������� :  ����picnum�ҵ���Ӧ���ڲο�pic��ָ��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
H264_STORABLEPIC_S *GetShortTermPicPoint(H264_CTX_S *pH264Ctx,SINT32 picNum)
{
    UINT32 i;
    SINT32 structure = pH264Ctx->CurrSlice.field_pic_flag ? pH264Ctx->CurrSlice.bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD : FRAME;

    for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
    {
        if (FRAME == structure)
        {
            if (3 == pH264Ctx->DPB.fs_ref[i]->is_reference) // frame, field pair
                if ((1 == pH264Ctx->DPB.fs_ref[i]->frame.is_short_term) && (0 == pH264Ctx->DPB.fs_ref[i]->frame.is_long_term) && (pH264Ctx->DPB.fs_ref[i]->frame.pic_num == picNum))
                {
                    return &pH264Ctx->DPB.fs_ref[i]->frame;
                }
        }
        else
        {
            if (pH264Ctx->DPB.fs_ref[i]->is_reference & 1) // top field
                if ((1==pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term) && (0==pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term) && (pH264Ctx->DPB.fs_ref[i]->top_field.pic_num==picNum))
                {
                    return &pH264Ctx->DPB.fs_ref[i]->top_field;
                }

                if (pH264Ctx->DPB.fs_ref[i]->is_reference & 2) // bottom field
                    if ((1==pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term) && (0==pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term) && (pH264Ctx->DPB.fs_ref[i]->bottom_field.pic_num==picNum))
                {
                        return &pH264Ctx->DPB.fs_ref[i]->bottom_field;
                }
        }
    }

    return NULL;
}


/*!
************************************************************************
* ����ԭ�� :  H264DEC_StorablePicture*  GetLongTermPicPoint(H264_CTX_S *pH264Ctx,SINT32 LongtermPicNum)
* �������� :  ����LongtermPicNum�ҵ���Ӧ���ڲο�pic��ָ��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
H264_STORABLEPIC_S *GetLongTermPicPoint(H264_CTX_S *pH264Ctx,SINT32 LongtermPicNum)
{
    UINT32 i;
    SINT32 structure = pH264Ctx->CurrSlice.field_pic_flag ? pH264Ctx->CurrSlice.bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD : FRAME;

    for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
    {
        if (FRAME==structure)
        {
            if (3==pH264Ctx->DPB.fs_ltref[i]->is_reference)//frame, field pair
                if ((0==pH264Ctx->DPB.fs_ltref[i]->frame.is_short_term)&& (1==pH264Ctx->DPB.fs_ltref[i]->frame.is_long_term) && (pH264Ctx->DPB.fs_ltref[i]->frame.long_term_pic_num==LongtermPicNum))
                { return &pH264Ctx->DPB.fs_ltref[i]->frame; }
        }
        else
        {
            if (pH264Ctx->DPB.fs_ltref[i]->is_reference & 1)//top field
                if ((0==pH264Ctx->DPB.fs_ltref[i]->top_field.is_short_term) && (1==pH264Ctx->DPB.fs_ltref[i]->top_field.is_long_term) && (pH264Ctx->DPB.fs_ltref[i]->top_field.long_term_pic_num==LongtermPicNum))
                { return &pH264Ctx->DPB.fs_ltref[i]->top_field; }

                if (pH264Ctx->DPB.fs_ltref[i]->is_reference & 2)//bottom field
                    if ((0==pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_short_term) && (1==pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_long_term) && (pH264Ctx->DPB.fs_ltref[i]->bottom_field.long_term_pic_num==LongtermPicNum))
                { return &pH264Ctx->DPB.fs_ltref[i]->bottom_field; }
        }
    }

    return NULL;
}


/*!
************************************************************************
* ����ԭ�� : VOID H264DEC_ReorderShortTerm(H264_STORABLEPIC_S **RefPicListX,
SINT32 num_ref_idx_lX_active_minus1, SINT32 picNumLX, SINT32 *refIdxLX)
* �������� :  ���Ŷ��ڲο��б�
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
VOID ReorderSTList(H264_CTX_S *pH264Ctx, H264_STORABLEPIC_S **RefPicListX,
                   SINT32 num_ref_idx_lX_active_minus1, SINT32 picNumLX, SINT32 *refIdxLX)
{
    SINT32 cIdx, nIdx;
    H264_STORABLEPIC_S *picLX;

    picLX = GetShortTermPicPoint(pH264Ctx, picNumLX);

    for (cIdx=num_ref_idx_lX_active_minus1+1; cIdx>*refIdxLX; cIdx--)
    {
        RefPicListX[cIdx] = RefPicListX[cIdx-1];
    }
    RefPicListX[(*refIdxLX)++] = picLX;

    nIdx = *refIdxLX;

    for (cIdx=*refIdxLX; cIdx<=num_ref_idx_lX_active_minus1+1; cIdx++)
    {
        if (RefPicListX[cIdx])
        {
            if ((0!=RefPicListX[cIdx]->is_long_term) || (RefPicListX[cIdx]->pic_num!=picNumLX))
            {
                RefPicListX[nIdx++] = RefPicListX[cIdx];
            }
        }
    }

    return;
}


/*!
************************************************************************
* ����ԭ�� : VOID H264DEC_ReorderLongTerm(H264_STORABLEPIC_S **RefPicListX,
SINT32 num_ref_idx_lX_active_minus1, SINT32 LongTermPicNum, SINT32 *refIdxLX)
* �������� :  ���ų����ڲο��б�
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
VOID ReorderLTList(H264_CTX_S *pH264Ctx, H264_STORABLEPIC_S **RefPicListX,
                   SINT32 num_ref_idx_lX_active_minus1, SINT32 LongTermPicNum, SINT32 *refIdxLX)
{
    SINT32 cIdx, nIdx;
    H264_STORABLEPIC_S *picLX;

    picLX = GetLongTermPicPoint(pH264Ctx, LongTermPicNum);

    for (cIdx=num_ref_idx_lX_active_minus1+1; cIdx>*refIdxLX; cIdx--)
    {
        RefPicListX[cIdx] = RefPicListX[cIdx-1];
    }
    RefPicListX[(*refIdxLX)++] = picLX;

    nIdx = *refIdxLX;

    for (cIdx=*refIdxLX; cIdx<=num_ref_idx_lX_active_minus1+1; cIdx++)
    {
        if (RefPicListX[cIdx])
        {
            if ((0==RefPicListX[cIdx]->is_long_term) || (RefPicListX[cIdx]->long_term_pic_num!=LongTermPicNum))
            {
                RefPicListX[nIdx++] = RefPicListX[cIdx];
            }
        }
    }

    return;
}


/*!
************************************************************************
* ����ԭ�� : SINT32 ReorderRefPiclist(H264_CTX_S *pH264Ctx,H264_STORABLEPIC_S **list, SINT32 *list_size, SINT32 num_ref_idx_lX_active_minus1,
SINT32 *reordering_of_pic_nums_idc, SINT32 *abs_diff_pic_num_minus1, SINT32 *long_term_pic_idx)
* �������� :  �ο��б����ź���
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
VOID ReorderRefPiclist(H264_CTX_S *pH264Ctx,H264_STORABLEPIC_S **list, UINT32 *list_size, UINT32 num_ref_idx_lX_active_minus1,
                       UINT32 *reordering_of_pic_nums_idc, UINT32 *abs_diff_pic_num_minus1, UINT32 *long_term_pic_idx)
{
    UINT32 i;
    SINT32 maxPicNum, currPicNum, picNumLXNoWrap, picNumLXPred, picNumLX;
    SINT32 refIdxLX = 0;
    SINT32 structure = pH264Ctx->CurrSlice.field_pic_flag ? pH264Ctx->CurrSlice.bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD : FRAME;
    SINT32 MaxFrameNum;
    H264_SPS_S *sps;
    H264_PPS_S *pps;

    pps = &pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id];
    sps = &pH264Ctx->SPS[pps->seq_parameter_set_id];

    MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4);

    if (FRAME==structure)
    {
        maxPicNum  = MaxFrameNum;
        currPicNum = pH264Ctx->CurrSlice.frame_num;
    }
    else
    {
        maxPicNum  = 2 * MaxFrameNum;
        currPicNum = 2 * pH264Ctx->CurrSlice.frame_num + 1;
    }
    picNumLXPred = currPicNum;

    for (i=0; i<(num_ref_idx_lX_active_minus1+2); i++)
    {
        if (3==reordering_of_pic_nums_idc[i])
        {
            break;
        }

        if (reordering_of_pic_nums_idc[i] < 2)
        {
            if (0==reordering_of_pic_nums_idc[i])
            {
                if (picNumLXPred - ((SINT32)abs_diff_pic_num_minus1[i] + 1) < 0)
                {
                    picNumLXNoWrap = (picNumLXPred - ((SINT32)abs_diff_pic_num_minus1[i] + 1)) + maxPicNum;
                }
                else
                {
                    picNumLXNoWrap = picNumLXPred - ((SINT32)abs_diff_pic_num_minus1[i] + 1);
                }
            }
            else // (1==reordering_of_pic_nums_idc[i])
            {
                if (picNumLXPred + (((SINT32)abs_diff_pic_num_minus1[i] + 1)) >= maxPicNum)
                {
                    picNumLXNoWrap = picNumLXPred + ((SINT32)abs_diff_pic_num_minus1[i] + 1) - maxPicNum;
                }
                else
                {
                    picNumLXNoWrap = picNumLXPred + ((SINT32)abs_diff_pic_num_minus1[i] + 1);
                }
            }
            picNumLXPred = picNumLXNoWrap;

            if (picNumLXNoWrap > currPicNum)
            {
                picNumLX = picNumLXNoWrap - maxPicNum;
            }
            else
            {
                picNumLX = picNumLXNoWrap;
            }

            ReorderSTList(pH264Ctx, list, num_ref_idx_lX_active_minus1, picNumLX, &refIdxLX);
        }
        else // (2==reordering_of_pic_nums_idc[i])
        {
            ReorderLTList(pH264Ctx, list, num_ref_idx_lX_active_minus1, long_term_pic_idx[i], &refIdxLX);
        }
    }

    return;
}


/*!
************************************************************************
* ����ԭ�� :  SINT32 ReorderlistX(H264_CTX_S *pH264Ctx)
* �������� :  �ο��б�����
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
VOID ReorderListX(H264_CTX_S *pH264Ctx)
{
    if (I_SLICE != pH264Ctx->CurrSlice.slice_type)
    {
        if (pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l0)
        {
            ReorderRefPiclist(pH264Ctx, pH264Ctx->pListX[0], &(pH264Ctx->CurrSlice.listXsize[0]),
                pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1,
                pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0,
                pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l0,
                pH264Ctx->CurrReorder.long_term_pic_idx_l0);
        }

        // that's a definition
        pH264Ctx->CurrSlice.listXsize[0] = pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1+1;
    }
    if (B_SLICE == pH264Ctx->CurrSlice.slice_type)
    {
        if (pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l1)
        {
            ReorderRefPiclist(pH264Ctx, pH264Ctx->pListX[1], &(pH264Ctx->CurrSlice.listXsize[1]),
                pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1,
                pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1,
                pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l1,
                pH264Ctx->CurrReorder.long_term_pic_idx_l1);
        }

        // that's a definition
        pH264Ctx->CurrSlice.listXsize[1] = pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1+1;
    }

    return;
}


/*!
************************************************************************
* ����ԭ�� : SINT32 IsSTRefFlg(H264_STORABLEPIC_S *s)
* �������� :  �ж�pic�Ƿ��Ƕ��ڲο�
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
UINT32 IsSTRefFlg(H264_STORABLEPIC_S *s)
{
    return ((1==s->is_short_term) && (0==s->is_long_term));
}


/*!
************************************************************************
* ����ԭ�� : SINT32 IsLTRefFlg(H264_STORABLEPIC_S *s)
* �������� :  �ж�pic�Ƿ��ǳ��ڲο�
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
UINT32 IsLTRefFlg(H264_STORABLEPIC_S *s)
{
    return ((0==s->is_short_term) && (1==s->is_long_term));
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_pic_by_pic_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�storablepicture�ṹ���е�Ԫ��pic_num�Ĵ�С��
*              ps:��һ��ȽϺ����Ƚϵ�Ԫ�ض�ӦЭ���й涨��ֵ����ЩֵҲ����JM�д�ŵĽṹ��
*              ��һ��.Ϊ�˱�֤��ȷ�ԣ����Լ������д��λ���ϵ�Ԫ���Ƿ���ȷ
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_pic_by_pic_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_STORABLEPIC_S**)arg1)->pic_num < (*(H264_STORABLEPIC_S**)arg2)->pic_num)
    {
        return 1;
    }

    if ((*(H264_STORABLEPIC_S**)arg1)->pic_num > (*(H264_STORABLEPIC_S**)arg2)->pic_num)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_pic_by_lt_pic_num_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�storablepicture�ṹ���е�Ԫ��pic_num�Ĵ�С��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_pic_by_lt_pic_num_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_STORABLEPIC_S**)arg1)->long_term_pic_num < (*(H264_STORABLEPIC_S**)arg2)->long_term_pic_num)
    {
        return -1;
    }

    if ((*(H264_STORABLEPIC_S**)arg1)->long_term_pic_num > (*(H264_STORABLEPIC_S**)arg2)->long_term_pic_num)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_fs_by_frame_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�framestore�ṹ���е�Ԫ��frame_num_wrap�Ĵ�С��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_fs_by_frame_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_FRAMESTORE_S**)arg1)->frame_num_wrap < (*(H264_FRAMESTORE_S**)arg2)->frame_num_wrap)
    {
        return 1;
    }

    if ((*(H264_FRAMESTORE_S**)arg1)->frame_num_wrap > (*(H264_FRAMESTORE_S**)arg2)->frame_num_wrap)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_fs_by_lt_pic_idx_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�framestore�ṹ���е�Ԫ��long_term_frame_idx�Ĵ�С��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_fs_by_lt_pic_idx_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_FRAMESTORE_S**)arg1)->long_term_frame_idx < (*(H264_FRAMESTORE_S**)arg2)->long_term_frame_idx)
    {
        return -1;
    }

    if ((*(H264_FRAMESTORE_S**)arg1)->long_term_frame_idx > (*(H264_FRAMESTORE_S**)arg2)->long_term_frame_idx)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_pic_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�H264_STORABLEPIC_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_pic_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_STORABLEPIC_S**)arg1)->poc < (*(H264_STORABLEPIC_S**)arg2)->poc)
    {
        return -1;
    }

    if ((*(H264_STORABLEPIC_S**)arg1)->poc > (*(H264_STORABLEPIC_S**)arg2)->poc)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_pic_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�H264_STORABLEPIC_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_pic_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_STORABLEPIC_S**)arg1)->poc < (*(H264_STORABLEPIC_S**)arg2)->poc)
    {
        return 1;
    }

    if ((*(H264_STORABLEPIC_S**)arg1)->poc > (*(H264_STORABLEPIC_S**)arg2)->poc)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_fs_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� : ������Ƚ�H264_FRAMESTORE_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_fs_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_FRAMESTORE_S**)arg1)->poc < (*(H264_FRAMESTORE_S**)arg2)->poc)
    {
        return -1;
    }

    if ((*(H264_FRAMESTORE_S**)arg1)->poc > (*(H264_FRAMESTORE_S**)arg2)->poc)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 compare_fs_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�H264_FRAMESTORE_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 compare_fs_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(H264_FRAMESTORE_S**)arg1)->poc < (*(H264_FRAMESTORE_S**)arg2)->poc)
    {
        return 1;
    }

    if ((*(H264_FRAMESTORE_S**)arg1)->poc > (*(H264_FRAMESTORE_S**)arg2)->poc)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


/*!
************************************************************************
* ����ԭ�� : SINT32 GenPiclistfromFrmlist(SINT32 currStrcture, H264_FRAMESTORE_S **fs_list,
*            SINT32 list_idx, H264_STORABLEPIC_S **list, SINT32 *list_size, SINT32 long_term)
* �������� :  ����frame�Ĳο�list���ɳ��Ĳο�list
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 GenPiclistfromFrmlist(UINT32 currStrcture, H264_FRAMESTORE_S **fs_list,
                             UINT32 list_idx, H264_STORABLEPIC_S **list, UINT32 *list_size, SINT32 long_term)
{
    UINT32 top_idx = 0;
    UINT32 bot_idx = 0;
    UINT32 (*is_ref)(H264_STORABLEPIC_S *s);

    if (list_idx>=32)
    {
        list_idx = 31; // for error resilence
    }

    /*top*/
    if (long_term)
    {
        is_ref = IsLTRefFlg;
    }
    else
    {
        is_ref = IsSTRefFlg;
    }

    if (TOP_FIELD==currStrcture)
    {
        while ((top_idx<list_idx) || (bot_idx<list_idx))
        {
            for ( ; top_idx<list_idx; top_idx++)
            {
                if (fs_list[top_idx]->is_used & 1)
                {
                    if (is_ref(&fs_list[top_idx]->top_field))
                    {
                        // short term ref pic
                        list[*list_size] = &fs_list[top_idx]->top_field;
                        (*list_size)++;
                        top_idx++;
                        break;
                    }
                }
            }
            for ( ; bot_idx<list_idx; bot_idx++)
            {
                if (fs_list[bot_idx]->is_used & 2)
                {
                    if (is_ref(&fs_list[bot_idx]->bottom_field))
                    {
                        // short term ref pic
                        list[*list_size] = &fs_list[bot_idx]->bottom_field;
                        (*list_size)++;
                        bot_idx++;
                        break;
                    }
                }
            }
        }
    }
    if (BOTTOM_FIELD==currStrcture)
    {
        while ((top_idx<list_idx) || (bot_idx<list_idx))
        {
            for ( ; bot_idx<list_idx; bot_idx++)
            {
                if (fs_list[bot_idx]->is_used & 2)
                {
                    if (is_ref(&fs_list[bot_idx]->bottom_field))
                    {
                        // short term ref pic
                        list[*list_size] = &fs_list[bot_idx]->bottom_field;
                        (*list_size)++;
                        bot_idx++;
                        break;
                    }
                }
            }
            for ( ; top_idx<list_idx; top_idx++)
            {
                if (fs_list[top_idx]->is_used & 1)
                {
                    if (is_ref(&fs_list[top_idx]->top_field))
                    {
                        // short term ref pic
                        list[*list_size] = &fs_list[top_idx]->top_field;
                        (*list_size)++;
                        top_idx++;
                        break;
                    }
                }
            }
        }
    }

    return 0;
}


/*!
************************************************************************
* ����ԭ�� : SINT32  InitListX(H264_CTX_S *pH264Ctx)
* �������� : �ο��б��ʼ��
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 InitListX(H264_CTX_S *pH264Ctx)
{
    H264_STORABLEPIC_S *tmp_s;
    UINT32 i, j;
    UINT32 list0idx   = 0;
    UINT32 list0idx_1 = 0;
    UINT32 listltidx  = 0;
    H264_FRAMESTORE_S *fs_list0[16];
    H264_FRAMESTORE_S *fs_list1[16];
    H264_FRAMESTORE_S *fs_listlt[16];

    if (P_SLICE == pH264Ctx->CurrSlice.slice_type)
    {
        if (FRAME == pH264Ctx->CurrPic.structure)
        {
            for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
            {
                if ((3==pH264Ctx->DPB.fs_ref[i]->is_reference) && (0==pH264Ctx->DPB.fs_ref[i]->frame.is_long_term)
                    && (1==pH264Ctx->DPB.fs_ref[i]->frame.is_short_term)) // frame, field pair
                {
                    pH264Ctx->pListX[0][list0idx++] = &pH264Ctx->DPB.fs_ref[i]->frame;
                }
            }
            // order list 0 by PicNum : short term
            qsort((VOID *)pH264Ctx->pListX[0], list0idx, sizeof(H264_STORABLEPIC_S*), compare_pic_by_pic_num_desc);
            pH264Ctx->CurrSlice.listXsize[0] = list0idx;

            // long term handling
            for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
            {
                if (3==pH264Ctx->DPB.fs_ltref[i]->is_reference && 1==pH264Ctx->DPB.fs_ltref[i]->frame.is_long_term && 0==pH264Ctx->DPB.fs_ltref[i]->frame.is_short_term) // frame, field pair
                {
                    pH264Ctx->pListX[0][list0idx++] = &pH264Ctx->DPB.fs_ltref[i]->frame;
                }
            }
            qsort((VOID *)&pH264Ctx->pListX[0][pH264Ctx->CurrSlice.listXsize[0]], list0idx - pH264Ctx->CurrSlice.listXsize[0], sizeof(H264_STORABLEPIC_S*), compare_pic_by_lt_pic_num_asc);
            pH264Ctx->CurrSlice.listXsize[0] = list0idx;
        }
        else
        {
            for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
            {
                if (0 != pH264Ctx->DPB.fs_ref[i]->is_reference)
                {
                    fs_list0[list0idx++] = pH264Ctx->DPB.fs_ref[i];
                }
            }
            qsort((VOID *)fs_list0, list0idx, sizeof(H264_FRAMESTORE_S*), compare_fs_by_frame_num_desc);

            pH264Ctx->CurrSlice.listXsize[0] = 0;
            GenPiclistfromFrmlist(pH264Ctx->CurrPic.structure, fs_list0, list0idx, pH264Ctx->pListX[0], &(pH264Ctx->CurrSlice.listXsize[0]), 0);

            // long term handling
            for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
            {
                fs_listlt[listltidx++]=pH264Ctx->DPB.fs_ltref[i];
            }

            qsort((VOID *)fs_listlt, listltidx, sizeof(H264_FRAMESTORE_S*), compare_fs_by_lt_pic_idx_asc);
            GenPiclistfromFrmlist(pH264Ctx->CurrPic.structure, fs_listlt, listltidx, pH264Ctx->pListX[0], &(pH264Ctx->CurrSlice.listXsize[0]), 1);
        }
        pH264Ctx->CurrSlice.listXsize[1] = 0;

        if (0 == pH264Ctx->CurrSlice.listXsize[0])
        {
            /* list�ǿյ�: ���������DEC_ORDER_SIMPLE_DPBģʽ�����õ�ǰ֡��Ϊ�ο�ͼǿ�ƽ���;
                           ���򷵻�list����. z56361. 20110803 */
            if (pH264Ctx->pstExtraData->s32DecOrderOutput == DEC_ORDER_SIMPLE_DPB)
            {
                if (pH264Ctx->CurrPic.structure == FRAME)
                {
                    pH264Ctx->pListX[0][0] = &pH264Ctx->CurrPic.frame_store->frame;
                }
                else if (pH264Ctx->CurrPic.structure == TOP_FIELD)
                {
                    pH264Ctx->pListX[0][0] = &pH264Ctx->CurrPic.frame_store->top_field;
                }
                else
                {
                    pH264Ctx->pListX[0][0] = &pH264Ctx->CurrPic.frame_store->bottom_field;
                }
                pH264Ctx->CurrSlice.listXsize[0] = 1;
            }
            else
            {
                dprint(PRN_ERROR, "for P slice size of list equal 0.\n");
                ClearCurrSlice(pH264Ctx);
                return H264_ERR;  //INIT_LIST_ERR
            }
        }
    }
    else  // B-Slice
    {
        if (FRAME == pH264Ctx->CurrPic.structure)
        {
            for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
            {
                if (3 == pH264Ctx->DPB.fs_ref[i]->is_reference)//frame, field pair
                {
                    if (pH264Ctx->CurrPic.framepoc >= pH264Ctx->DPB.fs_ref[i]->frame.poc)
                    {
                        pH264Ctx->pListX[0][list0idx++] = &pH264Ctx->DPB.fs_ref[i]->frame;
                    }
                }
            }
            qsort((VOID *)pH264Ctx->pListX[0], list0idx, sizeof(H264_STORABLEPIC_S*), compare_pic_by_poc_desc);
            list0idx_1 = list0idx;
            for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
            {
                if (3==pH264Ctx->DPB.fs_ref[i]->is_reference)  //frame, field pair
                {
                    if (pH264Ctx->CurrPic.framepoc < pH264Ctx->DPB.fs_ref[i]->frame.poc)  //����ж���ʵ���Բ�Ҫ��
                    {
                        pH264Ctx->pListX[0][list0idx++] = &pH264Ctx->DPB.fs_ref[i]->frame;
                    }
                }
            }
            qsort((VOID *)&pH264Ctx->pListX[0][list0idx_1], list0idx-list0idx_1, sizeof(H264_STORABLEPIC_S*), compare_pic_by_poc_asc);
            // ��Ϊ�����Ѿ��Ź����ˣ��������ﲻ��������ֻ�õ�һ����Ϳ�����
            for (j=0; j<list0idx_1; j++)
            {
                pH264Ctx->pListX[1][(list0idx -list0idx_1)+j]=pH264Ctx->pListX[0][j];
            }
            for (j=list0idx_1; j<list0idx; j++)
            {
                pH264Ctx->pListX[1][j-list0idx_1]=pH264Ctx->pListX[0][j];
            }

            pH264Ctx->CurrSlice.listXsize[0] = pH264Ctx->CurrSlice.listXsize[1] = list0idx;

            // long term handling
            for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
            {
                if (3 == pH264Ctx->DPB.fs_ltref[i]->is_used)//frame, field pair
                {
                    if (1 == pH264Ctx->DPB.fs_ltref[i]->frame.is_long_term)
                    {
                        pH264Ctx->pListX[0][list0idx]   = &pH264Ctx->DPB.fs_ltref[i]->frame;
                        pH264Ctx->pListX[1][list0idx++] = &pH264Ctx->DPB.fs_ltref[i]->frame;
                    }
                }
            }
            qsort((VOID *)&pH264Ctx->pListX[0][pH264Ctx->CurrSlice.listXsize[0]], list0idx - pH264Ctx->CurrSlice.listXsize[0], sizeof(H264_STORABLEPIC_S*), compare_pic_by_lt_pic_num_asc);
            qsort((VOID *)&pH264Ctx->pListX[1][pH264Ctx->CurrSlice.listXsize[0]], list0idx - pH264Ctx->CurrSlice.listXsize[0], sizeof(H264_STORABLEPIC_S*), compare_pic_by_lt_pic_num_asc);
            pH264Ctx->CurrSlice.listXsize[0] = pH264Ctx->CurrSlice.listXsize[1] = list0idx;
        }
        else
        {
            for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
            {
                if (0 != pH264Ctx->DPB.fs_ref[i]->is_used)
                {
                    if (pH264Ctx->CurrPic.thispoc >= pH264Ctx->DPB.fs_ref[i]->poc)
                    {
                        fs_list0[list0idx++] = pH264Ctx->DPB.fs_ref[i];
                    }
                }
            }
            qsort((VOID *)fs_list0, list0idx, sizeof(H264_FRAMESTORE_S*), compare_fs_by_poc_desc);
            list0idx_1 = list0idx;
            for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
            {
                if (0 != pH264Ctx->DPB.fs_ref[i]->is_used)
                {
                    if (pH264Ctx->CurrPic.thispoc < pH264Ctx->DPB.fs_ref[i]->poc)
                    {
                        fs_list0[list0idx++] = pH264Ctx->DPB.fs_ref[i];
                    }
                }
            }
            qsort((VOID *)&fs_list0[list0idx_1], list0idx-list0idx_1, sizeof(H264_FRAMESTORE_S*), compare_fs_by_poc_asc);

            for (j=0; j<list0idx_1; j++)
            {
                fs_list1[(list0idx -list0idx_1)+j] = fs_list0[j];
            }
            for (j=list0idx_1; j<list0idx; j++)
            {
                fs_list1[j-list0idx_1] = fs_list0[j];
            }

            pH264Ctx->CurrSlice.listXsize[0] = 0;
            pH264Ctx->CurrSlice.listXsize[1] = 0;
            GenPiclistfromFrmlist(pH264Ctx->CurrPic.structure, fs_list0, list0idx, pH264Ctx->pListX[0], &(pH264Ctx->CurrSlice.listXsize[0]), 0);
            GenPiclistfromFrmlist(pH264Ctx->CurrPic.structure, fs_list1, list0idx, pH264Ctx->pListX[1], &(pH264Ctx->CurrSlice.listXsize[1]), 0);

            // long term handling
            for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
            {
                fs_listlt[listltidx++] = pH264Ctx->DPB.fs_ltref[i];
            }

            qsort((VOID *)fs_listlt, listltidx, sizeof(H264_FRAMESTORE_S*), compare_fs_by_lt_pic_idx_asc);

            GenPiclistfromFrmlist(pH264Ctx->CurrPic.structure, fs_listlt, listltidx, pH264Ctx->pListX[0], &(pH264Ctx->CurrSlice.listXsize[0]), 1);
            GenPiclistfromFrmlist(pH264Ctx->CurrPic.structure, fs_listlt, listltidx, pH264Ctx->pListX[1], &(pH264Ctx->CurrSlice.listXsize[1]), 1);
        }

        if ((pH264Ctx->CurrSlice.listXsize[0] == pH264Ctx->CurrSlice.listXsize[1]) && (pH264Ctx->CurrSlice.listXsize[0]>1))
        {
            // check if lists are identical, if yes swap first two elements of g_pListX[1]
            SINT32 diff=0;
            for (j=0; j<(UINT32)(pH264Ctx->CurrSlice.listXsize[0]); j++)
            {
                if (pH264Ctx->pListX[0][j] != pH264Ctx->pListX[1][j])
                {
                    diff = 1;
                    break;
                }
            }
            if (!diff)
            {
                tmp_s = pH264Ctx->pListX[1][0];
                pH264Ctx->pListX[1][0] = pH264Ctx->pListX[1][1];
                pH264Ctx->pListX[1][1] = tmp_s;
            }
        }

        if ((0 == pH264Ctx->CurrSlice.listXsize[0]) && (0 == pH264Ctx->CurrSlice.listXsize[1]))
        {
            dprint(PRN_ERROR, "for B slice size of two list all equal 0.\n");
            ClearCurrSlice(pH264Ctx);
            return H264_ERR;  //INIT_LIST_ERR
        }
    }

    // set max size
    pH264Ctx->CurrSlice.listXsize[0] = MIN (pH264Ctx->CurrSlice.listXsize[0], pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1+1);
    pH264Ctx->CurrSlice.listXsize[1] = MIN (pH264Ctx->CurrSlice.listXsize[1], pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1+1);

    // set the unused list entries to NULL
    for (i = pH264Ctx->CurrSlice.listXsize[0]; i < (MAX_LIST_SIZE); i++)
    {
        pH264Ctx->pListX[0][i] = NULL;
    }
    for (i = pH264Ctx->CurrSlice.listXsize[1]; i<(MAX_LIST_SIZE); i++)
    {
        pH264Ctx->pListX[1][i] = NULL;
    }

    return H264_OK;
}


/***********************************************************************************
* Function:    DecList(H264_CTX_S *pH264Ctx)
* Description: ���ɵ�ǰslice�Ĳο��б�
* Input:       pH264Ctx->CurrSlice   ��ǰSlice����
*              g_pListX[2][MAX_LIST_SIZE]   List�б�
* Output:      g_pListX[2][MAX_LIST_SIZE]   List�б�
* Return:      DEC_NORMAL   ������ȷ����List�б�
*              DEC_ERR      ��������ȷ����List�б�
* Others:      ��
***********************************************************************************/
VOID DumpList(H264_CTX_S *pH264Ctx)
{
    UINT32 i, j; // just for write log
    UINT32 type;

    /* ��Ϣ���͹��� */
    type = 1 << PRN_REF;
    if (0 != (g_PrintEnable & type)) /* ������ӡû�д� */
    {
        for (j=0; j<2; j++)
        {
            for (i=0; i<pH264Ctx->CurrSlice.listXsize[j]; i++)
            {
                dprint(PRN_REF, "list[%d][%d]: frame_num=%d, poc=%d\n", j, i,
                    pH264Ctx->pListX[j][i]->frame_store->frame_num, pH264Ctx->pListX[j][i]->frame_store->poc);
            }
        }

        if (pH264Ctx->CurrSlice.listXsize[0]>0)
        {
            dprint(PRN_REF, "L0: ");
            for (i=0; i < pH264Ctx->CurrSlice.listXsize[0]; i++)
            {
                switch (pH264Ctx->pListX[0][i]->structure)
                {
                case FRAME:
                    dprint(PRN_REF, "%d ", pH264Ctx->pListX[0][i]->frame_store->poc);
                    break;
                case TOP_FIELD:
                    dprint(PRN_REF, "%dt ", pH264Ctx->pListX[0][i]->frame_store->poc);
                    break;
                case BOTTOM_FIELD:
                    dprint(PRN_REF, "%db ", pH264Ctx->pListX[0][i]->frame_store->poc);
                    break;
                default:
                    break;
                }
            }
            dprint(PRN_REF, "\n");
        }

        if (pH264Ctx->CurrSlice.listXsize[1]>0)
        {
            dprint(PRN_REF, "L1:");
            for (i=0; i < pH264Ctx->CurrSlice.listXsize[1]; i++)
            {
                switch (pH264Ctx->pListX[1][i]->structure)
                {
                case FRAME:
                    dprint(PRN_REF, "%d ", pH264Ctx->pListX[1][i]->frame_store->poc);
                    break;
                case TOP_FIELD:
                    dprint(PRN_REF, "%dt ", pH264Ctx->pListX[1][i]->frame_store->poc);
                    break;
                case BOTTOM_FIELD:
                    dprint(PRN_REF, "%db ", pH264Ctx->pListX[1][i]->frame_store->poc);
                    break;
                default:
                    break;
                }
            }
            dprint(PRN_REF, "\n");
        }
    }
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 FindNearestPOCPicId(H264_CTX_S *pH264Ctx)
*   �������� :  ��dpb����poc�����ͼ������islice�޲�
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
SINT32 FindNearestPOCPicId(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    SINT32 nearest_pic_id;
    SINT32 curr_poc, nearest_poc, nearest_poc_diff, new_poc_diff;

    if (pH264Ctx->DPB.used_size < 1)
    {
        return -1;  // return  a err pic id
    }

    curr_poc = pH264Ctx->CurrPic.thispoc;
    nearest_pic_id = 0;
    nearest_poc = pH264Ctx->DPB.fs[0]->poc;
    nearest_poc_diff = ABS(curr_poc - nearest_poc);

    for (i=1; i<pH264Ctx->DPB.used_size; i++)
    {
        //if (g_DPB.fs[i]->err_level < 3) //------------------------------�������ܻ�򿪴˴�
        {
            new_poc_diff = ABS(curr_poc - pH264Ctx->DPB.fs[i]->poc);
            if (new_poc_diff < nearest_poc_diff)
            {
                nearest_pic_id =  pH264Ctx->DPB.fs[i]->fs_image.image_id;
                //nearest_poc = pH264Ctx->DPB.fs[i]->poc;
                nearest_poc_diff = new_poc_diff;
            }
        }
    }

    return nearest_pic_id;
}


/***********************************************************************************
* Function:    FindMinRefIdx(H264_CTX_S *pH264Ctx)
* Description: �ҵ�ÿһ��apc��list0�е���Сrefidx
* Input:       ��
*
* Output:  ��(�ı�g_APC�е�ֵ)
* Return:  ��
*
* Others:      ��
***********************************************************************************/
VOID FindMinRefIdx(H264_CTX_S *pH264Ctx)
{
    UINT32 i, j;
    UINT32 minrefidx;

    if (I_SLICE == pH264Ctx->CurrSlice.slice_type)
    {
        return;
    }

    if (0 == pH264Ctx->CurrPic.structure) // frame reference
    {
        for (i = 0; i < 16; i++)
        {
            minrefidx = 32;
            for (j = 0; j < pH264Ctx->CurrSlice.listXsize[0]; j++)
            {
                if (i == pH264Ctx->pListX[0][j]->frame_store->apc_idc)
                {
                    //find same apc, compare refidx
                    minrefidx = MIN(j, minrefidx);
                    pH264Ctx->APC.RefIdx[2*i]  = minrefidx;
                    pH264Ctx->APC.RefIdx[2*i + 1]  = minrefidx;
                }
            }

            if (32 <= minrefidx)
            {
                pH264Ctx->APC.RefIdx[2*i] = 0;
                pH264Ctx->APC.RefIdx[2*i + 1] = 0;
            }
        }
    }
    else // field reference
    {
        for (i = 0; i < 32; i++)
        {
            minrefidx = 32;
            for (j = 0; j < pH264Ctx->CurrSlice.listXsize[0]; j++)
            {
                if (i == ((pH264Ctx->pListX[0][j]->frame_store->apc_idc << 1) | (2 == pH264Ctx->pListX[0][j]->structure)))
                {
                    //find same apc, compare refidx
                    minrefidx = MIN(j, minrefidx);
                    pH264Ctx->APC.RefIdx[i]  = minrefidx;
                }
            }

            if (32 <= minrefidx)
            {
                pH264Ctx->APC.RefIdx[i] = 0;
            }
        }
    }

    return;
}


/***********************************************************************************
* Function:    DecList(H264_CTX_S *pH264Ctx)
* Description: ���ɵ�ǰslice�Ĳο��б�
* Input:       pH264Ctx->CurrSlice   ��ǰSlice����
*              g_pListX[2][MAX_LIST_SIZE]   List�б�
* Output:      g_pListX[2][MAX_LIST_SIZE]   List�б�
* Return:      DEC_NORMAL   ������ȷ����List�б�
*              DEC_ERR      ��������ȷ����List�б�
* Others:      ��
***********************************************************************************/
SINT32 DecList(H264_CTX_S *pH264Ctx)
{
    SINT32 ret;

    pH264Ctx->CurrSlice.picid_nearpoc = FindNearestPOCPicId(pH264Ctx);
    pH264Ctx->CurrSlice.picid_refidx0 = -1;  // default pic id

    if (I_SLICE == pH264Ctx->CurrSlice.slice_type)
    {
        pH264Ctx->CurrSlice.listXsize[0] = 0;
        pH264Ctx->CurrSlice.listXsize[1] = 0;
        return H264_OK;
    }

    ret = InitListX(pH264Ctx);
    if (H264_OK != ret)
    {
        dprint(PRN_FATAL, "line: %d InitListX failed!\n", __LINE__);
        return H264_ERR;
    }

    dprint(PRN_REF, "L0: list after init.\n");
    DumpList(pH264Ctx);

    ReorderListX(pH264Ctx);
    ret = RepairList(pH264Ctx);
    if (H264_OK != ret)
    {
        return H264_ERR;
    }

    //add find apc->refidx(min)
    FindMinRefIdx(pH264Ctx);

    pH264Ctx->CurrSlice.picid_refidx0 = (pH264Ctx->pListX[0][0] != NULL) ? pH264Ctx->pListX[0][0]->frame_store->fs_image.image_id : -1;
    dprint(PRN_REF, "L0: list after reorder.\n");
    DumpList(pH264Ctx);

    return H264_OK;
}


/*
************************************************************************
*      ����ԭ�� :  VOID NoPicOut(H264_CTX_S *pH264Ctx)
*      �������� :  �ջ�DPB������ͼ��VO������ͼ���ջ�
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
VOID NoPicOut(H264_CTX_S *pH264Ctx)
{
    UINT32 i;

    // no pic out put, now get back g_frame_store in dpb compulsive
    for (i=0; i<pH264Ctx->TotalFsNum; i++)
    {
        if ((1 ==pH264Ctx->FrameStore[i].is_displayed) || (1 == pH264Ctx->FrameStore[i].is_in_dpb))
        {
            pH264Ctx->FrameStore[i].is_used = 0;
            pH264Ctx->FrameStore[i].is_reference = 0;
        }
    }

    // clean all pmv store
    for (i=0; i<pH264Ctx->TotalPmvNum; i++)
    {
        pH264Ctx->PmvStoreUsedFlag[i] = 0;
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 InitDPB(H264_CTX_S *pH264Ctx,UINT32 ReRangeFlg)
*      �������� :  ��ʼ��dpb
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
SINT32 InitDPB(H264_CTX_S *pH264Ctx)
{
    UINT32 i, j;
    SINT32 ret;
    ClearAllSlice(pH264Ctx);

    // DPB refresh
    for (i=0; i<16; i++)
    {
        if(pH264Ctx->DPB.fs[i] != NULL)
        {
            FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->DPB.fs[i]->logic_fs_id, 1);
        }
        if(pH264Ctx->DPB.fs_ref[i] != NULL)
        {
            FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->DPB.fs_ref[i]->logic_fs_id, 1);
        }
        if(pH264Ctx->DPB.fs_ltref[i] != NULL)
        {
            FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->DPB.fs_ltref[i]->logic_fs_id, 1);
        }
        pH264Ctx->DPB.fs[i] = pH264Ctx->DPB.fs_ref[i] = pH264Ctx->DPB.fs_ltref[i] = NULL;
    }
    pH264Ctx->DPB.size = pH264Ctx->CurrSPS.dpb_size_plus1 - 1;
    pH264Ctx->DPB.used_size = 0;
    pH264Ctx->DPB.ref_frames_in_buffer = 0;
    pH264Ctx->DPB.ltref_frames_in_buffer = 0;
    pH264Ctx->DPB.max_long_term_pic_idx = 0;
    /* lkf58351 20120110: �ڽ�pDirectOutBuf���ǰӦ�ж����Ƿ񻹲��������ݣ���˵���ѷ������߼�֡�棬��Ҫ�ͷŵ� */
    if((pH264Ctx->pDirectOutBuf != NULL) && (pH264Ctx->OldDecMode == I_MODE))
    {
        ret = OutputFrmToVO(pH264Ctx, pH264Ctx->pDirectOutBuf, 0);
    }
    if(pH264Ctx->pDirectOutBuf != NULL)
    {
        if (NULL != pH264Ctx->CurrPic.frame_store)
        {
            FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
        }
        pH264Ctx->pDirectOutBuf = NULL;
    }

    // apc init
    memset(&pH264Ctx->APC, 0, sizeof(H264_APC_S));
    pH264Ctx->APC.size = pH264Ctx->DPB.size;
    pH264Ctx->APC.used_size = 0;

    // pmv slot refresh
    for (i=0; i<MAX_PMV_STORE; i++)
    {
        pH264Ctx->PmvStoreUsedFlag[i] = 0;
    }

    // list refresh
    for (i=0; i<2; i++)
    {
        for (j=0; j<MAX_LIST_SIZE; j++)
        {
            pH264Ctx->pListX[i][j] = NULL;
        }
        pH264Ctx->CurrSlice.listXsize[i] = 0;
    }

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID GetBackPicFromVOQueue(H264_CTX_S *pH264Ctx)
*      �������� :  ǿ���ջش�����VO�����е�framestore
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
VOID GetBackPicFromVOQueue(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    UINT32 usrdat_idx;

    // clean VO queue
    ResetVoQueue(&pH264Ctx->ImageQue);
        
    // clean all frame store
    for (i=0; i<pH264Ctx->TotalFsNum; i++)
    {
        pH264Ctx->FrameStore[i].is_used = 0;
        pH264Ctx->FrameStore[i].is_reference = 0;
        for (usrdat_idx=0; usrdat_idx <4; usrdat_idx++)
        {
            if (NULL != pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx])
            {
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx]);
                pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx] = NULL;
            }
        }
    }

    // clean all pmv store
    for (i=0; i<pH264Ctx->TotalPmvNum; i++)
    {
        pH264Ctx->PmvStoreUsedFlag[i] = 0;
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :      SINT32 GetReRangeFlag(H264_CTX_S *pH264Ctx)
*      �������� :  �ж�֡��ռ��Ƿ�Ҫ���·���
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
SINT32 GetReRangeFlag(H264_CTX_S *pH264Ctx, UINT32 *p_dpb_changed)
{
    SINT32 ReRangeFlg;
    H264_SPS_S *pSPS;
    H264_PPS_S *pPPS;
    UINT32    oldw;
    UINT32    oldh;
    UINT32    neww;
    UINT32    newh;
    UINT32    oldasp;
    UINT32    newasp;
    UINT32    dpb_changed;

    ReRangeFlg = 0;
    pPPS = &pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id];
    pSPS = &pH264Ctx->SPS[pPPS->seq_parameter_set_id];

    /* z56361, 2011-11-1, oldw, oldh��֮ǰ�ɹ�����֡����¼����ֵ�������������жϿ���Ƿ�
       �仯ʱ���ÿ�AllowStartDec. ֮ǰ��д���ȽϷѽ���������. */
    oldw = pH264Ctx->CurWidthInMb;
    oldh = pH264Ctx->CurHeightInMb;
    neww = pSPS->pic_width_in_mbs_minus1 + 1;
    newh = (pSPS->pic_height_in_map_units_minus1 + 1)*(2 - pSPS->frame_mbs_only_flag);

    /* z56361, 20111202, dpb������Ҳ��Ҫ���»���֡�棬���Թ鵽֡��ߴ�仯���� */
    if ( ( neww != oldw) || ( newh != oldh) || (pSPS->dpb_size_plus1 > (pH264Ctx->DPB.size+1)))
    {
        ReRangeFlg = 1;
        REPORT_IMGSIZE_CHANGE( pH264Ctx->ChanID, oldw*16, oldh*16, neww*16, newh*16 );
        //dprint(PRN_ALWS,"H264 ReRangeFlag=1(resolution:%dx%d -> %dx%d, dpb_size:%d -> %d\n", oldw, oldh, neww, newh, (pH264Ctx->DPB.size+1), pSPS->dpb_size_plus1);
    }

    dpb_changed = 0;
    dpb_changed |= ( (pSPS->dpb_size_plus1 == pH264Ctx->CurrSPS.dpb_size_plus1) ? 0 : 1 );
    dpb_changed |= (pH264Ctx->CurrSPS.seq_parameter_set_id != pSPS->seq_parameter_set_id);
    dpb_changed |= ReRangeFlg;
    *p_dpb_changed = dpb_changed;

    oldasp = pH264Ctx->CurrSPS.vui_seq_parameters.aspect_ratio;
    newasp = pSPS->vui_seq_parameters.aspect_ratio;
    if (oldasp != newasp)
    {
        //REPORT_ASPR_CHANGE( oldasp, newasp);
    }

    return ReRangeFlg;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID DecPOC(H264_CTX_S *pH264Ctx)
*   �������� :  ��ǰ�ν���ͼ�����g_DPB����
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
VOID DecPOC(H264_CTX_S *pH264Ctx)
{
    H264_SPS_S *sps;
    H264_PPS_S *pps;
    SINT32 i;
    UINT32 MaxPicOrderCntLsb;
    UINT32 MaxFrameNum;
    SINT32 idr_flag = (NALU_TYPE_IDR == pH264Ctx->CurrSlice.nal_unit_type);

    pps = &pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id];
    sps = &pH264Ctx->SPS[pps->seq_parameter_set_id];
    MaxPicOrderCntLsb = (1 << (sps->log2_max_pic_order_cnt_lsb_minus4+4));
    MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4+4);

    switch (sps->pic_order_cnt_type)
    {
    case 0:
        /* 1st */
        if (idr_flag)
        {
            pH264Ctx->CurrPOC.PrevPicOrderCntMsb = 0;
            pH264Ctx->CurrPOC.PrevPicOrderCntLsb = 0;
        }
        else
        {
            if (pH264Ctx->CurrPOC.last_has_mmco_5)
            {
                if (pH264Ctx->CurrPOC.last_pic_bottom_field)
                {
                    pH264Ctx->CurrPOC.PrevPicOrderCntMsb = 0;
                    pH264Ctx->CurrPOC.PrevPicOrderCntLsb = 0;
                }
                else
                {
                    pH264Ctx->CurrPOC.PrevPicOrderCntMsb = 0;
                    pH264Ctx->CurrPOC.PrevPicOrderCntLsb = pH264Ctx->CurrPOC.toppoc;
                }
            }
        }
        /* Calculate the MSBs of current picture */
        if (pH264Ctx->CurrPOC.pic_order_cnt_lsb < pH264Ctx->CurrPOC.PrevPicOrderCntLsb &&
            (pH264Ctx->CurrPOC.PrevPicOrderCntLsb - pH264Ctx->CurrPOC.pic_order_cnt_lsb) >= (MaxPicOrderCntLsb / 2))
        {
            pH264Ctx->CurrPOC.PicOrderCntMsb = pH264Ctx->CurrPOC.PrevPicOrderCntMsb + MaxPicOrderCntLsb;
        }
        else if ((pH264Ctx->CurrPOC.pic_order_cnt_lsb>pH264Ctx->CurrPOC.PrevPicOrderCntLsb) &&
            (pH264Ctx->CurrPOC.pic_order_cnt_lsb-pH264Ctx->CurrPOC.PrevPicOrderCntLsb) > (MaxPicOrderCntLsb/2))
        {
            pH264Ctx->CurrPOC.PicOrderCntMsb = pH264Ctx->CurrPOC.PrevPicOrderCntMsb - MaxPicOrderCntLsb;
        }
        else
        {
            pH264Ctx->CurrPOC.PicOrderCntMsb = pH264Ctx->CurrPOC.PrevPicOrderCntMsb;
        }

        /* 2nd */
        if (!pH264Ctx->CurrSlice.field_pic_flag)
        {
            /* frame pix */
            pH264Ctx->CurrPOC.toppoc = pH264Ctx->CurrPOC.PicOrderCntMsb + pH264Ctx->CurrPOC.pic_order_cnt_lsb;
            pH264Ctx->CurrPOC.bottompoc = pH264Ctx->CurrPOC.toppoc + pH264Ctx->CurrPOC.delta_pic_order_cnt_bottom;
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.framepoc = ((pH264Ctx->CurrPOC.toppoc < pH264Ctx->CurrPOC.bottompoc) ? pH264Ctx->CurrPOC.toppoc : pH264Ctx->CurrPOC.bottompoc);
        }
        else if (!pH264Ctx->CurrSlice.bottom_field_flag)
        {
            /* top field */
            pH264Ctx->CurrPOC.ThisPOC= pH264Ctx->CurrPOC.toppoc = pH264Ctx->CurrPOC.PicOrderCntMsb + pH264Ctx->CurrPOC.pic_order_cnt_lsb;
        }
        else
        {
            /* bottom field */
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.bottompoc = pH264Ctx->CurrPOC.PicOrderCntMsb + pH264Ctx->CurrPOC.pic_order_cnt_lsb;
        }
        pH264Ctx->CurrPOC.framepoc = pH264Ctx->CurrPOC.ThisPOC;

        if (pH264Ctx->CurrPOC.frame_num != pH264Ctx->CurrPOC.PreviousFrameNum)
        {
            pH264Ctx->CurrPOC.PreviousFrameNum = pH264Ctx->CurrPOC.frame_num;
        }

        if (0 != pH264Ctx->CurrSlice.nal_ref_idc)
        {
            pH264Ctx->CurrPOC.PrevPicOrderCntLsb = pH264Ctx->CurrPOC.pic_order_cnt_lsb;
            pH264Ctx->CurrPOC.PrevPicOrderCntMsb = pH264Ctx->CurrPOC.PicOrderCntMsb;
        }
        break;

    case 1:
        /* 1st */
        if (idr_flag)
        {
            pH264Ctx->CurrPOC.FrameNumOffset=0;     /*  first pix of IDRGOP,  */
            //g_CurrPOC.delta_pic_order_cnt[0]=0; /* ignore first delta */
        }
        else
        {
            if (pH264Ctx->CurrPOC.last_has_mmco_5)
            {
                pH264Ctx->CurrPOC.PrevFrameNumOffset = 0;
                pH264Ctx->CurrPOC.PreviousFrameNum = 0;
            }
            if (pH264Ctx->CurrPOC.frame_num < pH264Ctx->CurrPOC.PreviousFrameNum)
            {
                /* not first pix of IDRGOP */
                pH264Ctx->CurrPOC.FrameNumOffset = pH264Ctx->CurrPOC.PrevFrameNumOffset + MaxFrameNum;
            }
            else
            {
                pH264Ctx->CurrPOC.FrameNumOffset = pH264Ctx->CurrPOC.PrevFrameNumOffset;
            }
        }

        /* 2nd */
        if (sps->num_ref_frames_in_pic_order_cnt_cycle)
        {
            pH264Ctx->CurrPOC.AbsFrameNum = pH264Ctx->CurrPOC.FrameNumOffset + pH264Ctx->CurrPOC.frame_num;
        }
        else
        {
            pH264Ctx->CurrPOC.AbsFrameNum=0;
        }
        if ((0 == pH264Ctx->CurrSlice.nal_ref_idc) && (0 <pH264Ctx->CurrPOC.AbsFrameNum))
        {
            pH264Ctx->CurrPOC.AbsFrameNum--;
        }
        // 3rd
        pH264Ctx->CurrPOC.ExpectedDeltaPerPicOrderCntCycle = 0;

        for (i=0; i<(SINT32)sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            pH264Ctx->CurrPOC.ExpectedDeltaPerPicOrderCntCycle += sps->offset_for_ref_frame[i];
        }
        if (pH264Ctx->CurrPOC.AbsFrameNum)
        {
            pH264Ctx->CurrPOC.PicOrderCntCycleCnt = (pH264Ctx->CurrPOC.AbsFrameNum-1) / sps->num_ref_frames_in_pic_order_cnt_cycle;
            pH264Ctx->CurrPOC.FrameNumInPicOrderCntCycle = (pH264Ctx->CurrPOC.AbsFrameNum-1) % sps->num_ref_frames_in_pic_order_cnt_cycle;
            pH264Ctx->CurrPOC.ExpectedPicOrderCnt = pH264Ctx->CurrPOC.PicOrderCntCycleCnt * pH264Ctx->CurrPOC.ExpectedDeltaPerPicOrderCntCycle;
            for (i=0; i<=(SINT32)pH264Ctx->CurrPOC.FrameNumInPicOrderCntCycle; i++)
            {
                pH264Ctx->CurrPOC.ExpectedPicOrderCnt += sps->offset_for_ref_frame[i];
            }
        }
        else
        {
            pH264Ctx->CurrPOC.ExpectedPicOrderCnt = 0;
        }
        if (0 == pH264Ctx->CurrSlice.nal_ref_idc)
        {
            pH264Ctx->CurrPOC.ExpectedPicOrderCnt += sps->offset_for_non_ref_pic;
        }
        if (!(pH264Ctx->CurrSlice.field_pic_flag))
        {
            /* frame pix */
            pH264Ctx->CurrPOC.toppoc = pH264Ctx->CurrPOC.ExpectedPicOrderCnt + pH264Ctx->CurrPOC.delta_pic_order_cnt[0];
            pH264Ctx->CurrPOC.bottompoc = pH264Ctx->CurrPOC.toppoc + sps->offset_for_top_to_bottom_field + pH264Ctx->CurrPOC.delta_pic_order_cnt[1];
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.framepoc = ((pH264Ctx->CurrPOC.toppoc < pH264Ctx->CurrPOC.bottompoc) ? pH264Ctx->CurrPOC.toppoc : pH264Ctx->CurrPOC.bottompoc);
        }
        else if (!(pH264Ctx->CurrSlice.bottom_field_flag))
        {
            /* top field  */
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.toppoc = pH264Ctx->CurrPOC.ExpectedPicOrderCnt + pH264Ctx->CurrPOC.delta_pic_order_cnt[0];
        }
        else
        {
            /* bottom field */
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.bottompoc = pH264Ctx->CurrPOC.ExpectedPicOrderCnt + sps->offset_for_top_to_bottom_field + pH264Ctx->CurrPOC.delta_pic_order_cnt[0];
        }
        pH264Ctx->CurrPOC.framepoc = pH264Ctx->CurrPOC.ThisPOC;

        pH264Ctx->CurrPOC.PreviousFrameNum = pH264Ctx->CurrPOC.frame_num;
        pH264Ctx->CurrPOC.PrevFrameNumOffset = pH264Ctx->CurrPOC.FrameNumOffset;
        break;

    case 2:
        if (idr_flag)
        {
            pH264Ctx->CurrPOC.FrameNumOffset=0;     /*  first pix of IDRGOP, */
            pH264Ctx->CurrPOC.ThisPOC = pH264Ctx->CurrPOC.framepoc = pH264Ctx->CurrPOC.toppoc = pH264Ctx->CurrPOC.bottompoc = 0;
        }
        else
        {
            if (pH264Ctx->CurrPOC.last_has_mmco_5)
            {
                pH264Ctx->CurrPOC.PreviousFrameNum = 0;
                pH264Ctx->CurrPOC.PrevFrameNumOffset = 0;
            }
            if (pH264Ctx->CurrPOC.frame_num < pH264Ctx->CurrPOC.PreviousFrameNum)
            {
                pH264Ctx->CurrPOC.FrameNumOffset = pH264Ctx->CurrPOC.PrevFrameNumOffset + MaxFrameNum;
            }
            else
            {
                pH264Ctx->CurrPOC.FrameNumOffset = pH264Ctx->CurrPOC.PrevFrameNumOffset;
            }

            pH264Ctx->CurrPOC.AbsFrameNum = pH264Ctx->CurrPOC.FrameNumOffset + pH264Ctx->CurrPOC.frame_num;
            if (0 == pH264Ctx->CurrSlice.nal_ref_idc)
            {
                pH264Ctx->CurrPOC.ThisPOC = (2*pH264Ctx->CurrPOC.AbsFrameNum - 1);
            }
            else
            {
                pH264Ctx->CurrPOC.ThisPOC = (2*pH264Ctx->CurrPOC.AbsFrameNum);
            }

            if (!pH264Ctx->CurrSlice.field_pic_flag)
            {
                pH264Ctx->CurrPOC.toppoc = pH264Ctx->CurrPOC.bottompoc = pH264Ctx->CurrPOC.framepoc = pH264Ctx->CurrPOC.ThisPOC;
            }
            else if (!pH264Ctx->CurrSlice.bottom_field_flag)
            {
                pH264Ctx->CurrPOC.toppoc = pH264Ctx->CurrPOC.framepoc = pH264Ctx->CurrPOC.ThisPOC;
            }
            else
            {
                pH264Ctx->CurrPOC.bottompoc = pH264Ctx->CurrPOC.framepoc = pH264Ctx->CurrPOC.ThisPOC;
            }
        }

        pH264Ctx->CurrPOC.PreviousFrameNum = pH264Ctx->CurrPOC.frame_num;
        pH264Ctx->CurrPOC.PrevFrameNumOffset = pH264Ctx->CurrPOC.FrameNumOffset;

        break;

    default:
        /* error must occurs */
        break;
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :   VOID CalcPicNum(VOID)
*      �������� :
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
VOID CalcPicNum(H264_CTX_S *pH264Ctx)
{
    H264_SPS_S *pSPS = &pH264Ctx->SPS[pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id].seq_parameter_set_id];
    UINT32 i;
    UINT32 MaxFrameNum = 1 << (pSPS->log2_max_frame_num_minus4 + 4);
    SINT32 add_top, add_bottom;

    add_top = (TOP_FIELD == pH264Ctx->CurrPic.structure) ? 1 : 0;
    add_bottom = (BOTTOM_FIELD == pH264Ctx->CurrPic.structure) ? 1 : 0;

    // pic num calculate
    if (FRAME==pH264Ctx->CurrPic.structure)
    {
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if (3==pH264Ctx->DPB.fs_ref[i]->is_reference) // frame, field pair
            {
                if ((1==pH264Ctx->DPB.fs_ref[i]->frame.is_short_term) && (0==pH264Ctx->DPB.fs_ref[i]->frame.is_long_term)) // ���ڲο�
                {
                    if (pH264Ctx->DPB.fs_ref[i]->frame_num > pH264Ctx->CurrPic.frame_num)
                    {
                        pH264Ctx->DPB.fs_ref[i]->frame_num_wrap = pH264Ctx->DPB.fs_ref[i]->frame_num - MaxFrameNum;
                    }
                    else
                    {
                        pH264Ctx->DPB.fs_ref[i]->frame_num_wrap = pH264Ctx->DPB.fs_ref[i]->frame_num;
                    }
                    pH264Ctx->DPB.fs_ref[i]->frame.pic_num = pH264Ctx->DPB.fs_ref[i]->frame_num_wrap;
                }
            }
        }

        // update long_term_pic_num
        for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
        {
            if (3 == pH264Ctx->DPB.fs_ltref[i]->is_reference) // field pair
            {
                if ((0 == pH264Ctx->DPB.fs_ltref[i]->frame.is_short_term) && (1 == pH264Ctx->DPB.fs_ltref[i]->frame.is_long_term))
                {
                    pH264Ctx->DPB.fs_ltref[i]->frame.long_term_pic_num = pH264Ctx->DPB.fs_ltref[i]->long_term_frame_idx;
                }
            }
        }
    }
    else //field
    {
        for (i=0; i<pH264Ctx->DPB.ref_frames_in_buffer; i++)
        {
            if (0 != pH264Ctx->DPB.fs_ref[i]->is_reference)
            {
                if (pH264Ctx->DPB.fs_ref[i]->frame_num > pH264Ctx->CurrPic.frame_num)
                {
                    pH264Ctx->DPB.fs_ref[i]->frame_num_wrap = pH264Ctx->DPB.fs_ref[i]->frame_num - MaxFrameNum;
                }
                else
                {
                    pH264Ctx->DPB.fs_ref[i]->frame_num_wrap = pH264Ctx->DPB.fs_ref[i]->frame_num;
                }
                if ((pH264Ctx->DPB.fs_ref[i]->is_reference & 1) &&
                    (1==pH264Ctx->DPB.fs_ref[i]->top_field.is_short_term) && (0==pH264Ctx->DPB.fs_ref[i]->top_field.is_long_term)) // include top field
                {
                    pH264Ctx->DPB.fs_ref[i]->top_field.pic_num = (2*pH264Ctx->DPB.fs_ref[i]->frame_num_wrap) + add_top;
                }
                if ((pH264Ctx->DPB.fs_ref[i]->is_reference & 2) &&
                    (1==pH264Ctx->DPB.fs_ref[i]->bottom_field.is_short_term) && (0==pH264Ctx->DPB.fs_ref[i]->bottom_field.is_long_term)) // include bottom field
                {
                    pH264Ctx->DPB.fs_ref[i]->bottom_field.pic_num = (2*pH264Ctx->DPB.fs_ref[i]->frame_num_wrap) + add_bottom;
                }
            }
        }
        // update long_term_pic_num
        for (i=0; i<pH264Ctx->DPB.ltref_frames_in_buffer; i++)
        {
            if (0 != pH264Ctx->DPB.fs_ltref[i]->is_reference)
            {
                if ((pH264Ctx->DPB.fs_ltref[i]->is_reference & 1) &&
                    (0==pH264Ctx->DPB.fs_ltref[i]->top_field.is_short_term) && (1==pH264Ctx->DPB.fs_ltref[i]->top_field.is_long_term)) // include top field
                {
                    pH264Ctx->DPB.fs_ltref[i]->top_field.long_term_pic_num = 2*pH264Ctx->DPB.fs_ltref[i]->long_term_frame_idx + add_top;
                }
                if ((pH264Ctx->DPB.fs_ltref[i]->is_reference & 2) &&
                    (0==pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_short_term) && (1==pH264Ctx->DPB.fs_ltref[i]->bottom_field.is_long_term))
                {
                    pH264Ctx->DPB.fs_ltref[i]->bottom_field.long_term_pic_num = 2*pH264Ctx->DPB.fs_ltref[i]->long_term_frame_idx + add_bottom;
                }
            }
        }
    }
}


SINT32  IsOutDPB(H264_CTX_S *pH264Ctx,H264_FRAMESTORE_S  *pFrameStore)
{
    UINT32 i;

    if (NULL == pFrameStore)
    {
        return 1;
    }

    for (i = 0; i < pH264Ctx->DPB.used_size; i++)
    {
        if (pH264Ctx->DPB.fs[i] == pFrameStore)
        {
            return 0;
        }
    }

    return 1;
}

/*!
************************************************************************
*      ����ԭ�� :  SINT32 AllocFrameStore(VOID)
*      �������� :  ����ǰͼ�����һ��֡��ռ�,pmv��λ
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL:��ɹ�;
*                  FS_ALLOC_ERR:�ܷ��䵽PMV��λ,�������ò��ɻָ���
*                               ��Ҫ�����������루��ĳ�̶ֳ���˵��
*                               �������֣��ض��ǳ�����bug��Ҫ��λ
*                               �����
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
SINT32 AllocFrameStore(H264_CTX_S *pH264Ctx, SINT32 IsDecGap)
{
    UINT32 structure = pH264Ctx->CurrSlice.field_pic_flag ? (pH264Ctx->CurrSlice.bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD) : FRAME;
    UINT32 i = 0;
    SINT32 LogicFsID = 0;
    FSP_LOGIC_FS_S *pstLFs = NULL;

    if (I_MODE == pH264Ctx->OldDecMode)
    {
    	pH264Ctx->CurrPic.field_pair_flag = 0;
    }
    else
    {
    	/*�ɶԳ����ж�*/
        pH264Ctx->CurrPic.field_pair_flag = 0;
        if ((BOTTOM_FIELD == structure && TOP_FIELD == pH264Ctx->PrevPic.structure) ||
            (TOP_FIELD == structure && BOTTOM_FIELD == pH264Ctx->PrevPic.structure))
        {
            if ((pH264Ctx->OldDecMode == I_MODE ||pH264Ctx->PrevPic.frame_num == pH264Ctx->CurrSlice.frame_num) &&
                ((0 == pH264Ctx->CurrSlice.nal_ref_idc && 0 == pH264Ctx->PrevPic.nal_ref_idc) ||
                (0 != pH264Ctx->CurrSlice.nal_ref_idc && 0 != pH264Ctx->PrevPic.nal_ref_idc)))
            {
                if ((pH264Ctx->PicCounter - pH264Ctx->PrevStorePicCounter) <= 2)
                {
                    pH264Ctx->CurrPic.field_pair_flag = 1;
    				if (pH264Ctx->CurrPic.frame_store != NULL)
    				{
                        if (BOTTOM_FIELD == structure)
        				{
        				    if (pH264Ctx->CurrPic.frame_store->is_used != 1)
        				    {
        				        pH264Ctx->CurrPic.field_pair_flag = 0;
        				    }
        				}

                        if (TOP_FIELD == structure)
        				{
        				    if (pH264Ctx->CurrPic.frame_store->is_used != 2)
        				    {
        				        pH264Ctx->CurrPic.field_pair_flag = 0;
        				    }
                        }
    				}
    				else
    				{
    				    pH264Ctx->CurrPic.field_pair_flag = 0;
    				}
				}
            }
        }
        else
        {
            pH264Ctx->CurrPic.field_pair_flag = 0;
        }
       #ifdef VFMW_CFG_CAP_HD_SIMPLE
       /* �ѵ�����������˵�,��ǰ���ɶԣ���֮ǰ����һ����ͼ */
        if((0 == pH264Ctx->CurrPic.field_pair_flag) && ((TOP_FIELD == pH264Ctx->PrevPic.structure)
            || (BOTTOM_FIELD == pH264Ctx->PrevPic.structure)))
        {
            RemoveFrameStoreOutDPB(pH264Ctx, pH264Ctx->PrevPic.Dpb_pos);
            FSP_SetRef(pH264Ctx->ChanID, pH264Ctx->PrevPic.image_id, 0);
            FSP_SetDisplay(pH264Ctx->ChanID, pH264Ctx->PrevPic.image_id, 0);
        }
       #endif
    }


    if (0 == pH264Ctx->CurrPic.field_pair_flag || NULL == pH264Ctx->CurrPic.frame_store)
    {
        // allocate frame_store
        pH264Ctx->CurrPic.state = EMPTY;

        for (i=0; i<pH264Ctx->TotalFsNum; i++)
        {
            if ((0 == pH264Ctx->FrameStore[i].is_used) && (1 == IsOutDPB(pH264Ctx, &pH264Ctx->FrameStore[i])))
            {
                pH264Ctx->CurrPic.state = PIC_DECODING;
                pH264Ctx->CurrPic.frame_store = &pH264Ctx->FrameStore[i];
                pH264Ctx->CurrPic.frame_store->err_level = 0;
                /* error_level������Ϊ�д������ڽ�gap����������ԭ����ʵ��δ���뵫������StorePicInDPB()�Ĳ��������������
                   ���������ͼ�п��ܱ����*/
                pH264Ctx->CurrPic.err_level = 101;
                pH264Ctx->CurrPic.fs_image = pH264Ctx->FrameStore[i].fs_image;
                pH264Ctx->CurrPic.fs_image.error_level = 0;
                pH264Ctx->CurrPic.frame_store->mode_skip_flag = 0;
                pH264Ctx->CurrPic.frame_store->is_displayed = 0;
                pH264Ctx->CurrPic.frame_store->is_reference = 0;
                pH264Ctx->CurrPic.frame_store->is_in_dpb = 1;
                pH264Ctx->CurrPic.frame_store->eFoState = FO_STATE_NA;
                // ���һЩ������գ����ڴ���DPBʱ����ȷ��
                break;
            }
        }

        if (pH264Ctx->CurrPic.state != PIC_DECODING)
        {
            dprint(PRN_FATAL, "FS_ALLOC_ERR, ClearAll\n");
            ClearAll(pH264Ctx,1);
            return H264_ERR;  //FS_ALLOC_ERR
        }

        /* FrameStore���뵽֮��, ��Ҫ�½��߼�֡�棬�������ܵõ������ĵ�ַ */
        LogicFsID = FSP_NewLogicFs(pH264Ctx->ChanID, (IsDecGap? 0: 1));
        pstLFs = FSP_GetLogicFs(pH264Ctx->ChanID, LogicFsID);

        if (LogicFsID >= 0 && NULL != pstLFs)
        {
            if (pH264Ctx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION)
            {
                IMAGE *pstImg = &pstLFs->stDispImg;
                UINT32 HeightAligh = pH264Ctx->CurHeightInMb*16;
                UINT32 ChromOffset = HeightAligh*pstImg->image_stride;

                pstImg->image_width        = pH264Ctx->CurWidthInMb*16;
                pstImg->image_height       = HeightAligh;
				//liucan mark it. fsp���Ѿ���ֵ��
            
                pstImg->chrom_phy_addr     = pstImg->luma_phy_addr + ChromOffset;
                pstImg->top_chrom_phy_addr = pstImg->top_luma_phy_addr + ChromOffset;
                pstImg->btm_chrom_phy_addr = pstImg->top_chrom_phy_addr + pstImg->image_stride;
                pstImg->chrom_2d_phy_addr  = pstImg->luma_2d_phy_addr + ChromOffset;
            }

            /* FrameStore��fs_image���پ���һһ��Ӧ��ϵ��Ϊ�������д������µ�image�������� */
            memcpy(&pH264Ctx->CurrPic.frame_store->fs_image, &pstLFs->stDispImg, sizeof(IMAGE));
            memcpy(&pH264Ctx->CurrPic.fs_image, &pH264Ctx->CurrPic.frame_store->fs_image, sizeof(IMAGE));

            pH264Ctx->CurrPic.frame_store->logic_fs_id = LogicFsID;
            pH264Ctx->CurrPic.pmv_address_idc = pstLFs->PmvIdc;
            //dprint(PRN_DBG,"=== dec(lfs=%d): 0x%x(id=%d), 2D: 0x%x(id=%d), pmv: 0x%x(id=%d) ===\n", LogicFsID,
            //        pH264Ctx->CurrPic.fs_image.luma_phy_addr, pstLFs->pstDecodeFs->PhyFsID,
            //        pstLFs->pstDispOutFs->PhyAddr, pstLFs->pstDispOutFs->PhyFsID,
            //        pstLFs->PmvAddr, pstLFs->PmvIdc);
        }
        else
        {
            pH264Ctx->CurrPic.state = EMPTY;
            pH264Ctx->CurrPic.frame_store->is_in_dpb = 0;
            dprint(PRN_FATAL, "Can not new logic fs! ClearAll\n");
            ClearAll(pH264Ctx,1);
            return H264_ERR;  //FS_ALLOC_ERR
        }
    }
    else
    {
    }

    pH264Ctx->CurrPic.structure = structure;
    /* ��frame.structureָ���洢�ṹ����Ҫ��֡�����Ծ������. */
    pH264Ctx->CurrPic.frame_store->frame.structure = pH264Ctx->CurrSlice.field_pic_flag? FIELD_PAIR: FRAME;

    if (1 == pH264Ctx->CurrPic.field_pair_flag)
    {
        pH264Ctx->PrevPic.structure = FRAME;
        pH264Ctx->PrevPic.frame_num = pH264Ctx->CurrSlice.frame_num;
        pH264Ctx->PrevPic.nal_ref_idc = pH264Ctx->CurrSlice.nal_ref_idc;
        pH264Ctx->PrevPic.pic_counter = pH264Ctx->PicCounter;
        pH264Ctx->PrevPic.image_id = LogicFsID;
        pH264Ctx->PrevPic.FrameStoreFlag = 0;
    }
    else
    {
        // update prevpic for next field pair judge
        pH264Ctx->PrevPic.structure = structure;
        pH264Ctx->PrevPic.frame_num = pH264Ctx->CurrSlice.frame_num;
        pH264Ctx->PrevPic.nal_ref_idc = pH264Ctx->CurrSlice.nal_ref_idc;
        pH264Ctx->PrevPic.pic_counter = pH264Ctx->PicCounter;
        pH264Ctx->PrevPic.image_id = LogicFsID;
        if(FRAME != structure)
        {
            pH264Ctx->PrevPic.FrameStoreFlag = 1;
        }
        else
        {
            pH264Ctx->PrevPic.FrameStoreFlag = 0;
        }
    }

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 DecGap(VOID)
*      �������� :  ��ʼ��dpb
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
SINT32 DecGap(H264_CTX_S *pH264Ctx)
{
    SINT32 CurrFrameNum = pH264Ctx->CurrSlice.frame_num;
    SINT32 MaxFrameNum = 1 << (pH264Ctx->CurrSPS.log2_max_frame_num_minus4 + 4);
    SINT32 UnusedShortTermFrameNum = (pH264Ctx->CurrSlice.prev_frame_num + 1) % MaxFrameNum;
    SINT32 tmp1 = pH264Ctx->CurrPOC.delta_pic_order_cnt[0];
    SINT32 tmp2 = pH264Ctx->CurrPOC.delta_pic_order_cnt[1];
    SINT32 tmp3 = pH264Ctx->Mark[pH264Ctx->CurrMarkIdx].adaptive_ref_pic_marking_mode_flag;
    SINT32 nal_ref_idc = (pH264Ctx->CurrSPS.gaps_in_frame_num_value_allowed_flag==1)? 2: 0;
    SINT32 ret;

    pH264Ctx->CurrPic.pMarkPara = &(pH264Ctx->Mark[pH264Ctx->CurrMarkIdx]);  //zhl add, to avoid coredump

    /* ���DPBΪ����Ӧ�ý�GAP����ʱ��GAP�Ǽ���. ������˼�GAPʱ���ܷ���H264_ERR����Ϊ�������GAP������
       ֮������InitPic�������ü���, ������OldSLice�������Ϣ����ˢ�»ᵼ�º���slice�ж�֡�߽���� */
    if (pH264Ctx->DPB.used_size == 0)
    {
        dprint(PRN_FATAL,"GAP found while DPB is empty!\n");
        return H264_OK;
    }

    // save poc para of currslice
    pH264Ctx->CurrPOC.delta_pic_order_cnt[0] = pH264Ctx->CurrPOC.delta_pic_order_cnt[1] = 0;

    dprint(PRN_CTRL, "frame num gap try to fill it \n");
    dprint(PRN_CTRL, "CurrFrameNum = %d  UnusedShortTermFrameNum = %d\n", CurrFrameNum, UnusedShortTermFrameNum);

    while (CurrFrameNum > UnusedShortTermFrameNum)
    {
        ret = AllocFrameStore(pH264Ctx, 1);
        if (H264_OK != ret)
        {
            pH264Ctx->CurrPOC.delta_pic_order_cnt[0] = tmp1;
            pH264Ctx->CurrPOC.delta_pic_order_cnt[1] = tmp2;
            pH264Ctx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = tmp3;

            dprint(PRN_FATAL, "cann't allocate frame store when gap find\n");
            return H264_ERR;
        }
        dprint(PRN_CTRL, "allocate frame store when gap find\n");

        /*pic para init*/
        pH264Ctx->CurrPic.nal_unit_type = NALU_TYPE_SLICE;
        pH264Ctx->CurrPic.frame_store->frame_num = UnusedShortTermFrameNum;
        pH264Ctx->CurrPic.frame_num = UnusedShortTermFrameNum;
        pH264Ctx->CurrPic.frame_store->frame.pic_num= UnusedShortTermFrameNum;
        pH264Ctx->CurrPic.nal_ref_idc= nal_ref_idc;
        pH264Ctx->CurrPic.frame_store->frame.long_term_pic_num = 0;
        pH264Ctx->CurrPic.frame_store->is_displayed = 1;
        pH264Ctx->CurrPic.frame_store->is_in_dpb = 0;
        pH264Ctx->CurrPic.non_existing = pH264Ctx->CurrPic.frame_store->non_existing = 1;

        /*CurrMark*/
        pH264Ctx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = 0;

        if (0 != pH264Ctx->CurrSPS.pic_order_cnt_type)
        {
            pH264Ctx->CurrPOC.frame_num = UnusedShortTermFrameNum;
            DecPOC(pH264Ctx);
            pH264Ctx->CurrPic.thispoc = pH264Ctx->CurrPOC.ThisPOC;
            pH264Ctx->CurrPic.framepoc = pH264Ctx->CurrPOC.framepoc;
            pH264Ctx->CurrPic.toppoc = pH264Ctx->CurrPOC.toppoc;
            pH264Ctx->CurrPic.bottompoc = pH264Ctx->CurrPOC.bottompoc;
        }
        pH264Ctx->CurrPic.is_long_term = 0;
        ret = StorePicInDPB(pH264Ctx, NULL);
        if (H264_OK != ret)
        {
            pH264Ctx->CurrPOC.delta_pic_order_cnt[0] = tmp1;
            pH264Ctx->CurrPOC.delta_pic_order_cnt[1] = tmp2;
            pH264Ctx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = tmp3;

            dprint(PRN_FATAL, "line %d: store gap pic err, ret=%d\n", __LINE__, ret);
            return H264_ERR;
        }

        //g_CurrPic.frame_store->is_in_dpb = 0;

        pH264Ctx->CurrSlice.prev_frame_num = UnusedShortTermFrameNum;

        UnusedShortTermFrameNum = (UnusedShortTermFrameNum + 1) % MaxFrameNum;
    }

    pH264Ctx->CurrPOC.delta_pic_order_cnt[0] = tmp1;
    pH264Ctx->CurrPOC.delta_pic_order_cnt[1] = tmp2;
    pH264Ctx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = tmp3;

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 InitPic(H264_CTX_S *pH264Ctx)
*      �������� :  ���ݵ�ǰslice���﷨Ԫ�ؼ��SPS��PPS�ĸ��£�
*                  ��frame_num���д���
*                  ��ʼ����ͼ��Ĳ�����
*      ����˵�� :  ��
*      ����ֵ   �� SPS����ʱ��VHB�ռ�������ʧ�ܵı�־
����֡��ռ䡢pmv�ռ�ʧ�ܵı�־��
*                  gap����ʱ����DPBģ��ķ���ֵ��
*                  gap����ı�־��
************************************************************************
*/
SINT32 InitPic(H264_CTX_S *pH264Ctx)
{
    UINT32 MaxFrameNum;
    SINT32 ret, dar;
    UINT32 ReRangeFlg;
    SINT32 uidxpic;
    UINT32 uidxsei;
    UINT32 ChromaFormat;
    UINT32 dpb_changed;
    UINT32 ImageWidth, ImageHeight;
    FSP_PHY_FS_S *pstDecFs;

    H264_SPS_S *pSPS;
    H264_PPS_S *pPPS;

    pPPS = &pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id];
    pSPS = &pH264Ctx->SPS[pPPS->seq_parameter_set_id];

    if (pH264Ctx->CurrPic.state != EMPTY)
    {
        if(pH264Ctx->CurrPic.frame_store != NULL)
        {
            if (pH264Ctx->CurrPic.frame_store->is_used == 0)
            {
                FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
                pH264Ctx->CurrPic.frame_store->is_used = 0;
                pH264Ctx->CurrPic.frame_store->is_in_dpb = 0;
            }
        }
		else
		{
	         dprint(PRN_DBG, "%s %d frame_store is null!!\n",__FUNCTION__,__LINE__);
		}

        pH264Ctx->CurrPic.state = EMPTY;
    }

    ReRangeFlg = GetReRangeFlag(pH264Ctx, &dpb_changed);

    /*1 refresh SPS and PPS */
    // sps id�仯,spsһ������,ͬid�ŵ�SPS�����и���,��ǰslice�����½���Ļָ���
    if ( (pH264Ctx->CurrSPS.seq_parameter_set_id != pSPS->seq_parameter_set_id) || pSPS->is_refreshed)
    {
        // new sps copy and init dpb
        pSPS->is_refreshed = 0;
        memmove(&(pH264Ctx->CurrSPS), pSPS, sizeof(H264_SPS_S));
    }

    /*1 refresh SPS and PPS */
    // sps id�仯,spsһ������,ͬid�ŵ�SPS�����и���,��ǰslice�����½���Ļָ���
    if ((dpb_changed)&&((pH264Ctx->pstExtraData->stChanOption.s32ReRangeEn == 0)&&(pH264Ctx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION)))
    {
        pH264Ctx->SeqCnt++;

        // SPS����ǣ�浽��֮ǰ����ͼ��Ĵ���
        if (ReRangeFlg)
        {
            ret = FlushDPB(pH264Ctx);
            if (H264_OK != ret)
            {
                dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                return H264_ERR;
            }
        }

        ret = InitDPB(pH264Ctx);
        if (H264_OK != ret)
        {
            dprint(PRN_FATAL, "line %d: init dpb return %d\n", __LINE__, ret);
            return H264_ERR;
        }

        ImageWidth = ((UINT32)pH264Ctx->CurrSPS.pic_width_in_mbs_minus1 + 1) * 16;
        ImageHeight = ((UINT32)pH264Ctx->CurrSPS.pic_height_in_map_units_minus1 + 1) * (2 - pH264Ctx->CurrSPS.frame_mbs_only_flag) * 16;

        if ((ReRangeFlg)&&(pH264Ctx->CurWidthInMb==0))
        {
            // when DPB size is gotten, FrameStore can use is gotten too
            pH264Ctx->TotalPmvNum = pH264Ctx->CurrSPS.dpb_size_plus1;

            if ((ImageWidth > pH264Ctx->pstExtraData->stChanOption.s32MaxWidth)||(ImageHeight > pH264Ctx->pstExtraData->stChanOption.s32MaxHeight))
            {
                dprint(PRN_FATAL, "resolution error, the CAP_LEVEL_USER_DEFINE_WITH_OPTION channel with s32ReRangeEn == 0 can't support such bitstream.\n");
                return H264_ERR;
            }
            if (pH264Ctx->pstExtraData->stChanOption.s32MaxRefFrameNum < pH264Ctx->DPB.size)
            {
                dprint(PRN_FATAL, "s32MaxRefFrameNum < pH264Ctx->DPB.size, the CAP_LEVEL_USER_DEFINE_WITH_OPTION channel can't support such bitstream.\n");
                return H264_ERR;
            }
            if (pH264Ctx->pstExtraData->stChanOption.s32SupportBFrame == 0)
			{
			    pH264Ctx->TotalPmvNum = 1;
			}
            if ( VF_OK != H264_ArrangeVahbMem(pH264Ctx, pH264Ctx->pstExtraData->stChanOption.s32MaxWidth, pH264Ctx->pstExtraData->stChanOption.s32MaxHeight, &pH264Ctx->TotalFsNum, pH264Ctx->TotalPmvNum) )
            {
                if(1 != pH264Ctx->pstExtraData->s32WaitFsFlag)
                {
	                pH264Ctx->CurWidthInMb = 0;
	                pH264Ctx->CurHeightInMb = 0;

	                dprint(PRN_FATAL, "mem arrange err, ClearAll\n");
	                ClearAll(pH264Ctx,0);
	                return H264_ERR;  //VHB_MEM_ERR
                }
                else
                {
                    return H264_NOTDEC;
                }
            }
        }
     
        /* ��¼��ǰ�Ľ����ߣ�Ŀ���Ƿ�������жϿ���Ƿ����仯 */
        pH264Ctx->CurWidthInMb = ImageWidth / 16;
        pH264Ctx->CurHeightInMb = ImageHeight / 16;
    }
    else if ((dpb_changed) || (1 == pH264Ctx->pstExtraData->s32WaitFsFlag))
    {
        // ���Ǹ�STB׼���Ĵ���
        pH264Ctx->SeqCnt++;

        // SPS����ǣ�浽��֮ǰ����ͼ��Ĵ���
        if ((ReRangeFlg) || (1 == pH264Ctx->pstExtraData->s32WaitFsFlag))
        {
            GetBackPicFromVOQueue(pH264Ctx);
        }
        else
        {
            ret = FlushDPB(pH264Ctx);
            if (H264_OK != ret)
            {
                dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                return H264_ERR;
            }
        }

        ret = InitDPB(pH264Ctx);
        if (H264_OK != ret)
        {
            dprint(PRN_FATAL, "line %d: init dpb return %d\n", __LINE__, ret);
            return H264_ERR;
        }

        if ((ReRangeFlg) || (1 == pH264Ctx->pstExtraData->s32WaitFsFlag))
        {
            // when DPB size is gotten, FrameStore can use is gotten too
            pH264Ctx->TotalPmvNum = pH264Ctx->CurrSPS.dpb_size_plus1;

            ImageWidth = ((UINT32)pH264Ctx->CurrSPS.pic_width_in_mbs_minus1 + 1) * 16;
            ImageHeight = ((UINT32)pH264Ctx->CurrSPS.pic_height_in_map_units_minus1 + 1) * (2 - pH264Ctx->CurrSPS.frame_mbs_only_flag) * 16;

            if ( VF_OK != H264_ArrangeVahbMem(pH264Ctx, ImageWidth, ImageHeight, &pH264Ctx->TotalFsNum, pH264Ctx->TotalPmvNum) )
            {
                if(pH264Ctx->pstExtraData->s32WaitFsFlag != 1)
                {
	                pH264Ctx->CurWidthInMb = 0;
	                pH264Ctx->CurHeightInMb = 0;

	                dprint(PRN_FATAL, "mem arrange err, ClearAll\n");
	                ClearAll(pH264Ctx,0);
	                return H264_ERR;  //VHB_MEM_ERR
                }
                else
                {
                    return H264_NOTDEC;
                }
            }
            else
            {
                /* ��¼��ǰ�Ľ����ߣ�Ŀ���Ƿ�������жϿ���Ƿ����仯 */
                pH264Ctx->CurWidthInMb = ImageWidth / 16;
                pH264Ctx->CurHeightInMb = ImageHeight / 16;
            }
        }
    }

    if ( (pH264Ctx->CurrPPS.pic_parameter_set_id != pPPS->pic_parameter_set_id) || pPPS->is_refreshed )
    {
        pPPS->is_refreshed = 0;
        memmove(&(pH264Ctx->CurrPPS), pPPS, sizeof(H264_PPS_S));
    }

    /*2 gap process*/
    MaxFrameNum = 1 << (pH264Ctx->CurrSPS.log2_max_frame_num_minus4 + 4);
    if (pH264Ctx->CurrPOC.last_has_mmco_5)
    {
        pH264Ctx->CurrSlice.prev_frame_num = 0;
        pH264Ctx->PrevPic.frame_num = 0;
    }

    /* ���������DEC_ORDER_SIMPLE_DPBģʽ��������gap. z56361, 20110729 */
    /* ���������I֡ģʽ��������gap. z56361, 20110830 */
    if ((pH264Ctx->OldDecMode != I_MODE) &&
        (pH264Ctx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB) &&
        (pH264Ctx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SKYPLAY) &&
        (pH264Ctx->CurrSlice.frame_num != pH264Ctx->CurrSlice.prev_frame_num) &&
        (pH264Ctx->CurrSlice.frame_num != ((pH264Ctx->CurrSlice.prev_frame_num + 1) % MaxFrameNum)))
    {
        if ((pH264Ctx->pstExtraData->s32MoreGapEnable == 0) 
        &&(!((pH264Ctx->pstExtraData->stPvrInfo.u32BFrmRefFlag==1)
		     &&(pH264Ctx->pstExtraData->stPvrInfo.u32IDRFlag==0)
		     &&(pH264Ctx->pstExtraData->stPvrInfo.u32ContinuousFlag==0)
		     &&(!((pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed >= -1024)
		         &&(pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed <= 1024))))))
        {
            if (pH264Ctx->CurrSlice.frame_num > pH264Ctx->CurrSlice.prev_frame_num)
            {
                if (pH264Ctx->CurrSPS.dpb_size_plus1 < (pH264Ctx->CurrSlice.frame_num - pH264Ctx->CurrSlice.prev_frame_num))
                {
                    if (I_SLICE != pH264Ctx->CurrSlice.slice_type)
                    {
                        H264_ClearCurrPic(pH264Ctx);
                        if (0 == g_apple_airplay_mirrorinig_flag)
                        {
                            ClearDPB(pH264Ctx);
                        }
                        dprint(PRN_FATAL, "line %d: frame gap(=%d) > dpb size(=%d)\n", __LINE__, (pH264Ctx->CurrSlice.frame_num - pH264Ctx->CurrSlice.prev_frame_num),
                            pH264Ctx->CurrSPS.dpb_size_plus1);
                        return H264_ERR;  //IDR_LOST
                    }
                    else
                    {
                        pH264Ctx->CurrSlice.prev_frame_num = (0 ==pH264Ctx->CurrSlice.frame_num) ?
                            MaxFrameNum - 1 : pH264Ctx->CurrSlice.frame_num - 1;
                        ret = FlushDPB(pH264Ctx);
                        if (H264_OK != ret)
                        {
                            dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                            return H264_ERR;
                        }
                    }
                }
            }
            else
            {
                if (pH264Ctx->CurrSPS.dpb_size_plus1 < (pH264Ctx->CurrSlice.frame_num +  MaxFrameNum - pH264Ctx->CurrSlice.prev_frame_num))
                {
                    if (I_SLICE != pH264Ctx->CurrSlice.slice_type)
                    {
                        H264_ClearCurrPic(pH264Ctx);
                        if (0 == g_apple_airplay_mirrorinig_flag)
                        {
                            ClearDPB(pH264Ctx);
                        }
                        dprint(PRN_FATAL, "line %d: frame gap(=%d) > dpb size(=%d)\n", __LINE__, (pH264Ctx->CurrSlice.frame_num - pH264Ctx->CurrSlice.prev_frame_num),
                            pH264Ctx->CurrSPS.dpb_size_plus1);
                        return H264_ERR;  //IDR_LOST
                    }
                    else
                    {
                        pH264Ctx->CurrSlice.prev_frame_num = (0 ==pH264Ctx->CurrSlice.frame_num)?
                            MaxFrameNum - 1 : pH264Ctx->CurrSlice.frame_num - 1;
                        ret = FlushDPB(pH264Ctx);
                        if (H264_OK != ret)
                        {
                            dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                            return H264_ERR;
                        }
                    }
                }
            }
        }
        if (0 == pH264Ctx->CurrSPS.gaps_in_frame_num_value_allowed_flag)
        {
            dprint(PRN_CTRL, "frame num gap don't allowed but gap find\n");
            if ((pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed == 0)||(pH264Ctx->pstExtraData->s32FastForwardBackwardSpeed == 1024))
            {
                if (pH264Ctx->CurrSlice.frame_num < ((pH264Ctx->CurrSlice.prev_frame_num + 1) % MaxFrameNum))
                {
                    // IDR lost return and find next recoverpoint
                    if (I_SLICE != pH264Ctx->CurrSlice.slice_type)
                    {
                        H264_ClearCurrPic(pH264Ctx);
                        if (0 == g_apple_airplay_mirrorinig_flag)
                        {
                            ClearDPB(pH264Ctx);
                        }
                        dprint(PRN_FATAL,"line %d: find gap in NON-I slice but here gap is not allowed\n", __LINE__);
                        return H264_ERR;  //IDR_LOST
                    }
                }
                else
                {
                    ret = DecGap(pH264Ctx);
                    if (H264_OK != ret)
                    {
                        dprint(PRN_FATAL,"line %d: dec gap failed\n", __LINE__);
                        return H264_ERR;
                    }
                }
            }
        }
        else
        {
            ret = DecGap(pH264Ctx);
            if (H264_OK != ret)
            {
                dprint(PRN_FATAL,"line %d: dec gap failed\n", __LINE__);
                return H264_ERR;
            }
        }
    }

    if (pH264Ctx->CurrSlice.nal_ref_idc)
    {
        pH264Ctx->CurrSlice.prev_frame_num = pH264Ctx->CurrSlice.frame_num;
    }

    /*3 calculate poc of g_CurrPic*/
    pH264Ctx->CurrPOC.frame_num = pH264Ctx->CurrSlice.frame_num;
    DecPOC(pH264Ctx);

    /*4 ����֡���PMV�ռ�*/
    ret = AllocFrameStore(pH264Ctx, 0);
    if (H264_OK != ret)
    {
        dprint(PRN_FATAL,"line %d: alloc framestore failed\n", __LINE__);
        return H264_ERR;
    }

    /*5 ��ʼ����ǰ����ͼ��Ĳ���*/
    // ���Ը��ݳɶԳ��������ظ���ֵ
    // pmv_address_idc,structure, *frame_store����֡����亯���в���
    pH264Ctx->CurrPic.mode_skip_flag =0;
    pH264Ctx->CurrPic.state = PIC_DECODING;
    pH264Ctx->CurrPic.nal_unit_type = pH264Ctx->CurrSlice.nal_unit_type;
    pH264Ctx->CurrPic.frame_num = pH264Ctx->CurrSlice.frame_num;
    pH264Ctx->CurrPic.nal_ref_idc = pH264Ctx->CurrSlice.nal_ref_idc;
    pH264Ctx->CurrPic.thispoc = pH264Ctx->CurrPOC.ThisPOC;
    pH264Ctx->CurrPic.framepoc = pH264Ctx->CurrPOC.framepoc;
    pH264Ctx->CurrPic.toppoc = pH264Ctx->CurrPOC.toppoc;
    pH264Ctx->CurrPic.bottompoc = pH264Ctx->CurrPOC.bottompoc;
    //dprint(PRN_DBG,"thispoc = %d, f_poc = %d, top_poc = %d, b_poc = %d\n", g_CurrPic.thispoc,g_CurrPic.framepoc,g_CurrPic.toppoc,g_CurrPic.bottompoc);
    pH264Ctx->CurrPic.non_existing = 0;
    pH264Ctx->CurrPic.is_long_term = pH264Ctx->CurrPic.is_short_term = 0;
    /*_LOG("g_CurrPic.thispoc = %d, g_CurrPic.frampoc = %d, g_CurrPic.toppoc= %d, g_CurrPic.bottompoc = %d\n",\
    g_CurrPic.thispoc,g_CurrPic.framepoc, g_CurrPic.toppoc, g_CurrPic.bottompoc);*/
    pH264Ctx->CurrPic.stream_base_addr = 0xffffffff;

    if (pH264Ctx->CurrSlice.field_pic_flag)
    {
        pH264Ctx->CurrSlice.field_pic_flag = 1;//just avoid 1+pH264Ctx->CurrSlice.field_pic_flag == 0
    }
    pH264Ctx->CurrPic.pic_height_in_mb = ((pH264Ctx->CurrSPS.pic_height_in_map_units_minus1+1)*(2 - pH264Ctx->CurrSPS.frame_mbs_only_flag)) / (1+pH264Ctx->CurrSlice.field_pic_flag);
    pH264Ctx->CurrPic.frame_height_in_mb = ((pH264Ctx->CurrSPS.pic_height_in_map_units_minus1+1)*(2 - pH264Ctx->CurrSPS.frame_mbs_only_flag));
    pH264Ctx->CurrPic.pic_width_in_mb = (pH264Ctx->CurrSPS.pic_width_in_mbs_minus1 + 1);
    pH264Ctx->CurrPic.pic_mbs = pH264Ctx->CurrPic.pic_width_in_mb*pH264Ctx->CurrPic.pic_height_in_mb;
    pH264Ctx->CurrPic.entropy_coding_mode_flag = pH264Ctx->CurrPPS.entropy_coding_mode_flag;
    pH264Ctx->CurrPic.MbaffFrameFlag = ((!pH264Ctx->CurrSlice.field_pic_flag) && (pH264Ctx->CurrSPS.mb_adaptive_frame_field_flag));
    pH264Ctx->DecPicPara.MbaffFrameFlag = pH264Ctx->CurrPic.MbaffFrameFlag;
    if (pH264Ctx->CurrSlice.nal_ref_idc)
    {
        pH264Ctx->CurrSlice.prev_frame_num = pH264Ctx->CurrSlice.frame_num;
    }

    // add mark para
    pH264Ctx->CurrPic.pMarkPara = &(pH264Ctx->Mark[pH264Ctx->CurrMarkIdx]);

    //!!! PTS�Ĵ���������ô�򵥡���ʱ���
    pH264Ctx->CurrPic.fs_image.PTS = pH264Ctx->pstExtraData->pts;
    pH264Ctx->CurrPic.fs_image.Usertag = pH264Ctx->pstExtraData->Usertag;
    pH264Ctx->CurrPic.fs_image.DispTime = pH264Ctx->pstExtraData->DispTime;
    pH264Ctx->CurrPic.fs_image.DispEnableFlag = pH264Ctx->pstExtraData->DispEnableFlag;
    pH264Ctx->CurrPic.fs_image.DispFrameDistance = pH264Ctx->pstExtraData->DispFrameDistance;
    pH264Ctx->CurrPic.fs_image.DistanceBeforeFirstFrame = pH264Ctx->pstExtraData->DistanceBeforeFirstFrame;
    pH264Ctx->CurrPic.fs_image.GopNum = pH264Ctx->pstExtraData->GopNum;
    pH264Ctx->CurrPic.fs_image.bit_depth_luma = pSPS->bit_depth_luma;
    pH264Ctx->CurrPic.fs_image.bit_depth_chroma = pSPS->bit_depth_chroma;

    pH264Ctx->pstExtraData->pts = (UINT64)(-1);
    dprint(PRN_PTS,"dec_pts: %lld\n",pH264Ctx->CurrPic.fs_image.PTS);
    dprint(PRN_PTS,"dec_usertag: %lld\n",pH264Ctx->CurrPic.fs_image.Usertag);
    pH264Ctx->CurrPic.fs_image.image_width = (pH264Ctx->CurrSPS.pic_width_in_mbs_minus1 + 1) << 4;
    pH264Ctx->CurrPic.fs_image.image_height = ((pH264Ctx->CurrSPS.pic_height_in_map_units_minus1+1)*(2 - pH264Ctx->CurrSPS.frame_mbs_only_flag))<<4;
    /*
    ȱʡֵ format =
    0x0  << 14  |
    0x2  << 12  |
    0x0  << 10  |
    0x2  << 8   |
    0x5  << 5   |
    0x0  << 2   |
    0x0;
    */
    dar = (pH264Ctx->CurrSPS.vui_seq_parameters.aspect_ratio & 0x7);
    pH264Ctx->CurrPic.fs_image.format = 0x22A0;
    ChromaFormat = (pH264Ctx->CurrSPS.chroma_format_idc == 1) ? 0 : 1;  // 4:2:0, 0;  4:0:0, 1;
    pH264Ctx->CurrPic.fs_image.format = (dar<< 14) |
        ((pH264Ctx->CurrSPS.vui_seq_parameters.video_format & 0x7) << 5) |
        ((ChromaFormat & 0x7 ) << 2);

	/* set display AspectWidth and AspectHeight */
	SetAspectRatio(&(pH264Ctx->CurrPic.fs_image), (VDEC_DAR_E)dar);

    pH264Ctx->CurrPic.fs_image.seq_cnt = pH264Ctx->SeqCnt;
    pH264Ctx->CurrPic.fs_image.is_fld_save = (0 != pH264Ctx->CurrPic.structure);

	if(NULL == pH264Ctx->CurrPic.frame_store)
	{
		dprint(PRN_DBG, "%s %d NULL == pH264Ctx->CurrPic.frame_store!!\n",__FUNCTION__,__LINE__);
		return H264_ERR;
	}

    pH264Ctx->CurrPic.frame_store->fs_image.format = pH264Ctx->CurrPic.fs_image.format;
    pH264Ctx->CurrPic.frame_store->fs_image.is_fld_save = pH264Ctx->CurrPic.fs_image.is_fld_save;
    pH264Ctx->CurrPic.frame_store->fs_image.u32AspectHeight = pH264Ctx->CurrPic.fs_image.u32AspectHeight;
    pH264Ctx->CurrPic.frame_store->fs_image.u32AspectWidth  = pH264Ctx->CurrPic.fs_image.u32AspectWidth;
	pH264Ctx->CurrPic.frame_store->fs_image.image_width     = pH264Ctx->CurrPic.fs_image.image_width;
	pH264Ctx->CurrPic.frame_store->fs_image.image_height    = pH264Ctx->CurrPic.fs_image.image_height;
    pH264Ctx->CurrPic.fs_image.disp_center_x = pH264Ctx->CurrPic.frame_store->fs_image.disp_center_x = pSPS->disp_center_x;
    pH264Ctx->CurrPic.fs_image.disp_center_y = pH264Ctx->CurrPic.frame_store->fs_image.disp_center_y = pSPS->disp_center_y;
    pH264Ctx->CurrPic.fs_image.disp_width    = pH264Ctx->CurrPic.frame_store->fs_image.disp_width    = pSPS->disp_width;
    pH264Ctx->CurrPic.fs_image.disp_height   = pH264Ctx->CurrPic.frame_store->fs_image.disp_height   = pSPS->disp_height;
    pH264Ctx->CurrPic.fs_image.bit_depth_luma   = pH264Ctx->CurrPic.fs_image.bit_depth_luma;
    pH264Ctx->CurrPic.fs_image.bit_depth_chroma = pH264Ctx->CurrPic.fs_image.bit_depth_chroma;
    pstDecFs = FSP_GetDispPhyFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id);
    if(NULL != pstDecFs)
    {
        FSP_PHY_FS_STORE_TYPE_E eStoreType;
        eStoreType = (pH264Ctx->CurrPic.fs_image.is_fld_save != 0)? FSP_PHY_FS_STORE_TYPE_FIELD : FSP_PHY_FS_STORE_TYPE_FRAME;
        FSP_SetStoreType(pH264Ctx->ChanID, FSP_PHY_FS_TYPE_DECODE, pstDecFs->PhyFsID, eStoreType);
    }

    for (uidxpic=0; uidxpic<4; uidxpic++)
    {
        if (NULL == pH264Ctx->CurrPic.fs_image.p_usrdat[uidxpic] )
        {
            //add usrdata
            if (0 < pH264Ctx->TotalUsrDatNum)
            {
                pH264Ctx->CurrPic.fs_image.p_usrdat[uidxpic] = pH264Ctx->pUsrDatArray[0];
                for (uidxsei=0; uidxsei<pH264Ctx->TotalUsrDatNum-1; uidxsei++)
                {
                    pH264Ctx->pUsrDatArray[uidxsei] = pH264Ctx->pUsrDatArray[uidxsei+1];
                }
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum-1] = NULL;
                pH264Ctx->TotalUsrDatNum--;
            }
        }
    }
    if (0 < pH264Ctx->TotalUsrDatNum)
    {
        for (uidxpic = pH264Ctx->TotalUsrDatNum; uidxpic > 0; uidxpic--)
        {
            FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[uidxpic - 1]);
            pH264Ctx->pUsrDatArray[uidxpic - 1]= NULL;
        }
    }
    pH264Ctx->TotalUsrDatNum = 0;
    pH264Ctx->CurrPic.pic_type = 0;  // Ĭ��Islice����ÿ��slice����ʱ����

    CalcPicNum(pH264Ctx);

    return H264_OK;
}


/*!
************************************************************************
* ����ԭ�� :  SINT32 WritePicMsg(H264_CTX_S *pH264Ctx)
* �������� :  д��Ϣ��pic para
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 WritePicMsg(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    FSP_LOGIC_FS_S *pstLogicFs;

    dprint(PRN_CTRL,"nal_ref_idc=%d, structure=%d, image_id=%d, pmv_idc=%d\n",
        pH264Ctx->CurrPic.nal_ref_idc , pH264Ctx->CurrPic.structure, pH264Ctx->CurrPic.fs_image.image_id,
        pH264Ctx->CurrPic.pmv_address_idc);

    pH264Ctx->SliceParaNum = 0;
    pH264Ctx->DecPicPara.Pts = pH264Ctx->CurrPic.fs_image.PTS;
    pH264Ctx->DecPicPara.pFirstSlice = &(pH264Ctx->DecSlicePara[0]);

    pH264Ctx->DecPicPara.imgstride = pH264Ctx->CurrPic.fs_image.image_stride;
    pH264Ctx->DecPicPara.nal_ref_idc = pH264Ctx->CurrPic.nal_ref_idc;
    pH264Ctx->DecPicPara.pic_structure = pH264Ctx->CurrPic.structure;
    pH264Ctx->DecPicPara.mb_adaptive_frame_field_flag = pH264Ctx->CurrSPS.mb_adaptive_frame_field_flag;
    pH264Ctx->DecPicPara.pic_height_in_mb = pH264Ctx->CurrPic.pic_height_in_mb;
    pH264Ctx->DecPicPara.frame_height_in_mb = pH264Ctx->CurrPic.frame_height_in_mb;
    pH264Ctx->DecPicPara.pic_width_in_mb = pH264Ctx->CurrSPS.pic_width_in_mbs_minus1+1;
    pH264Ctx->DecPicPara.frame_mbs_only_flag = pH264Ctx->CurrSPS.frame_mbs_only_flag;

    pH264Ctx->DecPicPara.transform_8x8_mode_flag = pH264Ctx->CurrPPS.transform_8x8_mode_flag;
    pH264Ctx->DecPicPara.entropy_coding_mode_flag = pH264Ctx->CurrPPS.entropy_coding_mode_flag;
    pH264Ctx->DecPicPara.chroma_format_idc = pH264Ctx->CurrSPS.chroma_format_idc;
    pH264Ctx->DecPicPara.constrained_intra_pred_flag = pH264Ctx->CurrPPS.constrained_intra_pred_flag;

    pH264Ctx->DecPicPara.ThisPoc = pH264Ctx->CurrPOC.ThisPOC;
    pH264Ctx->DecPicPara.TopPoc =  pH264Ctx->CurrPOC.toppoc;
    pH264Ctx->DecPicPara.BottomPoc = pH264Ctx->CurrPOC.bottompoc;

    /* ֡���Լ�PMV��ַ��/id��Ҫ��FSP�ṩ�Ļ���ȥ�� */
    pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id);
    if (pstLogicFs == NULL)
    {
        dprint(PRN_FATAL,"logic frame id(=%d): get LogicFs error!\n", pH264Ctx->CurrPic.frame_store->logic_fs_id);
        return H264_ERR;
    }
    else if (pstLogicFs->pstDecodeFs == NULL || pstLogicFs->pstDispOutFs == NULL)
    {
        dprint(PRN_FATAL,"phy fs is null: pstDecodeFs = %p, pstDispOutFs = %p\n",
                   pstLogicFs->pstDecodeFs, pstLogicFs->pstDispOutFs);
        return H264_ERR;
    }

    /* ֡���ַ���Լ���ǰ֡֡��ID */
    FSP_GetDecFsAddrTab(pH264Ctx->ChanID,
                &pH264Ctx->DecPicPara.ImgSlotNum, (SINT32*)pH264Ctx->DecPicPara.ImgPhyAddr,(SINT32*)pH264Ctx->DecPicPara.LineNumPhyAddr);
    pH264Ctx->DecPicPara.CurrPicImgIdc = pstLogicFs->pstDecodeFs->PhyFsID;

    /* PMV��ַ���Լ���ǰ֡PMV ID */
    FSP_GetPmvAddrTab(pH264Ctx->ChanID,
        &pH264Ctx->DecPicPara.PmvSlotNum, (SINT32*)pH264Ctx->DecPicPara.PmvPhyAddr);
    pH264Ctx->DecPicPara.CurrPicPmvIdc = pH264Ctx->CurrPic.pmv_address_idc;
    pH264Ctx->DecPicPara.half_pmvblk_offset = pstLogicFs->HalfPmvOffset;

    /* ��ǰ֡2D��ַ */
    pH264Ctx->DecPicPara.CurrPicImg2DAddr = pstLogicFs->pstDispOutFs->PhyAddr;
    pH264Ctx->DecPicPara.ChromaOffset     = pstLogicFs->pstDispOutFs->ChromOffset;

    pH264Ctx->DecPicPara.ApcSize = pH264Ctx->APC.size;
    for (i = 0; i < pH264Ctx->APC.size; i++)
    {
        pH264Ctx->DecPicPara.Apc2Idc[i]    = pH264Ctx->APC.idc[i];
        pH264Ctx->DecPicPara.Apc2Poc[0][i] = pH264Ctx->APC.poc[0][i];
        pH264Ctx->DecPicPara.Apc2Poc[1][i] = pH264Ctx->APC.poc[1][i];
    }

#ifndef Q_MATRIX_FIXED  // VDH V300R001(3798MV100)��ʼH264����������΢����Ϊ�˼��ݾɰ汾����������˺���ƣ�����Ӧ�ý������hal�㸳ֵ  y00226912 20140514
    if ((0 == pH264Ctx->CurrSPS.seq_scaling_matrix_present_flag) && (0 == pH264Ctx->CurrPPS.pic_scaling_matrix_present_flag))
    {
        for (i = 0; i < 24; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i] = quant_org[i%4];
        }

        for (i = 0; i < 32; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i + 24] = quant8_org[i%16];
        }
    }
    else if (1 == pH264Ctx->CurrPPS.pic_scaling_matrix_present_flag)
    {
        for (i = 0; i < 24; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i] = pH264Ctx->CurrPPS.qmatrix4x4[i/4][i%4];
        }

        for (i = 0; i < 32; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i + 24] = pH264Ctx->CurrPPS.qmatrix8x8[i /16][i%16];
        }
    }
    else
    {
        for (i = 0; i < 24; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i] = pH264Ctx->CurrSPS.qmatrix4x4[i/4][i%4];
        }

        for (i = 0; i < 32; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i + 24] = pH264Ctx->CurrSPS.qmatrix8x8[i /16][i%16];
        }
    }
#else
    if ((0 == pH264Ctx->CurrSPS.seq_scaling_matrix_present_flag) && (0 == pH264Ctx->CurrPPS.pic_scaling_matrix_present_flag))
    {
        for (i = 0; i < 24; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i] = (((quant_org[i%4]>>24) &0xff) <<24) | \
                                              (((quant_org[i%4]>>8)  &0xff) <<16) | \
                                              (((quant_org[i%4]>>16) &0xff) <<8)  | \
                                               ((quant_org[i%4])     &0xff);
        }

        for (i = 0; i < 16; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[2*i+24] = (((quant8_org[(2*i+1)%16]>>16) &0xff) <<24) | \
                                                     (((quant8_org[(2*i+1)%16])     &0xff) <<16) | \
                                                     (((quant8_org[(2*i)%16]>>16)   &0xff) <<8)  | \
                                                      ((quant8_org[(2*i)%16])       &0xff);

            pH264Ctx->DecPicPara.Qmatrix[(2*i+1)+24] = (((quant8_org[(2*i+1)%16]>>24) &0xff) <<24) | \
                                                         (((quant8_org[(2*i+1)%16])>>8  &0xff) <<16) | \
                                                         (((quant8_org[(2*i)%16]>>24)   &0xff) <<8)  | \
                                                          ((quant8_org[(2*i)%16])>>8    &0xff);
        }
    }
    else if (1 == pH264Ctx->CurrPPS.pic_scaling_matrix_present_flag)
    {
        for (i = 0; i < 24; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i] = (((pH264Ctx->CurrPPS.qmatrix4x4[i/4][i%4]>>24) &0xff) <<24) | \
                                              (((pH264Ctx->CurrPPS.qmatrix4x4[i/4][i%4]>>8)  &0xff) <<16) | \
                                              (((pH264Ctx->CurrPPS.qmatrix4x4[i/4][i%4]>>16) &0xff) <<8)  | \
                                               ((pH264Ctx->CurrPPS.qmatrix4x4[i/4][i%4])     &0xff);
        }

        for (i = 0; i < 16; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[2*i+24] = (((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16]>>16) &0xff) <<24) | \
                                                     (((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16])     &0xff) <<16) | \
                                                     (((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i)%16]>>16)   &0xff) <<8)  | \
                                                      ((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i)%16])       &0xff);

            pH264Ctx->DecPicPara.Qmatrix[(2*i+1)+24] = (((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16]>>24) &0xff) <<24) | \
                                                         (((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16]) >>8 &0xff) <<16) | \
                                                         (((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i)%16]>>24)   &0xff) <<8)  | \
                                                          ((pH264Ctx->CurrPPS.qmatrix8x8[i/8][(2*i)%16]) >>8   &0xff);
        }
     }
    else
    {
        for (i = 0; i < 24; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[i] = (((pH264Ctx->CurrSPS.qmatrix4x4[i/4][i%4]>>24) &0xff) <<24) | \
                                              (((pH264Ctx->CurrSPS.qmatrix4x4[i/4][i%4]>>8)  &0xff) <<16) | \
                                              (((pH264Ctx->CurrSPS.qmatrix4x4[i/4][i%4]>>16) &0xff) <<8)  | \
                                               ((pH264Ctx->CurrSPS.qmatrix4x4[i/4][i%4])     &0xff);
        }

        for (i = 0; i < 16; i++)
        {
            pH264Ctx->DecPicPara.Qmatrix[2*i+24] = (((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16]>>16) &0xff) <<24) | \
                                                     (((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16])     &0xff) <<16) | \
                                                     (((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i)%16]>>16)   &0xff) <<8)  | \
                                                      ((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i)%16])       &0xff);

            pH264Ctx->DecPicPara.Qmatrix[(2*i+1)+24] = (((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16]>>24) &0xff)<<24) | \
                                                         (((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16]) >>8 &0xff)<<16) | \
                                                         (((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i)%16]>>24)   &0xff)<<8)  | \
                                                          ((pH264Ctx->CurrSPS.qmatrix8x8[i/8][(2*i)%16]) >>8   &0xff);
        }
	}
#endif

    for (i = 0; i < pH264Ctx->APC.size; i++)
    {
        FSP_PHY_FS_STORE_TYPE_E  eStoreType;

        eStoreType = FSP_GetStoreType(pH264Ctx->ChanID, FSP_PHY_FS_TYPE_DECODE, pH264Ctx->APC.idc[i]);
        pH264Ctx->DecPicPara.RefPicType[i] = (eStoreType == FSP_PHY_FS_STORE_TYPE_FRAME)? 0 : 1;
        pH264Ctx->DecPicPara.RefPicFsId[i] = pH264Ctx->APC.idc[i];
    }

    return H264_OK;
}


VOID UpdatePicQpInf(SINT32 SlcQp, H264_STORABLEPIC_S *pCurPic)
{
    pCurPic->MaxQp = (pCurPic->MaxQp < SlcQp) ? SlcQp : pCurPic->MaxQp;
    pCurPic->MinQp = (pCurPic->MinQp > SlcQp) ? SlcQp : pCurPic->MinQp;

    return;
}


/*!
************************************************************************
* ����ԭ�� : VOID WriteSliceMsg(H264_CTX_S *pH264Ctx)
* �������� : д��Ϣ��slice para
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
VOID WriteSliceMsg(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    UINT32 ExistImgIdc_0, ExistImgIdc_1;
    FSP_LOGIC_FS_S *pstLogicFs;
    H264_DEC_SLICE_PARAM_S *pCurrSlicePara;
    UINT32 CurrSliceSlotNum = pH264Ctx->SliceParaNum;

    pH264Ctx->SliceParaNum++;

    pCurrSlicePara = &(pH264Ctx->DecSlicePara[CurrSliceSlotNum]);
    if (CurrSliceSlotNum < pH264Ctx->MaxSliceNum-1)
    {
        pCurrSlicePara->pNextSlice = &(pH264Ctx->DecSlicePara[CurrSliceSlotNum+1]);
    }
    else
    {
        pCurrSlicePara->pNextSlice = 0;
    }

    for (i = pH264Ctx->pCurrNal->nal_used_segment; ((i < pH264Ctx->pCurrNal->nal_segment) && (i < 2)) ; i++)
    {
        pCurrSlicePara->bitsoffset[i] = pH264Ctx->pCurrNal->stream[i].bitsoffset;
        pCurrSlicePara->valid_bitlen[i] = pH264Ctx->pCurrNal->stream[i].valid_bitlen;
        pCurrSlicePara->bytespos[i] = pH264Ctx->pCurrNal->stream[i].bytespos;
    }
    for (i = pH264Ctx->pCurrNal->nal_segment; i < 2; i++)
    {
        pCurrSlicePara->bitsoffset[i] = 0;
        pCurrSlicePara->valid_bitlen[i] = 0;
        pCurrSlicePara->bytespos[i] = 0;
    }
    pCurrSlicePara->MbaffFrameFlag = pH264Ctx->CurrPic.MbaffFrameFlag;

    pCurrSlicePara->SliceQPy =  (pH264Ctx->CurrPPS.pic_init_qp_minus26 + 26 + pH264Ctx->CurrSlice.slice_qp_delta); // % 52;
    if (0 == CurrSliceSlotNum)
    {
        // if it's the first slice of cur pic, do init
        pH264Ctx->CurrPic.frame_store->frame.MaxQp = pCurrSlicePara->SliceQPy;
        pH264Ctx->CurrPic.frame_store->frame.MinQp = pCurrSlicePara->SliceQPy;
    }
    else
    {
        // otherwise, update
        UpdatePicQpInf(pCurrSlicePara->SliceQPy, &pH264Ctx->CurrPic.frame_store->frame);
    }
    pCurrSlicePara->cabac_init_idc = pH264Ctx->CurrSlice.cabac_init_idc;
    pCurrSlicePara->num_ref_idx_l1_active_minus1 = pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1;
    pCurrSlicePara->num_ref_idx_l0_active_minus1 = pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1;
    pCurrSlicePara->slice_type = pH264Ctx->CurrSlice.slice_type;
    pCurrSlicePara->first_mb_in_slice = pH264Ctx->CurrSlice.first_mb_in_slice;

    pCurrSlicePara->direct_spatial_mv_pred_flag = pH264Ctx->CurrSlice.direct_spatial_mv_pred_flag;
    pCurrSlicePara->direct_8x8_inference_flag = pH264Ctx->CurrSPS.direct_8x8_inference_flag;
    pCurrSlicePara->listXsize[0] = pH264Ctx->CurrSlice.listXsize[0];
    pCurrSlicePara->listXsize[1] = pH264Ctx->CurrSlice.listXsize[1];

    if (B_SLICE == pH264Ctx->CurrSlice.slice_type) // B Slice
    {
        pCurrSlicePara->weight_flag = pH264Ctx->CurrPPS.weighted_bipred_idc;
    }
    else if (P_SLICE == pH264Ctx->CurrSlice.slice_type) // P Slice
    {
        pCurrSlicePara->weight_flag = pH264Ctx->CurrPPS.weighted_pred_flag;
    }
    else
    {
        pCurrSlicePara->weight_flag = 0;
    }
    pCurrSlicePara->chroma_qp_index_offset = pH264Ctx->CurrPPS.chroma_qp_index_offset;
    pCurrSlicePara->second_chroma_qp_index_offset = pH264Ctx->CurrPPS.second_chroma_qp_index_offset;
    pCurrSlicePara->slice_alpha_c0_offset_div2 = pH264Ctx->CurrSlice.slice_alpha_c0_offset_div2;
    pCurrSlicePara->slice_beta_offset_div2 = pH264Ctx->CurrSlice.slice_beta_offset_div2;
    pCurrSlicePara->disable_deblocking_filter_idc = pH264Ctx->CurrSlice.disable_deblocking_filter_idc;

    // �ο��б� & poc
    if (I_SLICE != pH264Ctx->CurrSlice.slice_type) // P,B Slice
    {
        /* Ϊ����GAP֡��ַ���ο��� */
        ExistImgIdc_0 = 0;
        ExistImgIdc_1 = 0;
        for (i=0; i<pH264Ctx->CurrSlice.listXsize[0]; i++)
        {
            if (pH264Ctx->pListX[0][i]->frame_store->non_existing == 0)
            {
                pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[0][i]->frame_store->logic_fs_id);
                if (NULL == pstLogicFs)
                {
                    dprint(PRN_FATAL,"line: %d pstLogicFs is null\n", __LINE__);
                    return;
                }
                ExistImgIdc_0 = pstLogicFs->pstDecodeFs->PhyFsID;
                break;
            }
            else
            {
                //dprint(PRN_DBG,".");
            }
        }
        for (i=0; i<pH264Ctx->CurrSlice.listXsize[1]; i++)
        {
            if (pH264Ctx->pListX[1][i]->frame_store->non_existing == 0)
            {
                pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[1][i]->frame_store->logic_fs_id);
                if (NULL == pstLogicFs)
                {
                    dprint(PRN_FATAL,"line: %d pstLogicFs is null\n", __LINE__);
                    return;
                }
                ExistImgIdc_1 = pstLogicFs->pstDecodeFs->PhyFsID;
                break;
            }
            else
            {
                //dprint(PRN_DBG,"x");
            }
        }

        /* ��֡�������Թ���ο��б� */
        if (0==pH264Ctx->CurrPic.structure) // frame reference
        {
            for (i=0; i<pH264Ctx->CurrSlice.listXsize[0]; i++)
            {
                pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[0][i]->frame_store->logic_fs_id);
                pCurrSlicePara->declist[0][i].pmv_address_idc = pH264Ctx->pListX[0][i]->frame_store->pmv_address_idc;
                pCurrSlicePara->declist[0][i].is_used = pH264Ctx->pListX[0][i]->frame_store->is_used;
                //pCurrSlicePara->declist[0][i].img_idc = pH264Ctx->pListX[0][i]->frame_store->fs_image.image_id;
                pCurrSlicePara->declist[0][i].img_idc = (pH264Ctx->pListX[0][i]->frame_store->non_existing == 1)? ExistImgIdc_0:
                    (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;
                pCurrSlicePara->declist[0][i].apc_idc = pH264Ctx->pListX[0][i]->frame_store->apc_idc;
                pCurrSlicePara->declist[0][i].poc = pH264Ctx->pListX[0][i]->frame_store->poc;
                pCurrSlicePara->declist[0][i].frame_is_long_term = pH264Ctx->pListX[0][i]->is_long_term;
                pCurrSlicePara->declist[0][i].frame_structure = pH264Ctx->pListX[0][i]->structure;
                pCurrSlicePara->declist[0][i].frame_poc = pH264Ctx->pListX[0][i]->frame_store->frame.poc;
                pCurrSlicePara->declist[0][i].top_poc = pH264Ctx->pListX[0][i]->frame_store->top_field.poc;
                pCurrSlicePara->declist[0][i].btm_poc = pH264Ctx->pListX[0][i]->frame_store->bottom_field.poc;
                pCurrSlicePara->declist[0][i].list_structure = 0; //frame
            }

            // list1
            if (B_SLICE == pH264Ctx->CurrSlice.slice_type)
            {
                for (i=0; i<pH264Ctx->CurrSlice.listXsize[1]; i++)
                {
                    pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[1][i]->frame_store->logic_fs_id);

                    pCurrSlicePara->declist[1][i].pmv_address_idc = pH264Ctx->pListX[1][i]->frame_store->pmv_address_idc;
                    pCurrSlicePara->declist[1][i].is_used = pH264Ctx->pListX[1][i]->frame_store->is_used;
                    //pCurrSlicePara->declist[1][i].img_idc = pH264Ctx->pListX[1][i]->frame_store->fs_image.image_id;
                    pCurrSlicePara->declist[0][i].img_idc = (pH264Ctx->pListX[1][i]->frame_store->non_existing == 1)? ExistImgIdc_1:
                        (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;
                    pCurrSlicePara->declist[1][i].apc_idc = pH264Ctx->pListX[1][i]->frame_store->apc_idc;
                    pCurrSlicePara->declist[1][i].poc = pH264Ctx->pListX[1][i]->frame_store->poc;
                    pCurrSlicePara->declist[1][i].frame_is_long_term = pH264Ctx->pListX[1][i]->is_long_term;
                    pCurrSlicePara->declist[1][i].frame_structure = pH264Ctx->pListX[1][i]->structure;
                    pCurrSlicePara->declist[1][i].frame_poc = pH264Ctx->pListX[1][i]->frame_store->frame.poc;
                    pCurrSlicePara->declist[1][i].top_poc = pH264Ctx->pListX[1][i]->frame_store->top_field.poc;
                    pCurrSlicePara->declist[1][i].btm_poc = pH264Ctx->pListX[1][i]->frame_store->bottom_field.poc;
                    pCurrSlicePara->declist[1][i].list_structure = 0; //frame
                }
            }
        }
        else // field reference
        {
            for (i=0; i<pH264Ctx->CurrSlice.listXsize[0]; i++)
            {
                pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[0][i]->frame_store->logic_fs_id);

                pCurrSlicePara->declist[0][i].pmv_address_idc = pH264Ctx->pListX[0][i]->frame_store->pmv_address_idc;
                pCurrSlicePara->declist[0][i].is_used = pH264Ctx->pListX[0][i]->frame_store->is_used;
                //pCurrSlicePara->declist[0][i].img_idc = pH264Ctx->pListX[0][i]->frame_store->fs_image.image_id;
                pCurrSlicePara->declist[0][i].img_idc = (pH264Ctx->pListX[0][i]->frame_store->non_existing == 1)? ExistImgIdc_0:
                    (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;
                pCurrSlicePara->declist[0][i].apc_idc = pH264Ctx->pListX[0][i]->frame_store->apc_idc;
                pCurrSlicePara->declist[0][i].poc = pH264Ctx->pListX[0][i]->frame_store->poc;
                pCurrSlicePara->declist[0][i].list_structure = pH264Ctx->pListX[0][i]->structure; //field
                pCurrSlicePara->declist[0][i].frame_structure = pH264Ctx->pListX[0][i]->frame_store->frame.structure;
                if (1 == pH264Ctx->pListX[0][i]->structure)
                {
                    //top field
                    pCurrSlicePara->declist[0][i].top_structure = pH264Ctx->pListX[0][i]->structure;
                    pCurrSlicePara->declist[0][i].top_is_long_term = pH264Ctx->pListX[0][i]->is_long_term;
                    pCurrSlicePara->declist[0][i].top_poc = pH264Ctx->pListX[0][i]->poc;
                }
                else
                {
                    //btm field
                    pCurrSlicePara->declist[0][i].btm_structure = pH264Ctx->pListX[0][i]->structure;
                    pCurrSlicePara->declist[0][i].btm_is_long_term = pH264Ctx->pListX[0][i]->is_long_term;
                    pCurrSlicePara->declist[0][i].btm_poc = pH264Ctx->pListX[0][i]->poc;
                }
            }

            if (B_SLICE == pH264Ctx->CurrSlice.slice_type) // B Slice
            {
                for (i=0; i<pH264Ctx->CurrSlice.listXsize[1]; i++)
                {
                    pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[1][i]->frame_store->logic_fs_id);

                    pCurrSlicePara->declist[1][i].pmv_address_idc = pH264Ctx->pListX[1][i]->frame_store->pmv_address_idc;
                    pCurrSlicePara->declist[1][i].is_used = pH264Ctx->pListX[1][i]->frame_store->is_used;
                    //pCurrSlicePara->declist[1][i].img_idc = pH264Ctx->pListX[1][i]->frame_store->fs_image.image_id;
                    pCurrSlicePara->declist[1][i].img_idc = (pH264Ctx->pListX[1][i]->frame_store->non_existing == 1)? ExistImgIdc_1:
                        (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;
                    pCurrSlicePara->declist[1][i].apc_idc = pH264Ctx->pListX[1][i]->frame_store->apc_idc;
                    pCurrSlicePara->declist[1][i].poc = pH264Ctx->pListX[1][i]->frame_store->poc;
                    pCurrSlicePara->declist[1][i].list_structure = pH264Ctx->pListX[1][i]->structure; //field
                    pCurrSlicePara->declist[1][i].frame_structure = pH264Ctx->pListX[1][i]->frame_store->frame.structure;
                    if (1 == pH264Ctx->pListX[1][i]->structure)
                    {
                        //top field
                        pCurrSlicePara->declist[1][i].top_structure = pH264Ctx->pListX[1][i]->structure;
                        pCurrSlicePara->declist[1][i].top_is_long_term = pH264Ctx->pListX[1][i]->is_long_term;
                        pCurrSlicePara->declist[1][i].top_poc = pH264Ctx->pListX[1][i]->poc;
                    }
                    else
                    {
                        //btm field
                        pCurrSlicePara->declist[1][i].btm_structure = pH264Ctx->pListX[1][i]->structure;
                        pCurrSlicePara->declist[1][i].btm_is_long_term = pH264Ctx->pListX[1][i]->is_long_term;
                        pCurrSlicePara->declist[1][i].btm_poc = pH264Ctx->pListX[1][i]->poc;
                    }
                }
            }
        }

        for (i = 0; i < 32; i++)
        {
            pCurrSlicePara->Apc2RefIdx[i] = pH264Ctx->APC.RefIdx[i];
        }

        // add weight tab
        pH264Ctx->DecPicPara.weighted_pred_flag = pH264Ctx->CurrPPS.weighted_pred_flag;
        pH264Ctx->DecPicPara.weighted_bipred_idc = pH264Ctx->CurrPPS.weighted_bipred_idc;
        if ((pH264Ctx->CurrPPS.weighted_pred_flag && ( P_SLICE == pH264Ctx->CurrSlice.slice_type)) ||
            ((1 == pH264Ctx->CurrPPS.weighted_bipred_idc) && ( B_SLICE == pH264Ctx->CurrSlice.slice_type)))
        {
            pCurrSlicePara->luma_log2_weight_denom = pH264Ctx->CurrSlice.luma_log2_weight_denom;
            pCurrSlicePara->chroma_log2_weight_denom = pH264Ctx->CurrSlice.chroma_log2_weight_denom;

            // list0
            // luma weight tab of list0
            for (i = 0; i <= pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1; i++)
            {
                pCurrSlicePara->wp_weight_l0_y[i] = pH264Ctx->CurrSlice.wp_weight_l0_y[i];
                pCurrSlicePara->wp_weight_l0_u[i] = pH264Ctx->CurrSlice.wp_weight_l0_u[i];
                pCurrSlicePara->wp_weight_l0_v[i] = pH264Ctx->CurrSlice.wp_weight_l0_v[i];

                pCurrSlicePara->wp_offset_l0_y[i] = pH264Ctx->CurrSlice.wp_offset_l0_y[i];
                pCurrSlicePara->wp_offset_l0_u[i] = pH264Ctx->CurrSlice.wp_offset_l0_u[i];
                pCurrSlicePara->wp_offset_l0_v[i] = pH264Ctx->CurrSlice.wp_offset_l0_v[i];
            }

            // list1
            if (B_SLICE  == pH264Ctx->CurrSlice.slice_type) // B Slice
            {
                for (i = 0; i <= pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1; i++)
                {
                    pCurrSlicePara->wp_weight_l1_y[i] = pH264Ctx->CurrSlice.wp_weight_l1_y[i];
                    pCurrSlicePara->wp_weight_l1_u[i] = pH264Ctx->CurrSlice.wp_weight_l1_u[i];
                    pCurrSlicePara->wp_weight_l1_v[i] = pH264Ctx->CurrSlice.wp_weight_l1_v[i];

                    pCurrSlicePara->wp_offset_l1_y[i] = pH264Ctx->CurrSlice.wp_offset_l1_y[i];
                    pCurrSlicePara->wp_offset_l1_u[i] = pH264Ctx->CurrSlice.wp_offset_l1_u[i];
                    pCurrSlicePara->wp_offset_l1_v[i] = pH264Ctx->CurrSlice.wp_offset_l1_v[i];
                }
            }
        }
    }

    pH264Ctx->SlcInfo[CurrSliceSlotNum].pSliceNal = pH264Ctx->pCurrNal;
    pH264Ctx->pCurrNal = NULL;

    return;
}


VOID StopPicNum(H264_CTX_S *pH264Ctx)
{
#if 0
    if ((800+1) == pH264Ctx->TotalPicNum)
    {
        dprint(PRN_DBG,"PicNum:\n", pH264Ctx->TotalPicNum-1);
        while (1)
        {
            SINT8 c_get;
            dprint(PRN_DBG, "input 'c' to continue\n");
            scanf("%c", &c_get);
            if ('c'==c_get)
            {
                break;
            }
            if ('a'==c_get)
            {
            }
        }
    }
#endif

    return;
}


/*!
************************************************************************
* ����ԭ�� : SINT32 DecVDM(H264_CTX_S *pH264Ctx)
* �������� : ����vdm����
* ����˵�� :
* ����ֵ   ��
************************************************************************
*/
SINT32 DecVDM(H264_CTX_S *pH264Ctx)
{
    UINT32 IPBDecMode;

    /* ͳ��֡�� */
    if (pH264Ctx->CurrPic.structure == FRAME || (pH264Ctx->CurrPic.structure != FRAME && pH264Ctx->CurrPic.field_pair_flag == 1))
    {
        VDEC_CHAN_STATE_S  *pChanState = &pH264Ctx->pstExtraData->stChanState;
        pH264Ctx->numTotalFrame++;
        pH264Ctx->numTotalFrame = (pH264Ctx->numTotalFrame < pH264Ctx->numOutputFrame)?
            pH264Ctx->numOutputFrame : pH264Ctx->numTotalFrame;

        pChanState->total_frame_num = pH264Ctx->numTotalFrame;
        pChanState->error_frame_num = pH264Ctx->numTotalFrame - pH264Ctx->numOutputFrame;
    }

    //clean up information, clean here for statistic err mb when not start vdm or  reset
    IPBDecMode = (pH264Ctx->advanced_cfg & 0x70000) >> 16;
    if ( (pH264Ctx->OldDecMode != IPBDecMode)
      && (pH264Ctx->OldDecMode == I_MODE || pH264Ctx->OldDecMode == DISCARD_MODE)
      && (IPBDecMode != I_MODE)
      && (IPBDecMode == IP_MODE || IPBDecMode == IPB_MODE || IPBDecMode == DISCARD_B_BF_P_MODE) )
    {
        dprint(PRN_CTRL, "----------------- Imode -> %d ------------------\n", IPBDecMode);
        pH264Ctx->OldDecMode = IPBDecMode;
        H264_ClearCurrPic(pH264Ctx);
        ClearDPB(pH264Ctx);
        return H264_ERR;  //DPB_ERR
    }

    pH264Ctx->OldDecMode = IPBDecMode;

    switch (pH264Ctx->OldDecMode)
    {
    case DISCARD_MODE:
        H264_ClearCurrPic(pH264Ctx);
        ClearDPB(pH264Ctx);
        return H264_ERR;  //IDR_LOST
    case I_MODE:
        //I dec mode, dec I pic only
        if (0 != pH264Ctx->CurrPic.pic_type )  //P��B pic
        {
            //p(1), b(2) pic, not start vdm dec; all mb not dec calculate to err_level
            pH264Ctx->CurrPic.mode_skip_flag = 1;
            H264_ClearCurrPic(pH264Ctx);
            return H264_ERR;
        }
        else  //I pic
        {
            if (0 != pH264Ctx->SliceParaNum)
            {
                if (0 != pH264Ctx->DecSlicePara[0].first_mb_in_slice)
                {
                    pH264Ctx->CurrPic.mode_skip_flag = 1;
                    H264_ClearCurrPic(pH264Ctx);
		            dprint(PRN_FATAL, "I_MODE but first_mb_in_slice != 0\n");
                    return H264_ERR;
                }
            }
        }
        break;
    case IP_MODE:
        if ( (2 == pH264Ctx->CurrPic.pic_type) && (0 == pH264Ctx->CurrPic.nal_ref_idc) )
        {
            //b(2) pic, and not ref pic
            pH264Ctx->CurrPic.mode_skip_flag = 1;
            H264_ClearCurrPic(pH264Ctx);
            return H264_ERR;
        }
        break;
    case IPB_MODE:
    case DISCARD_B_BF_P_MODE:
    default :
        break;
    }

    if (0 == pH264Ctx->SliceParaNum)
    {
        H264_ClearCurrPic(pH264Ctx);
        return H264_ERR;
    }

    /* CRCУ��ʱ��������֡�ݴ� */
	/* �ݴ������ڻع��׼����ʱ��5X6 ��������I ��������ſɽ��B֡���������ᶪ����B,
	   �����º�������ع�ʱע�͵� */
    if (pH264Ctx->pstExtraData->s8SpecMode != 1)
    {
        if ((DISCARD_B_BF_P_MODE == pH264Ctx->OldDecMode) && (0 == pH264Ctx->CurrPic.pic_type))
        {
            if (0 == pH264Ctx->PPicFound)
            {
                pH264Ctx->PPicFound = 2;/*find the first I*/
            }
            else
            {
                pH264Ctx->PPicFound = 1;/*find annother I*/
            }
        }

        if ( (2==pH264Ctx->CurrPic.pic_type) && (2==pH264Ctx->PPicFound) &&
           (NOT_DEC_ORDER==pH264Ctx->pstExtraData->s32DecOrderOutput) && (1 != g_allow_B_before_P)) // ����B֡
        {
            dprint(PRN_ERROR, "Discard this B(poc=%d) befor P, is_ref_idc=%d.\n",pH264Ctx->CurrPic.thispoc, pH264Ctx->CurrPic.nal_ref_idc);
            H264_ClearCurrPic(pH264Ctx);
            FSP_SetDisplay(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 0);
            return H264_ERR;
        }
        else if (1== pH264Ctx->CurrPic.pic_type)
        {
            pH264Ctx->PPicFound = 1;
        }
    }
    
    dprint(PRN_CTRL, "***** VDM start, TotalPicNum=%d, pH264Ctx->CurrPic.structure=%d.\n", pH264Ctx->TotalPicNum - 1, pH264Ctx->CurrPic.structure);
    pH264Ctx->DecPicPara.mbtodec = pH264Ctx->CurrPic.pic_mbs;
    pH264Ctx->DecPicPara.NewPicFlag = 1;
    pH264Ctx->DecPicPara.stream_base_addr = pH264Ctx->CurrPic.stream_base_addr;
    pH264Ctx->DecPicPara.SliceParaNum = pH264Ctx->SliceParaNum;

    if (pH264Ctx->TotalPicNum >= (START_FRAME+1))
    {
        /* ����I֡, �ϱ��¼� */
        if (0 == pH264Ctx->CurrPic.pic_type)
        {
            SINT32  H264_GetPicStreamSize(H264_CTX_S *pH264Ctx);
            REPORT_FIND_IFRAME(pH264Ctx->ChanID, H264_GetPicStreamSize(pH264Ctx));
        }
        else if (pH264Ctx->DPB.used_size <= 0)
        {
            /* SIMPLE_DPBģʽ�¼�ʹDPBΪ��Ҳǿ�ƽ���, �Ա�֤�����ʹû��I֡����Ҳ�ܼ��� */
            if (pH264Ctx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB)
            {
                H264_ClearCurrPic(pH264Ctx);
                return H264_ERR;
            }
        }

        pH264Ctx->pH264DecParam = &pH264Ctx->DecPicPara;

        if (1 == pH264Ctx->pstExtraData->stDisParams.s32Mode)
        {
            if ((NULL != pH264Ctx->pH264DecParam) && (2 == pH264Ctx->CurrPic.pic_type))
            {
                pH264Ctx->pH264DecParam = NULL;
                FSP_SetDisplay(pH264Ctx->ChanID,  pH264Ctx->CurrPic.frame_store->logic_fs_id, 0);
                H264_ClearCurrPic(pH264Ctx);
                return H264_ERR;
            }
        }
        else if (2 == pH264Ctx->pstExtraData->stDisParams.s32Mode)
        {
            if ((pH264Ctx->pstExtraData->stDisParams.s32DisNums > 0) && (NULL != pH264Ctx->pH264DecParam)
                && (2 == pH264Ctx->CurrPic.pic_type))
            {
                pH264Ctx->pH264DecParam = NULL;
                FSP_SetDisplay(pH264Ctx->ChanID,  pH264Ctx->CurrPic.frame_store->logic_fs_id, 0);
                pH264Ctx->pstExtraData->stDisParams.s32DisNums--;
                H264_ClearCurrPic(pH264Ctx);
                return H264_ERR;
            }
        }

		pH264Ctx->pH264DecParam->Compress_en = pH264Ctx->pstExtraData->s32Compress_en;
    }

    return H264_OK;
}


SINT32 H264_GetPicStreamSize(H264_CTX_S *pH264Ctx)
{
    H264_DEC_SLICE_PARAM_S *pH264SliceParam;
    SINT32 TotalSize = 0;

    pH264SliceParam = pH264Ctx->DecPicPara.pFirstSlice;
    while (pH264SliceParam)
    {
        TotalSize += ((pH264SliceParam->valid_bitlen[0] + pH264SliceParam->valid_bitlen[1] + 7) / 8);
        pH264SliceParam = pH264SliceParam->pNextSlice;
    }

    return TotalSize;
}


/*!
************************************************************************
*      ����ԭ�� : SINT32 SliceCheck(H264_CTX_S *pH264Ctx)
*      �������� : ���ݿ�����˵�״ֻ̬����I֡������Nalu Type���ˣ����ݵ�һ�ν��������ֻ����I֡������Nalu Type����;
*      ����˵�� : VOID
*      ����ֵ   ��DEC_NORMAL, DEC_ERR
************************************************************************
*/
SINT32 SliceCheck(H264_CTX_S *pH264Ctx)
{
    UINT32 SliceType;
	UINT32 totalMbMin1;
    pH264Ctx->CurrSlice.first_mb_in_slice = ue_v(pH264Ctx, "SH: first_mb_in_slice");
    SE_ERR_CHECK;

    if (MAX_MB_NUM_IN_PIC <= pH264Ctx->CurrSlice.first_mb_in_slice)
    {
        dprint(PRN_ERROR, "slicecheck first_mb_in_slice >= MAX_MB_NUM_IN_PIC error.\n");
        return H264_ERR;
    }
    totalMbMin1 = pH264Ctx->CurWidthInMb*pH264Ctx->CurHeightInMb - 1;
	if (pH264Ctx->CurrSlice.first_mb_in_slice > totalMbMin1)
	{
		return H264_ERR;
	}
	//end
    SliceType = ue_v(pH264Ctx, "SH: slice_type");
    dprint(PRN_SLICE, "slice type = %d\n", SliceType);
    SE_ERR_CHECK;

    switch (SliceType)
    {
    case 2:
    case 4:
    case 7:
    case 9:
        pH264Ctx->CurrSlice.slice_type = I_SLICE;
        break;
    case 0:
    case 3:
    case 5:
    case 8:
        pH264Ctx->CurrSlice.slice_type = P_SLICE;
        break;
    case 1:
    case 6:
        pH264Ctx->CurrSlice.slice_type = B_SLICE;
        break;
    default:
        dprint(PRN_ERROR, "slice type = %d, err\n", SliceType);
        return H264_ERR;
    }

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 PPSSPSCheck(H264_CTX_S *pH264Ctx)
*      �������� :  У��pps��sps�Ƿ�õ�
*      ����˵�� :
*      ����ֵ   �� DEC_NORMAL; DEC_ERR;
************************************************************************
*/
SINT32 PPSSPSCheck(H264_CTX_S *pH264Ctx)
{
    H264_PPS_S *pPPS;
    H264_SPS_S *pSPS;

    // check pps is get or not
    dprint(PRN_SLICE, "slice  pps id = %d\n", pH264Ctx->CurrSlice.pic_parameter_set_id);

    pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    if (!pPPS->is_valid)
    {
        dprint(PRN_ERROR, "pps with this pic_parameter_set_id = %d havn't decode\n", pH264Ctx->CurrSlice.pic_parameter_set_id);
        return H264_ERR;
    }

    // check sps is get or not
    pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);
    if (!pSPS->is_valid)
    {
        dprint(PRN_ERROR, "sps with this pic_parameter_set_id = %d havn't decode\n", pPPS->seq_parameter_set_id);
        return H264_ERR;
    }

    return H264_OK;
}



/*!
************************************************************************
*      ����ԭ�� :  SINT32 PPSSPSCheckTmpId(H264_CTX_S *pH264Ctx, UINT32 tmp_pic_parameter_set_id)
*      �������� :  У��pps��sps�Ƿ�õ�
*      ����˵�� :
*      ����ֵ   �� DEC_NORMAL; DEC_ERR;
************************************************************************
*/
SINT32 PPSSPSCheckTmpId(H264_CTX_S *pH264Ctx, UINT32 tmp_pic_parameter_set_id)
{
    H264_PPS_S *pPPS;
    H264_SPS_S *pSPS;

    // check pps is get or not
    dprint(PRN_SLICE, "tmp slice  pps id = %d\n", tmp_pic_parameter_set_id);

    pPPS = &(pH264Ctx->PPS[tmp_pic_parameter_set_id]);
    if (!pPPS->is_valid)
    {
        dprint(PRN_ERROR, "PPSSPSCheckTmpId: pps with this pic_parameter_set_id = %d havn't decode\n", tmp_pic_parameter_set_id);
        return H264_ERR;
    }

    if (pPPS->seq_parameter_set_id > (pH264Ctx->MaxSpsNum-1))
    {
        dprint(PRN_ERROR, "PPSSPSCheckTmpId: seq_parameter_set_id out of range.\n");
        return H264_ERR;
    }

    // check sps is get or not
    pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);
    if (!pSPS->is_valid)
    {
        dprint(PRN_ERROR, "PPSSPSCheckTmpId: sps with this pic_parameter_set_id = %d havn't decode\n", pPPS->seq_parameter_set_id);
        return H264_ERR;
    }

    return H264_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  UINT32 IsNewPic(H264_CTX_S *pH264Ctx)
*      �������� :  �Ƚ�slice �﷨Ԫ�أ�ȷ���Ƿ���new pic
*      ����˵�� :  ��
*      ����ֵ   �� �ȽϽ��
************************************************************************
*/
UINT32 IsNewPic(H264_CTX_S *pH264Ctx)
{
    UINT32 result = 0;

    result |= (pH264Ctx->PrevSlice.pic_parameter_set_id != pH264Ctx->CurrSlice.pic_parameter_set_id);
    result |= (pH264Ctx->PrevSlice.seq_parameter_set_id != pH264Ctx->CurrSPS.seq_parameter_set_id);
    result |= (pH264Ctx->PrevSlice.frame_num != pH264Ctx->CurrSlice.frame_num);
    result |= (pH264Ctx->PrevSlice.field_pic_flag != pH264Ctx->CurrSlice.field_pic_flag);
    if (pH264Ctx->PrevSlice.field_pic_flag && pH264Ctx->CurrSlice.field_pic_flag)
    {
        result |= (pH264Ctx->PrevSlice.bottom_field_flag != pH264Ctx->CurrSlice.bottom_field_flag);
    }
    result |= (pH264Ctx->PrevSlice.nal_ref_idc != pH264Ctx->CurrSlice.nal_ref_idc) && ((0==pH264Ctx->PrevSlice.nal_ref_idc) || (0==pH264Ctx->CurrSlice.nal_ref_idc));
    result |= (pH264Ctx->PrevSlice.nal_unit_type != pH264Ctx->CurrSlice.nal_unit_type);
    if (NALU_TYPE_IDR==pH264Ctx->CurrSlice.nal_unit_type && NALU_TYPE_IDR==pH264Ctx->PrevSlice.nal_unit_type)
    {
        result |= (pH264Ctx->PrevSlice.idr_pic_id != pH264Ctx->CurrSlice.idr_pic_id);
    }
    if (!pH264Ctx->CurrSPS.pic_order_cnt_type)
    {
        result |= (pH264Ctx->PrevSlice.pic_oder_cnt_lsb          != pH264Ctx->CurrPOC.pic_order_cnt_lsb);
        result |= (pH264Ctx->PrevSlice.delta_pic_oder_cnt_bottom != pH264Ctx->CurrPOC.delta_pic_order_cnt_bottom);
    }
    if (1 == pH264Ctx->CurrSPS.pic_order_cnt_type)
    {
        result |= (pH264Ctx->PrevSlice.delta_pic_order_cnt[0] != pH264Ctx->CurrPOC.delta_pic_order_cnt[0]);
        result |= (pH264Ctx->PrevSlice.delta_pic_order_cnt[1] != pH264Ctx->CurrPOC.delta_pic_order_cnt[1]);
    }

    // �е��������ر���һЩ������˵��������ǵ�Ҫ��һ�в���ͨ��
    result |= (pH264Ctx->CurrSlice.first_mb_in_slice == 0);

    dprint(PRN_SLICE, "new pic flag = %d\n", result);

    return result;
}


/***********************************************************************************
* Function:    ref_pic_list_reordering(H264_CTX_S *pH264Ctx)
* Description: ����Reorder����
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL      ���Խ���MMCO����
*              DEC_ERR         �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 ref_pic_list_reordering(H264_CTX_S *pH264Ctx)
{
    UINT32 MaxPicNum, i;
    H264_PPS_S *pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    H264_SPS_S *pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);

    pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l0 = 0;
    pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l1 = 0;
    if (pH264Ctx->CurrSlice.field_pic_flag)
    {
        MaxPicNum = 1 << (pSPS->log2_max_frame_num_minus4 + 5);
    }
    else
    {
        MaxPicNum = 1 << (pSPS->log2_max_frame_num_minus4 + 4);
    }

    if ((P_SLICE == pH264Ctx->CurrSlice.slice_type) || (B_SLICE == pH264Ctx->CurrSlice.slice_type))
    {
        pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l0 = u_1(pH264Ctx, "SH: ref_pic_list_reordering_flag_l0");
        if (pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l0)
        {
            for (i=0; i < (pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1+2); i++)
            {
                pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i] = ue_v(pH264Ctx, "SH: reordering_of_pic_nums_idc_l0");
                if ((3 < pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i]) || pH264Ctx->SeErrFlg)
                {
                    dprint(PRN_ERROR, "reorder idc l0 = %d, g_SeErrFlag=%d\n",
                        pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i], pH264Ctx->SeErrFlg);
                    return H264_ERR;
                }
                if (3 == pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i])
                {
                    break;
                }

                if ((0 == pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i]) || (1 == pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i]))
                {
                    pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l0[i] = ue_v(pH264Ctx, "SH: abs_diff_pic_num_minus1_l0");
                    if ((pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l0[i]>=MaxPicNum) || pH264Ctx->SeErrFlg)
                    {
                        dprint(PRN_ERROR,"abs_diff_pic_num_minus1_l0 = %d, g_SeErrFlag=%d\n",
                            pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l0[i], pH264Ctx->SeErrFlg);
                        return H264_ERR;
                    }
                }
                else if (2 == pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i])
                {
                    pH264Ctx->CurrReorder.long_term_pic_idx_l0[i] = ue_v(pH264Ctx, "SH: long_term_pic_idx_l0");
                }
            }

            // check the num of pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i] no exceed
            if ((i==(pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1+2)) && (pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i]!=3))
            {
                dprint(PRN_ERROR, "num of idc l0 exceed\n");
                return H264_ERR;
            }
        }
    }

    if (B_SLICE == pH264Ctx->CurrSlice.slice_type)
    {
        pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l1 = u_1(pH264Ctx, "SH: ref_pic_list_reordering_flag_l1");
        if (pH264Ctx->CurrReorder.ref_pic_list_reordering_flag_l1)
        {
            for (i=0; i<(pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1+2); i++)
            {
                pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i] = ue_v(pH264Ctx, "SH: reordering_of_pic_nums_idc_l1");
                if ((3 < pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i]) || pH264Ctx->SeErrFlg)
                {
                    dprint(PRN_ERROR, "reorder idc l1 = %d, g_SeErrFlag=%d\n",
                        pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i], pH264Ctx->SeErrFlg);
                    return H264_ERR;
                }
                if (3 == pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i])
                {
                    break;
                }

                if ((0 == pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i]) || (1 == pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i]))
                {
                    pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l1[i] = ue_v(pH264Ctx, "SH: abs_diff_pic_num_minus1_l1");
                    if ((pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l1[i]>=MaxPicNum) || pH264Ctx->SeErrFlg)
                    {
                        dprint(PRN_ERROR,"abs_diff_pic_num_minus1_l1 = %d, g_SeErrFlag=%d\n",
                            pH264Ctx->CurrReorder.abs_diff_pic_num_minus1_l1[i], pH264Ctx->SeErrFlg);
                        return H264_ERR;
                    }
                }
                else if (2==pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i])
                {
                    pH264Ctx->CurrReorder.long_term_pic_idx_l1[i] = ue_v(pH264Ctx, "SH: long_term_pic_idx_l1");
                }

                // check the num of pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l0[i] no exceed
                if ((i==(pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1+2)) && (pH264Ctx->CurrReorder.reordering_of_pic_nums_idc_l1[i]!=3))
                {
                    dprint(PRN_ERROR, "num of idc l1 exceed\n");
                    return H264_ERR;
                }
            }
        }
    }
    SE_ERR_CHECK;

    return H264_OK;
}


/***********************************************************************************
* Function:    pred_weight_table(H264_CTX_S *pH264Ctx)
* Description: ����weighted����
* Input:       ����������ʱ����������
* Output:      pH264Ctx->CurrSlice.wp_...   Slice�ṹ�����weighted����
* Return:      DEC_NORMAL           ���Խ���weighted����
*              DEC_ERR              �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 pred_weight_table(H264_CTX_S *pH264Ctx)
{
    SINT32 y_default_weight;
    SINT32 uv_default_weight = 0;
    UINT32 i;

    H264_PPS_S *pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    H264_SPS_S *pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);

    pH264Ctx->CurrSlice.luma_log2_weight_denom = ue_v(pH264Ctx, "SH: luma_log2_weight_denom");
    y_default_weight = 1 << pH264Ctx->CurrSlice.luma_log2_weight_denom;
    /*1 == chroma_format_idc -- 4:2:0*/
    if (0 != pSPS->chroma_format_idc)
    {
        pH264Ctx->CurrSlice.chroma_log2_weight_denom = ue_v(pH264Ctx, "SH: chroma_log2_weight_denom");
        uv_default_weight = 1 << pH264Ctx->CurrSlice.chroma_log2_weight_denom;
    }
    if ((7 < pH264Ctx->CurrSlice.luma_log2_weight_denom) || (7 < pH264Ctx->CurrSlice.chroma_log2_weight_denom) || pH264Ctx->SeErrFlg)
    {
        // �������ǯλ����������
        dprint(PRN_ERROR, "WP log2 exceed  and err flag = %d\n", pH264Ctx->SeErrFlg);
        return H264_ERR;
    }

    /* fill wp_parameter table */
    for (i=0; i<= pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1; i++) /* fw */
    {
        if (u_1(pH264Ctx, "SH: luma_weight_flag_l0"))
        {
            pH264Ctx->CurrSlice.wp_weight_l0_y[i] = se_v(pH264Ctx, "SH: luma_weight_l0");
            pH264Ctx->CurrSlice.wp_offset_l0_y[i] = se_v(pH264Ctx, "SH: luma_offset_l0");
        }
        else
        {
            pH264Ctx->CurrSlice.wp_weight_l0_y[i] = y_default_weight;
            pH264Ctx->CurrSlice.wp_offset_l0_y[i] = 0;
        }

        if (0 != pSPS->chroma_format_idc)
        {
            if (u_1(pH264Ctx, "SH: chroma_weight_flag_l0"))
            {
                pH264Ctx->CurrSlice.wp_weight_l0_u[i] = se_v(pH264Ctx, "SH: chroma_weight_l0");
                pH264Ctx->CurrSlice.wp_offset_l0_u[i] = se_v(pH264Ctx, "SH: chroma_offset_l0");
                pH264Ctx->CurrSlice.wp_weight_l0_v[i] = se_v(pH264Ctx, "SH: chroma_weight_l0");
                pH264Ctx->CurrSlice.wp_offset_l0_v[i] = se_v(pH264Ctx, "SH: chroma_offset_l0");
            }
            else
            {
                pH264Ctx->CurrSlice.wp_weight_l0_u[i] = uv_default_weight;
                pH264Ctx->CurrSlice.wp_offset_l0_u[i] = 0;
                pH264Ctx->CurrSlice.wp_weight_l0_v[i] = uv_default_weight;
                pH264Ctx->CurrSlice.wp_offset_l0_v[i] = 0;
            }
        }
        else
        {
            pH264Ctx->CurrSlice.wp_weight_l0_u[i] = 0;
            pH264Ctx->CurrSlice.wp_offset_l0_u[i] = 0;
            pH264Ctx->CurrSlice.wp_weight_l0_v[i] = 0;
            pH264Ctx->CurrSlice.wp_offset_l0_v[i] = 0;
        }
    }

    if ((1 == pPPS->weighted_bipred_idc) && (B_SLICE == pH264Ctx->CurrSlice.slice_type)) /* bw */
    {
        for (i = 0; i <= pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1; i++)
        {
            if (u_1(pH264Ctx, "SH: luma_weight_flag_l1"))
            {
                pH264Ctx->CurrSlice.wp_weight_l1_y[i] = se_v(pH264Ctx, "SH: luma_weight_l1");
                pH264Ctx->CurrSlice.wp_offset_l1_y[i] = se_v(pH264Ctx, "SH: luma_offset_l1");
            }
            else
            {
                pH264Ctx->CurrSlice.wp_weight_l1_y[i] = y_default_weight;
                pH264Ctx->CurrSlice.wp_offset_l1_y[i] = 0;
            }

            if (0 != pSPS->chroma_format_idc)
            {
                if (u_1(pH264Ctx, "SH: chroma_weight_flag_l1"))
                {
                    pH264Ctx->CurrSlice.wp_weight_l1_u[i] = se_v(pH264Ctx, "SH: chroma_weight_l1");
                    pH264Ctx->CurrSlice.wp_offset_l1_u[i] = se_v(pH264Ctx, "SH: chroma_offset_l1");
                    pH264Ctx->CurrSlice.wp_weight_l1_v[i] = se_v(pH264Ctx, "SH: chroma_weight_l1");
                    pH264Ctx->CurrSlice.wp_offset_l1_v[i] = se_v(pH264Ctx, "SH: chroma_offset_l1");
                }
                else
                {
                    pH264Ctx->CurrSlice.wp_weight_l1_u[i] = uv_default_weight;
                    pH264Ctx->CurrSlice.wp_offset_l1_u[i] = 0;
                    pH264Ctx->CurrSlice.wp_weight_l1_v[i] = uv_default_weight;
                    pH264Ctx->CurrSlice.wp_offset_l1_v[i] = 0;
                }
            }
            else
            {
                pH264Ctx->CurrSlice.wp_weight_l1_u[i] = 0;
                pH264Ctx->CurrSlice.wp_offset_l1_u[i] = 0;
                pH264Ctx->CurrSlice.wp_weight_l1_v[i] = 0;
                pH264Ctx->CurrSlice.wp_offset_l1_v[i] = 0;
            }
        }
    }
    SE_ERR_CHECK;

    return H264_OK;
}


/***********************************************************************************
* Function:    DecMMCO(H264_CTX_S *pH264Ctx,UINT32 save)
* Description: ����MMCO����
* Input:       ����������ʱ����������
*              save                        �Ƿ���Ҫ����MMCO����
* Return:      DEC_NORMAL                  ���Խ���MMCO����
*              DEC_ERR                     �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 DecMMCO(H264_CTX_S *pH264Ctx,UINT32 save)
{
    H264_PPS_S *pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    H264_SPS_S *pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);
    H264_MMCO_S *pMMCO;

    UINT32 MMCONum = 0;
    UINT32 memory_management_control_operation;
    UINT32 max_long_term_frame_idx_plus1;

    if (save)
    {
        do
        {
            if (MMCONum>=MAX_MMCO_LEN)
            {
                dprint(PRN_ERROR, "overflow MAX_MMCO_LEN\n");
                return H264_ERR;
            }

            pMMCO = &(pH264Ctx->Mark[pH264Ctx->CurrMarkIdx].MMCO[MMCONum]);

            pMMCO->memory_management_control_operation = ue_v(pH264Ctx, "SH: memory_management_control_operation");
            if ((1==pMMCO->memory_management_control_operation) || (3==pMMCO->memory_management_control_operation))
            {
                pMMCO->difference_of_pic_nums_minus1 = ue_v(pH264Ctx, "SH: difference_of_pic_nums_minus1");
            }
            if (2==pMMCO->memory_management_control_operation)
            {
                pMMCO->long_term_pic_num = ue_v(pH264Ctx, "SH: long_term_pic_num");
            }
            if ((3==pMMCO->memory_management_control_operation) || (pMMCO->memory_management_control_operation==6))
            {
                pMMCO->long_term_frame_idx = ue_v(pH264Ctx, "SH: long_term_frame_idx");
            }
            if (4==pMMCO->memory_management_control_operation)
            {
                pMMCO->max_long_term_frame_idx_plus1 = ue_v(pH264Ctx, "SH: max_long_term_frame_idx_plus1");
                if ((pSPS->num_ref_frames<pMMCO->max_long_term_frame_idx_plus1)
                    || (pMMCO->max_long_term_frame_idx_plus1<1) || pH264Ctx->SeErrFlg)
                {
                    // ������ж���Ҫ��һ���Ŀ���
                    // return DEC_ERR;
                }
            }

            if ((pMMCO->memory_management_control_operation>6) || pH264Ctx->SeErrFlg)
            {
                dprint(PRN_ERROR, "mmco exceed 6\n");
                return H264_ERR;
            }

            MMCONum++;
        }
        while (pMMCO->memory_management_control_operation != 0);
    }
    else // don't save
    {
        do
        {
            if (MMCONum>=MAX_MMCO_LEN)
            {
                return H264_ERR;
            }

            memory_management_control_operation = ue_v(pH264Ctx, "SH: memory_management_control_operation");
            if ((1==memory_management_control_operation) || (3==memory_management_control_operation))
            {
                ue_v(pH264Ctx, "SH: difference_of_pic_nums_minus1");
            }
            if (2==memory_management_control_operation)
            {
                ue_v(pH264Ctx, "SH: long_term_pic_num");
            }
            if ((3==memory_management_control_operation) || (6==memory_management_control_operation))
            {
                ue_v(pH264Ctx, "SH: long_term_frame_idx");
            }
            if (4==memory_management_control_operation)
            {
                max_long_term_frame_idx_plus1 = ue_v(pH264Ctx, "SH: max_long_term_frame_idx_plus1");
                if ((pSPS->num_ref_frames<max_long_term_frame_idx_plus1)
                    || (max_long_term_frame_idx_plus1<1) || pH264Ctx->SeErrFlg)
                {
                    // return DEC_ERR;
                }
            }

            if ((memory_management_control_operation>6) || pH264Ctx->SeErrFlg)
            {
                dprint(PRN_ERROR, "mmco exceed 6\n");
                return H264_ERR;
            }
            MMCONum++;
        }
        while (memory_management_control_operation != 0);
    }

    return H264_OK;
}


/***********************************************************************************
* Function:    dec_ref_pic_marking(H264_CTX_S *pH264Ctx)
* Description: ����MMCO����
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL               ���Խ���MMCO����
*              DEC_ERR                  �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 dec_ref_pic_marking(H264_CTX_S *pH264Ctx)
{
    H264_MARK_S *pMark;
    UINT32 tmpMarkIdx = pH264Ctx->CurrMarkIdx;

    if (pH264Ctx->CurrSlice.new_pic_flag)
    {
        pH264Ctx->CurrMarkIdx= pH264Ctx->CurrMarkIdx ? 0 : 1;
    }
    pMark = &(pH264Ctx->Mark[pH264Ctx->CurrMarkIdx]);

    if (pH264Ctx->CurrSlice.new_pic_flag || (0==pMark->is_valid))
    {
        if (NALU_TYPE_IDR==pH264Ctx->CurrSlice.nal_unit_type)
        {
            pMark->no_output_of_prior_pics_flag = u_1(pH264Ctx,"SH: no_output_of_prior_pics_flag");
            pMark->long_term_reference_flag = u_1(pH264Ctx,"SH: long_term_reference_flag");
            dprint(PRN_MARK_MMCO, "IDR and no_output_of_prior_pics_flag = %d\n", pMark->no_output_of_prior_pics_flag);
        }
        else
        {
            pMark->adaptive_ref_pic_marking_mode_flag = u_1(pH264Ctx, "SH: adaptive_ref_pic_marking_mode_flag");
            dprint(PRN_MARK_MMCO, "adaptive_ref_pic_marking_mode_flag = %d\n", pMark->adaptive_ref_pic_marking_mode_flag);
            if (pMark->adaptive_ref_pic_marking_mode_flag)
            {
                if ( H264_ERR == DecMMCO(pH264Ctx, 1) ) // save MMCO
                {
                    pMark->is_valid = 0;
                    pH264Ctx->CurrMarkIdx= tmpMarkIdx;
                    dprint(PRN_ERROR, "MMCO para would save dec err\n");
                    return H264_ERR;
                }
                pMark->is_valid = 1;
            }
        }
    }
    else
    {
        if (NALU_TYPE_IDR==pH264Ctx->CurrSlice.nal_unit_type)
        {
            if (pMark->no_output_of_prior_pics_flag != u_1(pH264Ctx,"SH: no_output_of_prior_pics_flag"))
            {
                // some one wrong, but don't know which used
                dprint(PRN_ERROR, "mulitislice diff no_out_of_prior_pics_flag, used first.\n");
                return H264_ERR;
            }

            if (pMark->long_term_reference_flag != u_1(pH264Ctx, "SH: long_term_reference_flag"))
            {
                // some one wrong, but don't know which used
                dprint(PRN_ERROR, "mulitislice diff long_term_reference_flag, used first.\n");
                return H264_ERR;
            }
        }
        else
        {
            if (pMark->adaptive_ref_pic_marking_mode_flag != u_1(pH264Ctx, "SH: adaptive_ref_pic_marking_mode_flag"))
            {
                // some one wrong, but don't know which used
                dprint(PRN_ERROR, "mulitislice diff adaptive_ref_pic_marking_mode_flag, used first.\n");
                dprint(PRN_ERROR, "TotalPicNum=%d, SliceNumInPic=%d, TotalNal=%d\n",
                    pH264Ctx->TotalPicNum, pH264Ctx->SliceNumInPic, pH264Ctx->TotalNal);
                return H264_ERR;
            }

            if (pMark->adaptive_ref_pic_marking_mode_flag)
            {
                return DecMMCO(pH264Ctx, 0);  // don't save
            }
        }
    }

    return H264_OK;
}


/***********************************************************************************
* Function:    ProcessSliceHeader(H264_CTX_S *pH264Ctx)
* Description: ����Slice Header, �ж��Ƿ���ͼ��
* Input:       �﷨��������������
* Output:      Slice���﷨Ԫ��,�Ƿ���ͼ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 ProcessSliceHeaderFirstPart(H264_CTX_S *pH264Ctx)
{
    UINT32 redundant_pic_cnt;
    UINT32 pic_size;
    UINT32 tmp_pic_parameter_set_id;

    H264_SPS_S *pSPS;
    H264_PPS_S *pPPS;

    // pH264Ctx->CurrSlice.PTS = g_pCurrNal->PTS;

    tmp_pic_parameter_set_id = ue_v(pH264Ctx, "SH: pic_parameter_set_id");
    if (((pH264Ctx->MaxPpsNum-1) < tmp_pic_parameter_set_id) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pps id:%d in sliceheader err\n", tmp_pic_parameter_set_id);
        return H264_ERR; // ���Կ��ǲ����أ�����pps idǿ��
    }

    if (H264_OK != PPSSPSCheckTmpId(pH264Ctx, tmp_pic_parameter_set_id))
    {
        dprint(PRN_ERROR, "PPS or SPS of this slice not valid\n");
        return H264_ERR;
    }
    pH264Ctx->CurrSlice.pic_parameter_set_id = tmp_pic_parameter_set_id;

    pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);

    // ����жϵ��������ܻ�������
    if (((NALU_TYPE_IDR == pH264Ctx->CurrSlice.nal_unit_type) || (0==pSPS->num_ref_frames)) &&
        (I_SLICE != pH264Ctx->CurrSlice.slice_type))
    {
        dprint(PRN_ERROR, "recover point but isn't I_SLICE.\n");
        return H264_ERR;
    }

    pH264Ctx->CurrSlice.frame_num = u_v(pH264Ctx, pSPS->log2_max_frame_num_minus4+4, "SH: frame_num");
    SE_ERR_CHECK;
    if (NALU_TYPE_IDR == pH264Ctx->CurrSlice.nal_unit_type)
    {
        pH264Ctx->CurrSlice.prev_frame_num = pH264Ctx->CurrSlice.frame_num;
        if ((0 != pH264Ctx->CurrSlice.frame_num) && (NALU_TYPE_IDR == pH264Ctx->CurrSlice.nal_unit_type))
        {
            dprint(PRN_ERROR, "IDR NAL but frame_num!=0.\n");
            // return DEC_ERR;
        }
    }

    /* Get picture structure */
    pH264Ctx->CurrSlice.field_pic_flag = 0;
    pH264Ctx->CurrSlice.bottom_field_flag = 0;
    if (!pSPS->frame_mbs_only_flag)
    {
        pH264Ctx->CurrSlice.field_pic_flag = u_1(pH264Ctx, "SH: field_pic_flag");
        if (pH264Ctx->CurrSlice.field_pic_flag)
        {
            pH264Ctx->CurrSlice.bottom_field_flag = u_1(pH264Ctx, "SH: bottom_field_flag");
        }
    }

    // check first_mb_in_slice
    pic_size = (pSPS->pic_width_in_mbs_minus1+1) *
        (pSPS->pic_height_in_map_units_minus1+1) *
        (!pSPS->frame_mbs_only_flag+1);
    pH264Ctx->CurrSlice.MbaffFrameFlag = (!pH264Ctx->CurrSlice.field_pic_flag && pSPS->mb_adaptive_frame_field_flag);
    if ((1 == pH264Ctx->CurrSlice.field_pic_flag) || (1 == pH264Ctx->CurrSlice.MbaffFrameFlag))
    {
        // max first_mb_in_slice is half pic_size - 1
        if ((pic_size/2) <= pH264Ctx->CurrSlice.first_mb_in_slice)
        {
            dprint(PRN_ERROR, "field first_mb_in_slice bigger than pic size\n");
            return H264_ERR;
        }
    }
    else
    {
        // max first_mb_in_slice is pic_size - 1
        if (pic_size <= pH264Ctx->CurrSlice.first_mb_in_slice)
        {
            dprint(PRN_ERROR, "frame first_mb_in_slice bigger than pic size\n");
            return H264_ERR;
        }
    }

    /* Get idr_pic_id */
    if (NALU_TYPE_IDR == pH264Ctx->CurrSlice.nal_unit_type)
    {
        pH264Ctx->CurrSlice.idr_pic_id = ue_v(pH264Ctx, "SH: idr_pic_id");
        if ((65535 <pH264Ctx->CurrSlice.idr_pic_id) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "idr_pic_id bigger than 65535\n");
            //return H264_ERR; // ����ȥ���������ǿ��
        }
    }

    /* Get POC parameters */
    pH264Ctx->CurrPOC.pic_order_cnt_lsb = 0;
    pH264Ctx->CurrPOC.delta_pic_order_cnt_bottom = 0;
    if (0 == pSPS->pic_order_cnt_type)
    {
        pH264Ctx->CurrPOC.pic_order_cnt_lsb = u_v(pH264Ctx, pSPS->log2_max_pic_order_cnt_lsb_minus4+4, "SH: pic_order_cnt_lsb");
        if (pPPS->pic_order_present_flag && (!pH264Ctx->CurrSlice.field_pic_flag))
        {
            pH264Ctx->CurrPOC.delta_pic_order_cnt_bottom = se_v(pH264Ctx, "SH: delta_pic_order_cnt_bottom");
        }
    }

    pH264Ctx->CurrPOC.delta_pic_order_cnt[0] = 0;
    pH264Ctx->CurrPOC.delta_pic_order_cnt[1] = 0;
    if ((1==pSPS->pic_order_cnt_type) && (!pSPS->delta_pic_order_always_zero_flag))
    {
        pH264Ctx->CurrPOC.delta_pic_order_cnt[0] = se_v(pH264Ctx, "SH: delta_pic_order_cnt[0]");
        if ((1==pPPS->pic_order_present_flag) && (!pH264Ctx->CurrSlice.field_pic_flag))
        {
            pH264Ctx->CurrPOC.delta_pic_order_cnt[1] = se_v(pH264Ctx, "SH: delta_pic_order_cnt[1]");
        }
    }
    SE_ERR_CHECK;

    if (pPPS->redundant_pic_cnt_present_flag)
    {
        redundant_pic_cnt = ue_v(pH264Ctx, "SH: redundant_pic_cnt");
        if ( (0<redundant_pic_cnt) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "redundant pic not support.\n");
            REPORT_UNSUPPORT(pH264Ctx->ChanID);
            return H264_ERR;
        }
    }

    pH264Ctx->CurrSlice.new_pic_flag = IsNewPic(pH264Ctx); // get new_pic_flag
    if ((0 == pH264Ctx->CurrSlice.new_pic_flag) && (pH264Ctx->CurrSlice.first_mb_in_slice <= pH264Ctx->PrevSlice.first_mb_in_slice))
    {
        //mb in slice err order
        dprint(PRN_ERROR, "first_mb_in_slice in currslice small than prevslice in same pic\n");
        return H264_ERR;
    }

    return H264_OK;
}


/***********************************************************************************
* Function:    ProcessSliceHeader(H264_CTX_S *pH264Ctx)
* Description: ����Slice Header, �ж��Ƿ���ͼ��
* Input:       �﷨��������������
* Output:      Slice���﷨Ԫ��,�Ƿ���ͼ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 ProcessSliceHeaderSecondPart(H264_CTX_S *pH264Ctx)
{
    UINT32 num_ref_idx_active_override_flag;
    H264_PPS_S *pPPS;

    if (((pH264Ctx->MaxPpsNum-1) < pH264Ctx->CurrSlice.pic_parameter_set_id) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pps id:%d in sliceheader err\n", pH264Ctx->CurrSlice.pic_parameter_set_id);
        return H264_ERR; // ���Կ��ǲ����أ�����pps idǿ��
    }

    if (H264_OK != PPSSPSCheck(pH264Ctx))
    {
        dprint(PRN_ERROR, "PPS or SPS of this slice not valid\n");
        return H264_ERR;
    }

    pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);

    /* Get mv direct predmode */
    if (B_SLICE == pH264Ctx->CurrSlice.slice_type)
    {
        pH264Ctx->CurrSlice.direct_spatial_mv_pred_flag = u_1(pH264Ctx, "SH: direct_spatial_mv_pred_flag");
    }

    /* Get num_ref_idx_lx_active_minus1 */
    num_ref_idx_active_override_flag = 0;
    pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1 = pPPS->num_ref_idx_l0_active_minus1;
    pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1 = pPPS->num_ref_idx_l1_active_minus1;

    if ((P_SLICE==pH264Ctx->CurrSlice.slice_type) || (B_SLICE==pH264Ctx->CurrSlice.slice_type))
    {
        num_ref_idx_active_override_flag = u_1(pH264Ctx, "SH: num_ref_idx_override_flag");
        if (num_ref_idx_active_override_flag)
        {
            pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1 = ue_v(pH264Ctx, "SH: num_ref_idx_l0_active_minus1");

            if (B_SLICE==pH264Ctx->CurrSlice.slice_type)
            {
                pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1 = ue_v(pH264Ctx, "SH: num_ref_idx_l1_active_minus1");
            }
        }

        if (0 == pH264Ctx->CurrSlice.field_pic_flag)
        {
            if ((15 < pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1)
                || ((15 <pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1) && (B_SLICE==pH264Ctx->CurrSlice.slice_type))
                || pH264Ctx->SeErrFlg)
            {
                dprint(PRN_ERROR, "frame num_ref_idx_lx_active_minus1 exceed\n");

                // ���Կ���ǯλȻ��ǿ��
                return H264_ERR;
            }
        }
        else
        {
            if ((31<pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1)
                || ((31<pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1) && (B_SLICE==pH264Ctx->CurrSlice.slice_type))
                || pH264Ctx->SeErrFlg)
            {
                dprint(PRN_ERROR, "field num_ref_idx_lx_active_minus1 exceed\n");

                // ���Կ���ǯλȻ��ǿ��
                return H264_ERR;
            }
        }
    }

    if (I_SLICE == pH264Ctx->CurrSlice.slice_type)
    {
        pH264Ctx->CurrSlice.num_ref_idx_l0_active_minus1 = 32; // ԭ���Ǹ�ֵ-1;
    }
    if (B_SLICE != pH264Ctx->CurrSlice.slice_type)
    {
        pH264Ctx->CurrSlice.num_ref_idx_l1_active_minus1 = 32; // ԭ���Ǹ�ֵ-1;
    }

    /* Get ref_pic_list_reordering parameters */
    if (H264_OK != ref_pic_list_reordering(pH264Ctx))
    {
        dprint(PRN_ERROR, "reordering para dec err\n");
        return H264_ERR;
    }

    /* Get pred_weight_table */
    pH264Ctx->CurrSlice.apply_weights_flag = ((pPPS->weighted_pred_flag && (P_SLICE==pH264Ctx->CurrSlice.slice_type)) ||
        ((1==pPPS->weighted_bipred_idc) && (B_SLICE==pH264Ctx->CurrSlice.slice_type)));
    dprint(PRN_SLICE, "apply_weights_flag=%d\n", pH264Ctx->CurrSlice.apply_weights_flag);
    pH264Ctx->CurrSlice.luma_log2_weight_denom = 0;
    pH264Ctx->CurrSlice.chroma_log2_weight_denom = 0;

    if (pH264Ctx->CurrSlice.apply_weights_flag)
    {
        if (H264_OK != pred_weight_table(pH264Ctx))
        {
            dprint(PRN_ERROR, "wpt dec err\n");
            return H264_ERR;
        }
    }

    /* Read MMCO marking parameters */
    if (pH264Ctx->CurrSlice.nal_ref_idc)
    {
        if (H264_OK != dec_ref_pic_marking(pH264Ctx))
        {
            dprint(PRN_ERROR, "mark para dec err\n");
            return H264_ERR;
        }
    }

    /* Get cabac_init_idc */
    pH264Ctx->CurrSlice.cabac_init_idc = 0;
    if (pPPS->entropy_coding_mode_flag && ((P_SLICE==pH264Ctx->CurrSlice.slice_type) || (B_SLICE==pH264Ctx->CurrSlice.slice_type)))
    {
        pH264Ctx->CurrSlice.cabac_init_idc = ue_v(pH264Ctx, "SH: cabac_init_idc");
        if ((3<=pH264Ctx->CurrSlice.cabac_init_idc) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "cabac_init_idc bigger than 3\n");
            // ���Բ²�һ��ֵ���¼�������
            return H264_ERR;
        }
    }

    /* Get slice_qp_delta */
    pH264Ctx->CurrSlice.slice_qp_delta = se_v(pH264Ctx, "SH: slice_qp_delta");

    /* Get deblocking filter control parameters */
    pH264Ctx->CurrSlice.disable_deblocking_filter_idc = 0;
    pH264Ctx->CurrSlice.slice_alpha_c0_offset_div2 = 0;
    pH264Ctx->CurrSlice.slice_beta_offset_div2 = 0;
    if (pPPS->deblocking_filter_control_present_flag)
    {
        pH264Ctx->CurrSlice.disable_deblocking_filter_idc = ue_v(pH264Ctx, "SH: disable_deblocking_filter_idc");
        if ((2 < pH264Ctx->CurrSlice.disable_deblocking_filter_idc) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "disable_deblocking_filter_idc dec err\n");

            // ���Կ������ǿ��
            return H264_ERR;
        }
        if (1 != pH264Ctx->CurrSlice.disable_deblocking_filter_idc)
        {
            pH264Ctx->CurrSlice.slice_alpha_c0_offset_div2 = se_v(pH264Ctx, "SH: slice_alpha_c0_offset_div2");
            if ((-6 > pH264Ctx->CurrSlice.slice_alpha_c0_offset_div2) || (6 <pH264Ctx->CurrSlice.slice_alpha_c0_offset_div2) || pH264Ctx->SeErrFlg)
            {
                dprint(PRN_ERROR, "slice_alpha_c0_offset_div2 dec err\n");

                // ����ǯλǿ��
                return H264_ERR;
            }
            pH264Ctx->CurrSlice.slice_beta_offset_div2 = se_v(pH264Ctx, "SH: slice_beta_offset_div2");
            if ((-6 > pH264Ctx->CurrSlice.slice_beta_offset_div2) || (6 < pH264Ctx->CurrSlice.slice_beta_offset_div2) || pH264Ctx->SeErrFlg)
            {
                dprint(PRN_ERROR, "slice_beta_offset_div2 dec err\n");

                // ����ǯλǿ��
                return H264_ERR;
            }
        }
    }

    return H264_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  ExitSlice (H264_CTX_S *pH264Ctx)
*   �������� :  ����ǰһ��slice�Ĳ���, Ϊ������ȷ�жϳ�new_pic_flag
*   ����˵�� :  ��
*   ����ֵ   �� ��
************************************************************************
*/
VOID ExitSlice(H264_CTX_S *pH264Ctx)
{
    H264_SPS_S *sps;
    H264_PPS_S *pps;

    pps = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    sps = &(pH264Ctx->SPS[pps->seq_parameter_set_id]);

    pH264Ctx->PrevSlice.pic_parameter_set_id = pH264Ctx->CurrSlice.pic_parameter_set_id;

    pH264Ctx->PrevSlice.seq_parameter_set_id = sps->seq_parameter_set_id;
    pH264Ctx->PrevSlice.first_mb_in_slice = pH264Ctx->CurrSlice.first_mb_in_slice;
    pH264Ctx->PrevSlice.frame_num = pH264Ctx->CurrSlice.frame_num;
    pH264Ctx->PrevSlice.field_pic_flag = pH264Ctx->CurrSlice.field_pic_flag;
    if (pH264Ctx->CurrSlice.field_pic_flag)
    {
        pH264Ctx->PrevSlice.bottom_field_flag = pH264Ctx->CurrSlice.bottom_field_flag;
    }
    pH264Ctx->PrevSlice.nal_ref_idc = pH264Ctx->CurrSlice.nal_ref_idc;
    pH264Ctx->PrevSlice.nal_unit_type = pH264Ctx->CurrSlice.nal_unit_type;
    if (NALU_TYPE_IDR==pH264Ctx->CurrSlice.nal_unit_type)
    {
        pH264Ctx->PrevSlice.idr_pic_id = pH264Ctx->CurrSlice.idr_pic_id;
    }
    if (!sps->pic_order_cnt_type)
    {
        pH264Ctx->PrevSlice.pic_oder_cnt_lsb          = pH264Ctx->CurrPOC.pic_order_cnt_lsb;
        pH264Ctx->PrevSlice.delta_pic_oder_cnt_bottom = pH264Ctx->CurrPOC.delta_pic_order_cnt_bottom;
    }
    if (1 == sps->pic_order_cnt_type)
    {
        pH264Ctx->PrevSlice.delta_pic_order_cnt[0] = pH264Ctx->CurrPOC.delta_pic_order_cnt[0];
        pH264Ctx->PrevSlice.delta_pic_order_cnt[1] = pH264Ctx->CurrPOC.delta_pic_order_cnt[1];
    }

    pH264Ctx->TotalSlice++;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID PicTypeStatistic(H264_CTX_S *pH264Ctx)
*      �������� :
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
VOID PicTypeStatistic(H264_CTX_S *pH264Ctx)
{
    switch (pH264Ctx->CurrSlice.slice_type)
    {
    case B_SLICE:
        pH264Ctx->CurrPic.pic_type = 2;
        pH264Ctx->CurrPic.is_idr_pic = 0;
        break;
    case P_SLICE:
        if (2 != pH264Ctx->CurrPic.pic_type)
        {
            pH264Ctx->CurrPic.pic_type = 1;
        }
        pH264Ctx->CurrPic.is_idr_pic = 0;
        break;
    case I_SLICE:
        if (NALU_TYPE_IDR == pH264Ctx->CurrSlice.nal_unit_type)
        {
            pH264Ctx->CurrPic.is_idr_pic = 1;
        }
        else
        {
            pH264Ctx->CurrPic.is_idr_pic = 0;
        }
        break;
    default :
        break;
    }
}


/*!
************************************************************************
*      ����ԭ�� :  VOID CalcStreamBits(H264_CTX_S *pH264Ctx)
*      �������� :
*      ����˵�� :
*      ����ֵ   ��
************************************************************************
*/
VOID CalcStreamBits(H264_CTX_S *pH264Ctx)
{
    UINT32 code_len_byte;
    UINT32 i;

#ifdef PRINT_STREAM
    UINT8 *buf;
    UINT8  i_stream_printf;
    SINT32 trail_zeros;
#endif

    if (pH264Ctx->CurrPPS.entropy_coding_mode_flag)
    {
        // cabac, byte_aligned
        pH264Ctx->pCurrNal->nal_bitoffset = (pH264Ctx->pCurrNal->nal_bitoffset + 7) & (~7);
        // stop one is valide bits
        pH264Ctx->pCurrNal->nal_trail_zero_bit_num -= 1;
    }

    // calculate bytes used befor curr segment
    code_len_byte = 0;
    for (i=0; i<(UINT32)pH264Ctx->pCurrNal->nal_used_segment; i++)
    {
        code_len_byte += pH264Ctx->pCurrNal->stream[i].bitstream_length;
    }
    // curr segment bitsoffset and so on
    dprint(PRN_STREAM, "pH264Ctx->pCurrNal->nal_bitoffset = %d; code_len_byte = %d; bitstream_length = %d; pH264Ctx->pCurrNal->nal_trail_zero_bit_num = %d\n",
        pH264Ctx->pCurrNal->nal_bitoffset, code_len_byte, pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bitstream_length, pH264Ctx->pCurrNal->nal_trail_zero_bit_num);
    // end
    pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bitsoffset = pH264Ctx->pCurrNal->nal_bitoffset & 7;
    pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bytespos = (pH264Ctx->pCurrNal->nal_bitoffset / 8 - code_len_byte) + pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].stream_phy_addr;
    pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].valid_bitlen = pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bitstream_length * 8 - (pH264Ctx->pCurrNal->nal_bitoffset - code_len_byte * 8);
    while ((pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bytespos & 3) != 0)
    {
        pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bitsoffset += 8;
        pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bytespos--;
    }

    dprint(PRN_STREAM, "stream[%d]: bitsoffset = %d; valid_bitlen = %d; bytespos = %x\n",
        i, pH264Ctx->pCurrNal->stream[i].bitsoffset, pH264Ctx->pCurrNal->stream[i].valid_bitlen, pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].bytespos);

     // last sement sub trailing zeros, when cavlc sun stop one
    if ((pH264Ctx->pCurrNal->nal_segment < 1) || (pH264Ctx->pCurrNal->nal_segment > 2))
    {
        dprint(PRN_FATAL, "nal_segment = %d is not expected value\n", pH264Ctx->pCurrNal->nal_segment);
        return;
    }

    // other segment bitsoffset and so on
    for (i=pH264Ctx->pCurrNal->nal_used_segment+1; i<pH264Ctx->pCurrNal->nal_segment; i++)
    {
        pH264Ctx->pCurrNal->stream[i].bitsoffset = 0;
        pH264Ctx->pCurrNal->stream[i].bytespos = pH264Ctx->pCurrNal->stream[i].stream_phy_addr;
        pH264Ctx->pCurrNal->stream[i].valid_bitlen = pH264Ctx->pCurrNal->stream[i].bitstream_length * 8;
        while ((pH264Ctx->pCurrNal->stream[i].bytespos & 3) != 0)
        {
            pH264Ctx->pCurrNal->stream[i].bitsoffset += 8;
            pH264Ctx->pCurrNal->stream[i].bytespos--;
        }
        dprint(PRN_STREAM, "stream[%d]: bitsoffset = %d; valid_bitlen = %d\n",
            i, pH264Ctx->pCurrNal->stream[i].bitsoffset, pH264Ctx->pCurrNal->stream[i].valid_bitlen);
    }

    pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_segment-1].valid_bitlen -= pH264Ctx->pCurrNal->nal_trail_zero_bit_num;

    // find stream_base_addr
    for (i=pH264Ctx->pCurrNal->nal_used_segment; i<pH264Ctx->pCurrNal->nal_segment; i++)
    {
        if (pH264Ctx->pCurrNal->stream[i].bytespos < pH264Ctx->CurrPic.stream_base_addr)
        {
            pH264Ctx->CurrPic.stream_base_addr = pH264Ctx->pCurrNal->stream[i].bytespos;
        }
    }

    // fprintf stream code, it must delete when decode
#ifdef PRINT_STREAM
    if (pH264Ctx->TotalPicNum >= (START_FRAME))
    {
        for (i=pH264Ctx->pCurrNal->nal_used_segment; i<pH264Ctx->pCurrNal->nal_segment; i++)
        {
            if (i == (pH264Ctx->pCurrNal->nal_segment-1))
            {
                trail_zeros = pH264Ctx->pCurrNal->nal_trail_zero_bit_num;
            }
            else
            {
                trail_zeros = 0;
            }

            buf = pH264Ctx->pCurrNal->stream[i].streamBuffer + (pH264Ctx->pCurrNal->stream[i].bytespos - pH264Ctx->pCurrNal->stream[i].stream_phy_addr);
            i_stream_printf = 0;
            while (buf < (pH264Ctx->pCurrNal->stream[i].streamBuffer + pH264Ctx->pCurrNal->stream[i].bitstream_length - trail_zeros / 8))
            {
                _LOG("%5x", *(buf));
                buf++;
                i_stream_printf++;
                if (16 == i_stream_printf)
                {
                    _LOG("\n");
                    i_stream_printf = 0;
                    //break;
                }
            }
            _LOG("\n");
        }
    }
#endif
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 H264_DecSlice(H264_CTX_S *pH264Ctx)
*      �������� :  ���һ��������nal��������ַ�����ȡ�������β0��������nal��
*      ����˵�� :  ���������ַ�������ַ������(�϶�������β0��stopbit)
*      ����ֵ   �� ��������
************************************************************************
*/
SINT32 H264_DecSlice(H264_CTX_S *pH264Ctx)
{
    SINT32 ret;
    UINT32 i;
    UINT32 Size0, Size1, Size, PrevNalSize;
	UINT8 *Addr0, *Addr1, *PrevNalAddr;
    SINT32 TotalMbs, MaxStreamThr;

    dprint(PRN_DBG, "\nSlice nal or IDR nal = %d\n", pH264Ctx->TotalNal - 1);

    pH264Ctx->CurrSlice.nal_unit_type = pH264Ctx->pCurrNal->nal_unit_type;
    pH264Ctx->CurrSlice.nal_ref_idc = pH264Ctx->pCurrNal->nal_ref_idc;

    ret = ProcessSliceHeaderFirstPart(pH264Ctx);
    if (H264_OK != ret)
    {
        dprint(PRN_ERROR, "sliceheader dec err\n");
        REPORT_STRMERR(pH264Ctx->ChanID, SLICE_HEADER_ERR);
        ClearCurrSlice(pH264Ctx);
        return H264_ERR;
    }

    ret = ProcessSliceHeaderSecondPart(pH264Ctx);
    if (H264_OK != ret)
    {
        dprint(PRN_ERROR, "sliceheader dec err\n");
        REPORT_STRMERR(pH264Ctx->ChanID, SLICE_HEADER_ERR);
        ClearCurrSlice(pH264Ctx);
        return H264_ERR;
    }

    /* ��Ӷڻ��������������ƶ�����vdm */
    // �ȼ�����ͷ���������������Ǵ�����������������������ռ��buffer�ĳ���,
    // ����ͷ������������ʹ�м����ɰ��ͷŵ�Ҳ��������������
    Size = 0;
    PrevNalAddr = 0;
    PrevNalSize = 0;
    for (i = 0; i < pH264Ctx->SliceParaNum; i++)
    {
        Addr0 = pH264Ctx->SlcInfo[i].pSliceNal->stream[0].streamBuffer;
        Size0 = (NULL != Addr0)? pH264Ctx->SlcInfo[i].pSliceNal->stream[0].bitstream_length: 0;
        Addr1 = pH264Ctx->SlcInfo[i].pSliceNal->stream[1].streamBuffer;
        Size1 = (NULL != Addr1)? pH264Ctx->SlcInfo[i].pSliceNal->stream[1].bitstream_length: 0;

        if (NULL != PrevNalAddr)
        {
            Size += (PrevNalAddr < Addr0)? ((Addr0 + Size0)- (PrevNalAddr+PrevNalSize)): Size0;
            Size += Size1;
        }
        else
        {
            Size += Size0 + Size1;
        }

        PrevNalAddr = (NULL != Addr1)? Addr1 : Addr0;
        PrevNalSize = (NULL != Addr1)? Size1 : Size0;
    }
    pH264Ctx->MaxBytesReceived = Size;

    //pH264Ctx->MaxBytesReceived��ֵ֮ǰΪ��ֵMAX_BYTES_START���ָ�Ϊw*h*1.5/3��z56361,20110428
    /* z56361, 2011-11-1, Q�ܷ���"ʯ��"��������Ϊ������ֵ̫С������������, ���Ըĳɱ���͸���ֱ��ò�ͬ����ֵ. */
    TotalMbs = pH264Ctx->CurrPic.pic_width_in_mb * pH264Ctx->CurrPic.pic_height_in_mb;
    if ( TotalMbs > 1620)
    {
        MaxStreamThr = TotalMbs * 128*2;
    }
    else
    {
        /* z6361,2011-11-23, ��������5x6����ʱ����TotalMbs*256��Ϊ��ֵ��Ȼ̫С����������. �ڳ�Сͼ����ʱ������ֵ
           Ӧ��Ӧ�Ӵ�ֻҪ��֤buf��ȫ����ֵ�����ܴ���ܼ�������. */
        MaxStreamThr = MAX(16*1024, (TotalMbs * 256 * 3 / 2));
    }

    if ( (pH264Ctx->SliceParaNum >= pH264Ctx->MaxSliceNum) || (pH264Ctx->MaxBytesReceived > MaxStreamThr) )
    {
        dprint(PRN_FATAL, "Start Reason: SliceParaNum, MaxBytesReceived = %d, %d(thr=%d)\n",
               pH264Ctx->SliceParaNum, pH264Ctx->MaxBytesReceived, MaxStreamThr);
        if (pH264Ctx->SliceParaNum >= pH264Ctx->MaxSliceNum)
        {
            REPORT_SLICE_NUM_OVER(pH264Ctx->ChanID, pH264Ctx->SliceParaNum, pH264Ctx->MaxSliceNum);
        }
        dprint(PRN_ERROR, "Too many slice or bitstream, err!\n");
        H264_ClearCurrPic(pH264Ctx);
        return H264_ERR;
    }
    else if (pH264Ctx->CurrSlice.new_pic_flag)
    {
        dprint(PRN_CTRL, "Start Reason: new_pic_flag\n");

        if (NALU_TYPE_IDR == pH264Ctx->CurrSlice.nal_unit_type)
        {
            pH264Ctx->IDRPicOfCurrPic = pH264Ctx->TotalPicNum;
        }
        pH264Ctx->SliceNumInPic = 0;
        pH264Ctx->TotalPicNum++;
        if (0 == pH264Ctx->CurrPic.structure)
        {
            pH264Ctx->TotalFrameNum++;
        }
        else
        {
            pH264Ctx->TotalFieldNum++;
        }

        ret = InitPic(pH264Ctx);
        if(H264_NOTDEC == ret)
        {
            return ret;
        }
        else if (H264_OK != ret)
        {
            dprint(PRN_ERROR, "init pic err, find next recover point or next valid sps, pps, or exit\n");
            return H264_ERR;
        }

        WritePicMsg(pH264Ctx);
    }

    PicTypeStatistic(pH264Ctx);

    ret = DecList(pH264Ctx);
    if (H264_OK != ret)
    {
        dprint(PRN_ERROR,"DecList error, ret=%d\n", ret);
        return H264_ERR;
    }

    ret = IsRefListWrong(pH264Ctx);
    if (H264_OK != ret)
    {
        dprint(PRN_ERROR, "dec list err.\n");
        ClearCurrSlice(pH264Ctx);
        return H264_ERR;
    }

    CalcStreamBits(pH264Ctx);

    WriteSliceMsg(pH264Ctx);
    ExitSlice(pH264Ctx);

    pH264Ctx->AllowStartDec = 1;
    pH264Ctx->SliceNumInPic++;

    return H264_OK;
}


/***********************************************************************************
* Function:    VOID Scaling_List(H264_CTX_S *pH264Ctx, SINT32 *scalingList, SINT32 sizeOfScalingList, UINT32 *UseDefaultScalingMatrix)
* Description: ������������ƴλ
* Input:         ��������ָ�룬�������С��ʹ��ȱʡָ��
* Output:      ȱʡʹ�ñ�ָ�븳ֵ
* Return:      ��
* Others:      ��
***********************************************************************************/
VOID Scaling_List(H264_CTX_S *pH264Ctx, UINT32 *scalingList_Org, UINT32 *scalingList, SINT32 sizeOfScalingList, UINT32 *UseDefaultScalingMatrix)
{
    SINT32 j;
    SINT32 delta_scale, lastScale, nextScale;
    SINT32 tmpscale;
    SINT32 w_posit, b_posit;
    UINT32 scanj;

    lastScale = 8;
    nextScale = 8;

    for (j=0; j<sizeOfScalingList; j++)
    {
        scanj = (sizeOfScalingList == 16) ? g_ZZ_SCAN[j] : g_ZZ_SCAN8[j];
        if (nextScale!=0)
        {
            delta_scale = se_v (pH264Ctx, "   : delta_sl   ");
            nextScale = (lastScale + delta_scale + 256) % 256;
            *UseDefaultScalingMatrix = (scanj==0 && nextScale==0);
        }

        tmpscale = (nextScale==0) ? lastScale:nextScale;
        scalingList_Org[scanj] = tmpscale;
        lastScale = tmpscale;

        //����ƴλ��λ��
        w_posit = scanj/4;
        b_posit = scanj%4;

        scalingList[w_posit] = (scalingList[w_posit] & (~(0xff << (8*b_posit)))) | ((tmpscale & 0xff) << (8*b_posit));
    }
}


/***********************************************************************************
* Function:    H264_DecPPS(H264_CTX_S *pH264Ctx)
* Description: ����PPS, ������ݴ���
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 H264_DecPPS(H264_CTX_S *pH264Ctx)
{
    static H264_PPS_S pps_tmp;
    UINT32 pic_parameter_set_id;
    UINT32 seq_parameter_set_id;

    pic_parameter_set_id = ue_v(pH264Ctx, "PPS: pic_parameter_set_id");
    if (pic_parameter_set_id > (pH264Ctx->MaxPpsNum-1))
    {
        dprint(PRN_ERROR, "pic_parameter_set_id out of range.\n");
        if (pic_parameter_set_id>(pH264Ctx->MaxPpsNum-1))
        {
            REPORT_PPS_NUM_OVER(pH264Ctx->ChanID, pic_parameter_set_id, pH264Ctx->MaxPpsNum);
        }
        // ���Կ���ǯλǿ��
        return H264_ERR;
    }

    seq_parameter_set_id = ue_v(pH264Ctx, "PPS: seq_parameter_set_id");
    if (seq_parameter_set_id > (pH264Ctx->MaxSpsNum-1))
    {
        dprint(PRN_ERROR, "seq_parameter_set_id out of range.\n");
        // ���Կ���ǯλǿ��, ǯλ֮��ע��sps�����Ե��ж�
        return H264_ERR;
    }
    else if (!(pH264Ctx->SPS[seq_parameter_set_id].is_valid))
    {
        dprint(PRN_ERROR, "SPS haven't decode.\n");
        return H264_ERR;
    }

    if (pH264Ctx->PPS[pic_parameter_set_id].is_valid)
    {
        pps_tmp.pic_parameter_set_id = pic_parameter_set_id;
        pps_tmp.seq_parameter_set_id = seq_parameter_set_id;

        if (H264_OK != ProcessPPS(pH264Ctx, &pps_tmp))
        {
            dprint(PRN_ERROR, "PPS decode error.\n");
            return H264_ERR;
        }

        if (H264_OK != PPSEqual(&pps_tmp, &(pH264Ctx->PPS[pic_parameter_set_id])))
        {
            pps_tmp.is_refreshed = 1;
            pps_tmp.is_valid = 1;
            memmove(&(pH264Ctx->PPS[pic_parameter_set_id]), &pps_tmp, sizeof(H264_PPS_S));
        }
    }
    else
    {
        pH264Ctx->PPS[pic_parameter_set_id].pic_parameter_set_id = pic_parameter_set_id;
        pH264Ctx->PPS[pic_parameter_set_id].seq_parameter_set_id = seq_parameter_set_id;

        if (H264_OK != ProcessPPS(pH264Ctx, &(pH264Ctx->PPS[pic_parameter_set_id])))
        {
            dprint(PRN_ERROR, "PPS decode error.\n");
            pH264Ctx->PPS[pic_parameter_set_id].is_refreshed = 1;
            pH264Ctx->PPS[pic_parameter_set_id].is_valid = 0;
            return H264_ERR;
        }
        else
        {
            pH264Ctx->PPS[pic_parameter_set_id].is_refreshed = 1;
            pH264Ctx->PPS[pic_parameter_set_id].is_valid = 1;
        }
    }

    return H264_OK;
}


/***********************************************************************************
* Function:    ProcessPPS(H264_CTX_S *pH264Ctx, H264_PPS_S *pPPS)
* Description: ����PPS, ������ݴ���
* Input:       ����������ʱ����������
* Output:      pPPS         ��PPS�����ݴ浽һ���ṹ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 ProcessPPS(H264_CTX_S *pH264Ctx, H264_PPS_S *pPPS)
{
    UINT32    residbits;
    UINT32    q4x4defaultflag[6];
    UINT32    q8x8defaultflag[2];
    UINT32    i;

    pPPS->entropy_coding_mode_flag               = u_1 (pH264Ctx, "PPS: entropy_coding_mode_flag"      );
    SE_ERR_CHECK;

    pPPS->pic_order_present_flag                 = u_1 (pH264Ctx, "PPS: pic_order_present_flag"        );
    SE_ERR_CHECK;

    pPPS->num_slice_groups_minus1                = ue_v(pH264Ctx, "PPS: num_slice_groups_minus1"       );
    if ((0!=pPPS->num_slice_groups_minus1) || pH264Ctx->SeErrFlg)  // num_slice_groups_minus1=0ʱ������Ƭ����
    {
        dprint(PRN_ERROR, "Baseline stream with FMO, not support.\n");
        REPORT_UNSUPPORT(pH264Ctx->ChanID);
        return H264_ERR;
    }

    pPPS->num_ref_idx_l0_active_minus1           = ue_v(pH264Ctx, "PPS: num_ref_idx_l0_active_minus1"  );
    if ((pPPS->num_ref_idx_l0_active_minus1>31) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_ERROR, "num_ref_idx_l0_active_minus1 out of range.\n");

        // ���Կ���ǯλǿ��
        return H264_ERR;
    }

    pPPS->num_ref_idx_l1_active_minus1           = ue_v(pH264Ctx, "PPS: num_ref_idx_l1_active_minus1"  );
    if ((pPPS->num_ref_idx_l1_active_minus1>31) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_ERROR, "num_ref_idx_l1_active_minus1 out of range.\n");

        // ���Կ���ǯλǿ��
        return H264_ERR;
    }

    pPPS->weighted_pred_flag                     = u_1 (pH264Ctx, "PPS: weighted prediction flag"      );
    SE_ERR_CHECK;

    pPPS->weighted_bipred_idc                    = u_v (pH264Ctx, 2, "PPS: weighted_bipred_idc"        );
    if (pPPS->weighted_bipred_idc >= 3)
    {
        dprint(PRN_ERROR, "weighted_bipred_idc out of range\n");

        // ���Կ���ǯλǿ��
        return H264_ERR;
    }

    pPPS->pic_init_qp_minus26                    = se_v(pH264Ctx, "PPS: pic_init_qp_minus26"           );
    se_v(pH264Ctx,"PPS: pic_init_qs_minus26"           );
    pPPS->chroma_qp_index_offset                 = se_v(pH264Ctx, "PPS: chroma_qp_index_offset"        );
    pPPS->deblocking_filter_control_present_flag = u_1 (pH264Ctx, "PPS: deblocking_filter_control_present_flag");
    pPPS->constrained_intra_pred_flag            = u_1 (pH264Ctx, "PPS: constrained_intra_pred_flag"   );
    pPPS->redundant_pic_cnt_present_flag         = u_1 (pH264Ctx, "PPS: redundant_pic_cnt_present_flag");

    // main profile not support redundant_pic_cnt or not support redundant_pic_cnt more than 0 ?
    if (0 != pPPS->redundant_pic_cnt_present_flag)
    {
        dprint(PRN_ERROR, "redundant pic not support when find redundant slice later, exit\n");
    }

    //init para may not present in stream
    pPPS->second_chroma_qp_index_offset = pPPS->chroma_qp_index_offset;
    pPPS->transform_8x8_mode_flag = 0;
    pPPS->pic_scaling_matrix_present_flag = 0;

    //more_rbsp_data, residue bits more than trail zero
    residbits = BsResidBits(&(pH264Ctx->BS));
    if (residbits >= (pH264Ctx->pCurrNal->nal_trail_zero_bit_num + 3) )
    {
        pPPS->transform_8x8_mode_flag = u_1 (pH264Ctx, "PPS: transform_8x8_mode_flag");
        pPPS->pic_scaling_matrix_present_flag = u_1 (pH264Ctx, "PPS: pic_scaling_matrix_present_flag");
        if (1 == pPPS->pic_scaling_matrix_present_flag)
        {
            for (i = 0; i < 6; i++)
            {
                q4x4defaultflag[i] = u_1 (pH264Ctx, "SPS: seq_scaling_list_present_flag");
                pPPS->pic_scaling_list_present_flag[i] = q4x4defaultflag[i];
                if (1 == q4x4defaultflag[i])
                {
                    Scaling_List(pH264Ctx, pPPS->ScalingList4x4[i], pPPS->qmatrix4x4[i], 16, &q4x4defaultflag[i]);
                    if (1 == q4x4defaultflag[i])
                    {
                        memcpy(pPPS->qmatrix4x4[i], (i<3) ? quant_intra_default : quant_inter_default, 4*4);
                    }
                }
                else
                {
                    if (0 == i || 3 == i)
                    {
                        if (1 == pH264Ctx->SPS[pPPS->seq_parameter_set_id].seq_scaling_matrix_present_flag)
                        {
                            //copy frome sps
                            memcpy(pPPS->qmatrix4x4[i],  pH264Ctx->SPS[pPPS->seq_parameter_set_id].qmatrix4x4[i],  4*4);
                        }
                        else
                        {
                            memcpy(pPPS->qmatrix4x4[i], (0 == i) ? quant_intra_default : quant_inter_default, 4*4);
                        }
                    }
                    else
                    {
                        memcpy(pPPS->qmatrix4x4[i],  pPPS->qmatrix4x4[i-1], 4*4);
                    }
                }
            }

            for (i = 0; i < 2; i++)
            {
                if (1 == pPPS->transform_8x8_mode_flag)
                {
                    q8x8defaultflag[i] = u_1 (pH264Ctx, "PPS: pic_scaling_list_present_flag");
                }
                else
                {
                    q8x8defaultflag[i]  = 0;
                }
                pPPS->pic_scaling_list_present_flag[6+i] = q8x8defaultflag[i];
                if (1 == q8x8defaultflag[i])
                {
                    Scaling_List(pH264Ctx, pPPS->ScalingList8x8[i], pPPS->qmatrix8x8[i], 64, &q8x8defaultflag[i]);
                    if (1 == q8x8defaultflag[i])
                    {
                        memcpy(pPPS->qmatrix8x8[i],  (0 == i) ? quant8_intra_default : quant8_inter_default, 4*16);
                    }
                }
                else
                {
                    if (1 == pH264Ctx->SPS[pPPS->seq_parameter_set_id].seq_scaling_matrix_present_flag)
                    {
                        //fall back B, copy frome sps
                        memcpy(pPPS->qmatrix8x8[i],  pH264Ctx->SPS[pPPS->seq_parameter_set_id].qmatrix8x8[i], 4*16);
                    }
                    else
                    {
                        memcpy(pPPS->qmatrix8x8[i],  (0 == i) ? quant8_intra_default : quant8_inter_default, 4*16);
                    }
                }
            }
        }

        pPPS->second_chroma_qp_index_offset = se_v (pH264Ctx, "PPS: second_chroma_qp_index_offset");
        if ((12 < pPPS->second_chroma_qp_index_offset) ||(-12 > pPPS->second_chroma_qp_index_offset)|| pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "second_chroma_qp_index_offset out of range.\n");
            return H264_ERR;
        }
    }

    return H264_OK;
}


/***********************************************************************************
* Function:    PPSEqual(H264_PPS_S *pPPS1, H264_PPS_S *pPPS2)
* Description: �Ƚ�����SPS�Ƿ����
* Input:       pPPS1, pPPS2  �������Ƚϵ�PPS
* Output:      ��
* Return:      DEC_TRUE   ����PPS����ȵ�
*              DEC_FALSE  ����PPS�����
* Others:      ��
***********************************************************************************/
SINT32 PPSEqual(H264_PPS_S *pPPS1, H264_PPS_S *pPPS2)
{
    UINT32 i, j;
    SINT32 equal = 1;
    SINT32 ret;

    CHECK_NULL_PTR_ReturnValue(pPPS1, H264_ERR);
    CHECK_NULL_PTR_ReturnValue(pPPS2, H264_ERR);

    equal &= (pPPS1->pic_parameter_set_id == pPPS2->pic_parameter_set_id);
    equal &= (pPPS1->seq_parameter_set_id == pPPS2->seq_parameter_set_id);
    equal &= (pPPS1->entropy_coding_mode_flag == pPPS2->entropy_coding_mode_flag);
    equal &= (pPPS1->pic_order_present_flag == pPPS2->pic_order_present_flag);
    equal &= (pPPS1->num_ref_idx_l0_active_minus1 == pPPS2->num_ref_idx_l0_active_minus1);
    equal &= (pPPS1->num_ref_idx_l1_active_minus1 == pPPS2->num_ref_idx_l1_active_minus1);
    equal &= (pPPS1->weighted_pred_flag == pPPS2->weighted_pred_flag);
    equal &= (pPPS1->weighted_bipred_idc == pPPS2->weighted_bipred_idc);
    equal &= (pPPS1->pic_init_qp_minus26 == pPPS2->pic_init_qp_minus26);
    equal &= (pPPS1->chroma_qp_index_offset == pPPS2->chroma_qp_index_offset);
    equal &= (pPPS1->deblocking_filter_control_present_flag == pPPS2->deblocking_filter_control_present_flag);
    equal &= (pPPS1->constrained_intra_pred_flag == pPPS2->constrained_intra_pred_flag);

    //Fidelity Range Extensions Stuff
    //It is initialized to zero, so should be ok to check all the time.
    equal &= (pPPS1->transform_8x8_mode_flag == pPPS2->transform_8x8_mode_flag);
    equal &= (pPPS1->pic_scaling_matrix_present_flag == pPPS2->pic_scaling_matrix_present_flag);
    if (pPPS1->pic_scaling_matrix_present_flag)
    {
        for (i = 0; i < (6 + (pPPS1->transform_8x8_mode_flag << 1) ); i++)
        {
            equal &= (pPPS1->pic_scaling_list_present_flag[i] == pPPS2->pic_scaling_list_present_flag[i]);
            if (pPPS1->pic_scaling_list_present_flag[i])
            {
                if (i < 6)
                {
                    for (j = 0; j < 16; j++)
                    {
                        equal &= (pPPS1->ScalingList4x4[i][j] == pPPS2->ScalingList4x4[i][j]);
                    }
                }
                else
                {
                    for (j = 0; j < 64; j++)
                    {
                        equal &= (pPPS1->ScalingList8x8[i-6][j] == pPPS2->ScalingList8x8[i-6][j]);
                    }
                }
            }
        }
    }
    equal &= (pPPS1->second_chroma_qp_index_offset == pPPS2->second_chroma_qp_index_offset);

    ret = equal ? H264_OK : H264_ERR;

    return ret;
}


/***********************************************************************************
* Function:    vui_parameters(H264_CTX_S *pH264Ctx)
* Description: ����VUI�﷨Ԫ��, ������ݴ���
* Input:       ����������ʱ����������
* Output:      pSPS        ֻ��Ӱ�������е�һ��Ԫ�ؽṹ���VUI����
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 vui_parameters(H264_CTX_S *pH264Ctx, VUI_SEQ_PARAMETERS_S *pVUI)
{
    pVUI->aspect_ratio_info_present_flag = u_1(pH264Ctx, "VUI: aspect_ratio_info_present_flag");
    if (pVUI->aspect_ratio_info_present_flag)
    {
        pVUI->aspect_ratio_idc = u_v(pH264Ctx,  8, "VUI: aspect_ratio_idc");
        if (255==pVUI->aspect_ratio_idc)
        {
            pVUI->sar_width    = u_v(pH264Ctx, 16, "VUI: sar_width" );
            pVUI->sar_height   = u_v(pH264Ctx, 16, "VUI: sar_height");
        }
    }
    else
    {
        pVUI->aspect_ratio_idc = 0;
    }

    pVUI->overscan_info_present_flag            = u_1(pH264Ctx, "VUI: overscan_info_present_flag");
    if (pVUI->overscan_info_present_flag)
    {
        pVUI->overscan_appropriate_flag         = u_1(pH264Ctx, "VUI: overscan_appropriate_flag");
    }

    pVUI->video_signal_type_present_flag        = u_1(pH264Ctx, "VUI: video_signal_type_present_flag");
    if (pVUI->video_signal_type_present_flag)
    {
        pVUI->video_format                      = u_v(pH264Ctx, 3, "VUI: video_format"               );
        pVUI->video_full_range_flag             = u_1(pH264Ctx, "VUI: video_full_range_flag"         );
        pVUI->colour_description_present_flag   = u_1(pH264Ctx, "VUI: color_description_present_flag");
        if (pVUI->colour_description_present_flag)
        {
            pVUI->colour_primaries              = u_v(pH264Ctx, 8, "VUI: colour_primaries"        );
            pVUI->transfer_characteristics      = u_v(pH264Ctx, 8, "VUI: transfer_characteristics");
            pVUI->matrix_coefficients           = u_v(pH264Ctx, 8, "VUI: matrix_coefficients"     );
        }
    }
    else
    {
        pVUI->video_format = 5;
    }
    pVUI->chroma_location_info_present_flag       = u_1  (pH264Ctx, "VUI: chroma_loc_info_present_flag");
    if (pVUI->chroma_location_info_present_flag)
    {
        pVUI->chroma_sample_loc_type_top_field    = ue_v (pH264Ctx, "VUI: chroma_sample_loc_type_top_field"   );
        pVUI->chroma_sample_loc_type_bottom_field = ue_v (pH264Ctx, "VUI: chroma_sample_loc_type_bottom_field");
    }
    pVUI->timing_info_present_flag                = u_1  (pH264Ctx, "VUI: timing_info_present_flag"       );
    if (pVUI->timing_info_present_flag)
    {
        pVUI->num_units_in_tick                   = u_v  (pH264Ctx, 32, "VUI: num_units_in_tick"          );
        pVUI->time_scale                          = u_v  (pH264Ctx, 32, "VUI: time_scale"                 );
        pVUI->fixed_frame_rate_flag               = u_1  (pH264Ctx,     "VUI: fixed_frame_rate_flag"      );
        if ( (1 == pVUI->fixed_frame_rate_flag) && (0 != pVUI->num_units_in_tick) )
        {
            pH264Ctx->frame_rate = pVUI->time_scale / pVUI->num_units_in_tick / 2;
        }
    }
    pVUI->nal_hrd_parameters_present_flag         = u_1  (pH264Ctx, "VUI: nal_hrd_parameters_present_flag");
    if (pVUI->nal_hrd_parameters_present_flag)
    {
        UINT32 SchedSelIdx;
        HRD_PARAMETERS_S *hrd = &(pVUI->nal_hrd_parameters);

        hrd->cpb_cnt_minus1                                       = ue_v (pH264Ctx,    "VUI: cpb_cnt_minus1");
        hrd->bit_rate_scale                                       = u_v  (pH264Ctx, 4, "VUI: bit_rate_scale");
        hrd->cpb_size_scale                                       = u_v  (pH264Ctx, 4, "VUI: cpb_size_scale");

        if ((hrd->cpb_cnt_minus1>31) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");
            return H264_ERR;
        }

        for (SchedSelIdx=0; SchedSelIdx<=hrd->cpb_cnt_minus1; SchedSelIdx++)
        {
            hrd->bit_rate_value_minus1[SchedSelIdx]               = ue_v (pH264Ctx, "VUI: bit_rate_value_minus1");
            hrd->cpb_size_value_minus1[SchedSelIdx]               = ue_v (pH264Ctx, "VUI: cpb_size_value_minus1");
            hrd->cbr_flag[SchedSelIdx]                            = u_1  (pH264Ctx, "VUI: cbr_flag"             );
        }

        hrd->initial_cpb_removal_delay_length_minus1              = u_v  (pH264Ctx, 5, "VUI: initial_cpb_removal_delay_length_minus1");
        hrd->cpb_removal_delay_length_minus1                      = u_v  (pH264Ctx, 5, "VUI: cpb_removal_delay_length_minus1"        );
        hrd->dpb_output_delay_length_minus1                       = u_v  (pH264Ctx, 5, "VUI: dpb_output_delay_length_minus1"         );
        hrd->time_offset_length                                   = u_v  (pH264Ctx, 5, "VUI: time_offset_length"                     );
    }
    pVUI->vcl_hrd_parameters_present_flag                         = u_1  (pH264Ctx, "VUI: vcl_hrd_parameters_present_flag"           );
    if (pVUI->vcl_hrd_parameters_present_flag)
    {
        UINT32 SchedSelIdx;
        HRD_PARAMETERS_S *hrd = &(pVUI->vcl_hrd_parameters);

        hrd->cpb_cnt_minus1                                       = ue_v (pH264Ctx,    "VUI: cpb_cnt_minus1");
        hrd->bit_rate_scale                                       = u_v  (pH264Ctx, 4, "VUI: bit_rate_scale");
        hrd->cpb_size_scale                                       = u_v  (pH264Ctx, 4, "VUI: cpb_size_scale");
        if ((hrd->cpb_cnt_minus1>31) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");
            return H264_ERR;
        }
        for (SchedSelIdx=0; SchedSelIdx<=hrd->cpb_cnt_minus1; SchedSelIdx++)
        {
            hrd->bit_rate_value_minus1[SchedSelIdx]               = ue_v (pH264Ctx, "VUI: bit_rate_value_minus1");
            hrd->cpb_size_value_minus1[SchedSelIdx]               = ue_v (pH264Ctx, "VUI: cpb_size_value_minus1");
            hrd->cbr_flag[SchedSelIdx]                            = u_1  (pH264Ctx, "VUI: cbr_flag"             );
        }

        hrd->initial_cpb_removal_delay_length_minus1              = u_v  (pH264Ctx, 5, "VUI: initial_cpb_removal_delay_length_minus1");
        hrd->cpb_removal_delay_length_minus1                      = u_v  (pH264Ctx, 5, "VUI: cpb_removal_delay_length_minus1"        );
        hrd->dpb_output_delay_length_minus1                       = u_v  (pH264Ctx, 5, "VUI: dpb_output_delay_length_minus1"         );
        hrd->time_offset_length                                   = u_v  (pH264Ctx, 5, "VUI: time_offset_length"                     );
    }
    if (pVUI->nal_hrd_parameters_present_flag || pVUI->vcl_hrd_parameters_present_flag)
    {
        pVUI->low_delay_hrd_flag                      = u_1 (pH264Ctx, "VUI: low_delay_hrd_flag"        );
    }
    pVUI->pic_struct_present_flag                     = u_1 (pH264Ctx, "VUI: pic_struct_present_flag"   );
    pVUI->bitstream_restriction_flag                  = u_1 (pH264Ctx, "VUI: bitstream_restriction_flag");
    if (pVUI->bitstream_restriction_flag)
    {
        pVUI->motion_vectors_over_pic_boundaries_flag = u_1 (pH264Ctx, "VUI: motion_vectors_over_pic_boundaries_flag");
        pVUI->max_bytes_per_pic_denom                 = ue_v(pH264Ctx, "VUI: max_bytes_per_pic_denom"                );
        pVUI->max_bits_per_mb_denom                   = ue_v(pH264Ctx, "VUI: max_bits_per_mb_denom"                  );
        pVUI->log2_max_mv_length_horizontal           = ue_v(pH264Ctx, "VUI: log2_max_mv_length_horizontal"          );
        pVUI->log2_max_mv_length_vertical             = ue_v(pH264Ctx, "VUI: log2_max_mv_length_vertical"            );
        pVUI->num_reorder_frames                      = ue_v(pH264Ctx, "VUI: num_reorder_frames"                     );
        pVUI->max_dec_frame_buffering                 = ue_v(pH264Ctx, "VUI: max_dec_frame_buffering"                );
        SE_ERR_CHECK;
    }
    SE_ERR_CHECK;

    return H264_OK;
}


/***********************************************************************************
* Function:    SPSEqual(H264_SPS_S *pSPS1, H264_SPS_S *pSPS2)
* Description: �Ƚ�����SPS�Ƿ����
* Input:       pSPS1, pSPS2  �������Ƚϵ�SPS
* Output:      ��
* Return:      DEC_TRUE   ����SPS����ȵ�
*              DEC_FALSE  ����SPS�����
* Others:      ��
***********************************************************************************/
SINT32 SPSEqual(H264_SPS_S *pSPS1, H264_SPS_S *pSPS2)
{
    UINT32 i, j;
    SINT32 equal = 1;
    SINT32 ret;

    CHECK_NULL_PTR_ReturnValue(pSPS1, H264_ERR);
    CHECK_NULL_PTR_ReturnValue(pSPS2, H264_ERR);

    equal &= (pSPS1->profile_idc == pSPS2->profile_idc);
    equal &= (pSPS1->constrained_set0_flag == pSPS2->constrained_set0_flag);
    equal &= (pSPS1->constrained_set1_flag == pSPS2->constrained_set1_flag);
    equal &= (pSPS1->constrained_set2_flag == pSPS2->constrained_set2_flag);
    equal &= (pSPS1->level_idc == pSPS2->level_idc);
    equal &= (pSPS1->seq_parameter_set_id == pSPS2->seq_parameter_set_id);

    equal &= (pSPS1->seq_scaling_matrix_present_flag == pSPS2->seq_scaling_matrix_present_flag);
    if (pSPS1->seq_scaling_matrix_present_flag)
    {
        for (i = 0; i < (6 + 2); i++)
        {
            equal &= (pSPS1->seq_scaling_list_present_flag[i] == pSPS2->seq_scaling_list_present_flag[i]);
            if (pSPS1->seq_scaling_list_present_flag[i])
            {
                if (i < 6)
                {
                    for (j = 0; j < 16; j++)
                    {
                        equal &= (pSPS1->ScalingList4x4[i][j] == pSPS2->ScalingList4x4[i][j]);
                    }
                }
                else
                {
                    for (j = 0; j < 64; j++)
                    {
                        equal &= (pSPS1->ScalingList8x8[i-6][j] == pSPS2->ScalingList8x8[i-6][j]);
                    }
                }
            }
        }
    }

    equal &= (pSPS1->log2_max_frame_num_minus4 == pSPS2->log2_max_frame_num_minus4);
    equal &= (pSPS1->pic_order_cnt_type == pSPS2->pic_order_cnt_type);

    switch (pSPS1->pic_order_cnt_type)
    {
    case 0:
        equal &= (pSPS1->log2_max_pic_order_cnt_lsb_minus4 == pSPS2->log2_max_pic_order_cnt_lsb_minus4);
        break;
    case 1:
        equal &= (pSPS1->delta_pic_order_always_zero_flag == pSPS2->delta_pic_order_always_zero_flag);
        equal &= (pSPS1->offset_for_non_ref_pic == pSPS2->offset_for_non_ref_pic);
        equal &= (pSPS1->offset_for_top_to_bottom_field == pSPS2->offset_for_top_to_bottom_field);
        equal &= (pSPS1->num_ref_frames_in_pic_order_cnt_cycle == pSPS2->num_ref_frames_in_pic_order_cnt_cycle);
        for (i=0; i<pSPS1->num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            equal &= (pSPS1->offset_for_ref_frame[i] == pSPS2->offset_for_ref_frame[i]);
        }
        break;
    default:
        break;
    }

    equal &= (pSPS1->num_ref_frames == pSPS2->num_ref_frames);
    equal &= (pSPS1->gaps_in_frame_num_value_allowed_flag == pSPS2->gaps_in_frame_num_value_allowed_flag);
    equal &= (pSPS1->pic_width_in_mbs_minus1 == pSPS2->pic_width_in_mbs_minus1);
    equal &= (pSPS1->pic_height_in_map_units_minus1 == pSPS2->pic_height_in_map_units_minus1);
    equal &= (pSPS1->frame_mbs_only_flag == pSPS2->frame_mbs_only_flag);

    if (!pSPS1->frame_mbs_only_flag)
    {
        equal &= (pSPS1->mb_adaptive_frame_field_flag == pSPS2->mb_adaptive_frame_field_flag);
    }
    equal &= (pSPS1->direct_8x8_inference_flag == pSPS2->direct_8x8_inference_flag);

    equal &= (pSPS1->frame_cropping_flag == pSPS2->frame_cropping_flag);
    if (pSPS1->frame_cropping_flag)
    {
        equal &= (pSPS1->frame_crop_left_offset == pSPS2->frame_crop_left_offset);
        equal &= (pSPS1->frame_crop_right_offset == pSPS2->frame_crop_right_offset);
        equal &= (pSPS1->frame_crop_top_offset == pSPS2->frame_crop_top_offset);
        equal &= (pSPS1->frame_crop_bottom_offset == pSPS2->frame_crop_bottom_offset);
    }
    equal &= (pSPS1->vui_parameters_present_flag == pSPS2->vui_parameters_present_flag);

    ret = equal ? H264_OK : H264_ERR;

    return ret;
}


/***********************************************************************************
* Function:      SINT32  H264GetDar(SINT32 ar_idc, SINT32 sar_width, SINT32 sar_height, SINT32 img_width, SINT32 img_height)
* Description:  ���� display_aspec_ratio
* Input:           sar width, sar height
* Output:
* Return:        aspec_ratio
* Others:      ��
***********************************************************************************/
#define CONST_DAR_235_100  (235*1024/100)
#define CONST_DAR_221_100  (221*1024/100)
#define CONST_DAR_16_9  (16*1024/9)
#define CONST_DAR_4_3   (4*1024/3)
SINT32  H264GetDar(SINT32 ar_idc, SINT32 sar_width, SINT32 sar_height, SINT32 img_width, SINT32 img_height)
{
    static SINT32 SarTable[][2] =
    {
        { 1, 1},{ 1, 1},{12,11},{10,11},{16,11}, {40,33},{24,11},{20,11},{32,11},{80,33},
        {18, 11}, {15, 11}, {64, 33}, {160, 99}, {4, 3}, { 3, 2}, { 2, 1}
    };
    SINT32 dar, final_dar = DAR_UNKNOWN;

    if (ar_idc == 0 || (ar_idc > 16 && ar_idc != 255) || ((sar_width == 0 || sar_height == 0) && (ar_idc == 255)))
    {
        final_dar = DAR_UNKNOWN;
    }
    /* z56361, 2011-11-21.
       sample_aspect_ratio = 1:1������ζ����ʾ��߱ȵ���ͼ��Ŀ��:�߶ȣ����·�֧�����޴�. ��ĿǰVO��1:1���Ϊ��Ч��
       ���������������ڣ���ʹ����ʾЧ�����ɿ�.
       ���ڸ�������VO�������Ķ�������ʹ����Ч�����ı������ͻ�Ͷ��. δ��VO��Կ�߱���һ�γ����ع���������ʱ������
       ar_idc==1�ķ�֧ȥ���������VO�����µ���������. */
    else if (ar_idc == 1)
    {
        final_dar = DAR_IMG_SIZE;
    }
    else
    {
        if (ar_idc != 255)
        {
            sar_width  = SarTable[ar_idc][0];
            sar_height = SarTable[ar_idc][1];
        }
        //dar = (sar_width * img_width * 1024) / (sar_height * img_height);
        /* 20131012: l165842 ����ļ��㷽���п��ܵ�����ֵ(sar_width * img_width * 1024)
           ��������ս�����SINT32���͵�daֵΪ -1 */
        dar = (((sar_width * 1024) / (sar_height)) * ((img_width * 1024) / (img_height))) / 1024;

        if (ABS(dar - CONST_DAR_235_100) < ABS(dar - CONST_DAR_221_100))
        {
            final_dar = DAR_235_100;
        }
        else if (ABS(dar - CONST_DAR_221_100) < ABS(dar - CONST_DAR_16_9))
        {
            final_dar = DAR_221_100;
        }
        else if (ABS(dar - CONST_DAR_16_9) < ABS(dar - CONST_DAR_4_3))
        {
            final_dar = DAR_16_9;
        }
        else
        {
            final_dar = DAR_4_3;
        }
    }

    dprint(PRN_DBG, "dar=%d\n", final_dar);

    return final_dar;
}

/***********************************************************************************
* Function:    ProcessSPS(H264_CTX_S *pH264Ctx, H264_SPS_S *pSPS)
* Description: ����SPS��VUI��Ϣ, ������ݴ���
* Input:       ����������ʱ����������
* Output:      pSPS         �ݴ������SPS��VUI������һ���ṹ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 ProcessSPS(H264_CTX_S *pH264Ctx, H264_SPS_S *pSPS)
{
    UINT32 i;
    UINT32 frame_cropping_flag;
    UINT32 frame_cropping_rect_left_offset;
    UINT32 frame_cropping_rect_right_offset;
    UINT32 frame_cropping_rect_top_offset;
    UINT32 frame_cropping_rect_bottom_offset;
    UINT32 size;
    UINT32 pic_size;
    UINT32 PicWidthInSmp, PicHeightInSmp;
    SINT32 CropW, CropH;
    UINT32 temp;
    UINT32 qpprime_y_zero_trans_bypass_flag;
    UINT32 q4x4defaultflag[6];
    UINT32 q8x8defaultflag[2];

    CHECK_NULL_PTR_ReturnValue(pSPS, H264_ERR);

    if (HIGHPROFILE == pSPS->profile_idc
	 || HIGH10P == pSPS->profile_idc
	 || HIGH422P== pSPS->profile_idc
	 || HIGH444P== pSPS->profile_idc
	 || SCALABLE_BASELINE_PROFILE == pSPS->profile_idc
	 || SCALABLE_HIGH_PROFILE == pSPS->profile_idc)
    {
        //high profile dec chroma_format_idc
        pSPS->chroma_format_idc   = ue_v (pH264Ctx, "SPS: chroma_format_idc");
        if (((0 != pSPS->chroma_format_idc) && (1 !=pSPS->chroma_format_idc)) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSPS->chroma_format_idc out of range.\n");
            return H264_ERR;
        }
        pSPS->bit_depth_luma                  = ue_v (pH264Ctx, "SPS: bit_depth_luma_minus8") + 8;
        if (8 != pSPS->bit_depth_luma || pH264Ctx->SeErrFlg)
        {
            // 3798MV100 ֻ֧��8bit
            dprint(PRN_ERROR, "bit_depth_luma(%d) not equal 8.\n", pSPS->bit_depth_luma);
            REPORT_UNSUPPORT_SPEC(pH264Ctx->ChanID, SPEC_BIT_DEPTH, pSPS->bit_depth_luma);
            //return H264_ERR;

        }
        pSPS->bit_depth_chroma                = ue_v (pH264Ctx, "SPS: bit_depth_chroma_minus8") + 8;
        if (8 !=  pSPS->bit_depth_chroma || pH264Ctx->SeErrFlg)
        {
            // 3798MV100 ֻ֧��8bit
            dprint(PRN_ERROR, "bit_depth_chroma(%d) not equal 8.\n", pSPS->bit_depth_chroma);
            REPORT_UNSUPPORT_SPEC(pH264Ctx->ChanID, SPEC_BIT_DEPTH, pSPS->bit_depth_chroma);
            //return H264_ERR;
        }
        qpprime_y_zero_trans_bypass_flag    = u_1  (pH264Ctx, "SPS: qpprime_y_zero_trans_bypass_flag");
        if (0 != qpprime_y_zero_trans_bypass_flag || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "qpprime_y_zero_trans_bypass_flag not equal 0.\n");
            return H264_ERR;
        }

        pSPS->seq_scaling_matrix_present_flag        = u_1  (pH264Ctx, "SPS: seq_scaling_matrix_present_flag");
        if (1 == pSPS->seq_scaling_matrix_present_flag)
        {
            for (i = 0; i < 6; i++)
            {
                q4x4defaultflag[i] = u_1  (pH264Ctx, "SPS: seq_scaling_list_present_flag");
                pSPS->seq_scaling_list_present_flag[i] = q4x4defaultflag[i];
                if (1 == q4x4defaultflag[i])
                {
                    Scaling_List(pH264Ctx, pSPS->ScalingList4x4[i], pSPS->qmatrix4x4[i], 16, &q4x4defaultflag[i]);
                    if (1 == q4x4defaultflag[i])
                    {
                        memcpy(pSPS->qmatrix4x4[i], (i<3)? quant_intra_default:quant_inter_default, 4*4);
                    }
                }
                else
                {
                    if (0 == i || 3 == i)
                    {
                        memcpy(pSPS->qmatrix4x4[i], (i == 0) ? quant_intra_default : quant_inter_default, 4*4);
                    }
                    else
                    {
                        memcpy(pSPS->qmatrix4x4[i],  pSPS->qmatrix4x4[i-1], 4*4);
                    }
                }
            }

            for (i = 0; i < 2; i++)
            {
                q8x8defaultflag[i] = u_1  (pH264Ctx, "SPS: seq_scaling_list_present_flag");
                pSPS->seq_scaling_list_present_flag[6+i] = q8x8defaultflag[i];
                if (1 == q8x8defaultflag[i])
                {
                    Scaling_List(pH264Ctx, pSPS->ScalingList8x8[i], pSPS->qmatrix8x8[i], 64, &q8x8defaultflag[i]);
                    if (1 == q8x8defaultflag[i] )
                    {
                        memcpy(pSPS->qmatrix8x8[i],  (0 == i) ? quant8_intra_default : quant8_inter_default, 4*16);
                    }
                }
                else
                {
                    memcpy(pSPS->qmatrix8x8[i],  (0 == i) ? quant8_intra_default : quant8_inter_default, 4*16);
                }
            }
        }
    }
    else
    {
        pSPS->chroma_format_idc = 1;
        pSPS->seq_scaling_matrix_present_flag = 0;
    }

    if  (0 == pSPS->seq_scaling_matrix_present_flag)
    {
        //add qmatrix4x4[][], qmatrix8x8[][]
    }

    pSPS->log2_max_frame_num_minus4 = ue_v(pH264Ctx, "SPS: log2_max_frame_num_minus4");
    if ((pSPS->log2_max_frame_num_minus4>12) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pSPS->log2_max_frame_num_minus4 out of range.\n");
        return H264_ERR;
    }

    pSPS->pic_order_cnt_type = ue_v(pH264Ctx, "SPS: pic_order_cnt_type");
    if ((pSPS->pic_order_cnt_type>2) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pSPS->pic_order_cnt_type out of range.\n");
        return H264_ERR;
    }

    if (0==pSPS->pic_order_cnt_type)
    {
        pSPS->log2_max_pic_order_cnt_lsb_minus4 = ue_v(pH264Ctx, "SPS: log2_max_pic_order_cnt_lsb_minus4");
        if ((pSPS->log2_max_pic_order_cnt_lsb_minus4>12) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSPS->log2_max_pic_order_cnt_lsb_minus4 out of range.\n");
            return H264_ERR;
        }
    }
    else if (1==pSPS->pic_order_cnt_type)
    {
        pSPS->delta_pic_order_always_zero_flag = u_1(pH264Ctx, "SPS: delta_pic_order_always_zero_flag");
        SE_ERR_CHECK;

        pSPS->offset_for_non_ref_pic = se_v(pH264Ctx, "SPS: offset_for_non_ref_pic");
        SE_ERR_CHECK;

        pSPS->offset_for_top_to_bottom_field = se_v(pH264Ctx, "SPS: offset_for_top_to_bottom_field");
        SE_ERR_CHECK;

        pSPS->num_ref_frames_in_pic_order_cnt_cycle = ue_v(pH264Ctx, "SPS: num_ref_frames_in_pic_order_cnt_cycle");
        if ((pSPS->num_ref_frames_in_pic_order_cnt_cycle>255) || pH264Ctx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSPS->num_ref_frames_in_pic_order_cnt_cycle out of range.\n");
            return H264_ERR;
        }

        for (i=0; i<pSPS->num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            pSPS->offset_for_ref_frame[i] = se_v(pH264Ctx, "SPS: offset_for_ref_frame[i]");
            SE_ERR_CHECK;
        }
        SE_ERR_CHECK;
    }

    pSPS->num_ref_frames = ue_v(pH264Ctx, "SPS: num_ref_frames");
    SE_ERR_CHECK;

    pSPS->gaps_in_frame_num_value_allowed_flag = u_1(pH264Ctx, "SPS: gaps_in_frame_num_value_allowed_flag");
    SE_ERR_CHECK;

    pSPS->pic_width_in_mbs_minus1 = ue_v(pH264Ctx, "SPS: pic_width_in_mbs_minus1");
    if (((pSPS->pic_width_in_mbs_minus1+1)<2) || ((pSPS->pic_width_in_mbs_minus1+1)>=MAX_IMG_WIDTH_IN_MB) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pSPS->pic_width_in_mbs_minus1 out of range(=%d).\n", pSPS->pic_width_in_mbs_minus1);
        return H264_ERR;
    }

    pSPS->pic_height_in_map_units_minus1 = ue_v(pH264Ctx, "SPS: pic_height_in_map_units_minus1");
    SE_ERR_CHECK;

    pSPS->frame_mbs_only_flag = u_1(pH264Ctx, "SPS: frame_mbs_only_flag");
    SE_ERR_CHECK;
    temp = (1+(!pSPS->frame_mbs_only_flag));
    if ( 1 != temp && 2 != temp)
    {
        dprint(PRN_ERROR, "frame_mbs_only_flag err\n");
        return H264_ERR;
    }

    if (!pSPS->frame_mbs_only_flag)
    {
        pSPS->mb_adaptive_frame_field_flag = u_1(pH264Ctx, "SPS: mb_adaptive_frame_field_flag");
        if (((pSPS->pic_height_in_map_units_minus1+1)>=MAX_IMG_HALF_HEIGHT_IN_MB) || ((pSPS->pic_width_in_mbs_minus1+1)<4)
            || ((pSPS->pic_height_in_map_units_minus1+1)<2))
        {
            dprint(PRN_ERROR, "pSPS->pic_height_in_map_units_minus1 out of range.\n");
            return H264_ERR;
        }
    }
    else
    {
        pSPS->mb_adaptive_frame_field_flag = 0; // default
        if (((pSPS->pic_height_in_map_units_minus1+1)>=MAX_IMG_HEIGHT_IN_MB) || ((pSPS->pic_height_in_map_units_minus1+1)<2))
        {
            dprint(PRN_ERROR, "pSPS->pic_height_in_map_units_minus1 out of range.\n");
            return H264_ERR;
        }
    }

    // ����һ��ͼ��Ĵ�С
    PicWidthInSmp =  (pSPS->pic_width_in_mbs_minus1+1) *16;
    PicHeightInSmp = (pSPS->pic_height_in_map_units_minus1+1) *temp*16 ;
    pic_size = (PicWidthInSmp*PicHeightInSmp/2)*3;
    if (pic_size > MAX_IMG_WIDTH_IN_MB*MAX_IMG_HEIGHT_IN_MB*384)
    {
        dprint(PRN_FATAL, "pic size too large.\n");
        return H264_ERR;
    }

    // ����Level����ͼ��ռ��С
    switch (pSPS->level_idc)
    {
    case 10:
        size = 152064;
        break;
    case 11:
        size = 345600;
        break;
    case 12:
        size = 912384;
        break;
    case 13:
        size = 912384;
        break;
    case 20:
        size = 912384;
        break;
    case 21:
        size = 1824768;
        break;
    case 22:
        size = 3110400;
        break;
    case 30:
        size = 3110400;
        break;
    case 31:
        size = 6912000;
        break;
    case 32:
        size = 7864320;
        break;
    case 40:
        size = 12582912;
        break;
    case 41:
        size = 12582912;
        break;
    default:
        dprint(PRN_ERROR, "level_idc %d not support.\n", pSPS->level_idc);
        return H264_ERR;  //LEVEL_ERR
    }

    size /= pic_size; // ����ɷ��µ�ͼ�����

    if (size > 16)
    {
        size = 16;
    }

#if 0 //#ifndef  NOT_CLIP_DPB_TO_5
    //�ϴ�ͼ����dpb size����5����Ҫ���Ƶ�5����ķ��ڴ����
    //YUV�ȶԻع�ʱ��Ҫ�رմ�����
    if ((5 < size) && ((512 < PicWidthInSmp) && (288 < PicHeightInSmp)))
    {
        dprint(PRN_DBG, "dpb size:%d clip to 5\n", size);
        size = 5;
    }
#endif
    dprint(PRN_SE, "dpb size according level : %d\n", size);

    if ((pSPS->num_ref_frames>size) || pH264Ctx->SeErrFlg)
    {
        dprint(PRN_FATAL, "pSPS->num_ref_frames(%d) > DPB size(%d), try to select the reasonable one.\n",
                        pSPS->num_ref_frames,size);

        if (pSPS->num_ref_frames>16)
        {
            pSPS->num_ref_frames = size;
        }
        else
        {
            size = MAX(pSPS->num_ref_frames, size);
            pSPS->num_ref_frames = size;
        }
    }

    pSPS->direct_8x8_inference_flag    = u_1 (pH264Ctx, "SPS: direct_8x8_inference_flag"        );
    SE_ERR_CHECK;

    pSPS->frame_cropping_flag          = u_1 (pH264Ctx, "SPS: frame_cropping_flag"              );
    SE_ERR_CHECK;

    frame_cropping_flag = pSPS->frame_cropping_flag;
    if (frame_cropping_flag)
    {
        pSPS->frame_crop_left_offset   = ue_v(pH264Ctx, "SPS: frame_cropping_rect_left_offset"  );
        pSPS->frame_crop_right_offset  = ue_v(pH264Ctx, "SPS: frame_cropping_rect_right_offset" );
        pSPS->frame_crop_top_offset    = ue_v(pH264Ctx, "SPS: frame_cropping_rect_top_offset"   );
        pSPS->frame_crop_bottom_offset = ue_v(pH264Ctx, "SPS: frame_cropping_rect_bottom_offset");

        frame_cropping_rect_left_offset   = pSPS->frame_crop_left_offset;
        frame_cropping_rect_right_offset  = pSPS->frame_crop_right_offset;
        frame_cropping_rect_top_offset    = pSPS->frame_crop_top_offset;
        frame_cropping_rect_bottom_offset = pSPS->frame_crop_bottom_offset;

        CropW = PicWidthInSmp - 2*frame_cropping_rect_left_offset - 2*frame_cropping_rect_right_offset;
        CropH = PicHeightInSmp - 2*temp*frame_cropping_rect_top_offset - 2*temp*frame_cropping_rect_bottom_offset;
        if (0 >= CropW || 0 >= CropH )
        {
            //dec err cropping para, not cropping
            pSPS->disp_width    = PicWidthInSmp;
            pSPS->disp_height   = PicHeightInSmp;
            pSPS->disp_center_x = PicWidthInSmp/2;
            pSPS->disp_center_y = PicHeightInSmp/2;
        }
        else
        {
            pSPS->disp_width    = (UINT32)CropW;
            pSPS->disp_height   = (UINT32)CropH;
            pSPS->disp_center_x = 2*frame_cropping_rect_left_offset + pSPS->disp_width/2;
            pSPS->disp_center_y = 2*temp*frame_cropping_rect_top_offset + pSPS->disp_height/2;
        }
    }
    else
    {
        pSPS->disp_width    = PicWidthInSmp;
        pSPS->disp_height   = PicHeightInSmp;
        pSPS->disp_center_x = PicWidthInSmp/2;
        pSPS->disp_center_y = PicHeightInSmp/2;
    }

    pSPS->vui_parameters_present_flag     = u_1 (pH264Ctx, "SPS: vui_parameters_present_flag"      );
    SE_ERR_CHECK;

    pSPS->vui_seq_parameters.matrix_coefficients = 2;
    if (pSPS->vui_parameters_present_flag)
    {
        if (H264_OK == vui_parameters(pH264Ctx, &(pSPS->vui_seq_parameters))) // ����VUI��Ϣ
        {
            if (pSPS->vui_seq_parameters.bitstream_restriction_flag)
            {
                if (pSPS->vui_seq_parameters.max_dec_frame_buffering > size ||
                    pSPS->vui_seq_parameters.max_dec_frame_buffering < 1)
                {
                    dprint(PRN_ERROR, "max_dec_frame_buffering(%d) > MaxDpbSize(%d)\n",
                           pSPS->vui_seq_parameters.max_dec_frame_buffering, size);
                }
                else
                {
                    size = MAX(pSPS->vui_seq_parameters.max_dec_frame_buffering, pSPS->num_ref_frames);
                }
            }
        }
    }
    else
    {
        // add some default value
        pSPS->vui_seq_parameters.aspect_ratio_idc = 0;
        pSPS->vui_seq_parameters.video_format = 5;
    }

    pSPS->vui_seq_parameters.aspect_ratio = H264GetDar(pSPS->vui_seq_parameters.aspect_ratio_idc, pSPS->vui_seq_parameters.sar_width, pSPS->vui_seq_parameters.sar_height, PicWidthInSmp, PicHeightInSmp);

    if (pSPS->num_ref_frames > 0)
    {
        size = MIN(size, pSPS->num_ref_frames);
    }
    
    size = MIN(size, 16);

    if ((pH264Ctx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION) &&
        (pH264Ctx->pstExtraData->stChanOption.s32ReRangeEn == 0)) /* contition??? */
    {
        SINT32 MaxRefNum, MaxWidth, MaxHeight;
        MaxRefNum = pH264Ctx->pstExtraData->stChanOption.s32MaxRefFrameNum;
        MaxWidth  = pH264Ctx->pstExtraData->stChanOption.s32MaxWidth;
        MaxHeight = pH264Ctx->pstExtraData->stChanOption.s32MaxHeight;
        if (size > MaxRefNum)
        {
            REPORT_REF_NUM_OVER(pH264Ctx->ChanID, size, pH264Ctx->pstExtraData->stChanOption.s32MaxRefFrameNum);
		    /*�˴�ֱ�ӷ���error���Ա��������������ֱ��ʳ�����߲ο�֡��������ʱ�ظ���ʼ��DPB���²���Ҫ���鷳*/
		    return H264_ERR;
        }

        if (PicWidthInSmp > MaxWidth || PicHeightInSmp > MaxHeight)
        {
            REPORT_IMGSIZE_OVER(pH264Ctx->ChanID, PicWidthInSmp, PicHeightInSmp, MaxWidth, MaxHeight);
    		/*�˴�ֱ�ӷ���error���Ա��������������ֱ��ʳ�����߲ο�֡��������ʱ�ظ���ʼ��DPB���²���Ҫ���鷳*/
    		return H264_ERR;
        }

    }


    /* ҵ��������ͨ��û��̫��ο�֡����������һ�� */
    if (PicHeightInSmp >= 1920)
    {
        size = MIN(size, 16);
    }

    pSPS->dpb_size_plus1 = size + 1;

    return H264_OK;
}
/*add by w00283384 for subsps in svc, ������Щ���������жϸ������Ƿ������������*/
SINT32 Sps_SvcExt(H264_CTX_S *pH264Ctx)
{
    H264_PPS_S *pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    H264_SPS_S *pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);
    UINT32 inter_layer_present_flag;
    UINT32 extended_spatial_scal_idc;
    UINT32 chroma_phase_x_plus1_flag;
    UINT32 chroma_phase_y_plus1;
    UINT32 seq_tcoeff_level_prediction_flag;
    UINT32 adaptive_tcoeff_level_prediction_flag;
    UINT32 slice_header_restriction_flag;
    UINT32 test_code;
    test_code = BsGet(&(pH264Ctx->BS), 32);
    inter_layer_present_flag = u_1(pH264Ctx, "SPS_SVC_EXT: inter_layer_deblocking_filter_control_present_flag");
    extended_spatial_scal_idc = u_v(pH264Ctx, 2, "SPS_SVC_EXT: extended_spatial_scalability_idc");
    if(1 == extended_spatial_scal_idc)
    {
        dprint(PRN_ERROR, "spatial scalability not support!!\n");
        return H264_ERR;
    }
    if(1 == pSPS->chroma_format_idc  || 2 == pSPS->chroma_format_idc)
    {
        chroma_phase_x_plus1_flag = u_1(pH264Ctx, "SPS_SVC_EXT: chroma_phase_x_plus1_flag");
    }
    if(1 == pSPS->chroma_format_idc)
    {
        chroma_phase_y_plus1 = u_v(pH264Ctx, 2, "SPS_SVC_EXT: chroma_phase_y_plus1");
    }
    seq_tcoeff_level_prediction_flag = u_1(pH264Ctx, "SPS_SVC_EXT: seq_tcoeff_level_prediction_flag");
    if(seq_tcoeff_level_prediction_flag)
    {
        adaptive_tcoeff_level_prediction_flag = u_1(pH264Ctx, "SPS_SVC_EXT: adaptive_tcoeff_level_prediction_flag");
    }
    slice_header_restriction_flag = u_1(pH264Ctx, "SPS_SVC_EXT: slice_header_restriction_flag");
    //fix me here
    if(slice_header_restriction_flag != 1)
    {
        dprint(PRN_ERROR,"slice_header_restriction_flag error:%d,%x,%d\n", slice_header_restriction_flag, test_code, pSPS->chroma_format_idc);
        //return H264_ERR;
    }
    return H264_OK;
}

/***********************************************************************************
* Function:    H264_DecSPS(H264_CTX_S *pH264Ctx)
* Description: ����SPS, ������ݴ���
* Input:       ����������ʱ����������
* Output:      g_SPS        ֻ��Ӱ�������е�һ��Ԫ�ؽṹ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 H264_DecSPS(H264_CTX_S *pH264Ctx)
{
    static H264_SPS_S sps_tmp;   /* ����ɾ�̬����ֹռ�ö�ջ̫�� */
    UINT32 profile_idc;
    UINT32 constrained_set0_flag;
    UINT32 constrained_set1_flag;
    UINT32 constrained_set2_flag;
    UINT32 constrained_set3_flag;
    UINT32 level_idc;
    UINT32 seq_parameter_set_id;

    profile_idc           = u_v (pH264Ctx, 8, "SPS: profile_idc"          );
    constrained_set0_flag = u_1 (pH264Ctx,    "SPS: constrained_set0_flag");
    constrained_set1_flag = u_1 (pH264Ctx,    "SPS: constrained_set1_flag");
    constrained_set2_flag = u_1 (pH264Ctx,    "SPS: constrained_set2_flag");
    constrained_set3_flag = u_1 (pH264Ctx,    "SPS: constrained_set3_flag");
    u_v (pH264Ctx, 4, "SPS: reserved_zero_4bits"  ); // reserved_zero
    level_idc             = u_v (pH264Ctx, 8, "SPS: level_idc"            );
    seq_parameter_set_id  = ue_v(pH264Ctx,    "SPS: seq_parameter_set_id" );

    if (pH264Ctx->SeErrFlg)
    {
        return H264_ERR;
    }

    if (seq_parameter_set_id > (pH264Ctx->MaxSpsNum-1))
    {
        dprint(PRN_ERROR, "seq_parameter_set_id out of range.\n");
        if (seq_parameter_set_id > (pH264Ctx->MaxSpsNum-1))
        {
            REPORT_SPS_NUM_OVER( pH264Ctx->ChanID, seq_parameter_set_id, pH264Ctx->MaxSpsNum );
        }
        // ���Կ���ǯλǿ��
        return H264_ERR;
    }

    switch (profile_idc)
    {
    case BASELINE:
        dprint(PRN_ERROR, "Baseline stream, try to decode, exit when FMO occurred.\n");
        break;
    case EXTENDED:
        dprint(PRN_ERROR, "EXTENDED stream, try to decode, exit when datapartition occurred.\n");
        break;
    case MAINPROFILE:
        break;
    case HIGHPROFILE:
        break;
    //add svc base profile and high profile by w283384
    case SCALABLE_BASELINE_PROFILE:
    case SCALABLE_HIGH_PROFILE:
        break;
    case HIGH10P:
    case HIGH422P:
    case HIGH444P:
        dprint(PRN_ERROR, "others High profile stream, try to decode, exit when high profile not support occurred.\n");
        break;
    default:
        dprint(PRN_ERROR, "profile_idc = %5d error, try to decode as main profile.\n", profile_idc);
        profile_idc = HIGHPROFILE;
        break;
    }

    if (level_idc < H264_MIN_LEVEL)
    {
        dprint(PRN_ERROR, "level_idc = %5d error, try to decode as level_idc %d.\n", level_idc, H264_MAX_LEVEL);
        level_idc = H264_MAX_LEVEL;
    }

    if (level_idc > H264_MAX_LEVEL)
    {
        dprint(PRN_ERROR, "level_idc = %5d error, try to decode as level_idc %d.\n", level_idc, H264_MAX_LEVEL);
        level_idc = H264_MAX_LEVEL;
    }

    if (pH264Ctx->SPS[seq_parameter_set_id].is_valid) // ������Թ���id��Ӧ��SPS
    {
        sps_tmp.profile_idc           = profile_idc;
        sps_tmp.constrained_set0_flag = constrained_set0_flag;
        sps_tmp.constrained_set1_flag = constrained_set1_flag;
        sps_tmp.constrained_set2_flag = constrained_set2_flag;
        sps_tmp.constrained_set3_flag = constrained_set3_flag;
        sps_tmp.level_idc             = level_idc;
        sps_tmp.seq_parameter_set_id  = seq_parameter_set_id;

        if (H264_OK != ProcessSPS(pH264Ctx, &sps_tmp))
        {
            dprint(PRN_ERROR, "Line %d: SPS[%d] decode error.\n", __LINE__, seq_parameter_set_id);
            return H264_ERR;
        }

        if (H264_OK != SPSEqual(&sps_tmp, &(pH264Ctx->SPS[seq_parameter_set_id])))
        {
            sps_tmp.is_refreshed = 1;
            sps_tmp.is_valid = 1;
            memmove(&(pH264Ctx->SPS[seq_parameter_set_id]), &sps_tmp, sizeof(H264_SPS_S));
        }
    }
    else
    {
        pH264Ctx->SPS[seq_parameter_set_id].profile_idc           = profile_idc;
        pH264Ctx->SPS[seq_parameter_set_id].constrained_set0_flag = constrained_set0_flag;
        pH264Ctx->SPS[seq_parameter_set_id].constrained_set1_flag = constrained_set1_flag;
        pH264Ctx->SPS[seq_parameter_set_id].constrained_set2_flag = constrained_set2_flag;
        pH264Ctx->SPS[seq_parameter_set_id].constrained_set3_flag = constrained_set3_flag;
        pH264Ctx->SPS[seq_parameter_set_id].level_idc             = level_idc;
        pH264Ctx->SPS[seq_parameter_set_id].seq_parameter_set_id  = seq_parameter_set_id;

        if (H264_OK != ProcessSPS(pH264Ctx, &(pH264Ctx->SPS[seq_parameter_set_id])))
        {
            dprint(PRN_ERROR, "Line %d: SPS[%d] decode error.\n", __LINE__, seq_parameter_set_id);
            pH264Ctx->SPS[seq_parameter_set_id].is_refreshed = 1;
            pH264Ctx->SPS[seq_parameter_set_id].is_valid = 0;
            return H264_ERR;
        }

        pH264Ctx->SPS[seq_parameter_set_id].is_refreshed = 1;
        pH264Ctx->SPS[seq_parameter_set_id].is_valid = 1;
    }
    /*add by w283384 for svc base profile and high profile*/
    if(SCALABLE_BASELINE_PROFILE == profile_idc || SCALABLE_HIGH_PROFILE == profile_idc)
    {
        if(H264_OK != Sps_SvcExt(pH264Ctx))
        {
            dprint(PRN_ERROR,"svc ext error:\n");
            return H264_ERR;
        }
    }
    return H264_OK;
}


/***********************************************************************************
* SINT32    PassBytes(H264_CTX_S *pH264Ctx, UINT32 len)
* Description: ����SEI
* Input:       pStream             ����������ʱ����������
*              size                SEI Nalu��С
* Output:      g_RecoverPointFlg   ����ָ����־
*              pSEI                �����SEI��Ϣ�ṹ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 PassBytes(H264_CTX_S *pH264Ctx, UINT32 len)
{
    UINT32 segment_idx;

    if (NULL == pH264Ctx->pCurrNal || 0 >= len)
    {
        return -1;
    }

    if (NULL == pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].streamBuffer)
    {
        return -1;
    }

    if (2 < pH264Ctx->pCurrNal->nal_segment || 1 < pH264Ctx->pCurrNal->nal_used_segment ||
        (UINT32)pH264Ctx->pCurrNal->nal_used_segment >= pH264Ctx->pCurrNal->nal_segment)
    {
        return -1;
    }

    len *= 8;
    for (segment_idx = pH264Ctx->pCurrNal->nal_used_segment; segment_idx < pH264Ctx->pCurrNal->nal_segment; segment_idx++)
    {
        if ((len + pH264Ctx->pCurrNal->stream[segment_idx].bitsoffset) <= (pH264Ctx->pCurrNal->stream[segment_idx].bitstream_length*8))
        {
            pH264Ctx->pCurrNal->stream[segment_idx].bitsoffset += len;

            // pass stream succeed
            return 1;
        }
        else
        {
            len = len + pH264Ctx->pCurrNal->stream[segment_idx].bitsoffset - pH264Ctx->pCurrNal->stream[segment_idx].bitstream_length*8;
            pH264Ctx->pCurrNal->stream[segment_idx].bitsoffset = pH264Ctx->pCurrNal->stream[segment_idx].bitstream_length*8;
            pH264Ctx->pCurrNal->nal_used_segment++;
        }
    }

    //no stream
    return 0;
}


/***********************************************************************************
* Function:    SINT32    GetBytes(H264_CTX_S *pH264Ctx, UINT8 * pdes, SINT32 len)
* Description: ����SEI
* Input:       pStream             ����������ʱ����������
*              size                SEI Nalu��С
* Output:      g_RecoverPointFlg   ����ָ����־
*              pSEI                �����SEI��Ϣ�ṹ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 GetBytes(H264_CTX_S *pH264Ctx, UINT8 *pdes, SINT32 len)
{
    UINT32 segment_idx;
    UINT8  *byte_pos, *end_byte;
    UINT32 cnt;

    if (NULL == pdes || NULL == pH264Ctx->pCurrNal || 0 >= len)
    {
        return -1;
    }

    if (NULL == pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_used_segment].streamBuffer)
    {
        return -1;
    }

    if (2 < pH264Ctx->pCurrNal->nal_segment || 1 < pH264Ctx->pCurrNal->nal_used_segment ||
       (UINT32)pH264Ctx->pCurrNal->nal_used_segment >= pH264Ctx->pCurrNal->nal_segment)
    {
        return -1;
    }

    cnt = 0;
    for (segment_idx = pH264Ctx->pCurrNal->nal_used_segment; segment_idx < pH264Ctx->pCurrNal->nal_segment; segment_idx++)
    {
        byte_pos =  pH264Ctx->pCurrNal->stream[segment_idx].streamBuffer +
            ((pH264Ctx->pCurrNal->stream[segment_idx].bitsoffset + 7)/8);
        end_byte = pH264Ctx->pCurrNal->stream[segment_idx].streamBuffer + pH264Ctx->pCurrNal->stream[segment_idx].bitstream_length;
        while (byte_pos < end_byte)
        {
            *(pdes++) = *(byte_pos);
            cnt++;
			byte_pos++;
            pH264Ctx->pCurrNal->stream[segment_idx].bitsoffset += 8;
            if (cnt >= len)
            {
                return len;
            }
        }
        pH264Ctx->pCurrNal->nal_used_segment++;
		#if 0
        if ((segment_idx+1) == pH264Ctx->pCurrNal->nal_segment)
        {
            return -1;
        }
		#endif
    }

    return cnt;
}


SINT32 H264_DecFramePackingSEI(H264_CTX_S *pH264Ctx, SINT32 PayloadSize)
{
    H264_FRAME_PACKING_ARRANGEMENT_S  *pFramePacking = &pH264Ctx->FramePackSEI;

    pFramePacking->frame_packing_arrangement_id = ue_v(pH264Ctx, "SEI: frame_packing_arrangement_id");
    pFramePacking->frame_packing_arrangement_cancel_flag = u_v(pH264Ctx, 1, "SEI: frame_packing_arrangement_cancel_flag");

    if (!pFramePacking->frame_packing_arrangement_cancel_flag)
    {
        pFramePacking->frame_packing_arrangement_type = u_v(pH264Ctx, 7, "SEI: frame_packing_arrangement_type");
        pFramePacking->quincunx_sampling_flag = u_v(pH264Ctx, 1, "SEI: quincunx_sampling_flag");
        pFramePacking->content_interpretation_type = u_v(pH264Ctx, 6, "SEI: content_interpretation_type");
        pFramePacking->spatial_flipping_flag = u_v(pH264Ctx, 1, "SEI: spatial_flipping_flag");
        pFramePacking->frame0_flipped_flag = u_v(pH264Ctx, 1, "SEI: frame0_flipped_flag");
        pFramePacking->field_views_flag = u_v(pH264Ctx, 1, "SEI: field_views_flag");
        pFramePacking->current_frame_is_frame0_flag = u_v(pH264Ctx, 1, "SEI: current_frame_is_frame0_flag");
        pFramePacking->frame0_self_contained_flag = u_v(pH264Ctx, 1, "SEI: frame0_self_contained_flag");
        pFramePacking->frame1_self_contained_flag = u_v(pH264Ctx, 1, "SEI: frame1_self_contained_flag");

        if (!pFramePacking->quincunx_sampling_flag && pFramePacking->frame_packing_arrangement_type != 5)
        {
            pFramePacking->frame0_grid_position_x = u_v(pH264Ctx, 1, "SEI: frame0_grid_position_x");
            pFramePacking->frame0_grid_position_y = u_v(pH264Ctx, 1, "SEI: frame0_grid_position_y");
            pFramePacking->frame1_grid_position_x = u_v(pH264Ctx, 1, "SEI: frame1_grid_position_x");
            pFramePacking->frame1_grid_position_y = u_v(pH264Ctx, 1, "SEI: frame1_grid_position_y");
        }
        pFramePacking->frame_packing_arrangement_reserved_byte = u_v(pH264Ctx, 8, "SEI: frame_packing_arrangement_reserved_byte");
        pFramePacking->frame_packing_arrangement_repetition_period = ue_v(pH264Ctx, "SEI: frame_packing_arrangement_repetition_period");
    }

    pFramePacking->frame_packing_arrangement_extension_flag = u_v(pH264Ctx, 1, "SEI: frame_packing_arrangement_extension_flag");

    pFramePacking->frame_packing_dec_ok = 1;
    return H264_OK;
}


SINT32 H264_DecPicTimingSEI(H264_CTX_S *pH264Ctx, SINT32 PayloadSize)
{
    H264_PIC_TIMING_SEI_S  *pPicTiming = &pH264Ctx->PicTimingSEI;
    H264_SPS_S  *pSPS;
    SINT32 cpb_removal_len = 0 ;
    SINT32 dpb_output_len = 0;
    SINT32 i = 0;

    pSPS = &pH264Ctx->SPS[pH264Ctx->CurrSPS.seq_parameter_set_id];
	if (!(pSPS->is_valid))
	{
        pSPS = NULL;
	    for(i=0; i<pH264Ctx->MaxSpsNum; i++)
	    {
	        if (pH264Ctx->SPS[i].is_valid)
	        {
	            pSPS = &pH264Ctx->SPS[i];
	            break;
	        }
	    }
        if (pSPS == NULL)
        {
            dprint(PRN_ERROR, "DecPicTimingSEI but SPS is invalid.\n");
	        return H264_ERR;
        }
	}

    if (pSPS->vui_parameters_present_flag && (pSPS->vui_seq_parameters.nal_hrd_parameters_present_flag ||
        pSPS->vui_seq_parameters.vcl_hrd_parameters_present_flag))
    {
        if (pSPS->vui_seq_parameters.nal_hrd_parameters_present_flag)
        {
            cpb_removal_len = pSPS->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
            dpb_output_len  = pSPS->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1  + 1;
        }
        else if (pSPS->vui_seq_parameters.vcl_hrd_parameters_present_flag)
        {
            cpb_removal_len = pSPS->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
            dpb_output_len  = pSPS->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1  + 1;
        }

        pPicTiming->cpb_removal_delay = u_v(pH264Ctx, cpb_removal_len, "SEI: cpb_removal_delay");
        pPicTiming->dpb_output_delay  = u_v(pH264Ctx, dpb_output_len, "SEI: dpb_output_delay");
    }

    if (pSPS->vui_seq_parameters.pic_struct_present_flag)
    {
        pPicTiming->pic_struct = u_v(pH264Ctx, 4, "pic_struct");
    }

    return H264_OK;
}


#define MAX_PAYLOAD_SIZE  (400*1024)

/***********************************************************************************
* Function:    H264_DecSEI(H264_CTX_S *pH264Ctx)
* Description: ����SEI
* Input:       pStream             ����������ʱ����������
*              size                SEI Nalu��С
* Output:      g_RecoverPointFlg   ����ָ����־
*              pSEI                �����SEI��Ϣ�ṹ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 H264_DecSEI(H264_CTX_S *pH264Ctx)
{
    UINT32 payloadType;
    SINT32 payloadSize;
    SINT32 ret;
    UINT32 min_len;
    UINT8 byte_tmp = 0;
    UINT32 TotalBitOffset;//just for dprint
    UINT32 segment_idx;
    UINT32 lenth_not_parse;
    UINT32 min_len_tmp;
    SINT32 len_tmp;
    SINT32 bitpos;
    SINT32 i;
    SINT32 nBits;
    SINT32 used_byte;
    SINT8 registered_flag = 0;

    //skip nal type
    pH264Ctx->pCurrNal->stream[0].bitsoffset = 32;

    payloadType = 0;

    do
    {
        ret = GetBytes(pH264Ctx, &byte_tmp, 1);
        payloadType += ((byte_tmp == 0xFF) ? 255 : byte_tmp);
        nBits = 8;
        if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
        {
            ret = 0;
        }
        else
        {
           BsSkip(&pH264Ctx->BS, nBits);
        }
    }
    while ((0 < ret) && (0xFF == byte_tmp));

    if (0 >= ret)
    {
        dprint(PRN_ERROR, "SEI nal dec payload type err\n");
        return H264_ERR;
    }


    do // sei_message()
    {
        payloadSize = 0;
        do
        {
            ret = GetBytes(pH264Ctx, &byte_tmp, 1);
            payloadSize += ((byte_tmp == 0xFF) ? 255 : byte_tmp);
            nBits = 8;
            if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
            {
                ret = 0;
            }
            else
            {
               BsSkip(&pH264Ctx->BS, nBits);
            }
        }
        while ((0 < ret) && (0xFF == byte_tmp));

        if ( (0 >= ret) || (payloadSize>MAX_PAYLOAD_SIZE) )
        {
            dprint(PRN_ERROR, "SEI nal dec payloadSize err\n");
            return H264_ERR;
        }
        //just for dprint
        if (2 == pH264Ctx->pCurrNal->nal_segment)
        {
            TotalBitOffset = (pH264Ctx->pCurrNal->stream[0].bitsoffset +  pH264Ctx->pCurrNal->stream[1].bitsoffset)/8;
        }
        else
        {
            TotalBitOffset = pH264Ctx->pCurrNal->stream[0].bitsoffset;
        }
        //end of dprint calc
        switch (payloadType)     // sei_payload(payloadType, payloadSize)
        {
        case SEI_BUFFERING_PERIOD:
            dprint(PRN_SEI,"SEI_BUFFERING_PERIOD,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // buffering_period(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }

            break;
        case SEI_PIC_TIMING:
            dprint(PRN_SEI,"SEI_PIC_TIMING,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            bitpos = pH264Ctx->BS.TotalPos;
            H264_DecPicTimingSEI(pH264Ctx, payloadSize);
            bitpos = pH264Ctx->BS.TotalPos - bitpos;
            if (8*payloadSize > bitpos)
            {
                for(i=0;i<((8*payloadSize-bitpos)/8);i++)
                {
                    nBits = 8;
                    if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pH264Ctx->BS, nBits);
                    }
                }
                nBits = (8*payloadSize-bitpos)%8;
                if ((pH264Ctx->BS.TotalPos+nBits)<=(pH264Ctx->BS.BsLen*8))
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            ret = PassBytes(pH264Ctx, payloadSize);
            break;
        case SEI_PAN_SCAN_RECT:
            dprint(PRN_SEI,"SEI_PAN_SCAN_RECT,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // pan_scan_rect(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_FILLER_PAYLOAD:
            dprint(PRN_SEI,"SEI_FILLER_PAYLOAD,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // filler_payload(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;

        case SEI_USER_DATA_REGISTERED_ITU_T_T35:
            registered_flag = 1;
        case SEI_USER_DATA_UNREGISTERED:
            if (1 == registered_flag)
            {
                dprint(PRN_SEI, "SEI_USER_DATA_REGISTERED_ITU_T_T35, offset = %d, payload_size = %d \n", TotalBitOffset, payloadSize);
            }
            else
            {
                dprint(PRN_SEI, "SEI_USER_DATA_UNREGISTERED, offset = %d, payload_size = %d \n", TotalBitOffset, payloadSize);
            }

            if (pH264Ctx->TotalUsrDatNum > (MAX_USERDATA_NUM - 1))
            {
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[0]);
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[1]);
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[2]);
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[3]);
				pH264Ctx->pUsrDatArray[0] = NULL;
				pH264Ctx->pUsrDatArray[1] = NULL;
				pH264Ctx->pUsrDatArray[2] = NULL;
				pH264Ctx->pUsrDatArray[3] = NULL;
                pH264Ctx->TotalUsrDatNum = 0;
            }
            pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum] = GetUsd(pH264Ctx->ChanID);
            if (NULL == pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum])
            {
                //dprint(PRN_ERROR, "cann't get usrdata pointer\n");
                return H264_ERR;
            }
            min_len = MIN(MAX_USRDAT_SIZE, payloadSize);
			lenth_not_parse = 0;
			for (segment_idx = pH264Ctx->pCurrNal->nal_used_segment; segment_idx < pH264Ctx->pCurrNal->nal_segment; segment_idx++)
            {
                lenth_not_parse += pH264Ctx->pCurrNal->stream[segment_idx].bitstream_length -
                                  ((pH264Ctx->pCurrNal->stream[segment_idx].bitsoffset + 7)/8);
            }
            if ((SINT32)min_len > (SINT32)lenth_not_parse)
            {
                min_len_tmp = lenth_not_parse;
            }
            else
            {
                min_len_tmp = min_len;
            }

            used_byte = 0;
            if (1 == registered_flag)    // SEI_USER_DATA_REGISTERED_ITU_T_T35 ������ز���   y00226912
            {
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->IsRegistered = 1;
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->itu_t_t35_country_code = u_v(pH264Ctx, 8, "SEI: itu_t_t35_country_code");
                if (pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->itu_t_t35_country_code != 0xFF)
                {
		            pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->itu_t_t35_country_code_extension_byte = 0;
		            used_byte = 1;
                }
                else
                {
	   	            pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->itu_t_t35_country_code_extension_byte = u_v(pH264Ctx, 8, "SEI: itu_t_t35_country_code_extension_byte");
		            used_byte = 2;
                }
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->itu_t_t35_provider_code = u_v(pH264Ctx, 16, "SEI: itu_t_t35_provider_code");
                used_byte += 2;
                PassBytes(pH264Ctx, used_byte);

                if (used_byte == min_len_tmp)
                {
	                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->PTS = pH264Ctx->pstExtraData->pts;
                    pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->from = USD_H264;
                    pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->seq_cnt = pH264Ctx->SeqCnt;
                    REPORT_USRDAT(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]);
                    pH264Ctx->TotalUsrDatNum++;
                    break;
                 }
            }

            len_tmp = min_len_tmp - used_byte;
            if (0 >= len_tmp )
            {
                dprint(PRN_ERROR, "len_tmp = %d cann't dec usrdata\n", len_tmp);
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]);
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum] = NULL;
                return H264_ERR;
            }

            ret = GetBytes(pH264Ctx, pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->data, len_tmp);
            if (0 >= ret)
            {
                dprint(PRN_ERROR, "cann't dec usrdata\n");
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]);
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum] = NULL;
                return H264_ERR;
            }

            if (ret < ((SINT32)min_len-used_byte))
            {
                //return len smaller than the len would read, must no stream
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->data_size =  ret;
                for(i=0;i<ret;i++)
                {
                    nBits = 8;
                    if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pH264Ctx->BS, nBits);
                    }
                }
            }
            else
            {
                pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->data_size = payloadSize - used_byte;

                //need pass the stream which don't write to usrdata but is apart of usrdata;
                if (payloadSize > min_len)
                {
                    ret = PassBytes(pH264Ctx, payloadSize - min_len);
                }
                for(i=0;i<(payloadSize-used_byte);i++)
                {
                    nBits = 8;
                    if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pH264Ctx->BS, nBits);
                    }
                }
            }

            pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->PTS = pH264Ctx->pstExtraData->pts;
            pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->from = 8;
            pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]->seq_cnt = pH264Ctx->SeqCnt;
            REPORT_USRDAT(pH264Ctx->ChanID, pH264Ctx->pUsrDatArray[pH264Ctx->TotalUsrDatNum]);
            break;
        case SEI_RECOVERY_POINT:
            dprint(PRN_SEI,"SEI_RECOVERY_POINT,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // recovery_point(pStream+offset, payloadSize, pSEI);
            // g_RecoverPointFlg = 1;
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            ret = PassBytes(pH264Ctx, payloadSize);
            break;
        case SEI_DEC_REF_PIC_MARKING_REPETITION:
            dprint(PRN_SEI,"SEI_DEC_REF_PIC_MARKING_REPETITION,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // dec_ref_pic_marking_repetition(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_SPARE_PIC:
            dprint(PRN_SEI,"SEI_SPARE_PIC,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // spare_pic(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_SCENE_INFO:
            dprint(PRN_SEI,"SEI_SCENE_INFO,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // scene_info(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_SUB_SEQ_INFO:
            dprint(PRN_SEI,"SEI_SUB_SEQ_INFO,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // sub_seq_info(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_SUB_SEQ_LAYER_CHARACTERISTICS:
            dprint(PRN_SEI,"SEI_SUB_SEQ_LAYER_CHARACTERISTICS,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // sub_seq_layer_characteristics(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_SUB_SEQ_CHARACTERISTICS:
            dprint(PRN_SEI,"SEI_SUB_SEQ_CHARACTERISTICS,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // sub_seq_characteristics(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_FULL_FRAME_FREEZE:
            dprint(PRN_SEI,"SEI_FULL_FRAME_FREEZE,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // full_frame_freeze(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_FULL_FRAME_FREEZE_RELEASE:
            dprint(PRN_SEI,"SEI_FULL_FRAME_FREEZE_RELEASE,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // full_frame_freeze_release(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_FULL_FRAME_SNAPSHOT:
            dprint(PRN_SEI,"SEI_FULL_FRAME_SNAPSHOT,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // full_frame_snapshot(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START:
            dprint(PRN_SEI,"SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // progressive_refinement_segment_start(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END:
            dprint(PRN_SEI,"SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // progressive_refinement_segment_end(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET:
            dprint(PRN_SEI,"SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // motion_constrained_slice_group_set(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_FILM_GRAIN_CHARACTERISTICS:
            dprint(PRN_SEI,"SEI_FILM_GRAIN_CHARACTERISTICS,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // film_grain_characteristics(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE:
            dprint(PRN_SEI,"SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // deblocking_filter_display_preference(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_STEREO_VIDEO_INFO:
            dprint(PRN_SEI,"SEI_STEREO_VIDEO_INFO,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // stereo_video_info(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        case SEI_FRAME_PACKING_ARRANGEMENT:
            bitpos = pH264Ctx->BS.TotalPos;
            H264_DecFramePackingSEI(pH264Ctx, payloadSize);
            bitpos = pH264Ctx->BS.TotalPos - bitpos;

            if (8*payloadSize > bitpos)
            {
                for(i=0;i<((8*payloadSize-bitpos)/8);i++)
                {
                    nBits = 8;
                    if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pH264Ctx->BS, nBits);
                    }
                }
                nBits = (8*payloadSize-bitpos)%8;
                if ((pH264Ctx->BS.TotalPos+nBits)<=(pH264Ctx->BS.BsLen*8))
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            ret = PassBytes(pH264Ctx, payloadSize);
            break;
        default:
            //dprint(PRN_SEI,"interpret_reserved_info,offset = %d,payload_size = %d \n",TotalBitOffset,payloadSize);
            // reserved_sei_message(pStream+offset, payloadSize, pSEI);
            ret = PassBytes(pH264Ctx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pH264Ctx->BS, nBits);
                }
            }
            break;
        }

        if (4 <= pH264Ctx->TotalUsrDatNum)
        {
            dprint(PRN_ERROR, "too much usrdat, cann't dec SEI\n");
            return H264_ERR;
        }

        payloadType = 0;
        do
        {
            ret = GetBytes(pH264Ctx, &byte_tmp, 1);
            payloadType += ((byte_tmp == 0xFF) ? 255 : byte_tmp);
            nBits = 8;
            if ((pH264Ctx->BS.TotalPos+nBits)>(pH264Ctx->BS.BsLen*8))
            {
                ret = 0;
            }
            else
            {
               BsSkip(&pH264Ctx->BS, nBits);
            }
        }
        while ((0 < ret) && (0xFF == byte_tmp));
    }
    while ((0 < ret) && (byte_tmp != 0x80));   // more_rbsp_data(), pStream[offset]!=0x80

    //RestoreSEIInputUT(pH264Ctx->TotalNal);
    return H264_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  InitOldSlice(H264_CTX_S  *pH264Ctx)
*   �������� :  ��ʼ��g_OldSlice, Ŀ����Ϊ��ʹ��һslice�����ж�new_pic_flag
*   ����˵�� :  ��
*   ����ֵ   �� ��
************************************************************************
*/
VOID InitOldSlice(H264_CTX_S  *pH264Ctx)
{
    pH264Ctx->PrevSlice.pic_parameter_set_id = pH264Ctx->MaxPpsNum;
    pH264Ctx->PrevSlice.frame_num = 88888;
    pH264Ctx->PrevSlice.field_pic_flag = 0;
    pH264Ctx->PrevSlice.bottom_field_flag = 2;
    pH264Ctx->PrevSlice.nal_ref_idc = 7;
    pH264Ctx->PrevSlice.nal_unit_type = 32;
    pH264Ctx->PrevSlice.pic_oder_cnt_lsb = 3;
    pH264Ctx->PrevSlice.delta_pic_oder_cnt_bottom = -1;
    pH264Ctx->PrevSlice.delta_pic_order_cnt[0] = 0;
    pH264Ctx->PrevSlice.delta_pic_order_cnt[1] = 0;
    pH264Ctx->PrevSlice.first_mb_in_slice = MAX_MB_NUM_IN_PIC;

    return;
}


/***********************************************************************************
* Function:    H264_FlushDecoder(H264_CTX_S *pH264Ctx)
* Description: ��iģʽ�½��������nal���ͣ������֡�����������״̬
* Input:       ��
* Output:      ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/
VOID H264_FlushDecoder(H264_CTX_S *pH264Ctx)
{
    SINT32 ret;

    //start vdm to dec curr pic, and postprocess of curr pic
    pH264Ctx->CurrSlice.first_mb_in_slice = 0;  //�������� ʱ ������ ���� ������
    pH264Ctx->CurrSlice.new_pic_flag = 1;
    pH264Ctx->StartMode = 1;
    ret = DecVDM(pH264Ctx);    // ����VDM����

    return;
}


/***********************************************************************************
* Function:    H264_IsNewPicNal( SINT32 *pNewPicDetector,  UINT32 *pu8 )
* Description: ����һ��nal�Ƿ������µ�һ֡.
*                �˺����ڶ�������seg��ʱ�����ã������ж϶�����seg���Ƿ������µ�һ֡��
*              ����ڴ�������PTS������������Ҫ��
* Input:       pu8    nal���������׵�ַ
* Return:      DEC_NORMAL   ��nal������pic
*              DEC_ERR      ��nal��������pic��������֮ǰ��nalͬ��һ��pic
* Others:      ��
***********************************************************************************/
SINT32 H264_IsNewPicNal( SINT32 *pNewPicDetector, UINT8 *pu8 )
{
    SINT32 ret = H264_ERR;
    UINT8 nal_unit_type;
    UINT32 tmpbits, zeros, first_mb_in_slice, NumBits;

    pu8 += 3;  /* skip '00 00 01' */
    nal_unit_type = (pu8[0] & 0x1f);
    switch (nal_unit_type)
    {
    case 1:   /* slice */
    case 5:   /* IDR slice */
        if ( 0 == *pNewPicDetector )
        {
            tmpbits = (UINT32)pu8[4] | ((UINT32)pu8[3]<<8) | ((UINT32)pu8[2]<<16) | ((UINT32)pu8[1]<<24);
            zeros = ZerosMS_32(tmpbits);
            if (zeros < 16) /* first_mb_in_slice��ֵ�������zerosһ��С��16�����������Ϊ�쳣������Ϊ��֡ */
            {
                ue_vld(tmpbits, NumBits, first_mb_in_slice, zeros);
                if (0 == first_mb_in_slice)
                {
                    ret = H264_OK;
                }
            }
        }
        *pNewPicDetector = 0;
        break;

    case 7:   /* sps */
    case 8:   /* pps */
        if ( 0 == *pNewPicDetector )
        {
            ret = H264_OK;
        }
        *pNewPicDetector = 1;
        break;
    }

    return ret;
}


/***********************************************************************************
* Function:    H264_DecOneNal(H264_CTX_S *pH264Ctx)
* Description: ����NALU, ������ݴ���
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      NalMode: 0->BsFromOuter, 1->BsFromInner
***********************************************************************************/
SINT32 H264_DecOneNal(H264_CTX_S *pH264Ctx, UINT32 NalMode)
{
    UINT32 nal_header, code0, code1, eopic0, eopic1;
    SINT32 ret;
    UINT32 i;

    for (i = 0; i <pH264Ctx->pCurrNal->nal_segment; i++)
    {
        pH264Ctx->MaxBytesReceived += pH264Ctx->pCurrNal->stream[i].bitstream_length;
    }
    pH264Ctx->pCurrNal->nal_used_segment = 0; // decode from first segment
    nal_header = BsGet(&(pH264Ctx->BS), 32);     // ȡ��һ��32bit, 0x00 0x00 0x01 0xxx
    pH264Ctx->pCurrNal->nal_unit_type = nal_header & 0x1f;
    pH264Ctx->pCurrNal->nal_ref_idc   = (nal_header >> 5) & 3;
    pH264Ctx->pCurrNal->forbidden_bit = (nal_header >> 7) & 1;
    pH264Ctx->pCurrNal->nal_bitoffset += 32;   // used bits when decode nal, ʹ����0x000001 + 8bit, ��32bit
    pH264Ctx->SeErrFlg = 0;                    // clear g_SeErrFlg

    pH264Ctx->TotalNal++;
    /*add by w00283384 start, ���nal_unit_type == 20������Ҫ��һ��24bit��ͷ*/
    if(NALU_TYPE_SLC_EXT==pH264Ctx->pCurrNal->nal_unit_type)
    {
        nal_header = BsGet(&(pH264Ctx->BS), 24);
        pH264Ctx->pCurrNal->svc_extension_flag = (nal_header >> 23)  & 1;
        if(pH264Ctx->pCurrNal->svc_extension_flag)
        {
            pH264Ctx->SVC_NALUSVCEXT_S.idr_flag = (nal_header >> 22) & 1;
            pH264Ctx->SVC_NALUSVCEXT_S.priority_id = (nal_header >> 16) & 0x3f;
            pH264Ctx->SVC_NALUSVCEXT_S.no_inter_layer_pred_flag = (nal_header >> 15) & 1;
            pH264Ctx->SVC_NALUSVCEXT_S.dependency_id = (nal_header >> 12) & 7;
            pH264Ctx->SVC_NALUSVCEXT_S.quality_id = (nal_header >> 8) & 0xf;
            pH264Ctx->SVC_NALUSVCEXT_S.temporal_id = (nal_header >> 5) & 7;
            pH264Ctx->SVC_NALUSVCEXT_S.use_ref_base_pic_flag = (nal_header >> 4) & 1;
            pH264Ctx->SVC_NALUSVCEXT_S.discardable_flag = (nal_header >> 3) & 1;
            pH264Ctx->SVC_NALUSVCEXT_S.output_flag = (nal_header >> 2) & 1;
            pH264Ctx->SVC_NALUSVCEXT_S.reserved_three_2bits = nal_header & 3;
        }

        if(pH264Ctx->SVC_NALUSVCEXT_S.quality_id != 0 ||
        pH264Ctx->SVC_NALUSVCEXT_S.no_inter_layer_pred_flag != 1 )
        {
            dprint(PRN_ERROR, "svc_extension_flag error:priority_id:%d, pred_flag%d\n",
            pH264Ctx->SVC_NALUSVCEXT_S.quality_id,pH264Ctx->SVC_NALUSVCEXT_S.no_inter_layer_pred_flag);
            ClearCurrNal(pH264Ctx);
            return H264_ERR;
        }

        if(pH264Ctx->SVC_NALUSVCEXT_S.idr_flag == 1)
        {
            pH264Ctx->pCurrNal->nal_unit_type = NALU_TYPE_IDR;//w00283384,�˴������ж��Ƿ�Ϊidr֡
        }
        
        pH264Ctx->pCurrNal->nal_bitoffset += 24;  //used bits when decode nal svc info
    }
    switch (pH264Ctx->pCurrNal->nal_unit_type)
    {
    case NALU_TYPE_SLICE:
    case NALU_TYPE_IDR:
    case NALU_TYPE_SLC_EXT:
        dprint(PRN_DBG, "***** NAL: IDR/Slice, nal_unit_type=%d, TotalSlice=%d\n", pH264Ctx->pCurrNal->nal_unit_type, pH264Ctx->TotalSlice);

        pH264Ctx->CurrSlice.nal_unit_type = pH264Ctx->pCurrNal->nal_unit_type;
        pH264Ctx->CurrSlice.nal_ref_idc = pH264Ctx->pCurrNal->nal_ref_idc;
		#ifdef VFMW_SCD_LOWDLY_SUPPORT
        pH264Ctx->CurrSlice.pic_over_flag = pH264Ctx->pCurrNal->dsp_check_pic_over_flag;
        #endif
        ret = SliceCheck(pH264Ctx);
        if (H264_OK != ret)
        {
            dprint(PRN_ERROR, "SliceCheck failed, clear current slice.\n");
            ClearCurrSlice(pH264Ctx);
            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
            return H264_ERR;
        }

        ret = H264_DecSlice(pH264Ctx);
        if(H264_NOTDEC == ret)
        {
            return ret;
        }
        else if (H264_OK != ret)
        {
            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
            return H264_ERR;
        }

        /*
           ���ӳ�ʱ���ߵķ�֧, ����⵽��ǰ���SLICEΪһ֡�Ľ���ʱ���ͻ���������
           VDH ����, �Ͳ���Ҫ������֮ǰ�ж�һ֡�����ķ����ˡ�(֮ǰ�ķ���: �����
           ����һ֡SLICEʱ�������жϵ�ǰ֡����)
        */
		#ifdef VFMW_SCD_LOWDLY_SUPPORT
        if (1 == pH264Ctx->CurrSlice.pic_over_flag)
        {

            REPORT_MEET_NEWFRM(pH264Ctx->ChanID);
            pH264Ctx->StartMode = 0;
            ret = DecVDM(pH264Ctx);  // ����VDM����
            if (H264_OK != ret)
            {
	            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
                return H264_ERR;
            }
            pH264Ctx->PicIsDecoded = 1;
        }
        #endif

        break;

    case NALU_TYPE_PPS:
        dprint(PRN_DBG, "***** NAL: PPS, nal_unit_type=%d, TotalPPS=%d\n", pH264Ctx->pCurrNal->nal_unit_type, pH264Ctx->TotalPPS);
        ret = H264_DecPPS(pH264Ctx);
        ClearCurrNal(pH264Ctx);
        if (H264_OK != ret)
        {
            dprint(PRN_ERROR, "PPS decode error.\n");
            REPORT_STRMERR(pH264Ctx->ChanID, PPS_ERR);
            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
            return H264_ERR;
        }
        pH264Ctx->TotalPPS++;
        break;
    case NALU_TYPE_SPS:
    case NALU_TYPE_SUB_SPS:
        dprint(PRN_DBG, "***** NAL: SPS, nal_unit_type=%d, TotalSPS=%d\n", pH264Ctx->pCurrNal->nal_unit_type, pH264Ctx->TotalSPS);
        ret = H264_DecSPS(pH264Ctx);
        ClearCurrNal(pH264Ctx);
        if (H264_OK != ret)
        {
            dprint(PRN_ERROR, "SPS decode error.\n");
            REPORT_STRMERR(pH264Ctx->ChanID, SPS_ERR);

	        REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
            return H264_ERR;
        }
        pH264Ctx->TotalSPS++;
        break;
    case NALU_TYPE_SEI:
        dprint(PRN_DBG, "***** NAL: SEI, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
        ret = H264_DecSEI(pH264Ctx);
        ClearCurrNal(pH264Ctx);
        if (H264_OK != ret)
        {
            dprint(PRN_ERROR, "SEI decode error.\n");
            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
        }
        break;
    case NALU_TYPE_AUD:
        dprint(PRN_DBG, "***** NAL: AUD, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
        ClearCurrNal(pH264Ctx);
        break;
    case NALU_TYPE_EOSEQ:
        dprint(PRN_DBG, "***** NAL: EOSEQ, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
        ClearCurrNal(pH264Ctx);
        break;
    case NALU_TYPE_FILL:
        dprint(PRN_DBG, "***** NAL: FILL, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
        ClearCurrNal(pH264Ctx);
        break;
    case NALU_TYPE_EOSTREAM:
        dprint(PRN_DBG, "***** NAL: EOSTREAM, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
        code0 = (UINT32)BsGet(&(pH264Ctx->BS), 32);
	    code1 = (UINT32)BsGet(&(pH264Ctx->BS), 32);
        if (HISI_STREAM_END_CODE1 == code0 && HISI_STREAM_END_CODE2 == code1)
        {
            pH264Ctx->IsStreamEndFlag = 1;
            REPORT_STREAM_END_SYNTAX(pH264Ctx->ChanID);
            pH264Ctx->StartMode = 0;
            ret = DecVDM(pH264Ctx);  // ����VDM����
            if (H264_OK != ret)
            {
	         	REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
                return H264_ERR;
            }
        }
        ClearCurrNal(pH264Ctx);
        break;
    case NALU_TYPE_SPSEXT:
        dprint(PRN_DBG, "***** NAL: SPSEXT, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
        ClearCurrNal(pH264Ctx);
        break;
    case NALU_TYPE_AUX:
        dprint(PRN_DBG, "***** NAL: AUX, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
        ClearCurrNal(pH264Ctx);
        break;
    case NALU_TYPE_EOPIC:
        dprint(PRN_DBG, "***** NAL: EOPIC, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);

        if (0 == NalMode)
        {
            ClearCurrNal(pH264Ctx);
        }

        code0 = BsGet(&(pH264Ctx->BS), 32);
        code1 = BsGet(&(pH264Ctx->BS), 32);
        code0 = REVERSE_ENDIAN32(code0);
        code1 = REVERSE_ENDIAN32(code1);
        eopic0 = *(UINT32 *)(&g_NalTypeEOPIC[4]);
        eopic1 = *(UINT32 *)(&g_NalTypeEOPIC[8]);


        if ( (code0==eopic0) && (code1==eopic1) )
        {
            REPORT_MEET_NEWFRM(pH264Ctx->ChanID);
            pH264Ctx->StartMode = 0;
            ret = DecVDM(pH264Ctx);  // ����VDM����
            if (H264_OK != ret)
            {
	            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
                return H264_ERR;
            }
        }

        break;
    default:

            
            if ((NALU_TYPE_NULL == pH264Ctx->pCurrNal->nal_unit_type) ||
                ((pH264Ctx->pCurrNal->nal_unit_type >= NALU_TYPE_UNSPECIFIED) &&
                 (pH264Ctx->pCurrNal->nal_unit_type <= NALU_TYPE_END)))
            {
                dprint(PRN_DBG, "**1** NAL: UNSUPPORT, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
            }
            else
            {
                dprint(PRN_FATAL, "***** NAL: UNSUPPORT, nal_unit_type=%d\n", pH264Ctx->pCurrNal->nal_unit_type);
                //20150422: decoder not report the unsupport event.
                //REPORT_UNSUPPORT(pH264Ctx->ChanID);
            }

            ClearCurrNal(pH264Ctx);
            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
            break;
    }

    return H264_OK;
}


/* ��һ��seg�ڲ�����0���صĸ���
   ����ֵ: 0���ظ������������ֵ����len*8���������seg��ȫ��0�ֽ����.
*/
#define TZ_BUF_SIZE  64
UINT8  TmpBuf[TZ_BUF_SIZE + 4];
SINT32  FindZeroBitsInSeg(UINT8 *ptr, SINT32 len)
{
    UINT8 *pLastByte, Byte, *ThisRoundPtr;
    SINT32 zero_byte = 0, zero_bit = 0;
    SINT32 nRound, i, r;
    SINT32 TotalLeftSize, ThisRoundSize;

#if 1
    nRound = (len / TZ_BUF_SIZE) + ((len % TZ_BUF_SIZE) != 0);
    for (r = nRound; r > 0; r--)
    {
        TotalLeftSize = (len - (nRound-r)*TZ_BUF_SIZE);

        ThisRoundSize = (TotalLeftSize >= TZ_BUF_SIZE)? TZ_BUF_SIZE : TotalLeftSize;
        ThisRoundPtr  = ptr + TotalLeftSize - ThisRoundSize;

        memcpy(TmpBuf, ThisRoundPtr, ThisRoundSize);
        pLastByte = TmpBuf + ThisRoundSize - 1;
        while(0x00 == *pLastByte)
        {
            zero_byte++;
            if(pLastByte == TmpBuf)
            {
                break;
            }

            pLastByte--;
        }

        if (*pLastByte != 0)
        {
            break;
        }
    }

#else
    pLastByte = ptr + len - 1;
    while(0x00 == *pLastByte)
    {
        zero_byte++;
        if(pLastByte == ptr)
        {
            break;
        }

        pLastByte--;
    }
#endif

    /* �ڷ����ֽ�������0���ظ��� */
    if (zero_byte < len)
    {
        Byte = *(ptr + len - zero_byte - 1);
        for (i = 0; i < 8; i++)
        {
            if ((Byte & 1) != 0)
            {
                break;
            }
            Byte >>= 1;
            zero_bit++;
        }
    }

    return (zero_byte * 8 + zero_bit);
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 FindTrailZeros(H264_CTX_S *pH264Ctx)
*      �������� :  ����β��ĺ���
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL; DEC_ERR;
************************************************************************
*/
SINT32 FindTrailZeros(H264_CTX_S *pH264Ctx)
{
    SINT32 zero_bit;

    zero_bit = -1;
    if (pH264Ctx->pCurrNal->nal_segment > 1)
    {
        zero_bit = FindZeroBitsInSeg(pH264Ctx->pCurrNal->stream[1].streamBuffer,
            pH264Ctx->pCurrNal->stream[1].bitstream_length);

        if (zero_bit >= pH264Ctx->pCurrNal->stream[1].bitstream_length * 8)
        {
            /* ��nal������seg��ɣ����ҵڶ���segȫΪ0���ͷŵ��ڶ���seg */
            SM_ReleaseStreamSeg(pH264Ctx->ChanID, pH264Ctx->pCurrNal->stream[1].StreamID);
            pH264Ctx->pCurrNal->stream[1].streamBuffer = NULL;
            pH264Ctx->pCurrNal->nal_segment = 1;
            zero_bit = -1;
        }
    }

    /* ���nalֻ��һ��seg����������2��seg���ڶ���segȫ0��������һ��seg����β0 */
    if(zero_bit == -1)
    {
        zero_bit = FindZeroBitsInSeg(pH264Ctx->pCurrNal->stream[0].streamBuffer,
            pH264Ctx->pCurrNal->stream[0].bitstream_length);
    }

	/* padding = 1000000..., ������Ҫȥ���ı����� = zero_bit + 1 */
    pH264Ctx->pCurrNal->nal_trail_zero_bit_num = zero_bit + 1;

    if (pH264Ctx->pCurrNal->nal_segment > 1)
    {
		if (pH264Ctx->pCurrNal->nal_trail_zero_bit_num >= pH264Ctx->pCurrNal->stream[1].bitstream_length * 8)
		{
			/* �ο�: BVTSDK-2268: ��β1����ȥ��������VDH����cabac������β1Ҳ��slice�Ľ��� */
            /* ������seg, ���ڶ���segֻ������β1+������β0�����Եڶ���segӦ�ñ����� */
            SM_ReleaseStreamSeg(pH264Ctx->ChanID, pH264Ctx->pCurrNal->stream[1].StreamID);
            pH264Ctx->pCurrNal->stream[1].streamBuffer = NULL;
            pH264Ctx->pCurrNal->nal_segment = 1;
			pH264Ctx->pCurrNal->nal_trail_zero_bit_num = 0;
		}
    }

    return H264_OK;
}

/*!
************************************************************************
*      ����ԭ�� :  VOID CombinePacket(VOID)
*      �������� :  ƴ�����ܺ���
*      ����˵�� :  ��
*      ����ֵ   �� VOID
************************************************************************
*/
VOID CombinePacket(H264_CTX_S *pH264Ctx)
{
    SINT32 StreamId;

    // ��ʾ�϶��ְ��ˣ�Ҳ����˵�϶�Buf������
    if (pH264Ctx->pCurrNal->stream[0].bitstream_length < GET_ONE_NALU_SIZE) // ��Ҫcopy������buf��ͷ��
    {
        dprint(PRN_STREAM, "\n\nEND of the bit buffer, copy the first packet!\n\n");

        // copy����ǰ��
        dprint(PRN_STREAM, "0:phy:0x%x, 1:phy:0x%x; len0:%d len1:%d\n",pH264Ctx->pCurrNal->stream[0].stream_phy_addr,
            pH264Ctx->pCurrNal->stream[1].stream_phy_addr, pH264Ctx->pCurrNal->stream[0].bitstream_length,
            pH264Ctx->pCurrNal->stream[1].bitstream_length);
        memcpy(pH264Ctx->pCurrNal->stream[1].streamBuffer - pH264Ctx->pCurrNal->stream[0].bitstream_length, \
            pH264Ctx->pCurrNal->stream[0].streamBuffer, pH264Ctx->pCurrNal->stream[0].bitstream_length);
        // ԭ����λ���ͷŵ�
        StreamId =  pH264Ctx->pCurrNal->stream[0].StreamID;
        pH264Ctx->pCurrNal->stream[0].StreamID = pH264Ctx->pCurrNal->stream[1].StreamID;
        pH264Ctx->pCurrNal->stream[0].bitsoffset = 0;
        pH264Ctx->pCurrNal->stream[0].streamBuffer = pH264Ctx->pCurrNal->stream[1].streamBuffer - pH264Ctx->pCurrNal->stream[0].bitstream_length;
        pH264Ctx->pCurrNal->stream[0].stream_phy_addr = pH264Ctx->pCurrNal->stream[1].stream_phy_addr - pH264Ctx->pCurrNal->stream[0].bitstream_length;
        pH264Ctx->pCurrNal->stream[0].bitstream_length = pH264Ctx->pCurrNal->stream[0].bitstream_length + pH264Ctx->pCurrNal->stream[1].bitstream_length;    //include nal header
        pH264Ctx->pCurrNal->nal_segment = 1;
        dprint(PRN_STREAM, "0:phy:0x%x, len0:%d\n",pH264Ctx->pCurrNal->stream[0].stream_phy_addr,pH264Ctx->pCurrNal->stream[0].bitstream_length);

        SM_ReleaseStreamSeg(pH264Ctx->ChanID, StreamId);
        pH264Ctx->pCurrNal->stream[1].streamBuffer = NULL;
        pH264Ctx->pCurrNal->stream[1].bitsoffset = 0;
        pH264Ctx->pCurrNal->stream[1].bitstream_length = 0;
    }
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 ReceivePacket(H264_STREAM_PACKET *pPacket)
*      �������� :  ��ȡһƬ���������жϸ�Ƭ�����Ƿ���������nal
*      ����˵�� :  ��
*      ����ֵ   �� 1:һ��������Nalu;0����һ��������Nalu;DEC_ERR:�������
************************************************************************
*/
//����������������,�������ִ�������Ҫ����,������ʱ�ȷ�������
SINT32 ReceivePacket(H264_CTX_S *pH264Ctx, H264_STREAM_PACKET *pPacket)
{
    SINT32 ret;

    dprint(PRN_STREAM, "\n receive streambuff=%p; phy_addr=0x%x; bitstream_length=%d\n", pPacket->p_stream, pPacket->strm_phy_addr, pPacket->stream_len);
    pH264Ctx->u32SCDWrapAround = 0;

    /* ���pCurrNal�Ƿ�����: ���pCurrNal���Ѿ����������һ�����ҵ�ǰ������LastSeg������pCurrNal�Ѿ���������nal����˵��pCurrNal״̬�Ѿ�����
       ��Ҫ����������Ա�֤�����������յ�ǰseg. */
    if((1 != pH264Ctx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) || \
       ((1 == pH264Ctx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) && \
       (0 == pH264Ctx->pstExtraData->s32WaitFsFlag)))
    {
        if((NULL != pH264Ctx->pCurrNal) && ((1 != pPacket->last_pack_in_nal) || (2 <= pH264Ctx->pCurrNal->nal_segment) || (1 == pH264Ctx->pCurrNal->nal_integ)) )
        {
            dprint(PRN_ERROR, "nal_release_err\n");
            ClearCurrNal(pH264Ctx);
        }
    }

    // ��һ���յ���
    if (NULL == pH264Ctx->pCurrNal)
    {
        // �յ�����İ�
        if ((0 < pPacket->stream_len) && (NULL != pPacket->p_stream) && (0 != pPacket->strm_phy_addr))
        {
            // find a good Nalu Slot to save current Nalu
            ret = FindNaluArraySlot(pH264Ctx);
            if (H264_ERR == ret)
            {
                dprint(PRN_FATAL, "cann't find slot for current nal\n");

                //����Ҳ�����Nalu Array Slot, �������ǼӴ�����
                ClearAllNal(pH264Ctx);
                H264_ClearCurrPic(pH264Ctx);
                ClearDPB(pH264Ctx);

                REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
                return H264_ERR;
            }
            pH264Ctx->pCurrNal = &(pH264Ctx->NalArray[ret]);

            pH264Ctx->pCurrNal->stream[0].streamBuffer = pPacket->p_stream;
            pH264Ctx->pCurrNal->stream[0].bitstream_length = pPacket->stream_len;    // include nal header
            pH264Ctx->pCurrNal->stream[0].stream_phy_addr = pPacket->strm_phy_addr;
            pH264Ctx->pCurrNal->stream[0].bitsoffset = 0;
            pH264Ctx->pCurrNal->stream[0].StreamID =  pPacket->StreamID;
            pH264Ctx->pCurrNal->PTS = pPacket->pts;
            pH264Ctx->pCurrNal->nal_segment = 1;
            pH264Ctx->pCurrNal->nal_used_segment = 0;
            pH264Ctx->pCurrNal->nal_integ = pPacket->last_pack_in_nal;
        }
        else
        {
            dprint(PRN_ERROR, "receive a zero packet\n");
            SM_ReleaseStreamSeg(pH264Ctx->ChanID, pPacket->StreamID);
            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
            return H264_ERR;
        }
    }
    else
    {
        // �յ��ڶ�������
        pH264Ctx->pCurrNal->nal_integ = pPacket->last_pack_in_nal;
       if((1 != pH264Ctx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) || \
       ((1 == pH264Ctx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) && \
       (0 == pH264Ctx->pstExtraData->s32WaitFsFlag)))
       {
        if ((0 < pPacket->stream_len) && (NULL != pPacket->p_stream) && (0 != pPacket->strm_phy_addr) && (pH264Ctx->pCurrNal->nal_segment <= 1))
        {
            pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_segment].streamBuffer = pPacket->p_stream;
            pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_segment].bitstream_length = pPacket->stream_len;    // include nal header
            pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_segment].stream_phy_addr = pPacket->strm_phy_addr;
            pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_segment].bitsoffset = 0;
            pH264Ctx->pCurrNal->stream[pH264Ctx->pCurrNal->nal_segment].StreamID =  pPacket->StreamID;
            pH264Ctx->pCurrNal->nal_segment++;

            // ƴ�����ܺ���
            CombinePacket(pH264Ctx);
            pH264Ctx->u32SCDWrapAround = 1;
        }
        else
        {
            // �ڶ�������Ϊ0��������ַΪ��,��ô��ֻ���һ������
            pH264Ctx->pCurrNal->nal_integ = 1;
            // �ڶ���ֻ��Ҫrelease
            SM_ReleaseStreamSeg(pH264Ctx->ChanID, pPacket->StreamID);
            REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
        }
       }
    }
	#ifdef VFMW_SCD_LOWDLY_SUPPORT
    pH264Ctx->pCurrNal->dsp_check_pic_over_flag = pPacket->pack_is_pic_over_flag;
    #endif
    ret = pH264Ctx->pCurrNal->nal_integ ? H264_OK : H264_ERR;

    return ret;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 FindNaluArraySlot(H264_CTX_S *pH264Ctx)
*      �������� :  find a good Nalu Slot to save current Nalu
*      ����˵�� :  ��
*      ����ֵ   �� ���ؿյ�Nalu�����п��õ�λ��
************************************************************************
*/
SINT32 FindNaluArraySlot(H264_CTX_S *pH264Ctx)
{
    UINT32 Pos;
    UINT32 FindPosFlag;

    FindPosFlag = 0;

    for (Pos=0; Pos<MAX_SLICE_SLOT_NUM+1; Pos++)
    {
        if (0 == pH264Ctx->NalArray[Pos].is_valid)
        {
            FindPosFlag = 1;
            break;
        }
    }

    if ( (1 == FindPosFlag) && (Pos < (MAX_SLICE_SLOT_NUM+1)) )
    {
        pH264Ctx->NalArray[Pos].is_valid = 1;
        return ((SINT32)Pos);
    }
    else
    {
        return H264_ERR;
    }
}

SINT32 InquireSliceExtProperty(H264_CTX_S *pH264Ctx)
{
    UINT32 nal_header;
    nal_header = BsGet(&(pH264Ctx->BS), 24);//parse 24 bit
    pH264Ctx->pCurrNal->svc_extension_flag = (nal_header >> 23)  & 1;
    if(pH264Ctx->pCurrNal->svc_extension_flag)
    {
        pH264Ctx->SVC_NALUSVCEXT_S.idr_flag = (nal_header >> 22) & 1;
        pH264Ctx->SVC_NALUSVCEXT_S.priority_id = (nal_header >> 16) & 0x3f;
        pH264Ctx->SVC_NALUSVCEXT_S.no_inter_layer_pred_flag = (nal_header >> 15) & 1;
        pH264Ctx->SVC_NALUSVCEXT_S.dependency_id = (nal_header >> 12) & 7;
        pH264Ctx->SVC_NALUSVCEXT_S.quality_id = (nal_header >> 8) & 0xf;
        pH264Ctx->SVC_NALUSVCEXT_S.temporal_id = (nal_header >> 5) & 7;
        pH264Ctx->SVC_NALUSVCEXT_S.use_ref_base_pic_flag = (nal_header >> 4) & 1;
        pH264Ctx->SVC_NALUSVCEXT_S.discardable_flag = (nal_header >> 3) & 1;
        pH264Ctx->SVC_NALUSVCEXT_S.output_flag = (nal_header >> 2) & 1;
        pH264Ctx->SVC_NALUSVCEXT_S.reserved_three_2bits = nal_header & 3;
    }

    if(pH264Ctx->SVC_NALUSVCEXT_S.idr_flag == 1)
    {
        pH264Ctx->pCurrNal->nal_unit_type = NALU_TYPE_IDR;
    }
    return H264_OK;
}

SINT32 InquireSliceProperty(H264_CTX_S *pH264Ctx, SINT32 *IsNewPic, SINT32 *IsSizeChanged)
{
    SINT32 ret;
    H264_SPS_S *pSPS;
    H264_PPS_S *pPPS;
    SINT32 oldw, oldh, neww, newh;

    dprint(PRN_DBG, "\nSlice nal or IDR nal = %d\n", pH264Ctx->TotalNal - 1);

    *IsNewPic = *IsSizeChanged = 0;

    pH264Ctx->CurrSlice.nal_unit_type = pH264Ctx->pCurrNal->nal_unit_type;
    pH264Ctx->CurrSlice.nal_ref_idc = pH264Ctx->pCurrNal->nal_ref_idc;

    if (H264_OK != SliceCheck(pH264Ctx))
    {
        return H264_ERR;
    }

    ret = ProcessSliceHeaderFirstPart(pH264Ctx);
    if (H264_OK != ret)
    {
        dprint(PRN_ERROR, "sliceheader dec err\n");
        return H264_ERR;
    }

    *IsNewPic = (1 == pH264Ctx->CurrSlice.new_pic_flag)? 1: 0;

    /* �жϸ�slice��ͼ��ߴ��Ƿ��֮ǰ�����˱仯 */
    pPPS = &(pH264Ctx->PPS[pH264Ctx->CurrSlice.pic_parameter_set_id]);
    pSPS = &(pH264Ctx->SPS[pPPS->seq_parameter_set_id]);

    oldw = pH264Ctx->CurWidthInMb;
    oldh = pH264Ctx->CurHeightInMb;
    neww = pSPS->pic_width_in_mbs_minus1 + 1;
    newh = (pSPS->pic_height_in_map_units_minus1 + 1)*(2 - pSPS->frame_mbs_only_flag);
    if ((oldw != neww) || (oldh != newh))
    {
        *IsSizeChanged = 1;
    }

    /*  z56361, 20111202,���ͼ��ߴ�û�仯��DPB����ˣ�Ҳ��Ҫ���»���֡��أ�������ͼ��ߴ�仯������ͬ������Ҳ�鵽�ߴ�仯 */
    if (pSPS->dpb_size_plus1 > (pH264Ctx->DPB.size+1))
    {
        //dprint(PRN_ALWS, "InquireSliceProperty: dpb_size:%d -> %d\n", (pH264Ctx->DPB.size+1), pSPS->dpb_size_plus1);
        *IsSizeChanged = 1;
    }

    if (1 == *IsSizeChanged && pH264Ctx->TotalPicNum != 0 && oldw!=0)
    {
        REPORT_FAKE_FRAME(pH264Ctx->ChanID);
    }

    return H264_OK;
}


SINT32 HaveSliceToDec(H264_CTX_S *pH264Ctx)
{
    if (pH264Ctx->SliceParaNum > 0)
    {
        return H264_OK;
    }
    else
    {
        return H264_ERR;
    }
}


/* ���ο��б��Ƿ��д��� */
SINT32 IsRefListWrong(H264_CTX_S *pH264Ctx)
{
    SINT32 i, ret = 0;
    SINT32 FindExistFrame;
    FSP_LOGIC_FS_S *pstLogicFs;

    /* ���list 0 */
    if (pH264Ctx->CurrSlice.listXsize[0] > 0)
    {
        FindExistFrame = 0;
        for (i=0; i<pH264Ctx->CurrSlice.listXsize[0]; i++)
        {
            pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[0][i]->frame_store->logic_fs_id);
            if((NULL == pstLogicFs) || ((NULL != pstLogicFs) && (NULL == pstLogicFs->pstDecodeFs)))
            {
                ret = 1;
                break;
            }

            if (pH264Ctx->pListX[0][i]->frame_store->non_existing ==0)
            {
                FindExistFrame = 1;
            }
        }
        //ret = (1 == FindExistFrame)? ret : 1;
    }

    /* ��list 0������󣬶���list1�ǿգ��������list 1 */
    if ((0 == ret) && (pH264Ctx->CurrSlice.listXsize[0] > 0))
    {
        FindExistFrame = 0;
        for (i=0; i<pH264Ctx->CurrSlice.listXsize[1]; i++)
        {
            pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, pH264Ctx->pListX[1][i]->frame_store->logic_fs_id);
            if((NULL == pstLogicFs) || ((NULL != pstLogicFs) && (NULL == pstLogicFs->pstDecodeFs)))
            {
                ret = 1;
                break;
            }

            if (pH264Ctx->pListX[1][i]->frame_store->non_existing == 0)
            {
                FindExistFrame = 1;
            }
        }
        //ret = (1 == FindExistFrame)? ret : 1;
    }

    ret = ret? H264_ERR: H264_OK;

    return ret;
}


/***********************************************************************************
* Function:    H264DEC_Init(H264_CTX_S *pH264Ctx)
* Description: ȫ�ֳ�ʼ��264����ģ��,���·����ַ
* Input:
* Output:
* Return:      ��ʼ���ɹ�����ʧ��
*
* Others:      ��
***********************************************************************************/
SINT32 H264DEC_Init(H264_CTX_S *pH264Ctx, SYNTAX_EXTRA_DATA_S *pstExtraData)
{
    UINT32 MemOffset = 0;
    UINT8 *pCtx_Vir = NULL;
    SINT32 ImgQueRstMagic;

    // ���������
    ImgQueRstMagic = pH264Ctx->ImageQue.ResetMagicWord;
    memset( pH264Ctx, 0, sizeof(H264_CTX_S) );
    pH264Ctx->ImageQue.ResetMagicWord = ImgQueRstMagic;
    ResetVoQueue(&pH264Ctx->ImageQue);

    pH264Ctx->pstExtraData = pstExtraData;
    pH264Ctx->pH264DecParam = NULL;
    pH264Ctx->StartMode = 0;

    // init SPS PPS SLICE_PARA
    if (pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION)
    {
        pH264Ctx->MaxSpsNum = pH264Ctx->pstExtraData->stChanOption.s32MaxSpsNum;
        pH264Ctx->MaxPpsNum = pH264Ctx->pstExtraData->stChanOption.s32MaxPpsNum;
        pH264Ctx->MaxSliceNum = pH264Ctx->pstExtraData->stChanOption.s32MaxSliceNum;
    }
	else
    {
        pH264Ctx->MaxSpsNum = MAX_SPS_NUM;
        pH264Ctx->MaxPpsNum = MAX_PPS_NUM;
        pH264Ctx->MaxSliceNum = MAX_SLICE_SLOT_NUM;
    }
    
    // ��ʼ��VPS SPS PPS
    pCtx_Vir = (UINT8 *)MEM_Phy2Vir(pH264Ctx->pstExtraData->s32ContextMemAddr);
    if (NULL == pCtx_Vir)
    {
        dprint(PRN_FATAL, "%s: Get context addr failed!\n", __func__);
        return VF_ERR_SYS;
    }
    MemOffset  = pH264Ctx->pstExtraData->s32BasicCtxSize;
    
    pH264Ctx->SPS = (H264_SPS_S*)(pCtx_Vir + MemOffset);
    MemOffset += sizeof(H264_SPS_S)*pH264Ctx->MaxSpsNum;
    
    pH264Ctx->PPS = (H264_PPS_S*)(pCtx_Vir + MemOffset);
    MemOffset += sizeof(H264_PPS_S)*pH264Ctx->MaxPpsNum;
    
    pH264Ctx->DecSlicePara = (H264_DEC_SLICE_PARAM_S*)(pCtx_Vir + MemOffset);
    MemOffset += sizeof(H264_DEC_SLICE_PARAM_S)*pH264Ctx->MaxSliceNum;

    if (MemOffset > pH264Ctx->pstExtraData->s32ContextMemSize)
    {
        dprint(PRN_FATAL, "%s FATAL: MemOffset %d > ContextMemSize %d.\n", __func__, MemOffset, pH264Ctx->pstExtraData->s32ContextMemSize);
        return VF_ERR_SYS;
    }

    if(NULL == pH264Ctx->SPS || NULL == pH264Ctx->PPS|| NULL == pH264Ctx->DecSlicePara)
    {
    	dprint(PRN_FATAL, "%s %d unknow error!!\n",__FUNCTION__,__LINE__);
    	return VF_ERR_SYS;
    }

    if (0 == pstExtraData->s32KeepSPSPPS
	 && pH264Ctx->SPS != NULL
	 && pH264Ctx->PPS != NULL)
    {
        memset(pH264Ctx->SPS, 0, pH264Ctx->MaxSpsNum*sizeof(H264_SPS_S));
        memset(pH264Ctx->PPS, 0, pH264Ctx->MaxPpsNum*sizeof(H264_PPS_S));
    }
    pH264Ctx->CurrPPS.pic_parameter_set_id = pH264Ctx->MaxPpsNum;
    pH264Ctx->CurrSPS.seq_parameter_set_id = pH264Ctx->MaxSpsNum;
	memset(pH264Ctx->DecSlicePara, 0, sizeof(H264_DEC_SLICE_PARAM_S)*pH264Ctx->MaxSliceNum);

    pH264Ctx->TotalNal = 0;
    pH264Ctx->TotalSPS = 0;
    pH264Ctx->TotalPPS = 0;
    pH264Ctx->TotalSlice = 0;
    pH264Ctx->SeqCnt = 0;
    pH264Ctx->TotalPicNum = 0;
    pH264Ctx->TotalFrameNum = 0;
    pH264Ctx->TotalFieldNum = 0;
    pH264Ctx->TotalFsNum  = MAX_FRAME_STORE;   // change while g_DPB.size change
    pH264Ctx->TotalPmvNum = MAX_PMV_STORE;     // change while g_DPB.size change
    pH264Ctx->MutexHold = 0;
    pH264Ctx->TotalUsrDatNum = 0;
    pH264Ctx->pUsrDatArray[0] = NULL;
    pH264Ctx->pUsrDatArray[1] = NULL;
    pH264Ctx->pUsrDatArray[2] = NULL;
    pH264Ctx->pUsrDatArray[3] = NULL;

    pH264Ctx->AllowStartDec = 0;
    pH264Ctx->IDRFound = 0;
    pH264Ctx->FirstFrameDisplayed = FIRST_DISP_INIT;
    pH264Ctx->PPicFound = 0;
    pH264Ctx->frame_rate = 25;

    pH264Ctx->ChanID = VCTRL_GetChanIDByCtx(pH264Ctx);
    if ( -1 == pH264Ctx->ChanID )
    {
        dprint(PRN_FATAL, "-1 == VCTRL_GetChanIDByCtx() Err! \n");
        return VF_ERR_SYS;
    }

    ClearAll(pH264Ctx, 0);
    
    pH264Ctx->bIsInit = 1;

    return VF_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  H264DEC_Destroy(H264_CTX_S *pH264Ctx)
*   �������� :  ����h264������
*   ����˵�� :  ��
*   ����ֵ   �� ��
************************************************************************
*/
VOID H264DEC_Destroy(H264_CTX_S *pH264Ctx)
{
    UINT32 i;
    UINT32 usrdat_idx;

    H264_ClearCurrPic(pH264Ctx);
    ClearAllNal(pH264Ctx);

    if ( 1 == pH264Ctx->MutexHold)
    {
        pH264Ctx->MutexHold = 0;
    }

    for (i=0; i<pH264Ctx->TotalUsrDatNum; i++)
    {
        if (NULL != pH264Ctx->pUsrDatArray[i])
        {
            FreeUsdByDec(pH264Ctx->ChanID,  pH264Ctx->pUsrDatArray[i]);
            pH264Ctx->pUsrDatArray[i] = NULL;
        }
    }

    for (i=0; i<MAX_FRAME_STORE; i++)
    {
        for (usrdat_idx=0; usrdat_idx <4; usrdat_idx++)
        {
            if (NULL != pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx])
            {
                FreeUsdByDec(pH264Ctx->ChanID, pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx]);
                pH264Ctx->FrameStore[i].fs_image.p_usrdat[usrdat_idx] = NULL;
            }
        }
    }

    dprint(PRN_CTRL, "H264Dec destroy\n");

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 H264DEC_DecodePacket(H264_STREAM_PACKET *pPacket)
*      �������� :  ��ȡһƬ���������жϸ�Ƭ�����Ƿ���������nal
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
SINT32 H264DEC_DecodePacket(H264_CTX_S *pH264Ctx, H264_STREAM_PACKET *pPacket)
{
    SINT32 ret = 0;
    UINT8 nal_header;
    UINT8 nal_unit_type;
    SINT32 RefNum, ReadNum, NewNum;

    if (pH264Ctx->bIsInit != 1)
    {
    	dprint(PRN_FATAL, "H264Ctx not init!\n");
        return H264_ERR;
    }
    
    RefNum = ReadNum = NewNum = 0;
    CHECK_NULL_PTR_ReturnValue(pPacket, H264_ERR);

    pH264Ctx->advanced_cfg  = (pH264Ctx->pstExtraData->s32DecMode << 16) & 0x70000;
    pH264Ctx->ref_error_thr = pH264Ctx->pstExtraData->s32RefErrThr;
    pH264Ctx->out_error_thr = pH264Ctx->pstExtraData->s32OutErrThr;
    pH264Ctx->SeErrFlg      = 0;

    // ���յ�һ����������ƴ��һ��������Nalu, ע��RecivePacket����������ֵ
    if (1 != pH264Ctx->HaveNalNotDecoded)
    {
        if((1 == pH264Ctx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) && \
           (1 == pH264Ctx->pstExtraData->s32WaitFsFlag) && \
           (1 == pH264Ctx->u32SCDWrapAround))
        {
            pH264Ctx->pCurrNal = pH264Ctx->pLastNal;
            pH264Ctx->u32SCDWrapAround = 0;
        }
        else
        {
            ret = ReceivePacket(pH264Ctx, pPacket);
            pH264Ctx->pLastNal = pH264Ctx->pCurrNal;
        }
    }
    else
    {
        ret = (NULL != pH264Ctx->pCurrNal) ? H264_OK : H264_ERR;
    }
    pH264Ctx->HaveNalNotDecoded = 0;  //��Ϊ��NALһ���ᱻ�õ�����������new_pic_flagʱ�����д���

    // �յ�һ��������Nalu��ʼ����
    if (H264_OK == ret)
    {
        //ȡһ��NALͷ�����ж��Ƿ�new pic
        pH264Ctx->pCurrNal->nal_bitoffset = 0;
        BsInit(&(pH264Ctx->BS), (UINT8 *)((pH264Ctx->pCurrNal->stream[0].streamBuffer)), pH264Ctx->pCurrNal->stream[0].bitstream_length);

        nal_header = BsGet(&(pH264Ctx->BS), 32);     // ȡ��һ��32bit, 0x00 0x00 0x01 0xxx
        pH264Ctx->pCurrNal->nal_unit_type = nal_unit_type = (UINT32)(nal_header & 0x1F);
        pH264Ctx->pCurrNal->nal_ref_idc   = (UINT32)((nal_header >> 5) & 3);
        if ( (NALU_TYPE_SLICE==nal_unit_type) || (NALU_TYPE_IDR==nal_unit_type)  || (NALU_TYPE_SLC_EXT ==nal_unit_type))//add svc by w283384
        {
            SINT32  IsNewPic, IsSizeChanged;
            if(NALU_TYPE_SLC_EXT ==nal_unit_type)
            {
                InquireSliceExtProperty(pH264Ctx);//�˴�Ҫ����svc��ص�24��bit������ԭ��:����idr �ͺ�������ƫ����ȷ
            }
            if ( H264_OK == InquireSliceProperty(pH264Ctx, &IsNewPic, &IsSizeChanged) )
            {
                pH264Ctx->PicCounter += (1 == IsNewPic);  /* 32λ����ζ�źܶ�֡��30fps�ɲ�4.4�꣬��˲�����32λ������� */
				if (1 == IsNewPic)
				{
                    if (pH264Ctx->CurrSlice.field_pic_flag == 1)
                    {
                        pH264Ctx->NewPicCounter = pH264Ctx->NewPicCounter + 1;
                    }
                    else
                    {
                        pH264Ctx->NewPicCounter = pH264Ctx->NewPicCounter + 2;
                    }
				}
                /*
                   ����һ֡��ͨ��dsp_check_pic_over_flag��־������VDH����ģ�����ʱ����
                   ��ʹ��֮ǰ�ķ���������VHD���룬��ʱֻ�������־λ PicIsDecoded �Ϳ���
                   �ˡ�
                */
				#ifdef VFMW_SCD_LOWDLY_SUPPORT
                if (1 == pH264Ctx->PicIsDecoded)
                {
                    pH264Ctx->PicIsDecoded = 0;
                }
                else
				#endif
                {
	                if ( (1 == IsNewPic) && (H264_OK == HaveSliceToDec(pH264Ctx)) )  //�������Sliceδ���룬˵����δ����NALU_TYPE_EOPIC������������һ��NALU_TYPE_EOPIC
	                {
	                    pH264Ctx->pCurrNal->nal_bitoffset = 0;
	                    BsInit(&(pH264Ctx->BS), (UINT8 *)(g_NalTypeEOPIC), sizeof(g_NalTypeEOPIC));
	                    H264_DecOneNal(pH264Ctx, 1);
	                    if (pH264Ctx->CurrSlice.field_pic_flag == 1)
	                    {
	                        pH264Ctx->NewPicCounter = pH264Ctx->NewPicCounter - 1;
	                    }
	                    else
	                    {
	                        pH264Ctx->NewPicCounter = pH264Ctx->NewPicCounter - 2;
	                    }
	                    pH264Ctx->HaveNalNotDecoded = 1;  //��ס�ղ���һ��NALδ����
	                    return H264_NOTDEC;
                    }
                }

                /* ���ͼ��ߴ��ڴ�slice�����˱仯��Ӧ�õ�֮ǰ�ѽ����ͼ��ȫ������󼶺��ټ����³ߴ�ͼ��Ľ��룬����
                   �ϳߴ��ͼ����ܻ��н϶�Ķ��������²���Ч�����ѡ�z56361, 2011-11-1 */
                if (1 == IsSizeChanged)
                {
		    		ret = FlushDPB(pH264Ctx);
		           	if (H264_OK != ret)
		           	{
		                dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
		            }

                    VCTRL_GetChanImgNum(pH264Ctx->ChanID, &RefNum, &ReadNum, &NewNum);
                    
                    if (pH264Ctx->pstExtraData->stChanOption.s32ReRangeEn == 1)
                    {
                     #ifdef PRODUCT_KIRIN
                        if (NewNum > 0)
                        {
                            pH264Ctx->HaveNalNotDecoded = 1;  //��ס�ղ���һ��NALδ����(��ԭ�ߴ��֡����)
                            return H264_NOTDEC;
                        }
                     #else
                        if (ReadNum + NewNum > 0)
                        {
                            pH264Ctx->HaveNalNotDecoded = 1;  //��ס�ղ���һ��NALδ����(��ԭ�ߴ��֡����)
                            return H264_NOTDEC;
                        }
                     #endif
                    }
                }
            }
            else
            {
                ClearCurrNal(pH264Ctx);
                REPORT_DECSYNTAX_ERR(pH264Ctx->ChanID);
                return H264_ERR;
            }
        }

        // ȥ��β�㺯��
        ret = FindTrailZeros(pH264Ctx);

        // ��g_BS�е�ֵ, BS�ĳ�ʼ����Ҫ��word�����λ�ÿ�ʼ
        pH264Ctx->pCurrNal->nal_bitoffset = 0;
        BsInit(&(pH264Ctx->BS), (UINT8 *)((pH264Ctx->pCurrNal->stream[0].streamBuffer)), pH264Ctx->pCurrNal->stream[0].bitstream_length);
        ret = H264_DecOneNal(pH264Ctx, 0);
        if(H264_NOTDEC == ret)
        {
            return ret;
        }
        else if (H264_OK != ret)
        {
            dprint(PRN_DBG, "H264_DecOneNal ERR\n");
            return H264_ERR;
        }
        else
        {
            return H264_OK;
        }
    }
    
    return H264_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID H264DEC_RecycleImage(H264_CTX_S *pH264Ctx, UINT32 PicID)
*   �������� :  h264����VO�Ѿ���ʾ��ͼ���λ
*   ����˵�� :
*   ����ֵ   ��
************************************************************************
*/
SINT32 H264DEC_RecycleImage(H264_CTX_S *pH264Ctx, UINT32 PicID)
{
    UINT32 i;
    FSP_LOGIC_FS_S *pstLogicFs;

    dprint(PRN_CTRL, "H264 recycle image\n");

    /* �ҵ�PicID����Ӧ���߼�֡�棬�Ӷ��ҵ�image�ṹ�壬���������ͷ�image��������userdata */
    pstLogicFs = FSP_GetLogicFs(pH264Ctx->ChanID, PicID);
    if (NULL != pstLogicFs)
    {
        /* �������߼�֡���Ƿ����ڵȴ���ʾ�ͷ� */
        if (pstLogicFs->s32DispState == FS_DISP_STATE_NOT_USED)
        {
            return H264_ERR;
        }

        /* �ͷ�userdata */
        for (i = 0; i < 4; i++)
        {
            if (NULL != pstLogicFs->stDispImg.p_usrdat[i])
            {
                FreeUsdByDec(pH264Ctx->ChanID, pstLogicFs->stDispImg.p_usrdat[i]);
                pstLogicFs->stDispImg.p_usrdat[i] = NULL;
            }
        }

        /* ��ͼ��'PicID'���Ϊ"��ʾ����" */
        FSP_SetDisplay(pH264Ctx->ChanID, PicID, 0);
        return H264_OK;
    }
    return H264_ERR;
}


UINT32 H264DEC_VDMPostProc( H264_CTX_S *pH264Ctx, SINT32 ErrRatio, SINT32 CalledByISR, LUMA_INFO_S *pLumaInfo, SINT32 ModuleLowlyEnable,SINT32 VdhId)
{
    SINT32 ret;

    if (0 == pH264Ctx->StartMode)
    {
        pH264Ctx->CurrPic.err_level = ErrRatio;
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
		if(ModuleLowlyEnable)
		{
			if(g_VdmDrvParam[VdhId].ChanIntState == 3)
			{
                ClearAllSlice(pH264Ctx);
				return H264_OK;
			}
			if(g_VdmDrvParam[VdhId].ChanIntState == 1)
			{
			    ClearAllSlice(pH264Ctx);
			}
		}
		else
		{
            ClearAllSlice(pH264Ctx);
		}
#else
		ClearAllSlice(pH264Ctx);
#endif
        ret = StorePicInDPB(pH264Ctx, pLumaInfo);
        if (H264_OK != ret && H264_ALREADY_OUT != ret)
        {
            FSP_ClearLogicFs(pH264Ctx->ChanID, pH264Ctx->CurrPic.frame_store->logic_fs_id, 1);
            dprint(PRN_ERROR, "store pic err, ret = %d\n", ret);

            H264_ClearCurrPic(pH264Ctx);
            ClearDPB(pH264Ctx);
            pH264Ctx->CurrPic.frame_store = NULL;
            return H264_ERR;
        }

        pH264Ctx->AllowStartDec = 1;
    }
    else  // (1 == pH264Ctx->StartMode)
    {
        // ͳ�Ƶ�ǰPic������������ж��Ƿ���Ҫ��g_CurrPic.err_level?
        pH264Ctx->CurrPic.err_level = ErrRatio;
        ClearAllSlice(pH264Ctx);

        ret = StorePicInDPB(pH264Ctx, pLumaInfo);
        if (H264_OK != ret)
        {
            dprint(PRN_ERROR, "imode nal store pic err, ret = %d\n", ret);
        }
        ret = FlushDPB(pH264Ctx);
        if (H264_OK != ret)
        {
            dprint(PRN_ERROR, "imode nal flush dpb err, ret = %d\n", ret);
        }

        pH264Ctx->AllowStartDec = 0;
        InitOldSlice(pH264Ctx);
    }

    return H264_OK;
}


SINT32 H264DEC_GetImageBuffer( H264_CTX_S *pH264Ctx )
{
    UINT32 i;
    SINT32 ReadImgNum, NewImgNum, ret;
#ifdef VFMW_CFG_CAP_HD_SIMPLE
    SINT32  LogicFsID = 0;
    FSP_LOGIC_FS_S *pstLFs = NULL;
#endif

    if (pH264Ctx->TotalFsNum < 1)
    {
        return 1;
    }
#ifdef VFMW_CFG_CAP_HD_SIMPLE
    if(1 == pH264Ctx->PrevPic.FrameStoreFlag)
    {
        LogicFsID = pH264Ctx->PrevPic.image_id;
        pstLFs = FSP_GetLogicFs(pH264Ctx->ChanID, LogicFsID);
        if ((NULL != pstLFs)&&(FS_DISP_STATE_ALLOCED == pstLFs->s32DispState))
        {
            return 1;
        }
    }
#endif
    // allocate frame_store
    for (i=0; i<pH264Ctx->TotalFsNum; i++)
    {
        if ((0 == pH264Ctx->FrameStore[i].is_used) && (1 == IsOutDPB(pH264Ctx, &pH264Ctx->FrameStore[i])))
        {
            /* ֡��ṹ��ȡ���ˣ���Ҫ��ѯFSP�Ƿ������֡����� */
			ret = FSP_IsNewFsAvalible(pH264Ctx->ChanID);
            if (ret == 1)
            {
                return 1;
            }
            else if (ret == -1)
            {
                FSP_ClearNotInVoQueue(pH264Ctx->ChanID, &(pH264Ctx->ImageQue));
                return 0;
            }
			else
			{
			    return 0;
			}
        }
    }

    /* ���֡���Ƿ�й© */
    GetQueueImgNum(&pH264Ctx->ImageQue, &ReadImgNum, &NewImgNum);
    dprint(PRN_FATAL, "cann't find FrameStore\n");
    dprint(PRN_FATAL, "========== H264 FrameStore state(is_used, is_in_dpb, IsOutDPB) ========\n");
    for (i=0; i<pH264Ctx->TotalFsNum; i++)
    {
        dprint(PRN_FATAL, "%02d: %d %d %d\n",i, pH264Ctx->FrameStore[i].is_used, pH264Ctx->FrameStore[i].is_in_dpb,
                          IsOutDPB(pH264Ctx, &pH264Ctx->FrameStore[i]));
    }
    dprint(PRN_FATAL, "ReadImgNum = %d, NewImgNum = %d\n", ReadImgNum, NewImgNum);
    if ((ReadImgNum+NewImgNum) < (SINT32)(pH264Ctx->TotalFsNum-pH264Ctx->DPB.used_size-2))
    {
        dprint(PRN_FATAL, "FrameStore leak, ClearAll\n");
        ClearAll(pH264Ctx,1);
    }

    return 0;
}


SINT32 H264DEC_GetImageWidth(H264_CTX_S *pH264Ctx)
{
    UINT32 uImageWidth = 0;

    uImageWidth = ((UINT32)pH264Ctx->CurrSPS.pic_width_in_mbs_minus1 + 1) * 16;

    return uImageWidth;

}

SINT32 H264DEC_GetImageHeight(H264_CTX_S *pH264Ctx)
{
    UINT32 uImageHeight = 0;

    uImageHeight = ((UINT32)pH264Ctx->CurrSPS.pic_height_in_map_units_minus1 + 1) * (2 - pH264Ctx->CurrSPS.frame_mbs_only_flag) * 16;

    return uImageHeight;
}


#endif // #ifdef H264_ENABLE
