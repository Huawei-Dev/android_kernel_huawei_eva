/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7omB5iYtHEGPqMDW7TpB3q4yvDckVri1GoBL4gKI3k0M86wMC5hMD5fQLRyjt
hU2r30H9Zx68gPViSqQR5ryaEejaADpDfkCjI7Cc+0K7xo/9ry0ePC9y3miX5D2HLDRMtPeb
goLZ0oCP5/d94dZXfOLCTgiLr+pTBL7F/qNLUsT2wx5g0gHdRb6gduvwL19jQ==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/***********************************************************************
*
* Copyright (c) 2007 HUAWEI - All Rights Reserved
*
* File     : $mvc.c$
* Date     : $2007/11/05$
* Revision : $v1.1$
* Purpose  : Mvc decoder file.
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
#include    "mvc.h"
#include    "public.h"
#include    "bitstream.h"
#include	"vdm_hal.h"

#include    "postprocess.h"
#include    "vfmw_ctrl.h"
#include    "fsp.h"


/* MVCģ����뿪�أ����û�д��򲻱��� */
#ifdef MVC_ENABLE


UINT8 MVC_g_NalTypeEOPIC[] = {0x00, 0x00, 0x01, 0x1E, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45, 0x4E, 0x44};

UINT32 MVC_g_AspecRatioIdc[14][2] = { { 1,  1}, { 1,  1}, {12, 11}, {10, 11}, {16, 11}, {40, 33}, { 24, 11}, 
    {20, 11}, {32, 11}, {80, 33}, {18, 11}, {15, 11}, {64, 33}, {160, 99}
};


UINT8 MVC_CalcZeroNum[256] = { 8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,
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
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const UINT8 MVC_g_ZZ_SCAN[16] =
{  
    0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

const UINT8 MVC_g_ZZ_SCAN8[64] =
{
     0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

UINT32 MVC_quant_intra_default[4] = { 0x1c140d06, 0x201c140d, 0x25201c14, 0x2a25201c };

UINT32 MVC_quant_inter_default[4] = { 0x18140e0a,  0x1b18140e, 0x1e1b1814, 0x221e1b18 };

UINT32 MVC_quant8_intra_default[16] = { 0x100d0a06, 0x1b191712, 0x12100b0a, 0x1d1b1917,
0x1712100d, 0x1f1d1b19, 0x19171210, 0x211f1d1b,
0x1b191712, 0x24211f1d, 0x1d1b1917, 0x2624211f,
0x1f1d1b19, 0x28262421, 0x211f1d1b, 0x2a282624
};

UINT32 MVC_quant8_inter_default[64] = { 0x110f0d09, 0x18161513, 0x13110d0d, 0x19181615,
0x1513110f, 0x1b191816, 0x16151311, 0x1c1b1918,
0x18161513, 0x1e1c1b19, 0x19181615, 0x201e1c1b,
0x1b191816, 0x21201e1c, 0x1c1b1918, 0x2321201e
};

UINT32 MVC_quant_org[4] = { 0x10101010, 0x10101010, 0x10101010, 0x10101010 };

UINT32 MVC_quant8_org[16] = { 0x10101010, 0x10101010, 0x10101010, 0x10101010,
0x10101010, 0x10101010, 0x10101010, 0x10101010,
0x10101010, 0x10101010, 0x10101010, 0x10101010,
0x10101010, 0x10101010, 0x10101010, 0x10101010
};



/*!
************************************************************************
*   ����ԭ�� :  SINT32 mvc_ue_v(MVC_CTX_S *pMvcCtx)
*   �������� :  �޷��ű䳤�����,ָ��ӳ�亯���͵��ú�����ȡ�﷨Ԫ�ص�������Ϣ
�ͽ����﷨Ԫ��ֵ
*   ����˵�� :  ������Ϣ���﷨Ԫ��SE_TRACEstring��Ϣ
*   ����ֵ   �� ���﷨Ԫ��ֵ 
************************************************************************
*/
UINT32 mvc_ue_v(MVC_CTX_S *pMvcCtx,UINT8 *SE_TRACEstring)
{
    UINT32 NumBits, tmpbits;
    UINT32 INFO;    
    UINT32 zeros;
    
    // ======== mb_type ==========
    tmpbits = BsShow(&(pMvcCtx->BS), 32);
    zeros = ZerosMS_32(tmpbits);
    if (zeros < 16)
    {
        mvc_ue_vld(tmpbits, NumBits, INFO, zeros);
        BsSkip(&(pMvcCtx->BS), NumBits);
    }
    else
    {
        if (zeros < 32)
        {
            // ��Ϊ������Ϊ2��31�η�������zeros+1��������Ϊ32
            BsSkip(&(pMvcCtx->BS), zeros);
            INFO = BsShow(&(pMvcCtx->BS), (zeros+1)) - 1;        
            BsSkip(&(pMvcCtx->BS), (zeros+1));
            NumBits = (zeros<<1) + 1; 
        }
        else
        {
            // ��ʱ��32���㣬˵���д�����
            pMvcCtx->SeErrFlg = 1;
            INFO = 0xffffffff;
            NumBits = 32;
        }
    }
    // NumBits = (zeros<<1) + 1;
    // INFO = ((tmpbits)>>(32-NumBits)) - 1;

    pMvcCtx->pCurrNal->nal_bitoffset += NumBits;    

    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);

    return INFO;    
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 mvc_se_v(MVC_CTX_S *pMvcCtx)
*   �������� :  �з��ű䳤�����,ָ��ӳ�亯���͵��ú�����ȡ�﷨Ԫ�ص�������Ϣ
�ͽ����﷨Ԫ��ֵ
*   ����˵�� :  ������Ϣ���﷨Ԫ��SE_TRACEstring��Ϣ
*   ����ֵ   �� ���﷨Ԫ��ֵ 
************************************************************************
*/
SINT32 mvc_se_v(MVC_CTX_S *pMvcCtx,UINT8 *SE_TRACEstring)
{ 
    UINT32 NumBits, tmpbits;
    SINT32 INFO;   
    UINT32 zeros;
    UINT32 INFOLastBit;

    // ======== mb_type ==========
    tmpbits = BsShow(&(pMvcCtx->BS), 32);
    zeros   = ZerosMS_32(tmpbits);

    if (zeros < 16)
    {
        mvc_se_vld(tmpbits, NumBits, INFO, zeros);
        BsSkip(&(pMvcCtx->BS), NumBits);
    }
    else
    {
        // ��Ϊ������Ϊ2��31�η�������zeros��������Ϊ32�������������������ҪBsShow����
        if (zeros < 32)
        {
            BsSkip(&(pMvcCtx->BS), zeros);
            INFO = BsGet(&(pMvcCtx->BS), zeros);
            INFOLastBit = BsGet(&(pMvcCtx->BS), 1);
            INFO = ((INFOLastBit&1) ? -1*INFO : INFO);
            NumBits = (zeros<<1) + 1;
        }
        else
        {
            // ��ʱ��32���㣬˵���д�����
            pMvcCtx->SeErrFlg = 1;
            INFO = 0x7fffffff;
            NumBits = 32;
        }
    }    
    
    pMvcCtx->pCurrNal->nal_bitoffset += NumBits;
    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);
    
    return INFO;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 mvc_u_v(MVC_CTX_S *pMvcCtx)
*   �������� :  �����﷨Ԫ�ؽ���
*   ����˵�� :  �﷨Ԫ�س��Ⱥ�SE_TRACEstring��������Ϣ
*   ����ֵ   �� �﷨Ԫ��ֵ
************************************************************************
*/
SINT32 mvc_u_v(MVC_CTX_S *pMvcCtx,UINT32 LenInBits, UINT8 *SE_TRACEstring)
{  
    SINT32 INFO = 0;
    
    INFO = BsGet(&(pMvcCtx->BS), LenInBits);
    pMvcCtx->pCurrNal->nal_bitoffset += LenInBits;
    
    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);
    
    return INFO;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 mvc_u_1(MVC_CTX_S *pMvcCtx)
*   �������� :  �����﷨Ԫ�ؽ��룬����1bit
*   ����˵�� :  �﷨Ԫ�س��Ⱥ�SE_TRACEstring��������Ϣ
*   ����ֵ   �� �﷨Ԫ��ֵ
************************************************************************
*/
UINT32 mvc_u_1(MVC_CTX_S *pMvcCtx,UINT8 *SE_TRACEstring)
{
    SINT32 INFO = 0;
    INFO = BsGet(&(pMvcCtx->BS), 1);
    pMvcCtx->pCurrNal->nal_bitoffset += 1;

    dprint(PRN_SE, "%-50s%50d\n", SE_TRACEstring, INFO);

    return INFO;
}


SINT32 MVC_ClearCurrNal(MVC_CTX_S *pMvcCtx)
{
    if (NULL != pMvcCtx->pCurrNal)
    {
        MVC_ReleaseNAL(pMvcCtx->ChanID, pMvcCtx->pCurrNal);
        pMvcCtx->pCurrNal = NULL;
    }

    return MVC_OK;
}


SINT32 MVC_ClearAllNal(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;

    for (i=0; i < MAX_SLICE_SLOT_NUM+1; i++)
    {
        if (1 == (pMvcCtx->NalArray[i].is_valid))
        {
            MVC_ReleaseNAL(pMvcCtx->ChanID, &(pMvcCtx->NalArray[i]));
        }
    }

    return MVC_OK;
}


SINT32 MVC_ClearCurrSlice(MVC_CTX_S *pMvcCtx)
{
    dprint(PRN_ERROR, "clear curr slice.\n");

    MVC_ClearCurrNal(pMvcCtx);
    pMvcCtx->TotalSlice++;

    return MVC_OK;
}


SINT32 MVC_ClearAllSlice(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;

    for (i = 0; i < MAX_SLICE_SLOT_NUM; i++)
    {
        if (NULL != pMvcCtx->SlcInfo[i].pSliceNal)
        {
            MVC_ReleaseNAL(pMvcCtx->ChanID, pMvcCtx->SlcInfo[i].pSliceNal);
            pMvcCtx->SlcInfo[i].pSliceNal = NULL;
        }
    }
    pMvcCtx->SliceParaNum = 0;
    pMvcCtx->MaxBytesReceived = 0;

    return MVC_OK;
}


SINT32 MVC_ClearCurrPic(MVC_CTX_S *pMvcCtx)
{

    MVC_ClearAllSlice(pMvcCtx);
    MVC_InitOldSlice(pMvcCtx);

    return MVC_OK;
}


SINT32 MVC_ClearDPB(MVC_CTX_S *pMvcCtx, SINT32 CurrViewId)
{
    
    if(MVC_OK != MVC_FlushDPB(pMvcCtx,CurrViewId))
    {
    	dprint(PRN_DBG, "%s %d MVC_OK != MVC_FlushDPB!!\n",__FUNCTION__,__LINE__);
    }
    
    if (MVC_OK != MVC_InitDPB(pMvcCtx))
    {
    	dprint(PRN_DBG, "%s %d MVC_OK != MVC_InitDPB!!\n",__FUNCTION__,__LINE__);
    }

    pMvcCtx->IDRFound = 0;
    pMvcCtx->FirstFrameDisplayed = MVC_FIRST_DISP_INIT;

    return MVC_OK;
}


SINT32 MVC_ClearAll(MVC_CTX_S *pMvcCtx,SINT32 flag)
{
    UINT32 i;
    UINT32 usrdat_idx;
    SINT32 ret;

    dprint(PRN_DBG, "clear all dec para\n");

    MVC_ClearCurrPic(pMvcCtx);
    MVC_ClearAllNal(pMvcCtx);

    ret = MVC_InitDPB(pMvcCtx);
    if (MVC_OK != ret)
    {
    	dprint(PRN_DBG, "%s %d MVC_OK != MVC_InitDPB!!\n",__FUNCTION__,__LINE__);
    }

    if(flag)
    {
        FSP_ClearNotInVoQueue(pMvcCtx->ChanID,&pMvcCtx->ImageQue);
    }
    else
    {
        ResetVoQueue(&pMvcCtx->ImageQue);
        FSP_EmptyInstance(pMvcCtx->ChanID);
    }
    for (i=0; i<MVC_MAX_FRAME_STORE; i++)
    {
        pMvcCtx->FrameStore[i].non_existing = 0;                 //a real pic;
        pMvcCtx->FrameStore[i].is_used = 0;                      //MVC_EMPTY;
        pMvcCtx->FrameStore[i].is_reference = 0;                 //not pic used for ref;
        pMvcCtx->FrameStore[i].long_term_frame_idx = 0;
        pMvcCtx->FrameStore[i].frame_num = 0;
        pMvcCtx->FrameStore[i].frame_num_wrap = -1;
        pMvcCtx->FrameStore[i].poc = 0;
        pMvcCtx->FrameStore[i].is_in_dpb = 0;                    //already out from g_DPB
        pMvcCtx->FrameStore[i].is_displayed = 1;                 //already out from VO
        pMvcCtx->FrameStore[i].pmv_address_idc = MVC_MAX_PMV_STORE;
        pMvcCtx->FrameStore[i].apc_idc = 16;
        pMvcCtx->FrameStore[i].fs_image.image_id = i;
        pMvcCtx->FrameStore[i].frame.frame_store = pMvcCtx->FrameStore[i].top_field.frame_store = pMvcCtx->FrameStore[i].bottom_field.frame_store = & pMvcCtx->FrameStore[i];
        pMvcCtx->FrameStore[i].frame.structure = 0;              //g_frame is MVC_FRAME, will be change when field paired
        pMvcCtx->FrameStore[i].top_field.structure = 1;          //g_top is top field;
        pMvcCtx->FrameStore[i].bottom_field.structure = 2;       //g_bottom is bottom field
        for (usrdat_idx=0; usrdat_idx <4; usrdat_idx++)
        {
            if (NULL != pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx])
            {
                FreeUsdByDec(pMvcCtx->ChanID, pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx]);
                pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx] = NULL;
            }
        }
    }

    // release usrdat of currpic
    for (i=0; i<4; i++)
    {
        if (NULL != pMvcCtx->CurrPic.fs_image.p_usrdat[i] )
        {
            FreeUsdByDec(pMvcCtx->ChanID, pMvcCtx->CurrPic.fs_image.p_usrdat[i]);
            pMvcCtx->CurrPic.fs_image.p_usrdat[i] = NULL;
        }
    }

    for (i = 0; i <pMvcCtx->TotalUsrDatNum; i++ )
    { 
        if (NULL !=pMvcCtx->pUsrDatArray[i])
        {
            FreeUsdByDec(pMvcCtx->ChanID, pMvcCtx->pUsrDatArray[i]);
            pMvcCtx->pUsrDatArray[i] = NULL;
        }
    }
    pMvcCtx->TotalUsrDatNum = 0;

    // refresh some statistic variable
    pMvcCtx->AllowStartDec = 0;
    pMvcCtx->SliceParaNum = 0;
    pMvcCtx->MaxBytesReceived = 0;
    pMvcCtx->IModeOutFlag = 0;
    pMvcCtx->IDRFound = 0;
    pMvcCtx->FirstFrameDisplayed = MVC_FIRST_DISP_INIT;
    pMvcCtx->PPicFound = 0;

    // init some global var
    pMvcCtx->pCurrNal = NULL;
    pMvcCtx->CurrMarkIdx = 0;
    pMvcCtx->Mark[0].is_valid = pMvcCtx->Mark[1].is_valid = 0;
    pMvcCtx->Mark[0].no_output_of_prior_pics_flag = pMvcCtx->Mark[1].no_output_of_prior_pics_flag = 0;

    // init pMvcCtx->CurrSlice 
    memset(&pMvcCtx->CurrSlice, 0, sizeof(MVC_SLICE_S));
    pMvcCtx->CurrSlice.slice_type = MVC_I_SLICE;
    pMvcCtx->CurrSlice.new_pic_flag = 1;
    pMvcCtx->CurrPic.state = MVC_EMPTY;

    pMvcCtx->PrevPic.structure = MVC_FRAME;
    pMvcCtx->PrevPic.frame_num = 256;
    pMvcCtx->PrevPic.nal_ref_idc = 0;

    return MVC_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 MVC_GetMinPOC(MVC_CTX_S *pMvcCtx,SINT32 *poc, SINT32 *pos)
*   �������� :  �ҳ���Сpoc ����Сpocͼ��λ��
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_GetMinPOC(MVC_CTX_S *pMvcCtx, SINT32 CurrViewId, SINT32 *poc, UINT32 *pos)
{
    UINT32 i;
    SINT32 pre_id = -1;
    FSP_LOGIC_FS_S *pLogicfs;

// *pos = 0;
    *pos = -1;
    *poc = 0x7fffffff;

    if (pMvcCtx->DPB.used_size > pMvcCtx->DPB.size)
    {
        pMvcCtx->DPB.used_size = pMvcCtx->DPB.size; // for error resilence
    }
//    for (i=0; i<pMvcCtx->DPB.used_size; i++)
    for (i=0; i<pMvcCtx->DPB.size; i++)		
    {
        if (pMvcCtx->DPB.fs[i] == NULL)
		{
		    continue;
		}
        pLogicfs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs[i]->logic_fs_id);
		if (pLogicfs == NULL)
		{
		    continue;
		}
        if (-1 == CurrViewId)
        {
            if ((pMvcCtx->DPB.fs[i]->is_in_dpb) && (FS_DISP_STATE_DEC_COMPLETE==pLogicfs->s32DispState || FS_DISP_STATE_ALLOCED==pLogicfs->s32DispState))
            {
                if (*poc > pMvcCtx->DPB.fs[i]->poc)
                { 
                    *poc = pMvcCtx->DPB.fs[i]->poc;
                    *pos = i;
                    pre_id = pMvcCtx->DPB.fs[i]->view_id;
                }
                else if (*poc == pMvcCtx->DPB.fs[i]->poc && pMvcCtx->DPB.fs[i]->view_id < pre_id)
                {
                    *poc = pMvcCtx->DPB.fs[i]->poc;
                    *pos = i; 
                    pre_id = pMvcCtx->DPB.fs[i]->view_id;
                }
            }
        }
        else
        {
        if ((*poc>pMvcCtx->DPB.fs[i]->poc) && (pMvcCtx->DPB.fs[i]->is_in_dpb) && (FS_DISP_STATE_DEC_COMPLETE==pLogicfs->s32DispState || FS_DISP_STATE_ALLOCED==pLogicfs->s32DispState ) &&//?az
            (pMvcCtx->DPB.fs[i]->view_id == CurrViewId || -1 == CurrViewId))
        {
            *poc = pMvcCtx->DPB.fs[i]->poc;
            *pos = i;
            }
        }
    }

    return *pos;
}


/*
************************************************************************
*   ����ԭ�� :  UINT32 MVC_FrameStoreRefFlg(MVC_FRAMESTORE_S *pFS)
*   �������� :  ָʾ��framestore�Ƿ����ڲο�
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
UINT32 MVC_FrameStoreRefFlg(const MVC_FRAMESTORE_S *pFS)
{
    return (pFS->is_reference > 0);
}


SINT32 MVC_NonLongTermRefFlg(const MVC_FRAMESTORE_S *pFS)
{
    if (3 == pFS->is_reference)
    {
        if (0 == pFS->frame.is_long_term)
        {
            return 1;
        }
    }
    
    if (pFS->is_reference & 1)
    {
        if (0 == pFS->top_field.is_long_term)
        {
            return 1;
        }
    }
    
    if (pFS->is_reference & 2)
    {
        if (0 == pFS->bottom_field.is_long_term)
        {
            return 1;
        }
    }
    
    return 0;      
}


/*!
************************************************************************
*   ����ԭ�� :  UINT32 MVC_ShortTermRefFlg(MVC_FRAMESTORE_S *pFS)
*   �������� :  �жϸ�֡���Ƿ�Ϊ���ڲο�
*   ����˵�� :  ֡��ָ��
*   ����ֵ   �� �жϽ��
************************************************************************
*/
UINT32 MVC_ShortTermRefFlg(const MVC_FRAMESTORE_S *pFS)
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
*   ����ԭ�� :  UINT32 MVC_LongTermRefFlg(MVC_FRAMESTORE_S *pFS)
*   �������� :  �жϸ�֡���Ƿ�Ϊ���ڲο�
*   ����˵�� :  ֡��ָ��
*   ����ֵ   �� �жϽ�� 
************************************************************************
*/
UINT32 MVC_LongTermRefFlg(const MVC_FRAMESTORE_S *pFS)
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
*      ����ԭ�� :  VOID MVC_UpdateLTReflist(MVC_CTX_S *pMvcCtx)
*      �������� :  ����DPB�г��ڲο��б�
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UpdateLTReflist(MVC_CTX_S *pMvcCtx)
{
    UINT32 i, j;
    
//    for (i=0, j=0; i<pMvcCtx->DPB.used_size; i++)
    for (i=0, j=0; i<pMvcCtx->DPB.size; i++)				
    {
        if (pMvcCtx->DPB.fs[i]!=NULL)
        {
            if (MVC_LongTermRefFlg(pMvcCtx->DPB.fs[i]) && pMvcCtx->DPB.fs[i]->view_id == pMvcCtx->CurrPic.view_id)
            {
                pMvcCtx->DPB.fs_ltref[j++] = pMvcCtx->DPB.fs[i];
            }
        }
    }
    pMvcCtx->DPB.ltref_frames_in_buffer = j;  
    while (j<pMvcCtx->DPB.size)
    {
        pMvcCtx->DPB.fs_ltref[j++] = (MVC_FRAMESTORE_S *) NULL;
    }
    
    return;  
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_UpdateReflist(MVC_CTX_S *pMvcCtx)
*      �������� :  ����DPB�ж��ڲο��б�
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UpdateReflist(MVC_CTX_S *pMvcCtx)
{
    UINT32 i, j;

    //for (i=0, j=0; i<pMvcCtx->DPB.used_size; i++)
    for (i=0, j=0; i<pMvcCtx->DPB.size; i++)    
    {
        if (pMvcCtx->DPB.fs[i] != NULL)
        {
            if (MVC_ShortTermRefFlg(pMvcCtx->DPB.fs[i]) && pMvcCtx->DPB.fs[i]->view_id == pMvcCtx->CurrPic.view_id)
            {
                pMvcCtx->DPB.fs_ref[j++] = pMvcCtx->DPB.fs[i];
            }
        }
    }    
    pMvcCtx->DPB.ref_frames_in_buffer = j;    
    while (j<pMvcCtx->DPB.size)
    {
        pMvcCtx->DPB.fs_ref[j++] = (MVC_FRAMESTORE_S *)NULL;
    }
    
    return;    
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_GetPicNumX(MVC_CURRPIC_S *pCurrPic, UINT32 difference_of_pic_nums_minus1)
*      �������� :  ����difference_of_pic_nums_minus1����pic_num,
*      ����˵�� :  difference_of_pic_nums_minus1�����ڼ���pic_num��pCurrPic��ǰͼ��ṹ��ָ��
*      ����ֵ   �� ��
************************************************************************
*/
SINT32 MVC_GetPicNumX(const MVC_CURRPIC_S *pCurrPic, UINT32 difference_of_pic_nums_minus1)
{
    UINT32 CurrPicNum;
    SINT32 result;
    
    if (MVC_FRAME == pCurrPic->structure)
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
*      ����ԭ�� :  VOID MVC_UnMarkFrameStoreRef(MVC_CTX_S *pMvcCtx,MVC_FRAMESTORE_S *pFrameStore)
*      �������� :  ��pFrameStore����֡��������ͼ����Ϊ���ο���ͬʱ�ͷ�pmv��λ
*      ����˵�� :  pFrameStore֡��ָ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UnMarkFrameStoreRef(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore)
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

    if (3 == pFrameStore->is_used) //MVC_FRAME, both field pair
    {
        pFrameStore->frame.is_short_term = 0;
        pFrameStore->frame.is_long_term = 0;
        pFrameStore->is_reference = 0;
    }

    //�������ϲ��費һ���Ͱ�is_reference����0���������ǿ��һ��
    pFrameStore->is_reference = 0;
 
    /* ���߼�֡����Ϊ���ο� */
    FSP_SetRef(pMvcCtx->ChanID, pFrameStore->logic_fs_id, 0);

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_UnMarkLTFrmByFrmIdx(UINT32 long_term_frame_idx)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UnMarkLTFrmByFrmIdx(MVC_CTX_S *pMvcCtx,UINT32 long_term_frame_idx)
{
    UINT32 i;
    
    for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
    {
        if (long_term_frame_idx == pMvcCtx->DPB.fs_ltref[i]->long_term_frame_idx && 
            pMvcCtx->DPB.fs_ltref[i]->view_id == pMvcCtx->CurrPic.view_id)
        {
            MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs_ltref[i]);
            return;
        }
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_UnMarkLTFldByFrmIdx(MVC_CTX_S *pMvcCtx,MVC_FRAMESTORE_S *pFS, UINT32 structure, long_term_frame_idx)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UnMarkLTFldByFrmIdx(MVC_CTX_S *pMvcCtx,const MVC_FRAMESTORE_S *pFS, UINT32 structure, UINT32 long_term_frame_idx)
{
    UINT32 i;
    
    for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
    {  
        if (long_term_frame_idx == pMvcCtx->DPB.fs_ltref[i]->long_term_frame_idx &&
            pMvcCtx->DPB.fs_ltref[i]->view_id == pMvcCtx->CurrPic.view_id)
        {
            if (MVC_TOP_FIELD == structure)
            {
                if ((0 == pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_long_term) || (pFS != pMvcCtx->DPB.fs_ltref[i])) // not long term field pair 
                {
                    MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs_ltref[i]);
                    break;  //!!!
                }
            }
            if (MVC_BOTTOM_FIELD == structure)
            {
                if ((0 == pMvcCtx->DPB.fs_ltref[i]->top_field.is_long_term) || (pFS != pMvcCtx->DPB.fs_ltref[i])) // not long term field pair
                {
                    MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs_ltref[i]);
                    break;  //!!!
                }
            }
        }
    }
    
    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_UnMarkSTRef(MVC_CTX_S *pMvcCtx,MVC_CURRPIC_S  *pCurrPic, UINT32 difference_of_pic_nums_minus1)
*      �������� :  ����difference_of_pic_nums_minus1����pic_num,
*                  ��pic_num��Ӧ�Ķ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  difference_of_pic_nums_minus1�����ڼ���pic_num��pCurrPic��ǰͼ�����ݽṹ��ָ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID  MVC_UnMarkSTRef(MVC_CTX_S *pMvcCtx,const MVC_CURRPIC_S  *pCurrPic, SINT32 difference_of_pic_nums_minus1)
{
    UINT32 i;
    SINT32 picNumX;

    /* Get picNumX */
    picNumX = MVC_GetPicNumX(pCurrPic, difference_of_pic_nums_minus1);
    
    if (MVC_FRAME == pCurrPic->structure)
    {
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            if (pMvcCtx->DPB.fs_ref[i]->frame.pic_num == picNumX)
            {
                if (3==pMvcCtx->DPB.fs_ref[i]->is_reference)
                {
                    if (0==pMvcCtx->DPB.fs_ref[i]->frame.is_long_term && 1==pMvcCtx->DPB.fs_ref[i]->frame.is_short_term &&
                        pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id)
                    {
                        MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs_ref[i]);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            if ((pMvcCtx->DPB.fs_ref[i]->is_reference & 1) && 
                0==pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term && 1==pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term &&
				pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id)
            {
                if (pMvcCtx->DPB.fs_ref[i]->top_field.pic_num == picNumX)                     //include top field
                {
                    //mark top field unused
                    pMvcCtx->DPB.fs_ref[i]->is_reference &= 2;
                    pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term =0;

                    //one field reference attribute modified and MVC_FRAME reference attribute need to modify
                    if (MVC_FIELD_PAIR == pMvcCtx->DPB.fs_ref[i]->frame.structure || MVC_FRAME == pMvcCtx->DPB.fs_ref[i]->frame.structure)
                    {
                        pMvcCtx->DPB.fs_ref[i]->frame.is_short_term = 0;
                        pMvcCtx->DPB.fs_ref[i]->frame.is_long_term = 0;
                    }

                    if (0 == pMvcCtx->DPB.fs_ref[i]->is_reference)
                    {
                        pMvcCtx->PmvStoreUsedFlag[pMvcCtx->DPB.fs_ref[i]->pmv_address_idc] = 0;          //pmv store
                        FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->DPB.fs_ref[i]->logic_fs_id, 0);   //z56361
                    }

                    return;
                }
            }
 
            if ((pMvcCtx->DPB.fs_ref[i]->is_reference & 2) && 
                 0 == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term && 1 == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term &&
				 pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id)
            {
                 if (pMvcCtx->DPB.fs_ref[i]->bottom_field.pic_num == picNumX)
                 {
                    //mark bottom field unused
                    pMvcCtx->DPB.fs_ref[i]->is_reference &= 1;
                    pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term = 0;

                    //one field reference attribute modified and MVC_FRAME reference attribute need to modify
                    if (MVC_FIELD_PAIR == pMvcCtx->DPB.fs_ref[i]->frame.structure || MVC_FRAME == pMvcCtx->DPB.fs_ref[i]->frame.structure)
                    {
                        pMvcCtx->DPB.fs_ref[i]->frame.is_short_term = 0;
                        pMvcCtx->DPB.fs_ref[i]->frame.is_long_term = 0;
                    }

                    if (0 == pMvcCtx->DPB.fs_ref[i]->is_reference)
                    {
                        pMvcCtx->PmvStoreUsedFlag[pMvcCtx->DPB.fs_ref[i]->pmv_address_idc] = 0;          //pmv store 
                        FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->DPB.fs_ref[i]->logic_fs_id, 0);   //z56361
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
*      ����ԭ�� :  VOID MVC_UnMarkLTRef(MVC_CTX_S *pMvcCtx,MVC_CURRPIC_S  *pCurrPic, UINT32 long_term_pic_num)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UnMarkLTRef(MVC_CTX_S *pMvcCtx,const MVC_CURRPIC_S *pCurrPic, SINT32 long_term_pic_num)
{
    UINT32 i;

    if (MVC_FRAME == pCurrPic->structure)
    {
        for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
        {
            if (long_term_pic_num == pMvcCtx->DPB.fs_ltref[i]->frame.long_term_pic_num)
            {
                if (3 == pMvcCtx->DPB.fs_ltref[i]->is_reference)
                {
                    if (1==pMvcCtx->DPB.fs_ltref[i]->frame.is_long_term && 0==pMvcCtx->DPB.fs_ltref[i]->frame.is_short_term &&
                        pMvcCtx->DPB.fs_ltref[i]->view_id == pMvcCtx->CurrPic.view_id)
                    {
                        MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs_ltref[i]);
                        return;
                    }
                }
            }
        } 
    }
    else
    {
        for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
        {
            if ((pMvcCtx->DPB.fs_ltref[i]->is_reference & 1) && 
                1==pMvcCtx->DPB.fs_ltref[i]->top_field.is_long_term && 0==pMvcCtx->DPB.fs_ltref[i]->top_field.is_short_term && 
				pMvcCtx->DPB.fs_ltref[i]->view_id == pMvcCtx->CurrPic.view_id)
            {
                if (long_term_pic_num == pMvcCtx->DPB.fs_ltref[i]->top_field.long_term_pic_num)                     //include top field
                {
                    //mark top field unused
                    pMvcCtx->DPB.fs_ltref[i]->is_reference &= 2;
                    pMvcCtx->DPB.fs_ltref[i]->top_field.is_long_term = 0;
                    
                    //one field reference attribute modified and MVC_FRAME reference attribute need to modify
                    if (MVC_FIELD_PAIR == pMvcCtx->DPB.fs_ltref[i]->frame.structure || MVC_FRAME == pMvcCtx->DPB.fs_ltref[i]->frame.structure)
                    {
                        pMvcCtx->DPB.fs_ltref[i]->frame.is_short_term = 0;       
                        pMvcCtx->DPB.fs_ltref[i]->frame.is_long_term = 0;
                    }
                    if (0 == pMvcCtx->DPB.fs_ltref[i]->is_reference)
                    {
                        pMvcCtx->PmvStoreUsedFlag[pMvcCtx->DPB.fs_ltref[i]->pmv_address_idc] = 0;          //pmv store 
                        FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->DPB.fs_ltref[i]->logic_fs_id, 0);//?az
                    } 
                    return;
                }
            }
           
            if ((pMvcCtx->DPB.fs_ltref[i]->is_reference & 2) && 
                 1==pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_long_term && 0==pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_short_term &&
				 pMvcCtx->DPB.fs_ltref[i]->view_id == pMvcCtx->CurrPic.view_id)
            {
                 if (long_term_pic_num == pMvcCtx->DPB.fs_ltref[i]->bottom_field.long_term_pic_num)
                 {
                    //mark bottom field unused
                    pMvcCtx->DPB.fs_ltref[i]->is_reference &= 1;
                    pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_long_term = 0;
                    
                    //one field reference attribute modified and MVC_FRAME reference attribute need to modify
                    if (MVC_FIELD_PAIR == pMvcCtx->DPB.fs_ltref[i]->frame.structure || MVC_FRAME == pMvcCtx->DPB.fs_ltref[i]->frame.structure)
                    {
                        pMvcCtx->DPB.fs_ltref[i]->frame.is_short_term = 0;
                        pMvcCtx->DPB.fs_ltref[i]->frame.is_long_term = 0;
                    }
                    if (0 == pMvcCtx->DPB.fs_ltref[i]->is_reference)
                    {
                        pMvcCtx->PmvStoreUsedFlag[pMvcCtx->DPB.fs_ltref[i]->pmv_address_idc] = 0;          //pmv store
                        FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->DPB.fs_ltref[i]->logic_fs_id, 0);//?az
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
*      ����ԭ�� :  VOID MVC_MarkPicLTRef(UINT32 long_term_pic_num, SINT32 picNumX, MVC_CURRPIC_S *pCurrPic)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_MarkPicLTRef(MVC_CTX_S *pMvcCtx, UINT32 long_term_frame_idx, SINT32 picNumX, const MVC_CURRPIC_S *pCurrPic)
{
    UINT32 i;
    UINT32 add_top = (MVC_TOP_FIELD == pCurrPic->structure) ? 1 : 0;
    UINT32 add_bottom = (MVC_BOTTOM_FIELD == pCurrPic->structure) ? 1 : 0;

    if (MVC_FRAME == pCurrPic->structure)
    {
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            if ((3 ==pMvcCtx->DPB.fs_ref[i]->is_reference) && (0 == pMvcCtx->DPB.fs_ref[i]->frame.is_long_term && 1 == pMvcCtx->DPB.fs_ref[i]->frame.is_short_term) 
                && (pMvcCtx->DPB.fs_ref[i]->frame.pic_num == picNumX) && (pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id))
            {         
                pMvcCtx->DPB.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
                pMvcCtx->DPB.fs_ref[i]->frame.long_term_pic_num = (SINT32)long_term_frame_idx;
                // mark to long term
                pMvcCtx->DPB.fs_ref[i]->frame.is_long_term = 1; 
                pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term = 1;
                pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term = 1;
                // not short term anymore
                pMvcCtx->DPB.fs_ref[i]->frame.is_short_term = 0; 
                pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term = 0;
                pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term = 0;
                break;
            }
        }
    }
    else
    {
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++) 
        {
            if (pMvcCtx->DPB.fs_ref[i]->is_reference & 1)
            {
                if ((1==pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term && 0==pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term)
                    && (picNumX==pMvcCtx->DPB.fs_ref[i]->top_field.pic_num) && (pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id))
                {
                    // top field is short term and not long term and picNum equals picNumX
                    if ((1 == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term) && 
                        (pMvcCtx->DPB.fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
                    {
                        // check bottom field is long term but long_term_frame_idx is diff
                        continue;
                    }

                    pMvcCtx->DPB.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
                    pMvcCtx->DPB.fs_ref[i]->top_field.long_term_pic_num = (SINT32)(2 * long_term_frame_idx + add_top);
                    pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term = 1;  // mark long term
                    pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term = 0; // mark not short term

                    // modify MVC_FRAME reference attribute
                    if (MVC_FIELD_PAIR == pMvcCtx->DPB.fs_ref[i]->frame.structure || \
                        MVC_FRAME == pMvcCtx->DPB.fs_ref[i]->frame.structure)
                    {
                        if (pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term)
                        {
                            pMvcCtx->DPB.fs_ref[i]->frame.is_long_term = pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term;
                            pMvcCtx->DPB.fs_ref[i]->frame.is_short_term = pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term;
                        }
                    }

                    break;
                }
            }
            if (pMvcCtx->DPB.fs_ref[i]->is_reference & 2) // include bottom field
            {
                if ((0 == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term && 1 == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term) && 
                    (pMvcCtx->DPB.fs_ref[i]->bottom_field.pic_num == picNumX) && (pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id))
                {
                    if ((1 == pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term) && (pMvcCtx->DPB.fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
                    {
                        continue;
                    }

                    pMvcCtx->DPB.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
                    pMvcCtx->DPB.fs_ref[i]->bottom_field.long_term_pic_num = (SINT32)(2 * long_term_frame_idx + add_bottom);
                    pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term = 1;  // mark long term
                    pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term = 0; // mark not short term

                    // modify MVC_FRAME reference attribute
                    if (MVC_FIELD_PAIR == pMvcCtx->DPB.fs_ref[i]->frame.structure ||MVC_FRAME == pMvcCtx->DPB.fs_ref[i]->frame.structure)
                    {
                        if (pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term)
                        {
                            pMvcCtx->DPB.fs_ref[i]->frame.is_long_term = pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term;
                            pMvcCtx->DPB.fs_ref[i]->frame.is_short_term = pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term;
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
*      ����ԭ�� :  VOID MVC_UnMarkLTRef(MVC_CTX_S *pMvcCtx, MVC_CURRPIC_S *pCurrPic, UINT32 long_term_pic_num)
*      �������� :  ��long_term_pic_num��Ӧ�ĳ��ڲο�ͼ����Ϊ���ο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬long_term_pic_num��Ҫ��ǵ�ͼ���long_term_pic_num
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_MarkSTToLTRef(MVC_CTX_S *pMvcCtx, const MVC_CURRPIC_S *pCurrPic, UINT32 difference_of_pic_nums_minus1, UINT32 long_term_frame_idx)
{
    SINT32 picNumX;
    UINT32 i;

    /* Get picNumX */
    picNumX = MVC_GetPicNumX(pCurrPic, difference_of_pic_nums_minus1);    

    /*then mark long term pic to unused which have this long_term_frame_idx */
    if (MVC_FRAME == pCurrPic->structure)// MVC_FRAME , both field pair
    {
        MVC_UnMarkLTFrmByFrmIdx(pMvcCtx, long_term_frame_idx);
    }
    else
    {
        //just find structure which will mark to long term
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            if ((pMvcCtx->DPB.fs_ref[i]->is_reference & 1) && \
                1 == pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term && 0 == pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term) // include top field
            {
                if (pMvcCtx->DPB.fs_ref[i]->top_field.pic_num == picNumX && pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id)
                {
                    MVC_UnMarkLTFldByFrmIdx(pMvcCtx, pMvcCtx->DPB.fs_ref[i], MVC_TOP_FIELD, long_term_frame_idx);
                    break;
                }
            }
            if ((pMvcCtx->DPB.fs_ref[i]->is_reference & 2) &&
                1 == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term && 0 == pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term) // include bottom field
            {
                if (pMvcCtx->DPB.fs_ref[i]->bottom_field.pic_num == picNumX && pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id)
                {
                    MVC_UnMarkLTFldByFrmIdx(pMvcCtx, pMvcCtx->DPB.fs_ref[i], BOTTOM_FIELD, long_term_frame_idx);
                    break;
                }
            }
        } 
    }

    /*finally mark short pic to long term*/
    MVC_MarkPicLTRef(pMvcCtx, long_term_frame_idx, picNumX, pCurrPic);

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_UpdateMaxLTFrmIdx(MVC_CTX_S *pMvcCtx, UINT32 max_long_term_frame_idx_plus1)
*      �������� :  ����DPB������ڲο�����
*      ����˵�� :  ���º������ڲο�֡����max_long_term_frame_idx_plus1
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UpdateMaxLTFrmIdx(MVC_CTX_S *pMvcCtx, UINT32 max_long_term_frame_idx_plus1)
{
    UINT32 i;

    pMvcCtx->DPB.max_long_term_pic_idx = max_long_term_frame_idx_plus1;

    // check for invalid frames
    for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
    {
        // ���������ת��һ��Ҫ����Ȼ��max_long_term_frame_idx_plus1����0��ʱ��ͳ�����
        if (pMvcCtx->DPB.fs_ltref[i]->long_term_frame_idx >= pMvcCtx->DPB.max_long_term_pic_idx &&
            pMvcCtx->DPB.fs_ltref[i]->view_id == pMvcCtx->CurrPic.view_id)
        {
            MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs_ltref[i]);
        }
    }
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_UnMarkAllSTRef(MVC_CTX_S *pMvcCtx)
*      �������� :  �����ж��ڲο�֡���Ϊ���ο�
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_UnMarkAllSTRef(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;

    for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
    {
        if(pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id)
        {
            MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs_ref[i]);
        }
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_MarkCurrPicLT (MVC_CTX_S *pMvcCtx, MVC_CURRPIC_S *pCurrPic, UINT32 long_term_frame_idx)
*      �������� :  �ѵ�ǰ����ͼ����Ϊ���ڲο�
*      ����˵�� :  pCurrPic��ǰͼ�����ݽṹ��ָ�룬
*                  long_term_frame_idx�������ǰ����ͼ��ĳ��ڲο�֡����
*      ����ֵ   �� ��
************************************************************************
*/
VOID MVC_MarkCurrPicLT(MVC_CTX_S *pMvcCtx, MVC_CURRPIC_S *pCurrPic, UINT32 long_term_frame_idx)
{    
    // remove long term pictures with same long_term_frame_idx
    if (MVC_FRAME == pCurrPic->structure)//MVC_FRAME ,field pair
    {
        // mark pic with long_term_frame_idx
        MVC_UnMarkLTFrmByFrmIdx(pMvcCtx, long_term_frame_idx);   
    }
    else
    {
        MVC_UnMarkLTFldByFrmIdx(pMvcCtx, pCurrPic->frame_store, pCurrPic->structure, long_term_frame_idx);
    }

    // mark current pic
    pCurrPic->is_long_term = 1;
    pCurrPic->is_short_term = 0;
    pCurrPic->long_term_frame_idx = long_term_frame_idx;
    
    return;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID MVC_RemoveFrameStoreOutDPB(MVC_CTX_S *pMvcCtx, UINT32 pos)
*   �������� :  ��framestoreָ���Ƴ�dpb
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
VOID MVC_RemoveFrameStoreOutDPB(MVC_CTX_S *pMvcCtx, UINT32 pos)
{
    //UINT32 i;
    FSP_LOGIC_FS_S *pstLogic;

    //release APC
    /* �ݴ������õ�ָ��ʱ��������ʲôʱ��Ӧ���ж��Ƿ�Ϊ�� */
    if (NULL == pMvcCtx->DPB.fs[pos])
    {
        return;
    }

    pMvcCtx->APC.is_used[pMvcCtx->DPB.fs[pos]->apc_idc] = 0;
    pMvcCtx->APC.used_size = (0 < pMvcCtx->APC.used_size) ? (pMvcCtx->APC.used_size-1) : 0;

    pstLogic = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs[pos]->logic_fs_id);
    if (NULL != pstLogic)
    {
        if (FS_DISP_STATE_WAIT_DISP != pstLogic->s32DispState)
        {
            FSP_SetDisplay(pMvcCtx->ChanID, pMvcCtx->DPB.fs[pos]->logic_fs_id, 0);
        }

        /* ���߼�֡����Ϊ���ο� */
        FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->DPB.fs[pos]->logic_fs_id, 0);
    }

    pMvcCtx->DPB.fs[pos]->is_used = 0;
    pMvcCtx->DPB.fs[pos]->is_in_dpb = 0;
    pMvcCtx->DPB.fs[pos]->is_reference = 0;
#if 0
    for (i=pos; i<pMvcCtx->DPB.used_size-1; i++)
    {
        pMvcCtx->DPB.fs[i] = pMvcCtx->DPB.fs[i+1];
    }
//    pMvcCtx->DPB.fs[pMvcCtx->DPB.used_size-1] = (MVC_FRAMESTORE_S *)NULL;
#endif
    pMvcCtx->DPB.fs[pos] = (MVC_FRAMESTORE_S *)NULL;

    pMvcCtx->DPB.used_size--;    

    return;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 MVC_RemoveUnUsedFrameStore(MVC_CTX_S *pMvcCtx)
*   �������� :  ����Ѿ���g_DPB����ķǲο�֡������Ѿ�vo��ʾ������������MVC_FRAME store ������ֵ
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
UINT32 MVC_RemoveUnUsedFrameStore(MVC_CTX_S *pMvcCtx, SINT32 CurrViewId)
{
    UINT32 i, j, k;
    //FSP_LOGIC_FS_S  *pstLogicFs;

    // check for frames that were already output and no longer used for reference
    /* z56361, 20111107, pMvcCtx->DPB.used_size����ֱ����Ϊ����ѭ���Ŀ��Ʊ�����������ΪRemove���������ʹused_size
       ��1����������ѭ�����Ʊ�������ʹ��DPB�е�ĳЩ֡ʵ���ϲ�δ���������Ӷ����±��ô�DPBɾ����֡��Ȼ����DPB��. ������
       DPB��FSP����Ϣ�ǳ�ͻ�ģ����������ܶ�����. 
       ͬ�ݿ����������ɴ�����ԭ����FSP��DPB��ͻ���MVC_GetAPC()ʧ�ܣ����������DPB�����¶�֡. */
//    k = pMvcCtx->DPB.used_size;
    k = pMvcCtx->DPB.size;
    for (i=0,j=0; j<k; j++)
    {
        if (pMvcCtx->DPB.fs[j]==NULL)
        {
            //j++;
			continue;
        }
		#if 0
        if((pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs[j]->logic_fs_id)) != NULL)
        {
            /* ���fsp֡���Ѿ����У�Ҫȷ����֡������DPB��ɾ�� */
            if (pstLogicFs->s32IsRef == 0 && pstLogicFs->s32DispState == FS_DISP_STATE_NOT_USED)
            {
                pMvcCtx->DPB.fs[j]->logic_fs_id = -1;
                pMvcCtx->DPB.fs[j]->is_reference= 0;
                /* �����֡�ǵ�һ֡���ձ������������δ�ֵ������������ʱ��Ҫ�����޳���DPB.
                   ���������Ŀ�����õ�һ֡���л��ᾭ������������������ھ�ȷ����open gop��ͷ2��B֡ */
                pMvcCtx->DPB.fs[j]->is_in_dpb = (pMvcCtx->DPB.fs[j]->eFoState != MVC_FO_STATE_FAST_OUTPUT)? 0: 1;
            }
        }
        else
        {
            pMvcCtx->DPB.fs[j]->is_in_dpb = (pMvcCtx->DPB.fs[j]->eFoState != MVC_FO_STATE_FAST_OUTPUT)? 0: 1;
        }
        #endif
        if (!MVC_FrameStoreRefFlg(pMvcCtx->DPB.fs[j]) &&((!pMvcCtx->DPB.fs[j]->is_in_dpb))// || FS_DISP_STATE_NOT_USED==pstLogicFs->s32DispState)
            && (pMvcCtx->DPB.fs[j]->view_id == CurrViewId || -1 == CurrViewId)
            && (pMvcCtx->DPB.fs[j]->eFoState != MVC_FO_STATE_FAST_OUTPUT))
        {
            pMvcCtx->DPB.fs[j]->is_used = 0;
            MVC_RemoveFrameStoreOutDPB(pMvcCtx, j);
            //return 1;
        }
		#if 0
        else
        {
            j++;
        }
		#endif
    }
	
    return 0;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 Check_FrameStore(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore)
*   �������� :  ����ͼ���Ƿ��ܹ������VO����
*   ����˵�� :  
*   ����ֵ   �� 0:�������;-1�������
************************************************************************
*/
SINT32 MVC_CheckFrameStore(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore)
{
    FSP_LOGIC_FS_S *pstLogicFs;
    UINT32 pic_type;
    SINT32  already_output_flag = 0;

    pic_type = pFrameStore->pic_type;  

     if (0 == pMvcCtx->IDRFound)
    {
        /* z56361, 20111206,
           �ú����ǰ���ʾ����õģ�֮ǰ��ͨ������"��һ����B֡"�ж�open gop��ǰ����B֡�Ѿ�������ϣ����ڴ����ʱ�������⡣
        ����ȡ�������������֡����"��B֡"��һ��I��+B����ɵ�֡��Ӧ��ΪB֡������264Э���У�������֡�п��ܾ��ǿɶ��������֡����I֡.
        ���ԣ���֡�����ж�open gop��ǰ����B֡�Ƿ��꣬�߼��ϴ���ȱ��. 
           �޸ķ���: ��Ϊ"��ʾ���ڵ�һ������֮֡ǰ��֡������". �����һ֡��I��+B����ɣ�ֻҪ���ܽ⣬�����Ͼ͵�ͬ��I֡. */
        //if (pFrameStore->eFoState == MVC_FO_STATE_FAST_OUTPUT || pMvcCtx->OldDecMode != IPB_MODE ||
            //pMvcCtx->pstExtraData->s32DecOrderOutput != NOT_DEC_ORDER)
        if (pFrameStore->as_first_out == 1)            
        {
        	pMvcCtx->IDRFound = 1;
        }
        else
        {
        	return MVC_ERR;
        }
    }
    
    pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pFrameStore->logic_fs_id);
    if (pstLogicFs != NULL)
    {
        if ((pstLogicFs->s32DispState == FS_DISP_STATE_WAIT_DISP) || (pstLogicFs->s32DispState == FS_DISP_STATE_NOT_USED))
        {
            already_output_flag = 1;
            dprint(PRN_ERROR, "MVC_FRAME %d, state=%d, is already output\n", pFrameStore->logic_fs_id, pstLogicFs->s32DispState);
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
        if (pFrameStore->eFoState == MVC_FO_STATE_FAST_OUTPUT)
        {
            pFrameStore->eFoState = MVC_FO_STATE_NORMAL_ORDER_OUTPUT;
        }
        return MVC_ERR_FS_OUT_AGAIN;
    }
    
    // the control condition would added
    if ((0==pFrameStore->is_used) || (1==pFrameStore->non_existing))
    {
        dprint(PRN_ERROR, "not used MVC_FRAME: (%d,%d)\n",pFrameStore->is_used, pFrameStore->non_existing);
        return MVC_ERR;
    }

    if ((3 == pFrameStore->is_used) && (3 == pFrameStore->mode_skip_flag))
    {
        dprint(PRN_ERROR, "MVC_FRAME skip: is_used=%d,skip=%d\n", pFrameStore->is_used, pFrameStore->mode_skip_flag);
        return MVC_ERR;
    }

    if ((1 == pFrameStore->is_used) && (1 == (pFrameStore->mode_skip_flag & 1)))
    {
        dprint(PRN_ERROR, "topfield skip: is_used=%d,skip=%d\n", pFrameStore->is_used, pFrameStore->mode_skip_flag);   
        return MVC_ERR;
    }

    if ((2 == pFrameStore->is_used) && (2 == (pFrameStore->mode_skip_flag & 2)))
    {
        dprint(PRN_ERROR, "bottom field skip: is_used=%d,skip=%d\n", pFrameStore->is_used, pFrameStore->mode_skip_flag);
        return MVC_ERR;
    }

    if (pstLogicFs->stDispImg.error_level > pMvcCtx->pstExtraData->s32OutErrThr)
    {
        dprint(PRN_ERROR, "err(%d) > out_thr(%d)\n", pstLogicFs->stDispImg.error_level, pMvcCtx->pstExtraData->s32OutErrThr);
        
        return MVC_ERR;
    }

    return MVC_OK;
}


VOID MVC_ExchangePts(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore)
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
//    for (i = 0; i < pMvcCtx->DPB.used_size; i++)
    for (i = 0; i < pMvcCtx->DPB.size; i++)
    {
        if (pMvcCtx->DPB.fs[i] != NULL)
        {
            if (NULL != (pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs[i]->logic_fs_id)))
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
            
            if (pMvcCtx->DPB.fs[i]->fs_image.PTS < MinPts)
            {
                MinPtsPos = i;
                MinPts = pMvcCtx->DPB.fs[i]->fs_image.PTS;
            }
        }
    }

    /* �����СPTS֡���ǵ�ǰ֡����ǰ֡���以��PTS */
    if (MinPtsPos != -1)
    {
        if (pFrameStore->fs_image.PTS > MinPts)
        {
            dprint(PRN_PTS,"exg_pts: %lld <-> %lld\n", pFrameStore->fs_image.PTS, MinPts);
            pMvcCtx->DPB.fs[MinPtsPos]->fs_image.PTS = pFrameStore->fs_image.PTS;
            pFrameStore->fs_image.PTS = MinPts;
        }
    }
    return;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID MVC_GetImagePara(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore)
*   �������� :  ��ֵҪ�����framestore��fs_image�ṹ��
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
VOID MVC_GetImagePara(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore)
{    
    UINT32 pic_type = pFrameStore->pic_type;
    UINT32 source_format;
    UINT32 valid_field;
    UINT8  top_field_first = 2;
    IMAGE  *pstLogicFsImage;
    MVC_SPS_S *sps;
    MVC_PPS_S *pps;

    pps = &pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id];
    sps = &pMvcCtx->SPS[pps->seq_parameter_set_id];    

    //clean format
    //pFrameStore->fs_image.format &= MVC_CLEAN_VIDEO_FORMAT;
    pFrameStore->fs_image.format &= MVC_CLEAN_SOURCE_FORMAT;
    pFrameStore->fs_image.format &= CLEAN_FRAME_TYPE;

    pFrameStore->fs_image.frame_rate = pMvcCtx->frame_rate * 1024;
	
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
                //progresive MVC_FRAME 
                source_format = 0x2;
            }
            else
            {
                //mbaff MVC_FRAME 
                source_format = 0x3;
            }
        }
        else
        {
            //field pair
            source_format = 0x3;
            
            //according  to err_level, modify valid_field and output level
            if (((pFrameStore->top_field.err_level > pMvcCtx->out_error_thr) &&
                (pFrameStore->bottom_field.err_level > pMvcCtx->out_error_thr)) ||
                ((pFrameStore->top_field.err_level <= pMvcCtx->out_error_thr) &&
                (pFrameStore->bottom_field.err_level <= pMvcCtx->out_error_thr))
                )
            {
                //all field err_level smaller than out_error_thr or bigger than out_error_thr,  don't modify 
                pFrameStore->fs_image.error_level =  pFrameStore->err_level;
                valid_field = 3;
                pic_type = pFrameStore->pic_type;
            }
            else if ((pFrameStore->top_field.err_level > pMvcCtx->out_error_thr) &&
                (pFrameStore->bottom_field.err_level <= pMvcCtx->out_error_thr))  
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
        if (pMvcCtx->PicTimingSEI.pic_struct == 3)
        {
            top_field_first = 1;
        }
        else if (pMvcCtx->PicTimingSEI.pic_struct == 4)
        {
            top_field_first = 0;
        }

        pFrameStore->fs_image.format &= MVC_CLEAN_TOP_FILED_FIRST;
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
	
	if (pMvcCtx->CurrSlice.mvcinfo_flag == 1)
	{
        pFrameStore->fs_image.is_3D = 1;
	}
    else
	{
        pFrameStore->fs_image.is_3D = 0;
	}		
	
    /* �������������������ǿ��ӵ绰Ӧ�ã���ʱ��Դ��ʽӦ�������� */
    if (pMvcCtx->pstExtraData->s32DecOrderOutput > NOT_DEC_ORDER && 
        0 == pFrameStore->frame.structure && pFrameStore->is_used == 3)
    {
        pFrameStore->fs_image.format &= MVC_CLEAN_SOURCE_FORMAT;  // ����
    }

    /* MVC pts ��ȡ��ʱ���ã�Android�Ǳ߱���ͬ������ */
    //MVC_ExchangePts(pMvcCtx, pFrameStore);

    /* if it is a 3D MVC_FRAME, assign its packing type */
    if (pMvcCtx->FramePackSEI.frame_packing_dec_ok)
    {
        if (pMvcCtx->FramePackSEI.frame_packing_arrangement_type == 3)
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_SIDE_BY_SIDE;
        }
        else if (pMvcCtx->FramePackSEI.frame_packing_arrangement_type == 4)
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_TOP_BOTTOM;
        }
        else if (pMvcCtx->FramePackSEI.frame_packing_arrangement_type == 5)
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_TIME_INTERLACED;
        }
        else
        {
            pFrameStore->fs_image.eFramePackingType = FRAME_PACKING_TYPE_NONE;
            dprint(PRN_FATAL,"stream MVC_FRAME packing type is %d! what can we do?\n", pMvcCtx->FramePackSEI.frame_packing_arrangement_type);
        }
    }

    pstLogicFsImage = FSP_GetFsImagePtr(pMvcCtx->ChanID, pFrameStore->logic_fs_id);
    if (NULL != pstLogicFsImage)
    {
        memcpy(pstLogicFsImage, &pFrameStore->fs_image, sizeof(IMAGE));
    }
    return;
}


VOID MVC_SetFrmRepeatCount(MVC_CTX_S *pMvcCtx, IMAGE *p_image)
{
    if (!((pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed > -2048)&&(pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed <= 2048)))
    {
        if (pMvcCtx->NewPicCounter/2 >= (pMvcCtx->PicOutCounter/2 + 1 + pMvcCtx->DPB.size))
        {
            if ((pMvcCtx->NewPicCounter/2 - (pMvcCtx->PicOutCounter/2 + 1 + pMvcCtx->DPB.size)) >= 2)
            {            
                p_image->u32RepeatCnt = 2;
                pMvcCtx->PicOutCounter+=4;
            }			
			else
            {            
                p_image->u32RepeatCnt = 1;
                pMvcCtx->PicOutCounter+=2;
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
*   ����ԭ�� :  SINT32 MVC_OutputFrmToVO(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore, UINT32 is_last_frame)
*   �������� :  ���һ֡ͼ��
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_OutputFrmToVO(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore, UINT32 is_last_frame)
{
    SINT32 ret;
    IMAGE *pstLogicFsImage;

    if ((NULL == pMvcCtx) || (NULL == pFrameStore))
    {
        dprint(PRN_FATAL, "pH264Ctx is %p, pFrameStore is %p\n", pMvcCtx, pFrameStore);
        return MVC_OUTPUT_FAIL; 
    }

    /* ��������������ʾ��������������������˴����ñ�Ҫ�ı�־�������������ͼ��.
       ע��: I֡ģʽ�²��������·�֧����ΪI֡ģʽ��direct ouput��Ч���ϵ�ͬ�ڽ�����
             ���; ����direct output���������������ͼ����������ķ�֧���ܽ�.*/
	if (pMvcCtx->pstExtraData->s32DecOrderOutput > NOT_DEC_ORDER && (pMvcCtx->OldDecMode != 2))
    {
        if (NULL != pMvcCtx->pDirectOutBuf)
        {
            pMvcCtx->pDirectOutBuf->is_used = 0;
            pMvcCtx->pDirectOutBuf = (MVC_FRAMESTORE_S *)NULL;
        }
        return MVC_ALREADY_OUT;
    }

    /* pFrameStore�����Ѿ�����������������뼰ʱ��ֹ���֡�Ĳ���.
       �������֡���ߵ������ԭ���ǣ�ĳЩ֡��������������ο������Ի���DPB��п����ٴα����г������. */
    ret = MVC_CheckFrameStore(pMvcCtx, pFrameStore);
    if (MVC_ERR_FS_OUT_AGAIN == ret)
    {
        return MVC_ALREADY_OUT;
    }
    
    if ((MVC_FRAMESTORE_S *)NULL != pMvcCtx->pDirectOutBuf)
    {
        MVC_GetImagePara(pMvcCtx, pMvcCtx->pDirectOutBuf);
        ret = MVC_CheckFrameStore(pMvcCtx, pMvcCtx->pDirectOutBuf);
        if (MVC_OK == ret)
        {
            FSP_SetDisplay(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id, 1);
            pstLogicFsImage = FSP_GetFsImagePtr(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id);
            if (NULL == pstLogicFsImage)
            {
                dprint(PRN_FATAL,"line: %d, pstLogicFsImage is NULL\n", __LINE__);
                return MVC_ERR;
            }
            
            if (pFrameStore == pMvcCtx->pDirectOutBuf && 1 == is_last_frame)
            {
                pstLogicFsImage->last_frame = 1;
            }

            ret = InsertImgToVoQueue(pMvcCtx->ChanID, VFMW_MVC, pMvcCtx, &pMvcCtx->ImageQue, pstLogicFsImage);
            if (VF_OK != ret)
            {
                //!! ������г�ʱ����Ҫ�˳�
                dprint(PRN_FATAL, "InsertVO err:%d, MVC_ClearAll\n", ret);

                pMvcCtx->pDirectOutBuf->is_used = 0;
                FSP_SetDisplay(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id, 0);
                pMvcCtx->pDirectOutBuf = (MVC_FRAMESTORE_S *)NULL;
                MVC_ClearAll(pMvcCtx,1);
                return MVC_ERR;  //OUT_FLAG
            }
            MVC_SetFrmRepeatCount(pMvcCtx, pstLogicFsImage);
            pMvcCtx->numOutputFrame++;
            pMvcCtx->PicOutCounter += 2;
        }
        else if (MVC_ERR_FS_OUT_AGAIN != ret)
        {
            dprint(PRN_ERROR, "mvc.c line %d: MVC_FRAME para err(ret=%d), recycle image self\n", __LINE__, ret);
            MVC_DEC_RecycleImage(pMvcCtx, pMvcCtx->pDirectOutBuf->fs_image.image_id);
        }

        pMvcCtx->pDirectOutBuf->is_used = 0;
        pMvcCtx->pDirectOutBuf = (MVC_FRAMESTORE_S *)NULL;
        if (pFrameStore == pMvcCtx->pDirectOutBuf && VF_OK == ret)
        {
            return MVC_OK;  
        }
    }

    MVC_GetImagePara(pMvcCtx, pFrameStore);
    ret = MVC_CheckFrameStore(pMvcCtx, pFrameStore);
    if (MVC_OK == ret)
    {
        FSP_SetDisplay(pMvcCtx->ChanID, pFrameStore->logic_fs_id, 1);
        pstLogicFsImage = FSP_GetFsImagePtr(pMvcCtx->ChanID, pFrameStore->logic_fs_id);
        if (NULL == pstLogicFsImage)
        {
            dprint(PRN_FATAL,"line: %d, pstLogicFsImage is NULL\n", __LINE__);
            return MVC_ERR;
        }

		    if (1 == is_last_frame)
		    {
	            pstLogicFsImage->last_frame = 1;
		    }
	
            ret = InsertImgToVoQueue(pMvcCtx->ChanID, VFMW_MVC, pMvcCtx, &pMvcCtx->ImageQue, pstLogicFsImage);
            if (VF_OK != ret)
            {
                //!! ������г�ʱ����Ҫ�˳�
                dprint(PRN_FATAL, "InsertVO err:%d, MVC_ClearAll\n", ret);
                FSP_SetDisplay(pMvcCtx->ChanID, pFrameStore->logic_fs_id, 0);
                MVC_ClearAll(pMvcCtx,1);
                return MVC_ERR;  //OUT_FLAG
            }

        MVC_SetFrmRepeatCount(pMvcCtx, pstLogicFsImage);
        pMvcCtx->numOutputFrame++;
        pMvcCtx->PicOutCounter += 2;
    }
    else
    {
        dprint(PRN_ERROR, "mvc.c line %d: MVC_FRAME para err(ret=%d), recycle image self\n", __LINE__, ret);
        MVC_DEC_RecycleImage(pMvcCtx, pFrameStore->fs_image.image_id);
        return MVC_OUTPUT_FAIL;  //OUT_FLAG
    }

    return MVC_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 MVC_OutputFrmFromDPB(MVC_CTX_S *pMvcCtx,, UINT32 pos)
*   �������� :  ��g_DPB�����һ֡��ͬʱ��һ֡��DPB��ɾ��
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_OutputFrmFromDPB(MVC_CTX_S *pMvcCtx, UINT32 pos)
{
    SINT32 ret;
    if (pMvcCtx->DPB.fs[pos] == NULL)
    {
        return MVC_OK;
    }
    pMvcCtx->DPB.fs[pos]->is_in_dpb = 0;  // here, output g_DPB to display
    ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->DPB.fs[pos], 0);
    if (MVC_ERR != ret)
    {
        ret = MVC_OK;
    }
    
    // free MVC_FRAME store and move MVC_EMPTY store to end of buffer
    if (!MVC_FrameStoreRefFlg(pMvcCtx->DPB.fs[pos]))
    {
        MVC_RemoveFrameStoreOutDPB(pMvcCtx, pos);
    }

    return ret;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_SplitFrmToFlds(MVC_FRAMESTORE_S *pFrameStore)
*      �������� :  ��һ֡�ֳ�����
*      ����˵�� :  pFrameStore:��������֡��
*      ����ֵ   �� ��
*
************************************************************************
*/
VOID MVC_SplitFrmToFlds(MVC_FRAMESTORE_S *pFrameStore)
{
    pFrameStore->top_field.is_long_term = pFrameStore->frame.is_long_term;
    pFrameStore->top_field.is_short_term = pFrameStore->frame.is_short_term;
    pFrameStore->top_field.structure = MVC_TOP_FIELD;
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
*      ����ԭ�� :  VOID MVC_CombineFldsToFrm(MVC_FRAMESTORE_S *pFrameStore)
*      �������� :  �ɶԵ���������֡��ƴ��һ֡
*      ����˵�� :  pFrameStore:��������֡��
*      ����ֵ   �� ��
*
************************************************************************
*/
VOID MVC_CombineFldsToFrm(MVC_FRAMESTORE_S *pFrameStore)
{
    pFrameStore->poc = pFrameStore->frame.poc = MIN(pFrameStore->top_field.poc, pFrameStore->bottom_field.poc);
    pFrameStore->frame.structure = MVC_FIELD_PAIR; // field pair more important
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
*   ����ԭ�� :  SINT32    MVC_GetAPC(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore)
*   �������� :  ����apc��û�ֵ�ʱ���ش���
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_GetAPC(MVC_CTX_S *pMvcCtx, MVC_FRAMESTORE_S *pFrameStore, UINT32 structure)
{
    UINT32 i;
    SINT32 toppoc, bottompoc;
    FSP_LOGIC_FS_S *pstLogicFs;

    CHECK_NULL_PTR_ReturnValue(pFrameStore, MVC_ERR);

    switch (structure)
    {
    case MVC_TOP_FIELD:
        toppoc = pFrameStore->top_field.poc;
        bottompoc = pFrameStore->top_field.poc;
        break;
    case BOTTOM_FIELD:
        toppoc = pFrameStore->bottom_field.poc;
        bottompoc = pFrameStore->bottom_field.poc;
        break;         
    case MVC_FRAME:
    default:
        toppoc = pFrameStore->top_field.poc;
        bottompoc = pFrameStore->bottom_field.poc;
    }

    for (i = 0; i < pMvcCtx->APC.size; i++)
    {
        if (0 == pMvcCtx->APC.is_used[i])
        {
            //apc is not used, allocate 
            pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pFrameStore->logic_fs_id);
            if (NULL != pstLogicFs && NULL != pstLogicFs->pstDecodeFs)
            {
                pMvcCtx->APC.is_used[i] = 1;
                pMvcCtx->APC.idc[i] = pstLogicFs->pstDecodeFs->PhyFsID;
                pMvcCtx->APC.poc[0][i] = toppoc;
                pMvcCtx->APC.poc[1][i] = bottompoc;
                pMvcCtx->APC.used_size = (16 <= pMvcCtx->APC.used_size) ? 16 : (pMvcCtx->APC.used_size+1);
                dprint(PRN_REF, "framestore %d allocate apc %d\n", pFrameStore->fs_image.image_id, i);

                pFrameStore->apc_idc = i;
                return MVC_OK;
            }
            else
            {
                dprint(PRN_FATAL,"find APC, but logic_fs_id %d abnormal(ref=%d,plfs=%p, pdfs=%p)\n", pFrameStore->logic_fs_id, 
                    pFrameStore->is_reference, pstLogicFs, (pstLogicFs)? pstLogicFs->pstDecodeFs: NULL);
                return MVC_ERR;
            }
        }
    }

    return MVC_ERR;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID MVC_InsertFrmInDPB(MVC_CTX_S pMvcCtx, UINT32 pos , MVC_CURRPIC_S *pCurrPic)
*   �������� :  ����һ��ͼ��g_DPB
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_InsertFrmInDPB(MVC_CTX_S *pMvcCtx, UINT32 pos, MVC_CURRPIC_S *pCurrPic)
{
    SINT32 ret;
    UINT64 OtherFieldPts;
    UINT64 OtherFieldUsertag;
    UINT64 OtherFieldDispTime;
	static SINT32 cnt = 0;

    // PTS ��ֵȱ
    if (pMvcCtx->DPB.used_size == 0)
    {
        cnt = 0;
    }
    switch (pCurrPic->structure)
    {
    case MVC_FRAME:
        pMvcCtx->DPB.fs[pos] = pCurrPic->frame_store;
        pMvcCtx->DPB.fs[pos]->non_existing = pCurrPic->non_existing;
        pMvcCtx->DPB.fs[pos]->is_used = 3;
        pMvcCtx->DPB.fs[pos]->is_in_dpb = pCurrPic->frame_store->is_in_dpb;
        pMvcCtx->DPB.fs[pos]->is_displayed = pCurrPic->frame_store->is_displayed;
        pMvcCtx->DPB.fs[pos]->is_reference = (pCurrPic->is_long_term ? 3 : (pCurrPic->is_short_term?3:0));
        pMvcCtx->DPB.fs[pos]->pmv_address_idc = pCurrPic->pmv_address_idc;
        pMvcCtx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;
        pMvcCtx->DPB.fs[pos]->err_level = pMvcCtx->DPB.fs[pos]->frame.err_level = pCurrPic->err_level;
        pMvcCtx->DPB.fs[pos]->frame.frame_store = pMvcCtx->DPB.fs[pos]->top_field.frame_store \
            = pMvcCtx->DPB.fs[pos]->bottom_field.frame_store = pCurrPic->frame_store;
        pMvcCtx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx; // ������Ч��Ϊ��ʡ�ж�ֱ�Ӹ�ֵ
        pMvcCtx->DPB.fs[pos]->frame_num = pCurrPic->frame_num;
        pMvcCtx->DPB.fs[pos]->frame.structure = MVC_FRAME;
        pMvcCtx->DPB.fs[pos]->frame.is_long_term = pCurrPic->is_long_term;
        pMvcCtx->DPB.fs[pos]->frame.is_short_term = pCurrPic->is_short_term;
        pMvcCtx->DPB.fs[pos]->pic_type = pMvcCtx->DPB.fs[pos]->frame.pic_type = pCurrPic->pic_type;
        pMvcCtx->DPB.fs[pos]->MbaffFrameFlag = pCurrPic->MbaffFrameFlag;

        pMvcCtx->DPB.fs[pos]->poc = pCurrPic->thispoc;
        pMvcCtx->DPB.fs[pos]->frame.poc = pCurrPic->framepoc;
        pMvcCtx->DPB.fs[pos]->top_field.poc = pCurrPic->toppoc;
        pMvcCtx->DPB.fs[pos]->bottom_field.poc = pCurrPic->bottompoc;   

        pMvcCtx->DPB.fs[pos]->MVC_DecPOC = pCurrPic->decthispoc;
        pMvcCtx->DPB.fs[pos]->frame.MVC_DecPOC = pCurrPic->decframepoc;
        pMvcCtx->DPB.fs[pos]->top_field.MVC_DecPOC = pCurrPic->dectoppoc;
        pMvcCtx->DPB.fs[pos]->bottom_field.MVC_DecPOC = pCurrPic->decbottompoc;   
        pMvcCtx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ? 3 : 0;

         /* MVC */
        pMvcCtx->DPB.fs[pos]->view_id = pCurrPic->view_id;
        pMvcCtx->DPB.fs[pos]->voidx = pCurrPic->voidx;
        pMvcCtx->DPB.fs[pos]->inter_view_flag[0] = pMvcCtx->DPB.fs[pos]->inter_view_flag[1] = pCurrPic->inter_view_flag;
        pMvcCtx->DPB.fs[pos]->anchor_pic_flag[0] = pMvcCtx->DPB.fs[pos]->anchor_pic_flag[1] = pCurrPic->anchor_pic_flag;
        /* End */
        pMvcCtx->DPB.fs[pos]->dpb_cnt = cnt;
		cnt++;
        MVC_SplitFrmToFlds(pMvcCtx->DPB.fs[pos]);
        ret = MVC_GetAPC(pMvcCtx, pCurrPic->frame_store, MVC_FRAME);
        if (MVC_OK != ret)
        {
            dprint(PRN_FATAL,"line %d, get APC error, ret %d\n", __LINE__, ret);
            MVC_ClearDPB(pMvcCtx,-1);  //APC_ERR
            return MVC_ERR;
        }
        break;
    case MVC_TOP_FIELD:
        pCurrPic->frame_store->frame.structure = MVC_FIELD_PAIR;
        if (0 == pCurrPic->field_pair_flag || (MVC_FRAMESTORE_S *)NULL == pMvcCtx->DPB.fs[pos])
        {
            // δ�ɶ�
            pMvcCtx->DPB.fs[pos] = pCurrPic->frame_store;

            pMvcCtx->DPB.fs[pos]->top_field.structure = MVC_TOP_FIELD;
            pMvcCtx->DPB.fs[pos]->top_field.frame_store = pCurrPic->frame_store;
            pMvcCtx->DPB.fs[pos]->top_field.is_long_term = pCurrPic->is_long_term;
            pMvcCtx->DPB.fs[pos]->top_field.is_short_term = pCurrPic->is_short_term;  
            pMvcCtx->DPB.fs[pos]->top_field.poc = pCurrPic->toppoc;
            pMvcCtx->DPB.fs[pos]->top_field.pic_type = pCurrPic->pic_type;

            pMvcCtx->DPB.fs[pos]->top_field.err_level = pCurrPic->err_level;
            pMvcCtx->DPB.fs[pos]->err_level = pMvcCtx->DPB.fs[pos]->frame.err_level = pMvcCtx->DPB.fs[pos]->top_field.err_level;

            pMvcCtx->DPB.fs[pos]->non_existing = pCurrPic->non_existing; // must be 0 
            pMvcCtx->DPB.fs[pos]->is_used = 1;
            pMvcCtx->DPB.fs[pos]->is_in_dpb = pCurrPic->frame_store->is_in_dpb;
            pMvcCtx->DPB.fs[pos]->is_displayed = pCurrPic->frame_store->is_displayed;     
            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pMvcCtx->DPB.fs[pos]->is_reference = 1;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pMvcCtx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pMvcCtx->DPB.fs[pos]->MbaffFrameFlag = 0;            
            pMvcCtx->DPB.fs[pos]->fs_image = pCurrPic->fs_image; // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            pMvcCtx->DPB.fs[pos]->pmv_address_idc = pCurrPic->pmv_address_idc;
            pMvcCtx->DPB.fs[pos]->frame_num = pCurrPic->frame_num;
            pMvcCtx->DPB.fs[pos]->poc = pCurrPic->thispoc;         
            pMvcCtx->DPB.fs[pos]->MVC_DecPOC = pCurrPic->decthispoc;
            pMvcCtx->DPB.fs[pos]->pic_type = pCurrPic->pic_type;
            pMvcCtx->DPB.fs[pos]->top_field.MVC_DecPOC = pCurrPic->dectoppoc;

             /* MVC */
            pMvcCtx->DPB.fs[pos]->view_id = pCurrPic->view_id;
            pMvcCtx->DPB.fs[pos]->voidx = pCurrPic->voidx;
            pMvcCtx->DPB.fs[pos]->inter_view_flag[0] = pCurrPic->inter_view_flag;
            pMvcCtx->DPB.fs[pos]->anchor_pic_flag[0] = pCurrPic->anchor_pic_flag;
            /* End */
            pMvcCtx->DPB.fs[pos]->dpb_cnt = cnt;
			cnt++;
            //g_DPB.fs[pos]->mode_skip_flag = pCurrPic->mode_skip_flag; 
            pMvcCtx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ? 1 : 0;
            ret = MVC_GetAPC(pMvcCtx, pCurrPic->frame_store, MVC_TOP_FIELD);
            if (MVC_OK != ret)
            {
                dprint(PRN_FATAL,"line %d, get APC error, ret %d\n", __LINE__, ret);
                MVC_ClearDPB(pMvcCtx,-1);  //APC_ERR
                return MVC_ERR;    
            }
        }
        else
        {
            pMvcCtx->DPB.fs[pos]->top_field.structure = MVC_TOP_FIELD;
            pMvcCtx->DPB.fs[pos]->top_field.pic_type = pCurrPic->pic_type;
            pMvcCtx->DPB.fs[pos]->top_field.is_long_term = pCurrPic->is_long_term;
            pMvcCtx->DPB.fs[pos]->top_field.is_short_term = pCurrPic->is_short_term;  
            pMvcCtx->DPB.fs[pos]->top_field.poc = pCurrPic->toppoc;
            pMvcCtx->DPB.fs[pos]->top_field.pic_type = pCurrPic->pic_type;

            pMvcCtx->DPB.fs[pos]->top_field.err_level = pCurrPic->err_level;
            pMvcCtx->DPB.fs[pos]->err_level = pMvcCtx->DPB.fs[pos]->frame.err_level = 
                pMvcCtx->DPB.fs[pos]->top_field.err_level + pMvcCtx->DPB.fs[pos]->bottom_field.err_level;

            // �ɶԣ�frame_store�еĹ����������Բ��ٸ�ֵ��ֻ��ֵ��������ı��
            pMvcCtx->DPB.fs[pos]->non_existing = pMvcCtx->DPB.fs[pos]->non_existing ? pCurrPic->non_existing : 0;
            pMvcCtx->DPB.fs[pos]->is_used |= 1;  
            //g_DPB.fs[pos]->mode_skip_flag |= pCurrPic->mode_skip_flag;
            pMvcCtx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ?
                (pMvcCtx->DPB.fs[pos]->mode_skip_flag | 1) : pMvcCtx->DPB.fs[pos]->mode_skip_flag;
            OtherFieldPts = pMvcCtx->DPB.fs[pos]->fs_image.PTS;
            OtherFieldUsertag = pMvcCtx->DPB.fs[pos]->fs_image.Usertag;
            OtherFieldDispTime = pMvcCtx->DPB.fs[pos]->fs_image.DispTime;
            pMvcCtx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            if(pCurrPic->fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pMvcCtx->DPB.fs[pos]->fs_image.PTS = OtherFieldPts;
            }
            pMvcCtx->DPB.fs[pos]->fs_image.Usertag = (OtherFieldUsertag < pCurrPic->fs_image.Usertag)?
                OtherFieldUsertag : pCurrPic->fs_image.Usertag;
            pMvcCtx->DPB.fs[pos]->fs_image.DispTime= (OtherFieldDispTime < pCurrPic->fs_image.DispTime)?
                OtherFieldDispTime : pCurrPic->fs_image.DispTime;
            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pMvcCtx->DPB.fs[pos]->is_reference |= 1;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pMvcCtx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pMvcCtx->DPB.fs[pos]->pic_type = MAX(pMvcCtx->DPB.fs[pos]->top_field.pic_type, pMvcCtx->DPB.fs[pos]->bottom_field.pic_type);
            //pMvcCtx->DPB.fs[pos]->poc = pCurrPic->thispoc;
            pMvcCtx->DPB.fs[pos]->top_field.MVC_DecPOC = pCurrPic->dectoppoc;
            pMvcCtx->DPB.fs[pos]->MVC_DecPOC = MIN(pMvcCtx->DPB.fs[pos]->top_field.MVC_DecPOC,pMvcCtx->DPB.fs[pos]->bottom_field.MVC_DecPOC);

            /* MVC */
            pMvcCtx->DPB.fs[pos]->inter_view_flag[0] = pCurrPic->inter_view_flag;
            pMvcCtx->DPB.fs[pos]->anchor_pic_flag[0] = pCurrPic->anchor_pic_flag;
            /* End */

            MVC_CombineFldsToFrm(pMvcCtx->DPB.fs[pos]);

            //modify apc poc
            pMvcCtx->APC.poc[0][pMvcCtx->DPB.fs[pos]->apc_idc] = pMvcCtx->DPB.fs[pos]->top_field.poc;
        }  
        break;
    case BOTTOM_FIELD:
        pCurrPic->frame_store->frame.structure = MVC_FIELD_PAIR;
        if (0 == pCurrPic->field_pair_flag || (MVC_FRAMESTORE_S *)NULL == pMvcCtx->DPB.fs[pos])
        {
            // δ�ɶ�
            pMvcCtx->DPB.fs[pos] = pCurrPic->frame_store;

            pMvcCtx->DPB.fs[pos]->bottom_field.structure = BOTTOM_FIELD;
            pMvcCtx->DPB.fs[pos]->bottom_field.frame_store = pCurrPic->frame_store;
            pMvcCtx->DPB.fs[pos]->bottom_field.is_long_term = pCurrPic->is_long_term;
            pMvcCtx->DPB.fs[pos]->bottom_field.is_short_term = pCurrPic->is_short_term;  
            pMvcCtx->DPB.fs[pos]->bottom_field.poc = pCurrPic->bottompoc;
            pMvcCtx->DPB.fs[pos]->bottom_field.pic_type = pCurrPic->pic_type;

            pMvcCtx->DPB.fs[pos]->bottom_field.err_level = pCurrPic->err_level;
            pMvcCtx->DPB.fs[pos]->err_level = pMvcCtx->DPB.fs[pos]->frame.err_level = pMvcCtx->DPB.fs[pos]->top_field.err_level;

            pMvcCtx->DPB.fs[pos]->non_existing = pCurrPic->non_existing;  //must be 0 
            pMvcCtx->DPB.fs[pos]->is_used = 2;
            pMvcCtx->DPB.fs[pos]->is_in_dpb = pCurrPic->frame_store->is_in_dpb;
            pMvcCtx->DPB.fs[pos]->is_displayed = pCurrPic->frame_store->is_displayed;
            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pMvcCtx->DPB.fs[pos]->is_reference = 2;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pMvcCtx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pMvcCtx->DPB.fs[pos]->MbaffFrameFlag = 0;   
            pMvcCtx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            pMvcCtx->DPB.fs[pos]->pmv_address_idc = pCurrPic->pmv_address_idc;
            pMvcCtx->DPB.fs[pos]->frame_num = pCurrPic->frame_num;
            pMvcCtx->DPB.fs[pos]->poc = pCurrPic->thispoc;         
            pMvcCtx->DPB.fs[pos]->MVC_DecPOC = pCurrPic->decthispoc;
            pMvcCtx->DPB.fs[pos]->pic_type = pMvcCtx->DPB.fs[pos]->bottom_field.pic_type = pCurrPic->pic_type;
            pMvcCtx->DPB.fs[pos]->bottom_field.MVC_DecPOC = pCurrPic->decbottompoc;

            /* MVC */
            pMvcCtx->DPB.fs[pos]->view_id = pCurrPic->view_id;
            pMvcCtx->DPB.fs[pos]->voidx = pCurrPic->voidx;
            pMvcCtx->DPB.fs[pos]->inter_view_flag[1] = pCurrPic->inter_view_flag;
            pMvcCtx->DPB.fs[pos]->anchor_pic_flag[1] = pCurrPic->anchor_pic_flag;		
            /* End */
            pMvcCtx->DPB.fs[pos]->dpb_cnt = cnt;
			cnt++;

            //g_DPB.fs[pos]->mode_skip_flag = pCurrPic->mode_skip_flag;
            pMvcCtx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ? 2 : 0;

            ret = MVC_GetAPC(pMvcCtx, pCurrPic->frame_store, BOTTOM_FIELD);
            if (MVC_OK != ret)
            {
                dprint(PRN_FATAL,"line %d, get APC error, ret %d\n", __LINE__, ret);
                MVC_ClearDPB(pMvcCtx,-1);  //APC_ERR
                return MVC_ERR;    
            }
        }
        else
        {
            // �ɶԣ�frame_store�еĹ����������Բ��ٸ�ֵ��ֻ��ֵ��������ı��
            pMvcCtx->DPB.fs[pos]->bottom_field.structure = BOTTOM_FIELD;
            pMvcCtx->DPB.fs[pos]->bottom_field.frame_store = pCurrPic->frame_store;
            pMvcCtx->DPB.fs[pos]->bottom_field.is_long_term = pCurrPic->is_long_term;
            pMvcCtx->DPB.fs[pos]->bottom_field.is_short_term = pCurrPic->is_short_term;  
            pMvcCtx->DPB.fs[pos]->bottom_field.poc = pCurrPic->bottompoc;
            pMvcCtx->DPB.fs[pos]->bottom_field.pic_type = pCurrPic->pic_type;

            pMvcCtx->DPB.fs[pos]->bottom_field.err_level = pCurrPic->err_level;
            pMvcCtx->DPB.fs[pos]->err_level = pMvcCtx->DPB.fs[pos]->frame.err_level = 
                pMvcCtx->DPB.fs[pos]->top_field.err_level + pMvcCtx->DPB.fs[pos]->bottom_field.err_level;
            
            pMvcCtx->DPB.fs[pos]->non_existing = pMvcCtx->DPB.fs[pos]->non_existing ? pCurrPic->non_existing : 0;
            pMvcCtx->DPB.fs[pos]->is_used |= 2;    
            //g_DPB.fs[pos]->mode_skip_flag |= pCurrPic->mode_skip_flag;
            pMvcCtx->DPB.fs[pos]->mode_skip_flag = (pCurrPic->mode_skip_flag == 1) ?
                (pMvcCtx->DPB.fs[pos]->mode_skip_flag | 2) : pMvcCtx->DPB.fs[pos]->mode_skip_flag;    
            OtherFieldPts = pMvcCtx->DPB.fs[pos]->fs_image.PTS;
            OtherFieldUsertag = pMvcCtx->DPB.fs[pos]->fs_image.Usertag;
            OtherFieldDispTime = pMvcCtx->DPB.fs[pos]->fs_image.DispTime;

            pMvcCtx->DPB.fs[pos]->fs_image = pCurrPic->fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            if(pCurrPic->fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pMvcCtx->DPB.fs[pos]->fs_image.PTS = OtherFieldPts;
            }
            pMvcCtx->DPB.fs[pos]->fs_image.Usertag = (OtherFieldUsertag < pCurrPic->fs_image.Usertag)?
                OtherFieldUsertag : pCurrPic->fs_image.Usertag;
            pMvcCtx->DPB.fs[pos]->fs_image.DispTime = (OtherFieldDispTime < pCurrPic->fs_image.DispTime)?
                OtherFieldDispTime : pCurrPic->fs_image.DispTime;

            if (1 == pCurrPic->is_long_term || 1 == pCurrPic->is_short_term)
            {
                pMvcCtx->DPB.fs[pos]->is_reference |= 2;
            }
            if (1 == pCurrPic->is_long_term)
            {
                pMvcCtx->DPB.fs[pos]->long_term_frame_idx = pCurrPic->long_term_frame_idx;
            }
            pMvcCtx->DPB.fs[pos]->pic_type = MAX(pMvcCtx->DPB.fs[pos]->top_field.pic_type, pMvcCtx->DPB.fs[pos]->bottom_field.pic_type);
            //pMvcCtx->DPB.fs[pos]->poc = pCurrPic->thispoc;         
            pMvcCtx->DPB.fs[pos]->bottom_field.MVC_DecPOC = pCurrPic->decbottompoc;
            pMvcCtx->DPB.fs[pos]->MVC_DecPOC = MIN(pMvcCtx->DPB.fs[pos]->top_field.MVC_DecPOC,pMvcCtx->DPB.fs[pos]->bottom_field.MVC_DecPOC);
            MVC_CombineFldsToFrm(pMvcCtx->DPB.fs[pos]);

            /* MVC */
            pMvcCtx->DPB.fs[pos]->inter_view_flag[1] = pCurrPic->inter_view_flag;
            pMvcCtx->DPB.fs[pos]->anchor_pic_flag[1] = pCurrPic->anchor_pic_flag;
            /* End */

            //modify apc poc
            pMvcCtx->APC.poc[1][pMvcCtx->DPB.fs[pos]->apc_idc] = pMvcCtx->DPB.fs[pos]->bottom_field.poc;
        }  
        break;
    default:
        // �ڳ��������£�Ϊ�˲��ı�DPB״����ֻ����ǰͼ����Ϊ�գ�
        dprint(PRN_FATAL,"InserFrmInDPB: cur pic struct = %d!\n", pCurrPic->structure);
        MVC_ClearAll(pMvcCtx,1);
        return MVC_OK;     
    }

    // ��ǰͼ���ÿ�
    //pCurrPic->state = MVC_EMPTY;

    /* �߼�֡���Ϊ���ο� */
    if (0 != pMvcCtx->DPB.fs[pos]->is_reference)
    {
        FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->DPB.fs[pos]->logic_fs_id, 1);
    }

    return MVC_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_DirectOutput(MVC_CTX_S *pMvcCtx)
*      �������� :  ��ǰ����ͼ�����ο�������poc��DPB������û�������ͼ��
*                  ��poc��С
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL:��������
*                  OUT_FLAG:�²㺯�����ܷ��ظ�ֵ��ϵͳҪ���˳���
*
************************************************************************
*/
SINT32 MVC_DirectOutput(MVC_CTX_S *pMvcCtx)
{
    SINT32 ret = MVC_OK;
    UINT64 OtherFieldPts;
    UINT64 OtherFieldUsertag;
    UINT64 OtherFieldDisptime;

    switch (pMvcCtx->CurrPic.structure)
    { 
    case MVC_FRAME:
        pMvcCtx->CurrPic.frame_store->frame.structure = MVC_FRAME;
        // �����directbuf���ٰ����ڽ����ͼ�����directbuf
        if ((MVC_FRAMESTORE_S *)NULL != pMvcCtx->pDirectOutBuf) 
        {
            ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
            if(pMvcCtx->pDirectOutBuf != NULL)
            {
                FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
            }
            pMvcCtx->pDirectOutBuf = NULL;
        }
        pMvcCtx->pDirectOutBuf = pMvcCtx->CurrPic.frame_store;
        pMvcCtx->pDirectOutBuf->frame.frame_store = pMvcCtx->CurrPic.frame_store;
        pMvcCtx->pDirectOutBuf->top_field.frame_store = pMvcCtx->CurrPic.frame_store;
        pMvcCtx->pDirectOutBuf->bottom_field.frame_store = pMvcCtx->CurrPic.frame_store;

        pMvcCtx->pDirectOutBuf->non_existing = pMvcCtx->CurrPic.non_existing;
        pMvcCtx->pDirectOutBuf->is_used = 3;
        pMvcCtx->pDirectOutBuf->is_reference = 0;
        pMvcCtx->pDirectOutBuf->is_in_dpb = 0;
        pMvcCtx->pDirectOutBuf->is_displayed = 0;
        pMvcCtx->pDirectOutBuf->fs_image = pMvcCtx->CurrPic.fs_image;
        pMvcCtx->pDirectOutBuf->err_level = pMvcCtx->pDirectOutBuf->frame.err_level = pMvcCtx->CurrPic.err_level;
        pMvcCtx->pDirectOutBuf->frame.structure = MVC_FRAME;
        pMvcCtx->pDirectOutBuf->pic_type = pMvcCtx->CurrPic.pic_type;
        pMvcCtx->pDirectOutBuf->MbaffFrameFlag = pMvcCtx->CurrPic.MbaffFrameFlag;
        pMvcCtx->pDirectOutBuf->mode_skip_flag = (pMvcCtx->CurrPic.mode_skip_flag == 1) ? 3 : 0;

        pMvcCtx->pDirectOutBuf->poc = pMvcCtx->CurrPic.thispoc;
        pMvcCtx->pDirectOutBuf->view_id = pMvcCtx->CurrPic.view_id;
        pMvcCtx->pDirectOutBuf->voidx = pMvcCtx->CurrPic.voidx;

        FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
        ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
        if(pMvcCtx->pDirectOutBuf != NULL)
        {
            FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
        }
        pMvcCtx->pDirectOutBuf = NULL;
        break;
    case MVC_TOP_FIELD:
        pMvcCtx->CurrPic.frame_store->frame.structure = MVC_FIELD_PAIR;
        if (0 == pMvcCtx->CurrPic.field_pair_flag)
        {
            // ���ɶ�
            if ((MVC_FRAMESTORE_S *)NULL != pMvcCtx->pDirectOutBuf)
            {
                // ֮ǰ����һ������Ҫ���
                ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
                pMvcCtx->pDirectOutBuf = pMvcCtx->CurrPic.frame_store;
                pMvcCtx->pDirectOutBuf->top_field.frame_store = pMvcCtx->CurrPic.frame_store;
            }
            else
            {
                pMvcCtx->pDirectOutBuf = pMvcCtx->CurrPic.frame_store;
                pMvcCtx->pDirectOutBuf->top_field.frame_store = pMvcCtx->CurrPic.frame_store;
            }
            pMvcCtx->pDirectOutBuf->non_existing = pMvcCtx->CurrPic.non_existing;  //must be 0 
            pMvcCtx->pDirectOutBuf->is_used = 1;
            pMvcCtx->pDirectOutBuf->is_in_dpb = 0;
            pMvcCtx->pDirectOutBuf->is_displayed = 0;
            pMvcCtx->pDirectOutBuf->is_reference = 0;
            pMvcCtx->pDirectOutBuf->pic_type = pMvcCtx->CurrPic.pic_type;
            pMvcCtx->pDirectOutBuf->MbaffFrameFlag = 0;
            pMvcCtx->pDirectOutBuf->fs_image = pMvcCtx->CurrPic.fs_image;  // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����       
            pMvcCtx->pDirectOutBuf->top_field.err_level = pMvcCtx->CurrPic.err_level;
            pMvcCtx->pDirectOutBuf->err_level = pMvcCtx->pDirectOutBuf->frame.err_level = pMvcCtx->pDirectOutBuf->top_field.err_level;
            pMvcCtx->pDirectOutBuf->top_field.structure = MVC_TOP_FIELD;
            pMvcCtx->pDirectOutBuf->mode_skip_flag = (pMvcCtx->CurrPic.mode_skip_flag == 1) ? 1 : 0;  

            pMvcCtx->pDirectOutBuf->poc = pMvcCtx->CurrPic.thispoc;
            pMvcCtx->pDirectOutBuf->view_id = pMvcCtx->CurrPic.view_id;
            pMvcCtx->pDirectOutBuf->voidx = pMvcCtx->CurrPic.voidx;

            if (1 == pMvcCtx->IModeOutFlag)
            {
                FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
                ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
                if(pMvcCtx->pDirectOutBuf != NULL)
                {
                    FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
                }
                pMvcCtx->pDirectOutBuf = NULL;
            }
        }
        else
        {
            // �ɶԣ�����pDirectOutBufһ������
            if ((MVC_FRAMESTORE_S *)NULL == pMvcCtx->pDirectOutBuf)
            {
                pMvcCtx->pDirectOutBuf = pMvcCtx->CurrPic.frame_store;
                pMvcCtx->pDirectOutBuf->top_field.frame_store = pMvcCtx->CurrPic.frame_store;
            }
            /* 20111212: �����ɶ�ʱ���轫������Ч�ĳ� pts ���� �ɶԵ� MVC_FRAME */
            OtherFieldPts = pMvcCtx->pDirectOutBuf->fs_image.PTS;
            OtherFieldUsertag = pMvcCtx->pDirectOutBuf->fs_image.Usertag;
            OtherFieldDisptime = pMvcCtx->pDirectOutBuf->fs_image.DispTime;
            pMvcCtx->pDirectOutBuf->fs_image = pMvcCtx->CurrPic.fs_image;  // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ���� 
            if(pMvcCtx->CurrPic.fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pMvcCtx->pDirectOutBuf->fs_image.PTS = OtherFieldPts;
            }
            pMvcCtx->pDirectOutBuf->fs_image.Usertag = (OtherFieldUsertag < pMvcCtx->CurrPic.fs_image.Usertag)?
                OtherFieldUsertag : pMvcCtx->CurrPic.fs_image.Usertag;
            pMvcCtx->pDirectOutBuf->fs_image.DispTime= (OtherFieldDisptime < pMvcCtx->CurrPic.fs_image.DispTime)?
                OtherFieldDisptime : pMvcCtx->CurrPic.fs_image.DispTime;
            pMvcCtx->pDirectOutBuf->non_existing = pMvcCtx->pDirectOutBuf->non_existing ? pMvcCtx->CurrPic.non_existing : 0;
            pMvcCtx->pDirectOutBuf->is_used |= 1;
            pMvcCtx->pDirectOutBuf->pic_type = MAX(pMvcCtx->CurrPic.pic_type, pMvcCtx->pDirectOutBuf->pic_type);
            pMvcCtx->pDirectOutBuf->top_field.structure = MVC_TOP_FIELD;
            pMvcCtx->pDirectOutBuf->frame.structure = MVC_FIELD_PAIR;  // field pair more important
            
            pMvcCtx->pDirectOutBuf->top_field.err_level = pMvcCtx->CurrPic.err_level;
            pMvcCtx->pDirectOutBuf->frame.err_level = (pMvcCtx->pDirectOutBuf->top_field.err_level + pMvcCtx->pDirectOutBuf->bottom_field.err_level)/2;
            pMvcCtx->pDirectOutBuf->err_level = pMvcCtx->pDirectOutBuf->frame.err_level;
            
            pMvcCtx->pDirectOutBuf->frame.frame_store = pMvcCtx->pDirectOutBuf->top_field.frame_store;  
            pMvcCtx->pDirectOutBuf->mode_skip_flag = (pMvcCtx->CurrPic.mode_skip_flag == 1) ?
                (pMvcCtx->pDirectOutBuf->mode_skip_flag | 1) : pMvcCtx->pDirectOutBuf->mode_skip_flag;

            pMvcCtx->pDirectOutBuf->poc = pMvcCtx->CurrPic.thispoc;
		
            FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
            ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
            if(pMvcCtx->pDirectOutBuf != NULL)
            {
                FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
            }
            pMvcCtx->pDirectOutBuf = NULL;            
        }

        //pMvcCtx->CurrPic.state = MVC_EMPTY;
        break;
    case BOTTOM_FIELD:
        pMvcCtx->CurrPic.frame_store->frame.structure = MVC_FIELD_PAIR;
        if (0 == pMvcCtx->CurrPic.field_pair_flag)
        {
            // ���ɶ�
            if ((MVC_FRAMESTORE_S *)NULL != pMvcCtx->pDirectOutBuf)
            {
                // ֮ǰ����һ������Ҫ���
                ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
                pMvcCtx->pDirectOutBuf = pMvcCtx->CurrPic.frame_store;
                pMvcCtx->pDirectOutBuf->bottom_field.frame_store = pMvcCtx->CurrPic.frame_store;
            }
            else
            {
                pMvcCtx->pDirectOutBuf = pMvcCtx->CurrPic.frame_store;
                pMvcCtx->pDirectOutBuf->bottom_field.frame_store = pMvcCtx->CurrPic.frame_store;
            }
            pMvcCtx->pDirectOutBuf->non_existing = pMvcCtx->CurrPic.non_existing;  //must be 0 
            pMvcCtx->pDirectOutBuf->is_used = 2;
            pMvcCtx->pDirectOutBuf->is_in_dpb = 0;
            pMvcCtx->pDirectOutBuf->is_displayed = 0;
            pMvcCtx->pDirectOutBuf->is_reference = 0;
            pMvcCtx->pDirectOutBuf->pic_type = pMvcCtx->CurrPic.pic_type;
            pMvcCtx->pDirectOutBuf->MbaffFrameFlag = 0;
            pMvcCtx->pDirectOutBuf->fs_image = pMvcCtx->CurrPic.fs_image;//Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ����
            
            pMvcCtx->pDirectOutBuf->bottom_field.err_level = pMvcCtx->CurrPic.err_level;
            pMvcCtx->pDirectOutBuf->err_level = pMvcCtx->pDirectOutBuf->frame.err_level = pMvcCtx->pDirectOutBuf->bottom_field.err_level;
            
            pMvcCtx->pDirectOutBuf->bottom_field.structure = BOTTOM_FIELD;  
            pMvcCtx->pDirectOutBuf->mode_skip_flag = (pMvcCtx->CurrPic.mode_skip_flag == 1) ? 2 : 0;

            pMvcCtx->pDirectOutBuf->poc = pMvcCtx->CurrPic.thispoc;
            pMvcCtx->pDirectOutBuf->view_id = pMvcCtx->CurrPic.view_id;
            pMvcCtx->pDirectOutBuf->voidx = pMvcCtx->CurrPic.voidx;

            if (1 == pMvcCtx->IModeOutFlag)
            {
                FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
                ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
                if(pMvcCtx->pDirectOutBuf != NULL)
                {
                    FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
                }
                pMvcCtx->pDirectOutBuf = NULL;                
            }
        }
        else
        {
            // �ɶԣ�����pDirectOutBufһ������
            if ((MVC_FRAMESTORE_S *)NULL == pMvcCtx->pDirectOutBuf)
            {
                pMvcCtx->pDirectOutBuf = pMvcCtx->CurrPic.frame_store;
                pMvcCtx->pDirectOutBuf->bottom_field.frame_store = pMvcCtx->CurrPic.frame_store;
            }
            /* 20111212: �����ɶ�ʱ���轫������Ч�ĳ� pts ���� �ɶԵ� MVC_FRAME */
            OtherFieldPts = pMvcCtx->pDirectOutBuf->fs_image.PTS;
            OtherFieldUsertag = pMvcCtx->pDirectOutBuf->fs_image.Usertag;
            OtherFieldDisptime = pMvcCtx->pDirectOutBuf->fs_image.DispTime;

            pMvcCtx->pDirectOutBuf->fs_image = pMvcCtx->CurrPic.fs_image; // Ҳ�����ǰ�γɶԳ��ģ���ֵ��һ���� 
            if(pMvcCtx->CurrPic.fs_image.PTS == (UINT64)(-1) && OtherFieldPts != (UINT64)(-1))
            {
                pMvcCtx->pDirectOutBuf->fs_image.PTS = OtherFieldPts;
            }
            pMvcCtx->pDirectOutBuf->fs_image.Usertag = (OtherFieldUsertag < pMvcCtx->CurrPic.fs_image.Usertag)?
                OtherFieldUsertag : pMvcCtx->CurrPic.fs_image.Usertag;
            pMvcCtx->pDirectOutBuf->fs_image.DispTime = (OtherFieldDisptime < pMvcCtx->CurrPic.fs_image.DispTime)?
                OtherFieldDisptime : pMvcCtx->CurrPic.fs_image.DispTime;
            pMvcCtx->pDirectOutBuf->non_existing = pMvcCtx->pDirectOutBuf->non_existing ? pMvcCtx->CurrPic.non_existing : 0;
            pMvcCtx->pDirectOutBuf->is_used |= 2;
            pMvcCtx->pDirectOutBuf->pic_type = MAX(pMvcCtx->CurrPic.pic_type,pMvcCtx->pDirectOutBuf->pic_type);
            pMvcCtx->pDirectOutBuf->bottom_field.structure = BOTTOM_FIELD;
            pMvcCtx->pDirectOutBuf->frame.structure = MVC_FIELD_PAIR; // field pair more important

            pMvcCtx->pDirectOutBuf->bottom_field.err_level = pMvcCtx->CurrPic.err_level;
            pMvcCtx->pDirectOutBuf->frame.err_level = (pMvcCtx->pDirectOutBuf->top_field.err_level + pMvcCtx->pDirectOutBuf->bottom_field.err_level)/2;
            pMvcCtx->pDirectOutBuf->err_level = pMvcCtx->pDirectOutBuf->frame.err_level;

            pMvcCtx->pDirectOutBuf->frame.frame_store = pMvcCtx->pDirectOutBuf->bottom_field.frame_store; 
            pMvcCtx->pDirectOutBuf->mode_skip_flag = (pMvcCtx->CurrPic.mode_skip_flag == 1) ?
                (pMvcCtx->pDirectOutBuf->mode_skip_flag | 2) : pMvcCtx->pDirectOutBuf->mode_skip_flag;

            pMvcCtx->pDirectOutBuf->poc = pMvcCtx->CurrPic.thispoc;

            FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->pDirectOutBuf->logic_fs_id, 0);  // USE_FSP
            ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
            if (pMvcCtx->pDirectOutBuf != NULL)
            {
                FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
            }
            pMvcCtx->pDirectOutBuf = NULL;            
        }
        //pMvcCtx->CurrPic.state = MVC_EMPTY;
        break; 
    default:
        //pMvcCtx->CurrPic.state = MVC_EMPTY;
        break;
    }

    return ret;
}


/*!
************************************************************************
*      ����ԭ�� :      SINT32 MVC_FlushDPB(MVC_CTX_S *pMvcCtx)
*      �������� :      ȫ�����g_DPB����ǰͼ��
*      ����˵�� :  
*      ����ֵ   ��
???????�����ǰͼ��ʹ�ʱDPB��ͼ��ɶԣ���ô���� 
************************************************************************
*/
SINT32 MVC_FlushDPB(MVC_CTX_S *pMvcCtx, SINT32 CurrViewId)
{
    UINT32 i;
    SINT32 ret;
    UINT32 pos;
    SINT32 min_poc;
    UINT32 flush_size = 0;
    SINT32 other_view_id = -1;
    // mark all frames unused
//    for (i=0; i<pMvcCtx->DPB.used_size; i++)
    for (i=0; i<pMvcCtx->DPB.size; i++)
    {
        if (pMvcCtx->DPB.fs[i]==NULL)
		{
		    continue;
        }
        if ((0 != pMvcCtx->DPB.fs[i]->is_reference) && 
            (pMvcCtx->DPB.fs[i]->view_id == CurrViewId || -1==CurrViewId))
        {
            MVC_UnMarkFrameStoreRef (pMvcCtx, pMvcCtx->DPB.fs[i]);
        }
    }

    //while ( MVC_RemoveUnUsedFrameStore(pMvcCtx, CurrViewId) );
    MVC_RemoveUnUsedFrameStore(pMvcCtx, CurrViewId);
	
//    for(i=0; i<pMvcCtx->DPB.used_size; i++)
    for(i=0; i<pMvcCtx->DPB.size; i++)
    {
        if (pMvcCtx->DPB.fs[i]==NULL)
		{
		    continue;
        }    
        if(pMvcCtx->DPB.fs[i]->view_id == CurrViewId || -1 == CurrViewId)
        {
            flush_size++;
        }
	 if (CurrViewId != -1 && pMvcCtx->DPB.fs[i]->view_id != CurrViewId )
	 {
	     other_view_id = pMvcCtx->DPB.fs[i]->view_id;
	 }
    }
	
    // output frames in POC order
    while (flush_size)
    {
        if (other_view_id != -1 && other_view_id < CurrViewId)
        {
            MVC_GetMinPOC(pMvcCtx, other_view_id, &min_poc, &pos);
            if (pos == (UINT32)(-1))
            {
                break;
            }
            ret = MVC_OutputFrmFromDPB(pMvcCtx, pos);
            if (MVC_OK != ret)
            {
                return MVC_ERR;
            }
        }

        MVC_GetMinPOC(pMvcCtx, CurrViewId, &min_poc, &pos);
        if (pos == (UINT32)(-1))
        {
            break;
        }

        ret = MVC_OutputFrmFromDPB(pMvcCtx, pos);
        if (MVC_OK != ret)
        {
            return MVC_ERR;
        }
        
        if (other_view_id != -1  && other_view_id > CurrViewId)
        {
            MVC_GetMinPOC(pMvcCtx, other_view_id, &min_poc, &pos);
            if (pos == (UINT32)(-1))
            {
                break;
            }

            ret = MVC_OutputFrmFromDPB(pMvcCtx, pos);
            if (MVC_OK != ret)
            {
                return MVC_ERR;
            }
	 }

        flush_size--;
    }

    return MVC_OK;
}

/*!
************************************************************************
*      ����ԭ�� :      SINT32 MVC_DEC_GetRemainImg(MVC_CTX_S *pMvcCtx)
*      �������� :      ȫ�����g_DPB����ǰͼ��, ����������һ֡
*      ����˵�� :  
*      ����ֵ   ��
************************************************************************
*/
SINT32 MVC_DEC_GetRemainImg(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    SINT32 ret = LAST_OUTPUT_FAIL;
    UINT32 pos;
    SINT32 min_poc;
    UINT32 flush_size = 0;
    FSP_LOGIC_FS_S *pLogicfs;

    for(i=0; i<pMvcCtx->DPB.size; i++)
    {
        if (pMvcCtx->DPB.fs[i] == NULL)
        {
            continue;
        }   

        if (pMvcCtx->DPB.fs[i]->is_in_dpb)
        {
            pLogicfs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs[i]->logic_fs_id);
            if (pLogicfs == NULL)
            {
                continue;
            }
            
            if (FS_DISP_STATE_DEC_COMPLETE==pLogicfs->s32DispState || FS_DISP_STATE_ALLOCED==pLogicfs->s32DispState)
            {
                flush_size++;
            }
        }
    }
    
    // output frames in POC order
    while (flush_size)
    {
        MVC_GetMinPOC(pMvcCtx, -1, &min_poc, &pos);
        if (pos == -1)
        {
            break;
        }

        ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->DPB.fs[pos], flush_size);

        flush_size--;
    }

    for (i=0; i<pMvcCtx->DPB.size; i++)
    {
        if (pMvcCtx->DPB.fs[i]==NULL)
        {
            continue;
        }
        if (0 != pMvcCtx->DPB.fs[i]->is_reference)
        {
            MVC_UnMarkFrameStoreRef (pMvcCtx, pMvcCtx->DPB.fs[i]);
        }
    }

    MVC_RemoveUnUsedFrameStore(pMvcCtx, -1);

    if (LAST_OUTPUT_OK != ret)
    {
        ret = GetVoLastImageID(&pMvcCtx->ImageQue);
    }

    return ret;
}

/*
************************************************************************
*   ����ԭ�� :  VOID MVC_SlidingWinMark(VOID)
*   �������� :  ���������
*   ����˵�� :  �� 
*   ����ֵ   �� ��
************************************************************************
*/
VOID MVC_SlidingWinMark(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    SINT32 poc = 0x7fffffff;
	SINT32 pos = 0;
    // if this is a reference pic with sliding sliding window, unmark first ref MVC_FRAME
    if (pMvcCtx->DPB.ref_frames_in_buffer == pMvcCtx->pCurrSPS->num_ref_frames - pMvcCtx->DPB.ltref_frames_in_buffer)
    {
    #if 0
//        for (i=0; i<pMvcCtx->DPB.used_size; i++)
        for (i=0; i<pMvcCtx->DPB.size; i++)
        {
            if (pMvcCtx->DPB.fs[i]!=NULL)
            {
                if (0 != pMvcCtx->DPB.fs[i]->is_reference && MVC_NonLongTermRefFlg(pMvcCtx->DPB.fs[i]) &&//MVC_ShortTermRefFlg(pMvcCtx->DPB.fs[i]) &&//?az
                    pMvcCtx->DPB.fs[i]->view_id == pMvcCtx->CurrPic.view_id) // short term reference
                {
                    MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs[i]);
                    MVC_UpdateReflist(pMvcCtx);
                    break;
                }
            }
        }
	#endif
        for (i=0; i<pMvcCtx->DPB.size; i++)		
        {
            if (pMvcCtx->DPB.fs[i] == NULL)
    		{
    		    continue;
    		}
            if (/*(poc>pMvcCtx->DPB.fs[i]->poc)*/(poc>pMvcCtx->DPB.fs[i]->dpb_cnt) && (0 != pMvcCtx->DPB.fs[i]->is_reference )
    			&& MVC_NonLongTermRefFlg(pMvcCtx->DPB.fs[i])//MVC_ShortTermRefFlg(pMvcCtx->DPB.fs[i]) &&//?az
                && (pMvcCtx->DPB.fs[i]->view_id == pMvcCtx->CurrPic.view_id))
            {
                //poc = pMvcCtx->DPB.fs[i]->poc;
                poc = pMvcCtx->DPB.fs[i]->dpb_cnt;				
                pos = i;
            }
        }	
        if (pMvcCtx->DPB.fs[pos]!=NULL)
        {
            if (0 != pMvcCtx->DPB.fs[pos]->is_reference && MVC_NonLongTermRefFlg(pMvcCtx->DPB.fs[pos]) &&//MVC_ShortTermRefFlg(pMvcCtx->DPB.fs[pos]) &&//?az
                pMvcCtx->DPB.fs[pos]->view_id == pMvcCtx->CurrPic.view_id) // short term reference
            {
                MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs[pos]);
                MVC_UpdateReflist(pMvcCtx);
            }
        }
	
    }   

    return;
}


VOID MVC_DumpDPB(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    UINT32 type;

    /* ��Ϣ���͹��� */
    type = 1 << PRN_DPB;
    if( 0 != (g_PrintEnable & type) ) /* ������ӡû�д� */
    {
        // if (20 == pMvcCtx->CurrSlice.frame_num)
        {
            for (i=0; i<pMvcCtx->DPB.used_size; i++)
            {            
                dprint(PRN_DPB, "pMvcCtx->DPB.fs[%d]: frame_num=%d poc=%d is_reference=%d\n", i, pMvcCtx->DPB.fs[i]->frame_num, pMvcCtx->DPB.fs[i]->poc, pMvcCtx->DPB.fs[i]->is_reference);
                //_LOG("fs_idc = %d;   pmv_idc=%d\n",g_DPB.fs[i]->fs_image.image_id,g_DPB.fs[i]->pmv_address_idc);
                //_LOG("fram_store = 0x%x\n",g_DPB.fs[i]);
            }            
            dprint(PRN_DPB, "\n");
        }
        
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            dprint(PRN_DPB, "pMvcCtx->DPB.fs_ref[%d]: frame_num=%d poc=%d is_reference=%d\n", i, pMvcCtx->DPB.fs_ref[i]->frame_num, pMvcCtx->DPB.fs_ref[i]->poc, pMvcCtx->DPB.fs_ref[i]->is_reference);
            dprint(PRN_DPB, "pMvcCtx->DPB.fs_ref[%d]: MVC_FRAME.poc=%d \n", i, pMvcCtx->DPB.fs_ref[i]->frame.poc);   
        }
        dprint(PRN_DPB, "\n");
    }
    
    return;
}


/*
************************************************************************
*      ����ԭ�� :  SINT32 MVC_AdaptiveMemMark(MVC_CTX_S *pMvcCtx)
*      �������� :  ���ݵ�ǰͼ���﷨Ԫ�ؽ���mmco����
*      ����˵�� :  �� 
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
SINT32 MVC_AdaptiveMemMark(MVC_CTX_S *pMvcCtx)
{
    SINT32 ret;
    UINT32 mmco_end_flag = 0;
    UINT32 i;
    MVC_MMCO_S *mmco_para = pMvcCtx->CurrPic.pMarkPara->MMCO;

    /* mmco operate*/
    for (i=0; i<MVC_MAX_MMCO_LEN; i++)
    {
        switch (mmco_para[i].memory_management_control_operation)
        {
        case 0:
            mmco_end_flag = 1;
            break;
        case 1:
            MVC_UnMarkSTRef(pMvcCtx, &pMvcCtx->CurrPic, mmco_para[i].difference_of_pic_nums_minus1);
            MVC_UpdateReflist(pMvcCtx);
            break;
        case 2:
            MVC_UnMarkLTRef(pMvcCtx, &pMvcCtx->CurrPic, mmco_para[i].long_term_pic_num);
            MVC_UpdateLTReflist(pMvcCtx);
            break;
        case 3:
            MVC_MarkSTToLTRef(pMvcCtx, &pMvcCtx->CurrPic, mmco_para[i].difference_of_pic_nums_minus1, mmco_para[i].long_term_frame_idx);
            MVC_UpdateReflist(pMvcCtx);
            MVC_UpdateLTReflist(pMvcCtx);
            break;
        case 4:
            MVC_UpdateMaxLTFrmIdx(pMvcCtx, mmco_para[i].max_long_term_frame_idx_plus1);
            MVC_UpdateLTReflist(pMvcCtx);
            break;          
        case 5:
            MVC_UnMarkAllSTRef(pMvcCtx);
            MVC_UpdateReflist(pMvcCtx);
            MVC_UpdateMaxLTFrmIdx (pMvcCtx, 0);
            MVC_UpdateLTReflist(pMvcCtx);
            pMvcCtx->CurrPOC.last_has_mmco_5 = 1;
            break;
        case 6:
            MVC_MarkCurrPicLT(pMvcCtx, &pMvcCtx->CurrPic, mmco_para[i].long_term_frame_idx);
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
    if (1 == pMvcCtx->CurrPOC.last_has_mmco_5)
    {
        // updata frame_num and pic_num of CurrPic
        pMvcCtx->CurrPic.frame_num = 0;
        pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] = 0;
        pMvcCtx->PrevPic.frame_num = 0;
        switch (pMvcCtx->CurrPic.structure)
        {
        case MVC_TOP_FIELD:
            pMvcCtx->CurrPic.thispoc = pMvcCtx->CurrPic.toppoc = 0;
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.toppoc = 0;
            break;
        case BOTTOM_FIELD:
            pMvcCtx->CurrPic.thispoc = pMvcCtx->CurrPic.bottompoc = 0;
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.bottompoc = 0;
            break;
        case MVC_FRAME:
            pMvcCtx->CurrPic.toppoc    -= pMvcCtx->CurrPic.thispoc;
            pMvcCtx->CurrPic.bottompoc -= pMvcCtx->CurrPic.thispoc;
            pMvcCtx->CurrPic.thispoc    = pMvcCtx->CurrPic.framepoc = MIN(pMvcCtx->CurrPic.toppoc, pMvcCtx->CurrPic.bottompoc);
            pMvcCtx->CurrPOC.toppoc    -= pMvcCtx->CurrPOC.ThisPOC;
            pMvcCtx->CurrPOC.bottompoc -= pMvcCtx->CurrPOC.ThisPOC;
            pMvcCtx->CurrPOC.ThisPOC    = pMvcCtx->CurrPOC.framepoc = MIN(pMvcCtx->CurrPOC.toppoc, pMvcCtx->CurrPOC.bottompoc);
            break;
        default:
            break;   
        }
        
        ret = MVC_FlushDPB(pMvcCtx,pMvcCtx->CurrPic.view_id);
        if (MVC_OK != ret)
        {
            return MVC_ERR;
        }        
    }

    return MVC_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_IDRMemMarking(MVC_CTX_S *pMvcCtx)
*      �������� :  ��ǰͼ��ΪIDRͼ�񣬸����﷨��DPB��ͼ����б��
*      ����˵�� :  �� 
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־ 
************************************************************************
*/
SINT32 MVC_IDRMemMarking(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    SINT32 ret;
    SINT32 k = 0;	
    // ���ж�֮ǰͼ��Ĵ�������У����IDRͼ���ǵڶ���ʱ����ô�����ͻ��
    if (pMvcCtx->CurrPic.pMarkPara->no_output_of_prior_pics_flag)
    {
        for (i=0; i<pMvcCtx->TotalPmvNum; i++)
        {
		    if (pMvcCtx->DPB.fs[i]!=NULL)
		    {
        		if(pMvcCtx->DPB.fs[i]->view_id == pMvcCtx->CurrPic.view_id)
        		{
                    pMvcCtx->PmvStoreUsedFlag[i] = 0; // �ر�ע�⣬��Ҫ�ƻ��˵�ǰͼ��Ĳ�λռ�ù�ϵ
        		}
		    }
        }
//		k = pMvcCtx->DPB.used_size;
		k = pMvcCtx->DPB.size;
		
        for (i=0; i<k; i++)
        {
            if (pMvcCtx->DPB.fs[i]!=NULL)
            {
                if(pMvcCtx->DPB.fs[i]->view_id == pMvcCtx->CurrPic.view_id)
                {
                // mark Ϊ������,����vo��ʾ�Ѿ�������vo buffer�е�ͼ��ʱ�����ͷ�ͼ���λ
                pMvcCtx->DPB.fs[i]->frame.is_long_term = pMvcCtx->DPB.fs[i]->frame.is_short_term = 0;
                pMvcCtx->DPB.fs[i]->top_field.is_long_term = pMvcCtx->DPB.fs[i]->top_field.is_short_term = 0;            
                pMvcCtx->DPB.fs[i]->bottom_field.is_long_term = pMvcCtx->DPB.fs[i]->bottom_field.is_short_term = 0;     
                pMvcCtx->DPB.fs[i]->is_reference = 0;
                FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->DPB.fs[i]->logic_fs_id, 0);
                //if (pMvcCtx->DPB.fs[i]->is_in_dpb || (!pMvcCtx->DPB.fs[i]->is_in_dpb && pMvcCtx->DPB.fs[i]->is_displayed)) 
                {
                    // ��ʱ�������:
                    // 1: ͼ��δ���g_DPB,���������,�ҿ��Էָ���������
                    // 2: ͼ�������vo,���Ѿ���ʾ,ֻ������Ϊ��
                    // 3: ͼ�������vo,����û��ʾ,��ͼ����ͷ���vo���
                    pMvcCtx->DPB.fs[i]->is_used = 0;
                    pMvcCtx->DPB.fs[i]->is_in_dpb = 0;
                    pMvcCtx->DPB.fs[i]->is_displayed = 1;
                    /* z56361, 2111223, ���֡�涼�ͷŵ��ˣ�����Ӧ��APCҲҪ�ͷŵ�  */
                    pMvcCtx->APC.is_used[pMvcCtx->DPB.fs[i]->apc_idc] = 0;
                    if (FSP_GetDisplay(pMvcCtx->ChanID, pMvcCtx->DPB.fs[i]->logic_fs_id) != FS_DISP_STATE_WAIT_DISP)
                    {
                        FSP_SetDisplay(pMvcCtx->ChanID, pMvcCtx->DPB.fs[i]->logic_fs_id, 0);
                    }
                }

                pMvcCtx->DPB.fs[i] = (MVC_FRAMESTORE_S *)NULL;
                pMvcCtx->DPB.fs_ref[i] = (MVC_FRAMESTORE_S *)NULL;
                pMvcCtx->DPB.fs_ltref[i] = (MVC_FRAMESTORE_S *)NULL;
    			pMvcCtx->DPB.used_size--;
    			}
            }
        }
        //pMvcCtx->DPB.used_size=0;

    }
    else
    {
        ret = MVC_FlushDPB(pMvcCtx, pMvcCtx->CurrPic.view_id);
        if (MVC_OK != ret)
        {
            return MVC_ERR;
        }
    }
    
    MVC_UpdateReflist(pMvcCtx);
    MVC_UpdateLTReflist(pMvcCtx);
    
    if (pMvcCtx->CurrPic.pMarkPara->long_term_reference_flag)
    {
        pMvcCtx->DPB.max_long_term_pic_idx = 1;
        pMvcCtx->CurrPic.long_term_frame_idx = 0;
        pMvcCtx->CurrPic.is_long_term = 1;
        pMvcCtx->CurrPic.is_short_term = 0;
    }
    else
    {
        pMvcCtx->DPB.max_long_term_pic_idx = 0; // �������·����ͬ���˴���ʵ������
        pMvcCtx->CurrPic.is_long_term  = 0;
        pMvcCtx->CurrPic.is_short_term = 1;
    }
    
    return MVC_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_Marking(MVC_CTX_S *pMvcCtx)
*      �������� :  ���ݵ�ǰͼ���mark������DPB��ͼ����б�ǣ�
*      ����˵�� :  �� 
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
SINT32 MVC_Marking(MVC_CTX_S *pMvcCtx)
{
    SINT32 ret;

    pMvcCtx->CurrPOC.last_has_mmco_5 = 0;
    pMvcCtx->CurrPOC.last_pic_bottom_field = (MVC_BOTTOM_FIELD == pMvcCtx->CurrPic.structure);

    /*IDR mark and adaptive mmco mark*/
    if ((MVC_NALU_TYPE_IDR == pMvcCtx->CurrPic.nal_unit_type)
		||((pMvcCtx->CurrPic.pic_type==0)
		     &&(((pMvcCtx->pstExtraData->stPvrInfo.u32BFrmRefFlag==1)
		     &&(pMvcCtx->pstExtraData->stPvrInfo.u32IDRFlag==0)
		     &&(pMvcCtx->pstExtraData->stPvrInfo.u32ContinuousFlag==0)
		     &&(!((pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed >= -1024)
		         &&(pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed <= 1024)))))))
    {
        ret = MVC_IDRMemMarking(pMvcCtx);
        if (MVC_OK != ret)
        {
            return MVC_ERR;
        }
    }
    else if (pMvcCtx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag && (0 != pMvcCtx->CurrPic.nal_ref_idc))
    {  
        // adaptive memory management
        ret = MVC_AdaptiveMemMark(pMvcCtx);
        pMvcCtx->CurrPic.pMarkPara->is_valid = 0;
        if (MVC_OK != ret)
        {
            return MVC_ERR;
        }
    }

    /*sliding win mark*/
    if ((MVC_NALU_TYPE_IDR != pMvcCtx->CurrPic.nal_unit_type) && (0 != pMvcCtx->CurrPic.nal_ref_idc) &&
        (!pMvcCtx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag) && (!pMvcCtx->CurrPic.field_pair_flag))
    {
        MVC_SlidingWinMark(pMvcCtx);
        pMvcCtx->CurrPic.pMarkPara->is_valid = 0;
        pMvcCtx->CurrPic.is_long_term = 0;
    }

    /*mark CurrPic*/
    if (0 != pMvcCtx->CurrPic.nal_ref_idc)
    {
        pMvcCtx->CurrPic.is_short_term = !pMvcCtx->CurrPic.is_long_term;
    } 

    return MVC_OK;
}

/** ����֡����Ϣ.
    ��һ��picture����DPBʱ�������֡����Ϣ���ɽ�ԭ�����зֲ��ڶദ����ͬ����ϲ����������������Ĵ���.
*/
VOID  MVC_UpdateCurrFrameInfo(MVC_CTX_S *pMvcCtx)
{
    MVC_CURRPIC_S *pCurrPic;
    MVC_FRAMESTORE_S *pCurrFrame;

    pCurrPic = &pMvcCtx->CurrPic;
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
    case MVC_FRAME:
        pCurrFrame->is_used             = 3;

        pCurrFrame->frame.structure     = MVC_FRAME;
        pCurrFrame->frame.frame_store   = pCurrFrame;
        pCurrFrame->frame.is_long_term  = pCurrPic->is_long_term;
        pCurrFrame->frame.is_short_term = pCurrPic->is_short_term;
        pCurrFrame->frame.poc           = pCurrPic->framepoc;
        pCurrFrame->frame.pic_type      = pCurrPic->pic_type;
        pCurrFrame->frame.err_level     = pCurrPic->err_level;

        pCurrFrame->top_field.poc       = pCurrPic->toppoc;
        pCurrFrame->bottom_field.poc    = pCurrPic->bottompoc;   

        pCurrFrame->fs_image            = pCurrPic->fs_image;

        MVC_SplitFrmToFlds(pCurrFrame);
        break;
        
    case MVC_TOP_FIELD:
        pCurrFrame->top_field.structure     = MVC_TOP_FIELD;
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

            MVC_CombineFldsToFrm(pCurrFrame);
        }        
        break;
        
    case BOTTOM_FIELD:
        pCurrFrame->bottom_field.structure     = MVC_TOP_FIELD;
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

            MVC_CombineFldsToFrm(pCurrFrame);
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
VOID  MVC_SimpleSlideDPB(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    SINT32 poc = 0x7fffffff;
	SINT32 pos = 0;
#if 0
    if(pMvcCtx->DPB.used_size >= pMvcCtx->DPB.size)
    {
        /* �ѵ�0��֡���ʶΪ���ο�������DPB��ɾ�����֡ */
        MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs[0]);
        MVC_RemoveFrameStoreOutDPB(pMvcCtx, 0);
    }
#endif
    if(pMvcCtx->DPB.used_size >= pMvcCtx->DPB.size)
    {
        for (i=0; i<pMvcCtx->DPB.size; i++)		
        {
            if (pMvcCtx->DPB.fs[i] == NULL)
    		{
    		    continue;
    		}
            if (/*(poc>pMvcCtx->DPB.fs[i]->poc)*/(poc>pMvcCtx->DPB.fs[i]->dpb_cnt)
                && (pMvcCtx->DPB.fs[i]->view_id == pMvcCtx->CurrPic.view_id))
            {
                //poc = pMvcCtx->DPB.fs[i]->poc;
                poc = pMvcCtx->DPB.fs[i]->dpb_cnt;				
                pos = i;
            }
        }	
    
        /* �ѵ�0��֡���ʶΪ���ο�������DPB��ɾ�����֡ */
        MVC_UnMarkFrameStoreRef(pMvcCtx,pMvcCtx->DPB.fs[pos]);
        MVC_RemoveFrameStoreOutDPB(pMvcCtx, pos);
    }

    pMvcCtx->CurrPic.is_long_term = 0;

    /*mark CurrPic*/
    if (0 != pMvcCtx->CurrPic.nal_ref_idc)
    {
        pMvcCtx->CurrPic.is_short_term = !pMvcCtx->CurrPic.is_long_term;
    }     
}

/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_StorePicInDpb(MVC_CTX_S *pMvcCtx,)
*      �������� :  ���ݵ�ǰͼ���mark������DPB��ͼ����б�ǣ�
*                  �ѵ�ǰͼ�����DPB
*      ����˵�� :  �� 
*      ����ֵ   �� DEC_NORMAL: ��ɹ�;
*                  DPB_ERR: �ڴ���DPBʱ����DPB�����ش������Ҫfirmware����
*                  OUT_FLAG: ��⵽�������Ҫ�˳������ظñ�־
************************************************************************
*/
#define MVC_STORE_PIC_RET(ret_val) \
do { \
    ret = ret_val;\
    dprint(PRN_DPB,"MVC_StorePicInDpb return(%d) from L%d\n",ret_val, __LINE__); \
    goto store_pic_exit;\
} while(0)

SINT32 MVC_StorePicInDpb(MVC_CTX_S *pMvcCtx)
{
    SINT32 ret = MVC_OK;
    UINT32 i;
    SINT32 min_poc;
    UINT32 pos;
    UINT32 pre_pos       = 0xffffffff;
    UINT32 pre_used_size = 0xffffffff;
    
    pMvcCtx->CurrPOC.last_has_mmco_5 = 0;
    pMvcCtx->CurrPOC.last_pic_bottom_field = (MVC_BOTTOM_FIELD == pMvcCtx->CurrPic.structure);
    
    //dprint(PRN_ALWS,"CurrPOC = %d, view_id = %d\n", pMvcCtx->CurrPic.thispoc, pMvcCtx->CurrPic.view_id);
    
    /*�����������ǰͼ��Ϊ�գ���Ҫ�˳�*/
    if (MVC_EMPTY == pMvcCtx->CurrPic.state)
    {
        //return MVC_OK;  //���Կ��Ǳ�һ�������ϲ㴦��
        dprint(PRN_FATAL,"CurrPic.state is 'MVC_EMPTY'\n");
        MVC_STORE_PIC_RET(MVC_ERR);
    }
        
    pMvcCtx->PrevStorePicCounter = pMvcCtx->PicCounter;

    if (pMvcCtx->CurrPic.is_idr_pic)
    {
       if (pMvcCtx->CurrPic.pMarkPara->no_output_of_prior_pics_flag)
       {
           ret = MVC_InitDPB(pMvcCtx);
           if (MVC_OK != ret)
           {
                dprint(PRN_FATAL,"Is IDR, but init DPB failed!\n");
                MVC_STORE_PIC_RET(MVC_ERR);
           }
       }
       else
       {
           ret = MVC_FlushDPB(pMvcCtx, -1);
           if (MVC_OK != ret)
           {
                dprint(PRN_FATAL,"Is IDR, but FlushDPB failed!\n");
                MVC_STORE_PIC_RET(MVC_ERR);
           }
       }
    }
      
    /******************** ����1: I֡ģʽ�½���MVC_DirectOutput�Լ����� ************************/
    if (2 == pMvcCtx->OldDecMode)
    {
        dprint(PRN_CTRL, "MVC_DirectOutput mode\n");
        ret = MVC_DirectOutput(pMvcCtx);
        
        //pmv��λ���ó�ȱʡֵ
        if (pMvcCtx->CurrPic.pmv_address_idc != pMvcCtx->TotalPmvNum)
        {
            pMvcCtx->PmvStoreUsedFlag[pMvcCtx->CurrPic.pmv_address_idc] = 0;          //pmv store 
            pMvcCtx->CurrPic.pmv_address_idc = pMvcCtx->TotalPmvNum;
        }

        //return ret;
        MVC_STORE_PIC_RET(ret);
    }

    /******************** ����2: ��������������ߵ�һ֡������������⴦�� ************************/
    /* ������������: ���CurrPic��֡ͼ��ֱ�����������ǳ�ͼ��ƴ���Ժ������*/   
	#if 1
    if ((pMvcCtx->pstExtraData->s32DecOrderOutput > NOT_DEC_ORDER || (MVC_FIRST_DISP_OUTPUT != pMvcCtx->FirstFrameDisplayed) ) 
          && (pMvcCtx->CurrPic.non_existing == 0))
    {
        MVC_FRAMESTORE_S *pFrameStore = pMvcCtx->CurrPic.frame_store;

        UINT32 one_fld_flg = 0;
        
        one_fld_flg = (MVC_TOP_FIELD==pMvcCtx->CurrPic.structure) || (BOTTOM_FIELD==pMvcCtx->CurrPic.structure);
            
        if ( (pMvcCtx->CurrPic.structure == MVC_FRAME) || 
             (pFrameStore->is_used > 0) ||
             one_fld_flg
           )
        {
            if ( one_fld_flg && (MVC_FIRST_DISP_INIT == pMvcCtx->FirstFrameDisplayed) )
            {
                pMvcCtx->FirstFrameDisplayed = MVC_FIRST_DISP_UNPAIRED;
                pMvcCtx->LastPic = pMvcCtx->CurrPic;
            }
            else if ( (one_fld_flg && (MVC_FIRST_DISP_UNPAIRED == pMvcCtx->FirstFrameDisplayed)) || 
                      (pMvcCtx->CurrPic.structure == MVC_FRAME) || 
                      (pFrameStore->is_used > 0))
            {
                MVC_CURRPIC_S *pOutPic;
                MVC_FRAMESTORE_S *pOutFrameStore;
                
                if (one_fld_flg && (MVC_FIRST_DISP_UNPAIRED == pMvcCtx->FirstFrameDisplayed))
                {
                    pOutPic = &pMvcCtx->LastPic;
                    pOutFrameStore = pMvcCtx->LastPic.frame_store;
                }
                else
                {
                    pOutPic = &pMvcCtx->CurrPic;
                    pOutFrameStore = pFrameStore;
                }

		#if 1  
           /* ��һ֡��������ᵼ�»��������ǵ� mvc��ʱ����Ҫ��һ֡���������
              ����ֻ��DECģʽ�²Ż�������������ڽ������simple dpbģʽʱ���治������ y00226912*/
           if (pMvcCtx->pstExtraData->s32DecOrderOutput > NOT_DEC_ORDER)
           {
               IMAGE *pImg = NULL;
                /* ����error�Ĵ����ں���������������������ʱ����һ��.
                    ������ĳ�MVC_UpdateCurrFrameInfo()�ķ�ʽ������ʱ����if(){}��Ҫɾ�� */
                if (pOutPic->structure != MVC_FRAME && pOutPic->field_pair_flag == 1)
                {
                    pOutFrameStore->is_used = 3;
                    pOutFrameStore->err_level += pOutPic->err_level;
                }
                else
                {
                    pOutFrameStore->err_level = pOutPic->err_level;
                }
                
                MVC_GetImagePara(pMvcCtx, pOutFrameStore);
                pImg = FSP_GetFsImagePtr(pMvcCtx->ChanID, pOutFrameStore->logic_fs_id);
                if (NULL == pImg)
                {
                    MVC_STORE_PIC_RET(MVC_ERR);
                }

                pImg->PTS = pOutPic->fs_image.PTS;
                pImg->Usertag = pOutPic->fs_image.Usertag;
                pImg->DispTime = pOutPic->fs_image.DispTime;

                pImg->error_level = pOutFrameStore->err_level;
                if (pImg->error_level > 0 && ((pImg->format&3)==0)) /* ���I֡�д����ϱ�һ���¼�֪ͨӦ�ò� */
                {
                    dprint(PRN_ERROR, "line %d, REPORT_IFRAME_ERR\n", __LINE__);
                    REPORT_IFRAME_ERR(pMvcCtx->ChanID);
                }
                if (pImg->error_level == 0 || pImg->error_level <= pMvcCtx->pstExtraData->s32OutErrThr)
                {
                    FSP_SetDisplay(pMvcCtx->ChanID, pOutFrameStore->logic_fs_id, 1);
                    if (VF_OK != InsertImgToVoQueue(pMvcCtx->ChanID, VFMW_MVC, pMvcCtx, &pMvcCtx->ImageQue, pImg))
                    {
                        FSP_SetDisplay(pMvcCtx->ChanID, pOutFrameStore->logic_fs_id, 0);
                    }
                    MVC_SetFrmRepeatCount(pMvcCtx, pImg);
                    pMvcCtx->PicOutCounter += 2;
                }
                else
                {
                    FSP_SetDisplay(pMvcCtx->ChanID, pOutFrameStore->logic_fs_id, 0);
                    MVC_STORE_PIC_RET(MVC_ERR);
                }
            }
            #endif
            
                if (MVC_FIRST_DISP_OUTPUT != pMvcCtx->FirstFrameDisplayed)
                {
                    /* ����ǿ������֡������֡�䱻��������ھ���������������ܴ�DPB��ɾ�����Ƚ����� */
                    pOutFrameStore->eFoState = MVC_FO_STATE_FAST_OUTPUT;
                    pFrameStore->as_first_out = 1;
                }
                pMvcCtx->FirstFrameDisplayed = MVC_FIRST_DISP_OUTPUT;
            }
        }
    }
	#endif

    /******************** ����3: Ϊ���ӶԽ����Ƶļ�DPB����Ŀ������������ʱ����I֡Ҳ�ܽ��� ************************/
    /* ���s32DecOrderOutput == DEC_ORDER_SIMPLE_DPB, ����·ԭDPB������ƣ����û�����ʽ����ο�֡.
       z56361. 20110803 */
    /* z56361. 20120302, ԭ������û�������Ƿ�ο�ȫ��slide DPB, ʵ���Ϸǲο�֡���ܲ��뻬�������򼷵��������Ĳο�֡.
       ����ϷSVC-T������������. ��������Ǽ���(0 != pMvcCtx->CurrPic.nal_ref_idc)���жϡ� */
    if ((pMvcCtx->pstExtraData->s32DecOrderOutput == DEC_ORDER_SIMPLE_DPB) && (0 != pMvcCtx->CurrPic.nal_ref_idc))
    {
        MVC_SimpleSlideDPB(pMvcCtx);

        for(i=0;i<pMvcCtx->DPB.size;i++)
        {
            if (pMvcCtx->DPB.fs[i]==NULL)
            {
                break;
            }
        }
        /*�ѵ�ǰͼ�����DPB*/
        ret = MVC_InsertFrmInDPB(pMvcCtx,i/*pMvcCtx->DPB.used_size*/, &pMvcCtx->CurrPic);
        if (MVC_OK != ret) 
        {
            //return MVC_ERR;
            dprint(PRN_FATAL,"line %d, return %d\n",__LINE__, ret);
            MVC_STORE_PIC_RET(MVC_ERR);
        }
        pMvcCtx->DPB.used_size++;
        
        MVC_UpdateReflist(pMvcCtx);
        MVC_UpdateLTReflist(pMvcCtx);
        
        MVC_STORE_PIC_RET(MVC_OK);
    }

    /******************** ��ʼ��׼��DPB�������� ************************/
    /* ����ǲο�֡��MVC_Marking����*/
    if (0 != pMvcCtx->CurrPic.nal_ref_idc)
    {
        //all pic dec down, add ref_err_thr; if this pic will display, the code return IDR_LOST
        if ((pMvcCtx->ref_error_thr < pMvcCtx->CurrPic.err_level) && (pMvcCtx->CurrPic.non_existing == 0))
        {
            dprint(PRN_ERROR, "err(%d) > ref_thr(%d)\n", pMvcCtx->CurrPic.err_level, pMvcCtx->ref_error_thr);
            REPORT_OVER_REFTHR(pMvcCtx->ChanID, pMvcCtx->CurrPic.err_level, pMvcCtx->ref_error_thr);

            MVC_ClearCurrPic(pMvcCtx);
            MVC_ClearDPB(pMvcCtx,-1);
            //return MVC_ERR;  //IDR_LOST
            MVC_STORE_PIC_RET(MVC_ERR);
        }

        ret = MVC_Marking(pMvcCtx);
        if (MVC_OK != ret)
        {
            //return MVC_ERR;
            dprint(PRN_ERROR,"MVC_Marking return %d\n", ret);
            MVC_STORE_PIC_RET(MVC_ERR);
        }
        
        /* z56361,2011-11-23, MVC_Marking֮��DPB�еĲο�֡�����ᷢ���ı䣬������Ҫ��ʱ����ref_frames_in_buffer��ltref_frames_in_buffer. ����
           ������������MVC_Marking֮ǰ��ֵ��������������������(���紥���Ժ��"DPB�������ж�")*/
        MVC_UpdateReflist(pMvcCtx);
        MVC_UpdateLTReflist(pMvcCtx);		

        /* z56361, 20111213, MVC_Marking֮����ܻ����'un-used'֡�棬��Щ֡��Ҫ�����DPB�������ȥ����Ϊ��ʱ�ܿ���fsp�Ѿ��������Ӧ��¼��
          ������Щ֡�����DPB���Ż����� */
        MVC_RemoveUnUsedFrameStore(pMvcCtx,pMvcCtx->CurrPic.view_id);
    }

    /*��ǰͼ������*/
    if (1 == pMvcCtx->CurrPic.field_pair_flag)
    {
        if (pMvcCtx->CurrPic.frame_store == pMvcCtx->pDirectOutBuf && (MVC_FRAMESTORE_S *)NULL != pMvcCtx->pDirectOutBuf)
        {
            // first field in MVC_DirectOutput buffer
            ret = MVC_DirectOutput(pMvcCtx);
            if (MVC_ERR == ret)
            {
                dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
            }
            //return ret;
            MVC_STORE_PIC_RET(ret);
        }

//        for (i=0; i<pMvcCtx->DPB.used_size; i++)
        for (i=0; i<pMvcCtx->DPB.size; i++)
        {
            if (pMvcCtx->DPB.fs[i]==NULL)
            {
                continue;
            }
            if (pMvcCtx->CurrPic.frame_store == pMvcCtx->DPB.fs[i])
            {
                // first field already in g_DPB
                ret = MVC_InsertFrmInDPB(pMvcCtx, i, &(pMvcCtx->CurrPic));
                if (MVC_ERR == ret) 
                {
                    //return MVC_ERR;
                    dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
                    MVC_STORE_PIC_RET(MVC_ERR);
                }
                MVC_UpdateReflist(pMvcCtx);
                MVC_UpdateLTReflist(pMvcCtx);
                //return MVC_OK;
                MVC_STORE_PIC_RET(MVC_OK);
            }
        }

        //����ɶԳ������������ܵ����˵��֮ǰ����ʲô���⣬Ӧ�ö�����ǰͼ�񣬺������
    }

    /*DPB�������ж�*/
    if ((0 != pMvcCtx->CurrPic.nal_ref_idc) &&   
        (pMvcCtx->DPB.size <= (pMvcCtx->DPB.ref_frames_in_buffer + pMvcCtx->DPB.ltref_frames_in_buffer)))
    {
        // ������������ݴ������簴��������markһ��
        dprint(PRN_ERROR, "line %d: pMvcCtx->DPB.size = %d, ref %d, ltref %d\n", __LINE__, pMvcCtx->DPB.size, 
            pMvcCtx->DPB.ref_frames_in_buffer, pMvcCtx->DPB.ltref_frames_in_buffer);
        MVC_ClearDPB(pMvcCtx,-1);
        //return MVC_ERR;  //DPB_ERR
        MVC_STORE_PIC_RET(MVC_ERR);
    }

    /*����ͼ�����*/
    while (pMvcCtx->DPB.used_size >= pMvcCtx->DPB.size)
    {
        if (MVC_GetMinPOC(pMvcCtx, -1, &min_poc, &pos) < 0) //������ӶԲ��ҵ���Сpos��λ�õĴ�������ж�
        {
            dprint(PRN_FATAL,"line %d, MVC_GetMinPOC failed\n", __LINE__);
            MVC_ClearDPB(pMvcCtx, -1);
            break;
        }
        
		//�ݴ���: ��used_size����ǰ���£����ظ���ȡͬһ��pos���ᵼ����ѭ������ǿ���˳��� y00226912 2014.04.19
        if (pMvcCtx->DPB.used_size == pre_used_size && pos == pre_pos)
        {
            dprint(PRN_ERROR,"%s: pos(%d) = pre_pos, force return.\n",__func__, pos);
            MVC_STORE_PIC_RET(MVC_ERR);
        }
        else
        {
            pre_pos = pos;
            pre_used_size = pMvcCtx->DPB.used_size;
        }
        
        if ((0==pMvcCtx->CurrPic.nal_ref_idc) && (pMvcCtx->CurrPic.thispoc<min_poc))
        {
            if ((2 == pMvcCtx->OldDecMode) && (0 != pMvcCtx->CurrPic.structure))
            {
                pMvcCtx->pLastFieldBuf = pMvcCtx->CurrPic.frame_store;
            }
            ret = MVC_DirectOutput(pMvcCtx);
            //return ret;
            if (MVC_ERR == ret)
            {
                dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
            }
            MVC_STORE_PIC_RET(ret);
        }
        ret = MVC_OutputFrmFromDPB(pMvcCtx, pos);
        if (MVC_ERR == ret)
        {
            //return MVC_ERR;
            dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
            MVC_STORE_PIC_RET(MVC_ERR);
        }
    }

    /*����DPBǰ�Ĵ����ж�*/
    if (0 != pMvcCtx->CurrPic.nal_ref_idc && 0 == pMvcCtx->CurrPic.is_long_term)
    {
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            if (pMvcCtx->DPB.fs_ref[i]->frame_num == pMvcCtx->CurrPic.frame_num &&
                pMvcCtx->DPB.fs_ref[i]->view_id == pMvcCtx->CurrPic.view_id)
            {
                pMvcCtx->DPB.fs_ref[i]->is_reference = 0;
                if (1 == pMvcCtx->DPB.fs_ref[i]->is_in_dpb)
                {
                    pMvcCtx->DPB.fs_ref[i]->is_displayed = 1;
                    pMvcCtx->DPB.fs_ref[i]->is_in_dpb = 0;
                }

                //pmv��λ���ó�ȱʡֵ
                if (pMvcCtx->DPB.fs_ref[i]->pmv_address_idc != pMvcCtx->TotalPmvNum)
                {
                    pMvcCtx->PmvStoreUsedFlag[pMvcCtx->DPB.fs_ref[i]->pmv_address_idc] = 0;          //pmv store 
                    pMvcCtx->DPB.fs_ref[i]->pmv_address_idc = pMvcCtx->TotalPmvNum;
                }
            }
        }
    }
    
    for(i=0;i<pMvcCtx->DPB.size;i++)
    {
        if (pMvcCtx->DPB.fs[i]==NULL)
        {
            break;
        }
    }
    /*�ѵ�ǰͼ�����DPB*/
    ret = MVC_InsertFrmInDPB(pMvcCtx,i/*pMvcCtx->DPB.used_size*/, &pMvcCtx->CurrPic);
    if (MVC_OK != ret) 
    {
        //return MVC_ERR;
        dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
        MVC_STORE_PIC_RET(MVC_ERR);
    }
    pMvcCtx->DPB.used_size++;

    MVC_UpdateReflist(pMvcCtx);
    MVC_UpdateLTReflist(pMvcCtx);

store_pic_exit:

    if (ret == MVC_ERR)
    {
        FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
        dprint(PRN_ERROR,"line %d, return %d\n",__LINE__, ret);
        /* z56361, 20111223, ��ȻStoreʧ�ܣ���֡��֡��Ӧ���ͷŵ��������������֡��й© */
        pMvcCtx->CurrPic.frame_store->is_in_dpb = 0;
        pMvcCtx->CurrPic.frame_store->is_used   = 0;
    }
    pMvcCtx->CurrPic.state = MVC_EMPTY;

    return ret;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID MVC_ReleaseNAL(SINT32 ChanID, MVC_NALU_S *pNalu)
*   �������� :  �ͷ�һ��nal
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
VOID MVC_ReleaseNAL(SINT32 ChanID, MVC_NALU_S *pNalu)
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
*SINT32 MVC_ArrangeVahbMem(MVC_CTX_S *pMvcCtx, UINT32 ImgWidth, UINT32 ImgHeight, UINT32 ImgNum, UINT32 PmvBlkNum)
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
SINT32 MVC_ArrangeVahbMem(MVC_CTX_S *pMvcCtx, UINT32 ImgWidth, UINT32 ImgHeight, UINT32 *pImgNum, UINT32 PmvBlkNum)
{
    FSP_INST_CFG_S FspInstCfg;

    memset(&FspInstCfg, 0, sizeof(FSP_INST_CFG_S));
    FspInstCfg.s32DecFsWidth        = ImgWidth;
    FspInstCfg.s32DecFsHeight       = ImgHeight;
    FspInstCfg.s32DispFsWidth       = ImgWidth;
    FspInstCfg.s32DispFsHeight      = ImgHeight;
    FspInstCfg.s32ExpectedDecFsNum  = (pMvcCtx->pstExtraData->eCapLevel == CAP_LEVEL_SINGLE_IFRAME_FHD)? 0: (pMvcCtx->DPB.size+3);
    FspInstCfg.s32ExpectedDispFsNum = 5;
    FspInstCfg.s32ExpectedPmvNum    = (pMvcCtx->pstExtraData->eCapLevel == CAP_LEVEL_SINGLE_IFRAME_FHD)? 1: PmvBlkNum;
	
    /* ����FSPʵ�������ָ�֡�� */
    if (FSP_OK == FSP_ConfigInstance(pMvcCtx->ChanID, &FspInstCfg))
    {
        SINT32 Ret, UsedMemSize;
        Ret = FSP_PartitionFsMemory(pMvcCtx->ChanID, pMvcCtx->pstExtraData->s32SyntaxMemAddr, 
              pMvcCtx->pstExtraData->s32SyntaxMemSize, &UsedMemSize);
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
* ����ԭ�� : SINT32 MVC_RepairList(MVC_CTX_S *pMvcCtx,)
* �������� :  �޲�list,ֻ����ȥ��list�е�NULL
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_RepairList(MVC_CTX_S *pMvcCtx)
{
    UINT8  found;
    UINT32 i, i_list, i_list_cnt;
    MVC_STORABLEPIC_S *pRepairPic = (MVC_STORABLEPIC_S *)NULL;

    switch (pMvcCtx->CurrSlice.slice_type)
    {
    case MVC_P_SLICE:
        i_list_cnt = 1;
        break;
    case MVC_B_SLICE:
        i_list_cnt = 2;
        break;
    case MVC_I_SLICE:
        i_list_cnt = 0;
        break;
    default:
        MVC_ClearCurrSlice(pMvcCtx);
        return MVC_ERR;  //NULL_LIST
    }

    for (i_list=0; i_list<i_list_cnt; i_list++)
    {
        if (0 < pMvcCtx->CurrSlice.listXsize[i_list])
        {
            found = 0;
            for (i = 0; i < pMvcCtx->CurrSlice.listXsize[i_list]; i++)
            {
                //!! the condition must add
                if ((MVC_STORABLEPIC_S *)NULL != pMvcCtx->pListX[i_list][i])
                {
                    /*
                    if (g_VfmwCfg.ref_error_thr < g_pListX[i_list][i]->err_level)
                    {
                        //Ϊ�˺õ�Ч�����裬���������֮������idr�޹�
                        continue;
                    }
                    */
                    found = 1;
                    pRepairPic = pMvcCtx->pListX[i_list][i];
                    break;
                }
            }

            if ((0==found) || (NULL==pRepairPic))
            {
                if (pMvcCtx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB)
                {
                    //!!! ����listԪ��Ϊ�գ����԰�list��Ϊ����Ӳ��ȥ�ݴ�
                    //!!! Ҳ���԰�slice������
                    //!!! ǰ��Ϊʹ��p bslice��ȫi���ʱ�ɽ⣬����Ӳ���ݴ�
                    //!!! ���߶�Ӳ����Ϊ��ȫ������Ҫ�޲��ܶ� slice
                    //ASSERT(0);
                    MVC_ClearCurrSlice(pMvcCtx);
                    return MVC_ERR;  //NULL_LIST
                }
                else
                {
                    /* ���������DEC_ORDER_SIMPLE_DPB, ���õ�ǰpic�������list�ڵ� */
                    pRepairPic = &pMvcCtx->CurrPic.frame_store->frame;
                }
            }

            for (i = 0; i < pMvcCtx->CurrSlice.listXsize[i_list]; i++)
            {
                //!! the condition must add
                if ((MVC_STORABLEPIC_S *)NULL == pMvcCtx->pListX[i_list][i])
                {
                    pMvcCtx->pListX[i_list][i] = pRepairPic;
                }
            }
        }
        else
        {
            if (pMvcCtx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB)
            {
                MVC_ClearCurrSlice(pMvcCtx);
                return MVC_ERR;  //NULL_LIST
            }
            else
            {
                /* ���������DEC_ORDER_SIMPLE_DPB, ���õ�ǰpic�������list�ڵ� */
                pMvcCtx->CurrSlice.listXsize[i_list] = 1;
                pMvcCtx->pListX[i_list][0] = &pMvcCtx->CurrPic.frame_store->frame;
            }
            
        }
    }

    return MVC_OK;
}


/*!
************************************************************************
* ����ԭ�� :  MVC_STORABLEPIC_S*  MVC_GetShortTermPicPoint(MVC_CTX_S *pMvcCtx,,SINT32 picNum)
* �������� :  ����picnum�ҵ���Ӧ���ڲο�pic��ָ��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
MVC_STORABLEPIC_S *MVC_GetShortTermPicPoint(MVC_CTX_S *pMvcCtx,SINT32 picNum)
{
    UINT32 i;
    SINT32 structure = pMvcCtx->CurrSlice.field_pic_flag ? pMvcCtx->CurrSlice.bottom_field_flag ? MVC_BOTTOM_FIELD : MVC_TOP_FIELD : MVC_FRAME;

    for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
    {
        if (MVC_FRAME == structure)
        {
            if (3 == pMvcCtx->DPB.fs_ref[i]->is_reference) // MVC_FRAME, field pair
                if ((1 == pMvcCtx->DPB.fs_ref[i]->frame.is_short_term) && (0 == pMvcCtx->DPB.fs_ref[i]->frame.is_long_term) && (pMvcCtx->DPB.fs_ref[i]->frame.pic_num == picNum))
                { return &pMvcCtx->DPB.fs_ref[i]->frame; }
        }
        else
        {
            if (pMvcCtx->DPB.fs_ref[i]->is_reference & 1) // top field
                if ((1==pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term) && (0==pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term) && (pMvcCtx->DPB.fs_ref[i]->top_field.pic_num==picNum))
                { return &pMvcCtx->DPB.fs_ref[i]->top_field; }
                
                if (pMvcCtx->DPB.fs_ref[i]->is_reference & 2) // bottom field
                    if ((1==pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term) && (0==pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term) && (pMvcCtx->DPB.fs_ref[i]->bottom_field.pic_num==picNum))
                { return &pMvcCtx->DPB.fs_ref[i]->bottom_field; }
        }
    }

    return NULL;
}


/*!
************************************************************************
* ����ԭ�� :  MVC_STORABLEPIC_S*  MVC_GetLongTermPicPoint(MVC_CTX_S *pMvcCtx,SINT32 LongtermPicNum)
* �������� :  ����LongtermPicNum�ҵ���Ӧ���ڲο�pic��ָ��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
MVC_STORABLEPIC_S *MVC_GetLongTermPicPoint(MVC_CTX_S *pMvcCtx,SINT32 LongtermPicNum)
{
    UINT32 i;
    SINT32 structure = pMvcCtx->CurrSlice.field_pic_flag ? pMvcCtx->CurrSlice.bottom_field_flag ? MVC_BOTTOM_FIELD : MVC_TOP_FIELD : MVC_FRAME;
    
    for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
    {
        if (MVC_FRAME==structure)
        {
            if (3==pMvcCtx->DPB.fs_ltref[i]->is_reference)//MVC_FRAME, field pair
                if ((0==pMvcCtx->DPB.fs_ltref[i]->frame.is_short_term)&& (1==pMvcCtx->DPB.fs_ltref[i]->frame.is_long_term) && (pMvcCtx->DPB.fs_ltref[i]->frame.long_term_pic_num==LongtermPicNum))
                { return &pMvcCtx->DPB.fs_ltref[i]->frame; }
        }
        else
        {
            if (pMvcCtx->DPB.fs_ltref[i]->is_reference & 1)//top field
                if ((0==pMvcCtx->DPB.fs_ltref[i]->top_field.is_short_term) && (1==pMvcCtx->DPB.fs_ltref[i]->top_field.is_long_term) && (pMvcCtx->DPB.fs_ltref[i]->top_field.long_term_pic_num==LongtermPicNum))
                { return &pMvcCtx->DPB.fs_ltref[i]->top_field; }
                
                if (pMvcCtx->DPB.fs_ltref[i]->is_reference & 2)//bottom field
                    if ((0==pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_short_term) && (1==pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_long_term) && (pMvcCtx->DPB.fs_ltref[i]->bottom_field.long_term_pic_num==LongtermPicNum))
                { return &pMvcCtx->DPB.fs_ltref[i]->bottom_field; }
        }
    }
    
    return NULL;
}


/*!
************************************************************************
* ����ԭ�� : VOID MVC_ReorderSTList(MVC_STORABLEPIC_S **RefPicListX, 
SINT32 num_ref_idx_lX_active_minus1, SINT32 picNumLX, SINT32 *refIdxLX)
* �������� :  ���Ŷ��ڲο��б�
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
VOID MVC_ReorderSTList(MVC_CTX_S *pMvcCtx, UINT32 listidx, SINT32 num_ref_idx_lX_active_minus1, 
                                    SINT32 picNumLX, SINT32 *refIdxLX, SINT32 CurrViewId)
{
    SINT32 cIdx, nIdx;
    MVC_STORABLEPIC_S *picLX;
    MVC_STORABLEPIC_S **RefPicListX = pMvcCtx->pListX[listidx];

    picLX = MVC_GetShortTermPicPoint(pMvcCtx, picNumLX);
    
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
            if ((0!=RefPicListX[cIdx]->is_long_term) || (RefPicListX[cIdx]->pic_num!=picNumLX) || (RefPicListX[cIdx]->frame_store->view_id != CurrViewId))
            {
                RefPicListX[nIdx++] = RefPicListX[cIdx];
            }
        }
    }
    
    return;
}


/*!
************************************************************************
* ����ԭ�� : VOID MVC_ReorderLTList(MVC_STORABLEPIC_S **RefPicListX, 
SINT32 num_ref_idx_lX_active_minus1, SINT32 LongTermPicNum, SINT32 *refIdxLX)
* �������� :  ���ų����ڲο��б�
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
VOID MVC_ReorderLTList(MVC_CTX_S *pMvcCtx, UINT32 listidx, SINT32 num_ref_idx_lX_active_minus1, 
                                    SINT32 LongTermPicNum, SINT32 *refIdxLX, SINT32 CurrViewId)
{
    SINT32 cIdx, nIdx;
    MVC_STORABLEPIC_S *picLX;
    MVC_STORABLEPIC_S **RefPicListX = pMvcCtx->pListX[listidx];

    picLX = MVC_GetLongTermPicPoint(pMvcCtx, LongTermPicNum);

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
            if ((0==RefPicListX[cIdx]->is_long_term) || (RefPicListX[cIdx]->long_term_pic_num!=LongTermPicNum) || 
                (RefPicListX[cIdx]->frame_store->view_id != CurrViewId))
            {
                RefPicListX[nIdx++] = RefPicListX[cIdx];
            }
        }
    }

    return;
}


/*!
************************************************************************
* ����ԭ�� :  SINT32 MVC_GetMaxViewIdx (MVC_CTX_S *pMvcCtx, SINT32 CurrViewId, UINT32 anchor_pic_flag, int listidx)
* �������� :  ��õ�ǰview_id��Ӧ���ο�֡��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
SINT32 MVC_GetMaxViewIdx (MVC_CTX_S *pMvcCtx, SINT32 CurrViewId, UINT32 anchor_pic_flag, int listidx)
{
  SINT32 VOIdx;
  SINT32 maxViewIdx = 0;

  VOIdx = MVC_GetVOIdx(pMvcCtx->CurrSUBSPS.view_id, pMvcCtx->CurrSUBSPS.num_views_minus1+1, CurrViewId);
  if(VOIdx >= 0)
  {
    if(anchor_pic_flag)
        { maxViewIdx = pMvcCtx->CurrSUBSPS.num_anchor_refs[listidx][VOIdx]; }
    else
        { maxViewIdx = pMvcCtx->CurrSUBSPS.num_non_anchor_refs[listidx][VOIdx]; }
  }

  return maxViewIdx;	
}
/* End */
/*!
************************************************************************
* ����ԭ�� :  MVC_STORABLEPIC_S*  mvc_get_inter_view_pic(MVC_CTX_S *pMvcCtx, SINT32 targetViewID, UINT32 currPOC, UINT32 listidx)
* �������� :  ���targetViewID��Ӧ��pic��ַ
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
MVC_STORABLEPIC_S*  mvc_get_inter_view_pic(MVC_CTX_S *pMvcCtx, SINT32 targetViewID, SINT32 currPOC, UINT32 listidx)
{
    UINT32 i;
    UINT32 interviewlistsize;
    MVC_FRAMESTORE_S *fs_interviewlist;

    fs_interviewlist = pMvcCtx->InterviewListX[listidx];
    interviewlistsize = pMvcCtx->CurrSlice.interviewlistXsize[listidx];

    for(i=0; i<interviewlistsize; i++)
    {
        if (fs_interviewlist[i].view_id == targetViewID)
        {
            if(pMvcCtx->CurrPic.structure==MVC_FRAME && fs_interviewlist[i].frame.MVC_DecPOC == currPOC)
            {
                return &fs_interviewlist[i].frame;
            }
            else if(pMvcCtx->CurrPic.structure==MVC_TOP_FIELD && fs_interviewlist[i].top_field.MVC_DecPOC == currPOC)
            {
                return &fs_interviewlist[i].top_field;
            }
            else if(pMvcCtx->CurrPic.structure==MVC_BOTTOM_FIELD && fs_interviewlist[i].bottom_field.MVC_DecPOC == currPOC)
            {
                return &fs_interviewlist[i].bottom_field;
            }
        }
    }
    return NULL;
}
/* End */
/*!
************************************************************************
* ����ԭ�� :  void mvc_reorder_interview(MVC_CTX_S *pMvcCtx, UINT32 num_ref_idx_lX_active_minus1, UINT32 *refIdxLX, UINT32 targetViewID, UINT32 currpoc, UINT32 listidx)
* �������� :  �ο��б�����
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
void mvc_reorder_interview(MVC_CTX_S *pMvcCtx, UINT32 num_ref_idx_lX_active_minus1, UINT32 *refIdxLX, SINT32 targetViewID, SINT32 currpoc, UINT32 listidx)
{
    UINT32 cIdx, nIdx;
    MVC_STORABLEPIC_S *picLX;
    MVC_STORABLEPIC_S **list = pMvcCtx->pListX[listidx];

    picLX = mvc_get_inter_view_pic(pMvcCtx, targetViewID, currpoc, listidx);

    if (picLX)
    {
        for( cIdx = num_ref_idx_lX_active_minus1+1; cIdx > *refIdxLX; cIdx-- )
        {
            list[ cIdx ] = list[ cIdx - 1];
        }

        list[ (*refIdxLX)++ ] = picLX;

        nIdx = *refIdxLX;

        for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1+1; cIdx++ )
        {
            if((NULL == list[cIdx]) || (list[cIdx]->frame_store->view_id != targetViewID) || (list[cIdx]->MVC_DecPOC != currpoc))
            {
                list[ nIdx++ ] = list[ cIdx ];
            }
        }
    }

    return;
}
/* End */
/*!
************************************************************************
* ����ԭ�� : SINT32 MVC_ReorderRefPiclist(MVC_CTX_S *pMvcCtx,MVC_STORABLEPIC_S **list, SINT32 *list_size, SINT32 num_ref_idx_lX_active_minus1, 
SINT32 *reordering_of_pic_nums_idc, SINT32 *abs_diff_pic_num_minus1, SINT32 *long_term_pic_idx)
* �������� :  �ο��б����ź���
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
VOID MVC_ReorderRefPiclist(MVC_CTX_S *pMvcCtx, UINT32 listidx, UINT32 num_ref_idx_lX_active_minus1)
{
    UINT32 i;
    SINT32 maxPicNum, currPicNum, picNumLXNoWrap, picNumLXPred, picNumLX;
    SINT32 refIdxLX = 0;
    SINT32 structure = pMvcCtx->CurrSlice.field_pic_flag ? pMvcCtx->CurrSlice.bottom_field_flag ? MVC_BOTTOM_FIELD : MVC_TOP_FIELD : MVC_FRAME;
    SINT32 MaxFrameNum;
    SINT32 CurrVOIdx = 0;
    SINT32 picViewIdxLXPred=0, picViewIdxLX, targetViewId;
    SINT32 MaxViewIdx = 0;
    UINT32 init_flag = 1;
	#if 0
                pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1, 
                pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0, 
                pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l0, 
                pMvcCtx->CurrReorder.long_term_pic_idx_l0;
	#endif
    UINT32 *reordering_of_pic_nums_idc;
    UINT32 *abs_diff_pic_num_minus1;
    UINT32 *long_term_pic_idx;
    UINT32 *abs_diff_view_idx_minus1;

    if (listidx !=0 && listidx != 1)
    {
        return;
    }

    if (0 == listidx)
    {
        reordering_of_pic_nums_idc = pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0;
        abs_diff_pic_num_minus1 = pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l0;
        long_term_pic_idx = pMvcCtx->CurrReorder.long_term_pic_idx_l0;
        abs_diff_view_idx_minus1 = pMvcCtx->CurrReorder.abs_diff_view_idx_minus1_l0;
    }
    else
    {
        reordering_of_pic_nums_idc = pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1;
        abs_diff_pic_num_minus1 = pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l1;
        long_term_pic_idx = pMvcCtx->CurrReorder.long_term_pic_idx_l1;
        abs_diff_view_idx_minus1 = pMvcCtx->CurrReorder.abs_diff_view_idx_minus1_l1;
    }
    
    MaxFrameNum = 1 << (pMvcCtx->pCurrSPS->log2_max_frame_num_minus4 + 4); 
    
    if (MVC_FRAME==structure)
    {
        maxPicNum  = MaxFrameNum;
        currPicNum = pMvcCtx->CurrSlice.frame_num;
    }
    else
    {
        maxPicNum  = 2 * MaxFrameNum;
        currPicNum = 2 * pMvcCtx->CurrSlice.frame_num + 1;
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

            MVC_ReorderSTList(pMvcCtx, listidx, num_ref_idx_lX_active_minus1, picNumLX, &refIdxLX, pMvcCtx->CurrSlice.view_id);
        }
        else if (2==reordering_of_pic_nums_idc[i])
        {
            MVC_ReorderLTList(pMvcCtx, listidx, num_ref_idx_lX_active_minus1, long_term_pic_idx[i], &refIdxLX, pMvcCtx->CurrSlice.view_id);
        }
        else
        {
            if(1==init_flag)
            {
                init_flag = 0;
                CurrVOIdx = MVC_GetVOIdx(pMvcCtx->CurrSUBSPS.view_id, pMvcCtx->CurrSUBSPS.num_views_minus1+1, pMvcCtx->CurrSlice.view_id);
                MaxViewIdx = MVC_GetMaxViewIdx(pMvcCtx, pMvcCtx->CurrSlice.view_id, pMvcCtx->CurrSlice.anchor_pic_flag, listidx);	
                picViewIdxLXPred=-1;
            }
			
            if(reordering_of_pic_nums_idc[i] == 4) //(remapping_of_pic_nums_idc[i] == 4)
            {
                picViewIdxLX = picViewIdxLXPred - (abs_diff_view_idx_minus1[i] + 1);
                if( picViewIdxLX <0)
                {
                    picViewIdxLX += MaxViewIdx;
                }
            }
            else //(remapping_of_pic_nums_idc[i] == 5)
            {
                picViewIdxLX = picViewIdxLXPred + (abs_diff_view_idx_minus1[i] + 1);
                if( picViewIdxLX >= MaxViewIdx)
                {
                    picViewIdxLX -= MaxViewIdx;
                }
            }
            picViewIdxLXPred = picViewIdxLX;
			
            if (pMvcCtx->CurrSlice.anchor_pic_flag)
            {
                targetViewId = pMvcCtx->CurrSUBSPS.anchor_ref[listidx][CurrVOIdx][picViewIdxLX];
            }
            else
            {
                targetViewId = pMvcCtx->CurrSUBSPS.non_anchor_ref[listidx][CurrVOIdx][picViewIdxLX];
            }

            mvc_reorder_interview(pMvcCtx, num_ref_idx_lX_active_minus1, &refIdxLX, targetViewId, pMvcCtx->CurrPic.thispoc, listidx);
        }
    }

    return;
}


/*!
************************************************************************
* ����ԭ�� :  SINT32 MVC_ReorderListX(MVC_CTX_S *pMvcCtx)
* �������� :  �ο��б�����
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
VOID MVC_ReorderListX(MVC_CTX_S *pMvcCtx)
{
    if (MVC_I_SLICE != pMvcCtx->CurrSlice.slice_type)
    {
        if (pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l0)//?az
        {
            MVC_ReorderRefPiclist(pMvcCtx, 0, pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1);  
        }
        
        // that's a definition
        pMvcCtx->CurrSlice.listXsize[0] = pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1+1;
    }
    if (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        if (pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l1)
        {
            MVC_ReorderRefPiclist(pMvcCtx, 1, pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1);
        }
        
        // that's a definition
        pMvcCtx->CurrSlice.listXsize[1] = pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1+1;
    }

    return;  
}


/*!
************************************************************************
* ����ԭ�� : SINT32 MVC_IsSTRefFlg(MVC_STORABLEPIC_S *s)
* �������� :  �ж�pic�Ƿ��Ƕ��ڲο�
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
UINT32 MVC_IsSTRefFlg(MVC_STORABLEPIC_S *s)
{
    return ((1==s->is_short_term) && (0==s->is_long_term));
}


/*!
************************************************************************
* ����ԭ�� : SINT32 MVC_IsLTRefFlg(MVC_STORABLEPIC_S *s)
* �������� :  �ж�pic�Ƿ��ǳ��ڲο�
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
UINT32 MVC_IsLTRefFlg(MVC_STORABLEPIC_S *s)
{
    return ((0==s->is_short_term) && (1==s->is_long_term));
}


/*!
************************************************************************
* ����ԭ�� : SINT32 MVC_compare_pic_by_pic_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�storablepicture�ṹ���е�Ԫ��pic_num�Ĵ�С��
*              ps:��һ��ȽϺ����Ƚϵ�Ԫ�ض�ӦЭ���й涨��ֵ����ЩֵҲ����JM�д�ŵĽṹ��
*              ��һ��.Ϊ�˱�֤��ȷ�ԣ����Լ������д��λ���ϵ�Ԫ���Ƿ���ȷ
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_pic_by_pic_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_STORABLEPIC_S**)arg1)->pic_num < (*(MVC_STORABLEPIC_S**)arg2)->pic_num)
    {
        return 1;
    }

    if ((*(MVC_STORABLEPIC_S**)arg1)->pic_num > (*(MVC_STORABLEPIC_S**)arg2)->pic_num)
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
* ����ԭ�� : SINT32 MVC_compare_pic_by_lt_pic_num_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�storablepicture�ṹ���е�Ԫ��pic_num�Ĵ�С��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_pic_by_lt_pic_num_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_STORABLEPIC_S**)arg1)->long_term_pic_num < (*(MVC_STORABLEPIC_S**)arg2)->long_term_pic_num)
    {
        return -1;
    }
    
    if ((*(MVC_STORABLEPIC_S**)arg1)->long_term_pic_num > (*(MVC_STORABLEPIC_S**)arg2)->long_term_pic_num)
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
* ����ԭ�� : SINT32 MVC_compare_fs_by_frame_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�framestore�ṹ���е�Ԫ��frame_num_wrap�Ĵ�С��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_fs_by_frame_num_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_FRAMESTORE_S**)arg1)->frame_num_wrap < (*(MVC_FRAMESTORE_S**)arg2)->frame_num_wrap)
    {
        return 1;
    }
    
    if ((*(MVC_FRAMESTORE_S**)arg1)->frame_num_wrap > (*(MVC_FRAMESTORE_S**)arg2)->frame_num_wrap)
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
* ����ԭ�� : SINT32 MVC_compare_fs_by_lt_pic_idx_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�framestore�ṹ���е�Ԫ��long_term_frame_idx�Ĵ�С��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_fs_by_lt_pic_idx_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_FRAMESTORE_S**)arg1)->long_term_frame_idx < (*(MVC_FRAMESTORE_S**)arg2)->long_term_frame_idx)
    {  
        return -1;
    }

    if ((*(MVC_FRAMESTORE_S**)arg1)->long_term_frame_idx > (*(MVC_FRAMESTORE_S**)arg2)->long_term_frame_idx)
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
* ����ԭ�� : SINT32 MVC_compare_pic_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�MVC_STORABLEPIC_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_pic_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_STORABLEPIC_S**)arg1)->poc < (*(MVC_STORABLEPIC_S**)arg2)->poc)
    {
        return -1;
    }
    
    if ((*(MVC_STORABLEPIC_S**)arg1)->poc > (*(MVC_STORABLEPIC_S**)arg2)->poc)
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
* ����ԭ�� : SINT32 MVC_compare_pic_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�MVC_STORABLEPIC_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_pic_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_STORABLEPIC_S**)arg1)->poc < (*(MVC_STORABLEPIC_S**)arg2)->poc)
    {
        return 1;        
    }
    
    if ((*(MVC_STORABLEPIC_S**)arg1)->poc > (*(MVC_STORABLEPIC_S**)arg2)->poc)
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
* ����ԭ�� : SINT32 MVC_compare_fs_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� : ������Ƚ�MVC_FRAMESTORE_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_fs_by_poc_asc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_FRAMESTORE_S**)arg1)->poc < (*(MVC_FRAMESTORE_S**)arg2)->poc)
    {
        return -1;
    }
    
    if ((*(MVC_FRAMESTORE_S**)arg1)->poc > (*(MVC_FRAMESTORE_S**)arg2)->poc)
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
* ����ԭ�� : SINT32 MVC_compare_fs_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
* �������� :  ������Ƚ�MVC_FRAMESTORE_S�ṹ���е�Ԫ��poc�Ĵ�С��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_compare_fs_by_poc_desc(CONSTVOID *arg1, CONSTVOID *arg2)
{
    if ((*(MVC_FRAMESTORE_S**)arg1)->poc < (*(MVC_FRAMESTORE_S**)arg2)->poc)
    {
        return 1;
    }
    
    if ((*(MVC_FRAMESTORE_S**)arg1)->poc > (*(MVC_FRAMESTORE_S**)arg2)->poc)
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
* ����ԭ�� : SINT32 MVC_GenPiclistfromFrmlist(SINT32 currStrcture, MVC_FRAMESTORE_S **fs_list, 
*            SINT32 list_idx, MVC_STORABLEPIC_S **list, SINT32 *list_size, SINT32 long_term)
* �������� :  ����MVC_FRAME�Ĳο�list���ɳ��Ĳο�list
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_GenPiclistfromFrmlist(UINT32 currStrcture, MVC_FRAMESTORE_S **fs_list, 
                             UINT32 list_idx, MVC_STORABLEPIC_S **list, UINT32 *list_size, SINT32 long_term)
{
    UINT32 top_idx = 0;
    UINT32 bot_idx = 0;
    UINT32 (*is_ref)(MVC_STORABLEPIC_S *s);

    if (list_idx>=32)
    {
        list_idx = 31; // for error resilence
    }

    /*top*/
    if (long_term)
    {
        is_ref = MVC_IsLTRefFlg;
    }
    else
    {
        is_ref = MVC_IsSTRefFlg;
    }

    if (MVC_TOP_FIELD==currStrcture)
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
* ����ԭ�� : SINT32  MVC_GetBaseViewId(MVC_CTX_S *pMvcCtx)
* �������� : ��õ�ǰ����base view��viewID��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
SINT32  MVC_GetBaseViewId(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    SINT32 baseviewid=-1;
	
    for(i=0; i<32; i++)
    {
        if(pMvcCtx->SUBSPS[i].num_views_minus1>0 && pMvcCtx->SUBSPS[i].is_valid)
        {
            baseviewid = pMvcCtx->SUBSPS[i].view_id[0];
            break;
        }
    }

    return baseviewid;
}
/* End */
/*!
************************************************************************
* ����ԭ�� : SINT32 MVC_GetVOIdx(SINT32 *ref_view_id, UINT32 num_views, SINT32 CurrViewId)
* �������� : ��õ�ǰview_id��Ӧ��index��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
SINT32 MVC_GetVOIdx(SINT32 *ref_view_id, UINT32 num_views, SINT32 CurrViewId)
{
    SINT32 i;
    
    for(i=0;i<num_views;i++)
    {
        if(CurrViewId==ref_view_id[i])
        {
            break;
        }
    }
    return i;
}
/* End */
/*!
************************************************************************
* ����ԭ�� : SINT32  MVC_is_view_id_in_ref_view_list(SINT32 view_id, SINT32 *ref_view_id, UINT32 num_ref_views)
* �������� : ��ѯ��ǰview_id�Ƿ��ڲο�view_id������
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
SINT32  MVC_is_view_id_in_ref_view_list(SINT32 view_id, SINT32 *ref_view_id, UINT32 num_ref_views)
{
    SINT32 i;
    for(i=0; i<num_ref_views; i++)
    {
        if(view_id == ref_view_id[i])
        {
            break;
        }		
    }
    return (num_ref_views && (i<num_ref_views));
}
/* End */

/*!
************************************************************************
* ����ԭ�� : VOID MVC_GenPiclistfromFrmlist_Interview(UINT32 currStrcture, MVC_FRAMESTORE_S **fs_list, 
                                                                             UINT32 list_idx, MVC_STORABLEPIC_S **list, UINT32 *list_size)
* �������� : ��interview�ο�֡��ֳ�����
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
VOID MVC_GenPiclistfromFrmlist_Interview(UINT32 currStrcture, MVC_FRAMESTORE_S *fs_list, 
    UINT32 list_idx, MVC_STORABLEPIC_S **list, UINT32 *list_size)
{
    UINT32 i;

    if (currStrcture == MVC_TOP_FIELD)
    {
        for (i=0; i<list_idx; i++)
        {
            list[(*list_size)] = &fs_list[i].top_field;
            (*list_size)++;
        }
    }
    if (currStrcture == BOTTOM_FIELD)
    {
        for (i=0; i<list_idx; i++)
        {
            list[(*list_size)] = &fs_list[i].bottom_field;
            (*list_size)++;
        }
    }
}
/* End */
/*!
************************************************************************
* ����ԭ�� : void mvc_append_interview_list(MVC_CTX_S *pMvcCtx, UINT32 listidx, 	MVC_FRAMESTORE_S **list, UINT32 *listXsize)
* �������� : ��DPB�л��interview�ο�����
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
/* MVC */
void mvc_append_interview_list(MVC_CTX_S *pMvcCtx, UINT32 listidx, MVC_FRAMESTORE_S *list, UINT32 *listXsize)
{
    SINT32 i;
    UINT32 iVOIdx = MVC_GetVOIdx(pMvcCtx->CurrSUBSPS.view_id,pMvcCtx->CurrSUBSPS.num_views_minus1+1,pMvcCtx->CurrSlice.view_id);
    UINT32 pic_avail;
    SINT32 poc = 0;
    UINT32 fld_idx;
    UINT32 num_ref_views, *ref_view_id;

    if(pMvcCtx->CurrSlice.anchor_pic_flag)
    {
        num_ref_views = pMvcCtx->CurrSUBSPS.num_anchor_refs[listidx][iVOIdx];
        ref_view_id = pMvcCtx->CurrSUBSPS.anchor_ref[listidx][iVOIdx];
    }
    else
    {
        num_ref_views = pMvcCtx->CurrSUBSPS.num_non_anchor_refs[listidx][iVOIdx];
        ref_view_id = pMvcCtx->CurrSUBSPS.non_anchor_ref[listidx][iVOIdx];
    }

    fld_idx = (pMvcCtx->CurrPic.structure== BOTTOM_FIELD) ? 1 : 0;
    //for(i=pMvcCtx->DPB.used_size-1; i>=0; i--)
    for(i=pMvcCtx->DPB.size-1; i>=0; i--)
    {
        if (pMvcCtx->DPB.fs[i]==NULL)
        {
            continue;
        }
        if(pMvcCtx->CurrPic.structure==MVC_FRAME)
        {
            pic_avail = (pMvcCtx->DPB.fs[i]->is_used == 3);
            if (pic_avail)
            {
                 poc = pMvcCtx->DPB.fs[i]->frame.MVC_DecPOC;
            }
        }
        else if(pMvcCtx->CurrPic.structure==MVC_TOP_FIELD)
        {
            pic_avail = pMvcCtx->DPB.fs[i]->is_used & 1;
            if(pMvcCtx->CurrPic.field_pair_flag)
            {
                pic_avail &=(pMvcCtx->DPB.fs[i]->bottom_field.MVC_DecPOC==pMvcCtx->CurrPic.frame_store->bottom_field.MVC_DecPOC);
            }
            if (pic_avail)
            {
                poc = pMvcCtx->DPB.fs[i]->top_field.MVC_DecPOC;
            }
        }
        else if(pMvcCtx->CurrPic.structure==BOTTOM_FIELD)
        {
            pic_avail = (pMvcCtx->DPB.fs[i]->is_used & 2)>>1;
            if(pMvcCtx->CurrPic.field_pair_flag)
            {
                pic_avail &=(pMvcCtx->DPB.fs[i]->top_field.MVC_DecPOC==pMvcCtx->CurrPic.frame_store->top_field.MVC_DecPOC);
            }
            if (pic_avail)
            {
                poc = pMvcCtx->DPB.fs[i]->bottom_field.MVC_DecPOC;
            }
        }
        else
        {
            pic_avail =0;
        }

        if(pic_avail && pMvcCtx->DPB.fs[i]->inter_view_flag[fld_idx])
        {
            if(poc == pMvcCtx->CurrPic.thispoc)
            {
                if(MVC_is_view_id_in_ref_view_list(pMvcCtx->DPB.fs[i]->view_id, ref_view_id, num_ref_views))
                {
                    memcpy(&list[*listXsize],pMvcCtx->DPB.fs[i],sizeof(MVC_FRAMESTORE_S));
                    //pstLFs = FSP_GetLogicFs(pMvcCtx->ChanID,list[*listXsize].logic_fs_id);
                    list[*listXsize].frame.frame_store = 
                    list[*listXsize].top_field.frame_store = 
                    list[*listXsize].bottom_field.frame_store = &list[*listXsize];
                    list[*listXsize].pmv_address_idc = pMvcCtx->TotalPmvNum-1;
                    
                    //add one inter-view reference;
                    //list[*listXsize] = pMvcCtx->DPB.fs[i];
                   //next;
                   if(*listXsize == num_ref_views)
                   	{
                   	    break;
                   	}
                   (*listXsize)++;
                }
           }
        }
    }
}
/* End */

/*!
************************************************************************
* ����ԭ�� : SINT32  MVC_InitListX(MVC_CTX_S *pMvcCtx)
* �������� : �ο��б��ʼ��
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_InitListX(MVC_CTX_S *pMvcCtx)
{
    MVC_STORABLEPIC_S *tmp_s;
    UINT32 i, j;
    UINT32 list0idx   = 0;
    UINT32 list0idx_1 = 0;
    UINT32 listltidx  = 0; 
    MVC_FRAMESTORE_S *fs_list0[16];
    MVC_FRAMESTORE_S *fs_list1[16];
    MVC_FRAMESTORE_S *fs_listlt[16];
    pMvcCtx->CurrSlice.interviewlistXsize[0] = pMvcCtx->CurrSlice.interviewlistXsize[1] = 0;

    if (MVC_P_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        if (MVC_FRAME == pMvcCtx->CurrPic.structure)  
        {
            for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
            {
                if ((3==pMvcCtx->DPB.fs_ref[i]->is_reference) && (0==pMvcCtx->DPB.fs_ref[i]->frame.is_long_term)
                    && (1==pMvcCtx->DPB.fs_ref[i]->frame.is_short_term)) // MVC_FRAME, field pair
                {
                    pMvcCtx->pListX[0][list0idx++] = &pMvcCtx->DPB.fs_ref[i]->frame;
                }
            }
            // order list 0 by PicNum : short term
            qsort((VOID *)pMvcCtx->pListX[0], list0idx, sizeof(MVC_STORABLEPIC_S*), MVC_compare_pic_by_pic_num_desc);
            pMvcCtx->CurrSlice.listXsize[0] = list0idx;
            
            // long term handling
            for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
            {
                if (3==pMvcCtx->DPB.fs_ltref[i]->is_reference && 1==pMvcCtx->DPB.fs_ltref[i]->frame.is_long_term && 0==pMvcCtx->DPB.fs_ltref[i]->frame.is_short_term) // MVC_FRAME, field pair
                {
                    pMvcCtx->pListX[0][list0idx++] = &pMvcCtx->DPB.fs_ltref[i]->frame;
                }
            }
            qsort((VOID *)&pMvcCtx->pListX[0][pMvcCtx->CurrSlice.listXsize[0]], list0idx - pMvcCtx->CurrSlice.listXsize[0], sizeof(MVC_STORABLEPIC_S*), MVC_compare_pic_by_lt_pic_num_asc);
            pMvcCtx->CurrSlice.listXsize[0] = list0idx;
        }
        else
        {
            for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
            {
                if (0 != pMvcCtx->DPB.fs_ref[i]->is_reference)
                {
                    fs_list0[list0idx++] = pMvcCtx->DPB.fs_ref[i];
                }
            }
            qsort((VOID *)fs_list0, list0idx, sizeof(MVC_FRAMESTORE_S*), MVC_compare_fs_by_frame_num_desc);

            pMvcCtx->CurrSlice.listXsize[0] = 0;
            MVC_GenPiclistfromFrmlist(pMvcCtx->CurrPic.structure, fs_list0, list0idx, pMvcCtx->pListX[0], &(pMvcCtx->CurrSlice.listXsize[0]), 0);

            // long term handling
            for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
            {
                fs_listlt[listltidx++]=pMvcCtx->DPB.fs_ltref[i];
            }

            qsort((VOID *)fs_listlt, listltidx, sizeof(MVC_FRAMESTORE_S*), MVC_compare_fs_by_lt_pic_idx_asc);      
            MVC_GenPiclistfromFrmlist(pMvcCtx->CurrPic.structure, fs_listlt, listltidx, pMvcCtx->pListX[0], &(pMvcCtx->CurrSlice.listXsize[0]), 1);
        }
        pMvcCtx->CurrSlice.listXsize[1] = 0;
        #if 1
		if(0!=pMvcCtx->CurrSlice.svc_extension_flag)
		{
            if (0 == pMvcCtx->CurrSlice.listXsize[0])
            {
                /* list�ǿյ�: ���������DEC_ORDER_SIMPLE_DPBģʽ�����õ�ǰ֡��Ϊ�ο�ͼǿ�ƽ���;
                               ���򷵻�list����. z56361. 20110803 */
                if (pMvcCtx->pstExtraData->s32DecOrderOutput == DEC_ORDER_SIMPLE_DPB)
                {
                    if (pMvcCtx->CurrPic.structure == MVC_FRAME)
                    {
                        pMvcCtx->pListX[0][0] = &pMvcCtx->CurrPic.frame_store->frame;
                    }
                    else if (pMvcCtx->CurrPic.structure == MVC_TOP_FIELD)
                    {
                        pMvcCtx->pListX[0][0] = &pMvcCtx->CurrPic.frame_store->top_field;
                    }
                    else
                    {
                        pMvcCtx->pListX[0][0] = &pMvcCtx->CurrPic.frame_store->bottom_field;
                    }
                    pMvcCtx->CurrSlice.listXsize[0] = 1;
                }
                else
                {
                    dprint(PRN_ERROR, "for P slice size of list equal 0(1st).\n");
                    MVC_ClearCurrSlice(pMvcCtx);
                    return MVC_ERR;  //INIT_LIST_ERR
                }
            }
		}
		#endif
    }
    else  // B-Slice
    {
        if (MVC_FRAME == pMvcCtx->CurrPic.structure)  
        {
            for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
            {
                if (3 == pMvcCtx->DPB.fs_ref[i]->is_reference)//MVC_FRAME, field pair
                {
                    if (pMvcCtx->CurrPic.framepoc >= pMvcCtx->DPB.fs_ref[i]->frame.poc)
                    {
                        pMvcCtx->pListX[0][list0idx++] = &pMvcCtx->DPB.fs_ref[i]->frame;
                    }
                }
            }
            qsort((VOID *)pMvcCtx->pListX[0], list0idx, sizeof(MVC_STORABLEPIC_S*), MVC_compare_pic_by_poc_desc);
            list0idx_1 = list0idx;
            for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
            {
                if (3==pMvcCtx->DPB.fs_ref[i]->is_reference)  //MVC_FRAME, field pair
                {
                    if (pMvcCtx->CurrPic.framepoc < pMvcCtx->DPB.fs_ref[i]->frame.poc)  //����ж���ʵ���Բ�Ҫ��
                    {
                        pMvcCtx->pListX[0][list0idx++] = &pMvcCtx->DPB.fs_ref[i]->frame;
                    }
                }
            }
            qsort((VOID *)&pMvcCtx->pListX[0][list0idx_1], list0idx-list0idx_1, sizeof(MVC_STORABLEPIC_S*), MVC_compare_pic_by_poc_asc);
            // ��Ϊ�����Ѿ��Ź����ˣ��������ﲻ��������ֻ�õ�һ����Ϳ�����
            for (j=0; j<list0idx_1; j++)
            {
                pMvcCtx->pListX[1][(list0idx -list0idx_1)+j]=pMvcCtx->pListX[0][j];
            }
            for (j=list0idx_1; j<list0idx; j++)
            {
                pMvcCtx->pListX[1][j-list0idx_1]=pMvcCtx->pListX[0][j];
            }
            
            pMvcCtx->CurrSlice.listXsize[0] = pMvcCtx->CurrSlice.listXsize[1] = list0idx;            
            
            // long term handling
            for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
            {
                if (3 == pMvcCtx->DPB.fs_ltref[i]->is_used)//MVC_FRAME, field pair
                {
                    if (1 == pMvcCtx->DPB.fs_ltref[i]->frame.is_long_term)
                    {
                        pMvcCtx->pListX[0][list0idx]   = &pMvcCtx->DPB.fs_ltref[i]->frame;
                        pMvcCtx->pListX[1][list0idx++] = &pMvcCtx->DPB.fs_ltref[i]->frame;
                    }
                }
            }
            qsort((VOID *)&pMvcCtx->pListX[0][pMvcCtx->CurrSlice.listXsize[0]], list0idx - pMvcCtx->CurrSlice.listXsize[0], sizeof(MVC_STORABLEPIC_S*), MVC_compare_pic_by_lt_pic_num_asc);
            qsort((VOID *)&pMvcCtx->pListX[1][pMvcCtx->CurrSlice.listXsize[0]], list0idx - pMvcCtx->CurrSlice.listXsize[0], sizeof(MVC_STORABLEPIC_S*), MVC_compare_pic_by_lt_pic_num_asc);
            pMvcCtx->CurrSlice.listXsize[0] = pMvcCtx->CurrSlice.listXsize[1] = list0idx;
        }
        else
        {   
            for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
            {
                if (0 != pMvcCtx->DPB.fs_ref[i]->is_used)
                {
                    if (pMvcCtx->CurrPic.thispoc >= pMvcCtx->DPB.fs_ref[i]->poc)
                    {
                        fs_list0[list0idx++] = pMvcCtx->DPB.fs_ref[i];
                    }
                }
            }
            qsort((VOID *)fs_list0, list0idx, sizeof(MVC_FRAMESTORE_S*), MVC_compare_fs_by_poc_desc);
            list0idx_1 = list0idx;
            for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
            {
                if (0 != pMvcCtx->DPB.fs_ref[i]->is_used)
                {
                    if (pMvcCtx->CurrPic.thispoc < pMvcCtx->DPB.fs_ref[i]->poc)
                    {
                        fs_list0[list0idx++] = pMvcCtx->DPB.fs_ref[i];
                    }
                }
            }
            qsort((VOID *)&fs_list0[list0idx_1], list0idx-list0idx_1, sizeof(MVC_FRAMESTORE_S*), MVC_compare_fs_by_poc_asc);
            
            for (j=0; j<list0idx_1; j++)
            {
                fs_list1[(list0idx -list0idx_1)+j] = fs_list0[j];
            }
            for (j=list0idx_1; j<list0idx; j++)
            {
                fs_list1[j-list0idx_1] = fs_list0[j];
            }

            pMvcCtx->CurrSlice.listXsize[0] = 0;
            pMvcCtx->CurrSlice.listXsize[1] = 0;
            MVC_GenPiclistfromFrmlist(pMvcCtx->CurrPic.structure, fs_list0, list0idx, pMvcCtx->pListX[0], &(pMvcCtx->CurrSlice.listXsize[0]), 0);
            MVC_GenPiclistfromFrmlist(pMvcCtx->CurrPic.structure, fs_list1, list0idx, pMvcCtx->pListX[1], &(pMvcCtx->CurrSlice.listXsize[1]), 0);
            
            // long term handling
            for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
            {
                fs_listlt[listltidx++] = pMvcCtx->DPB.fs_ltref[i];
            }

            qsort((VOID *)fs_listlt, listltidx, sizeof(MVC_FRAMESTORE_S*), MVC_compare_fs_by_lt_pic_idx_asc);

            MVC_GenPiclistfromFrmlist(pMvcCtx->CurrPic.structure, fs_listlt, listltidx, pMvcCtx->pListX[0], &(pMvcCtx->CurrSlice.listXsize[0]), 1);
            MVC_GenPiclistfromFrmlist(pMvcCtx->CurrPic.structure, fs_listlt, listltidx, pMvcCtx->pListX[1], &(pMvcCtx->CurrSlice.listXsize[1]), 1);
        }

        if ((pMvcCtx->CurrSlice.listXsize[0] == pMvcCtx->CurrSlice.listXsize[1]) && (pMvcCtx->CurrSlice.listXsize[0]>1))
        {
            // check if lists are identical, if yes swap first two elements of g_pListX[1]
            SINT32 diff=0;
            for (j=0; j<(UINT32)(pMvcCtx->CurrSlice.listXsize[0]); j++)
            {
                if (pMvcCtx->pListX[0][j] != pMvcCtx->pListX[1][j])
                {
                    diff = 1;
                    break;
                }
            }
            if (!diff)
            {
                tmp_s = pMvcCtx->pListX[1][0];
                pMvcCtx->pListX[1][0] = pMvcCtx->pListX[1][1];
                pMvcCtx->pListX[1][1] = tmp_s;
            }
        }
    }

    if(0==pMvcCtx->CurrSlice.svc_extension_flag)
    {
        mvc_append_interview_list(pMvcCtx, 0, pMvcCtx->InterviewListX[0], &pMvcCtx->CurrSlice.interviewlistXsize[0]);
        if (pMvcCtx->CurrPic.structure == MVC_FRAME)
        {	
            list0idx = pMvcCtx->CurrSlice.listXsize[0];
            for (i=0; i<pMvcCtx->CurrSlice.interviewlistXsize[0]; i++)
           {
               pMvcCtx->pListX[0][list0idx++]=&pMvcCtx->InterviewListX[0][i].frame;
           }
           pMvcCtx->CurrSlice.listXsize[0] = list0idx;
        }
        else
        {
            MVC_GenPiclistfromFrmlist_Interview(pMvcCtx->CurrPic.structure, pMvcCtx->InterviewListX[0],pMvcCtx->CurrSlice.interviewlistXsize[0], pMvcCtx->pListX[0], &pMvcCtx->CurrSlice.listXsize[0]);
        }

        if(MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type)
        {
            mvc_append_interview_list(pMvcCtx, 1, pMvcCtx->InterviewListX[1], &pMvcCtx->CurrSlice.interviewlistXsize[1]);
            if (pMvcCtx->CurrPic.structure == MVC_FRAME)
            {	
                list0idx = pMvcCtx->CurrSlice.listXsize[1];
                for (i=0; i<pMvcCtx->CurrSlice.interviewlistXsize[1]; i++)
               {
                   pMvcCtx->pListX[1][list0idx++]=&pMvcCtx->InterviewListX[1][i].frame;
               }
               pMvcCtx->CurrSlice.listXsize[1] = list0idx;
            }
            else
            {
                MVC_GenPiclistfromFrmlist_Interview(pMvcCtx->CurrPic.structure, pMvcCtx->InterviewListX[1],pMvcCtx->CurrSlice.interviewlistXsize[1], pMvcCtx->pListX[1], &pMvcCtx->CurrSlice.listXsize[1]);
            }
        }
    }
	
    if(MVC_P_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        if (0 == pMvcCtx->CurrSlice.listXsize[0])
        {
            dprint(PRN_ERROR, "for P slice size of list equal 0(2nd).\n");
            MVC_ClearCurrSlice(pMvcCtx);
            return MVC_ERR;  //INIT_LIST_ERR
        }
    }
    else
    {
        if ((0 == pMvcCtx->CurrSlice.listXsize[0]) && (0 == pMvcCtx->CurrSlice.listXsize[1]))
        {
            dprint(PRN_ERROR, "for B slice size of two list all equal 0.\n");
            MVC_ClearCurrSlice(pMvcCtx);
            return MVC_ERR;  //INIT_LIST_ERR
        }
    }

    // set max size
    pMvcCtx->CurrSlice.listXsize[0] = MIN (pMvcCtx->CurrSlice.listXsize[0], pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1+1);
    pMvcCtx->CurrSlice.listXsize[1] = MIN (pMvcCtx->CurrSlice.listXsize[1], pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1+1);

    // set the unused list entries to NULL
    for (i = pMvcCtx->CurrSlice.listXsize[0]; i < (MVC_MAX_LIST_SIZE); i++)
    {
        pMvcCtx->pListX[0][i] = NULL;
    }
    for (i = pMvcCtx->CurrSlice.listXsize[1]; i<(MVC_MAX_LIST_SIZE); i++)
    {
        pMvcCtx->pListX[1][i] = NULL;
    }

    return MVC_OK;
}


/***********************************************************************************
* Function:    MVC_DecList(MVC_CTX_S *pMvcCtx)
* Description: ���ɵ�ǰslice�Ĳο��б�
* Input:       pMvcCtx->CurrSlice   ��ǰSlice����
*              g_pListX[2][MVC_MAX_LIST_SIZE]   List�б�
* Output:      g_pListX[2][MVC_MAX_LIST_SIZE]   List�б�
* Return:      DEC_NORMAL   ������ȷ����List�б�
*              DEC_ERR      ��������ȷ����List�б�
* Others:      ��
***********************************************************************************/ 
VOID MVC_DumpList(MVC_CTX_S *pMvcCtx)
{
    UINT32 i, j; // just for write log
    UINT32 type;
    
    /* ��Ϣ���͹��� */
    type = 1 << PRN_REF;
    if (0 != (g_PrintEnable & type)) /* ������ӡû�д� */
    {
        for (j=0; j<2; j++)
        {
            for (i=0; i<pMvcCtx->CurrSlice.listXsize[j]; i++)
            {
                dprint(PRN_REF, "list[%d][%d]: frame_num=%d, poc=%d\n", j, i,
                    pMvcCtx->pListX[j][i]->frame_store->frame_num, pMvcCtx->pListX[j][i]->frame_store->poc);
            }
        }
        #if 0
        if (pMvcCtx->CurrSlice.listXsize[0]>0)
        {
            dprint(PRN_REF, "L0: ");
            for (i=0; i < pMvcCtx->CurrSlice.listXsize[0]; i++)
            {
                switch (pMvcCtx->pListX[0][i]->structure)
                {
                case MVC_FRAME:
                    dprint(PRN_REF, "%d ", pMvcCtx->pListX[0][i]->frame_store->poc);
                    break;
                case MVC_TOP_FIELD:
                    dprint(PRN_REF, "%dt ", pMvcCtx->pListX[0][i]->frame_store->poc);
                    break;
                case BOTTOM_FIELD:
                    dprint(PRN_REF, "%db ", pMvcCtx->pListX[0][i]->frame_store->poc);
                    break;
                default:
                    break;
                }
            }
            dprint(PRN_REF, "\n");
        }
        
        if (pMvcCtx->CurrSlice.listXsize[1]>0)
        {
            dprint(PRN_REF, "L1:");
            for (i=0; i < pMvcCtx->CurrSlice.listXsize[1]; i++)
            {
                switch (pMvcCtx->pListX[1][i]->structure)
                {
                case MVC_FRAME:
                    dprint(PRN_REF, "%d ", pMvcCtx->pListX[1][i]->frame_store->poc);
                    break;
                case MVC_TOP_FIELD:
                    dprint(PRN_REF, "%dt ", pMvcCtx->pListX[1][i]->frame_store->poc);
                    break;
                case BOTTOM_FIELD:
                    dprint(PRN_REF, "%db ", pMvcCtx->pListX[1][i]->frame_store->poc);
                    break;
                default:
                    break;                        
                }
            }
            dprint(PRN_REF, "\n");
        }
        #endif
    }    
}


/*!
************************************************************************
*   ����ԭ�� :  SINT32 MVC_FindNearestPOCPicId(MVC_CTX_S *pMvcCtx)
*   �������� :  ��dpb����poc�����ͼ������islice�޲�
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_FindNearestPOCPicId(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    SINT32 nearest_pic_id;
    SINT32 curr_poc, nearest_poc, nearest_poc_diff, new_poc_diff;
	
    /*Ӧ�Զ��view�����ʱ��fs[0]���п���ΪNULL�ģ�����ж�������used_size�Ϳ�����*/
    if ((pMvcCtx->DPB.used_size < 1))//||(pMvcCtx->DPB.fs[0]==NULL))
    {
        return -1;  // return  a err pic id
    }

    curr_poc = pMvcCtx->CurrPic.thispoc;
	#if 0
    nearest_pic_id = 0;   
    nearest_poc = pMvcCtx->DPB.fs[0]->poc;
    nearest_poc_diff = ABS(curr_poc - nearest_poc);
	#endif
    nearest_pic_id = 0;   
    nearest_poc = 0x7fffffff;
    nearest_poc_diff = 0x7fffffff;

//    for (i=1; i<pMvcCtx->DPB.used_size; i++)  
    for (i=0; i<pMvcCtx->DPB.size; i++)  
    {       
        //if (g_DPB.fs[i]->err_level < 3) //------------------------------�������ܻ�򿪴˴�
        if (pMvcCtx->DPB.fs[i]!=NULL)
        {
            new_poc_diff = ABS(curr_poc - pMvcCtx->DPB.fs[i]->poc);
            if (new_poc_diff < nearest_poc_diff)
            {
                nearest_pic_id =  pMvcCtx->DPB.fs[i]->fs_image.image_id;
                nearest_poc = pMvcCtx->DPB.fs[i]->poc;
                nearest_poc_diff = new_poc_diff;
            }
        }
    }

    return nearest_pic_id;
}


/***********************************************************************************
* Function:    MVC_FindMinRefIdx(MVC_CTX_S *pMvcCtx)
* Description: �ҵ�ÿһ��apc��list0�е���Сrefidx
* Input:       ��
*              
* Output:  ��(�ı�g_APC�е�ֵ)
* Return:  ��
*              
* Others:      ��
***********************************************************************************/ 
VOID MVC_FindMinRefIdx(MVC_CTX_S *pMvcCtx)
{
    UINT32 i, j;
    UINT32 minrefidx; 

    if (MVC_I_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        return;
    }

    if (0 == pMvcCtx->CurrPic.structure) // MVC_FRAME reference
    {
        for (i = 0; i < 16; i++) 
        {
            minrefidx = 32;
            for (j = 0; j < pMvcCtx->CurrSlice.listXsize[0]; j++)
            {
                if (i == pMvcCtx->pListX[0][j]->frame_store->apc_idc)
                {
                    //find same apc, compare refidx
                    minrefidx = MIN(j, minrefidx);
                    pMvcCtx->APC.RefIdx[2*i]  = minrefidx;
                    pMvcCtx->APC.RefIdx[2*i + 1]  = minrefidx;
                }                     
            }

            if (32 <= minrefidx)
            {
                pMvcCtx->APC.RefIdx[2*i] = 0;
                pMvcCtx->APC.RefIdx[2*i + 1] = 0;                    
            }
        }    
    }
    else // field reference
    {            
        for (i = 0; i < 32; i++)
        {
            minrefidx = 32;
            for (j = 0; j < pMvcCtx->CurrSlice.listXsize[0]; j++)
            {
                if (i == ((pMvcCtx->pListX[0][j]->frame_store->apc_idc << 1) | (2 == pMvcCtx->pListX[0][j]->structure)))
                {
                    //find same apc, compare refidx
                    minrefidx = MIN(j, minrefidx);
                    pMvcCtx->APC.RefIdx[i]  = minrefidx;                        
                }
            }

            if (32 <= minrefidx)
            {
                pMvcCtx->APC.RefIdx[i] = 0;
            }
        }     
    }

    return;
}


/***********************************************************************************
* Function:    MVC_DecList(MVC_CTX_S *pMvcCtx)
* Description: ���ɵ�ǰslice�Ĳο��б�
* Input:       pMvcCtx->CurrSlice   ��ǰSlice����
*              g_pListX[2][MVC_MAX_LIST_SIZE]   List�б�
* Output:      g_pListX[2][MVC_MAX_LIST_SIZE]   List�б�
* Return:      DEC_NORMAL   ������ȷ����List�б�
*              DEC_ERR      ��������ȷ����List�б�
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_DecList(MVC_CTX_S *pMvcCtx)
{    
    SINT32 ret;

    pMvcCtx->CurrSlice.picid_nearpoc = MVC_FindNearestPOCPicId(pMvcCtx);
    pMvcCtx->CurrSlice.picid_refidx0 = -1;  // default pic id

    if (MVC_I_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        pMvcCtx->CurrSlice.listXsize[0] = 0;
        pMvcCtx->CurrSlice.listXsize[1] = 0;
        return MVC_OK;
    }

    ret = MVC_InitListX(pMvcCtx);
    if (MVC_OK != ret)
    {
        dprint(PRN_REF, "init list error.\n");
        return MVC_ERR;
    } 

    //dprint(PRN_REF, "L0: list after init.\n");
    //MVC_DumpList(pMvcCtx);

    MVC_ReorderListX(pMvcCtx);	
    ret = MVC_RepairList(pMvcCtx);
    if (MVC_OK != ret)
    {
        return MVC_ERR;
    }

    //add find apc->refidx(min)
    MVC_FindMinRefIdx(pMvcCtx);

    pMvcCtx->CurrSlice.picid_refidx0 = (pMvcCtx->pListX[0][0] != NULL) ? pMvcCtx->pListX[0][0]->frame_store->fs_image.image_id : -1;
    //dprint(PRN_REF, "L0: list after reorder.\n");
    //MVC_DumpList(pMvcCtx);

    return MVC_OK;
}


/*
************************************************************************
*      ����ԭ�� :  VOID MVC_NoPicOut(MVC_CTX_S *pMvcCtx)
*      �������� :  �ջ�DPB������ͼ��VO������ͼ���ջ�
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
VOID MVC_NoPicOut(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;

    // no pic out put, now get back g_frame_store in dpb compulsive
    for (i=0; i<pMvcCtx->TotalFsNum; i++)
    {
        if ((1 ==pMvcCtx->FrameStore[i].is_displayed) || (1 == pMvcCtx->FrameStore[i].is_in_dpb))
        {
            pMvcCtx->FrameStore[i].is_used = 0;
            pMvcCtx->FrameStore[i].is_reference = 0;
        }
    }

    // clean all pmv store
    for (i=0; i<pMvcCtx->TotalPmvNum; i++)
    {
        pMvcCtx->PmvStoreUsedFlag[i] = 0;
    }

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_InitDPB(MVC_CTX_S *pMvcCtx,UINT32 ReRangeFlg)
*      �������� :  ��ʼ��dpb
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_InitDPB(MVC_CTX_S *pMvcCtx)
{
    UINT32 i, j;
    SINT32 ret;
    MVC_ClearAllSlice(pMvcCtx);

    // DPB refresh
    for (i=0; i<16; i++)
    {
        if(pMvcCtx->DPB.fs[i] != NULL)
        {
            FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs[i]->logic_fs_id, 1);
        }
        if(pMvcCtx->DPB.fs_ref[i] != NULL)
        {
            FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs_ref[i]->logic_fs_id, 1);
        }
        if(pMvcCtx->DPB.fs_ltref[i] != NULL)
        {
            FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs_ltref[i]->logic_fs_id, 1);
        }
        pMvcCtx->DPB.fs[i] = pMvcCtx->DPB.fs_ref[i] = pMvcCtx->DPB.fs_ltref[i] = NULL;
    }
    pMvcCtx->DPB.size = pMvcCtx->CurrDPBSize;

    pMvcCtx->DPB.used_size = 0;
    pMvcCtx->DPB.ref_frames_in_buffer = 0;
    pMvcCtx->DPB.ltref_frames_in_buffer = 0;
    pMvcCtx->DPB.max_long_term_pic_idx = 0;
    /* lkf58351 20120110: �ڽ�pDirectOutBuf���ǰӦ�ж����Ƿ񻹲��������ݣ���˵���ѷ������߼�֡�棬��Ҫ�ͷŵ� */
    if((pMvcCtx->pDirectOutBuf != NULL) && (pMvcCtx->OldDecMode == I_MODE))
    {
        ret = MVC_OutputFrmToVO(pMvcCtx, pMvcCtx->pDirectOutBuf, 0);
    }
    if(pMvcCtx->pDirectOutBuf != NULL)
    {
        if (NULL != pMvcCtx->CurrPic.frame_store)
        {
            FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
        }
        pMvcCtx->pDirectOutBuf = NULL;
    }

    // apc init
    memset(&pMvcCtx->APC, 0, sizeof(MVC_APC_S));
    pMvcCtx->APC.size = pMvcCtx->DPB.size;
    pMvcCtx->APC.used_size = 0;

    // pmv slot refresh
    for (i=0; i<MVC_MAX_PMV_STORE; i++)
    {
        pMvcCtx->PmvStoreUsedFlag[i] = 0;
    }

    // list refresh
    for (i=0; i<2; i++)
    {
        for (j=0; j<MVC_MAX_LIST_SIZE; j++)
        {
            pMvcCtx->pListX[i][j] = NULL;
        }
        pMvcCtx->CurrSlice.listXsize[i] = 0;
    }

    return MVC_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_GetBackPicFromVOQueue(MVC_CTX_S *pMvcCtx)
*      �������� :  ǿ���ջش�����VO�����е�framestore
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
VOID MVC_GetBackPicFromVOQueue(MVC_CTX_S *pMvcCtx)
{    
    UINT32 i;
    UINT32 usrdat_idx;

    // clean VO queue
    ResetVoQueue(&pMvcCtx->ImageQue);
    
    // clean all MVC_FRAME store
    for (i=0; i<pMvcCtx->TotalFsNum; i++)
    {
        pMvcCtx->FrameStore[i].is_used = 0;
        pMvcCtx->FrameStore[i].is_reference = 0;
        for (usrdat_idx=0; usrdat_idx <4; usrdat_idx++)
        {
            if (NULL != pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx])
            {
                FreeUsdByDec(pMvcCtx->ChanID, pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx]);
                pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx] = NULL;
            }
        }   
    }

    // clean all pmv store
    for (i=0; i<pMvcCtx->TotalPmvNum; i++)
    {
        pMvcCtx->PmvStoreUsedFlag[i] = 0;
    }    

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID mvc_wait_vo(VOID)
*      �������� :  ����VO�ӿ�,VO�������ʾ��ͼ�񿽵��ض�λ�ã�
���ͷ�����VO���е�ͼ��
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
VOID mvc_wait_vo(VOID)
{    
    // ps: ��VO�ṩ�ӿ�
    return;
}


SINT32 MVC_RoundLog2 (SINT32 iValue)
{
    SINT32 iRet = 0;
    SINT32 iValue_square = iValue * iValue;
    while ((1 << (iRet + 1)) <= iValue_square)
    { ++iRet; }

    iRet = (iRet + 1) >> 1;
    return iRet;
}

/*!
************************************************************************
*      ����ԭ�� :      SINT32 MVC_GetReRangeFlag(MVC_CTX_S *pMvcCtx)
*      �������� :  �ж�֡��ռ��Ƿ�Ҫ���·���
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_GetReRangeFlag(MVC_CTX_S *pMvcCtx, UINT32 *p_dpb_changed)
{
    UINT32     i;
    SINT32 ReRangeFlg;
    MVC_SPS_S *pSPS;
    MVC_PPS_S *pPPS;
    UINT32    oldw;
    UINT32    oldh;
    UINT32    neww;
    UINT32    newh;
    UINT32    oldasp;
    UINT32    newasp;
    UINT32    dpb_size=0;
    UINT32    dpb_changed;

    ReRangeFlg = 0;
    pPPS = &pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id];
    //pSPS = &pMvcCtx->SPS[pPPS->seq_parameter_set_id];

    if(-1==pMvcCtx->CurrSlice.svc_extension_flag)
    {
        pSPS = &pMvcCtx->SPS[pPPS->seq_parameter_set_id];
    }
    //mvc non base view��֧,��mvc����������
    else if(0==pMvcCtx->CurrSlice.svc_extension_flag)  // mvc non base view
    {
        if(1==pMvcCtx->Init_Subsps_Flag)
        {
            pSPS = &pMvcCtx->SUBSPS[pPPS->seq_parameter_set_id].sps;
        }
        else
        {
            if(pMvcCtx->CurrSUBSPSID!=pPPS->seq_parameter_set_id)
            {
                dprint(PRN_ERROR,"In one access unit,all non-base view should have the same subsps!\n");
                return MVC_ERR;  //mvc, all non-base view should have the same subsps in a sequence 
            }
            pSPS = &pMvcCtx->CurrSUBSPS.sps;
        }
    }
	else
	{
	    pSPS = NULL;
        dprint(PRN_ERROR, "sps but mvc flag %d is wrong\n", pMvcCtx->CurrSlice.svc_extension_flag);        
        return MVC_ERR;
	}

    /* z56361, 2011-11-1, oldw, oldh��֮ǰ�ɹ�����֡����¼����ֵ�������������жϿ���Ƿ�
       �仯ʱ���ÿ�AllowStartDec. ֮ǰ��д���ȽϷѽ���������. */
    oldw = pMvcCtx->CurWidthInMb;
    oldh = pMvcCtx->CurHeightInMb;
    neww = pSPS->pic_width_in_mbs_minus1 + 1;
    newh = (pSPS->pic_height_in_map_units_minus1 + 1)*(2 - pSPS->frame_mbs_only_flag);

    /* z56361, 20111202, dpb������Ҳ��Ҫ���»���֡�棬���Թ鵽֡��ߴ�仯���� */
    if ( ( neww != oldw) || ( newh != oldh) || (pSPS->dpb_size_plus1 > (pMvcCtx->DPB.size+1)))
    {
        ReRangeFlg = 1;
        REPORT_IMGSIZE_CHANGE( pMvcCtx->ChanID, oldw*16, oldh*16, neww*16, newh*16 );
    }

    if(1==pMvcCtx->CurrSlice.mvcinfo_flag)
    {
        for(i=0;i<32;i++)
        {
            if(dpb_size<pMvcCtx->SUBSPS[i].sps.dpb_size_plus1)
            {
                dpb_size = pMvcCtx->SUBSPS[i].sps.dpb_size_plus1-1;
            }
        }
        dpb_size = MIN(2*dpb_size+MVC_MAX_NUM_VIEWS,MAX(1,MVC_RoundLog2(MVC_MAX_NUM_VIEWS)*16));  //���MVC_MAX_NUM_VIEWS֡,���AVC����࿼��interview�ο�
        //dpb_size = MIN(((pSPS->dpb_size_plus1+dpb_size)<<1)+2, 16);
    }
    else
    {
        dpb_size = pSPS->dpb_size_plus1-1;
    }

    dpb_changed = 0;
    dpb_changed |= ( (dpb_size == pMvcCtx->CurrDPBSize) ? 0 : 1 );
    //dpb_changed |= (pMvcCtx->pCurrSPS->seq_parameter_set_id != pSPS->seq_parameter_set_id);
    dpb_changed |= ReRangeFlg;
    *p_dpb_changed = dpb_changed;
    pMvcCtx->CurrDPBSize = dpb_size;
    oldasp = pMvcCtx->CurrSPS.vui_seq_parameters.aspect_ratio;
    newasp = pSPS->vui_seq_parameters.aspect_ratio;
    if (oldasp != newasp)
    {
        //REPORT_ASPR_CHANGE( oldasp, newasp);
    }

    return ReRangeFlg;
}


/*!
************************************************************************
*   ����ԭ�� :  VOID MVC_DecPOC(MVC_CTX_S *pMvcCtx)
*   �������� :  ��ǰ�ν���ͼ�����g_DPB����
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
VOID MVC_DecPOC(MVC_CTX_S *pMvcCtx)
{
    MVC_SPS_S *sps;
    MVC_PPS_S *pps;
    SINT32 i;
    UINT32 MaxPicOrderCntLsb;
    UINT32 MaxFrameNum;
    SINT32 idr_flag = (MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type);

    pps = &pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id];
    sps = pMvcCtx->pCurrSPS;
    //sps = &pMvcCtx->SPS[pps->seq_parameter_set_id]; 
    MaxPicOrderCntLsb = (1 << (sps->log2_max_pic_order_cnt_lsb_minus4+4));
    MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4+4);

    switch (sps->pic_order_cnt_type)
    {
    case 0: 
        /* 1st */
        if (idr_flag)
        {
            pMvcCtx->CurrPOC.PrevPicOrderCntMsb = 0;
            pMvcCtx->CurrPOC.PrevPicOrderCntLsb = 0;
        }
        else
        {
            if (pMvcCtx->CurrPOC.last_has_mmco_5) 
            {
                if (pMvcCtx->CurrPOC.last_pic_bottom_field)
                {
                    pMvcCtx->CurrPOC.PrevPicOrderCntMsb = 0;
                    pMvcCtx->CurrPOC.PrevPicOrderCntLsb = 0;
                }
                else
                {
                    pMvcCtx->CurrPOC.PrevPicOrderCntMsb = 0;
                    pMvcCtx->CurrPOC.PrevPicOrderCntLsb = pMvcCtx->CurrPOC.toppoc;
                }
            }
        }
        /* Calculate the MSBs of current picture */
        if (pMvcCtx->CurrPOC.pic_order_cnt_lsb < pMvcCtx->CurrPOC.PrevPicOrderCntLsb &&  
            (pMvcCtx->CurrPOC.PrevPicOrderCntLsb - pMvcCtx->CurrPOC.pic_order_cnt_lsb) >= (MaxPicOrderCntLsb / 2))
        {
            pMvcCtx->CurrPOC.PicOrderCntMsb = pMvcCtx->CurrPOC.PrevPicOrderCntMsb + MaxPicOrderCntLsb;
        }
        else if ((pMvcCtx->CurrPOC.pic_order_cnt_lsb>pMvcCtx->CurrPOC.PrevPicOrderCntLsb) &&
            (pMvcCtx->CurrPOC.pic_order_cnt_lsb-pMvcCtx->CurrPOC.PrevPicOrderCntLsb) > (MaxPicOrderCntLsb/2))
        {
            pMvcCtx->CurrPOC.PicOrderCntMsb = pMvcCtx->CurrPOC.PrevPicOrderCntMsb - MaxPicOrderCntLsb;
        }
        else
        {
            pMvcCtx->CurrPOC.PicOrderCntMsb = pMvcCtx->CurrPOC.PrevPicOrderCntMsb;
        } 

        /* 2nd */
        if (!pMvcCtx->CurrSlice.field_pic_flag)
        {
            /* MVC_FRAME pix */
            pMvcCtx->CurrPOC.toppoc = pMvcCtx->CurrPOC.PicOrderCntMsb + pMvcCtx->CurrPOC.pic_order_cnt_lsb;
            pMvcCtx->CurrPOC.bottompoc = pMvcCtx->CurrPOC.toppoc + pMvcCtx->CurrPOC.delta_pic_order_cnt_bottom;
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.framepoc = ((pMvcCtx->CurrPOC.toppoc < pMvcCtx->CurrPOC.bottompoc) ? pMvcCtx->CurrPOC.toppoc : pMvcCtx->CurrPOC.bottompoc); 
        }
        else if (!pMvcCtx->CurrSlice.bottom_field_flag)
        {  
            /* top field */
            pMvcCtx->CurrPOC.ThisPOC= pMvcCtx->CurrPOC.toppoc = pMvcCtx->CurrPOC.PicOrderCntMsb + pMvcCtx->CurrPOC.pic_order_cnt_lsb;
        }
        else
        { 
            /* bottom field */
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.bottompoc = pMvcCtx->CurrPOC.PicOrderCntMsb + pMvcCtx->CurrPOC.pic_order_cnt_lsb;
        }
        pMvcCtx->CurrPOC.framepoc = pMvcCtx->CurrPOC.ThisPOC;
        
        if (pMvcCtx->CurrPOC.frame_num != pMvcCtx->CurrPOC.PreviousFrameNum)
        {
            pMvcCtx->CurrPOC.PreviousFrameNum = pMvcCtx->CurrPOC.frame_num;
        }

        if (0 != pMvcCtx->CurrSlice.nal_ref_idc)
        {
            pMvcCtx->CurrPOC.PrevPicOrderCntLsb = pMvcCtx->CurrPOC.pic_order_cnt_lsb;
            pMvcCtx->CurrPOC.PrevPicOrderCntMsb = pMvcCtx->CurrPOC.PicOrderCntMsb;
        }
        break;

    case 1: 
        /* 1st */
        if (idr_flag)
        {
            pMvcCtx->CurrPOC.FrameNumOffset=0;     /*  first pix of IDRGOP,  */
            //g_CurrPOC.delta_pic_order_cnt[0]=0; /* ignore first delta */
        }
        else 
        {
            if (pMvcCtx->CurrPOC.last_has_mmco_5)
            {
                pMvcCtx->CurrPOC.PrevFrameNumOffset = 0;
                pMvcCtx->CurrPOC.PreviousFrameNum = 0;
            }
            if (pMvcCtx->CurrPOC.frame_num < pMvcCtx->CurrPOC.PreviousFrameNum)
            {   
                /* not first pix of IDRGOP */
                pMvcCtx->CurrPOC.FrameNumOffset = pMvcCtx->CurrPOC.PrevFrameNumOffset + MaxFrameNum;
            }
            else 
            {
                pMvcCtx->CurrPOC.FrameNumOffset = pMvcCtx->CurrPOC.PrevFrameNumOffset;
            }
        }

        /* 2nd */
        if (sps->num_ref_frames_in_pic_order_cnt_cycle) 
        {
            pMvcCtx->CurrPOC.AbsFrameNum = pMvcCtx->CurrPOC.FrameNumOffset + pMvcCtx->CurrPOC.frame_num;
        }
        else 
        {
            pMvcCtx->CurrPOC.AbsFrameNum=0;
        }
        if ((0 == pMvcCtx->CurrSlice.nal_ref_idc) && (0 <pMvcCtx->CurrPOC.AbsFrameNum))
        {
            pMvcCtx->CurrPOC.AbsFrameNum--;
        }
        // 3rd
        pMvcCtx->CurrPOC.ExpectedDeltaPerPicOrderCntCycle = 0;
        
        for (i=0; i<(SINT32)sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            pMvcCtx->CurrPOC.ExpectedDeltaPerPicOrderCntCycle += sps->offset_for_ref_frame[i];
        }
        if (pMvcCtx->CurrPOC.AbsFrameNum)
        {
            pMvcCtx->CurrPOC.PicOrderCntCycleCnt = (pMvcCtx->CurrPOC.AbsFrameNum-1) / sps->num_ref_frames_in_pic_order_cnt_cycle;
            pMvcCtx->CurrPOC.FrameNumInPicOrderCntCycle = (pMvcCtx->CurrPOC.AbsFrameNum-1) % sps->num_ref_frames_in_pic_order_cnt_cycle;
            pMvcCtx->CurrPOC.ExpectedPicOrderCnt = pMvcCtx->CurrPOC.PicOrderCntCycleCnt * pMvcCtx->CurrPOC.ExpectedDeltaPerPicOrderCntCycle;
            for (i=0; i<=(SINT32)pMvcCtx->CurrPOC.FrameNumInPicOrderCntCycle; i++)
            {
                pMvcCtx->CurrPOC.ExpectedPicOrderCnt += sps->offset_for_ref_frame[i];
            }
        }
        else 
        {
            pMvcCtx->CurrPOC.ExpectedPicOrderCnt = 0;
        }
        if (0 == pMvcCtx->CurrSlice.nal_ref_idc)
        {
            pMvcCtx->CurrPOC.ExpectedPicOrderCnt += sps->offset_for_non_ref_pic;
        }
        if (!(pMvcCtx->CurrSlice.field_pic_flag))
        {    
            /* MVC_FRAME pix */
            pMvcCtx->CurrPOC.toppoc = pMvcCtx->CurrPOC.ExpectedPicOrderCnt + pMvcCtx->CurrPOC.delta_pic_order_cnt[0];
            pMvcCtx->CurrPOC.bottompoc = pMvcCtx->CurrPOC.toppoc + sps->offset_for_top_to_bottom_field + pMvcCtx->CurrPOC.delta_pic_order_cnt[1];
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.framepoc = ((pMvcCtx->CurrPOC.toppoc < pMvcCtx->CurrPOC.bottompoc) ? pMvcCtx->CurrPOC.toppoc : pMvcCtx->CurrPOC.bottompoc); 
        }
        else if (!(pMvcCtx->CurrSlice.bottom_field_flag))
        {  
            /* top field  */
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.toppoc = pMvcCtx->CurrPOC.ExpectedPicOrderCnt + pMvcCtx->CurrPOC.delta_pic_order_cnt[0];
        } 
        else
        {  
            /* bottom field */
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.bottompoc = pMvcCtx->CurrPOC.ExpectedPicOrderCnt + sps->offset_for_top_to_bottom_field + pMvcCtx->CurrPOC.delta_pic_order_cnt[0];
        }
        pMvcCtx->CurrPOC.framepoc = pMvcCtx->CurrPOC.ThisPOC;
        
        pMvcCtx->CurrPOC.PreviousFrameNum = pMvcCtx->CurrPOC.frame_num;
        pMvcCtx->CurrPOC.PrevFrameNumOffset = pMvcCtx->CurrPOC.FrameNumOffset;
        break;

    case 2: 
        if (idr_flag) 
        {
            pMvcCtx->CurrPOC.FrameNumOffset=0;     /*  first pix of IDRGOP, */
            pMvcCtx->CurrPOC.ThisPOC = pMvcCtx->CurrPOC.framepoc = pMvcCtx->CurrPOC.toppoc = pMvcCtx->CurrPOC.bottompoc = 0;
        }
        else
        {
            if (pMvcCtx->CurrPOC.last_has_mmco_5)
            {
                pMvcCtx->CurrPOC.PreviousFrameNum = 0;
                pMvcCtx->CurrPOC.PrevFrameNumOffset = 0;
            }
            if (pMvcCtx->CurrPOC.frame_num < pMvcCtx->CurrPOC.PreviousFrameNum)
            {
                pMvcCtx->CurrPOC.FrameNumOffset = pMvcCtx->CurrPOC.PrevFrameNumOffset + MaxFrameNum;
            }
            else 
            {
                pMvcCtx->CurrPOC.FrameNumOffset = pMvcCtx->CurrPOC.PrevFrameNumOffset;
            }
            
            pMvcCtx->CurrPOC.AbsFrameNum = pMvcCtx->CurrPOC.FrameNumOffset + pMvcCtx->CurrPOC.frame_num;
            if (0 == pMvcCtx->CurrSlice.nal_ref_idc)
            {
                pMvcCtx->CurrPOC.ThisPOC = (2*pMvcCtx->CurrPOC.AbsFrameNum - 1);
            }
            else
            {
                pMvcCtx->CurrPOC.ThisPOC = (2*pMvcCtx->CurrPOC.AbsFrameNum);
            }
            
            if (!pMvcCtx->CurrSlice.field_pic_flag)
            {
                pMvcCtx->CurrPOC.toppoc = pMvcCtx->CurrPOC.bottompoc = pMvcCtx->CurrPOC.framepoc = pMvcCtx->CurrPOC.ThisPOC;
            }
            else if (!pMvcCtx->CurrSlice.bottom_field_flag)
            {
                pMvcCtx->CurrPOC.toppoc = pMvcCtx->CurrPOC.framepoc = pMvcCtx->CurrPOC.ThisPOC;
            }
            else
            {
                pMvcCtx->CurrPOC.bottompoc = pMvcCtx->CurrPOC.framepoc = pMvcCtx->CurrPOC.ThisPOC;
            }
        }
        
        pMvcCtx->CurrPOC.PreviousFrameNum = pMvcCtx->CurrPOC.frame_num;
        pMvcCtx->CurrPOC.PrevFrameNumOffset = pMvcCtx->CurrPOC.FrameNumOffset;
        
        break;
        
    default:
        /* error must occurs */
        break;
    }
    
    return;    
}


/*!
************************************************************************
*      ����ԭ�� :   VOID MVC_CalcPicNum(VOID)
*      �������� :  
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
VOID MVC_CalcPicNum(MVC_CTX_S *pMvcCtx)
{
    MVC_SPS_S *pSPS = pMvcCtx->pCurrSPS;
    UINT32 i;
    UINT32 MaxFrameNum = 1 << (pSPS->log2_max_frame_num_minus4 + 4);
    SINT32 add_top, add_bottom;

    add_top = (MVC_TOP_FIELD == pMvcCtx->CurrPic.structure) ? 1 : 0;
    add_bottom = (MVC_BOTTOM_FIELD == pMvcCtx->CurrPic.structure) ? 1 : 0;

    // pic num calculate
    if (MVC_FRAME==pMvcCtx->CurrPic.structure)  
    {
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            if (3==pMvcCtx->DPB.fs_ref[i]->is_reference) // MVC_FRAME, field pair
            {
                if ((1==pMvcCtx->DPB.fs_ref[i]->frame.is_short_term) && (0==pMvcCtx->DPB.fs_ref[i]->frame.is_long_term)) // ���ڲο�
                {
                    if (pMvcCtx->DPB.fs_ref[i]->frame_num > pMvcCtx->CurrPic.frame_num)
                    {
                        pMvcCtx->DPB.fs_ref[i]->frame_num_wrap = pMvcCtx->DPB.fs_ref[i]->frame_num - MaxFrameNum;
                    }
                    else
                    {
                        pMvcCtx->DPB.fs_ref[i]->frame_num_wrap = pMvcCtx->DPB.fs_ref[i]->frame_num;
                    }
                    pMvcCtx->DPB.fs_ref[i]->frame.pic_num = pMvcCtx->DPB.fs_ref[i]->frame_num_wrap;
                }
            }
        }

        // update long_term_pic_num
        for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
        {
            if (3 == pMvcCtx->DPB.fs_ltref[i]->is_reference) // field pair
            {
                if ((0 == pMvcCtx->DPB.fs_ltref[i]->frame.is_short_term) && (1 == pMvcCtx->DPB.fs_ltref[i]->frame.is_long_term))
                {
                    pMvcCtx->DPB.fs_ltref[i]->frame.long_term_pic_num = pMvcCtx->DPB.fs_ltref[i]->long_term_frame_idx;
                }
            }
        }
    }
    else //field
    {  
        for (i=0; i<pMvcCtx->DPB.ref_frames_in_buffer; i++)
        {
            if (0 != pMvcCtx->DPB.fs_ref[i]->is_reference)
            {
                if (pMvcCtx->DPB.fs_ref[i]->frame_num > pMvcCtx->CurrPic.frame_num)
                {
                    pMvcCtx->DPB.fs_ref[i]->frame_num_wrap = pMvcCtx->DPB.fs_ref[i]->frame_num - MaxFrameNum;
                }
                else
                {
                    pMvcCtx->DPB.fs_ref[i]->frame_num_wrap = pMvcCtx->DPB.fs_ref[i]->frame_num;
                }
                if ((pMvcCtx->DPB.fs_ref[i]->is_reference & 1) && 
                    (1==pMvcCtx->DPB.fs_ref[i]->top_field.is_short_term) && (0==pMvcCtx->DPB.fs_ref[i]->top_field.is_long_term)) // include top field
                {
                    pMvcCtx->DPB.fs_ref[i]->top_field.pic_num = (2*pMvcCtx->DPB.fs_ref[i]->frame_num_wrap) + add_top;
                }
                if ((pMvcCtx->DPB.fs_ref[i]->is_reference & 2) && 
                    (1==pMvcCtx->DPB.fs_ref[i]->bottom_field.is_short_term) && (0==pMvcCtx->DPB.fs_ref[i]->bottom_field.is_long_term)) // include bottom field
                {
                    pMvcCtx->DPB.fs_ref[i]->bottom_field.pic_num = (2*pMvcCtx->DPB.fs_ref[i]->frame_num_wrap) + add_bottom;
                }
            }
        }
        // update long_term_pic_num
        for (i=0; i<pMvcCtx->DPB.ltref_frames_in_buffer; i++)
        {
            if (0 != pMvcCtx->DPB.fs_ltref[i]->is_reference)
            {
                if ((pMvcCtx->DPB.fs_ltref[i]->is_reference & 1) && 
                    (0==pMvcCtx->DPB.fs_ltref[i]->top_field.is_short_term) && (1==pMvcCtx->DPB.fs_ltref[i]->top_field.is_long_term)) // include top field
                {
                    pMvcCtx->DPB.fs_ltref[i]->top_field.long_term_pic_num = 2*pMvcCtx->DPB.fs_ltref[i]->long_term_frame_idx + add_top;
                }
                if ((pMvcCtx->DPB.fs_ltref[i]->is_reference & 2) && 
                    (0==pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_short_term) && (1==pMvcCtx->DPB.fs_ltref[i]->bottom_field.is_long_term)) 
                {
                    pMvcCtx->DPB.fs_ltref[i]->bottom_field.long_term_pic_num = 2*pMvcCtx->DPB.fs_ltref[i]->long_term_frame_idx + add_bottom;
                }
            }
        }
    }
}


SINT32  MVC_IsOutDPB(MVC_CTX_S *pMvcCtx,MVC_FRAMESTORE_S  *pFrameStore)
{
    UINT32 i;
    
    if (NULL == pFrameStore)
    {
        return 1;
    }
    
//    for (i = 0; i < pMvcCtx->DPB.used_size; i++)
    for (i = 0; i < pMvcCtx->DPB.size; i++)
    {
        if (pMvcCtx->DPB.fs[i]==NULL)
        {
            continue;
        }    
        if (pMvcCtx->DPB.fs[i] == pFrameStore)
        {
            return 0;
        }
    }
    
    return 1;
}

/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_AllocFrameStore(VOID)
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
SINT32 MVC_AllocFrameStore(MVC_CTX_S *pMvcCtx, SINT32 IsDecGap)
{
    UINT32 structure = pMvcCtx->CurrSlice.field_pic_flag ? (pMvcCtx->CurrSlice.bottom_field_flag ? MVC_BOTTOM_FIELD : MVC_TOP_FIELD) : MVC_FRAME;
    UINT32 i = 0;
    SINT32 j = 0;
    SINT32 LogicFsID = 0;
    FSP_LOGIC_FS_S *pstLFs = NULL;

    if(1 == IsDecGap)
    {
        structure = MVC_FRAME;
    }

    if (I_MODE == pMvcCtx->OldDecMode)
    {
    	pMvcCtx->CurrPic.field_pair_flag = 0;
    }
    else
    {   
    	/*�ɶԳ����ж�*/
        pMvcCtx->CurrPic.field_pair_flag = 0;
		
       if(MVC_TOP_FIELD==structure || MVC_BOTTOM_FIELD==structure)
       {
           if(  !((MVC_FRAME!=pMvcCtx->PrevPic.structure) && (pMvcCtx->PrevPic.view_id==pMvcCtx->CurrSlice.view_id)) )
           {
   //             for(j=pMvcCtx->DPB.used_size-1;j>=0;j--)
                for(j=pMvcCtx->DPB.size-1;j>=0;j--)
               {
                   if (pMvcCtx->DPB.fs[j]==NULL)
                   {
                       continue;
                   }
                   if(((MVC_TOP_FIELD==structure&&2==pMvcCtx->DPB.fs[j]->is_used)||(MVC_BOTTOM_FIELD==structure&&1==pMvcCtx->DPB.fs[j]->is_used)) &&
                       pMvcCtx->CurrSlice.frame_num==pMvcCtx->DPB.fs[j]->frame_num && pMvcCtx->CurrSlice.view_id==pMvcCtx->DPB.fs[j]->view_id &&
                       ((0 == pMvcCtx->CurrSlice.nal_ref_idc && 0 == pMvcCtx->DPB.fs[j]->is_reference) ||
                       (0 != pMvcCtx->CurrSlice.nal_ref_idc && 0 != pMvcCtx->DPB.fs[j]->is_reference)))
                   {
                       pMvcCtx->CurrPic.field_pair_flag=1;
                       //pMvcCtx->CurrPic.fs_image=pMvcCtx->DPB.fs[i]->fs_image;
                       //pMvcCtx->CurrPic.frame_store=&pMvcCtx->FrameStore[pMvcCtx->DPB.fs[i]->fs_image.image_id];
                       //pMvcCtx->CurrPic.pmv_address_idc = pMvcCtx->DPB.fs[i]->pmv_address_idc;
                       pMvcCtx->CurrPic.frame_store = pMvcCtx->DPB.fs[j];
                       pMvcCtx->CurrPic.fs_image = pMvcCtx->DPB.fs[j]->fs_image;
                       pMvcCtx->CurrPic.frame_store->logic_fs_id = pMvcCtx->DPB.fs[j]->logic_fs_id;
                       pstLFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->DPB.fs[j]->logic_fs_id);
                       if (NULL == pstLFs)
                       {
                           dprint(PRN_FATAL, "line: %d, pImage is NULL!\n", __LINE__);
                           return MVC_ERR;
                       }

                       pMvcCtx->CurrPic.pmv_address_idc = pstLFs->PmvIdc;
                       break;
                   }
                   else
                   {
                       pMvcCtx->CurrPic.field_pair_flag = 0;
                   }
               }
           }
           else
           {
           if ((MVC_BOTTOM_FIELD == structure && MVC_TOP_FIELD == pMvcCtx->PrevPic.structure) || 
               (MVC_TOP_FIELD == structure && MVC_BOTTOM_FIELD == pMvcCtx->PrevPic.structure))
           {
               if ((pMvcCtx->OldDecMode == I_MODE ||pMvcCtx->PrevPic.frame_num == pMvcCtx->CurrSlice.frame_num) &&
                        (pMvcCtx->PrevPic.view_id==pMvcCtx->CurrSlice.view_id) &&
                   ((0 == pMvcCtx->CurrSlice.nal_ref_idc && 0 == pMvcCtx->PrevPic.nal_ref_idc) ||
                   (0 != pMvcCtx->CurrSlice.nal_ref_idc && 0 != pMvcCtx->PrevPic.nal_ref_idc)))
               {
                   if ((pMvcCtx->PicCounter - pMvcCtx->PrevStorePicCounter) <= 2)
                   {
                       pMvcCtx->CurrPic.field_pair_flag = 1;
       				if (pMvcCtx->CurrPic.frame_store != NULL)
       				{
                           if (MVC_BOTTOM_FIELD == structure)
           				{
           				    if (pMvcCtx->CurrPic.frame_store->is_used != 1)
           				    {
           				        pMvcCtx->CurrPic.field_pair_flag = 0;
           				    }
           				}
           				else if (MVC_TOP_FIELD == structure)
           				{
           				    if (pMvcCtx->CurrPic.frame_store->is_used != 2)
           				    {
           				        pMvcCtx->CurrPic.field_pair_flag = 0;
           				    }
                           }
       				}
       				else
       				{
       				    pMvcCtx->CurrPic.field_pair_flag = 0;
       				}
   				}
               }
           }
           else
           {
               pMvcCtx->CurrPic.field_pair_flag = 0;
           }
   	  }
       }
       #ifdef VFMW_CFG_CAP_HD_SIMPLE
       /* �ѵ�����������˵�,��ǰ���ɶԣ���֮ǰ����һ����ͼ */
        if ((0 == pMvcCtx->CurrPic.field_pair_flag) && ((MVC_TOP_FIELD == pMvcCtx->PrevPic.structure) 
            || (MVC_BOTTOM_FIELD == pMvcCtx->PrevPic.structure)))
        {
            MVC_RemoveFrameStoreOutDPB(pMvcCtx, pMvcCtx->PrevPic.Dpb_pos);
            FSP_SetRef(pMvcCtx->ChanID, pMvcCtx->PrevPic.image_id, 0);
            FSP_SetDisplay(pMvcCtx->ChanID, pMvcCtx->PrevPic.image_id, 0);
        }
       #endif
    }
       
    if (0 == pMvcCtx->CurrPic.field_pair_flag || NULL == pMvcCtx->CurrPic.frame_store)
    {
        // allocate frame_store
        pMvcCtx->CurrPic.state = MVC_EMPTY;

        for (i=0; i<pMvcCtx->TotalFsNum; i++)
        {
            if ((0 == pMvcCtx->FrameStore[i].is_used) && (1 == MVC_IsOutDPB(pMvcCtx, &pMvcCtx->FrameStore[i])))
            {
                pMvcCtx->CurrPic.state = MVC_PIC_DECODING;
                pMvcCtx->CurrPic.frame_store = &pMvcCtx->FrameStore[i];
                pMvcCtx->CurrPic.frame_store->err_level = 0;
                /* error_level������Ϊ�д������ڽ�gap����������ԭ����ʵ��δ���뵫������MVC_StorePicInDpb()�Ĳ��������������
                   ���������ͼ�п��ܱ����*/
                pMvcCtx->CurrPic.err_level = 101;
                pMvcCtx->CurrPic.fs_image = pMvcCtx->FrameStore[i].fs_image;
                pMvcCtx->CurrPic.fs_image.error_level = 0;
                pMvcCtx->CurrPic.frame_store->mode_skip_flag = 0;
                pMvcCtx->CurrPic.frame_store->is_displayed = 0;
                pMvcCtx->CurrPic.frame_store->is_reference = 0;
                pMvcCtx->CurrPic.frame_store->is_in_dpb = 1;
                pMvcCtx->CurrPic.frame_store->eFoState = MVC_FO_STATE_NA;
                // ���һЩ������գ����ڴ���DPBʱ����ȷ��
                break;
            }
        }

        if (pMvcCtx->CurrPic.state != MVC_PIC_DECODING)
        {
            dprint(PRN_FATAL, "FS_ALLOC_ERR, MVC_ClearAll\n");
            MVC_ClearAll(pMvcCtx,1);
            return MVC_ERR;  //FS_ALLOC_ERR
        }

        /* FrameStore���뵽֮��, ��Ҫ�½��߼�֡�棬�������ܵõ������ĵ�ַ */
        LogicFsID = FSP_NewLogicFs(pMvcCtx->ChanID, (IsDecGap? 0: 1));
        pstLFs = FSP_GetLogicFs(pMvcCtx->ChanID, LogicFsID);

        if (LogicFsID >= 0 && NULL != pstLFs)
        {
            if (pMvcCtx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION)
            {
                IMAGE *pstImg = &pstLFs->stDispImg;
                UINT32 HeightAligh = pMvcCtx->CurHeightInMb*16;
                UINT32 ChromOffset = HeightAligh*pstImg->image_stride;
    
                pstImg->image_width        = pMvcCtx->CurWidthInMb*16;
                pstImg->image_height       = HeightAligh;
                pstImg->chrom_phy_addr     = pstImg->luma_phy_addr + ChromOffset;
                pstImg->top_chrom_phy_addr = pstImg->top_luma_phy_addr + ChromOffset;
                pstImg->btm_chrom_phy_addr = pstImg->top_chrom_phy_addr + pstImg->image_stride;
                pstImg->chrom_2d_phy_addr  = pstImg->luma_2d_phy_addr + ChromOffset;
            }
            /* FrameStore��fs_image���پ���һһ��Ӧ��ϵ��Ϊ�������д������µ�image�������� */
            memcpy(&pMvcCtx->CurrPic.frame_store->fs_image, &pstLFs->stDispImg, sizeof(IMAGE));
            memcpy(&pMvcCtx->CurrPic.fs_image, &pMvcCtx->CurrPic.frame_store->fs_image, sizeof(IMAGE));

            pMvcCtx->CurrPic.frame_store->logic_fs_id = LogicFsID;
            pMvcCtx->CurrPic.pmv_address_idc = pstLFs->PmvIdc;
            //dprint(PRN_DBG,"=== dec(lfs=%d): 0x%x(id=%d), 2D: 0x%x(id=%d), pmv: 0x%x(id=%d) ===\n", LogicFsID, 
            //        pMvcCtx->CurrPic.fs_image.luma_phy_addr, pstLFs->pstDecodeFs->PhyFsID,
            //        pstLFs->pstDispOutFs->PhyAddr, pstLFs->pstDispOutFs->PhyFsID,
            //        pstLFs->PmvAddr, pstLFs->PmvIdc);
        }
        else
        {
            pMvcCtx->CurrPic.state = MVC_EMPTY;
            pMvcCtx->CurrPic.frame_store->is_in_dpb = 0;
            dprint(PRN_FATAL, "Can not new logic fs! MVC_ClearAll\n");
            MVC_ClearAll(pMvcCtx,1); 
            return MVC_ERR;  //FS_ALLOC_ERR
        }
    }
    else
    {
    }

    pMvcCtx->CurrPic.structure = structure;  

    if(1 == IsDecGap)
    {
        return MVC_OK;
    }
	
    /* ��MVC_FRAME.structureָ���洢�ṹ����Ҫ��֡�����Ծ������. */
    pMvcCtx->CurrPic.frame_store->frame.structure = pMvcCtx->CurrSlice.field_pic_flag? MVC_FIELD_PAIR: MVC_FRAME;

    if (1 == pMvcCtx->CurrPic.field_pair_flag)
    {
        pMvcCtx->PrevPic.structure = MVC_FRAME;
        pMvcCtx->PrevPic.frame_num = pMvcCtx->CurrSlice.frame_num;
        pMvcCtx->PrevPic.nal_ref_idc = pMvcCtx->CurrSlice.nal_ref_idc;    
        pMvcCtx->PrevPic.pic_counter = pMvcCtx->PicCounter;
        pMvcCtx->PrevPic.image_id = LogicFsID;
        pMvcCtx->PrevPic.FrameStoreFlag = 0;        
		pMvcCtx->PrevPic.view_id = pMvcCtx->CurrSlice.view_id;
    }
    else
    {
        // update prevpic for next field pair judge
        pMvcCtx->PrevPic.structure = structure;
        pMvcCtx->PrevPic.frame_num = pMvcCtx->CurrSlice.frame_num;
        pMvcCtx->PrevPic.nal_ref_idc = pMvcCtx->CurrSlice.nal_ref_idc;
        pMvcCtx->PrevPic.pic_counter = pMvcCtx->PicCounter;
		pMvcCtx->PrevPic.view_id = pMvcCtx->CurrSlice.view_id;
        pMvcCtx->PrevPic.image_id = LogicFsID;
        if(MVC_FRAME != structure)
        {
            pMvcCtx->PrevPic.FrameStoreFlag = 1;
        }
        else
        {
            pMvcCtx->PrevPic.FrameStoreFlag = 0;
        }
    }

    return MVC_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_DecGap(VOID)
*      �������� :  ��ʼ��dpb
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_DecGap(MVC_CTX_S *pMvcCtx)
{
    SINT32 CurrFrameNum = pMvcCtx->CurrSlice.frame_num;
    SINT32 MaxFrameNum = 1 << (pMvcCtx->pCurrSPS->log2_max_frame_num_minus4 + 4); 
    SINT32 UnusedShortTermFrameNum = (pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] + 1) % MaxFrameNum;
    SINT32 tmp1 = pMvcCtx->CurrPOC.delta_pic_order_cnt[0];
    SINT32 tmp2 = pMvcCtx->CurrPOC.delta_pic_order_cnt[1];
    SINT32 tmp3 = pMvcCtx->Mark[pMvcCtx->CurrMarkIdx].adaptive_ref_pic_marking_mode_flag;
    SINT32 nal_ref_idc = (pMvcCtx->pCurrSPS->gaps_in_frame_num_value_allowed_flag==1)? 2: 0;
    SINT32 ret;

    pMvcCtx->CurrPic.pMarkPara = &(pMvcCtx->Mark[pMvcCtx->CurrMarkIdx]);  //zhl add, to avoid coredump

    /* ���DPBΪ����Ӧ�ý�GAP����ʱ��GAP�Ǽ���. ������˼�GAPʱ���ܷ���MVC_ERR����Ϊ�������GAP������
       ֮������MVC_InitPic�������ü���, ������OldSLice�������Ϣ����ˢ�»ᵼ�º���slice�ж�֡�߽���� */
    if (pMvcCtx->DPB.used_size == 0)
    {
        dprint(PRN_FATAL,"GAP found while DPB is MVC_EMPTY!\n");
        return MVC_OK;
    }

    // save poc para of currslice
    pMvcCtx->CurrPOC.delta_pic_order_cnt[0] = pMvcCtx->CurrPOC.delta_pic_order_cnt[1] = 0;

    dprint(PRN_CTRL, "MVC_FRAME num gap try to fill it \n");
    dprint(PRN_CTRL, "CurrFrameNum = %d  UnusedShortTermFrameNum = %d\n", CurrFrameNum, UnusedShortTermFrameNum);

    while (CurrFrameNum > UnusedShortTermFrameNum)
    {
        ret = MVC_AllocFrameStore(pMvcCtx, 1); 
        if (MVC_OK != ret)
        {         
            pMvcCtx->CurrPOC.delta_pic_order_cnt[0] = tmp1;
            pMvcCtx->CurrPOC.delta_pic_order_cnt[1] = tmp2;
            pMvcCtx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = tmp3; 
            
            dprint(PRN_FATAL, "cann't allocate MVC_FRAME store when gap find\n");
            return MVC_ERR;        
        }
        dprint(PRN_CTRL, "allocate MVC_FRAME store when gap find\n");

        /*pic para init*/
        pMvcCtx->CurrPic.nal_unit_type = MVC_NALU_TYPE_SLICE;
        pMvcCtx->CurrPic.frame_store->frame_num = UnusedShortTermFrameNum;
        pMvcCtx->CurrPic.frame_num = UnusedShortTermFrameNum;
        pMvcCtx->CurrPic.frame_store->frame.pic_num= UnusedShortTermFrameNum; 
        pMvcCtx->CurrPic.nal_ref_idc= nal_ref_idc;
        pMvcCtx->CurrPic.frame_store->frame.long_term_pic_num = 0;
        pMvcCtx->CurrPic.frame_store->is_displayed = 1;
        pMvcCtx->CurrPic.frame_store->is_in_dpb = 0;
        pMvcCtx->CurrPic.non_existing = pMvcCtx->CurrPic.frame_store->non_existing = 1;
        pMvcCtx->CurrPic.view_id = pMvcCtx->CurrSlice.view_id;  //mvc

        /*CurrMark*/
        pMvcCtx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = 0;

        if (0 != pMvcCtx->pCurrSPS->pic_order_cnt_type)
        {
            pMvcCtx->CurrPOC.frame_num = UnusedShortTermFrameNum;
            MVC_DecPOC(pMvcCtx);
            pMvcCtx->CurrPic.thispoc = pMvcCtx->CurrPOC.ThisPOC;
            pMvcCtx->CurrPic.framepoc = pMvcCtx->CurrPOC.framepoc;
            pMvcCtx->CurrPic.toppoc = pMvcCtx->CurrPOC.toppoc;
            pMvcCtx->CurrPic.bottompoc = pMvcCtx->CurrPOC.bottompoc;
        }
        pMvcCtx->CurrPic.is_long_term = 0;
        ret = MVC_StorePicInDpb(pMvcCtx);
        if (MVC_OK != ret)
        {
            pMvcCtx->CurrPOC.delta_pic_order_cnt[0] = tmp1;
            pMvcCtx->CurrPOC.delta_pic_order_cnt[1] = tmp2;
            pMvcCtx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = tmp3; 
            
            dprint(PRN_FATAL, "line %d: store gap pic err, ret=%d\n", __LINE__, ret);
            return MVC_ERR;
        }

        // g_CurrPic.frame_store->is_in_dpb = 0;        

        pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] = UnusedShortTermFrameNum;

        UnusedShortTermFrameNum = (UnusedShortTermFrameNum + 1) % MaxFrameNum;      
    }

    pMvcCtx->CurrPOC.delta_pic_order_cnt[0] = tmp1;
    pMvcCtx->CurrPOC.delta_pic_order_cnt[1] = tmp2;
    pMvcCtx->CurrPic.pMarkPara->adaptive_ref_pic_marking_mode_flag = tmp3; 

    return MVC_OK;    
}

/***********************************************************************************
* Function: mvc_combine_scalinglist(UINT32 *scalingList_Org, UINT32 *scalingList, SINT32 sizeOfScalingList )
* Description:��4�ֽڵ���������ƴ��һ��word 
* Input:      
* Output:    
* Return:   
*              
* Others:      ��
***********************************************************************************/ 
/* MVC */
VOID mvc_combine_scalinglist(UINT32 *scalingList_Org, UINT32 *scalingList, SINT32 sizeOfScalingList )
{
    SINT32 i;
    UINT32 scani;
    UINT32 w_posit;
    UINT32 b_posit;

    for(i=0; i<sizeOfScalingList; i++)
    {
        scani = (sizeOfScalingList == 16) ? MVC_g_ZZ_SCAN[i] : MVC_g_ZZ_SCAN8[i];
        w_posit = scani/4;
        b_posit = scani%4;

        scalingList[w_posit] = (scalingList[w_posit] & (~(0xff << (8*b_posit)))) | ((scalingList_Org[scani]& 0xff) << (8*b_posit)); 
    }	
}
/* End */
/***********************************************************************************
* Function: mvc_assign_quant_params(MVC_SPS_S *pSPS, MVC_PPS_S *pPPS)
* Description: ����PPS��������
* Input:      
* Output:    
* Return:   MVC_OK      ���PPS������
*              MVC_ERR     δ���PPS������
* Others:      ��
***********************************************************************************/
/* MVC */
SINT32  mvc_assign_quant_params(MVC_SPS_S *pSPS, MVC_PPS_S *pPPS)
{
    UINT32 i;

    CHECK_NULL_PTR_ReturnValue(pSPS, MVC_ERR);
    CHECK_NULL_PTR_ReturnValue(pPPS, MVC_ERR);

    if(pPPS->pic_scaling_matrix_present_flag) // then check pps
    {
        for(i=0; i<8; i++)
        {
            if(i<6)
            {
                if(!pPPS->pic_scaling_list_present_flag[i]) // fall-back rule B
                {
                    if (i==0)
                    {
                        if(!pSPS->seq_scaling_matrix_present_flag)
                        {
                            memcpy(pPPS->qmatrix4x4[i], MVC_quant_intra_default, 4*4);
                        }
                        else
                        {
                            memcpy(pPPS->qmatrix4x4[i],  pSPS->qmatrix4x4[i],  4*4);
                        }
                    }
                    else if (i==3)
                    {
                        if(!pSPS->seq_scaling_matrix_present_flag)
                        {
                            memcpy(pPPS->qmatrix4x4[i], MVC_quant_inter_default, 4*4);
                        }
                        else
                        {
                            memcpy(pPPS->qmatrix4x4[i],  pSPS->qmatrix4x4[i],  4*4);
                        }
                    }
                    else
                    {
                        memcpy(pPPS->qmatrix4x4[i],  pPPS->qmatrix4x4[i-1], 4*4);
                    }
                }
                else
                {
                    if(pPPS->UseDefaultScalingMatrix4x4Flag[i])
                    {
                        memcpy(pPPS->qmatrix4x4[i], (i<3) ? MVC_quant_intra_default : MVC_quant_inter_default, 4*4); 
                    }
                    else
                    {
                        //mvc_combine_scalinglist(pPPS->ScalingList4x4[i], pPPS->qmatrix4x4[i], 4*4);
                    }
                }
            }
            else
            {
                if(!pPPS->pic_scaling_list_present_flag[i]) // fall-back rule B
                {
                    if(!pSPS->seq_scaling_matrix_present_flag)
                    {
                         memcpy(pPPS->qmatrix8x8[i-6],  (6 == i) ? MVC_quant8_intra_default : MVC_quant8_inter_default, 4*16);
                    }
                    else
                    {
                        memcpy(pPPS->qmatrix8x8[i-6],  pSPS->qmatrix8x8[i-6], 4*16);
                    }
                    
                }
                else
                {
                    if(pPPS->UseDefaultScalingMatrix8x8Flag[i-6])
                    {
                        memcpy(pPPS->qmatrix8x8[i-6],  (6 == i) ? MVC_quant8_intra_default : MVC_quant8_inter_default, 4*16);
                    }
                    else
                    {
                        //mvc_combine_scalinglist(pPPS->ScalingList8x8[i-6], pPPS->qmatrix8x8[i-6], 4*16);
                    }
                }
            }
        }
    }

    return MVC_OK;
}
/* End */
/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_InitPic(MVC_CTX_S *pMvcCtx)
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
SINT32 MVC_InitPic(MVC_CTX_S *pMvcCtx)
{
    UINT32 MaxFrameNum;
    SINT32 ret, dar;
    UINT32 ReRangeFlg;
    SINT32 uidxpic;
    UINT32 uidxsei;
    UINT32 ChromaFormat;
    UINT32 dpb_changed = 0; 
    UINT32 ImageWidth, ImageHeight;
    FSP_PHY_FS_S *pstDecFs;

    MVC_SPS_S *pSPS;
    MVC_PPS_S *pPPS;

    pPPS = &pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id];
    pSPS = &pMvcCtx->SPS[pPPS->seq_parameter_set_id];

    if (pMvcCtx->CurrPic.state != MVC_EMPTY)
    {
        if(pMvcCtx->CurrPic.frame_store != NULL)
        {
            if (pMvcCtx->CurrPic.frame_store->is_used == 0)
            {
                FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
                pMvcCtx->CurrPic.frame_store->is_used = 0;
                pMvcCtx->CurrPic.frame_store->is_in_dpb = 0;
            }
        }
        
        pMvcCtx->CurrPic.state = MVC_EMPTY;
    }

    ReRangeFlg = MVC_GetReRangeFlag(pMvcCtx, &dpb_changed);


    if(-1==pMvcCtx->CurrSlice.svc_extension_flag)
    {
        pMvcCtx->Init_Subsps_Flag = 1; //mvc
        pMvcCtx->CurrSPSID = pPPS->seq_parameter_set_id;
        pSPS = &pMvcCtx->SPS[pPPS->seq_parameter_set_id];
        /*1 refresh SPS and PPS */
        // sps id�仯,spsһ������,ͬid�ŵ�SPS�����и���,��ǰslice�����½���Ļָ���
        if ( (pMvcCtx->CurrSPS.seq_parameter_set_id != pSPS->seq_parameter_set_id) || pSPS->is_refreshed)
        {
            pMvcCtx->SeqCnt++;
            // new sps copy and init dpb
            pSPS->is_refreshed = 0;
            memmove(&(pMvcCtx->CurrSPS), pSPS, sizeof(MVC_SPS_S));
        }
        pMvcCtx->pCurrSPS = &pMvcCtx->CurrSPS;				
    }

    //mvc non base view��֧,��mvc����������
    if(0==pMvcCtx->CurrSlice.svc_extension_flag)  // mvc non base view
    {
        if(1==pMvcCtx->Init_Subsps_Flag)
        {
            pMvcCtx->Init_Subsps_Flag = 0;
            memmove(&(pMvcCtx->CurrSUBSPS), &pMvcCtx->SUBSPS[pPPS->seq_parameter_set_id], sizeof(MVC_SUBSPS_S));
            pMvcCtx->pCurrSPS = &pMvcCtx->CurrSUBSPS.sps;
            pMvcCtx->CurrSUBSPSID = pMvcCtx->pCurrSPS->seq_parameter_set_id;
        }
        else
        {
            if(pMvcCtx->CurrSUBSPSID!=pPPS->seq_parameter_set_id)
            {
                dprint(PRN_ERROR,"In one access unit,all non-base view should have the same subsps!\n");
                return MVC_ERR;  //mvc, all non-base view should have the same subsps in a sequence 
            }
            pMvcCtx->pCurrSPS = &pMvcCtx->CurrSUBSPS.sps;
        }
    }

    if ( (pMvcCtx->CurrPPS.pic_parameter_set_id != pPPS->pic_parameter_set_id) || pPPS->is_refreshed )
    {
        pPPS->is_refreshed = 0;
        mvc_assign_quant_params(pMvcCtx->pCurrSPS, pPPS);
        memmove(&(pMvcCtx->CurrPPS), pPPS, sizeof(MVC_PPS_S));
    }
	
    /*1 refresh SPS and PPS */
    // sps id�仯,spsһ������,ͬid�ŵ�SPS�����и���,��ǰslice�����½���Ļָ���
    if ((dpb_changed)&&((pMvcCtx->pstExtraData->stChanOption.s32ReRangeEn == 0)&&(pMvcCtx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION)))
    {
        pMvcCtx->SeqCnt++;
        //RerangeMemFlag = MVC_GetReRangeFlag(pMvcCtx);

        // SPS����ǣ�浽��֮ǰ����ͼ��Ĵ���
        if (ReRangeFlg)
        {
            ret = MVC_FlushDPB(pMvcCtx, -1);
            if (MVC_OK != ret)
            {
                dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                return MVC_ERR;
            }
        }

        ret = MVC_InitDPB(pMvcCtx);
        if (MVC_OK != ret)
        {
            dprint(PRN_FATAL, "line %d: init dpb return %d\n", __LINE__, ret);
            return MVC_ERR;
        }

        ImageWidth = ((UINT32)pMvcCtx->CurrSPS.pic_width_in_mbs_minus1 + 1) * 16;
        ImageHeight = ((UINT32)pMvcCtx->CurrSPS.pic_height_in_map_units_minus1 + 1) * (2 - pMvcCtx->CurrSPS.frame_mbs_only_flag) * 16;
        if ((ReRangeFlg)&&(pMvcCtx->CurWidthInMb==0))
        {   
            // when DPB size is gotten, FrameStore can use is gotten too
            //pMvcCtx->TotalPmvNum = pMvcCtx->CurrSPS.dpb_size_plus1;
            // MVC ��Ҫpmv֡�����double��y226912
            pMvcCtx->TotalPmvNum = pMvcCtx->CurrSPS.dpb_size_plus1*2;

            if ((ImageWidth > pMvcCtx->pstExtraData->stChanOption.s32MaxWidth)||(ImageHeight > pMvcCtx->pstExtraData->stChanOption.s32MaxHeight))
            {
                dprint(PRN_FATAL, "resolution error, the CAP_LEVEL_USER_DEFINE_WITH_OPTION channel with s32ReRangeEn == 0 can't support such bitstream.\n");
                return MVC_ERR;
            }
            if (pMvcCtx->pstExtraData->stChanOption.s32MaxRefFrameNum < pMvcCtx->DPB.size)
            {
                dprint(PRN_FATAL, "s32MaxRefFrameNum < pMvcCtx->DPB.size, the CAP_LEVEL_USER_DEFINE_WITH_OPTION channel can't support such bitstream.\n");
                return MVC_ERR;
            }
            if (pMvcCtx->pstExtraData->stChanOption.s32SupportBFrame == 0)
			{
			    pMvcCtx->TotalPmvNum = 1;
			}

            if ( VF_OK != MVC_ArrangeVahbMem(pMvcCtx, pMvcCtx->pstExtraData->stChanOption.s32MaxWidth, pMvcCtx->pstExtraData->stChanOption.s32MaxHeight, &pMvcCtx->TotalFsNum, pMvcCtx->TotalPmvNum) )
            {
                if(1 != pMvcCtx->pstExtraData->s32WaitFsFlag)
                {
	                pMvcCtx->CurWidthInMb = 0;
	                pMvcCtx->CurHeightInMb = 0;

	                dprint(PRN_FATAL, "mem arrange err, MVC_ClearAll\n");
	                MVC_ClearAll(pMvcCtx,0);
	                return MVC_ERR;  //VHB_MEM_ERR
                }
                else
                {
                    return MVC_NOTDEC;
                }
            }
        }
            //else
            {
                /* ��¼��ǰ�Ľ����ߣ�Ŀ���Ƿ�������жϿ���Ƿ����仯 */
                pMvcCtx->CurWidthInMb = ImageWidth / 16;
                pMvcCtx->CurHeightInMb = ImageHeight / 16;
            }
		
    }
    else if ( (dpb_changed) || (1 == pMvcCtx->pstExtraData->s32WaitFsFlag))
    {
        pMvcCtx->SeqCnt++;
        //RerangeMemFlag = MVC_GetReRangeFlag(pMvcCtx);

        // SPS����ǣ�浽��֮ǰ����ͼ��Ĵ���
        if ((ReRangeFlg) || (1 == pMvcCtx->pstExtraData->s32WaitFsFlag))
        {
            dprint(PRN_DBG, "get back frm\n");
            mvc_wait_vo();
            MVC_GetBackPicFromVOQueue(pMvcCtx);  
        }
        else
        {
            ret = MVC_FlushDPB(pMvcCtx, -1);
            if (MVC_OK != ret)
            {
                dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                return MVC_ERR;
            }
        }

        ret = MVC_InitDPB(pMvcCtx);
        if (MVC_OK != ret)
        {
            dprint(PRN_FATAL, "line %d: init dpb return %d\n", __LINE__, ret);
            return MVC_ERR;
        }

        if ((ReRangeFlg) || (1 == pMvcCtx->pstExtraData->s32WaitFsFlag))
        {   
            // when DPB size is gotten, FrameStore can use is gotten too
            //pMvcCtx->TotalPmvNum = pMvcCtx->CurrSPS.dpb_size_plus1;
            // MVC ��Ҫpmv֡�����double��y226912
            pMvcCtx->TotalPmvNum = pMvcCtx->CurrSPS.dpb_size_plus1*2;

            ImageWidth = ((UINT32)pMvcCtx->CurrSPS.pic_width_in_mbs_minus1 + 1) * 16;
            ImageHeight = ((UINT32)pMvcCtx->CurrSPS.pic_height_in_map_units_minus1 + 1) * (2 - pMvcCtx->CurrSPS.frame_mbs_only_flag) * 16;

            if ( VF_OK != MVC_ArrangeVahbMem(pMvcCtx, ImageWidth, ImageHeight, &pMvcCtx->TotalFsNum, pMvcCtx->TotalPmvNum) )
            {
                if(pMvcCtx->pstExtraData->s32WaitFsFlag != 1)
                {
	                pMvcCtx->CurWidthInMb = 0;
	                pMvcCtx->CurHeightInMb = 0;

	                dprint(PRN_FATAL, "mem arrange err, MVC_ClearAll\n");
	                MVC_ClearAll(pMvcCtx,0);
	                return MVC_ERR;  //VHB_MEM_ERR
                }
                else
                {
                    return MVC_NOTDEC;
                }
            }
            else
            {
                /* ��¼��ǰ�Ľ����ߣ�Ŀ���Ƿ�������жϿ���Ƿ����仯 */
                pMvcCtx->CurWidthInMb = ImageWidth / 16;
                pMvcCtx->CurHeightInMb = ImageHeight / 16;
            }
        }
    }

    if ( (pMvcCtx->CurrPPS.pic_parameter_set_id != pPPS->pic_parameter_set_id) || pPPS->is_refreshed )
    {
        pPPS->is_refreshed = 0;
        memmove(&(pMvcCtx->CurrPPS), pPPS, sizeof(MVC_PPS_S));
    }



    if(-1 == pMvcCtx->CurrSlice.svc_extension_flag)
    {
        pMvcCtx->CurrSlice.voidx = 0;
    }

    if(0 == pMvcCtx->CurrSlice.svc_extension_flag)
    {
        pMvcCtx->CurrSlice.voidx = MVC_GetVOIdx(pMvcCtx->CurrSUBSPS.view_id, pMvcCtx->CurrSUBSPS.num_views_minus1+1, pMvcCtx->CurrSlice.view_id);
    }
    /*2 gap process*/
    MaxFrameNum = 1 << (pMvcCtx->pCurrSPS->log2_max_frame_num_minus4 + 4);
    if(MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type)
    {
        pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] = pMvcCtx->CurrSlice.frame_num;
    }

    #if 0
    if (pMvcCtx->CurrPOC.last_has_mmco_5)
    {
        pMvcCtx->CurrSlice.prev_frame_num = 0;
        pMvcCtx->PrevPic.frame_num = 0;
    }
    #endif
    /* ���������I֡ģʽ��������gap. z56361, 20110830 */
    if ((pMvcCtx->OldDecMode != I_MODE) &&
        (pMvcCtx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB) &&
        (pMvcCtx->CurrSlice.frame_num != pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx]) && 
        (pMvcCtx->CurrSlice.frame_num != ((pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] + 1) % MaxFrameNum)))
    {
	    pMvcCtx->pstExtraData->s32MoreGapEnable = 1; //after seek to avoid clear dpb when num is discontinous.
        if ((pMvcCtx->pstExtraData->s32MoreGapEnable == 0)  //��BVTĳЩӦ�ó����»ᱻ��Ϊ1
        &&(!((pMvcCtx->pstExtraData->stPvrInfo.u32BFrmRefFlag==1)
		     &&(pMvcCtx->pstExtraData->stPvrInfo.u32IDRFlag==0)
		     &&(pMvcCtx->pstExtraData->stPvrInfo.u32ContinuousFlag==0)
		     &&(!((pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed >= -1024)
		         &&(pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed <= 1024))))))
        {
        if (pMvcCtx->CurrSlice.frame_num > pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx])
            {
            if (pMvcCtx->CurrSPS.dpb_size_plus1 < (pMvcCtx->CurrSlice.frame_num - pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx])
                && pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] != 0)  // view 1 �ڸ�λ��frame numͨ����ܴ���Ȼ����I֡�����ǿ��Խ⣬���ﲻӦ�ö���������Ӧ�ü��Ϻ�һ���ж����� y00226912
                {
                    if (MVC_I_SLICE != pMvcCtx->CurrSlice.slice_type)
                    {
                        MVC_ClearCurrPic(pMvcCtx);
                        MVC_ClearDPB(pMvcCtx,-1);
                        dprint(PRN_FATAL, "line %d: MVC_FRAME gap(=%d) > dpb size(=%d)\n", __LINE__, (pMvcCtx->CurrSlice.frame_num - pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx]),
                        pMvcCtx->CurrSPS.dpb_size_plus1);
                        return MVC_ERR;  //IDR_LOST
                    }
                    else
                    {
                    pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] = (0 ==pMvcCtx->CurrSlice.frame_num) ? 
                            MaxFrameNum - 1 : pMvcCtx->CurrSlice.frame_num - 1;
                        ret = MVC_FlushDPB(pMvcCtx,-1);
                        if (MVC_OK != ret)
                        {
                            dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                            return MVC_ERR;
                        }
                    }
                }
            }
            else
            {
            if (pMvcCtx->CurrSPS.dpb_size_plus1 < (pMvcCtx->CurrSlice.frame_num +  MaxFrameNum - pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx])
                && pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] != 0)    // view 1 �ڸ�λ��frame numͨ����ܴ���Ȼ����I֡�����ǿ��Խ⣬���ﲻӦ�ö���������Ӧ�ü��Ϻ�һ���ж����� y00226912
                {
                    if (MVC_I_SLICE != pMvcCtx->CurrSlice.slice_type)
                    {
                        MVC_ClearCurrPic(pMvcCtx);
                    MVC_ClearDPB(pMvcCtx, -1);
                    dprint(PRN_FATAL, "line %d: MVC_FRAME gap(=%d) > dpb size(=%d)\n", __LINE__, (pMvcCtx->CurrSlice.frame_num - pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx]),
                            pMvcCtx->CurrSPS.dpb_size_plus1);
                        return MVC_ERR;  //IDR_LOST
                    }
                    else
                    {
                    pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] = (0 ==pMvcCtx->CurrSlice.frame_num)?
                            MaxFrameNum - 1 : pMvcCtx->CurrSlice.frame_num - 1;
                    ret = MVC_FlushDPB(pMvcCtx, -1);
                        if (MVC_OK != ret)
                        {
                            dprint(PRN_FATAL, "line %d: flush dpb return %d\n", __LINE__, ret);
                            return MVC_ERR;
                        }
                    }
                }
            }
        }
        if (0 == pMvcCtx->CurrSPS.gaps_in_frame_num_value_allowed_flag)
        {
            dprint(PRN_CTRL, "MVC_FRAME num gap don't allowed but gap find\n");
            if ((pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed == 0)||(pMvcCtx->pstExtraData->s32FastForwardBackwardSpeed == 1024))
            {
                /*����������ʱ��poc�����Сʱ��dependent view���з�I Slice���¶���*/
                if (pMvcCtx->CurrSlice.view_id == 0
                 && pMvcCtx->CurrSlice.frame_num < ((pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] + 1) % MaxFrameNum))
                {
                    // IDR lost return and find next recoverpoint
                    if (MVC_I_SLICE != pMvcCtx->CurrSlice.slice_type)
                    {
                        MVC_ClearCurrPic(pMvcCtx);
                        MVC_ClearDPB(pMvcCtx,-1);
                        dprint(PRN_FATAL,"line %d: frame num(%d/%d) find gap in NON-I slice but here gap is not allowed\n", __LINE__, pMvcCtx->CurrSlice.frame_num, ((pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] + 1) % MaxFrameNum));
                        return MVC_ERR;  //IDR_LOST
                    }		
                }
                /* ���MVC seek�������⣬�ϸ���Э��涨����������²���gap */
                /*  else
                {
                    ret = MVC_DecGap(pMvcCtx);
                    if (MVC_OK != ret)
                    {
                        dprint(PRN_FATAL,"line %d: dec gap failed\n", __LINE__);
                        return MVC_ERR;
                    }
                }
                */
            }
        }
        else
        {
            ret = MVC_DecGap(pMvcCtx);
            if (MVC_OK != ret)
            {
                dprint(PRN_FATAL,"line %d: dec gap failed\n", __LINE__);
                return MVC_ERR;
            }            
        }
    }

    if (pMvcCtx->CurrSlice.nal_ref_idc)
    {
        pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] = pMvcCtx->CurrSlice.frame_num;
    }

    /*3 calculate poc of g_CurrPic*/    
    pMvcCtx->CurrPOC.frame_num = pMvcCtx->CurrSlice.frame_num;
    MVC_DecPOC(pMvcCtx);       

    /*4 ����֡���PMV�ռ�*/
    ret = MVC_AllocFrameStore(pMvcCtx, 0);
    if (MVC_OK != ret)
    {
        dprint(PRN_FATAL,"line %d: alloc framestore failed\n", __LINE__);
        return MVC_ERR;
    }

    /*5 ��ʼ����ǰ����ͼ��Ĳ���*/
    // ���Ը��ݳɶԳ��������ظ���ֵ
    // pmv_address_idc,structure, *frame_store����֡����亯���в���
    pMvcCtx->CurrPic.mode_skip_flag =0;    
    pMvcCtx->CurrPic.state = MVC_PIC_DECODING;
    pMvcCtx->CurrPic.nal_unit_type = pMvcCtx->CurrSlice.nal_unit_type;
    pMvcCtx->CurrPic.frame_num = pMvcCtx->CurrSlice.frame_num;
    pMvcCtx->CurrPic.nal_ref_idc = pMvcCtx->CurrSlice.nal_ref_idc;
    pMvcCtx->CurrPic.thispoc = pMvcCtx->CurrPOC.ThisPOC;
    pMvcCtx->CurrPic.framepoc = pMvcCtx->CurrPOC.framepoc;
    pMvcCtx->CurrPic.toppoc = pMvcCtx->CurrPOC.toppoc;
    pMvcCtx->CurrPic.bottompoc = pMvcCtx->CurrPOC.bottompoc;

    pMvcCtx->CurrPic.decthispoc = pMvcCtx->CurrPOC.ThisPOC;
    pMvcCtx->CurrPic.decframepoc = pMvcCtx->CurrPOC.framepoc;
    pMvcCtx->CurrPic.dectoppoc = pMvcCtx->CurrPOC.toppoc;
    pMvcCtx->CurrPic.decbottompoc = pMvcCtx->CurrPOC.bottompoc;
	
    //dprint(PRN_ALWS,"thispoc = %d, f_poc = %d, top_poc = %d, b_poc = %d\n", g_CurrPic.thispoc,g_CurrPic.framepoc,g_CurrPic.toppoc,g_CurrPic.bottompoc);
    pMvcCtx->CurrPic.non_existing = 0;
    pMvcCtx->CurrPic.is_long_term = pMvcCtx->CurrPic.is_short_term = 0;
    /*_LOG("g_CurrPic.thispoc = %d, g_CurrPic.frampoc = %d, g_CurrPic.toppoc= %d, g_CurrPic.bottompoc = %d\n",\
    g_CurrPic.thispoc,g_CurrPic.framepoc, g_CurrPic.toppoc, g_CurrPic.bottompoc);*/
    pMvcCtx->CurrPic.stream_base_addr = 0xffffffff;

    if (pMvcCtx->CurrSlice.field_pic_flag)
    {
        pMvcCtx->CurrSlice.field_pic_flag = 1;//just avoid 1+pMvcCtx->CurrSlice.field_pic_flag == 0
    }
    pMvcCtx->CurrPic.pic_height_in_mb = ((pMvcCtx->pCurrSPS->pic_height_in_map_units_minus1+1)*(2 - pMvcCtx->pCurrSPS->frame_mbs_only_flag)) / (1+pMvcCtx->CurrSlice.field_pic_flag);
    pMvcCtx->CurrPic.frame_height_in_mb = ((pMvcCtx->pCurrSPS->pic_height_in_map_units_minus1+1)*(2 - pMvcCtx->pCurrSPS->frame_mbs_only_flag));
    pMvcCtx->CurrPic.pic_width_in_mb = (pMvcCtx->pCurrSPS->pic_width_in_mbs_minus1 + 1);
    pMvcCtx->CurrPic.pic_mbs = pMvcCtx->CurrPic.pic_width_in_mb*pMvcCtx->CurrPic.pic_height_in_mb;
    pMvcCtx->CurrPic.entropy_coding_mode_flag = pMvcCtx->CurrPPS.entropy_coding_mode_flag;
    pMvcCtx->CurrPic.MbaffFrameFlag = ((!pMvcCtx->CurrSlice.field_pic_flag) && (pMvcCtx->pCurrSPS->mb_adaptive_frame_field_flag)); 
    if (pMvcCtx->CurrSlice.nal_ref_idc)
    {
        pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.voidx] = pMvcCtx->CurrSlice.frame_num;
    }

    /* MVC */
    if(pMvcCtx->CurrPic.frame_store != NULL)
    {
        pMvcCtx->CurrPic.frame_store->svc_extension_flag = pMvcCtx->CurrSlice.svc_extension_flag;
    }
    pMvcCtx->CurrPic.view_id = -1;
    if(pMvcCtx->CurrSlice.mvcinfo_flag)
    {
        pMvcCtx->CurrPic.view_id = pMvcCtx->CurrSlice.view_id;
		
        pMvcCtx->CurrPic.anchor_pic_flag = pMvcCtx->CurrSlice.anchor_pic_flag;
        pMvcCtx->CurrPic.inter_view_flag = pMvcCtx->CurrSlice.inter_view_flag;
    }
    pMvcCtx->CurrPic.voidx = pMvcCtx->CurrSlice.voidx;
    /* End */
    // add mark para
    pMvcCtx->CurrPic.pMarkPara = &(pMvcCtx->Mark[pMvcCtx->CurrMarkIdx]);

    //!!! PTS�Ĵ���������ô�򵥡���ʱ���
    pMvcCtx->CurrPic.fs_image.PTS = pMvcCtx->pstExtraData->pts;
    pMvcCtx->CurrPic.fs_image.Usertag = pMvcCtx->pstExtraData->Usertag;
    pMvcCtx->CurrPic.fs_image.DispTime = pMvcCtx->pstExtraData->DispTime;

    pMvcCtx->pstExtraData->pts = (UINT64)(-1);
    dprint(PRN_PTS,"dec_pts: %lld\n",pMvcCtx->CurrPic.fs_image.PTS);
    dprint(PRN_PTS,"dec_usertag: %lld\n",pMvcCtx->CurrPic.fs_image.Usertag);
    pMvcCtx->CurrPic.fs_image.image_width = (pMvcCtx->pCurrSPS->pic_width_in_mbs_minus1 + 1) << 4;
    pMvcCtx->CurrPic.fs_image.image_height = ((pMvcCtx->pCurrSPS->pic_height_in_map_units_minus1+1)*(2 - pMvcCtx->pCurrSPS->frame_mbs_only_flag))<<4;
    /*
    ȱʡֵ
    g_CurrPic.fs_image.format = 0x0  << 14  |
    0x2  << 12  |
    0x0  << 10  |
    0x2  << 8    |
    0x5  << 5   |
    0x0  << 2   |
    0x0;
    */
    dar = (pMvcCtx->CurrSPS.vui_seq_parameters.aspect_ratio & 0x7);
    pMvcCtx->CurrPic.fs_image.format = 0x22A0;
    ChromaFormat = (pMvcCtx->pCurrSPS->chroma_format_idc == 1) ? 0 : 1;  // 4:2:0, 0;  4:0:0, 1;
    pMvcCtx->CurrPic.fs_image.format = ((pMvcCtx->pCurrSPS->vui_seq_parameters.aspect_ratio & 0xff) << 14) |
        ((pMvcCtx->pCurrSPS->vui_seq_parameters.video_format & 0x7) << 5) |
        ((ChromaFormat & 0x7 ) << 2);

	/* set display AspectWidth and AspectHeight */
	SetAspectRatio(&(pMvcCtx->CurrPic.fs_image), (VDEC_DAR_E)dar);
	
    pMvcCtx->CurrPic.fs_image.seq_cnt = pMvcCtx->SeqCnt;

#if 0
    pMvcCtx->CurrPic.fs_image.disp_width = pMvcCtx->pCurrSPS->disp_width;
    pMvcCtx->CurrPic.fs_image.disp_height = pMvcCtx->pCurrSPS->disp_height;
    pMvcCtx->CurrPic.fs_image.disp_center_x = pMvcCtx->pCurrSPS->disp_center_x;
    pMvcCtx->CurrPic.fs_image.disp_center_y = pMvcCtx->pCurrSPS->disp_center_y;
#endif

   if(pMvcCtx->CurrPic.frame_store == NULL)
    {
        dprint(PRN_FATAL,"line %d: CurrPic.frame_store is NULL\n", __LINE__);
        return MVC_ERR;
    }
    pMvcCtx->CurrPic.fs_image.disp_center_x = pMvcCtx->CurrPic.frame_store->fs_image.disp_center_x = pMvcCtx->pCurrSPS->disp_center_x;//pSPS->disp_center_x;
    pMvcCtx->CurrPic.fs_image.disp_center_y = pMvcCtx->CurrPic.frame_store->fs_image.disp_center_y = pMvcCtx->pCurrSPS->disp_center_y;//pSPS->disp_center_y;
    pMvcCtx->CurrPic.fs_image.disp_width    = pMvcCtx->CurrPic.frame_store->fs_image.disp_width    = pMvcCtx->pCurrSPS->disp_width;//pSPS->disp_width;
    pMvcCtx->CurrPic.fs_image.disp_height   = pMvcCtx->CurrPic.frame_store->fs_image.disp_height   = pMvcCtx->pCurrSPS->disp_height;//pSPS->disp_height;
	
//?az
    pMvcCtx->CurrPic.fs_image.is_fld_save = (0 != pMvcCtx->CurrPic.structure);
    pMvcCtx->CurrPic.frame_store->fs_image.format = pMvcCtx->CurrPic.fs_image.format;
    pMvcCtx->CurrPic.frame_store->fs_image.is_fld_save = pMvcCtx->CurrPic.fs_image.is_fld_save;
    pMvcCtx->CurrPic.fs_image.frame_num = pMvcCtx->CurrSlice.frame_num;
    pMvcCtx->CurrPic.fs_image.view_id = pMvcCtx->CurrSlice.view_id;
    pMvcCtx->CurrPic.frame_store->fs_image.frame_num = pMvcCtx->CurrSlice.frame_num;
    pMvcCtx->CurrPic.frame_store->fs_image.view_id = pMvcCtx->CurrSlice.view_id;
	
    pstDecFs = FSP_GetDispPhyFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id);
    if(NULL != pstDecFs)
    {
        FSP_PHY_FS_STORE_TYPE_E eStoreType;
        eStoreType = (pMvcCtx->CurrPic.fs_image.is_fld_save != 0)? FSP_PHY_FS_STORE_TYPE_FIELD : FSP_PHY_FS_STORE_TYPE_FRAME;
        FSP_SetStoreType(pMvcCtx->ChanID, FSP_PHY_FS_TYPE_DECODE, pstDecFs->PhyFsID, eStoreType);
    }

    for (uidxpic=0; uidxpic<4; uidxpic++)
    {
        if (NULL == pMvcCtx->CurrPic.fs_image.p_usrdat[uidxpic] )
        {
            //add usrdata
            if (0 < pMvcCtx->TotalUsrDatNum)
            {
                pMvcCtx->CurrPic.fs_image.p_usrdat[uidxpic] = pMvcCtx->pUsrDatArray[0];
                for (uidxsei=0; uidxsei<pMvcCtx->TotalUsrDatNum-1; uidxsei++)
                {
                    pMvcCtx->pUsrDatArray[uidxsei] = pMvcCtx->pUsrDatArray[uidxsei+1];
                }
                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum-1] = NULL;
                pMvcCtx->TotalUsrDatNum--;
            }
        }
    }
    if (0 < pMvcCtx->TotalUsrDatNum)
    {
        for (uidxpic = pMvcCtx->TotalUsrDatNum; uidxpic > 0; uidxpic--)
        {
            FreeUsdByDec(pMvcCtx->ChanID, pMvcCtx->pUsrDatArray[uidxpic - 1]);
            pMvcCtx->pUsrDatArray[uidxpic - 1]= NULL;
        }
    }
    pMvcCtx->TotalUsrDatNum = 0;
    pMvcCtx->CurrPic.pic_type = 0;  // Ĭ��Islice����ÿ��slice����ʱ����

    MVC_UpdateReflist(pMvcCtx);
    MVC_UpdateLTReflist(pMvcCtx);
    MVC_CalcPicNum(pMvcCtx);

    return MVC_OK;
}

/*!
************************************************************************
* ����ԭ�� :  SINT32 MVC_WritePicMsg(MVC_CTX_S *pMvcCtx)
* �������� :  д��Ϣ��pic para
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_WritePicMsg(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    FSP_LOGIC_FS_S *pstLogicFs;

    dprint(PRN_CTRL,"nal_ref_idc=%d, structure=%d, image_id=%d, pmv_idc=%d\n", 
        pMvcCtx->CurrPic.nal_ref_idc , pMvcCtx->CurrPic.structure, pMvcCtx->CurrPic.fs_image.image_id,
        pMvcCtx->CurrPic.pmv_address_idc);
    pMvcCtx->SliceParaNum = 0;
    pMvcCtx->DecPicPara.Pts = pMvcCtx->CurrPic.fs_image.PTS;
    pMvcCtx->DecPicPara.pFirstSlice = &(pMvcCtx->DecSlicePara[0]);

    pMvcCtx->DecPicPara.imgstride = pMvcCtx->CurrPic.fs_image.image_stride;
    pMvcCtx->DecPicPara.nal_ref_idc = pMvcCtx->CurrPic.nal_ref_idc;
    pMvcCtx->DecPicPara.pic_structure = pMvcCtx->CurrPic.structure;
    pMvcCtx->DecPicPara.mb_adaptive_frame_field_flag = pMvcCtx->pCurrSPS->mb_adaptive_frame_field_flag;
    pMvcCtx->DecPicPara.pic_height_in_mb = pMvcCtx->CurrPic.pic_height_in_mb;
    pMvcCtx->DecPicPara.frame_height_in_mb = pMvcCtx->CurrPic.frame_height_in_mb;
    pMvcCtx->DecPicPara.pic_width_in_mb = pMvcCtx->pCurrSPS->pic_width_in_mbs_minus1+1;
    pMvcCtx->DecPicPara.frame_mbs_only_flag = pMvcCtx->CurrSPS.frame_mbs_only_flag;

    pMvcCtx->DecPicPara.transform_8x8_mode_flag = pMvcCtx->CurrPPS.transform_8x8_mode_flag;
    pMvcCtx->DecPicPara.entropy_coding_mode_flag = pMvcCtx->CurrPPS.entropy_coding_mode_flag;
    pMvcCtx->DecPicPara.chroma_format_idc = pMvcCtx->pCurrSPS->chroma_format_idc;
    pMvcCtx->DecPicPara.constrained_intra_pred_flag = pMvcCtx->CurrPPS.constrained_intra_pred_flag;

    pMvcCtx->DecPicPara.ThisPoc = pMvcCtx->CurrPOC.ThisPOC;   
    pMvcCtx->DecPicPara.TopPoc =  pMvcCtx->CurrPOC.toppoc;
    pMvcCtx->DecPicPara.BottomPoc = pMvcCtx->CurrPOC.bottompoc;

    /* ֡���Լ�PMV��ַ��/id��Ҫ��FSP�ṩ�Ļ���ȥ�� */
    pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id);
    if (pstLogicFs == NULL)
    {
        dprint(PRN_FATAL,"logic MVC_FRAME id(=%d): get LogicFs error!\n", pMvcCtx->CurrPic.frame_store->logic_fs_id);
        return MVC_ERR;
    }
    else if (pstLogicFs->pstDecodeFs == NULL || pstLogicFs->pstDispOutFs == NULL)
    {
        dprint(PRN_FATAL,"phy fs is null: pstDecodeFs = %p, pstDispOutFs = %p\n",
                   pstLogicFs->pstDecodeFs, pstLogicFs->pstDispOutFs);
        return MVC_ERR;
    }

    /* ֡���ַ���Լ���ǰ֡֡��ID */
    FSP_GetDecFsAddrTab(pMvcCtx->ChanID, &pMvcCtx->DecPicPara.ImgSlotNum, 
                        (SINT32*)pMvcCtx->DecPicPara.ImgPhyAddr,(SINT32*)pMvcCtx->DecPicPara.LineNumPhyAddr);
    pMvcCtx->DecPicPara.CurrPicImgIdc = pstLogicFs->pstDecodeFs->PhyFsID;

    /* PMV��ַ���Լ���ǰ֡PMV ID */
    FSP_GetPmvAddrTab(pMvcCtx->ChanID, 
        &pMvcCtx->DecPicPara.PmvSlotNum, (SINT32*)pMvcCtx->DecPicPara.PmvPhyAddr);
    pMvcCtx->DecPicPara.CurrPicPmvIdc = pMvcCtx->CurrPic.pmv_address_idc;
    pMvcCtx->DecPicPara.half_pmvblk_offset = pstLogicFs->HalfPmvOffset;

    /* ��ǰ֡2D��ַ */
    pMvcCtx->DecPicPara.CurrPicImg2DAddr = pstLogicFs->pstDispOutFs->PhyAddr;

    pMvcCtx->DecPicPara.ApcSize = pMvcCtx->APC.size;
    for (i = 0; i < pMvcCtx->APC.size; i++)
    {
        pMvcCtx->DecPicPara.Apc2Idc[i] = pMvcCtx->APC.idc[i];
        pMvcCtx->DecPicPara.Apc2Poc[0][i] = pMvcCtx->APC.poc[0][i];
        pMvcCtx->DecPicPara.Apc2Poc[1][i] = pMvcCtx->APC.poc[1][i];
    }

#ifndef Q_MATRIX_FIXED  // VDH V300R001(3798MV100)��ʼH264����������΢����Ϊ�˼��ݾɰ汾����������˺���ƣ�����Ӧ�ý������hal�㸳ֵ  y00226912 20140514
    if ((0 == pMvcCtx->pCurrSPS->seq_scaling_matrix_present_flag) && (0 == pMvcCtx->CurrPPS.pic_scaling_matrix_present_flag))
    {
        for (i = 0; i < 24; i++)
        {
            pMvcCtx->DecPicPara.Qmatrix[i] = MVC_quant_org[i%4];
        }
        
        for (i = 0; i < 32; i++)
        {
            pMvcCtx->DecPicPara.Qmatrix[i + 24] = MVC_quant8_org[i%16];
        }
    }
    else if (1 == pMvcCtx->CurrPPS.pic_scaling_matrix_present_flag)
    {
        for (i = 0; i < 24; i++)
        {
            pMvcCtx->DecPicPara.Qmatrix[i] = pMvcCtx->CurrPPS.qmatrix4x4[i/4][i%4];
        }
        
        for (i = 0; i < 32; i++)
        {
            pMvcCtx->DecPicPara.Qmatrix[i + 24] = pMvcCtx->CurrPPS.qmatrix8x8[i /16][i%16];
        }
    }
    else
    {
        for (i = 0; i < 24; i++)
        {
            pMvcCtx->DecPicPara.Qmatrix[i] = pMvcCtx->pCurrSPS->qmatrix4x4[i/4][i%4];
        }
        
        for (i = 0; i < 32; i++)
        {
            pMvcCtx->DecPicPara.Qmatrix[i + 24] = pMvcCtx->pCurrSPS->qmatrix8x8[i /16][i%16];
        }
    }       
#else
	if ((0 == pMvcCtx->CurrSPS.seq_scaling_matrix_present_flag) && (0 == pMvcCtx->CurrPPS.pic_scaling_matrix_present_flag))
	{
		for (i = 0; i<24; i++)
		{
            pMvcCtx->DecPicPara.Qmatrix[i] = ((((MVC_quant_org[i%4]>>24) &0xff)<<24) |\
                                              (((MVC_quant_org[i%4]>>8)  &0xff)<<16) |\
                                              (((MVC_quant_org[i%4]>>16) &0xff)<<8 ) |\
                                               ((MVC_quant_org[i%4])     &0xff));
		}
		
		for (i = 0; i<16; i++)
		{
            pMvcCtx->DecPicPara.Qmatrix[2*i + 24] = ((((MVC_quant8_org[(2*i+1)%16]>>16) &0xff)<<24) |\
                                                     (((MVC_quant8_org[(2*i+1)%16])     &0xff)<<16) |\
                                                     (((MVC_quant8_org[(2*i)%16]>>16)   &0xff)<<8 ) |\
                                                     (((MVC_quant8_org[(2*i)%16])       &0xff)));
            
            pMvcCtx->DecPicPara.Qmatrix[(2*i+1) + 24] = ((((MVC_quant8_org[(2*i+1)%16]>>24) &0xff)<<24) |\
                                                         (((MVC_quant8_org[(2*i+1)%16])>>8  &0xff)<<16) |\
                                                         (((MVC_quant8_org[(2*i)%16]>>24)   &0xff)<<8 ) |\
                                                         (((MVC_quant8_org[(2*i)%16])>>8    &0xff)));
		}
	}
	else if (1 == pMvcCtx->CurrPPS.pic_scaling_matrix_present_flag)
	{
		for (i = 0; i<24; i++)
		{
            pMvcCtx->DecPicPara.Qmatrix[i] = ((((pMvcCtx->CurrPPS.qmatrix4x4[i/4][i%4]>>24) &0xff)<<24) |\
                                              (((pMvcCtx->CurrPPS.qmatrix4x4[i/4][i%4]>>8)  &0xff)<<16) |\
                                              (((pMvcCtx->CurrPPS.qmatrix4x4[i/4][i%4]>>16) &0xff)<<8 ) |\
                                               ((pMvcCtx->CurrPPS.qmatrix4x4[i/4][i%4])     &0xff));
		}
        
		for (i = 0; i < 16; i++)
		{
            pMvcCtx->DecPicPara.Qmatrix[2*i + 24] = ((((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16]>>16) &0xff)<<24) |\
                                                     (((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16])     &0xff)<<16) |\
                                                     (((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i)%16]>>16)   &0xff)<<8 ) |\
                                                     (((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i)%16])       &0xff)));
            
            pMvcCtx->DecPicPara.Qmatrix[(2*i+1) + 24] = ((((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16]>>24) &0xff)<<24) |\
                                                         (((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i+1)%16])>>8  &0xff)<<16) |\
                                                         (((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i)%16]>>24)   &0xff)<<8 ) |\
                                                         (((pMvcCtx->CurrPPS.qmatrix8x8[i/8][(2*i)%16])>>8    &0xff)));
		}
	 }
	else
	{
		for (i = 0; i < 24; i++)
		{
            pMvcCtx->DecPicPara.Qmatrix[i] = ((((pMvcCtx->CurrSPS.qmatrix4x4[i/4][i%4]>>24) &0xff)<<24) |\
                                              (((pMvcCtx->CurrSPS.qmatrix4x4[i/4][i%4]>>8)  &0xff)<<16) |\
                                              (((pMvcCtx->CurrSPS.qmatrix4x4[i/4][i%4]>>16) &0xff)<<8 ) |\
                                               ((pMvcCtx->CurrSPS.qmatrix4x4[i/4][i%4])     &0xff));
		}
        
		for (i = 0; i < 16; i++)
		{
            pMvcCtx->DecPicPara.Qmatrix[2*i + 24] = ((((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16]>>16) &0xff)<<24) |\
                                                     (((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16])     &0xff)<<16) |\
                                                     (((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i)%16]>>16)   &0xff)<<8 ) |\
                                                     (((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i)%16])       &0xff)));
            
            pMvcCtx->DecPicPara.Qmatrix[(2*i+1) + 24] = ((((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16]>>24) &0xff)<<24) |\
                                                         (((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i+1)%16])>>8  &0xff)<<16) |\
                                                         (((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i)%16]>>24)   &0xff)<<8 ) |\
                                                         (((pMvcCtx->CurrSPS.qmatrix8x8[i/8][(2*i)%16])>>8    &0xff)));

		}
	}
#endif

    for (i = 0; i < pMvcCtx->APC.size; i++)
    {
        FSP_PHY_FS_STORE_TYPE_E  eStoreType;

        eStoreType = FSP_GetStoreType(pMvcCtx->ChanID, FSP_PHY_FS_TYPE_DECODE, pMvcCtx->APC.idc[i]);
        pMvcCtx->DecPicPara.RefPicType[i] = (eStoreType == FSP_PHY_FS_STORE_TYPE_FRAME)? 0 : 1;
        pMvcCtx->DecPicPara.RefPicFsId[i] = pMvcCtx->APC.idc[i];
    }

    return MVC_OK;
}


VOID MVC_UpdatePicQpInf(SINT32 SlcQp, MVC_STORABLEPIC_S *pCurPic)
{
    pCurPic->MaxQp = (pCurPic->MaxQp < SlcQp) ? SlcQp : pCurPic->MaxQp;
    pCurPic->MinQp = (pCurPic->MinQp > SlcQp) ? SlcQp : pCurPic->MinQp;

    return;
}


/*!
************************************************************************
* ����ԭ�� : VOID MVC_WriteSliceMsg(MVC_CTX_S *pMvcCtx)
* �������� : д��Ϣ��slice para
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
VOID MVC_WriteSliceMsg(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    UINT32 ExistImgIdc_0, ExistImgIdc_1;
    FSP_LOGIC_FS_S *pstLogicFs;
    MVC_DEC_SLICE_PARAM_S *pCurrSlicePara;
    UINT32 CurrSliceSlotNum = pMvcCtx->SliceParaNum;

    pMvcCtx->SliceParaNum++;
    pCurrSlicePara = &(pMvcCtx->DecSlicePara[CurrSliceSlotNum]);
    if (CurrSliceSlotNum < pMvcCtx->MaxSliceNum-1)
    {
        pCurrSlicePara->pNextSlice = &(pMvcCtx->DecSlicePara[CurrSliceSlotNum+1]);
    }
    else
    {
        pCurrSlicePara->pNextSlice = 0;
    }

    for (i = pMvcCtx->pCurrNal->nal_used_segment; ((i < pMvcCtx->pCurrNal->nal_segment) && (i < 2)) ; i++)
    {
        pCurrSlicePara->bitsoffset[i] = pMvcCtx->pCurrNal->stream[i].bitsoffset;
        pCurrSlicePara->valid_bitlen[i] = pMvcCtx->pCurrNal->stream[i].valid_bitlen;
        pCurrSlicePara->bytespos[i] = pMvcCtx->pCurrNal->stream[i].bytespos;
    }
    for (i = pMvcCtx->pCurrNal->nal_segment; i < 2; i++)
    {
        pCurrSlicePara->bitsoffset[i] = 0;
        pCurrSlicePara->valid_bitlen[i] = 0;
        pCurrSlicePara->bytespos[i] = 0;
    }
    pCurrSlicePara->MbaffFrameFlag = pMvcCtx->CurrPic.MbaffFrameFlag;

    pCurrSlicePara->SliceQPy =  (pMvcCtx->CurrPPS.pic_init_qp_minus26 + 26 + pMvcCtx->CurrSlice.slice_qp_delta); // % 52;
    if (0 == CurrSliceSlotNum)
    {
        // if it's the first slice of cur pic, do init
        pMvcCtx->CurrPic.frame_store->frame.MaxQp = pCurrSlicePara->SliceQPy;
        pMvcCtx->CurrPic.frame_store->frame.MinQp = pCurrSlicePara->SliceQPy;
    }
    else
    {
        // otherwise, update
        MVC_UpdatePicQpInf(pCurrSlicePara->SliceQPy, &pMvcCtx->CurrPic.frame_store->frame);
    }
    pCurrSlicePara->cabac_init_idc = pMvcCtx->CurrSlice.cabac_init_idc;
    pCurrSlicePara->num_ref_idx_l1_active_minus1 = pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1;
    pCurrSlicePara->num_ref_idx_l0_active_minus1 = pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1;
    pCurrSlicePara->slice_type = pMvcCtx->CurrSlice.slice_type;
    pCurrSlicePara->first_mb_in_slice = pMvcCtx->CurrSlice.first_mb_in_slice;

    pCurrSlicePara->direct_spatial_mv_pred_flag = pMvcCtx->CurrSlice.direct_spatial_mv_pred_flag;
    pCurrSlicePara->direct_8x8_inference_flag = pMvcCtx->pCurrSPS->direct_8x8_inference_flag;
    pCurrSlicePara->listXsize[0] = pMvcCtx->CurrSlice.listXsize[0];
    pCurrSlicePara->listXsize[1] = pMvcCtx->CurrSlice.listXsize[1];

    if (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type) // B Slice
    {
        pCurrSlicePara->weight_flag = pMvcCtx->CurrPPS.weighted_bipred_idc;
    }
    else if (MVC_P_SLICE == pMvcCtx->CurrSlice.slice_type) // P Slice
    {
        pCurrSlicePara->weight_flag = pMvcCtx->CurrPPS.weighted_pred_flag;
    }
    else
    {
        pCurrSlicePara->weight_flag = 0;
    }
    pCurrSlicePara->chroma_qp_index_offset = pMvcCtx->CurrPPS.chroma_qp_index_offset;
    pCurrSlicePara->second_chroma_qp_index_offset = pMvcCtx->CurrPPS.second_chroma_qp_index_offset;
    pCurrSlicePara->slice_alpha_c0_offset_div2 = pMvcCtx->CurrSlice.slice_alpha_c0_offset_div2;
    pCurrSlicePara->slice_beta_offset_div2 = pMvcCtx->CurrSlice.slice_beta_offset_div2;
    pCurrSlicePara->disable_deblocking_filter_idc = pMvcCtx->CurrSlice.disable_deblocking_filter_idc;

    // �ο��б� & poc
    if (MVC_I_SLICE != pMvcCtx->CurrSlice.slice_type) // P,B Slice  
    {
        /* Ϊ����GAP֡��ַ���ο��� */
        ExistImgIdc_0 = 0;
        ExistImgIdc_1 = 0;
        for (i=0; i<pMvcCtx->CurrSlice.listXsize[0]; i++)
        {
            if (pMvcCtx->pListX[0][i]->frame_store->non_existing == 0)
            {
                pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[0][i]->frame_store->logic_fs_id);
			   if (NULL == pstLogicFs)
			   {
			       dprint(PRN_FATAL, "fuction return value is null,%s %d unknow error!!\n",__FUNCTION__,__LINE__);
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
        for (i=0; i<pMvcCtx->CurrSlice.listXsize[1]; i++)
        {
            if (pMvcCtx->pListX[1][i]->frame_store->non_existing == 0)
            {
                pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[1][i]->frame_store->logic_fs_id);
			  	if (NULL == pstLogicFs)
			   	{
			    	dprint(PRN_FATAL, "fuction return value is null,%s %d unknow error!!\n",__FUNCTION__,__LINE__);
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
        if (0==pMvcCtx->CurrPic.structure) // MVC_FRAME reference
        {
            for (i=0; i<pMvcCtx->CurrSlice.listXsize[0]; i++) 
            {
                pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[0][i]->frame_store->logic_fs_id);
                pCurrSlicePara->declist[0][i].pmv_address_idc = pMvcCtx->pListX[0][i]->frame_store->pmv_address_idc;
                pCurrSlicePara->declist[0][i].is_used = pMvcCtx->pListX[0][i]->frame_store->is_used;
                //pCurrSlicePara->declist[0][i].img_idc = pMvcCtx->pListX[0][i]->frame_store->fs_image.image_id;
                pCurrSlicePara->declist[0][i].img_idc = (pMvcCtx->pListX[0][i]->frame_store->non_existing == 1)? ExistImgIdc_0:
                    (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;
                pCurrSlicePara->declist[0][i].apc_idc = pMvcCtx->pListX[0][i]->frame_store->apc_idc;
                pCurrSlicePara->declist[0][i].poc = pMvcCtx->pListX[0][i]->frame_store->poc;
                pCurrSlicePara->declist[0][i].frame_is_long_term = pMvcCtx->pListX[0][i]->is_long_term;
                pCurrSlicePara->declist[0][i].frame_structure = pMvcCtx->pListX[0][i]->structure;
                pCurrSlicePara->declist[0][i].frame_poc = pMvcCtx->pListX[0][i]->frame_store->frame.poc;
                pCurrSlicePara->declist[0][i].top_poc = pMvcCtx->pListX[0][i]->frame_store->top_field.poc;                 
                pCurrSlicePara->declist[0][i].btm_poc = pMvcCtx->pListX[0][i]->frame_store->bottom_field.poc;
                pCurrSlicePara->declist[0][i].list_structure = 0; //MVC_FRAME
            }

            // list1        
            if (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type)
            {
                for (i=0; i<pMvcCtx->CurrSlice.listXsize[1]; i++)
                {
                    pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[1][i]->frame_store->logic_fs_id);

                    pCurrSlicePara->declist[1][i].pmv_address_idc = pMvcCtx->pListX[1][i]->frame_store->pmv_address_idc;
                    pCurrSlicePara->declist[1][i].is_used = pMvcCtx->pListX[1][i]->frame_store->is_used;                     
                    //pCurrSlicePara->declist[1][i].img_idc = pMvcCtx->pListX[1][i]->frame_store->fs_image.image_id;
                    pCurrSlicePara->declist[0][i].img_idc = (pMvcCtx->pListX[1][i]->frame_store->non_existing == 1)? ExistImgIdc_1:
                        (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;                    
                    pCurrSlicePara->declist[1][i].apc_idc = pMvcCtx->pListX[1][i]->frame_store->apc_idc;           
                    pCurrSlicePara->declist[1][i].poc = pMvcCtx->pListX[1][i]->frame_store->poc;                     
                    pCurrSlicePara->declist[1][i].frame_is_long_term = pMvcCtx->pListX[1][i]->is_long_term;
                    pCurrSlicePara->declist[1][i].frame_structure = pMvcCtx->pListX[1][i]->structure;
                    pCurrSlicePara->declist[1][i].frame_poc = pMvcCtx->pListX[1][i]->frame_store->frame.poc;
                    pCurrSlicePara->declist[1][i].top_poc = pMvcCtx->pListX[1][i]->frame_store->top_field.poc;                 
                    pCurrSlicePara->declist[1][i].btm_poc = pMvcCtx->pListX[1][i]->frame_store->bottom_field.poc;
                    pCurrSlicePara->declist[1][i].list_structure = 0; //MVC_FRAME
                }     
            }
        }
        else // field reference
        {            
            for (i=0; i<pMvcCtx->CurrSlice.listXsize[0]; i++)
            {
                pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[0][i]->frame_store->logic_fs_id);

                pCurrSlicePara->declist[0][i].pmv_address_idc = pMvcCtx->pListX[0][i]->frame_store->pmv_address_idc;
                pCurrSlicePara->declist[0][i].is_used = pMvcCtx->pListX[0][i]->frame_store->is_used;
                //pCurrSlicePara->declist[0][i].img_idc = pMvcCtx->pListX[0][i]->frame_store->fs_image.image_id;
                pCurrSlicePara->declist[0][i].img_idc = (pMvcCtx->pListX[0][i]->frame_store->non_existing == 1)? ExistImgIdc_0:
                    (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;
                pCurrSlicePara->declist[0][i].apc_idc = pMvcCtx->pListX[0][i]->frame_store->apc_idc;         
                pCurrSlicePara->declist[0][i].poc = pMvcCtx->pListX[0][i]->frame_store->poc;
                pCurrSlicePara->declist[0][i].list_structure = pMvcCtx->pListX[0][i]->structure; //field
                pCurrSlicePara->declist[0][i].frame_structure = pMvcCtx->pListX[0][i]->frame_store->frame.structure;
                if (1 == pMvcCtx->pListX[0][i]->structure)
                {
                    //top field
                    pCurrSlicePara->declist[0][i].top_structure = pMvcCtx->pListX[0][i]->structure;  
                    pCurrSlicePara->declist[0][i].top_is_long_term = pMvcCtx->pListX[0][i]->is_long_term;
                    pCurrSlicePara->declist[0][i].top_poc = pMvcCtx->pListX[0][i]->poc;  
                }
                else
                {
                    //btm field
                    pCurrSlicePara->declist[0][i].btm_structure = pMvcCtx->pListX[0][i]->structure;  
                    pCurrSlicePara->declist[0][i].btm_is_long_term = pMvcCtx->pListX[0][i]->is_long_term;
                    pCurrSlicePara->declist[0][i].btm_poc = pMvcCtx->pListX[0][i]->poc;       
                }
            }

            if (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type) // B Slice
            {
                for (i=0; i<pMvcCtx->CurrSlice.listXsize[1]; i++)
                {
                    pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[1][i]->frame_store->logic_fs_id);
                                        
                    pCurrSlicePara->declist[1][i].pmv_address_idc = pMvcCtx->pListX[1][i]->frame_store->pmv_address_idc;
                    pCurrSlicePara->declist[1][i].is_used = pMvcCtx->pListX[1][i]->frame_store->is_used;
                    //pCurrSlicePara->declist[1][i].img_idc = pMvcCtx->pListX[1][i]->frame_store->fs_image.image_id;
                    pCurrSlicePara->declist[1][i].img_idc = (pMvcCtx->pListX[1][i]->frame_store->non_existing == 1)? ExistImgIdc_1:
                        (pstLogicFs == NULL)? 0 :pstLogicFs->pstDecodeFs->PhyFsID;
                    pCurrSlicePara->declist[1][i].apc_idc = pMvcCtx->pListX[1][i]->frame_store->apc_idc;           
                    pCurrSlicePara->declist[1][i].poc = pMvcCtx->pListX[1][i]->frame_store->poc;
                    pCurrSlicePara->declist[1][i].list_structure = pMvcCtx->pListX[1][i]->structure; //field
                    pCurrSlicePara->declist[1][i].frame_structure = pMvcCtx->pListX[1][i]->frame_store->frame.structure;
                    if (1 == pMvcCtx->pListX[1][i]->structure)
                    {
                        //top field
                        pCurrSlicePara->declist[1][i].top_structure = pMvcCtx->pListX[1][i]->structure;  
                        pCurrSlicePara->declist[1][i].top_is_long_term = pMvcCtx->pListX[1][i]->is_long_term;
                        pCurrSlicePara->declist[1][i].top_poc = pMvcCtx->pListX[1][i]->poc;  
                    }
                    else
                    {
                        //btm field
                        pCurrSlicePara->declist[1][i].btm_structure = pMvcCtx->pListX[1][i]->structure;  
                        pCurrSlicePara->declist[1][i].btm_is_long_term = pMvcCtx->pListX[1][i]->is_long_term;
                        pCurrSlicePara->declist[1][i].btm_poc = pMvcCtx->pListX[1][i]->poc;       
                    }
                }
            }     
        }

        for (i = 0; i < 32; i++)
        {
            pCurrSlicePara->Apc2RefIdx[i] = pMvcCtx->APC.RefIdx[i];
        }

        // add weight tab
        pMvcCtx->DecPicPara.weighted_pred_flag = pMvcCtx->CurrPPS.weighted_pred_flag;
        pMvcCtx->DecPicPara.weighted_bipred_idc = pMvcCtx->CurrPPS.weighted_bipred_idc;
        if ((pMvcCtx->CurrPPS.weighted_pred_flag && ( MVC_P_SLICE == pMvcCtx->CurrSlice.slice_type)) ||
            ((1 == pMvcCtx->CurrPPS.weighted_bipred_idc) && ( MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type))) 
        {
            pCurrSlicePara->luma_log2_weight_denom = pMvcCtx->CurrSlice.luma_log2_weight_denom;
            pCurrSlicePara->chroma_log2_weight_denom = pMvcCtx->CurrSlice.chroma_log2_weight_denom;

            // list0            
            // luma weight tab of list0
            for (i = 0; i <= pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1; i++)
            {
                pCurrSlicePara->wp_weight_l0_y[i] = pMvcCtx->CurrSlice.wp_weight_l0_y[i];
                pCurrSlicePara->wp_weight_l0_u[i] = pMvcCtx->CurrSlice.wp_weight_l0_u[i];
                pCurrSlicePara->wp_weight_l0_v[i] = pMvcCtx->CurrSlice.wp_weight_l0_v[i];
                
                pCurrSlicePara->wp_offset_l0_y[i] = pMvcCtx->CurrSlice.wp_offset_l0_y[i];
                pCurrSlicePara->wp_offset_l0_u[i] = pMvcCtx->CurrSlice.wp_offset_l0_u[i];
                pCurrSlicePara->wp_offset_l0_v[i] = pMvcCtx->CurrSlice.wp_offset_l0_v[i];                
            }

            // list1
            if (MVC_B_SLICE  == pMvcCtx->CurrSlice.slice_type) // B Slice
            {
                for (i = 0; i <= pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1; i++)
                {
                    pCurrSlicePara->wp_weight_l1_y[i] = pMvcCtx->CurrSlice.wp_weight_l1_y[i];
                    pCurrSlicePara->wp_weight_l1_u[i] = pMvcCtx->CurrSlice.wp_weight_l1_u[i];
                    pCurrSlicePara->wp_weight_l1_v[i] = pMvcCtx->CurrSlice.wp_weight_l1_v[i];
                    
                    pCurrSlicePara->wp_offset_l1_y[i] = pMvcCtx->CurrSlice.wp_offset_l1_y[i];
                    pCurrSlicePara->wp_offset_l1_u[i] = pMvcCtx->CurrSlice.wp_offset_l1_u[i];
                    pCurrSlicePara->wp_offset_l1_v[i] = pMvcCtx->CurrSlice.wp_offset_l1_v[i];
                }          
            }          
        }        
    }

    pMvcCtx->SlcInfo[CurrSliceSlotNum].pSliceNal = pMvcCtx->pCurrNal;
    pMvcCtx->pCurrNal = NULL;

    return;
}


VOID MVC_StopPicNum(MVC_CTX_S *pMvcCtx)
{
#if 0
    if ((800+1) == pMvcCtx->TotalPicNum)
    {
        dprint(PRN_DBG,"PicNum:\n", pMvcCtx->TotalPicNum-1);
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
* ����ԭ�� : SINT32 MVC_DecVDM(MVC_CTX_S *pMvcCtx)
* �������� : ����vdm����
* ����˵�� :  
* ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_DecVDM(MVC_CTX_S *pMvcCtx)
{
    UINT32 IPBDecMode;

    /* ͳ��֡�� */
    if (pMvcCtx->CurrPic.structure == MVC_FRAME || (pMvcCtx->CurrPic.structure != MVC_FRAME && pMvcCtx->CurrPic.field_pair_flag == 1))
    {
        VDEC_CHAN_STATE_S  *pChanState = &pMvcCtx->pstExtraData->stChanState;
        pMvcCtx->numTotalFrame++;
        pMvcCtx->numTotalFrame = (pMvcCtx->numTotalFrame < pMvcCtx->numOutputFrame)?
            pMvcCtx->numOutputFrame : pMvcCtx->numTotalFrame;

        pChanState->total_frame_num = pMvcCtx->numTotalFrame;
        pChanState->error_frame_num = pMvcCtx->numTotalFrame - pMvcCtx->numOutputFrame;
    }


    //clean up information, clean here for statistic err mb when not start vdm or  reset
    IPBDecMode = (pMvcCtx->advanced_cfg & 0x30000) >> 16;    
    if ( ( pMvcCtx->OldDecMode != IPBDecMode && IPBDecMode != I_MODE ) &&
         ( (pMvcCtx->OldDecMode == I_MODE || pMvcCtx->OldDecMode == DISCARD_MODE) &&
         (IPBDecMode == IP_MODE || IPBDecMode == IPB_MODE || IPBDecMode == DISCARD_B_BF_P_MODE) ) )
    {
        dprint(PRN_CTRL, "----------------- MVC_IMODE -> %d ------------------\n", IPBDecMode);
        pMvcCtx->OldDecMode = IPBDecMode;
        MVC_ClearCurrPic(pMvcCtx);
        MVC_ClearDPB(pMvcCtx,-1);
        return MVC_ERR;  //DPB_ERR
    }

    pMvcCtx->OldDecMode = IPBDecMode;

    switch (pMvcCtx->OldDecMode)
    {
    case DISCARD_MODE:
        MVC_ClearCurrPic(pMvcCtx);
        MVC_ClearDPB(pMvcCtx,-1);
        return MVC_ERR;  //IDR_LOST
    case I_MODE:
        //I dec mode, dec I pic only
        if (0 != pMvcCtx->CurrPic.pic_type )  //P��B pic
        {
            //p(1), b(2) pic, not start vdm dec; all mb not dec calculate to err_level
            pMvcCtx->CurrPic.mode_skip_flag = 1;
            MVC_ClearCurrPic(pMvcCtx);
            return MVC_ERR;
        }
        else  //I pic
        {
            if (0 != pMvcCtx->SliceParaNum)
            {
                if (0 != pMvcCtx->DecSlicePara[0].first_mb_in_slice)
                {
                    pMvcCtx->CurrPic.mode_skip_flag = 1;
                    MVC_ClearCurrPic(pMvcCtx);
                    return MVC_ERR;
                }
            }
        }
        break;
    case IP_MODE:
        if ( (2 == pMvcCtx->CurrPic.pic_type) && (0 == pMvcCtx->CurrPic.nal_ref_idc) )
        {
            //b(2) pic, and not ref pic
            pMvcCtx->CurrPic.mode_skip_flag = 1;
            MVC_ClearCurrPic(pMvcCtx);
            return MVC_ERR;
        }
        break;
    case IPB_MODE:
    case DISCARD_B_BF_P_MODE:    
    default :
        break;
    }
    if (pMvcCtx->PrevPicIsIDR == 1 && pMvcCtx->CurrPic.pic_type == 2)
    {
        pMvcCtx->PrevPicIsIDR = pMvcCtx->CurrPic.is_idr_pic;
        //return MVC_ERR;
    }
    else
    {
        pMvcCtx->PrevPicIsIDR = pMvcCtx->CurrPic.is_idr_pic;
    }

    if (0 == pMvcCtx->SliceParaNum)
    {
        MVC_ClearCurrPic(pMvcCtx);
        return MVC_ERR;
    }
#if 1
	/* �ݴ������ڻع��׼����ʱ��5X6 ��������I ��������ſɽ��B֡���������ᶪ����B,
		�����º�������ع�ʱע�͵�*/

    if ( (DISCARD_B_BF_P_MODE==pMvcCtx->OldDecMode) && (0==pMvcCtx->CurrPic.pic_type) )
    {
         pMvcCtx->PPicFound = 0;
    }
    if ( (2==pMvcCtx->CurrPic.pic_type) && (0==pMvcCtx->PPicFound) && 
       (NOT_DEC_ORDER==pMvcCtx->pstExtraData->s32DecOrderOutput) ) // ����B֡
    {
        MVC_ClearCurrPic(pMvcCtx);
        FSP_SetDisplay(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 0);
        return MVC_ERR;
    }
    else if (1== pMvcCtx->CurrPic.pic_type && 0 == pMvcCtx->CurrPic.view_id)  // base view�ҵ���P֡����Ч
    {
        pMvcCtx->PPicFound = 1;
    }
#endif
    dprint(PRN_CTRL, "***** VDM start, TotalPicNum=%d, pMvcCtx->CurrPic.structure=%d.\n", pMvcCtx->TotalPicNum - 1, pMvcCtx->CurrPic.structure);               
    pMvcCtx->DecPicPara.mbtodec = pMvcCtx->CurrPic.pic_mbs;
    pMvcCtx->DecPicPara.NewPicFlag = 1;
    pMvcCtx->DecPicPara.stream_base_addr = pMvcCtx->CurrPic.stream_base_addr;
    pMvcCtx->DecPicPara.SliceParaNum = pMvcCtx->SliceParaNum;

    if (pMvcCtx->TotalPicNum >= (MVC_START_FRAME+1))
    {
        /* ����I֡, �ϱ��¼� */
        if (0 == pMvcCtx->CurrPic.pic_type)
        {
            SINT32  MVC_GetPicStreamSize(MVC_CTX_S *pMvcCtx);
            REPORT_FIND_IFRAME(pMvcCtx->ChanID, MVC_GetPicStreamSize(pMvcCtx));
        }
        else if (pMvcCtx->DPB.used_size <= 0)
        {
            /* SIMPLE_DPBģʽ�¼�ʹDPBΪ��Ҳǿ�ƽ���, �Ա�֤�����ʹû��I֡����Ҳ�ܼ��� */
            if (pMvcCtx->pstExtraData->s32DecOrderOutput != DEC_ORDER_SIMPLE_DPB)
            {
                MVC_ClearCurrPic(pMvcCtx);
                return MVC_ERR;
            }
        }

        pMvcCtx->pMvcDecParam = &pMvcCtx->DecPicPara;

        if (1 == pMvcCtx->pstExtraData->stDisParams.s32Mode)
        {
            if ((NULL != pMvcCtx->pMvcDecParam) && (2 == pMvcCtx->CurrPic.pic_type))
            {
                pMvcCtx->pMvcDecParam = NULL;
                FSP_SetDisplay(pMvcCtx->ChanID,  pMvcCtx->CurrPic.frame_store->logic_fs_id, 0);
                MVC_ClearCurrPic(pMvcCtx);
                return MVC_ERR;
            }
        }
        else if (2 == pMvcCtx->pstExtraData->stDisParams.s32Mode)
        {
            if ((pMvcCtx->pstExtraData->stDisParams.s32DisNums > 0) && (NULL != pMvcCtx->pMvcDecParam)
                && (2 == pMvcCtx->CurrPic.pic_type))
            {
                pMvcCtx->pMvcDecParam = NULL;
                FSP_SetDisplay(pMvcCtx->ChanID,  pMvcCtx->CurrPic.frame_store->logic_fs_id, 0);
                pMvcCtx->pstExtraData->stDisParams.s32DisNums--;
                MVC_ClearCurrPic(pMvcCtx);
                return MVC_ERR;
            }
        }
        
		pMvcCtx->pMvcDecParam->Compress_en = pMvcCtx->pstExtraData->s32Compress_en;
    }
    return MVC_OK;
}


SINT32 MVC_GetPicStreamSize(MVC_CTX_S *pMvcCtx)
{
    MVC_DEC_SLICE_PARAM_S *pMVCSliceParam;
    SINT32 TotalSize = 0;

    pMVCSliceParam = pMvcCtx->DecPicPara.pFirstSlice;
    while (pMVCSliceParam)
    {
        TotalSize += ((pMVCSliceParam->valid_bitlen[0] + pMVCSliceParam->valid_bitlen[1] + 7) / 8);
        pMVCSliceParam = pMVCSliceParam->pNextSlice;
    }

    return TotalSize;
}


/*!
************************************************************************
*      ����ԭ�� : SINT32 MVC_SliceCheck(MVC_CTX_S *pMvcCtx)
*      �������� : ���ݿ�����˵�״ֻ̬����I֡������Nalu Type���ˣ����ݵ�һ�ν��������ֻ����I֡������Nalu Type����;
*      ����˵�� : VOID
*      ����ֵ   ��DEC_NORMAL, DEC_ERR
************************************************************************
*/
SINT32 MVC_SliceCheck(MVC_CTX_S *pMvcCtx)
{
    UINT32 SliceType;
	UINT32 totalMbMin1;
    pMvcCtx->CurrSlice.first_mb_in_slice = mvc_ue_v(pMvcCtx, "SH: first_mb_in_slice");  
    MVC_SE_ERR_CHECK;

    if (MAX_MB_NUM_IN_PIC <= pMvcCtx->CurrSlice.first_mb_in_slice)
    {
        dprint(PRN_ERROR, "MVC_SliceCheck first_mb_in_slice >= MAX_MB_NUM_IN_PIC error.\n");
        return MVC_ERR; 
    }
    totalMbMin1 = pMvcCtx->CurWidthInMb*pMvcCtx->CurHeightInMb - 1;
	if (pMvcCtx->CurrSlice.first_mb_in_slice > totalMbMin1)
	{
		return MVC_ERR;
	}
	//end
    SliceType = mvc_ue_v(pMvcCtx, "SH: slice_type");
    dprint(PRN_SLICE, "slice type = %d\n", SliceType);
    MVC_SE_ERR_CHECK;

    switch (SliceType)
    {
    case 2:
    case 4:
    case 7:
    case 9:
        pMvcCtx->CurrSlice.slice_type = MVC_I_SLICE;
        break;
    case 0:
    case 3:
    case 5:
    case 8:
        pMvcCtx->CurrSlice.slice_type = MVC_P_SLICE;
        break;
    case 1:
    case 6:
        pMvcCtx->CurrSlice.slice_type = MVC_B_SLICE;
        break;
    default:
        dprint(PRN_ERROR, "slice type = %d, err\n", SliceType);
        return MVC_ERR;
    }

    return MVC_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_PPSSPSCheck(MVC_CTX_S *pMvcCtx)
*      �������� :  У��pps��sps�Ƿ�õ� 
*      ����˵�� :  
*      ����ֵ   �� DEC_NORMAL; DEC_ERR;
************************************************************************
*/
SINT32 MVC_PPSSPSCheck(MVC_CTX_S *pMvcCtx)
{
    MVC_PPS_S *pPPS;
    MVC_SPS_S *pSPS;    
    MVC_SUBSPS_S *pSUBSPS;

    // check pps is get or not
    dprint(PRN_SLICE, "slice  pps id = %d\n", pMvcCtx->CurrSlice.pic_parameter_set_id);

    pPPS = &(pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id]);
    if (!pPPS->is_valid)
    {
        dprint(PRN_ERROR, "pps with this pic_parameter_set_id = %d havn't decode\n", pMvcCtx->CurrSlice.pic_parameter_set_id);        
        return MVC_ERR;
    }

    // check sps is get or not
    if(-1==pMvcCtx->CurrSlice.svc_extension_flag)
    {
        pSPS = &(pMvcCtx->SPS[pPPS->seq_parameter_set_id]);
        if (!pSPS->is_valid)
        {
            dprint(PRN_ERROR, "sps with this pic_parameter_set_id havn't decode\n", pPPS->seq_parameter_set_id);        
            return MVC_ERR;
        }
    }
    else if(0==pMvcCtx->CurrSlice.svc_extension_flag)  //mvc non base view branch
    {
        pSUBSPS = &(pMvcCtx->SUBSPS[pPPS->seq_parameter_set_id]);
        pSPS = &(pSUBSPS->sps);
        if((!pSUBSPS->is_valid)||(!pSPS->is_valid))
        {
            dprint(PRN_ERROR, "subsps with this seq_parameter_set_id = %d havn't decoded\n", pPPS->seq_parameter_set_id);
            return MVC_ERR;
        }
    }
	else
	{
	    pSPS = NULL;
        dprint(PRN_ERROR, "sps but mvc flag %d is wrong\n", pMvcCtx->CurrSlice.svc_extension_flag);        
        return MVC_ERR;
	}

    pMvcCtx->pPPS = pPPS;
    pMvcCtx->pSPS = pSPS;

    return MVC_OK;
}



/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_PPSSPSCheckTmpId(MVC_CTX_S *pMvcCtx, UINT32 tmp_pic_parameter_set_id)
*      �������� :  У��pps��sps�Ƿ�õ� 
*      ����˵�� :  
*      ����ֵ   �� DEC_NORMAL; DEC_ERR;
************************************************************************
*/
SINT32 MVC_PPSSPSCheckTmpId(MVC_CTX_S *pMvcCtx, UINT32 tmp_pic_parameter_set_id)
{
    MVC_PPS_S *pPPS = NULL;
    MVC_SPS_S *pSPS = NULL;    
	
    // check pps is get or not
    dprint(PRN_SLICE, "tmp slice  pps id = %d\n", tmp_pic_parameter_set_id);

    pPPS = &(pMvcCtx->PPS[tmp_pic_parameter_set_id]);
    if (!pPPS->is_valid)
    {
        dprint(PRN_ERROR, "MVC_PPSSPSCheckTmpId: pps with this pic_parameter_set_id = %d havn't decode\n", tmp_pic_parameter_set_id);        
        return MVC_ERR;
    }
	
    if (pPPS->seq_parameter_set_id > (pMvcCtx->MaxSpsNum-1))
    {
        dprint(PRN_ERROR, "MVC_PPSSPSCheckTmpId: seq_parameter_set_id out of range.\n");
        return MVC_ERR;
    }

    // check sps is get or not     
    if(-1==pMvcCtx->CurrSlice.svc_extension_flag)
    {
        pSPS = &(pMvcCtx->SPS[pPPS->seq_parameter_set_id]);
        if (!pSPS->is_valid)
        {
            dprint(PRN_ERROR, "MVC_PPSSPSCheckTmpId: sps with this pic_parameter_set_id = %d havn't decode\n", pPPS->seq_parameter_set_id);
            return MVC_ERR;
        } 
    }

    return MVC_OK;
}


/*!
************************************************************************
*      ����ԭ�� :  UINT32 MVC_IsNewPic(MVC_CTX_S *pMvcCtx)
*      �������� :  �Ƚ�slice �﷨Ԫ�أ�ȷ���Ƿ���new pic
*      ����˵�� :  ��
*      ����ֵ   �� �ȽϽ��
************************************************************************
*/
UINT32 MVC_IsNewPic(MVC_CTX_S *pMvcCtx)
{
    UINT32 result = 0;

    result |= (pMvcCtx->PrevSlice.pic_parameter_set_id != pMvcCtx->CurrSlice.pic_parameter_set_id); 
    result |= (pMvcCtx->PrevSlice.seq_parameter_set_id != pMvcCtx->pCurrSPS->seq_parameter_set_id);
    result |= (pMvcCtx->PrevSlice.frame_num != pMvcCtx->CurrSlice.frame_num); 
    result |= (pMvcCtx->PrevSlice.field_pic_flag != pMvcCtx->CurrSlice.field_pic_flag);
    if (pMvcCtx->PrevSlice.field_pic_flag && pMvcCtx->CurrSlice.field_pic_flag)
    {
        result |= (pMvcCtx->PrevSlice.bottom_field_flag != pMvcCtx->CurrSlice.bottom_field_flag);
    }
    result |= (pMvcCtx->PrevSlice.nal_ref_idc != pMvcCtx->CurrSlice.nal_ref_idc) && ((0==pMvcCtx->PrevSlice.nal_ref_idc) || (0==pMvcCtx->CurrSlice.nal_ref_idc));
    result |= (pMvcCtx->PrevSlice.nal_unit_type != pMvcCtx->CurrSlice.nal_unit_type);
    if (MVC_NALU_TYPE_IDR==pMvcCtx->CurrSlice.nal_unit_type && MVC_NALU_TYPE_IDR==pMvcCtx->PrevSlice.nal_unit_type)
    {
        result |= (pMvcCtx->PrevSlice.idr_pic_id != pMvcCtx->CurrSlice.idr_pic_id);
    }   
    if (!pMvcCtx->pCurrSPS->pic_order_cnt_type) 
    {
        result |= (pMvcCtx->PrevSlice.pic_oder_cnt_lsb          != pMvcCtx->CurrPOC.pic_order_cnt_lsb);
        result |= (pMvcCtx->PrevSlice.delta_pic_oder_cnt_bottom != pMvcCtx->CurrPOC.delta_pic_order_cnt_bottom);
    }
    if (1 == pMvcCtx->pCurrSPS->pic_order_cnt_type) 
    {
        result |= (pMvcCtx->PrevSlice.delta_pic_order_cnt[0] != pMvcCtx->CurrPOC.delta_pic_order_cnt[0]); 
        result |= (pMvcCtx->PrevSlice.delta_pic_order_cnt[1] != pMvcCtx->CurrPOC.delta_pic_order_cnt[1]); 
    }
    
    /* MVC */
    if(pMvcCtx->CurrSlice.mvcinfo_flag)
    {
        result |= (pMvcCtx->PrevSlice.view_id != pMvcCtx->CurrSlice.view_id);
        result |= (pMvcCtx->PrevSlice.anchor_pic_flag != pMvcCtx->CurrSlice.anchor_pic_flag);
        result |= (pMvcCtx->PrevSlice.inter_view_flag != pMvcCtx->CurrSlice.inter_view_flag);
    }	
    
    /* End */
    // �е��������ر���һЩ������˵��������ǵ�Ҫ��һ�в���ͨ��
    result |= (pMvcCtx->CurrSlice.first_mb_in_slice == 0);

    dprint(PRN_SLICE, "new pic flag = %d\n", result);

    return result;
}


/***********************************************************************************
* Function:    mvc_ref_pic_list_reordering(MVC_CTX_S *pMvcCtx)
* Description: ����Reorder����
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL      ���Խ���MMCO����
*              DEC_ERR         �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 mvc_ref_pic_list_reordering(MVC_CTX_S *pMvcCtx)
{
    UINT32 MaxPicNum, i;
    UINT32 max_reorder_num = pMvcCtx->CurrSlice.mvcinfo_flag?5:3;
    //MVC_PPS_S *pPPS = &(pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id]);
    //MVC_SPS_S *pSPS = &(pMvcCtx->SPS[pPPS->seq_parameter_set_id]);
    MVC_SPS_S *pSPS = pMvcCtx->pSPS;
    
    pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l0 = 0;
    pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l1 = 0;
    if (pMvcCtx->CurrSlice.field_pic_flag)
    {
        MaxPicNum = 1 << (pSPS->log2_max_frame_num_minus4 + 5);
    }
    else
    {
        MaxPicNum = 1 << (pSPS->log2_max_frame_num_minus4 + 4);
    }
    
    if ((MVC_P_SLICE == pMvcCtx->CurrSlice.slice_type) || (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type))
    {
        pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l0 = mvc_u_1(pMvcCtx, "SH: ref_pic_list_reordering_flag_l0");
        if (pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l0)
        {
            for (i=0; i < (pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1+2); i++)
            {
                pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i] = mvc_ue_v(pMvcCtx, "SH: reordering_of_pic_nums_idc_l0");
                if ((max_reorder_num < pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i]) || pMvcCtx->SeErrFlg)
                {
                    dprint(PRN_ERROR, "reorder idc l0 = %d, g_SeErrFlag=%d\n", 
                        pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i], pMvcCtx->SeErrFlg);
                    return MVC_ERR;
                }
                if (3 == pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i])
                {
                    break;
                }
                
                if ((0 == pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i]) || (1 == pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i]))
                {
                    pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l0[i] = mvc_ue_v(pMvcCtx, "SH: abs_diff_pic_num_minus1_l0");
                    if ((pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l0[i]>=MaxPicNum) || pMvcCtx->SeErrFlg)
                    {
                        dprint(PRN_ERROR,"abs_diff_pic_num_minus1_l0 = %d, g_SeErrFlag=%d\n", 
                            pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l0[i], pMvcCtx->SeErrFlg);
                        return MVC_ERR;
                    }
                }
                else if (2 == pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i])
                {
                    pMvcCtx->CurrReorder.long_term_pic_idx_l0[i] = mvc_ue_v(pMvcCtx, "SH: long_term_pic_idx_l0");
                }
                else if ((4==pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i]) || (5==pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i]))
                {
                    pMvcCtx->CurrReorder.abs_diff_view_idx_minus1_l0[i] = mvc_ue_v(pMvcCtx, "SH: abs_diff_view_idx_minus1_l0");
                }
            } 
            
            // check the num of pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i] no exceed
            if ((i==(pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1+2)) && (pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i]!=3))
            {
                dprint(PRN_ERROR, "num of idc l0 exceed\n");
                return MVC_ERR;
            }            
        }
    }
    
    if (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l1 = mvc_u_1(pMvcCtx, "SH: ref_pic_list_reordering_flag_l1");
        if (pMvcCtx->CurrReorder.ref_pic_list_reordering_flag_l1)
        {
            for (i=0; i<(pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1+2); i++)
            {
                pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i] = mvc_ue_v(pMvcCtx, "SH: reordering_of_pic_nums_idc_l1");
                if ((max_reorder_num < pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i]) || pMvcCtx->SeErrFlg)
                {
                    dprint(PRN_ERROR, "reorder idc l1 = %d, g_SeErrFlag=%d\n", 
                        pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i], pMvcCtx->SeErrFlg);                
                    return MVC_ERR;
                }
                if (3 == pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i])
                {
                    break;
                }
                
                if ((0 == pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i]) || (1 == pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i]))
                {
                    pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l1[i] = mvc_ue_v(pMvcCtx, "SH: abs_diff_pic_num_minus1_l1");
                    if ((pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l1[i]>=MaxPicNum) || pMvcCtx->SeErrFlg)
                    {
                        dprint(PRN_ERROR,"abs_diff_pic_num_minus1_l1 = %d, g_SeErrFlag=%d\n", 
                            pMvcCtx->CurrReorder.abs_diff_pic_num_minus1_l1[i], pMvcCtx->SeErrFlg);                    
                        return MVC_ERR;
                    }                    
                }
                else if (2==pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i])
                {
                    pMvcCtx->CurrReorder.long_term_pic_idx_l1[i] = mvc_ue_v(pMvcCtx, "SH: long_term_pic_idx_l1");
                }
                else if ((4==pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i]) || (5==pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i]))
                {
                    pMvcCtx->CurrReorder.abs_diff_view_idx_minus1_l1[i] = mvc_ue_v(pMvcCtx, "SH: abs_diff_view_idx_minus1_l1");
                }
                
                // check the num of pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l0[i] no exceed
                if ((i==(pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1+2)) && (pMvcCtx->CurrReorder.reordering_of_pic_nums_idc_l1[i]!=3))
                {
                    dprint(PRN_ERROR, "num of idc l1 exceed\n");
                    return MVC_ERR;             
                }          
            }            
        }
    } 
    MVC_SE_ERR_CHECK;    
    
    return MVC_OK;       
}


/***********************************************************************************
* Function:    mvc_pred_weight_table(MVC_CTX_S *pMvcCtx)
* Description: ����weighted����
* Input:       ����������ʱ����������
* Output:      pMvcCtx->CurrSlice.wp_...   Slice�ṹ�����weighted����
* Return:      DEC_NORMAL           ���Խ���weighted����
*              DEC_ERR              �����Խ���
* Others:      ��
***********************************************************************************/  
SINT32 mvc_pred_weight_table(MVC_CTX_S *pMvcCtx)
{   
    SINT32 y_default_weight;
    SINT32 uv_default_weight = 0;
    UINT32 i;
    
    MVC_PPS_S *pPPS = pMvcCtx->pPPS;
    MVC_SPS_S *pSPS = pMvcCtx->pSPS;
    
    pMvcCtx->CurrSlice.luma_log2_weight_denom = mvc_ue_v(pMvcCtx, "SH: luma_log2_weight_denom");
    y_default_weight = 1 << pMvcCtx->CurrSlice.luma_log2_weight_denom;
    /*1 == chroma_format_idc -- 4:2:0*/
    if (0 != pSPS->chroma_format_idc)
    {
        pMvcCtx->CurrSlice.chroma_log2_weight_denom = mvc_ue_v(pMvcCtx, "SH: chroma_log2_weight_denom");
        uv_default_weight = 1 << pMvcCtx->CurrSlice.chroma_log2_weight_denom;
    }
    if ((7 < pMvcCtx->CurrSlice.luma_log2_weight_denom) || (7 < pMvcCtx->CurrSlice.chroma_log2_weight_denom) || pMvcCtx->SeErrFlg)
    {
        // �������ǯλ����������
        dprint(PRN_ERROR, "WP log2 exceed  and err flag = %d\n", pMvcCtx->SeErrFlg);
        return MVC_ERR;
    }
    
    /* fill wp_parameter table */
    for (i=0; i<= pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1; i++) /* fw */
    {
        if (mvc_u_1(pMvcCtx, "SH: luma_weight_flag_l0"))
        {
            pMvcCtx->CurrSlice.wp_weight_l0_y[i] = mvc_se_v(pMvcCtx, "SH: luma_weight_l0");
            pMvcCtx->CurrSlice.wp_offset_l0_y[i] = mvc_se_v(pMvcCtx, "SH: luma_offset_l0");
        }
        else
        {
            pMvcCtx->CurrSlice.wp_weight_l0_y[i] = y_default_weight;
            pMvcCtx->CurrSlice.wp_offset_l0_y[i] = 0;        
        }
        
        if (0 != pSPS->chroma_format_idc)
        {
            if (mvc_u_1(pMvcCtx, "SH: chroma_weight_flag_l0"))
            {
                pMvcCtx->CurrSlice.wp_weight_l0_u[i] = mvc_se_v(pMvcCtx, "SH: chroma_weight_l0");      
                pMvcCtx->CurrSlice.wp_offset_l0_u[i] = mvc_se_v(pMvcCtx, "SH: chroma_offset_l0");
                pMvcCtx->CurrSlice.wp_weight_l0_v[i] = mvc_se_v(pMvcCtx, "SH: chroma_weight_l0");
                pMvcCtx->CurrSlice.wp_offset_l0_v[i] = mvc_se_v(pMvcCtx, "SH: chroma_offset_l0");
            }
            else
            {
                pMvcCtx->CurrSlice.wp_weight_l0_u[i] = uv_default_weight;        
                pMvcCtx->CurrSlice.wp_offset_l0_u[i] = 0;
                pMvcCtx->CurrSlice.wp_weight_l0_v[i] = uv_default_weight;
                pMvcCtx->CurrSlice.wp_offset_l0_v[i] = 0;        
            }   
        }
        else
        {
            pMvcCtx->CurrSlice.wp_weight_l0_u[i] = 0;        
            pMvcCtx->CurrSlice.wp_offset_l0_u[i] = 0;
            pMvcCtx->CurrSlice.wp_weight_l0_v[i] = 0;
            pMvcCtx->CurrSlice.wp_offset_l0_v[i] = 0;         
        }
    }
    
    if ((1 == pPPS->weighted_bipred_idc) && (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type)) /* bw */
    {
        for (i = 0; i <= pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1; i++)
        {
            if (mvc_u_1(pMvcCtx, "SH: luma_weight_flag_l1"))
            {
                pMvcCtx->CurrSlice.wp_weight_l1_y[i] = mvc_se_v(pMvcCtx, "SH: luma_weight_l1");
                pMvcCtx->CurrSlice.wp_offset_l1_y[i] = mvc_se_v(pMvcCtx, "SH: luma_offset_l1");
            }
            else
            {
                pMvcCtx->CurrSlice.wp_weight_l1_y[i] = y_default_weight;
                pMvcCtx->CurrSlice.wp_offset_l1_y[i] = 0;          
            }
            
            if (0 != pSPS->chroma_format_idc)
            {
                if (mvc_u_1(pMvcCtx, "SH: chroma_weight_flag_l1"))
                {
                    pMvcCtx->CurrSlice.wp_weight_l1_u[i] = mvc_se_v(pMvcCtx, "SH: chroma_weight_l1");      
                    pMvcCtx->CurrSlice.wp_offset_l1_u[i] = mvc_se_v(pMvcCtx, "SH: chroma_offset_l1");
                    pMvcCtx->CurrSlice.wp_weight_l1_v[i] = mvc_se_v(pMvcCtx, "SH: chroma_weight_l1");
                    pMvcCtx->CurrSlice.wp_offset_l1_v[i] = mvc_se_v(pMvcCtx, "SH: chroma_offset_l1");        
                }
                else
                {
                    pMvcCtx->CurrSlice.wp_weight_l1_u[i] = uv_default_weight;
                    pMvcCtx->CurrSlice.wp_offset_l1_u[i] = 0;
                    pMvcCtx->CurrSlice.wp_weight_l1_v[i] = uv_default_weight;
                    pMvcCtx->CurrSlice.wp_offset_l1_v[i] = 0;
                }    
            }
            else
            {
                pMvcCtx->CurrSlice.wp_weight_l1_u[i] = 0;
                pMvcCtx->CurrSlice.wp_offset_l1_u[i] = 0;
                pMvcCtx->CurrSlice.wp_weight_l1_v[i] = 0;
                pMvcCtx->CurrSlice.wp_offset_l1_v[i] = 0;          
            }
        }
    }  
    MVC_SE_ERR_CHECK;  

    return MVC_OK;    
}


/***********************************************************************************
* Function:    MVC_DecMMCO(MVC_CTX_S *pMvcCtx,UINT32 save)
* Description: ����MMCO����
* Input:       ����������ʱ����������
*              save                        �Ƿ���Ҫ����MMCO����
* Return:      DEC_NORMAL                  ���Խ���MMCO����
*              DEC_ERR                     �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 MVC_DecMMCO(MVC_CTX_S *pMvcCtx,UINT32 save)
{
    //MVC_PPS_S *pPPS = pMvcCtx->pPPS;
    MVC_SPS_S *pSPS = pMvcCtx->pSPS;
    MVC_MMCO_S *pMMCO;    
    
    UINT32 MMCONum = 0;    
    UINT32 memory_management_control_operation;
    UINT32 max_long_term_frame_idx_plus1;

    if (save)
    {
        do
        {
            if (MMCONum>=MVC_MAX_MMCO_LEN)
            {
                dprint(PRN_ERROR, "overflow MVC_MAX_MMCO_LEN\n");
                return MVC_ERR;
            }
            
            pMMCO = &(pMvcCtx->Mark[pMvcCtx->CurrMarkIdx].MMCO[MMCONum]);
            
            pMMCO->memory_management_control_operation = mvc_ue_v(pMvcCtx, "SH: memory_management_control_operation");            
            if ((1==pMMCO->memory_management_control_operation) || (3==pMMCO->memory_management_control_operation)) 
            {
                pMMCO->difference_of_pic_nums_minus1 = mvc_ue_v(pMvcCtx, "SH: difference_of_pic_nums_minus1");
            }
            if (2==pMMCO->memory_management_control_operation)
            {
                pMMCO->long_term_pic_num = mvc_ue_v(pMvcCtx, "SH: long_term_pic_num");
            }
            if ((3==pMMCO->memory_management_control_operation) || (pMMCO->memory_management_control_operation==6))
            {
                pMMCO->long_term_frame_idx = mvc_ue_v(pMvcCtx, "SH: long_term_frame_idx");
            }
            if (4==pMMCO->memory_management_control_operation)
            {
                pMMCO->max_long_term_frame_idx_plus1 = mvc_ue_v(pMvcCtx, "SH: max_long_term_frame_idx_plus1");
                if ((pSPS->num_ref_frames<pMMCO->max_long_term_frame_idx_plus1) 
                    || (pMMCO->max_long_term_frame_idx_plus1<1) || pMvcCtx->SeErrFlg)
                {
                    // ������ж���Ҫ��һ���Ŀ���
                    // return DEC_ERR;               
                }
            }

            if ((pMMCO->memory_management_control_operation>6) || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "mmco exceed 6\n");
                return MVC_ERR;
            }      

            MMCONum++;
        }
        while (pMMCO->memory_management_control_operation != 0);
    } 
    else // don't save
    {
        do
        {
            if (MMCONum>=MVC_MAX_MMCO_LEN)
            {
                return MVC_ERR;
            }

            memory_management_control_operation = mvc_ue_v(pMvcCtx, "SH: memory_management_control_operation");
            if ((1==memory_management_control_operation) || (3==memory_management_control_operation)) 
            {
                mvc_ue_v(pMvcCtx, "SH: difference_of_pic_nums_minus1");
            }
            if (2==memory_management_control_operation)
            {
                mvc_ue_v(pMvcCtx, "SH: long_term_pic_num");
            }
            if ((3==memory_management_control_operation) || (6==memory_management_control_operation))
            {
                mvc_ue_v(pMvcCtx, "SH: long_term_frame_idx");
            }
            if (4==memory_management_control_operation)
            {
                max_long_term_frame_idx_plus1 = mvc_ue_v(pMvcCtx, "SH: max_long_term_frame_idx_plus1");
                if ((pSPS->num_ref_frames<max_long_term_frame_idx_plus1) 
                    || (max_long_term_frame_idx_plus1<1) || pMvcCtx->SeErrFlg)
                {
                    // return DEC_ERR;
                }
            }

            if ((memory_management_control_operation>6) || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "mmco exceed 6\n");
                return MVC_ERR;           
            }       
            MMCONum++;
        }
        while (memory_management_control_operation != 0);
    }

    return MVC_OK;
}


/***********************************************************************************
* Function:    mvc_dec_ref_pic_marking(MVC_CTX_S *pMvcCtx)
* Description: ����MMCO����
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL               ���Խ���MMCO����
*              DEC_ERR                  �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 mvc_dec_ref_pic_marking(MVC_CTX_S *pMvcCtx)
{    
    MVC_MARK_S *pMark;
    UINT32 tmpMarkIdx = pMvcCtx->CurrMarkIdx;

    if (pMvcCtx->CurrSlice.new_pic_flag)
    {
        pMvcCtx->CurrMarkIdx= pMvcCtx->CurrMarkIdx ? 0 : 1;
    }
    pMark = &(pMvcCtx->Mark[pMvcCtx->CurrMarkIdx]);
    
    if (pMvcCtx->CurrSlice.new_pic_flag || (0==pMark->is_valid))
    {
        if (MVC_NALU_TYPE_IDR==pMvcCtx->CurrSlice.nal_unit_type)
        {
            pMark->no_output_of_prior_pics_flag = mvc_u_1(pMvcCtx,"SH: no_output_of_prior_pics_flag");
            pMark->long_term_reference_flag = mvc_u_1(pMvcCtx,"SH: long_term_reference_flag");
            dprint(PRN_MARK_MMCO, "IDR and no_output_of_prior_pics_flag = %d\n", pMark->no_output_of_prior_pics_flag);
        }
        else
        {
            pMark->adaptive_ref_pic_marking_mode_flag = mvc_u_1(pMvcCtx, "SH: adaptive_ref_pic_marking_mode_flag");
            dprint(PRN_MARK_MMCO, "adaptive_ref_pic_marking_mode_flag = %d\n", pMark->adaptive_ref_pic_marking_mode_flag);
            if (pMark->adaptive_ref_pic_marking_mode_flag)
            {
                if ( MVC_ERR == MVC_DecMMCO(pMvcCtx, 1) ) // save MMCO
                {
                    pMark->is_valid = 0;
                    pMvcCtx->CurrMarkIdx= tmpMarkIdx;
                    dprint(PRN_ERROR, "MMCO para would save dec err\n");
                    return MVC_ERR;
                }
                pMark->is_valid = 1;
            }
        }
    }
    else
    {
        if (MVC_NALU_TYPE_IDR==pMvcCtx->CurrSlice.nal_unit_type)
        {            
            if (pMark->no_output_of_prior_pics_flag != mvc_u_1(pMvcCtx,"SH: no_output_of_prior_pics_flag")) 
            {
                // some one wrong, but don't know which used
                dprint(PRN_ERROR, "mulitislice diff no_out_of_prior_pics_flag, used first.\n");
                return MVC_ERR;
            }

            if (pMark->long_term_reference_flag != mvc_u_1(pMvcCtx, "SH: long_term_reference_flag"))
            {
                // some one wrong, but don't know which used
                dprint(PRN_ERROR, "mulitislice diff long_term_reference_flag, used first.\n");   
                return MVC_ERR;
            }
        }
        else
        {
            if (pMark->adaptive_ref_pic_marking_mode_flag != mvc_u_1(pMvcCtx, "SH: adaptive_ref_pic_marking_mode_flag"))
            {
                // some one wrong, but don't know which used
                dprint(PRN_ERROR, "mulitislice diff adaptive_ref_pic_marking_mode_flag, used first.\n");
                dprint(PRN_ERROR, "TotalPicNum=%d, SliceNumInPic=%d, TotalNal=%d\n", 
                    pMvcCtx->TotalPicNum, pMvcCtx->SliceNumInPic, pMvcCtx->TotalNal);
                return MVC_ERR;
            }

            if (pMark->adaptive_ref_pic_marking_mode_flag)
            {
                return MVC_DecMMCO(pMvcCtx, 0);  // don't save
            }                   
        }
    }

    return MVC_OK;
}         


/***********************************************************************************
* Function:    ProcessSliceHeader(MVC_CTX_S *pMvcCtx)
* Description: ����Slice Header, �ж��Ƿ���ͼ��
* Input:       �﷨��������������
* Output:      Slice���﷨Ԫ��,�Ƿ���ͼ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_ProcessSliceHeaderFirstPart(MVC_CTX_S *pMvcCtx)
{ 
    UINT32 redundant_pic_cnt;
    UINT32 pic_size;
    UINT32 tmp_pic_parameter_set_id;
	
    MVC_SPS_S *pSPS;
    MVC_PPS_S *pPPS;

    // pMvcCtx->CurrSlice.PTS = g_pCurrNal->PTS;
    
    tmp_pic_parameter_set_id = mvc_ue_v(pMvcCtx, "SH: pic_parameter_set_id");
    if (((pMvcCtx->MaxPpsNum-1) < tmp_pic_parameter_set_id) || pMvcCtx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pps id:%d in sliceheader err\n", tmp_pic_parameter_set_id);
        return MVC_ERR; // ���Կ��ǲ����أ�����pps idǿ��
    }

    if (MVC_OK != MVC_PPSSPSCheckTmpId(pMvcCtx, tmp_pic_parameter_set_id))
    {
        dprint(PRN_ERROR, "PPS or SPS of this slice not valid\n");
        return MVC_ERR; 
    }
    pMvcCtx->CurrSlice.pic_parameter_set_id = tmp_pic_parameter_set_id;
	
    if (MVC_OK != MVC_PPSSPSCheck(pMvcCtx))
    {
        dprint(PRN_ERROR, "MVC_PPSSPSCheck failed, PPS or SPS of this slice not valid\n");
        return MVC_ERR; 
    }
    
    pPPS = pMvcCtx->pPPS;
    pSPS = pMvcCtx->pSPS;

    // ����жϵ��������ܻ�������
    #if 0
    if (((MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type) || (0==pSPS->num_ref_frames)) &&
        (MVC_I_SLICE != pMvcCtx->CurrSlice.slice_type))
    {
        dprint(PRN_ERROR, "recover point but isn't MVC_I_SLICE.\n");
        return MVC_ERR;
    }
    #endif
    pMvcCtx->CurrSlice.frame_num = mvc_u_v(pMvcCtx, pSPS->log2_max_frame_num_minus4+4, "SH: frame_num"); 
    MVC_SE_ERR_CHECK;
    if (MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type)
    {
//        pMvcCtx->CurrSlice.prev_frame_num[pMvcCtx->CurrSlice.view_id] = pMvcCtx->CurrSlice.frame_num;
        if ((0 != pMvcCtx->CurrSlice.frame_num) && (MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type) && (pMvcCtx->CurrSlice.view_id<=0))
        {
            dprint(PRN_ERROR, "IDR NAL but frame_num!=0.\n");
            // return DEC_ERR;
        }
    }

    /* Get picture structure */
    pMvcCtx->CurrSlice.field_pic_flag = 0;
    pMvcCtx->CurrSlice.bottom_field_flag = 0;
    if (!pSPS->frame_mbs_only_flag)
    {
        pMvcCtx->CurrSlice.field_pic_flag = mvc_u_1(pMvcCtx, "SH: field_pic_flag");
        if (pMvcCtx->CurrSlice.field_pic_flag)
        {
            pMvcCtx->CurrSlice.bottom_field_flag = mvc_u_1(pMvcCtx, "SH: bottom_field_flag");
        }
    }

    // check first_mb_in_slice
    pic_size = (pSPS->pic_width_in_mbs_minus1+1) * 
        (pSPS->pic_height_in_map_units_minus1+1) * 
        (!pSPS->frame_mbs_only_flag+1);
    pMvcCtx->CurrSlice.MbaffFrameFlag = (!pMvcCtx->CurrSlice.field_pic_flag && pSPS->mb_adaptive_frame_field_flag);
    if ((1 == pMvcCtx->CurrSlice.field_pic_flag) || (1 == pMvcCtx->CurrSlice.MbaffFrameFlag))
    {
        // max first_mb_in_slice is half pic_size - 1
        if ((pic_size/2) <= pMvcCtx->CurrSlice.first_mb_in_slice)
        {
            dprint(PRN_ERROR, "field first_mb_in_slice bigger than pic size\n");
            return MVC_ERR;
        }
    }
    else
    {
        // max first_mb_in_slice is pic_size - 1
        if (pic_size <= pMvcCtx->CurrSlice.first_mb_in_slice)
        {
            dprint(PRN_ERROR, "MVC_FRAME first_mb_in_slice bigger than pic size\n");
            return MVC_ERR;
        }    
    }

    /* Get idr_pic_id */
    if (MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type)
    {
        pMvcCtx->CurrSlice.idr_pic_id = mvc_ue_v(pMvcCtx, "SH: idr_pic_id");
        if ((65535 <pMvcCtx->CurrSlice.idr_pic_id) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "idr_pic_id bigger than 65535\n");            
            //return MVC_ERR; // ����ȥ���������ǿ��           
        }
    }

    /* Get POC parameters */
    pMvcCtx->CurrPOC.pic_order_cnt_lsb = 0;
    pMvcCtx->CurrPOC.delta_pic_order_cnt_bottom = 0;
    if (0 == pSPS->pic_order_cnt_type)
    {
        pMvcCtx->CurrPOC.pic_order_cnt_lsb = mvc_u_v(pMvcCtx, pSPS->log2_max_pic_order_cnt_lsb_minus4+4, "SH: pic_order_cnt_lsb");
        if (pPPS->pic_order_present_flag && (!pMvcCtx->CurrSlice.field_pic_flag))
        {
            pMvcCtx->CurrPOC.delta_pic_order_cnt_bottom = mvc_se_v(pMvcCtx, "SH: delta_pic_order_cnt_bottom");
        }
    }

    pMvcCtx->CurrPOC.delta_pic_order_cnt[0] = 0;
    pMvcCtx->CurrPOC.delta_pic_order_cnt[1] = 0;
    if ((1==pSPS->pic_order_cnt_type) && (!pSPS->delta_pic_order_always_zero_flag)) 
    {
        pMvcCtx->CurrPOC.delta_pic_order_cnt[0] = mvc_se_v(pMvcCtx, "SH: delta_pic_order_cnt[0]");
        if ((1==pPPS->pic_order_present_flag) && (!pMvcCtx->CurrSlice.field_pic_flag))
        {
            pMvcCtx->CurrPOC.delta_pic_order_cnt[1] = mvc_se_v(pMvcCtx, "SH: delta_pic_order_cnt[1]");
        }
    }
    MVC_SE_ERR_CHECK;

    if (pPPS->redundant_pic_cnt_present_flag)
    {
        redundant_pic_cnt = mvc_ue_v(pMvcCtx, "SH: redundant_pic_cnt");
        if ( (0<redundant_pic_cnt) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "redundant pic not support.\n");     
            REPORT_UNSUPPORT(pMvcCtx->ChanID);
            return MVC_ERR;
        }
    }

    pMvcCtx->CurrSlice.new_pic_flag = MVC_IsNewPic(pMvcCtx); // get new_pic_flag
    if ((0 == pMvcCtx->CurrSlice.new_pic_flag) && (pMvcCtx->CurrSlice.first_mb_in_slice <= pMvcCtx->PrevSlice.first_mb_in_slice))
    {
        //mb in slice err order
        dprint(PRN_ERROR, "first_mb_in_slice in currslice small than prevslice in same pic\n");
        return MVC_ERR;
    }

    return MVC_OK;
}


/***********************************************************************************
* Function:    ProcessSliceHeader(MVC_CTX_S *pMvcCtx)
* Description: ����Slice Header, �ж��Ƿ���ͼ��
* Input:       �﷨��������������
* Output:      Slice���﷨Ԫ��,�Ƿ���ͼ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_ProcessSliceHeaderSecondPart(MVC_CTX_S *pMvcCtx)
{ 
    UINT32 num_ref_idx_active_override_flag;
    MVC_PPS_S *pPPS = pMvcCtx->pPPS;

    if (((pMvcCtx->MaxPpsNum-1) < pMvcCtx->CurrSlice.pic_parameter_set_id) || pMvcCtx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pps id:%d in sliceheader err\n", pMvcCtx->CurrSlice.pic_parameter_set_id);
        return MVC_ERR; // ���Կ��ǲ����أ�����pps idǿ��
    }   

    if (MVC_OK != MVC_PPSSPSCheck(pMvcCtx))
    {
        dprint(PRN_ERROR, "PPS or SPS of this slice not valid\n");
        return MVC_ERR; 
    }


    /* Get mv direct predmode */
    if (MVC_B_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        pMvcCtx->CurrSlice.direct_spatial_mv_pred_flag = mvc_u_1(pMvcCtx, "SH: direct_spatial_mv_pred_flag");
    }

    /* Get num_ref_idx_lx_active_minus1 */
    num_ref_idx_active_override_flag = 0;
    pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1 = pPPS->num_ref_idx_l0_active_minus1;
    pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1 = pPPS->num_ref_idx_l1_active_minus1;

    if ((MVC_P_SLICE==pMvcCtx->CurrSlice.slice_type) || (MVC_B_SLICE==pMvcCtx->CurrSlice.slice_type))
    {
        num_ref_idx_active_override_flag = mvc_u_1(pMvcCtx, "SH: num_ref_idx_override_flag");
        if (num_ref_idx_active_override_flag)
        {
            pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1 = mvc_ue_v(pMvcCtx, "SH: num_ref_idx_l0_active_minus1");
            
            if (MVC_B_SLICE==pMvcCtx->CurrSlice.slice_type)
            {
                pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1 = mvc_ue_v(pMvcCtx, "SH: num_ref_idx_l1_active_minus1");
            }
        }

        if (0 == pMvcCtx->CurrSlice.field_pic_flag)
        {
            if ((15 < pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1)
                || ((15 <pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1) && (MVC_B_SLICE==pMvcCtx->CurrSlice.slice_type))
                || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "MVC_FRAME num_ref_idx_lx_active_minus1 exceed\n");
                
                // ���Կ���ǯλȻ��ǿ��
                return MVC_ERR;
            }
        }
        else
        {
            if ((31<pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1)
                || ((31<pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1) && (MVC_B_SLICE==pMvcCtx->CurrSlice.slice_type))
                || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "field num_ref_idx_lx_active_minus1 exceed\n");
                
                // ���Կ���ǯλȻ��ǿ��
                return MVC_ERR;
            } 
        }
    } 

    if (MVC_I_SLICE == pMvcCtx->CurrSlice.slice_type)
    {
        pMvcCtx->CurrSlice.num_ref_idx_l0_active_minus1 = 32; // ԭ���Ǹ�ֵ-1;
    }
    if (MVC_B_SLICE != pMvcCtx->CurrSlice.slice_type)
    {
        pMvcCtx->CurrSlice.num_ref_idx_l1_active_minus1 = 32; // ԭ���Ǹ�ֵ-1;
    }

    /* Get mvc_ref_pic_list_reordering parameters */
    if (MVC_OK != mvc_ref_pic_list_reordering(pMvcCtx))
    {
        dprint(PRN_ERROR, "reordering para dec err\n");    
        return MVC_ERR; 
    }

    /* Get mvc_pred_weight_table */
    pMvcCtx->CurrSlice.apply_weights_flag = ((pPPS->weighted_pred_flag && (MVC_P_SLICE==pMvcCtx->CurrSlice.slice_type)) ||
        ((1==pPPS->weighted_bipred_idc) && (MVC_B_SLICE==pMvcCtx->CurrSlice.slice_type)));
    dprint(PRN_SLICE, "apply_weights_flag=%d\n", pMvcCtx->CurrSlice.apply_weights_flag);
    pMvcCtx->CurrSlice.luma_log2_weight_denom = 0;
    pMvcCtx->CurrSlice.chroma_log2_weight_denom = 0;

    if (pMvcCtx->CurrSlice.apply_weights_flag)
    {
        if (MVC_OK != mvc_pred_weight_table(pMvcCtx))
        {
            dprint(PRN_ERROR, "wpt dec err\n");
            return MVC_ERR;
        }      
    }

    /* Read MMCO MVC_Marking parameters */
    if (pMvcCtx->CurrSlice.nal_ref_idc)
    {
        if (MVC_OK != mvc_dec_ref_pic_marking(pMvcCtx))
        {
            dprint(PRN_ERROR, "mark para dec err\n");
            return MVC_ERR; 
        }
    }

    /* Get cabac_init_idc */
    pMvcCtx->CurrSlice.cabac_init_idc = 0;
    if (pPPS->entropy_coding_mode_flag && ((MVC_P_SLICE==pMvcCtx->CurrSlice.slice_type) || (MVC_B_SLICE==pMvcCtx->CurrSlice.slice_type)))
    {
        pMvcCtx->CurrSlice.cabac_init_idc = mvc_ue_v(pMvcCtx, "SH: cabac_init_idc");
        if ((3<=pMvcCtx->CurrSlice.cabac_init_idc) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "cabac_init_idc bigger than 3\n");
            // ���Բ²�һ��ֵ���¼�������
            return MVC_ERR;
        }
    }

    /* Get slice_qp_delta */
    pMvcCtx->CurrSlice.slice_qp_delta = mvc_se_v(pMvcCtx, "SH: slice_qp_delta");

    /* Get deblocking filter control parameters */
    pMvcCtx->CurrSlice.disable_deblocking_filter_idc = 0;
    pMvcCtx->CurrSlice.slice_alpha_c0_offset_div2 = 0;
    pMvcCtx->CurrSlice.slice_beta_offset_div2 = 0;
    if (pPPS->deblocking_filter_control_present_flag)
    {
        pMvcCtx->CurrSlice.disable_deblocking_filter_idc = mvc_ue_v(pMvcCtx, "SH: disable_deblocking_filter_idc");
        if ((2 < pMvcCtx->CurrSlice.disable_deblocking_filter_idc) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "disable_deblocking_filter_idc dec err\n");

            // ���Կ������ǿ��
            return MVC_ERR;     
        }
        if (1 != pMvcCtx->CurrSlice.disable_deblocking_filter_idc)
        {
            pMvcCtx->CurrSlice.slice_alpha_c0_offset_div2 = mvc_se_v(pMvcCtx, "SH: slice_alpha_c0_offset_div2");
            if ((-6 > pMvcCtx->CurrSlice.slice_alpha_c0_offset_div2) || (6 <pMvcCtx->CurrSlice.slice_alpha_c0_offset_div2) || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "slice_alpha_c0_offset_div2 dec err\n");

                // ����ǯλǿ��
                return MVC_ERR;     
            }
            pMvcCtx->CurrSlice.slice_beta_offset_div2 = mvc_se_v(pMvcCtx, "SH: slice_beta_offset_div2");
            if ((-6 > pMvcCtx->CurrSlice.slice_beta_offset_div2) || (6 < pMvcCtx->CurrSlice.slice_beta_offset_div2) || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "slice_beta_offset_div2 dec err\n");
                
                // ����ǯλǿ��
                return MVC_ERR;     
            }
        }
    }

    return MVC_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  MVC_ExitSlice (MVC_CTX_S *pMvcCtx)
*   �������� :  ����ǰһ��slice�Ĳ���, Ϊ������ȷ�жϳ�new_pic_flag
*   ����˵�� :  ��
*   ����ֵ   �� ��
************************************************************************
*/
VOID MVC_ExitSlice(MVC_CTX_S *pMvcCtx)
{
    MVC_SPS_S *sps;
    MVC_PPS_S *pps;    
    
    pps = &(pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id]);
    sps = pMvcCtx->pCurrSPS;
    
    pMvcCtx->PrevSlice.pic_parameter_set_id = pMvcCtx->CurrSlice.pic_parameter_set_id;     
    
    pMvcCtx->PrevSlice.seq_parameter_set_id = sps->seq_parameter_set_id;
    pMvcCtx->PrevSlice.first_mb_in_slice = pMvcCtx->CurrSlice.first_mb_in_slice;
    pMvcCtx->PrevSlice.frame_num = pMvcCtx->CurrSlice.frame_num; 
    pMvcCtx->PrevSlice.field_pic_flag = pMvcCtx->CurrSlice.field_pic_flag; 
    if (pMvcCtx->CurrSlice.field_pic_flag)
    {
        pMvcCtx->PrevSlice.bottom_field_flag = pMvcCtx->CurrSlice.bottom_field_flag;
    }
    pMvcCtx->PrevSlice.nal_ref_idc = pMvcCtx->CurrSlice.nal_ref_idc;
    pMvcCtx->PrevSlice.nal_unit_type = pMvcCtx->CurrSlice.nal_unit_type;
    if (MVC_NALU_TYPE_IDR==pMvcCtx->CurrSlice.nal_unit_type)
    {
        pMvcCtx->PrevSlice.idr_pic_id = pMvcCtx->CurrSlice.idr_pic_id;
    }  
    if (!sps->pic_order_cnt_type)
    {
        pMvcCtx->PrevSlice.pic_oder_cnt_lsb          = pMvcCtx->CurrPOC.pic_order_cnt_lsb; 
        pMvcCtx->PrevSlice.delta_pic_oder_cnt_bottom = pMvcCtx->CurrPOC.delta_pic_order_cnt_bottom; 
    }
    if (1 == sps->pic_order_cnt_type)
    {
        pMvcCtx->PrevSlice.delta_pic_order_cnt[0] = pMvcCtx->CurrPOC.delta_pic_order_cnt[0];
        pMvcCtx->PrevSlice.delta_pic_order_cnt[1] = pMvcCtx->CurrPOC.delta_pic_order_cnt[1]; 
    }
    if(1==pMvcCtx->CurrSlice.mvcinfo_flag)
    {
        pMvcCtx->PrevSlice.view_id = pMvcCtx->CurrSlice.view_id;
        pMvcCtx->PrevSlice.anchor_pic_flag = pMvcCtx->CurrSlice.anchor_pic_flag;
        pMvcCtx->PrevSlice.inter_view_flag = pMvcCtx->CurrSlice.inter_view_flag;
    }
    pMvcCtx->TotalSlice++;
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_PicTypeStatistic(MVC_CTX_S *pMvcCtx)
*      �������� :  
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
VOID MVC_PicTypeStatistic(MVC_CTX_S *pMvcCtx)
{
    switch (pMvcCtx->CurrSlice.slice_type)
    {
    case MVC_B_SLICE:
        pMvcCtx->CurrPic.pic_type = 2;
        pMvcCtx->CurrPic.is_idr_pic = 0;
        break;
    case MVC_P_SLICE:
        if (2 != pMvcCtx->CurrPic.pic_type)
        {
            pMvcCtx->CurrPic.pic_type = 1;
        }
        pMvcCtx->CurrPic.is_idr_pic = 0;
        break;
    case MVC_I_SLICE:
        if (MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type)
        {
            pMvcCtx->CurrPic.is_idr_pic = 1;
        }
        else
        {
            pMvcCtx->CurrPic.is_idr_pic = 0;
        }
        break;
    default :
        break;
    }
}


/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_CalcStreamBits(MVC_CTX_S *pMvcCtx)
*      �������� :  
*      ����˵�� :  
*      ����ֵ   �� 
************************************************************************
*/
VOID MVC_CalcStreamBits(MVC_CTX_S *pMvcCtx)
{
    UINT32 code_len_byte;
    UINT32 i;

#ifdef MVC_PRINT_STREAM
    UINT8 *buf;
    UINT8  i_stream_printf;
    SINT32 trail_zeros;
#endif

    if (pMvcCtx->CurrPPS.entropy_coding_mode_flag)
    {
        // cabac, byte_aligned
        pMvcCtx->pCurrNal->nal_bitoffset = (pMvcCtx->pCurrNal->nal_bitoffset + 7) & (~7);
        // stop one is valide bits
        pMvcCtx->pCurrNal->nal_trail_zero_bit_num -= 1;
    }

    // calculate bytes used befor curr segment
    code_len_byte = 0;
    for (i=0; i<(UINT32)pMvcCtx->pCurrNal->nal_used_segment; i++)
    {
        code_len_byte += pMvcCtx->pCurrNal->stream[i].bitstream_length;
    }
    // curr segment bitsoffset and so on
    dprint(PRN_STREAM, "pMvcCtx->pCurrNal->nal_bitoffset = %d; code_len_byte = %d; bitstream_length = %d; pMvcCtx->pCurrNal->nal_trail_zero_bit_num = %d\n", 
        pMvcCtx->pCurrNal->nal_bitoffset, code_len_byte, pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bitstream_length, pMvcCtx->pCurrNal->nal_trail_zero_bit_num);
    // end
    pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bitsoffset = pMvcCtx->pCurrNal->nal_bitoffset & 7;
    pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bytespos = (pMvcCtx->pCurrNal->nal_bitoffset / 8 - code_len_byte) + pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].stream_phy_addr;
    pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].valid_bitlen = pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bitstream_length * 8 - (pMvcCtx->pCurrNal->nal_bitoffset - code_len_byte * 8);
    while ((pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bytespos & 3) != 0)
    {
        pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bitsoffset += 8;
        pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bytespos--;
    }
    
    dprint(PRN_STREAM, "stream[%d]: bitsoffset = %d; valid_bitlen = %d; bytespos = %x\n", 
        i, pMvcCtx->pCurrNal->stream[i].bitsoffset, pMvcCtx->pCurrNal->stream[i].valid_bitlen, pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].bytespos);

    // last sement sub trailing zeros, when cavlc sun stop one 
    if ((pMvcCtx->pCurrNal->nal_segment < 1) || (pMvcCtx->pCurrNal->nal_segment > 2))
    {
        dprint(PRN_FATAL, "nal_segment = %d is not expected value\n", pMvcCtx->pCurrNal->nal_segment);
        return;
    }
    // other segment bitsoffset and so on
    for (i=pMvcCtx->pCurrNal->nal_used_segment+1; i<pMvcCtx->pCurrNal->nal_segment; i++)
    {
        pMvcCtx->pCurrNal->stream[i].bitsoffset = 0;
        pMvcCtx->pCurrNal->stream[i].bytespos = pMvcCtx->pCurrNal->stream[i].stream_phy_addr;
        pMvcCtx->pCurrNal->stream[i].valid_bitlen = pMvcCtx->pCurrNal->stream[i].bitstream_length * 8;
        while ((pMvcCtx->pCurrNal->stream[i].bytespos & 3) != 0)
        {
            pMvcCtx->pCurrNal->stream[i].bitsoffset += 8;
            pMvcCtx->pCurrNal->stream[i].bytespos--; 
        }
        dprint(PRN_STREAM, "stream[%d]: bitsoffset = %d; valid_bitlen = %d\n",
            i, pMvcCtx->pCurrNal->stream[i].bitsoffset, pMvcCtx->pCurrNal->stream[i].valid_bitlen);
    }

    
    pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_segment-1].valid_bitlen -= pMvcCtx->pCurrNal->nal_trail_zero_bit_num;

    // find stream_base_addr
    for (i=pMvcCtx->pCurrNal->nal_used_segment; i<pMvcCtx->pCurrNal->nal_segment; i++)
    {
        if (pMvcCtx->pCurrNal->stream[i].bytespos < pMvcCtx->CurrPic.stream_base_addr)
        {
            pMvcCtx->CurrPic.stream_base_addr = pMvcCtx->pCurrNal->stream[i].bytespos;
        }
    }

    // fprintf stream code, it must delete when decode
#ifdef PRINT_STREAM
    if (pMvcCtx->TotalPicNum >= (MVC_START_FRAME))
    {
        for (i=pMvcCtx->pCurrNal->nal_used_segment; i<pMvcCtx->pCurrNal->nal_segment; i++)
        {
            if (i == (pMvcCtx->pCurrNal->nal_segment-1))
            {
                trail_zeros = pMvcCtx->pCurrNal->nal_trail_zero_bit_num;
            }
            else
            {
                trail_zeros = 0;
            }

            buf = pMvcCtx->pCurrNal->stream[i].streamBuffer + (pMvcCtx->pCurrNal->stream[i].bytespos - pMvcCtx->pCurrNal->stream[i].stream_phy_addr);
            i_stream_printf = 0;
            while (buf < (pMvcCtx->pCurrNal->stream[i].streamBuffer + pMvcCtx->pCurrNal->stream[i].bitstream_length - trail_zeros / 8))
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
*      ����ԭ�� :  SINT32 MVC_DecSlice(MVC_CTX_S *pMvcCtx)
*      �������� :  ���һ��������nal��������ַ�����ȡ�������β0��������nal��
*      ����˵�� :  ���������ַ�������ַ������(�϶�������β0��stopbit)
*      ����ֵ   �� ��������
************************************************************************
*/
SINT32 MVC_DecSlice(MVC_CTX_S *pMvcCtx)
{
    SINT32 ret;
    UINT32 i;
    UINT32 Size0, Size1, Size, PrevNalSize;
	UINT8  *Addr0, *Addr1, *PrevNalAddr;
    SINT32 TotalMbs, MaxStreamThr;

    dprint(PRN_DBG, "\nSlice nal or IDR nal = %d\n", pMvcCtx->TotalNal - 1);

    pMvcCtx->CurrSlice.nal_unit_type = pMvcCtx->pCurrNal->nal_unit_type;
    pMvcCtx->CurrSlice.nal_ref_idc = pMvcCtx->pCurrNal->nal_ref_idc;

    ret = MVC_ProcessSliceHeaderFirstPart(pMvcCtx);
    if (MVC_OK != ret)
    {
        dprint(PRN_ERROR, "sliceheader dec err\n");
        REPORT_STRMERR(pMvcCtx->ChanID, MVC_SLICE_HEADER_ERR);
        MVC_ClearCurrSlice(pMvcCtx);
        return MVC_ERR; 
    }

    ret = MVC_ProcessSliceHeaderSecondPart(pMvcCtx);
    if (MVC_OK != ret)
    {
        dprint(PRN_ERROR, "sliceheader dec err\n");
        REPORT_STRMERR(pMvcCtx->ChanID, MVC_SLICE_HEADER_ERR);
        MVC_ClearCurrSlice(pMvcCtx);
        return MVC_ERR; 
    }

    /* ��Ӷڻ��������������ƶ�����vdm */
    // �ȼ�����ͷ���������������Ǵ�����������������������ռ��buffer�ĳ���, 
    // ����ͷ������������ʹ�м����ɰ��ͷŵ�Ҳ��������������
    Size = 0;
    PrevNalAddr = NULL;
    PrevNalSize = 0;
    for (i = 0; i < pMvcCtx->SliceParaNum; i++)
    {
        Addr0 =  pMvcCtx->SlcInfo[i].pSliceNal->stream[0].streamBuffer;
        Size0 = ( NULL != Addr0 )? pMvcCtx->SlcInfo[i].pSliceNal->stream[0].bitstream_length: 0;
        Addr1 =  pMvcCtx->SlcInfo[i].pSliceNal->stream[1].streamBuffer;
        Size1 = ( NULL != Addr1 )? pMvcCtx->SlcInfo[i].pSliceNal->stream[1].bitstream_length: 0;

        if ( NULL != PrevNalAddr )
        {
            Size += (PrevNalAddr < Addr0)? ((Addr0 + Size0)- (PrevNalAddr+PrevNalSize)): Size0;
            Size += Size1;
        }
        else
        {
            Size += Size0 + Size1;
        }

        PrevNalAddr = ( NULL != Addr1 )? Addr1 : Addr0;
        PrevNalSize = ( NULL != Addr1 )? Size1 : Size0;
    }
    pMvcCtx->MaxBytesReceived = Size;

    //pMvcCtx->MaxBytesReceived��ֵ֮ǰΪ��ֵMVC_MAX_BYTES_START���ָ�Ϊw*h*1.5/3��z56361,20110428
    /* z56361, 2011-11-1, Q�ܷ���"ʯ��"��������Ϊ������ֵ̫С������������, ���Ըĳɱ���͸���ֱ��ò�ͬ����ֵ. */
    TotalMbs = pMvcCtx->CurrPic.pic_width_in_mb * pMvcCtx->CurrPic.pic_height_in_mb;
    if ( TotalMbs > 1620)
    {
        MaxStreamThr = TotalMbs * 128;
    }
    else
    {
        /* z6361,2011-11-23, ��������5x6����ʱ����TotalMbs*256��Ϊ��ֵ��Ȼ̫С����������. �ڳ�Сͼ����ʱ������ֵ
           Ӧ��Ӧ�Ӵ�ֻҪ��֤buf��ȫ����ֵ�����ܴ���ܼ�������. */
        MaxStreamThr = MAX(16*1024, (TotalMbs * 256 * 3 / 2));
    }

    if ( (pMvcCtx->SliceParaNum >= pMvcCtx->MaxSliceNum) || (pMvcCtx->MaxBytesReceived > MaxStreamThr) )
    {
        dprint(PRN_FATAL, "Start Reason: SliceParaNum, MaxBytesReceived = %d, %d(thr=%d)\n", 
               pMvcCtx->SliceParaNum, pMvcCtx->MaxBytesReceived, MaxStreamThr);
        if (pMvcCtx->SliceParaNum >= pMvcCtx->MaxSliceNum)
        {
            REPORT_SLICE_NUM_OVER(pMvcCtx->ChanID, pMvcCtx->SliceParaNum, pMvcCtx->MaxSliceNum);
        }
        dprint(PRN_ERROR, "Too many slice or bitstream, err!\n");
        MVC_ClearCurrPic(pMvcCtx);
        return MVC_ERR; 
    }
    else if (pMvcCtx->CurrSlice.new_pic_flag)
    {
        dprint(PRN_CTRL, "Start Reason: new_pic_flag\n");
        pMvcCtx->SliceNumInPic = 0;
        pMvcCtx->TotalPicNum++;
        if (0 == pMvcCtx->CurrPic.structure)
        {
            pMvcCtx->TotalFrameNum++;
        }
        else
        {
            pMvcCtx->TotalFieldNum++;
        }

        ret = MVC_InitPic(pMvcCtx);
        if(MVC_NOTDEC == ret)
        {
            return ret;
        }
        else if (MVC_OK != ret)
        {
            dprint(PRN_ERROR, "init pic err, find next recover point or next valid sps, pps, or exit\n");
            return MVC_ERR;
        }

        MVC_WritePicMsg(pMvcCtx);
    }

    MVC_PicTypeStatistic(pMvcCtx);

    ret = MVC_DecList(pMvcCtx);
    if (MVC_OK != ret)
    {
        dprint(PRN_ERROR,"MVC_DecList error, ret=%d\n", ret);
        return MVC_ERR;
    }

    ret = MVC_IsRefListWrong(pMvcCtx);
    if (MVC_OK != ret)
    {
        dprint(PRN_ERROR, "dec list err.\n");
        MVC_ClearCurrSlice(pMvcCtx);
        return MVC_ERR;
    }

    MVC_CalcStreamBits(pMvcCtx);

    MVC_WriteSliceMsg(pMvcCtx);
    MVC_ExitSlice(pMvcCtx);

    pMvcCtx->AllowStartDec = 1;
    pMvcCtx->SliceNumInPic++;

    return MVC_OK;    
}


/***********************************************************************************
* Function:    VOID MVC_Scaling_List(MVC_CTX_S *pMvcCtx, SINT32 *scalingList, SINT32 sizeOfScalingList, UINT32 *UseDefaultScalingMatrix)
* Description: ������������ƴλ
* Input:         ��������ָ�룬�������С��ʹ��ȱʡָ��
* Output:      ȱʡʹ�ñ�ָ�븳ֵ
* Return:      ��
* Others:      ��
***********************************************************************************/ 
VOID MVC_Scaling_List(MVC_CTX_S *pMvcCtx, UINT32 *scalingList_Org, UINT32 *scalingList, SINT32 sizeOfScalingList, UINT32 *UseDefaultScalingMatrix)
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
        scanj = (sizeOfScalingList == 16) ? MVC_g_ZZ_SCAN[j] : MVC_g_ZZ_SCAN8[j];  
        if (nextScale!=0)
        {
            delta_scale = mvc_se_v (pMvcCtx, "   : delta_sl   ");
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
* Function:    MVC_DecPPS(MVC_CTX_S *pMvcCtx)
* Description: ����PPS, ������ݴ���
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_DecPPS(MVC_CTX_S *pMvcCtx)
{
    static MVC_PPS_S pps_tmp; 
    UINT32 pic_parameter_set_id;
    UINT32 seq_parameter_set_id;

    pic_parameter_set_id = mvc_ue_v(pMvcCtx, "PPS: pic_parameter_set_id");
    if (pic_parameter_set_id > (pMvcCtx->MaxPpsNum-1))
    {
        dprint(PRN_ERROR, "pic_parameter_set_id(%d) out of range.\n", pic_parameter_set_id);    
        if (pic_parameter_set_id>(pMvcCtx->MaxPpsNum-1))
        {
            REPORT_PPS_NUM_OVER(pMvcCtx->ChanID, pic_parameter_set_id, pMvcCtx->MaxPpsNum);
        }
        // ���Կ���ǯλǿ��
        return MVC_ERR;
    }
    
    seq_parameter_set_id = mvc_ue_v(pMvcCtx, "PPS: seq_parameter_set_id");
    if (seq_parameter_set_id > (pMvcCtx->MaxSpsNum-1))
    {
        dprint(PRN_ERROR, "seq_parameter_set_id out of range.\n");        
        // ���Կ���ǯλǿ��, ǯλ֮��ע��sps�����Ե��ж�
        return MVC_ERR;
    }
    /*else if (!(pMvcCtx->SPS[seq_parameter_set_id].is_valid))
    {
        dprint(PRN_ERROR, "SPS haven't decode.\n");    
        return MVC_ERR;
    }*/  
    
    if (pMvcCtx->PPS[pic_parameter_set_id].is_valid)
    {
        pps_tmp.pic_parameter_set_id = pic_parameter_set_id;
        pps_tmp.seq_parameter_set_id = seq_parameter_set_id;
        
        if (MVC_OK != MVC_ProcessPPS(pMvcCtx, &pps_tmp))
        {
            dprint(PRN_ERROR, "PPS decode error line: %d.\n", __LINE__); 
            return MVC_ERR;
        }

        if (MVC_OK != MVC_PPSEqual(&pps_tmp, &(pMvcCtx->PPS[pic_parameter_set_id])))
        {
            pps_tmp.is_refreshed = 1;
            pps_tmp.is_valid = 1;
            memmove(&(pMvcCtx->PPS[pic_parameter_set_id]), &pps_tmp, sizeof(MVC_PPS_S));      
        }
    }
    else
    {
        pMvcCtx->PPS[pic_parameter_set_id].pic_parameter_set_id = pic_parameter_set_id;
        pMvcCtx->PPS[pic_parameter_set_id].seq_parameter_set_id = seq_parameter_set_id;

        if (MVC_OK != MVC_ProcessPPS(pMvcCtx, &(pMvcCtx->PPS[pic_parameter_set_id])))
        {
            dprint(PRN_ERROR, "PPS decode error line: %d.\n", __LINE__); 
            pMvcCtx->PPS[pic_parameter_set_id].is_refreshed = 1;
            pMvcCtx->PPS[pic_parameter_set_id].is_valid = 0;            
            return MVC_ERR;
        }
        else
        {
            pMvcCtx->PPS[pic_parameter_set_id].is_refreshed = 1;
            pMvcCtx->PPS[pic_parameter_set_id].is_valid = 1;
        }
    } 
    
    return MVC_OK;
}


/***********************************************************************************
* Function:    MVC_ProcessPPS(MVC_CTX_S *pMvcCtx, MVC_PPS_S *pPPS)
* Description: ����PPS, ������ݴ���
* Input:       ����������ʱ����������
* Output:      pPPS         ��PPS�����ݴ浽һ���ṹ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/
SINT32 MVC_ProcessPPS(MVC_CTX_S *pMvcCtx, MVC_PPS_S *pPPS)
{
    UINT32    residbits;
    UINT32    q4x4defaultflag[6];
    UINT32    q8x8defaultflag[2];    
    UINT32    i;

    pPPS->entropy_coding_mode_flag               = mvc_u_1 (pMvcCtx, "PPS: entropy_coding_mode_flag"      );
    MVC_SE_ERR_CHECK;

    pPPS->pic_order_present_flag                 = mvc_u_1 (pMvcCtx, "PPS: pic_order_present_flag"        );
    MVC_SE_ERR_CHECK;

    pPPS->num_slice_groups_minus1                = mvc_ue_v(pMvcCtx, "PPS: num_slice_groups_minus1"       );
    if ((0!=pPPS->num_slice_groups_minus1) || pMvcCtx->SeErrFlg)  // num_slice_groups_minus1=0ʱ������Ƭ����
    {
        dprint(PRN_ERROR, "MVC_BASELINE stream with FMO, not support.\n");
        REPORT_UNSUPPORT(pMvcCtx->ChanID);
        return MVC_ERR;
    }

    pPPS->num_ref_idx_l0_active_minus1           = mvc_ue_v(pMvcCtx, "PPS: num_ref_idx_l0_active_minus1"  );
    if ((pPPS->num_ref_idx_l0_active_minus1>31) || pMvcCtx->SeErrFlg)
    {
        dprint(PRN_ERROR, "num_ref_idx_l0_active_minus1 out of range.\n");

        // ���Կ���ǯλǿ��
        return MVC_ERR;      
    }

    pPPS->num_ref_idx_l1_active_minus1           = mvc_ue_v(pMvcCtx, "PPS: num_ref_idx_l1_active_minus1"  );
    if ((pPPS->num_ref_idx_l1_active_minus1>31) || pMvcCtx->SeErrFlg)
    {
        dprint(PRN_ERROR, "num_ref_idx_l1_active_minus1 out of range.\n");
        
        // ���Կ���ǯλǿ��
        return MVC_ERR;      
    }

    pPPS->weighted_pred_flag                     = mvc_u_1 (pMvcCtx, "PPS: weighted prediction flag"      );
    MVC_SE_ERR_CHECK;

    pPPS->weighted_bipred_idc                    = mvc_u_v (pMvcCtx, 2, "PPS: weighted_bipred_idc"        );
    if (pPPS->weighted_bipred_idc >= 3) 
    {
        dprint(PRN_ERROR, "weighted_bipred_idc out of range\n");
        
        // ���Կ���ǯλǿ��
        return MVC_ERR;
    }

    pPPS->pic_init_qp_minus26                    = mvc_se_v(pMvcCtx, "PPS: pic_init_qp_minus26"           );
    mvc_se_v(pMvcCtx,"PPS: pic_init_qs_minus26"           );
    pPPS->chroma_qp_index_offset                 = mvc_se_v(pMvcCtx, "PPS: chroma_qp_index_offset"        );
    pPPS->deblocking_filter_control_present_flag = mvc_u_1 (pMvcCtx, "PPS: deblocking_filter_control_present_flag");
    pPPS->constrained_intra_pred_flag            = mvc_u_1 (pMvcCtx, "PPS: constrained_intra_pred_flag"   );
    pPPS->redundant_pic_cnt_present_flag         = mvc_u_1 (pMvcCtx, "PPS: redundant_pic_cnt_present_flag");

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
    residbits = BsResidBits(&(pMvcCtx->BS));
    if (residbits >= (pMvcCtx->pCurrNal->nal_trail_zero_bit_num + 3) )
    {
        pPPS->transform_8x8_mode_flag = mvc_u_1 (pMvcCtx, "PPS: transform_8x8_mode_flag");
        pPPS->pic_scaling_matrix_present_flag = mvc_u_1 (pMvcCtx, "PPS: pic_scaling_matrix_present_flag");
        if (1 == pPPS->pic_scaling_matrix_present_flag)
        {
            for (i = 0; i < 6; i++)
            {
                q4x4defaultflag[i] = mvc_u_1 (pMvcCtx, "SPS: seq_scaling_list_present_flag");
                pPPS->pic_scaling_list_present_flag[i] = q4x4defaultflag[i];
                if (1 == q4x4defaultflag[i])
                {
                    MVC_Scaling_List(pMvcCtx, pPPS->ScalingList4x4[i], pPPS->qmatrix4x4[i], 16, &q4x4defaultflag[i]);
                    if (1 == q4x4defaultflag[i])
                    {
                        memcpy(pPPS->qmatrix4x4[i], (i<3) ? MVC_quant_intra_default : MVC_quant_inter_default, 4*4); 
                    }
                }
                else
                {
                    if (0 == i || 3 == i)
                    {
                        if (1 == pMvcCtx->SPS[pPPS->seq_parameter_set_id].seq_scaling_matrix_present_flag)
                        {
                            //copy frome sps
                            memcpy(pPPS->qmatrix4x4[i],  pMvcCtx->SPS[pPPS->seq_parameter_set_id].qmatrix4x4[i],  4*4);
                        }
                        else
                        {
                            memcpy(pPPS->qmatrix4x4[i], (0 == i) ? MVC_quant_intra_default : MVC_quant_inter_default, 4*4);
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
                    q8x8defaultflag[i] = mvc_u_1 (pMvcCtx, "PPS: pic_scaling_list_present_flag");
                }
                else
                {
                    q8x8defaultflag[i]  = 0;
                }
                pPPS->pic_scaling_list_present_flag[6+i] = q8x8defaultflag[i];
                if (1 == q8x8defaultflag[i])
                {
                    MVC_Scaling_List(pMvcCtx, pPPS->ScalingList8x8[i], pPPS->qmatrix8x8[i], 64, &q8x8defaultflag[i]);
                    if (1 == q8x8defaultflag[i])
                    {
                        memcpy(pPPS->qmatrix8x8[i],  (0 == i) ? MVC_quant8_intra_default : MVC_quant8_inter_default, 4*16);
                    }
                }
                else
                {
                    if (1 == pMvcCtx->SPS[pPPS->seq_parameter_set_id].seq_scaling_matrix_present_flag)
                    {
                        //fall back B, copy frome sps
                        memcpy(pPPS->qmatrix8x8[i],  pMvcCtx->SPS[pPPS->seq_parameter_set_id].qmatrix8x8[i], 4*16);
                    }
                    else
                    {
                        memcpy(pPPS->qmatrix8x8[i],  (0 == i) ? MVC_quant8_intra_default : MVC_quant8_inter_default, 4*16);
                    }
                }
            }
        }

        pPPS->second_chroma_qp_index_offset = mvc_se_v (pMvcCtx, "PPS: second_chroma_qp_index_offset");
        if ((12 < pPPS->second_chroma_qp_index_offset) ||(-12 > pPPS->second_chroma_qp_index_offset)|| pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "second_chroma_qp_index_offset out of range.\n");
            
            return MVC_ERR;
        }
    }

    return MVC_OK;
}


/***********************************************************************************
* Function:    MVC_PPSEqual(MVC_PPS_S *pPPS1, MVC_PPS_S *pPPS2)
* Description: �Ƚ�����SPS�Ƿ����
* Input:       pPPS1, pPPS2  �������Ƚϵ�PPS
* Output:      ��
* Return:      DEC_TRUE   ����PPS����ȵ�
*              DEC_FALSE  ����PPS�����
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_PPSEqual(MVC_PPS_S *pPPS1, MVC_PPS_S *pPPS2)
{
    UINT32 i, j;
    SINT32 equal = 1;
    SINT32 ret;

    CHECK_NULL_PTR_ReturnValue(pPPS1, MVC_ERR);
    CHECK_NULL_PTR_ReturnValue(pPPS2, MVC_ERR); 

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
                { //yyc not sure
                    equal &= (pPPS1->UseDefaultScalingMatrix4x4Flag[i] == pPPS2->UseDefaultScalingMatrix4x4Flag[i]);
                    if(!pPPS1->UseDefaultScalingMatrix4x4Flag[i])
                    {
                        for (j = 0; j < 16; j++)
                        {
                            equal &= (pPPS1->ScalingList4x4[i][j] == pPPS2->ScalingList4x4[i][j]);
                        }
                    }
                }
                else
                {
                    equal &= (pPPS1->UseDefaultScalingMatrix8x8Flag[i-6] == pPPS2->UseDefaultScalingMatrix8x8Flag[i-6]);
                    if(!pPPS1->UseDefaultScalingMatrix8x8Flag[i-6])
                    {
                        for (j = 0; j < 64; j++)
                        {
                            equal &= (pPPS1->ScalingList8x8[i-6][j] == pPPS2->ScalingList8x8[i-6][j]);
                        }
                    }
                }
            }
        }
    }
    equal &= (pPPS1->second_chroma_qp_index_offset == pPPS2->second_chroma_qp_index_offset);

    ret = equal ? MVC_OK : MVC_ERR;

    return ret;
}


/***********************************************************************************
* Function:    mvc_vui_parameters(MVC_CTX_S *pMvcCtx)
* Description: ����VUI�﷨Ԫ��, ������ݴ���
* Input:       ����������ʱ����������
* Output:      pSPS        ֻ��Ӱ�������е�һ��Ԫ�ؽṹ���VUI����
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 mvc_vui_parameters(MVC_CTX_S *pMvcCtx, MVC_VUI_SEQ_PARAMETERS_S *pVUI)
{
    pVUI->aspect_ratio_info_present_flag = mvc_u_1(pMvcCtx, "VUI: aspect_ratio_info_present_flag");
    if (pVUI->aspect_ratio_info_present_flag)
    {
        pVUI->aspect_ratio_idc = mvc_u_v(pMvcCtx,  8, "VUI: aspect_ratio_idc");
        if (255==pVUI->aspect_ratio_idc)
        {
            pVUI->sar_width    = mvc_u_v(pMvcCtx, 16, "VUI: sar_width" );
            pVUI->sar_height   = mvc_u_v(pMvcCtx, 16, "VUI: sar_height");
        }
    }
    else
    {
        pVUI->aspect_ratio_idc = 0;
    }
    
    pVUI->overscan_info_present_flag            = mvc_u_1(pMvcCtx, "VUI: overscan_info_present_flag");
    if (pVUI->overscan_info_present_flag)
    {
        pVUI->overscan_appropriate_flag         = mvc_u_1(pMvcCtx, "VUI: overscan_appropriate_flag");
    }
    
    pVUI->video_signal_type_present_flag        = mvc_u_1(pMvcCtx, "VUI: video_signal_type_present_flag");
    if (pVUI->video_signal_type_present_flag)
    {
        pVUI->video_format                      = mvc_u_v(pMvcCtx, 3, "VUI: video_format"               );
        pVUI->video_full_range_flag             = mvc_u_1(pMvcCtx, "VUI: video_full_range_flag"         );
        pVUI->colour_description_present_flag   = mvc_u_1(pMvcCtx, "VUI: color_description_present_flag");
        if (pVUI->colour_description_present_flag)
        {
            pVUI->colour_primaries              = mvc_u_v(pMvcCtx, 8, "VUI: colour_primaries"        );
            pVUI->transfer_characteristics      = mvc_u_v(pMvcCtx, 8, "VUI: transfer_characteristics");
            pVUI->matrix_coefficients           = mvc_u_v(pMvcCtx, 8, "VUI: matrix_coefficients"     );
        }
    }
    else
    {
        pVUI->video_format = 5;
    }
    pVUI->chroma_location_info_present_flag       = mvc_u_1  (pMvcCtx, "VUI: chroma_loc_info_present_flag");
    if (pVUI->chroma_location_info_present_flag)
    {
        pVUI->chroma_sample_loc_type_top_field    = mvc_ue_v (pMvcCtx, "VUI: chroma_sample_loc_type_top_field"   );
        pVUI->chroma_sample_loc_type_bottom_field = mvc_ue_v (pMvcCtx, "VUI: chroma_sample_loc_type_bottom_field");
    }
    pVUI->timing_info_present_flag                = mvc_u_1  (pMvcCtx, "VUI: timing_info_present_flag"       );
    if (pVUI->timing_info_present_flag)          
    {                                            
        pVUI->num_units_in_tick                   = mvc_u_v  (pMvcCtx, 32, "VUI: num_units_in_tick"          );
        pVUI->time_scale                          = mvc_u_v  (pMvcCtx, 32, "VUI: time_scale"                 );
        pVUI->fixed_frame_rate_flag               = mvc_u_1  (pMvcCtx,     "VUI: fixed_frame_rate_flag"      );
        if ( (1 == pVUI->fixed_frame_rate_flag) && (0 != pVUI->num_units_in_tick) )
        {
            pMvcCtx->frame_rate = pVUI->time_scale / pVUI->num_units_in_tick / 2;
        }
    }                                            
    pVUI->nal_hrd_parameters_present_flag         = mvc_u_1  (pMvcCtx, "VUI: nal_hrd_parameters_present_flag");
    if (pVUI->nal_hrd_parameters_present_flag)
    {
        UINT32 SchedSelIdx;
        MVC_HRD_PARAMETERS_S *hrd = &(pVUI->nal_hrd_parameters);
        
        hrd->cpb_cnt_minus1                                       = mvc_ue_v (pMvcCtx,    "VUI: cpb_cnt_minus1");
        hrd->bit_rate_scale                                       = mvc_u_v  (pMvcCtx, 4, "VUI: bit_rate_scale");
        hrd->cpb_size_scale                                       = mvc_u_v  (pMvcCtx, 4, "VUI: cpb_size_scale");
        
        if ((hrd->cpb_cnt_minus1>31) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");
            return MVC_ERR;
        }
        
        for (SchedSelIdx=0; SchedSelIdx<=hrd->cpb_cnt_minus1; SchedSelIdx++) 
        {
            hrd->bit_rate_value_minus1[SchedSelIdx]               = mvc_ue_v (pMvcCtx, "VUI: bit_rate_value_minus1");
            hrd->cpb_size_value_minus1[SchedSelIdx]               = mvc_ue_v (pMvcCtx, "VUI: cpb_size_value_minus1");
            hrd->cbr_flag[SchedSelIdx]                            = mvc_u_1  (pMvcCtx, "VUI: cbr_flag"             );
        }
        
        hrd->initial_cpb_removal_delay_length_minus1              = mvc_u_v  (pMvcCtx, 5, "VUI: initial_cpb_removal_delay_length_minus1");
        hrd->cpb_removal_delay_length_minus1                      = mvc_u_v  (pMvcCtx, 5, "VUI: cpb_removal_delay_length_minus1"        );
        hrd->dpb_output_delay_length_minus1                       = mvc_u_v  (pMvcCtx, 5, "VUI: dpb_output_delay_length_minus1"         );
        hrd->time_offset_length                                   = mvc_u_v  (pMvcCtx, 5, "VUI: time_offset_length"                     );
    }
    pVUI->vcl_hrd_parameters_present_flag                         = mvc_u_1  (pMvcCtx, "VUI: vcl_hrd_parameters_present_flag"           );
    if (pVUI->vcl_hrd_parameters_present_flag)
    {
        UINT32 SchedSelIdx;
        MVC_HRD_PARAMETERS_S *hrd = &(pVUI->vcl_hrd_parameters);
        
        hrd->cpb_cnt_minus1                                       = mvc_ue_v (pMvcCtx,    "VUI: cpb_cnt_minus1");
        hrd->bit_rate_scale                                       = mvc_u_v  (pMvcCtx, 4, "VUI: bit_rate_scale");
        hrd->cpb_size_scale                                       = mvc_u_v  (pMvcCtx, 4, "VUI: cpb_size_scale");
        if ((hrd->cpb_cnt_minus1>31) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");        
            return MVC_ERR;
        }
        for (SchedSelIdx=0; SchedSelIdx<=hrd->cpb_cnt_minus1; SchedSelIdx++) 
        {
            hrd->bit_rate_value_minus1[SchedSelIdx]               = mvc_ue_v (pMvcCtx, "VUI: bit_rate_value_minus1");
            hrd->cpb_size_value_minus1[SchedSelIdx]               = mvc_ue_v (pMvcCtx, "VUI: cpb_size_value_minus1");
            hrd->cbr_flag[SchedSelIdx]                            = mvc_u_1  (pMvcCtx, "VUI: cbr_flag"             );
        }
        
        hrd->initial_cpb_removal_delay_length_minus1              = mvc_u_v  (pMvcCtx, 5, "VUI: initial_cpb_removal_delay_length_minus1");
        hrd->cpb_removal_delay_length_minus1                      = mvc_u_v  (pMvcCtx, 5, "VUI: cpb_removal_delay_length_minus1"        );
        hrd->dpb_output_delay_length_minus1                       = mvc_u_v  (pMvcCtx, 5, "VUI: dpb_output_delay_length_minus1"         );
        hrd->time_offset_length                                   = mvc_u_v  (pMvcCtx, 5, "VUI: time_offset_length"                     );
    }
    if (pVUI->nal_hrd_parameters_present_flag || pVUI->vcl_hrd_parameters_present_flag)
    {
        pVUI->low_delay_hrd_flag                      = mvc_u_1 (pMvcCtx, "VUI: low_delay_hrd_flag"        );
    }                                                 
    pVUI->pic_struct_present_flag                     = mvc_u_1 (pMvcCtx, "VUI: pic_struct_present_flag"   );
    pVUI->bitstream_restriction_flag                  = mvc_u_1 (pMvcCtx, "VUI: bitstream_restriction_flag");
    if (pVUI->bitstream_restriction_flag)
    {
        pVUI->motion_vectors_over_pic_boundaries_flag = mvc_u_1 (pMvcCtx, "VUI: motion_vectors_over_pic_boundaries_flag");
        pVUI->max_bytes_per_pic_denom                 = mvc_ue_v(pMvcCtx, "VUI: max_bytes_per_pic_denom"                );
        pVUI->max_bits_per_mb_denom                   = mvc_ue_v(pMvcCtx, "VUI: max_bits_per_mb_denom"                  );
        pVUI->log2_max_mv_length_horizontal           = mvc_ue_v(pMvcCtx, "VUI: log2_max_mv_length_horizontal"          );
        pVUI->log2_max_mv_length_vertical             = mvc_ue_v(pMvcCtx, "VUI: log2_max_mv_length_vertical"            );
        pVUI->num_reorder_frames                      = mvc_ue_v(pMvcCtx, "VUI: num_reorder_frames"                     );
        pVUI->max_dec_frame_buffering                 = mvc_ue_v(pMvcCtx, "VUI: max_dec_frame_buffering"                );
        MVC_SE_ERR_CHECK;
    }
    MVC_SE_ERR_CHECK;
    
    return MVC_OK;
}


/***********************************************************************************
* Function:    MVC_SPSEqual(MVC_SPS_S *pSPS1, MVC_SPS_S *pSPS2)
* Description: �Ƚ�����SPS�Ƿ����
* Input:       pSPS1, pSPS2  �������Ƚϵ�SPS
* Output:      ��
* Return:      DEC_TRUE   ����SPS����ȵ�
*              DEC_FALSE  ����SPS�����
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_SPSEqual(MVC_SPS_S *pSPS1, MVC_SPS_S *pSPS2)
{
    UINT32 i, j;
    SINT32 equal = 1;
    SINT32 ret;

    CHECK_NULL_PTR_ReturnValue(pSPS1, MVC_ERR);
    CHECK_NULL_PTR_ReturnValue(pSPS2, MVC_ERR);

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

    ret = equal ? MVC_OK : MVC_ERR;

    return ret;
}


/***********************************************************************************
* Function:      SINT32  MVC_GetDar(SINT32 ar_idc, SINT32 sar_width, SINT32 sar_height, SINT32 img_width, SINT32 img_height)
* Description:  ���� display_aspec_ratio
* Input:           sar width, sar height
* Output:       
* Return:        aspec_ratio
* Others:      ��
***********************************************************************************/ 
#define MVC_CONST_DAR_235_100  (235*1024/100)
#define MVC_CONST_DAR_221_100  (221*1024/100)
#define MVC_CONST_DAR_16_9  (16*1024/9)
#define MVC_CONST_DAR_4_3   (4*1024/3)
SINT32  MVC_GetDar(SINT32 ar_idc, SINT32 sar_width, SINT32 sar_height, SINT32 img_width, SINT32 img_height)
{
    static SINT32 MVC_SarTable[][2] =
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
            sar_width  = MVC_SarTable[ar_idc][0];
            sar_height = MVC_SarTable[ar_idc][1];
        }
        dar = (sar_width * img_width * 1024) / (sar_height * img_height);

        if (ABS(dar - MVC_CONST_DAR_235_100) < ABS(dar - MVC_CONST_DAR_221_100))
        {
            final_dar = DAR_235_100;
        }
        else if (ABS(dar - MVC_CONST_DAR_221_100) < ABS(dar - MVC_CONST_DAR_16_9))
        {
            final_dar = DAR_221_100;
        }
        else if (ABS(dar - MVC_CONST_DAR_16_9) < ABS(dar - MVC_CONST_DAR_4_3))
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
* Function:    MVC_ProcessSPS(MVC_CTX_S *pMvcCtx, MVC_SPS_S *pSPS)
* Description: ����SPS��VUI��Ϣ, ������ݴ���
* Input:       ����������ʱ����������
* Output:      pSPS         �ݴ������SPS��VUI������һ���ṹ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_ProcessSPS(MVC_CTX_S *pMvcCtx, MVC_SPS_S *pSPS)
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
    UINT32 bit_depth_luma_minus8;
    UINT32 bit_depth_chroma_minus8;
    UINT32 qpprime_y_zero_trans_bypass_flag;
    UINT32 q4x4defaultflag[6];
    UINT32 q8x8defaultflag[2];

    CHECK_NULL_PTR_ReturnValue(pSPS, MVC_ERR);

    if (MVC_HIGHPROFILE == pSPS->profile_idc || MVC_HIGH10P == pSPS->profile_idc || 
        MVC_HIGH422P== pSPS->profile_idc || MVC_HIGH444P== pSPS->profile_idc ||
        MVC_MULTIVIEWHIGH == pSPS->profile_idc || STEREOHIGH == pSPS->profile_idc)
    {
        //high profile dec chroma_format_idc
        pSPS->chroma_format_idc   = mvc_ue_v (pMvcCtx, "SPS: chroma_format_idc");
        if (((0 != pSPS->chroma_format_idc) && (1 !=pSPS->chroma_format_idc)) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSPS->chroma_format_idc out of range.\n");
            return MVC_ERR;            
        }
        bit_depth_luma_minus8                  = mvc_ue_v (pMvcCtx, "SPS: bit_depth_luma_minus8");
        if (0 != bit_depth_luma_minus8 || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "bit_depth_luma_minus8 not equal 0.\n");
            //return MVC_ERR;                
        }
        bit_depth_chroma_minus8                = mvc_ue_v (pMvcCtx, "SPS: bit_depth_chroma_minus8");
        if (0 != bit_depth_chroma_minus8 || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "bit_depth_chroma_minus8 not equal 0.\n");
            //return MVC_ERR;                
        }        
        qpprime_y_zero_trans_bypass_flag    = mvc_u_1  (pMvcCtx, "SPS: qpprime_y_zero_trans_bypass_flag");
        if (0 != qpprime_y_zero_trans_bypass_flag || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "qpprime_y_zero_trans_bypass_flag not equal 0.\n");
            return MVC_ERR;                
        }            

        pSPS->seq_scaling_matrix_present_flag        = mvc_u_1  (pMvcCtx, "SPS: seq_scaling_matrix_present_flag");
        if (1 == pSPS->seq_scaling_matrix_present_flag)
        {
            for (i = 0; i < 6; i++)
            {
                q4x4defaultflag[i] = mvc_u_1  (pMvcCtx, "SPS: seq_scaling_list_present_flag");
                pSPS->seq_scaling_list_present_flag[i] = q4x4defaultflag[i];
                if (1 == q4x4defaultflag[i])
                {
                    MVC_Scaling_List(pMvcCtx, pSPS->ScalingList4x4[i], pSPS->qmatrix4x4[i], 16, &q4x4defaultflag[i]);
                    if (1 == q4x4defaultflag[i])
                    {
                        memcpy(pSPS->qmatrix4x4[i], (i<3)? MVC_quant_intra_default:MVC_quant_inter_default, 4*4); 
                    }
                }
                else
                {
                    if (0 == i || 3 == i)
                    {
                        memcpy(pSPS->qmatrix4x4[i], (i == 0) ? MVC_quant_intra_default : MVC_quant_inter_default, 4*4);
                    }
                    else
                    {
                        memcpy(pSPS->qmatrix4x4[i],  pSPS->qmatrix4x4[i-1], 4*4);
                    }
                }
            }

            for (i = 0; i < 2; i++)
            {
                q8x8defaultflag[i] = mvc_u_1  (pMvcCtx, "SPS: seq_scaling_list_present_flag");
                pSPS->seq_scaling_list_present_flag[6+i] = q8x8defaultflag[i];
                if (1 == q8x8defaultflag[i])
                {
                    MVC_Scaling_List(pMvcCtx, pSPS->ScalingList8x8[i], pSPS->qmatrix8x8[i], 64, &q8x8defaultflag[i]);
                    if (1 == q8x8defaultflag[i] )
                    {
                        memcpy(pSPS->qmatrix8x8[i],  (0 == i) ? MVC_quant8_intra_default : MVC_quant8_inter_default, 4*16);
                    }
                }
                else
                {
                    memcpy(pSPS->qmatrix8x8[i],  (0 == i) ? MVC_quant8_intra_default : MVC_quant8_inter_default, 4*16);
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

    pSPS->log2_max_frame_num_minus4 = mvc_ue_v(pMvcCtx, "SPS: log2_max_frame_num_minus4");
    if ((pSPS->log2_max_frame_num_minus4>12) || pMvcCtx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pSPS->log2_max_frame_num_minus4 out of range.\n");
        return MVC_ERR;
    }

    pSPS->pic_order_cnt_type = mvc_ue_v(pMvcCtx, "SPS: pic_order_cnt_type");
    if ((pSPS->pic_order_cnt_type>2) || pMvcCtx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pSPS->pic_order_cnt_type out of range.\n");
        return MVC_ERR;
    }

    if (0==pSPS->pic_order_cnt_type)
    {
        pSPS->log2_max_pic_order_cnt_lsb_minus4 = mvc_ue_v(pMvcCtx, "SPS: log2_max_pic_order_cnt_lsb_minus4");
        if ((pSPS->log2_max_pic_order_cnt_lsb_minus4>12) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSPS->log2_max_pic_order_cnt_lsb_minus4 out of range.\n");
            return MVC_ERR;
        }
    }
    else if (1==pSPS->pic_order_cnt_type)
    {
        pSPS->delta_pic_order_always_zero_flag = mvc_u_1(pMvcCtx, "SPS: delta_pic_order_always_zero_flag");
        MVC_SE_ERR_CHECK;

        pSPS->offset_for_non_ref_pic = mvc_se_v(pMvcCtx, "SPS: offset_for_non_ref_pic");
        MVC_SE_ERR_CHECK;

        pSPS->offset_for_top_to_bottom_field = mvc_se_v(pMvcCtx, "SPS: offset_for_top_to_bottom_field");
        MVC_SE_ERR_CHECK;

        pSPS->num_ref_frames_in_pic_order_cnt_cycle = mvc_ue_v(pMvcCtx, "SPS: num_ref_frames_in_pic_order_cnt_cycle");
        if ((pSPS->num_ref_frames_in_pic_order_cnt_cycle>255) || pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSPS->num_ref_frames_in_pic_order_cnt_cycle out of range.\n");
            return MVC_ERR;
        }

        for (i=0; i<pSPS->num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            pSPS->offset_for_ref_frame[i] = mvc_se_v(pMvcCtx, "SPS: offset_for_ref_frame[i]");
            MVC_SE_ERR_CHECK;
        }
        MVC_SE_ERR_CHECK;
    }

    pSPS->num_ref_frames = mvc_ue_v(pMvcCtx, "SPS: num_ref_frames");
    MVC_SE_ERR_CHECK;

    pSPS->gaps_in_frame_num_value_allowed_flag = mvc_u_1(pMvcCtx, "SPS: gaps_in_frame_num_value_allowed_flag");
    MVC_SE_ERR_CHECK;

    pSPS->pic_width_in_mbs_minus1 = mvc_ue_v(pMvcCtx, "SPS: pic_width_in_mbs_minus1");
    if (((pSPS->pic_width_in_mbs_minus1+1)<2) || ((pSPS->pic_width_in_mbs_minus1+1)>=MAX_IMG_WIDTH_IN_MB) || pMvcCtx->SeErrFlg)
    {
        dprint(PRN_ERROR, "pSPS->pic_width_in_mbs_minus1 out of range(=%d).\n", pSPS->pic_width_in_mbs_minus1);
        return MVC_ERR;
    }

    pSPS->pic_height_in_map_units_minus1 = mvc_ue_v(pMvcCtx, "SPS: pic_height_in_map_units_minus1");
    MVC_SE_ERR_CHECK;

    pSPS->frame_mbs_only_flag = mvc_u_1(pMvcCtx, "SPS: frame_mbs_only_flag");
    MVC_SE_ERR_CHECK;
    temp = (1+(!pSPS->frame_mbs_only_flag));
    if ( 1 != temp && 2 != temp)
    {
        dprint(PRN_ERROR, "frame_mbs_only_flag err\n");
        return MVC_ERR;
    }

    if (!pSPS->frame_mbs_only_flag)
    {
        pSPS->mb_adaptive_frame_field_flag = mvc_u_1(pMvcCtx, "SPS: mb_adaptive_frame_field_flag");
        if (((pSPS->pic_height_in_map_units_minus1+1)>=MAX_IMG_HALF_HEIGHT_IN_MB) || ((pSPS->pic_width_in_mbs_minus1+1)<4) 
            || ((pSPS->pic_height_in_map_units_minus1+1)<2))
        {
            dprint(PRN_ERROR, "pSPS->pic_height_in_map_units_minus1 out of range.\n");
            return MVC_ERR;
        }
    }
    else
    {
        pSPS->mb_adaptive_frame_field_flag = 0; // default    
        if (((pSPS->pic_height_in_map_units_minus1+1)>=MAX_IMG_HEIGHT_IN_MB) || ((pSPS->pic_height_in_map_units_minus1+1)<2))
        {
            dprint(PRN_ERROR, "pSPS->pic_height_in_map_units_minus1 out of range.\n");
            return MVC_ERR;
        }        
    }

    // ����һ��ͼ��Ĵ�С
    PicWidthInSmp =  (pSPS->pic_width_in_mbs_minus1+1) *16;
    PicHeightInSmp = (pSPS->pic_height_in_map_units_minus1+1) *temp*16 ;
    pic_size = (PicWidthInSmp*PicHeightInSmp/2)*3;
    if (pic_size > MAX_IMG_WIDTH_IN_MB*MAX_IMG_HEIGHT_IN_MB*384)
    {
        dprint(PRN_FATAL, "pic size too large.\n");
        return MVC_ERR;
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
        return MVC_ERR;  //LEVEL_ERR
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

    if ((pSPS->num_ref_frames>size) || pMvcCtx->SeErrFlg)
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

    pSPS->direct_8x8_inference_flag    = mvc_u_1 (pMvcCtx, "SPS: direct_8x8_inference_flag"        );
    MVC_SE_ERR_CHECK;

    pSPS->frame_cropping_flag          = mvc_u_1 (pMvcCtx, "SPS: frame_cropping_flag"              );
    MVC_SE_ERR_CHECK;

    frame_cropping_flag = pSPS->frame_cropping_flag;
    if (frame_cropping_flag)
    {
        pSPS->frame_crop_left_offset   = mvc_ue_v(pMvcCtx, "SPS: frame_cropping_rect_left_offset"  );
        pSPS->frame_crop_right_offset  = mvc_ue_v(pMvcCtx, "SPS: frame_cropping_rect_right_offset" );
        pSPS->frame_crop_top_offset    = mvc_ue_v(pMvcCtx, "SPS: frame_cropping_rect_top_offset"   );
        pSPS->frame_crop_bottom_offset = mvc_ue_v(pMvcCtx, "SPS: frame_cropping_rect_bottom_offset");

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

    pSPS->vui_parameters_present_flag     = mvc_u_1 (pMvcCtx, "SPS: vui_parameters_present_flag"      );
    MVC_SE_ERR_CHECK;

    pSPS->vui_seq_parameters.matrix_coefficients = 2;
    if (pSPS->vui_parameters_present_flag)
    {
        if (MVC_OK == mvc_vui_parameters(pMvcCtx, &(pSPS->vui_seq_parameters))) // ����VUI��Ϣ
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

    pSPS->vui_seq_parameters.aspect_ratio = MVC_GetDar(pSPS->vui_seq_parameters.aspect_ratio_idc, 
        pSPS->vui_seq_parameters.sar_width, pSPS->vui_seq_parameters.sar_height, PicWidthInSmp, PicHeightInSmp);

    //dprint(PRN_ALWS,"pSPS->num_ref_frames = %d\n", pSPS->num_ref_frames);
    if (pSPS->num_ref_frames > 0)
    {
        size = MIN(size, pSPS->num_ref_frames);
    }
    size = MIN(size, 16);

    if ((pMvcCtx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION) && 
        (pMvcCtx->pstExtraData->stChanOption.s32ReRangeEn == 0)) /* contition??? */
    {
        SINT32 MaxRefNum, MaxWidth, MaxHeight;
        MaxRefNum = pMvcCtx->pstExtraData->stChanOption.s32MaxRefFrameNum;
        MaxWidth  = pMvcCtx->pstExtraData->stChanOption.s32MaxWidth;
        MaxHeight = pMvcCtx->pstExtraData->stChanOption.s32MaxHeight;
        if (size > MaxRefNum)
        {
                  REPORT_REF_NUM_OVER(pMvcCtx->ChanID, size, pMvcCtx->pstExtraData->stChanOption.s32MaxRefFrameNum);
		    /*�˴�ֱ�ӷ���error���Ա��������������ֱ��ʳ�����߲ο�֡��������ʱ�ظ���ʼ��DPB���²���Ҫ���鷳*/
		    return MVC_ERR;			
        }

        if (PicWidthInSmp > MaxWidth || PicHeightInSmp > MaxHeight)
        {
              REPORT_IMGSIZE_OVER(pMvcCtx->ChanID, PicWidthInSmp, PicHeightInSmp, MaxWidth, MaxHeight);
    		/*�˴�ֱ�ӷ���error���Ա��������������ֱ��ʳ�����߲ο�֡��������ʱ�ظ���ʼ��DPB���²���Ҫ���鷳*/
    		return MVC_ERR;			
        }

    }
    

    /* ҵ��������ͨ��û��̫��ο�֡����������һ�� */
    if (PicHeightInSmp >= 1920)
    {
        size = MIN(size, 4);
    }

    pSPS->dpb_size_plus1 = size + 1;

    return MVC_OK;
}


/***********************************************************************************
* Function:    MVC_DecSPS(MVC_CTX_S *pMvcCtx)
* Description: ����SPS, ������ݴ���
* Input:       ����������ʱ����������
* Output:      g_SPS        ֻ��Ӱ�������е�һ��Ԫ�ؽṹ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_DecSPS(MVC_CTX_S *pMvcCtx)
{
    static MVC_SPS_S sps_tmp;   /* ����ɾ�̬����ֹռ�ö�ջ̫�� */
    UINT32 profile_idc;
    UINT32 constrained_set0_flag;
    UINT32 constrained_set1_flag;
    UINT32 constrained_set2_flag;
    UINT32 constrained_set3_flag;
    UINT32 constrained_set4_flag;
    UINT32 constrained_set5_flag;
    UINT32 level_idc;
    UINT32 seq_parameter_set_id;

    profile_idc           = mvc_u_v (pMvcCtx, 8, "SPS: profile_idc"          );
    constrained_set0_flag = mvc_u_1 (pMvcCtx,    "SPS: constrained_set0_flag");
    constrained_set1_flag = mvc_u_1 (pMvcCtx,    "SPS: constrained_set1_flag");
    constrained_set2_flag = mvc_u_1 (pMvcCtx,    "SPS: constrained_set2_flag");
    constrained_set3_flag = mvc_u_1 (pMvcCtx,    "SPS: constrained_set3_flag");
    constrained_set4_flag = mvc_u_1(pMvcCtx,     "SPS: constrained_set4_flag");
    constrained_set5_flag = mvc_u_1(pMvcCtx,     "SPS: constrained_set5_flag");
    mvc_u_v (pMvcCtx, 2, "SPS: reserved_zero_2bits"  ); // reserved_zero
    level_idc             = mvc_u_v (pMvcCtx, 8, "SPS: level_idc"            );
    seq_parameter_set_id  = mvc_ue_v(pMvcCtx,    "SPS: seq_parameter_set_id" );
    if (pMvcCtx->SeErrFlg)
    {
        return MVC_ERR;
    }

    if (seq_parameter_set_id > (pMvcCtx->MaxSpsNum-1))
    {
        dprint(PRN_ERROR, "seq_parameter_set_id out of range.\n");
        if (seq_parameter_set_id > (pMvcCtx->MaxSpsNum-1))
        {
            REPORT_SPS_NUM_OVER( pMvcCtx->ChanID, seq_parameter_set_id, pMvcCtx->MaxSpsNum );
        }
        // ���Կ���ǯλǿ��
        return MVC_ERR;
    }

    switch (profile_idc)
    {
    case MVC_BASELINE:
        dprint(PRN_ERROR, "MVC_BASELINE stream, try to decode, exit when FMO occurred.\n");
        break;        
    case MVC_EXTENDED:
        dprint(PRN_ERROR, "MVC_EXTENDED stream, try to decode, exit when datapartition occurred.\n");
        break;
    case MVC_MAINPROFILE:
        break;
    case MVC_HIGHPROFILE:
        break;
    case MVC_HIGH10P:
    case MVC_HIGH422P:
    case MVC_HIGH444P:
        dprint(PRN_ERROR, "others High profile stream, try to decode, exit when high profile not support occurred.\n");
        break;            
    default:
        dprint(PRN_ERROR, "profile_idc = %5d error, try to decode as main profile.\n", profile_idc);
        profile_idc = MVC_HIGHPROFILE;
        break;
    }

    if (level_idc < MVC_MIN_LEVEL)
    {
        dprint(PRN_ERROR, "level_idc = %5d error, try to decode as level_idc %d.\n", level_idc, MVC_MAX_LEVEL);
        level_idc = MVC_MAX_LEVEL;
    }
    
    if (level_idc > MVC_MAX_LEVEL)
    {
        dprint(PRN_ERROR, "level_idc = %5d error, try to decode as level_idc %d.\n", level_idc, MVC_MAX_LEVEL);
        level_idc = MVC_MAX_LEVEL;
    }

    if (pMvcCtx->SPS[seq_parameter_set_id].is_valid) // ������Թ���id��Ӧ��SPS
    {
        sps_tmp.profile_idc           = profile_idc;
        sps_tmp.constrained_set0_flag = constrained_set0_flag;
        sps_tmp.constrained_set1_flag = constrained_set1_flag;
        sps_tmp.constrained_set2_flag = constrained_set2_flag;
        sps_tmp.constrained_set3_flag = constrained_set3_flag;
        sps_tmp.constrained_set4_flag = constrained_set4_flag;
        sps_tmp.constrained_set5_flag = constrained_set5_flag;
        sps_tmp.level_idc             = level_idc;
        sps_tmp.seq_parameter_set_id  = seq_parameter_set_id;

        if (MVC_OK != MVC_ProcessSPS(pMvcCtx, &sps_tmp))
        {
            dprint(PRN_ERROR, "Line %d: SPS[%d] decode error.\n", __LINE__, seq_parameter_set_id);
            return MVC_ERR;
        }

        if (MVC_OK != MVC_SPSEqual(&sps_tmp, &(pMvcCtx->SPS[seq_parameter_set_id])))
        {
            sps_tmp.is_refreshed = 1;
            sps_tmp.is_valid = 1;
            memmove(&(pMvcCtx->SPS[seq_parameter_set_id]), &sps_tmp, sizeof(MVC_SPS_S));   
        }
    }
    else
    {
        pMvcCtx->SPS[seq_parameter_set_id].profile_idc           = profile_idc;
        pMvcCtx->SPS[seq_parameter_set_id].constrained_set0_flag = constrained_set0_flag;
        pMvcCtx->SPS[seq_parameter_set_id].constrained_set1_flag = constrained_set1_flag;
        pMvcCtx->SPS[seq_parameter_set_id].constrained_set2_flag = constrained_set2_flag;
        pMvcCtx->SPS[seq_parameter_set_id].constrained_set3_flag = constrained_set3_flag;
        pMvcCtx->SPS[seq_parameter_set_id].constrained_set4_flag = constrained_set4_flag;
        pMvcCtx->SPS[seq_parameter_set_id].constrained_set5_flag = constrained_set5_flag;
        pMvcCtx->SPS[seq_parameter_set_id].level_idc             = level_idc;
        pMvcCtx->SPS[seq_parameter_set_id].seq_parameter_set_id  = seq_parameter_set_id;      

        if (MVC_OK != MVC_ProcessSPS(pMvcCtx, &(pMvcCtx->SPS[seq_parameter_set_id])))
        {
            dprint(PRN_ERROR, "Line %d: SPS[%d] decode error.\n", __LINE__, seq_parameter_set_id);
            pMvcCtx->SPS[seq_parameter_set_id].is_refreshed = 1;
            pMvcCtx->SPS[seq_parameter_set_id].is_valid = 0;
            return MVC_ERR;
        }

        pMvcCtx->SPS[seq_parameter_set_id].is_refreshed = 1;
        pMvcCtx->SPS[seq_parameter_set_id].is_valid = 1;
    }

    return MVC_OK;
}

/***********************************************************************************
* Function: MVC_FreeMvcExtMem(MVC_SUBSPS_S *pSubsps)
* Description: �ͷ�SUBSPS��mvc extension syntax��̬������ڴ�
* Input:      
* Output:    
* Return:   
*              
* Others:      ��
***********************************************************************************/
#define MVC_FREEPTR(ptr)                     \
if (ptr)                                          \
{                                                  \
    FREE(ptr);                                 \
    (ptr) = NULL;                              \
}
SINT32 MVC_FreeMvcExtMem(MVC_SUBSPS_S *pSubsps)
{
#if 0
    UINT32 i,j;
    UINT32 num_level = pSubsps->num_level_values_signalled_minus1 + 1;
    UINT32 num_ops;

    CHECK_NULL_PTR_ReturnValue(pSubsps, MVC_ERR);

    if(num_level<=64)
    {
        for (i=0;i<num_level;i++)
        {
            num_ops = pSubsps->num_applicable_ops_minus1[i] + 1;
	     for (j=0;j<num_ops&&num_ops<=1024;j++)
	     {
                MVC_FREEPTR(pSubsps->applicable_op_target_view_id[i][j]);
	     }
            MVC_FREEPTR(pSubsps->applicable_op_target_view_id[i]);
            MVC_FREEPTR(pSubsps->applicable_op_temporal_id[i]);
            MVC_FREEPTR(pSubsps->applicable_op_num_target_views_minus1[i]);
            MVC_FREEPTR(pSubsps->applicable_op_num_views_minus1[i]);
        }

        MVC_FREEPTR(pSubsps->level_idc);
        MVC_FREEPTR(pSubsps->num_applicable_ops_minus1);
        MVC_FREEPTR(pSubsps->applicable_op_temporal_id);
        MVC_FREEPTR(pSubsps->applicable_op_num_target_views_minus1);
        MVC_FREEPTR(pSubsps->applicable_op_target_view_id);
        MVC_FREEPTR(pSubsps->applicable_op_num_views_minus1);
    }
#endif
    return MVC_OK;
}
/***********************************************************************************
* Function: MVC_FreeMvcVuiExtMem(MVCVUI_SUBSPS_S *pMvc_vui_parameter)
* Description: �ͷ�SUBSPS��mvc VUI parameter extension syntax��̬������ڴ�
* Input:      
* Output:    
* Return:   
*              
* Others:      ��
***********************************************************************************/
SINT32 MVC_FreeMvcVuiExtMem(MVCVUI_SUBSPS_S *pMvc_vui_parameter)
{
#if 0
    UINT32 i;
    UINT32 num_ops = pMvc_vui_parameter->vui_mvc_num_ops_minus1 + 1;
	
    CHECK_NULL_PTR_ReturnValue(pMvc_vui_parameter, MVC_ERR);

    for (i=0;i<num_ops;i++)
    {
        MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_view_id[i]);
    }
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_temporal_id);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_num_target_output_views_minus1);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_timing_info_present_flag);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_num_units_in_tick);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_time_scale);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_fixed_frame_rate_flag);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_nal_hrd_parameters_present_flag);
    MVC_FREEPTR(pMvc_vui_parameter->mvc_nal_hrd_parameters);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_vcl_hrd_parameters_present_flag);
    MVC_FREEPTR(pMvc_vui_parameter->mvc_vcl_hrd_parameters);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_low_delay_hrd_flag);
    MVC_FREEPTR(pMvc_vui_parameter->vui_mvc_pic_struct_present_flag);
#endif
    return MVC_OK;
}

/***********************************************************************************
* Function: MVC_ProcessSUBSPSMvcExt(MVC_CTX_S pMvcCtx,MVC_SUBSPS_S *pSubsps)
* Description: ����SUBSPS��MVC extension syntax
* Input:        ����ʱ����������
* Output:    
* Return:      MVC_OK  �������� 
                   MVC_ERR ��������
*              
* Others:      ��
***********************************************************************************/
SINT32 MVC_ProcessSUBSPSMvcExt(MVC_CTX_S *pMvcCtx,MVC_SUBSPS_S *pSubsps)
{
    UINT32 i,j,k;
    UINT32 num_level;
    UINT32 num_ops;
    UINT32 applicable_op_num_target_views;

    CHECK_NULL_PTR_ReturnValue(pSubsps, MVC_ERR);

    pSubsps->num_views_minus1 = mvc_ue_v( pMvcCtx,"SUBSPS: num_views_minus1" );
    if ( (pSubsps->num_views_minus1>MVC_MAX_NUM_VIEWS-1)|| pMvcCtx->SeErrFlg)
    {
        pSubsps->num_level_values_signalled_minus1 = 0xfffe;
        dprint(PRN_ERROR, "pSubsps->num_views_minus1(%d) out of range.\n", pSubsps->num_views_minus1);
        return MVC_ERR; 
    }
   
    for (i=0;i<=pSubsps->num_views_minus1;i++)
    {
        pSubsps->view_id[i] = mvc_ue_v( pMvcCtx,"SUBSPS: view_id[]" );
    }

    pSubsps->num_anchor_refs[0][0]=0;
    pSubsps->num_anchor_refs[1][0]=0;
    for (i=1;i<=pSubsps->num_views_minus1;i++)
    {
        pSubsps->num_anchor_refs[0][i] = mvc_ue_v( pMvcCtx,"SUBSPS: num_anchor_refs_l0[]" );
        if ( (pSubsps->num_anchor_refs[0][i]>MIN(15,pSubsps->num_views_minus1))|| pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSubsps->num_anchor_refs_l0(%d) out of range.\n", pSubsps->num_anchor_refs[0][i]);
            return MVC_ERR; 
        }
		
        for (j=0;j<pSubsps->num_anchor_refs[0][i];j++)
        {
            pSubsps->anchor_ref[0][i][j] = mvc_ue_v( pMvcCtx,"SUBSPS: anchor_ref_l0[][]" );
        }

        pSubsps->num_anchor_refs[1][i] = mvc_ue_v( pMvcCtx,"SUBSPS: num_anchor_refs_l1[]" );
        if ( (pSubsps->num_anchor_refs[1][i]>MIN(15,pSubsps->num_views_minus1))|| pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSubsps->num_anchor_refs_l1(%d) out of range.\n", pSubsps->num_anchor_refs[1][i]);
            return MVC_ERR;
        }

        for (j=0;j<pSubsps->num_anchor_refs[1][i];j++)
        {
            pSubsps->anchor_ref[1][i][j] = mvc_ue_v( pMvcCtx,"SUBSPS: anchor_ref_l1[][]" );
        }
    }

    pSubsps->num_non_anchor_refs[0][0] = 0;
    pSubsps->num_non_anchor_refs[1][0] = 0;
    for (i=1;i<=pSubsps->num_views_minus1;i++)
    {
        pSubsps->num_non_anchor_refs[0][i] = mvc_ue_v( pMvcCtx,"SUBSPS: num_non_anchor_refs_l0[]" );
        if ( (pSubsps->num_non_anchor_refs[0][i]>MIN(15,pSubsps->num_views_minus1))|| pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSubsps->num_non_anchor_refs_l0(%d) out of range.\n", pSubsps->num_non_anchor_refs[0][i]);
            return MVC_ERR;            
        }	 	

        for (j=0;j<pSubsps->num_non_anchor_refs[0][i];j++)
        {
            pSubsps->non_anchor_ref[0][i][j] = mvc_ue_v( pMvcCtx,"SUBSPS: non_anchor_ref_l0[][]" );
        }

        pSubsps->num_non_anchor_refs[1][i] = mvc_ue_v( pMvcCtx,"SUBSPS: num_non_anchor_refs_l1[]" );
        if ( (pSubsps->num_non_anchor_refs[1][i]>MIN(15,pSubsps->num_views_minus1))|| pMvcCtx->SeErrFlg)
        {
            dprint(PRN_ERROR, "pSubsps->num_non_anchor_refs_l1(%d) out of range.\n", pSubsps->num_non_anchor_refs[1][i]);
            return MVC_ERR;            
        }

        for (j=0;j<pSubsps->num_non_anchor_refs[1][i];j++)
        {
            pSubsps->non_anchor_ref[1][i][j] = mvc_ue_v( pMvcCtx,"SUBSPS: non_anchor_ref_l1[][]" ); 
        }
    }

    pSubsps->num_level_values_signalled_minus1 = mvc_ue_v( pMvcCtx,"SUBSPS: num_level_values_signalled_minus1" );
    num_level = pSubsps->num_level_values_signalled_minus1 + 1;
    if ( (num_level>16)|| pMvcCtx->SeErrFlg )  // ԭ����num_level>64�ģ����ǿ��ܻ������������Խ��
    {
        pSubsps->num_level_values_signalled_minus1 = 0xfffe;
        dprint(PRN_ERROR, "num_level(%d) out of range.\n", num_level);
        return MVC_ERR;            
    }
    #if 0
    pSubsps->level_idc = (UINT32 *)MALLOC(num_level*sizeof(UINT32),4);
    pSubsps->num_applicable_ops_minus1 = (UINT32 *)MALLOC(num_level*sizeof(UINT32),4);
    pSubsps->applicable_op_temporal_id = (UINT32 **)MALLOC(num_level*sizeof(UINT32 *),4);
    pSubsps->applicable_op_num_target_views_minus1 = (UINT32 **)MALLOC(num_level*sizeof(UINT32 *),4);
    pSubsps->applicable_op_target_view_id = (UINT32 ***)MALLOC(num_level*sizeof(UINT32 **),4);
    pSubsps->applicable_op_num_views_minus1 = (UINT32 **)MALLOC(num_level*sizeof(UINT32 *),4);
    #endif
    for (i=0;i<num_level;i++)
    {
        pSubsps->level_idc[i] = mvc_u_v(pMvcCtx,8, "SUBSPS: level_idc[]" );
        pSubsps->num_applicable_ops_minus1[i] = mvc_ue_v( pMvcCtx,"SUBSPS: num_applicable_ops_minus1" );
        num_ops = pSubsps->num_applicable_ops_minus1[i] + 1;
        if ( (num_ops>64)|| pMvcCtx->SeErrFlg) //ԭΪ num_ops>1024
        {
            pSubsps->num_applicable_ops_minus1[i] = 0xfffe;
            dprint(PRN_ERROR, "num_ops(%d) out of range.\n", num_ops);
            return MVC_ERR;            
        }
        #if 0
        pSubsps->applicable_op_temporal_id[i] = (UINT32 *)MALLOC(num_ops*sizeof(UINT32),4);
        pSubsps->applicable_op_num_target_views_minus1[i] = (UINT32 *)MALLOC(num_ops*sizeof(UINT32),4);
        pSubsps->applicable_op_target_view_id[i] = (UINT32 **)MALLOC(num_ops*sizeof(UINT32 *),4);
        pSubsps->applicable_op_num_views_minus1[i] = (UINT32 *)MALLOC(num_ops*sizeof(UINT32),4);
		#endif
        for (j=0;j<num_ops;j++)
        {
            pSubsps->applicable_op_temporal_id[i][j] = mvc_u_v(pMvcCtx,3, "SUBSPS: applicable_op_temporal_id[][]" );
            pSubsps->applicable_op_num_target_views_minus1[i][j] = mvc_ue_v( pMvcCtx,"SUBSPS: applicable_op_num_target_views_minus1[][]" );
            applicable_op_num_target_views = pSubsps->applicable_op_num_target_views_minus1[i][j] + 1;
            if ( (applicable_op_num_target_views>64)|| pMvcCtx->SeErrFlg) // ԭΪapplicable_op_num_target_views>1024
            {
                pSubsps->applicable_op_num_target_views_minus1[i][j] = 0xfffe;
                dprint(PRN_ERROR, "applicable_op_num_target_views(%d) out of range.\n", applicable_op_num_target_views);
                return MVC_ERR;            
            }
            #if 0
            pSubsps->applicable_op_target_view_id[i][j] = (UINT32 *)MALLOC(applicable_op_num_target_views*sizeof(UINT32),4);
			#endif
            for (k=0;k<applicable_op_num_target_views;k++)
            {
                pSubsps->applicable_op_target_view_id[i][j][k] = mvc_ue_v( pMvcCtx,"SUBSPS: applicable_op_target_view_id[][][]" );				
            }

            pSubsps->applicable_op_num_views_minus1[i][j] = mvc_ue_v( pMvcCtx,"SUBSPS: applicable_op_num_views_minus1" );
            if ( (pSubsps->applicable_op_num_views_minus1[i][j]>1023)|| pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "pSubsps->applicable_op_num_views_minus1(%d) out of range.\n", pSubsps->applicable_op_num_views_minus1[i][j]);
                return MVC_ERR;            
            }
        }
    }

    return MVC_OK;
}

/***********************************************************************************
* Function: MVC_ProcessSUBSPSMvcVuiExt(MVC_CTX_S pMvcCtx,MVC_SUBSPS_S *pSubsps)
* Description: ����SUBSPS��MVC VUI parameter extension syntax
* Input:        ����ʱ����������
* Output:    
* Return:      MVC_OK  �������� 
                   MVC_ERR ��������
*              
* Others:      ��
***********************************************************************************/
SINT32 MVC_ProcessSUBSPSMvcVuiExt(MVC_CTX_S *pMvcCtx,MVC_SUBSPS_S *pSubsps)
{
    UINT32 i,j;
    UINT32 num_ops;
    UINT32 num_target_output_views;
    MVCVUI_SUBSPS_S *mvc_vui_parameters = &pSubsps->mvcvui_subseq_parameters;

    mvc_vui_parameters->vui_mvc_num_ops_minus1 = mvc_ue_v( pMvcCtx,"MVC VUI: vui_mvc_num_ops_minus1" );

    if (1023 < mvc_vui_parameters->vui_mvc_num_ops_minus1)
    {
        dprint(PRN_ERROR, "mvc_vui_parameters->vui_mvc_num_ops_minus1:%d out of range[0,1023]\n",
               mvc_vui_parameters->vui_mvc_num_ops_minus1);
        return MVC_ERR;
    }

    /*20150318 CodeCC, ����:Ϊʲônum_ops��Ҫ��64λ������? �鿴��MVCЭ���׼:
      The value of vui_mvc_num_ops_minus1 shall be in the range of 0 to 1023, �ʽ�
      �䶨���޸�ΪUINT32
    */
    num_ops = mvc_vui_parameters->vui_mvc_num_ops_minus1 + 1;

    /*20150318 CodeCC, Ϊʲô����Ķ���Ϊ MVC_MAX_NUM_64? ��Ϊ���ֵ��vui_mvc_num_ops_minus1
      �ı�׼ȡֵ��Χ��һ����ʡ�ڴ滹��?
    */
    if (num_ops >= MVC_MAX_NUM_64)
    {
        dprint(PRN_ERROR, "num_ops:%d out of range[0,63]\n", num_ops);
        return MVC_ERR;
    }

    for (i=0;i<num_ops;i++)
    {
        mvc_vui_parameters->vui_mvc_temporal_id[i] = mvc_u_v(pMvcCtx,3, "MVC VUI: vui_mvc_temporal_id[]" );
        mvc_vui_parameters->vui_mvc_num_target_output_views_minus1[i] = mvc_ue_v( pMvcCtx,"SUBSPS VUI: vui_mvc_num_target_output_views_minus1[]" );

        /*20150318 CodeCC, ����:Ϊʲônum_ops��Ҫ��64λ������? �鿴��MVCЭ���׼:
          The value of vui_mvc_num_ops_minus1 shall be in the range of 0 to 1023, �ʽ�
          �䶨���޸�ΪUINT32
        */
        if (1023 < mvc_vui_parameters->vui_mvc_num_target_output_views_minus1[i])
        {
            dprint(PRN_ERROR, "vui_mvc_num_target_output_views_minus1[%d]:%d out of range[0,1023]\n",
                   i, mvc_vui_parameters->vui_mvc_num_target_output_views_minus1[i]);
            return MVC_ERR;
        }

        num_target_output_views = mvc_vui_parameters->vui_mvc_num_target_output_views_minus1[i] + 1;

        /*20150318 CodeCC, Ϊʲô����Ķ���Ϊ MVC_MAX_NUM_64? ��Ϊ���ֵ��vui_mvc_num_target_output_views_minus1
          �ı�׼ȡֵ��Χ��һ����ʡ�ڴ滹��?
        */
        if (num_target_output_views >= MVC_MAX_NUM_64)
        {
            dprint(PRN_ERROR, "num_target_output_views:%d out of range[0,63]\n", num_target_output_views);
            return MVC_ERR;
        }

        for (j = 0; j < num_target_output_views; j++)
        {
            mvc_vui_parameters->vui_mvc_view_id[i][j] = mvc_ue_v( pMvcCtx,"MVC VUI: vui_mvc_view_id[][]" );
        }

        mvc_vui_parameters->vui_mvc_timing_info_present_flag[i] = mvc_u_1( pMvcCtx,"MVC VUI: vui_mvc_timing_info_present_flag" );

        if (mvc_vui_parameters->vui_mvc_timing_info_present_flag[i])
        {
            mvc_vui_parameters->vui_mvc_num_units_in_tick[i] = mvc_u_v(pMvcCtx,32,"MVC VUI: vui_mvc_num_units_in_tick" );
            mvc_vui_parameters->vui_mvc_time_scale[i] = mvc_u_v(pMvcCtx,32,"MVC VUI: vui_mvc_time_scale[]");
            mvc_vui_parameters->vui_mvc_fixed_frame_rate_flag[i] = mvc_u_1( pMvcCtx,"MVC VUI: vui_mvc_fixed_frame_rate_flag" );
        }

        mvc_vui_parameters->vui_mvc_nal_hrd_parameters_present_flag[i] = mvc_u_1(pMvcCtx,"MVC VUI: vui_mvc_nal_hrd_parameters_present_flag[]");
        if (mvc_vui_parameters->vui_mvc_nal_hrd_parameters_present_flag[i])
        {
            UINT32 SchedSelIdx;
            MVC_HRD_PARAMETERS_S *hrd = &(mvc_vui_parameters->mvc_nal_hrd_parameters[i]);

            hrd->cpb_cnt_minus1 = mvc_ue_v (pMvcCtx,"MVC VUI: cpb_cnt_minus1");
            hrd->bit_rate_scale   = mvc_u_v  (pMvcCtx,4, "MVC VUI: bit_rate_scale");
            hrd->cpb_size_scale  = mvc_u_v  (pMvcCtx,4, "MVC VUI: cpb_size_scale");

            if ((hrd->cpb_cnt_minus1>31) || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");
                return MVC_ERR;
            }

            for (SchedSelIdx=0; SchedSelIdx<=hrd->cpb_cnt_minus1; SchedSelIdx++) 
            {
                hrd->bit_rate_value_minus1[SchedSelIdx]  = mvc_ue_v (pMvcCtx,"MVC VUI: bit_rate_value_minus1");
                hrd->cpb_size_value_minus1[SchedSelIdx] = mvc_ue_v (pMvcCtx,"MVC VUI: cpb_size_value_minus1");
                hrd->cbr_flag[SchedSelIdx]                     = mvc_u_1  (pMvcCtx,"MVC VUI: cbr_flag");
            }

            hrd->initial_cpb_removal_delay_length_minus1 = mvc_u_v(pMvcCtx,5, "MVC VUI: initial_cpb_removal_delay_length_minus1");
            hrd->cpb_removal_delay_length_minus1         = mvc_u_v(pMvcCtx,5, "MVC VUI: cpb_removal_delay_length_minus1");
            hrd->dpb_output_delay_length_minus1          = mvc_u_v(pMvcCtx,5, "MVC VUI: dpb_output_delay_length_minus1");
            hrd->time_offset_length                              = mvc_u_v(pMvcCtx,5, "MVC VUI: time_offset_length");
        }

        mvc_vui_parameters->vui_mvc_vcl_hrd_parameters_present_flag[i] = mvc_u_1(pMvcCtx,"MVC VUI: vui_mvc_vcl_hrd_parameters_present_flag[]" );
        if (mvc_vui_parameters->vui_mvc_vcl_hrd_parameters_present_flag[i])
        {
            UINT32 SchedSelIdx;
            MVC_HRD_PARAMETERS_S *hrd = &(mvc_vui_parameters->mvc_vcl_hrd_parameters[i]);

            hrd->cpb_cnt_minus1 = mvc_ue_v (pMvcCtx,"MVC VUI: cpb_cnt_minus1");
            hrd->bit_rate_scale   = mvc_u_v  (pMvcCtx,4, "MVC VUI: bit_rate_scale");
            hrd->cpb_size_scale  = mvc_u_v  (pMvcCtx,4, "MVC VUI: cpb_size_scale");
            if ((hrd->cpb_cnt_minus1>31) || pMvcCtx->SeErrFlg)
            {
                dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");        
                return MVC_ERR;
            }
            for (SchedSelIdx=0; SchedSelIdx<=hrd->cpb_cnt_minus1; SchedSelIdx++) 
            {
                hrd->bit_rate_value_minus1[SchedSelIdx]  = mvc_ue_v (pMvcCtx,"MVC VUI: bit_rate_value_minus1");
                hrd->cpb_size_value_minus1[SchedSelIdx] = mvc_ue_v (pMvcCtx,"MVC VUI: cpb_size_value_minus1");
                hrd->cbr_flag[SchedSelIdx]                     = mvc_u_1 (pMvcCtx,"MVC VUI: cbr_flag");
            }

            hrd->initial_cpb_removal_delay_length_minus1 = mvc_u_v  (pMvcCtx,5, "MVC VUI: initial_cpb_removal_delay_length_minus1");
            hrd->cpb_removal_delay_length_minus1         = mvc_u_v  (pMvcCtx,5, "MVC VUI: cpb_removal_delay_length_minus1");
            hrd->dpb_output_delay_length_minus1           = mvc_u_v  (pMvcCtx,5, "MVC VUI: dpb_output_delay_length_minus1");
            hrd->time_offset_length                               = mvc_u_v  (pMvcCtx,5, "MVC VUI: time_offset_length");
        }

        if (mvc_vui_parameters->vui_mvc_nal_hrd_parameters_present_flag[i] ||mvc_vui_parameters->vui_mvc_vcl_hrd_parameters_present_flag[i])
        {
            mvc_vui_parameters->vui_mvc_low_delay_hrd_flag[i] = mvc_u_1(pMvcCtx,"MVC VUI: vui_mvc_low_delay_hrd_flag[]");
        }

        mvc_vui_parameters->vui_mvc_pic_struct_present_flag[i] = mvc_u_1(pMvcCtx,"MVC VUI: vui_mvc_pic_struct_present_flag[]");
    }

    return MVC_OK;
}

/***********************************************************************************
* Function: MVC_DecSubSPS(MVC_CTX_S *pMvcCtx)
* Description: ����SubSPS
* Input:      ����������ʱ����������
* Output:    ֻ�ı�SUBSPS������һ��Ԫ�ؽṹ��
* Return:   DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_DecSubSPS(MVC_CTX_S *pMvcCtx)
{
    UINT32 profile_idc;
    UINT32 constrained_set0_flag;
    UINT32 constrained_set1_flag;
    UINT32 constrained_set2_flag;
    UINT32 constrained_set3_flag;
    UINT32 constrained_set4_flag;
    UINT32 constrained_set5_flag;
    UINT32 level_idc;
    UINT32 seq_parameter_set_id;

    UINT32 orig_profile_idc;
    UINT32 orig_mvc_vui_present_flag;

    profile_idc = mvc_u_v (pMvcCtx, 8, "SUBSPS: profile_idc");
    constrained_set0_flag = mvc_u_1 (pMvcCtx, "SUBSPS: constrained_set0_flag");
    constrained_set1_flag = mvc_u_1 (pMvcCtx, "SUBSPS: constrained_set1_flag");
    constrained_set2_flag = mvc_u_1 (pMvcCtx, "SUBSPS: constrained_set2_flag");
    constrained_set3_flag = mvc_u_1 (pMvcCtx, "SUBSPS: constrained_set3_flag");
    constrained_set4_flag = mvc_u_1 (pMvcCtx, "SUBSPS: constrained_set4_flag");
    constrained_set5_flag = mvc_u_1 (pMvcCtx, "SUBSPS: constrained_set5_flag");
    mvc_u_v (pMvcCtx, 2, "SUBSPS: reserved_zero_2bits"); // reserved_zero
    level_idc             = mvc_u_v (pMvcCtx, 8, "SUBSPS: level_idc");
    seq_parameter_set_id  = mvc_ue_v(pMvcCtx, "SUBSPS: seq_parameter_set_id");
    if (pMvcCtx->SeErrFlg)
    {
        return MVC_ERR;
    }

    if (seq_parameter_set_id > 31)
    {
        dprint(PRN_ERROR, "seq_parameter_set_id out of range.\n");

        // ���Կ���ǯλǿ��
        return MVC_ERR;
    }	

     switch (profile_idc)
    {
        case MVC_BASELINE:
            dprint(PRN_ERROR, "MVC_BASELINE stream, try to decode, exit when FMO occurred.\n");
            break;        
        case MVC_EXTENDED:
            dprint(PRN_ERROR, "MVC_EXTENDED stream, try to decode, exit when datapartition occurred.\n");
            break;
        case MVC_MAINPROFILE:
            break;
        case MVC_HIGHPROFILE:
            break;
        case MVC_MULTIVIEWHIGH:
            break;
        case STEREOHIGH:
            break;
        case MVC_HIGH10P:
        case MVC_HIGH422P:
        case MVC_HIGH444P:
            dprint(PRN_ERROR, "others High profile stream, try to decode, exit when high profile not support occurred.\n");
            break;                    
        default:
            dprint(PRN_ERROR, "profile_idc = %5d error\n", profile_idc);
            return MVC_ERR;
            break;
    }

    if (level_idc > 41)
    {
        dprint(PRN_ERROR, "level_idc = %5d error, try to decode as level_idc 41.\n", level_idc);
        level_idc = 41;
    }

    if(pMvcCtx->SUBSPS[seq_parameter_set_id].is_valid)
    {
        orig_profile_idc = pMvcCtx->SUBSPS[seq_parameter_set_id].sps.profile_idc;

        if(MVC_MULTIVIEWHIGH == orig_profile_idc || STEREOHIGH == orig_profile_idc)
        {
            //MVC_FreeMvcExtMem(&pMvcCtx->SUBSPS[seq_parameter_set_id]);

            orig_mvc_vui_present_flag = pMvcCtx->SUBSPS[seq_parameter_set_id].mvc_vui_parameters_present_flag;
            if(orig_mvc_vui_present_flag)
            {
                //MVC_FreeMvcVuiExtMem(&pMvcCtx->SUBSPS[seq_parameter_set_id].mvcvui_subseq_parameters);
            }
        }
    }

    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.profile_idc = profile_idc;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.constrained_set0_flag = constrained_set0_flag;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.constrained_set1_flag = constrained_set1_flag;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.constrained_set2_flag = constrained_set2_flag;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.constrained_set3_flag = constrained_set3_flag;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.constrained_set4_flag = constrained_set4_flag;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.constrained_set5_flag = constrained_set5_flag;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.level_idc = level_idc;
    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.seq_parameter_set_id = seq_parameter_set_id;

    if (MVC_OK != MVC_ProcessSPS(pMvcCtx,&pMvcCtx->SUBSPS[seq_parameter_set_id].sps))
    {
        dprint(PRN_ERROR, "SUBSPS decode error.\n");
        pMvcCtx->SUBSPS[seq_parameter_set_id].is_valid = 0;
        pMvcCtx->SUBSPS[seq_parameter_set_id].sps.is_valid = 0;
        return MVC_ERR;
    }

    pMvcCtx->SUBSPS[seq_parameter_set_id].sps.is_valid = 1;

    if (MVC_MULTIVIEWHIGH == profile_idc || STEREOHIGH == profile_idc)
    {
        pMvcCtx->SUBSPS[seq_parameter_set_id].bit_equal_to_one = mvc_u_1( pMvcCtx,"SUBSPS: bit_equal_to_one" );

	 if (MVC_OK != MVC_ProcessSUBSPSMvcExt(pMvcCtx,&pMvcCtx->SUBSPS[seq_parameter_set_id]))
        {
            dprint(PRN_ERROR, "SUBSPS Mvc Ext decode error.\n");
            //MVC_FreeMvcExtMem(&pMvcCtx->SUBSPS[seq_parameter_set_id]);
            pMvcCtx->SUBSPS[seq_parameter_set_id].is_valid = 0;
            return MVC_ERR;
        }

        pMvcCtx->SUBSPS[seq_parameter_set_id].mvc_vui_parameters_present_flag = mvc_u_1( pMvcCtx,"SUBSPS: mvc_vui_parameters_present_flag" );
        if (pMvcCtx->SUBSPS[seq_parameter_set_id].mvc_vui_parameters_present_flag)
        {
            if (MVC_OK != MVC_ProcessSUBSPSMvcVuiExt(pMvcCtx,&pMvcCtx->SUBSPS[seq_parameter_set_id]))
            {
                dprint(PRN_ERROR, "SUBSPS Mvc Vui Ext decode error.\n");
                //MVC_FreeMvcVuiExtMem(&pMvcCtx->SUBSPS[seq_parameter_set_id].mvcvui_subseq_parameters);
                pMvcCtx->SUBSPS[seq_parameter_set_id].is_valid = 0;
                return MVC_ERR;
            }
        }
    }

    pMvcCtx->SUBSPS[seq_parameter_set_id].is_valid = 1;

    return MVC_OK;
}


/***********************************************************************************
* SINT32    MVC_PassBytes(MVC_CTX_S *pMvcCtx, UINT32 len)
* Description: ����SEI
* Input:       pStream             ����������ʱ����������
*              size                SEI Nalu��С
* Output:      g_RecoverPointFlg   ����ָ����־
*              pSEI                �����SEI��Ϣ�ṹ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_PassBytes(MVC_CTX_S *pMvcCtx, UINT32 len)
{
    UINT32 segment_idx;

    if (NULL == pMvcCtx->pCurrNal || 0 >= len)
    {
        return -1;
    }

    if (NULL == pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].streamBuffer)
    {
        return -1;
    }

    if (2 < pMvcCtx->pCurrNal->nal_segment || 1 < pMvcCtx->pCurrNal->nal_used_segment ||
        (UINT32)pMvcCtx->pCurrNal->nal_used_segment >= pMvcCtx->pCurrNal->nal_segment)
    {
        return -1;
    } 

    len *= 8;
    for (segment_idx = pMvcCtx->pCurrNal->nal_used_segment; segment_idx < pMvcCtx->pCurrNal->nal_segment; segment_idx++)
    {
        if ((len + pMvcCtx->pCurrNal->stream[segment_idx].bitsoffset) <= (pMvcCtx->pCurrNal->stream[segment_idx].bitstream_length*8))
        {
            pMvcCtx->pCurrNal->stream[segment_idx].bitsoffset += len;
            
            // pass stream succeed
            return 1;
        }
        else
        {
            len = len + pMvcCtx->pCurrNal->stream[segment_idx].bitsoffset - pMvcCtx->pCurrNal->stream[segment_idx].bitstream_length*8;
            pMvcCtx->pCurrNal->stream[segment_idx].bitsoffset = pMvcCtx->pCurrNal->stream[segment_idx].bitstream_length*8;
            pMvcCtx->pCurrNal->nal_used_segment++;
        }
    }

    //no stream
    return 0;  
}


/***********************************************************************************
* Function:    SINT32    MVC_GetBytes(MVC_CTX_S *pMvcCtx, UINT8 * pdes, UINT32 len)
* Description: ����SEI
* Input:       pStream             ����������ʱ����������
*              size                SEI Nalu��С
* Output:      g_RecoverPointFlg   ����ָ����־
*              pSEI                �����SEI��Ϣ�ṹ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_GetBytes(MVC_CTX_S *pMvcCtx, UINT8 *pdes, UINT32 len)
{
    UINT32 segment_idx;
    UINT8  *byte_pos, *end_byte;
    UINT32 cnt;

    if (NULL == pdes || NULL == pMvcCtx->pCurrNal || 0 >= len)
    {
        return -1;
    }
    
    if (NULL == pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_used_segment].streamBuffer)
    {
        return -1;
    }
    
    if (2 < pMvcCtx->pCurrNal->nal_segment || 1 < pMvcCtx->pCurrNal->nal_used_segment ||
       (UINT32)pMvcCtx->pCurrNal->nal_used_segment >= pMvcCtx->pCurrNal->nal_segment)
    {
        return -1;
    }
    
    cnt = 0;
    for (segment_idx = pMvcCtx->pCurrNal->nal_used_segment; segment_idx < pMvcCtx->pCurrNal->nal_segment; segment_idx++)
    {
        byte_pos =  pMvcCtx->pCurrNal->stream[segment_idx].streamBuffer + 
            ((pMvcCtx->pCurrNal->stream[segment_idx].bitsoffset + 7)/8);
        end_byte = pMvcCtx->pCurrNal->stream[segment_idx].streamBuffer + pMvcCtx->pCurrNal->stream[segment_idx].bitstream_length;
        while (byte_pos < end_byte) 
        {
            *(pdes++) = *(byte_pos);
            cnt++;
			byte_pos++;
            pMvcCtx->pCurrNal->stream[segment_idx].bitsoffset += 8;
            if (cnt >= len)
            {
                return len;
            } 
        }
        pMvcCtx->pCurrNal->nal_used_segment++;
		#if 0
        if ((segment_idx+1) == pMvcCtx->pCurrNal->nal_segment)
        {
            return -1;
        }
		#endif
    }
    
    return cnt;  
}


SINT32 MVC_DecFramePackingSEI(MVC_CTX_S *pMvcCtx, SINT32 PayloadSize)
{
    MVC_FRAME_PACKING_ARRANGEMENT_S  *pFramePacking = &pMvcCtx->FramePackSEI;
    
    pFramePacking->frame_packing_arrangement_id = mvc_ue_v(pMvcCtx, "SEI: frame_packing_arrangement_id");
    pFramePacking->frame_packing_arrangement_cancel_flag = mvc_u_v(pMvcCtx, 1, "SEI: frame_packing_arrangement_cancel_flag");

    if (!pFramePacking->frame_packing_arrangement_cancel_flag)
    {
        pFramePacking->frame_packing_arrangement_type = mvc_u_v(pMvcCtx, 7, "SEI: frame_packing_arrangement_type");
        pFramePacking->quincunx_sampling_flag = mvc_u_v(pMvcCtx, 1, "SEI: quincunx_sampling_flag");
        pFramePacking->content_interpretation_type = mvc_u_v(pMvcCtx, 6, "SEI: content_interpretation_type");
        pFramePacking->spatial_flipping_flag = mvc_u_v(pMvcCtx, 1, "SEI: spatial_flipping_flag");
        pFramePacking->frame0_flipped_flag = mvc_u_v(pMvcCtx, 1, "SEI: frame0_flipped_flag");
        pFramePacking->field_views_flag = mvc_u_v(pMvcCtx, 1, "SEI: field_views_flag");
        pFramePacking->current_frame_is_frame0_flag = mvc_u_v(pMvcCtx, 1, "SEI: current_frame_is_frame0_flag");
        pFramePacking->frame0_self_contained_flag = mvc_u_v(pMvcCtx, 1, "SEI: frame0_self_contained_flag");
        pFramePacking->frame1_self_contained_flag = mvc_u_v(pMvcCtx, 1, "SEI: frame1_self_contained_flag");

        if (!pFramePacking->quincunx_sampling_flag && pFramePacking->frame_packing_arrangement_type != 5)
        {
            pFramePacking->frame0_grid_position_x = mvc_u_v(pMvcCtx, 1, "SEI: frame0_grid_position_x");
            pFramePacking->frame0_grid_position_y = mvc_u_v(pMvcCtx, 1, "SEI: frame0_grid_position_y");
            pFramePacking->frame1_grid_position_x = mvc_u_v(pMvcCtx, 1, "SEI: frame1_grid_position_x");
            pFramePacking->frame1_grid_position_y = mvc_u_v(pMvcCtx, 1, "SEI: frame1_grid_position_y");
        }
        pFramePacking->frame_packing_arrangement_reserved_byte = mvc_u_v(pMvcCtx, 8, "SEI: frame_packing_arrangement_reserved_byte");
        pFramePacking->frame_packing_arrangement_repetition_period = mvc_ue_v(pMvcCtx, "SEI: frame_packing_arrangement_repetition_period");
    }

    pFramePacking->frame_packing_arrangement_extension_flag = mvc_u_v(pMvcCtx, 1, "SEI: frame_packing_arrangement_extension_flag");

    pFramePacking->frame_packing_dec_ok = 1;
    return MVC_OK;
}


SINT32 MVC_DecPicTimingSEI(MVC_CTX_S *pMvcCtx, SINT32 PayloadSize)
{
    MVC_PIC_TIMING_SEI_S  *pPicTiming = &pMvcCtx->PicTimingSEI;
    MVC_SPS_S  *pSPS;
    SINT32 cpb_removal_len =0 ;
    SINT32 dpb_output_len =0 ;
    SINT32 i = 0;
	
    pSPS = &pMvcCtx->SPS[pMvcCtx->CurrSPS.seq_parameter_set_id];
	if (!(pSPS->is_valid))
	{
        pSPS = NULL;
	    for(i=0; i<pMvcCtx->MaxSpsNum; i++)
	    {
	        if (pMvcCtx->SPS[i].is_valid)
	        {
	            pSPS = &pMvcCtx->SPS[i];
	            break;
	        }
	    }
        if (pSPS == NULL)
        {
            dprint(PRN_ERROR, "DecPicTimingSEI but SPS is invalid.\n");
	        return MVC_ERR;
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
        
        pPicTiming->cpb_removal_delay = mvc_u_v(pMvcCtx, cpb_removal_len, "SEI: cpb_removal_delay");
        pPicTiming->dpb_output_delay  = mvc_u_v(pMvcCtx, dpb_output_len, "SEI: dpb_output_delay");
    }

    if (pSPS->vui_seq_parameters.pic_struct_present_flag)
    {
        pPicTiming->pic_struct = mvc_u_v(pMvcCtx, 4, "pic_struct");
    }

    return MVC_OK;
}


#define MVC_MAX_PAYLOAD_SIZE  (400*1024)

/***********************************************************************************
* Function:    MVC_DecSEI(MVC_CTX_S *pMvcCtx)
* Description: ����SEI
* Input:       pStream             ����������ʱ����������
*              size                SEI Nalu��С
* Output:      g_RecoverPointFlg   ����ָ����־
*              pSEI                �����SEI��Ϣ�ṹ��
* Return:      DEC_NORMAL          ���Խ����Nalu����
*              DEC_ERR             �����Խ���
* Others:      ��
***********************************************************************************/ 
SINT32 MVC_DecSEI(MVC_CTX_S *pMvcCtx)
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
    SINT32 bitpos;
    SINT32 i;
    SINT32 nBits;
    SINT32 used_byte;
    SINT8 registered_flag = 0;

    //skip nal type
    pMvcCtx->pCurrNal->stream[0].bitsoffset = 32;
    
    payloadType = 0;

    do
    {
        ret = MVC_GetBytes(pMvcCtx, &byte_tmp, 1);
        payloadType += ((byte_tmp == 0xFF) ? 255 : byte_tmp);
        nBits = 8;
        if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
        {
            ret = 0;
        }
        else
        {
           BsSkip(&pMvcCtx->BS, nBits);
        }
    }
    while ((0 < ret) && (0xFF == byte_tmp));
    
    if (0 >= ret)
    {
        dprint(PRN_ERROR, "SEI nal dec payload type err\n");
        return MVC_ERR;
    }
    
    
    do // sei_message()
    {        
        payloadSize = 0;
        do
        {
            ret = MVC_GetBytes(pMvcCtx, &byte_tmp, 1);
            payloadSize += ((byte_tmp == 0xFF) ? 255 : byte_tmp);
            nBits = 8;
            if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
            {
                ret = 0;
            }
            else
            {
               BsSkip(&pMvcCtx->BS, nBits);
            }
        }
        while ((0 < ret) && (0xFF == byte_tmp));
        
        if ( (0 >= ret) || (payloadSize>MVC_MAX_PAYLOAD_SIZE) )
        {
            dprint(PRN_ERROR, "SEI nal dec payloadSize err\n");
            return MVC_ERR;
        }
        //just for dprint
        if (2 == pMvcCtx->pCurrNal->nal_segment)
        {
            TotalBitOffset = (pMvcCtx->pCurrNal->stream[0].bitsoffset +  pMvcCtx->pCurrNal->stream[1].bitsoffset)/8;
        }
        else
        {
            TotalBitOffset = pMvcCtx->pCurrNal->stream[0].bitsoffset;
        }
        //end of dprint calc
        switch (payloadType)     // sei_payload(payloadType, payloadSize)
        {
        case MVC_SEI_BUFFERING_PERIOD:
            dprint(PRN_SEI,"MVC_SEI_BUFFERING_PERIOD,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // buffering_period(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }

            break;
        case MVC_SEI_PIC_TIMING:
            dprint(PRN_SEI,"MVC_SEI_PIC_TIMING,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            bitpos = pMvcCtx->BS.TotalPos;
            MVC_DecPicTimingSEI(pMvcCtx, payloadSize);
            bitpos = pMvcCtx->BS.TotalPos - bitpos;
            if (8*payloadSize > bitpos)
            {
                for(i=0;i<((8*payloadSize-bitpos)/8);i++)
                {
                    nBits = 8;
                    if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pMvcCtx->BS, nBits);
                    }
                }
                nBits = (8*payloadSize-bitpos)%8;
                if ((pMvcCtx->BS.TotalPos+nBits)<=(pMvcCtx->BS.BsLen*8))
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            ret = MVC_PassBytes(pMvcCtx, payloadSize);
            break;
        case MVC_SEI_PAN_SCAN_RECT:
            dprint(PRN_SEI,"MVC_SEI_PAN_SCAN_RECT,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // pan_scan_rect(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);  
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_FILLER_PAYLOAD:
            dprint(PRN_SEI,"MVC_SEI_FILLER_PAYLOAD,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // filler_payload(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);      
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_USER_DATA_REGISTERED_ITU_T_T35:
            registered_flag = 1;
        case MVC_SEI_USER_DATA_UNREGISTERED:
            if (1 == registered_flag)
            {
                dprint(PRN_SEI, "MVC_SEI_USER_DATA_REGISTERED_ITU_T_T35, offset = %d, payload_size = %d \n", TotalBitOffset, payloadSize);
            }
            else
            {
                dprint(PRN_SEI, "MVC_SEI_USER_DATA_UNREGISTERED, offset = %d, payload_size = %d \n", TotalBitOffset, payloadSize);
            }
            
            pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum] = GetUsd(pMvcCtx->ChanID);
            if (NULL == pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum])
            {
                //dprint(PRN_ERROR, "cann't get usrdata pointer\n");
                return MVC_ERR;
            }
            min_len = MIN(MAX_USRDAT_SIZE, payloadSize);
			lenth_not_parse = 0;
			for (segment_idx = pMvcCtx->pCurrNal->nal_used_segment; segment_idx < pMvcCtx->pCurrNal->nal_segment; segment_idx++)
            {
                lenth_not_parse += pMvcCtx->pCurrNal->stream[segment_idx].bitstream_length - 
                                  ((pMvcCtx->pCurrNal->stream[segment_idx].bitsoffset + 7)/8);
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
            if (1 == registered_flag)    // MVC_SEI_USER_DATA_REGISTERED_ITU_T_T35 ������ز���   y00226912
            {
                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->IsRegistered = 1;
                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->itu_t_t35_country_code = mvc_u_v(pMvcCtx, 8, "SEI: itu_t_t35_country_code");
                if (pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->itu_t_t35_country_code != 0xFF)
                {
		            pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->itu_t_t35_country_code_extension_byte = 0;
		            used_byte = 1;
                }
                else
                {
	   	            pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->itu_t_t35_country_code_extension_byte = mvc_u_v(pMvcCtx, 8, "SEI: itu_t_t35_country_code_extension_byte");
		            used_byte = 2;
                }
                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->itu_t_t35_provider_code = mvc_u_v(pMvcCtx, 16, "SEI: itu_t_t35_provider_code");		      
                used_byte += 2;
                MVC_PassBytes(pMvcCtx, used_byte);

                if (used_byte == min_len_tmp)
                {
	                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->PTS = pMvcCtx->pstExtraData->pts;
                    pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->from = 8;
                    pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->seq_cnt = pMvcCtx->SeqCnt;
                    REPORT_USRDAT(pMvcCtx->ChanID, pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]);
                    break;
                }
		   }		
		 
            ret = MVC_GetBytes(pMvcCtx, pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->data, min_len_tmp-used_byte);
            if (0 >= ret)
            {
                dprint(PRN_ERROR, "cann't dec usrdata\n");
                FreeUsdByDec(pMvcCtx->ChanID, pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]);
                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum] = NULL;
                return MVC_ERR;
            }
            
            if (ret < ((SINT32)min_len-used_byte))
            {
                //return len smaller than the len would read, must no stream
                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->data_size =  ret;
                for(i=0;i<ret;i++)
                {
                    nBits = 8;
                    if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pMvcCtx->BS, nBits);
                    }
                }
            }
            else
            {
                pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->data_size = payloadSize - used_byte;
                
                //need pass the stream which don't write to usrdata but is apart of usrdata;
                if (payloadSize > min_len)
                {
                    ret = MVC_PassBytes(pMvcCtx, payloadSize - min_len);
                }
                for(i=0;i<(payloadSize-used_byte);i++)
                {
                    nBits = 8;
                    if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pMvcCtx->BS, nBits);
                    }
                }
            }

            pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->PTS = pMvcCtx->pstExtraData->pts;
            pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->from = 8;
            pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]->seq_cnt = pMvcCtx->SeqCnt;
            REPORT_USRDAT(pMvcCtx->ChanID, pMvcCtx->pUsrDatArray[pMvcCtx->TotalUsrDatNum]);
            break;
        case MVC_SEI_RECOVERY_POINT:
            dprint(PRN_SEI,"MVC_SEI_RECOVERY_POINT,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // recovery_point(pStream+offset, payloadSize, pSEI);
            // g_RecoverPointFlg = 1;
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            ret = MVC_PassBytes(pMvcCtx, payloadSize);                
            break;
        case MVC_SEI_DEC_REF_PIC_MARKING_REPETITION:
            dprint(PRN_SEI,"MVC_SEI_DEC_REF_PIC_MARKING_REPETITION,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // dec_ref_pic_marking_repetition(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_SPARE_PIC:
            dprint(PRN_SEI,"MVC_SEI_SPARE_PIC,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // spare_pic(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);      
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_SCENE_INFO:
            dprint(PRN_SEI,"MVC_SEI_SCENE_INFO,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // scene_info(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);   
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_SUB_SEQ_INFO:
            dprint(PRN_SEI,"MVC_SEI_SUB_SEQ_INFO,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // sub_seq_info(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);  
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_SUB_SEQ_LAYER_CHARACTERISTICS:
            dprint(PRN_SEI,"MVC_SEI_SUB_SEQ_LAYER_CHARACTERISTICS,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // sub_seq_layer_characteristics(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_SUB_SEQ_CHARACTERISTICS:
            dprint(PRN_SEI,"MVC_SEI_SUB_SEQ_CHARACTERISTICS,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // sub_seq_characteristics(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);   
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_FULL_FRAME_FREEZE:
            dprint(PRN_SEI,"MVC_SEI_FULL_FRAME_FREEZE,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // full_frame_freeze(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_FULL_FRAME_FREEZE_RELEASE:
            dprint(PRN_SEI,"MVC_SEI_FULL_FRAME_FREEZE_RELEASE,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // full_frame_freeze_release(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);   
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_FULL_FRAME_SNAPSHOT:
            dprint(PRN_SEI,"MVC_SEI_FULL_FRAME_SNAPSHOT,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // full_frame_snapshot(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START:
            dprint(PRN_SEI,"MVC_SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // progressive_refinement_segment_start(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);     
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END:
            dprint(PRN_SEI,"MVC_SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // progressive_refinement_segment_end(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);     
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET:
            dprint(PRN_SEI,"MVC_SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // motion_constrained_slice_group_set(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_FILM_GRAIN_CHARACTERISTICS:
            dprint(PRN_SEI,"MVC_SEI_FILM_GRAIN_CHARACTERISTICS,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // film_grain_characteristics(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE:
            dprint(PRN_SEI,"MVC_SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // deblocking_filter_display_preference(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_STEREO_VIDEO_INFO:
            dprint(PRN_SEI,"MVC_SEI_STEREO_VIDEO_INFO,offset = %d,payload_size = %d \n", TotalBitOffset,payloadSize);
            // stereo_video_info(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);   
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        case MVC_SEI_FRAME_PACKING_ARRANGEMENT:
            bitpos = pMvcCtx->BS.TotalPos;
            MVC_DecFramePackingSEI(pMvcCtx, payloadSize);
            bitpos = pMvcCtx->BS.TotalPos - bitpos;

            if (8*payloadSize > bitpos)
            {
                for(i=0;i<((8*payloadSize-bitpos)/8);i++)
                {
                    nBits = 8;
                    if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                    {
                        break;
                    }
                    else
                    {
                       BsSkip(&pMvcCtx->BS, nBits);
                    }
                }
                nBits = (8*payloadSize-bitpos)%8;
                if ((pMvcCtx->BS.TotalPos+nBits)<=(pMvcCtx->BS.BsLen*8))
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            ret = MVC_PassBytes(pMvcCtx, payloadSize);
            break;
        default:
            //dprint(PRN_SEI,"interpret_reserved_info,offset = %d,payload_size = %d \n",TotalBitOffset,payloadSize);
            // reserved_sei_message(pStream+offset, payloadSize, pSEI);
            ret = MVC_PassBytes(pMvcCtx, payloadSize);    
            for(i=0;i<payloadSize;i++)
            {
                nBits = 8;
                if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
                {
                    break;
                }
                else
                {
                   BsSkip(&pMvcCtx->BS, nBits);
                }
            }
            break;
        }

        if (4 <= pMvcCtx->TotalUsrDatNum)
        {
            dprint(PRN_ERROR, "too much usrdat, cann't dec SEI\n");
            return MVC_ERR;
        }

        payloadType = 0;
        do
        {
            ret = MVC_GetBytes(pMvcCtx, &byte_tmp, 1);
            payloadType += ((byte_tmp == 0xFF) ? 255 : byte_tmp);
            nBits = 8;
            if ((pMvcCtx->BS.TotalPos+nBits)>(pMvcCtx->BS.BsLen*8))
            {
                ret = 0;
            }
            else
            {
               BsSkip(&pMvcCtx->BS, nBits);
            }
        }
        while ((0 < ret) && (0xFF == byte_tmp));
    }
    while ((0 < ret) && (byte_tmp != 0x80));   // more_rbsp_data(), pStream[offset]!=0x80

    //RestoreSEIInputUT(pMvcCtx->TotalNal);
    return MVC_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  MVC_InitOldSlice(MVC_CTX_S  *pMvcCtx)
*   �������� :  ��ʼ��g_OldSlice, Ŀ����Ϊ��ʹ��һslice�����ж�new_pic_flag
*   ����˵�� :  ��
*   ����ֵ   �� ��
************************************************************************
*/
VOID MVC_InitOldSlice(MVC_CTX_S  *pMvcCtx)
{
    pMvcCtx->PrevSlice.pic_parameter_set_id = pMvcCtx->MaxPpsNum;
    pMvcCtx->PrevSlice.frame_num = 88888;
    pMvcCtx->PrevSlice.field_pic_flag = 0;
    pMvcCtx->PrevSlice.bottom_field_flag = 2;
    pMvcCtx->PrevSlice.nal_ref_idc = 7;
    pMvcCtx->PrevSlice.nal_unit_type = 32;
    pMvcCtx->PrevSlice.pic_oder_cnt_lsb = 3;
    pMvcCtx->PrevSlice.delta_pic_oder_cnt_bottom = -1;
    pMvcCtx->PrevSlice.delta_pic_order_cnt[0] = 0;
    pMvcCtx->PrevSlice.delta_pic_order_cnt[1] = 0;
    pMvcCtx->PrevSlice.first_mb_in_slice = MAX_MB_NUM_IN_PIC;

    return;
}


/***********************************************************************************
* Function:    MVC_FlushDecoder(MVC_CTX_S *pMvcCtx)
* Description: ��iģʽ�½��������nal���ͣ������֡�����������״̬
* Input:       ��
* Output:      ��
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      ��
***********************************************************************************/  
VOID MVC_FlushDecoder(MVC_CTX_S *pMvcCtx)
{
    SINT32 ret;

    //start vdm to dec curr pic, and postprocess of curr pic
    pMvcCtx->CurrSlice.first_mb_in_slice = 0;  //�������� ʱ ������ ���� ������ 
    pMvcCtx->CurrSlice.new_pic_flag = 1;    
    pMvcCtx->StartMode = 1;
    ret = MVC_DecVDM(pMvcCtx);    // ����VDM����

    return; 
}


/***********************************************************************************
* Function:    MVC_IsNewPicNal( SINT32 *pNewPicDetector,  UINT32 *pu8 )
* Description: ����һ��nal�Ƿ������µ�һ֡.
*                �˺����ڶ�������seg��ʱ�����ã������ж϶�����seg���Ƿ������µ�һ֡��
*              ����ڴ�������PTS������������Ҫ��
* Input:       pu8    nal���������׵�ַ
* Return:      DEC_NORMAL   ��nal������pic
*              DEC_ERR      ��nal��������pic��������֮ǰ��nalͬ��һ��pic
* Others:      ��
***********************************************************************************/  
SINT32 MVC_IsNewPicNal( SINT32 *pNewPicDetector, UINT8 *pu8 )
{
    SINT32 ret = MVC_ERR;
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
                mvc_ue_vld(tmpbits, NumBits, first_mb_in_slice, zeros);
                if (0 == first_mb_in_slice)
                {
                    ret = MVC_OK;
                }
            }
        }
        *pNewPicDetector = 0;
        break;

    case 7:   /* sps */
    case 8:   /* pps */
        if ( 0 == *pNewPicDetector )
        {
            ret = MVC_OK;
        }
        *pNewPicDetector = 1;
        break;
    }

    return ret;
}


/***********************************************************************************
* Function:    MVC_DecOneNal(MVC_CTX_S *pMvcCtx)
* Description: ����NALU, ������ݴ���
* Input:       ����������ʱ����������
* Return:      DEC_NORMAL   ���Խ����Nalu����
*              DEC_ERR      �����Խ���
* Others:      NalMode: 0->BsFromOuter, 1->BsFromInner
***********************************************************************************/  
SINT32 MVC_DecOneNal(MVC_CTX_S *pMvcCtx, UINT32 NalMode)
{
    UINT32 nal_header, code0, code1, eopic0, eopic1;
    SINT32 ret;
    UINT32 i;

    for (i = 0; i <pMvcCtx->pCurrNal->nal_segment; i++)
    {
        pMvcCtx->MaxBytesReceived += pMvcCtx->pCurrNal->stream[i].bitstream_length;
    }
    pMvcCtx->pCurrNal->nal_used_segment = 0; // decode from first segment    
    nal_header = BsGet(&(pMvcCtx->BS), 32);     // ȡ��һ��32bit, 0x00 0x00 0x01 0xxx
    pMvcCtx->pCurrNal->nal_unit_type = nal_header & 0x1f;
    pMvcCtx->pCurrNal->nal_ref_idc   = (nal_header >> 5) & 3;
    pMvcCtx->pCurrNal->forbidden_bit = (nal_header >> 7) & 1;
    pMvcCtx->pCurrNal->nal_bitoffset += 32;   // used bits when decode nal, ʹ����0x000001 + 8bit, ��32bit
    pMvcCtx->SeErrFlg = 0;                    // clear g_SeErrFlg   

    /* MVC */
    pMvcCtx->pCurrNal->svc_extension_flag = -1;
    if(MVC_NALU_TYPE_PREFIX==pMvcCtx->pCurrNal->nal_unit_type || MVC_NALU_TYPE_SLICEEXT==pMvcCtx->pCurrNal->nal_unit_type)
    {
        nal_header = BsGet(&(pMvcCtx->BS), 24);
        pMvcCtx->pCurrNal->svc_extension_flag = (nal_header >> 23)  & 1;
        if(0==pMvcCtx->pCurrNal->svc_extension_flag)
        {
            pMvcCtx->CurrNalMvcInfo.is_valid = 1;
            //pMvcCtx->CurrNalMvcInfo.mvcinfo_flag = 1;
            pMvcCtx->CurrNalMvcInfo.non_idr_flag = (nal_header >> 22) & 1;
            pMvcCtx->CurrNalMvcInfo.priority_id = (nal_header >> 16) & 0x3f;
            pMvcCtx->CurrNalMvcInfo.view_id = (nal_header >> 6) & 0x3ff;
            pMvcCtx->CurrNalMvcInfo.temporal_id = (nal_header >> 3) & 7;
            pMvcCtx->CurrNalMvcInfo.anchor_pic_flag = (nal_header >> 2) & 1;
            pMvcCtx->CurrNalMvcInfo.inter_view_flag = (nal_header >> 1) & 1;
            pMvcCtx->CurrNalMvcInfo.reserved_one_bit = nal_header & 1;

            if(MVC_NALU_TYPE_SLICEEXT==pMvcCtx->pCurrNal->nal_unit_type)
            {
                pMvcCtx->pCurrNal->nal_unit_type = pMvcCtx->CurrNalMvcInfo.non_idr_flag ? MVC_NALU_TYPE_SLICE : MVC_NALU_TYPE_IDR;
            }
        }
        pMvcCtx->pCurrNal->nal_bitoffset += 24;  //used bits when decode nal mvc info
    }
    /* End */

    dprint(PRN_DBG, "pMvcCtx->TotalNal = %d, type:%d\n", pMvcCtx->TotalNal, pMvcCtx->pCurrNal->nal_unit_type);
    if (371 == pMvcCtx->TotalNal) // && 0 == pMvcCtx->SliceNumInPic)
    {
        dprint(PRN_DBG, "stop i want\n");
    }

    pMvcCtx->TotalNal++;

    switch (pMvcCtx->pCurrNal->nal_unit_type)
    {
    case MVC_NALU_TYPE_SLICE:
    case MVC_NALU_TYPE_IDR:
        dprint(PRN_DBG, "***** NAL: IDR/Slice, nal_unit_type=%d, TotalSlice=%d\n", pMvcCtx->pCurrNal->nal_unit_type, pMvcCtx->TotalSlice);

        pMvcCtx->CurrSlice.nal_unit_type = pMvcCtx->pCurrNal->nal_unit_type;
        pMvcCtx->CurrSlice.nal_ref_idc = pMvcCtx->pCurrNal->nal_ref_idc;
        pMvcCtx->CurrSlice.svc_extension_flag = pMvcCtx->pCurrNal->svc_extension_flag;
        pMvcCtx->CurrSlice.mvcinfo_flag = 0;
        pMvcCtx->CurrSlice.view_id = -1;
        if(1==pMvcCtx->CurrNalMvcInfo.is_valid)
        {
            pMvcCtx->CurrSlice.mvcinfo_flag = 1;
            pMvcCtx->CurrSlice.view_id = pMvcCtx->CurrNalMvcInfo.view_id;
            pMvcCtx->CurrSlice.anchor_pic_flag = pMvcCtx->CurrNalMvcInfo.anchor_pic_flag;
            pMvcCtx->CurrSlice.inter_view_flag = pMvcCtx->CurrNalMvcInfo.inter_view_flag;
            pMvcCtx->CurrNalMvcInfo.is_valid = 0;
        }
        else if(-1 == pMvcCtx->CurrSlice.svc_extension_flag && pMvcCtx->TotalSUBSPS > 0)  //���û��prefix nal�ṩmvc��Ϣ�����
        {
            if(pMvcCtx->CurrSUBSPS.num_views_minus1<=0 || 1 != pMvcCtx->CurrSUBSPS.is_valid)
            {
                pMvcCtx->CurrSlice.view_id = MVC_GetBaseViewId(pMvcCtx);
            }
            else
            {
                pMvcCtx->CurrSlice.view_id = pMvcCtx->CurrSUBSPS.view_id[0];
            }

            if(-1 != pMvcCtx->CurrSlice.view_id)
            {
                pMvcCtx->CurrSlice.mvcinfo_flag =1;
                pMvcCtx->CurrSlice.inter_view_flag =1;
                pMvcCtx->CurrSlice.anchor_pic_flag = (MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type);
            }
        }

        dprint(PRN_SE,"*******TotalPicNum=%d********\n",pMvcCtx->TotalPicNum);
        ret = MVC_SliceCheck(pMvcCtx);
        if (MVC_OK != ret)
        {
            MVC_ClearCurrSlice(pMvcCtx);
            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
            return MVC_ERR;
        }

        ret = MVC_DecSlice(pMvcCtx);
        if(MVC_NOTDEC == ret)
        {
            return ret;
        }
        else if (MVC_OK != ret)
        {
            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
            return MVC_ERR;
        }
        break;

    case MVC_NALU_TYPE_PPS:
        dprint(PRN_DBG, "***** NAL: PPS, nal_unit_type=%d, TotalPPS=%d\n", pMvcCtx->pCurrNal->nal_unit_type, pMvcCtx->TotalPPS);
        ret = MVC_DecPPS(pMvcCtx);
        MVC_ClearCurrNal(pMvcCtx);
        if (MVC_OK != ret)
        {
            dprint(PRN_ERROR, "PPS decode error.\n");  
            REPORT_STRMERR(pMvcCtx->ChanID, MVC_PPS_ERR);
            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
            return MVC_ERR;
        }
        pMvcCtx->TotalPPS++;
        break;
    case MVC_NALU_TYPE_SPS:
        dprint(PRN_DBG, "***** NAL: SPS, nal_unit_type=%d, TotalSPS=%d\n", pMvcCtx->pCurrNal->nal_unit_type, pMvcCtx->TotalSPS);
        ret = MVC_DecSPS(pMvcCtx);
        MVC_ClearCurrNal(pMvcCtx);
        if (MVC_OK != ret)
        {
            dprint(PRN_ERROR, "SPS decode error.\n");
            REPORT_STRMERR(pMvcCtx->ChanID, MVC_SPS_ERR);
			
	        REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
            return MVC_ERR;
        }
        pMvcCtx->TotalSPS++;
        break;
    case MVC_NALU_TYPE_SEI:
        dprint(PRN_DBG, "***** NAL: SEI, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);
        ret = MVC_DecSEI(pMvcCtx);
        MVC_ClearCurrNal(pMvcCtx);
        if (MVC_OK != ret)
        {
            dprint(PRN_ERROR, "SEI decode error.\n");  
            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);  
        }
        break;            
    case MVC_NALU_TYPE_AUD:
        dprint(PRN_DBG, "***** NAL: AUD, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);
        MVC_ClearCurrNal(pMvcCtx);
        break;
    case MVC_NALU_TYPE_EOSEQ:
        dprint(PRN_DBG, "***** NAL: EOSEQ, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);         
        MVC_ClearCurrNal(pMvcCtx);
        break;
    case MVC_NALU_TYPE_FILL:
        dprint(PRN_DBG, "***** NAL: FILL, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);         
        MVC_ClearCurrNal(pMvcCtx);
        break;
    case MVC_NALU_TYPE_EOSTREAM:
        dprint(PRN_DBG, "***** NAL: EOSTREAM, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);       
        code0 = (UINT32)BsGet(&(pMvcCtx->BS), 32);
	    code1 = (UINT32)BsGet(&(pMvcCtx->BS), 32);
        if (HISI_STREAM_END_CODE1 == code0 && HISI_STREAM_END_CODE2 == code1)
        {
            pMvcCtx->IsStreamEndFlag = 1;
            REPORT_MEET_NEWFRM(pMvcCtx->ChanID);
            pMvcCtx->StartMode = 0;
            ret = MVC_DecVDM(pMvcCtx);  // ����VDM����
            if (MVC_OK != ret)
            {
	         REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
                return MVC_ERR;
            }
        }
        MVC_ClearCurrNal(pMvcCtx);
        break;
    case NALU_TYPE_SPSEXT:
        dprint(PRN_DBG, "***** NAL: SPSEXT, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);
        MVC_ClearCurrNal(pMvcCtx);
        break;
    /* MVC */
    case MVC_NALU_TYPE_PREFIX:
        dprint(PRN_DBG, "***** NAL: PREFIX, nal_unit_type=%d\n",pMvcCtx->pCurrNal->nal_unit_type);
        MVC_ClearCurrNal(pMvcCtx);
        break;
    case MVC_NALU_TYPE_SUBSPS:
        dprint(PRN_DBG, "***** NAL: SUBSPS, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);
        ret=MVC_DecSubSPS(pMvcCtx);
        MVC_ClearCurrNal(pMvcCtx);
        if(MVC_OK!=ret)
        {
            dprint(PRN_ERROR, "SUBSPS decode error.\n");
            REPORT_STRMERR(pMvcCtx->ChanID, MVC_SUBMVC_SPS_ERR);
            return MVC_ERR;
        }
        pMvcCtx->TotalSUBSPS++;
        break;
    /* End */ 
    case MVC_NALU_TYPE_AUX:
        dprint(PRN_DBG, "***** NAL: AUX, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);
        MVC_ClearCurrNal(pMvcCtx);
        break;
    case MVC_NALU_TYPE_EOPIC:
        dprint(PRN_DBG, "***** NAL: EOPIC, nal_unit_type=%d\n", pMvcCtx->pCurrNal->nal_unit_type);

        if (0 == NalMode)
        {
            MVC_ClearCurrNal(pMvcCtx);
        }

        code0 = BsGet(&(pMvcCtx->BS), 32);
        code1 = BsGet(&(pMvcCtx->BS), 32);
        code0 = REVERSE_ENDIAN32(code0);
        code1 = REVERSE_ENDIAN32(code1);
        eopic0 = *(UINT32 *)(&MVC_g_NalTypeEOPIC[4]);
        eopic1 = *(UINT32 *)(&MVC_g_NalTypeEOPIC[8]);

        if ( (code0==eopic0) && (code1==eopic1) )
        {
            REPORT_MEET_NEWFRM(pMvcCtx->ChanID);
            pMvcCtx->StartMode = 0;
            ret = MVC_DecVDM(pMvcCtx);  // ����VDM����
            if (MVC_OK != ret)
            {
	            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
                return MVC_ERR;
            }
        }

        break;
    default:
        dprint(PRN_DBG, "***** NAL: UNSUPPORT, nal_unit_type=%d,nalu header:%x\n", pMvcCtx->pCurrNal->nal_unit_type,nal_header);
        if (nal_header != 0x00000100)
        {
            dprint(PRN_ERROR, "nal_header != 0x00000100 not support.\n");
            //REPORT_UNSUPPORT(pMvcCtx->ChanID);
        }
        MVC_ClearCurrNal(pMvcCtx);
        REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
        break;
    }

    return MVC_OK;
}


/* ��һ��seg�ڲ�����0���صĸ���
   ����ֵ: 0���ظ������������ֵ����len*8���������seg��ȫ��0�ֽ����.
*/
#define MVC_TZ_BUF_SIZE  64
UINT8  MvcTmpBuf[MVC_TZ_BUF_SIZE + 4];
SINT32  MVC_FindZeroBitsInSeg(UINT8 *ptr, SINT32 len)
{
    UINT8 *pLastByte, Byte, *ThisRoundPtr;
    SINT32 zero_byte = 0, zero_bit = 0;
    SINT32 nRound, i, r;
    SINT32 TotalLeftSize, ThisRoundSize;

#if 1    
    nRound = (len / MVC_TZ_BUF_SIZE) + ((len % MVC_TZ_BUF_SIZE) != 0);
    for (r = nRound; r > 0; r--)
    {
        TotalLeftSize = (len - (nRound-r)*MVC_TZ_BUF_SIZE);

        ThisRoundSize = (TotalLeftSize >= MVC_TZ_BUF_SIZE)? MVC_TZ_BUF_SIZE : TotalLeftSize;
        ThisRoundPtr  = ptr + TotalLeftSize - ThisRoundSize;
        
        memcpy(MvcTmpBuf, ThisRoundPtr, ThisRoundSize);
        pLastByte = MvcTmpBuf + ThisRoundSize - 1;
        while(0x00 == *pLastByte)
        {
            zero_byte++;
            if(pLastByte == MvcTmpBuf)
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
*      ����ԭ�� :  SINT32 MVC_FindTrailZeros(MVC_CTX_S *pMvcCtx)
*      �������� :  ����β��ĺ���
*      ����˵�� :  ��
*      ����ֵ   �� DEC_NORMAL; DEC_ERR;
************************************************************************
*/
SINT32 MVC_FindTrailZeros(MVC_CTX_S *pMvcCtx)
{
    SINT32 zero_bit;

    zero_bit = -1;
    if (pMvcCtx->pCurrNal->nal_segment > 1)
    {
        zero_bit = MVC_FindZeroBitsInSeg(pMvcCtx->pCurrNal->stream[1].streamBuffer,
            pMvcCtx->pCurrNal->stream[1].bitstream_length);

        if (zero_bit >= pMvcCtx->pCurrNal->stream[1].bitstream_length * 8)
        {
            /* ��nal������seg��ɣ����ҵڶ���segȫΪ0���ͷŵ��ڶ���seg */
            SM_ReleaseStreamSeg(pMvcCtx->ChanID, pMvcCtx->pCurrNal->stream[1].StreamID);
            pMvcCtx->pCurrNal->stream[1].streamBuffer = NULL;
            pMvcCtx->pCurrNal->nal_segment = 1;
            zero_bit = -1;
        }
    }

    /* ���nalֻ��һ��seg����������2��seg���ڶ���segȫ0��������һ��seg����β0 */
    if(zero_bit == -1)
    {
        zero_bit = MVC_FindZeroBitsInSeg(pMvcCtx->pCurrNal->stream[0].streamBuffer,
            pMvcCtx->pCurrNal->stream[0].bitstream_length);
    }

	/* padding = 1000000..., ������Ҫȥ���ı����� = zero_bit + 1 */
    pMvcCtx->pCurrNal->nal_trail_zero_bit_num = zero_bit + 1; 
	
    if (pMvcCtx->pCurrNal->nal_segment > 1)
    {
		if (pMvcCtx->pCurrNal->nal_trail_zero_bit_num >= pMvcCtx->pCurrNal->stream[1].bitstream_length * 8)
		{
            /* ������seg, ���ڶ���segֻ������β1+������β0�����Եڶ���segӦ�ñ����� */
            SM_ReleaseStreamSeg(pMvcCtx->ChanID, pMvcCtx->pCurrNal->stream[1].StreamID);
            pMvcCtx->pCurrNal->stream[1].streamBuffer = NULL;
            pMvcCtx->pCurrNal->nal_segment = 1;
			pMvcCtx->pCurrNal->nal_trail_zero_bit_num = 0;
		}
    }
    
    return MVC_OK;    
}

/*!
************************************************************************
*      ����ԭ�� :  VOID MVC_CombinePacket(VOID)
*      �������� :  ƴ�����ܺ���
*      ����˵�� :  ��
*      ����ֵ   �� VOID
************************************************************************
*/
VOID MVC_CombinePacket(MVC_CTX_S *pMvcCtx)
{
    SINT32 StreamId;

    // ��ʾ�϶��ְ��ˣ�Ҳ����˵�϶�Buf������
    if (pMvcCtx->pCurrNal->stream[0].bitstream_length < MVC_GET_ONE_NALU_SIZE) // ��Ҫcopy������buf��ͷ��
    {                       
        dprint(PRN_STREAM, "\n\nEND of the bit buffer, copy the first packet!\n\n");

        // copy����ǰ��
        dprint(PRN_STREAM, "0:phy:0x%x, 1:phy:0x%x; len0:%d len1:%d\n",pMvcCtx->pCurrNal->stream[0].stream_phy_addr,
            pMvcCtx->pCurrNal->stream[1].stream_phy_addr, pMvcCtx->pCurrNal->stream[0].bitstream_length, 
            pMvcCtx->pCurrNal->stream[1].bitstream_length);
        memcpy(pMvcCtx->pCurrNal->stream[1].streamBuffer - pMvcCtx->pCurrNal->stream[0].bitstream_length, \
            pMvcCtx->pCurrNal->stream[0].streamBuffer, pMvcCtx->pCurrNal->stream[0].bitstream_length);
        // ԭ����λ���ͷŵ�
        StreamId =  pMvcCtx->pCurrNal->stream[0].StreamID;
        pMvcCtx->pCurrNal->stream[0].StreamID = pMvcCtx->pCurrNal->stream[1].StreamID;
        pMvcCtx->pCurrNal->stream[0].bitsoffset = 0;
        pMvcCtx->pCurrNal->stream[0].streamBuffer = pMvcCtx->pCurrNal->stream[1].streamBuffer - pMvcCtx->pCurrNal->stream[0].bitstream_length;
        pMvcCtx->pCurrNal->stream[0].stream_phy_addr = pMvcCtx->pCurrNal->stream[1].stream_phy_addr - pMvcCtx->pCurrNal->stream[0].bitstream_length; 
        pMvcCtx->pCurrNal->stream[0].bitstream_length = pMvcCtx->pCurrNal->stream[0].bitstream_length + pMvcCtx->pCurrNal->stream[1].bitstream_length;    //include nal header              
        pMvcCtx->pCurrNal->nal_segment = 1;    
        dprint(PRN_STREAM, "0:phy:0x%x, len0:%d\n",pMvcCtx->pCurrNal->stream[0].stream_phy_addr,pMvcCtx->pCurrNal->stream[0].bitstream_length);

        SM_ReleaseStreamSeg(pMvcCtx->ChanID, StreamId);
        pMvcCtx->pCurrNal->stream[1].streamBuffer = NULL; 
        pMvcCtx->pCurrNal->stream[1].bitsoffset = 0;
        pMvcCtx->pCurrNal->stream[1].bitstream_length = 0;
    }
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 ReceivePacket(MVC_STREAM_PACKET *pPacket)
*      �������� :  ��ȡһƬ���������жϸ�Ƭ�����Ƿ���������nal
*      ����˵�� :  ��
*      ����ֵ   �� 1:һ��������Nalu;0����һ��������Nalu;DEC_ERR:�������
************************************************************************
*/
//����������������,�������ִ�������Ҫ����,������ʱ�ȷ�������
SINT32 MVC_ReceivePacket(MVC_CTX_S *pMvcCtx, MVC_STREAM_PACKET *pPacket)
{
    SINT32 ret;

    dprint(PRN_STREAM, "\n receive streambuff=%p; phy_addr=0x%x; bitstream_length=%d\n", pPacket->p_stream, pPacket->strm_phy_addr, pPacket->stream_len);


     pMvcCtx->u32SCDWrapAround = 0;
    /* ���pCurrNal�Ƿ�����: ���pCurrNal���Ѿ����������һ�����ҵ�ǰ������LastSeg������pCurrNal�Ѿ���������nal����˵��pCurrNal״̬�Ѿ�����
       ��Ҫ����������Ա�֤�����������յ�ǰseg. */
    if((1 != pMvcCtx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) || \
    ((1 == pMvcCtx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) && \
    (0 == pMvcCtx->pstExtraData->s32WaitFsFlag)))   
    {
    if ( (NULL != pMvcCtx->pCurrNal) &&
         ((1 != pPacket->last_pack_in_nal) || (2 <= pMvcCtx->pCurrNal->nal_segment) || (1 == pMvcCtx->pCurrNal->nal_integ)) )
    {
        dprint(PRN_ERROR, "nal_releaMVC_SE_ERR\n");
        MVC_ClearCurrNal(pMvcCtx);
        }
    }

    // ��һ���յ���
    if (NULL == pMvcCtx->pCurrNal)
    {   
        // �յ�����İ�
        if ((0 < pPacket->stream_len) && (NULL != pPacket->p_stream) && (0 != pPacket->strm_phy_addr))
        { 
            // find a good Nalu Slot to save current Nalu
            ret = MVC_FindNaluArraySlot(pMvcCtx);
            if (MVC_ERR == ret)
            {
                dprint(PRN_FATAL, "cann't find slot for current nal\n");

                //����Ҳ�����Nalu Array Slot, �������ǼӴ�����
                MVC_ClearAllNal(pMvcCtx);
                MVC_ClearCurrPic(pMvcCtx);
                MVC_ClearDPB(pMvcCtx,-1);
				
                REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
                return MVC_ERR;
            }
            pMvcCtx->pCurrNal = &(pMvcCtx->NalArray[ret]);

            pMvcCtx->pCurrNal->stream[0].streamBuffer = pPacket->p_stream;
            pMvcCtx->pCurrNal->stream[0].bitstream_length = pPacket->stream_len;    // include nal header   
            pMvcCtx->pCurrNal->stream[0].stream_phy_addr = pPacket->strm_phy_addr;
            pMvcCtx->pCurrNal->stream[0].bitsoffset = 0;
            pMvcCtx->pCurrNal->stream[0].StreamID =  pPacket->StreamID;
            pMvcCtx->pCurrNal->PTS = pPacket->pts;
            pMvcCtx->pCurrNal->nal_segment = 1;
            pMvcCtx->pCurrNal->nal_used_segment = 0;
            pMvcCtx->pCurrNal->nal_integ = pPacket->last_pack_in_nal;
        }
        else
        {
            dprint(PRN_ERROR, "receive a zero packet\n");
            SM_ReleaseStreamSeg(pMvcCtx->ChanID, pPacket->StreamID);
            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
            return MVC_ERR;
        }
    }
    else
    {
        // �յ��ڶ�������
        pMvcCtx->pCurrNal->nal_integ = pPacket->last_pack_in_nal;
        if((1 != pMvcCtx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) || \
        ((1 == pMvcCtx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) && \
        (0 == pMvcCtx->pstExtraData->s32WaitFsFlag)))
        {
        if ((0 < pPacket->stream_len) && (NULL != pPacket->p_stream) && (0 != pPacket->strm_phy_addr) && (pMvcCtx->pCurrNal->nal_segment <= 1))
        {
            pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_segment].streamBuffer = pPacket->p_stream;
            pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_segment].bitstream_length = pPacket->stream_len;    // include nal header      
            pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_segment].stream_phy_addr = pPacket->strm_phy_addr;
            pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_segment].bitsoffset = 0;    
            pMvcCtx->pCurrNal->stream[pMvcCtx->pCurrNal->nal_segment].StreamID =  pPacket->StreamID;
            pMvcCtx->pCurrNal->nal_segment++;

            // ƴ�����ܺ���
            MVC_CombinePacket(pMvcCtx);
            pMvcCtx->u32SCDWrapAround = 1;
        }
        else
        {
            // �ڶ�������Ϊ0��������ַΪ��,��ô��ֻ���һ������
            pMvcCtx->pCurrNal->nal_integ = 1;
            // �ڶ���ֻ��Ҫrelease
            SM_ReleaseStreamSeg(pMvcCtx->ChanID, pPacket->StreamID);
            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
            }
        }
    }

    ret = pMvcCtx->pCurrNal->nal_integ ? MVC_OK : MVC_ERR;

    return ret;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_FindNaluArraySlot(MVC_CTX_S *pMvcCtx)
*      �������� :  find a good Nalu Slot to save current Nalu
*      ����˵�� :  ��
*      ����ֵ   �� ���ؿյ�Nalu�����п��õ�λ��
************************************************************************
*/
SINT32 MVC_FindNaluArraySlot(MVC_CTX_S *pMvcCtx)
{
    UINT32 Pos;
    UINT32 FindPosFlag;

    FindPosFlag = 0;

    for (Pos=0; Pos<MAX_SLICE_SLOT_NUM+1; Pos++)
    {
        if (0 == pMvcCtx->NalArray[Pos].is_valid)
        {   
            FindPosFlag = 1;
            break;
        }
    }

    if ( (1 == FindPosFlag) && (Pos < (MAX_SLICE_SLOT_NUM+1)) )
    {
        pMvcCtx->NalArray[Pos].is_valid = 1;
        return ((SINT32)Pos);
    }
    else
    {
        return MVC_ERR;
    }
}


SINT32 MVC_InquireSliceProperty(MVC_CTX_S *pMvcCtx, SINT32 *MVC_IsNewPic, SINT32 *IsSizeChanged)
{
    SINT32 ret;
    MVC_SPS_S *pSPS;
    MVC_PPS_S *pPPS;
    SINT32 oldw, oldh, neww, newh;

    dprint(PRN_DBG, "\nSlice nal or IDR nal = %d\n", pMvcCtx->TotalNal - 1);

    *MVC_IsNewPic = *IsSizeChanged = 0;

    pMvcCtx->CurrSlice.nal_unit_type = pMvcCtx->pCurrNal->nal_unit_type;
    pMvcCtx->CurrSlice.nal_ref_idc = pMvcCtx->pCurrNal->nal_ref_idc;
    pMvcCtx->CurrSlice.svc_extension_flag = pMvcCtx->pCurrNal->svc_extension_flag;
    pMvcCtx->CurrSlice.mvcinfo_flag = 0;
    pMvcCtx->CurrSlice.view_id = -1;
    if(1==pMvcCtx->CurrNalMvcInfo.is_valid)
    {
        pMvcCtx->CurrSlice.mvcinfo_flag = 1;
        pMvcCtx->CurrSlice.view_id = pMvcCtx->CurrNalMvcInfo.view_id;
        pMvcCtx->CurrSlice.anchor_pic_flag = pMvcCtx->CurrNalMvcInfo.anchor_pic_flag;
        pMvcCtx->CurrSlice.inter_view_flag = pMvcCtx->CurrNalMvcInfo.inter_view_flag;
    }
    else if(-1 == pMvcCtx->CurrSlice.svc_extension_flag && pMvcCtx->TotalSUBSPS > 0)  //���û��prefix nal�ṩmvc��Ϣ�����
    {
        if(pMvcCtx->CurrSUBSPS.num_views_minus1<=0 && 0 == pMvcCtx->CurrSUBSPS.is_valid)
        {
            pMvcCtx->CurrSlice.view_id = MVC_GetBaseViewId(pMvcCtx);
        }
        else
        {
            pMvcCtx->CurrSlice.view_id = pMvcCtx->CurrSUBSPS.view_id[0];
        }

        if(-1 != pMvcCtx->CurrSlice.view_id)
        {
            pMvcCtx->CurrSlice.mvcinfo_flag =1;
            pMvcCtx->CurrSlice.inter_view_flag =1;
            pMvcCtx->CurrSlice.anchor_pic_flag = (MVC_NALU_TYPE_IDR == pMvcCtx->CurrSlice.nal_unit_type);
        }
    }

    if (MVC_OK != MVC_SliceCheck(pMvcCtx))
    {
        return MVC_ERR;
    }

    ret = MVC_ProcessSliceHeaderFirstPart(pMvcCtx);
    if (MVC_OK != ret)
    {
        dprint(PRN_ERROR, "sliceheader dec err\n");
        return MVC_ERR;
    }

    *MVC_IsNewPic = (1 == pMvcCtx->CurrSlice.new_pic_flag)? 1: 0;
#if 1
    /* �жϸ�slice��ͼ��ߴ��Ƿ��֮ǰ�����˱仯 */
    pPPS = &(pMvcCtx->PPS[pMvcCtx->CurrSlice.pic_parameter_set_id]);
    //pSPS = &(pMvcCtx->SPS[pPPS->seq_parameter_set_id]);

    if(-1==pMvcCtx->CurrSlice.svc_extension_flag)
    {
        pSPS = &pMvcCtx->SPS[pPPS->seq_parameter_set_id];
    }
    //mvc non base view��֧,��mvc����������
    else if(0==pMvcCtx->CurrSlice.svc_extension_flag)  // mvc non base view
    {
        if(1==pMvcCtx->Init_Subsps_Flag)
        {
            pSPS = &pMvcCtx->SUBSPS[pPPS->seq_parameter_set_id].sps;
        }
        else
        {
            if(pMvcCtx->CurrSUBSPSID!=pPPS->seq_parameter_set_id)
            {
                dprint(PRN_ERROR,"In one access unit,all non-base view should have the same subsps!\n");
                return MVC_ERR;  //mvc, all non-base view should have the same subsps in a sequence 
            }
            pSPS = &pMvcCtx->CurrSUBSPS.sps;
        }
    }
	else
	{
	    pSPS = NULL;
        dprint(PRN_ERROR, "MVC_InquireSliceProperty mvc flag %d is wrong\n", pMvcCtx->CurrSlice.svc_extension_flag);        
        return MVC_ERR;
	}

    oldw = pMvcCtx->CurWidthInMb;
    oldh = pMvcCtx->CurHeightInMb;
    neww = pSPS->pic_width_in_mbs_minus1 + 1;
    newh = (pSPS->pic_height_in_map_units_minus1 + 1)*(2 - pSPS->frame_mbs_only_flag);
    if ((oldw != neww) || (oldh != newh))
    {
        *IsSizeChanged = 1;
    }
	
    /*  z56361, 20111202,���ͼ��ߴ�û�仯��DPB����ˣ�Ҳ��Ҫ���»���֡��أ�������ͼ��ߴ�仯������ͬ������Ҳ�鵽�ߴ�仯 */
    if (pSPS->dpb_size_plus1 > (pMvcCtx->DPB.size+1))
    {
        *IsSizeChanged = 1;
    }
	#endif
    return MVC_OK;
}


SINT32 MVC_HaveSliceToDec(MVC_CTX_S *pMvcCtx)
{
    if (pMvcCtx->SliceParaNum > 0)
    {
        return MVC_OK;
    }
    else
    {
        return MVC_ERR;
    }
}


/* ���ο��б��Ƿ��д��� */
SINT32 MVC_IsRefListWrong(MVC_CTX_S *pMvcCtx)
{
    SINT32 i, ret = 0;
    SINT32 FindExistFrame;
    FSP_LOGIC_FS_S *pstLogicFs;

    /* ���list 0 */
    if (pMvcCtx->CurrSlice.listXsize[0] > 0)
    {
        FindExistFrame = 0;
        for (i=0; i<pMvcCtx->CurrSlice.listXsize[0]; i++)
        {
            pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[0][i]->frame_store->logic_fs_id);
            if((NULL == pstLogicFs) || ((NULL != pstLogicFs) && (NULL == pstLogicFs->pstDecodeFs)))
            {
                ret = 1;
                break;
            }

            if (pMvcCtx->pListX[0][i]->frame_store->non_existing ==0)
            {
                FindExistFrame = 1;
            }
        }
        //ret = (1 == FindExistFrame)? ret : 1;
    }

    /* ��list 0������󣬶���list1�ǿգ��������list 1 */
    if ((0 == ret) && (pMvcCtx->CurrSlice.listXsize[0] > 0))
    {
        FindExistFrame = 0;
        for (i=0; i<pMvcCtx->CurrSlice.listXsize[1]; i++)
        {
            pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, pMvcCtx->pListX[1][i]->frame_store->logic_fs_id);
            if((NULL == pstLogicFs) || ((NULL != pstLogicFs) && (NULL == pstLogicFs->pstDecodeFs)))
            {
                ret = 1;
                break;
            }
                    
            if (pMvcCtx->pListX[1][i]->frame_store->non_existing == 0)
            {
                FindExistFrame = 1;
            }
        }
        //ret = (1 == FindExistFrame)? ret : 1;
    }

    ret = ret? MVC_ERR: MVC_OK;

    return ret;
}


/***********************************************************************************
* Function:    MVC_DEC_Init(MVC_CTX_S *pMvcCtx)
* Description: ȫ�ֳ�ʼ��mvc����ģ��,���·����ַ
* Input:       
* Output:      
* Return:      ��ʼ���ɹ�����ʧ��
*              
* Others:      ��
***********************************************************************************/
SINT32 MVC_DEC_Init(MVC_CTX_S *pMvcCtx, SYNTAX_EXTRA_DATA_S *pstExtraData)
{
    UINT32 i;
    SINT32 ImgQueRstMagic;

    // ��������� 
    ImgQueRstMagic = pMvcCtx->ImageQue.ResetMagicWord;
    memset( pMvcCtx, 0, sizeof(MVC_CTX_S) );
    pMvcCtx->ImageQue.ResetMagicWord = ImgQueRstMagic;
    ResetVoQueue(&pMvcCtx->ImageQue);
        

    pMvcCtx->pstExtraData = pstExtraData;
    pMvcCtx->pMvcDecParam = NULL;
    pMvcCtx->StartMode = 0;

    // init SPS PPS SLICE_PARA
    if (pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION)
    {
        pMvcCtx->MaxSpsNum = pMvcCtx->pstExtraData->stChanOption.s32MaxSpsNum;
        pMvcCtx->MaxPpsNum = pMvcCtx->pstExtraData->stChanOption.s32MaxPpsNum;
        pMvcCtx->MaxSliceNum = pMvcCtx->pstExtraData->stChanOption.s32MaxSliceNum;
    }
	else
    {
        pMvcCtx->MaxSpsNum = MVC_MAX_SPS_NUM;
        pMvcCtx->MaxPpsNum = MVC_MAX_PPS_NUM;
        pMvcCtx->MaxSliceNum = MAX_SLICE_SLOT_NUM;
    }

    pMvcCtx->SPS = (MVC_SPS_S*)((UINT8 *)MEM_Phy2Vir(pMvcCtx->pstExtraData->s32ContextMemAddr) + pMvcCtx->pstExtraData->s32BasicCtxSize);//CHAN_CTX_MEM_SIZE;
    pMvcCtx->PPS = (MVC_PPS_S*)((UINT8 *)(pMvcCtx->SPS) + sizeof(MVC_SPS_S)*pMvcCtx->MaxSpsNum);
    pMvcCtx->DecSlicePara = (MVC_DEC_SLICE_PARAM_S*)((UINT8 *)(pMvcCtx->PPS) + sizeof(MVC_PPS_S)*pMvcCtx->MaxPpsNum);

    if(NULL== pMvcCtx->SPS || NULL == pMvcCtx->PPS || NULL == pMvcCtx->DecSlicePara)
    {
    	dprint(PRN_DBG, "%s %d NULL== pMvcCtx->SPS || NULL == pMvcCtx->PPS || NULL == pMvcCtx->DecSlicePara!!\n",__FUNCTION__,__LINE__);
    	return VF_ERR_SYS;
    }

    if (0 == pstExtraData->s32KeepSPSPPS)
    {
        memset(pMvcCtx->SPS, 0, pMvcCtx->MaxSpsNum*sizeof(MVC_SPS_S));
        memset(pMvcCtx->PPS, 0, pMvcCtx->MaxPpsNum*sizeof(MVC_PPS_S));
    }
    pMvcCtx->CurrPPS.pic_parameter_set_id = pMvcCtx->MaxPpsNum;
    pMvcCtx->CurrSPS.seq_parameter_set_id = pMvcCtx->MaxSpsNum;
	memset(pMvcCtx->DecSlicePara, 0, sizeof(MVC_DEC_SLICE_PARAM_S)*pMvcCtx->MaxSliceNum);

    pMvcCtx->OldDecMode = (pMvcCtx->advanced_cfg & 0x30000) >> 16;
    pMvcCtx->TotalNal = 0;
    pMvcCtx->TotalSPS = 0;
    pMvcCtx->TotalPPS = 0;
    pMvcCtx->TotalSlice = 0;    
    pMvcCtx->SeqCnt = 0;
    pMvcCtx->TotalPicNum = 0;
    pMvcCtx->TotalFrameNum = 0;
    pMvcCtx->TotalFieldNum = 0;  
    pMvcCtx->TotalFsNum  = MVC_MAX_FRAME_STORE;   // change while g_DPB.size change
    pMvcCtx->TotalPmvNum = MVC_MAX_PMV_STORE;     // change while g_DPB.size change        
    pMvcCtx->TotalUsrDatNum = 0;
    pMvcCtx->pUsrDatArray[0] = NULL;
    pMvcCtx->pUsrDatArray[1] = NULL;
    pMvcCtx->pUsrDatArray[2] = NULL;
    pMvcCtx->pUsrDatArray[3] = NULL;

    pMvcCtx->AllowStartDec = 0;
    pMvcCtx->IDRFound = 0;
    pMvcCtx->FirstFrameDisplayed = MVC_FIRST_DISP_INIT;
    pMvcCtx->PPicFound = 0;
    pMvcCtx->frame_rate = 25;
	
     /* MVC */
    memset(pMvcCtx->SUBSPS, 0, 32*sizeof(MVC_SUBSPS_S));
    memset(&pMvcCtx->CurrSUBSPS,0,sizeof(MVC_SUBSPS_S));
    for(i=0;i<32;i++)
    {
        pMvcCtx->SUBSPS[i].is_valid = 0;
        pMvcCtx->SUBSPS[i].sps.is_valid = 0;
    }
    pMvcCtx->CurrNalMvcInfo.is_valid = 0;
    pMvcCtx->TotalSUBSPS = 0;
    pMvcCtx->CurrSPSID = 32;
    pMvcCtx->CurrSUBSPSID =32;
    pMvcCtx->Init_Subsps_Flag = 0;
    pMvcCtx->pCurrSPS = &pMvcCtx->CurrSPS;
    /* End */

    pMvcCtx->ChanID = VCTRL_GetChanIDByCtx(pMvcCtx);
    if ( -1 == pMvcCtx->ChanID )
    {
        dprint(PRN_FATAL, "-1 == VCTRL_GetChanIDByCtx() Err! \n");
        return VF_ERR_SYS;
    }

    MVC_ClearAll(pMvcCtx, 0);
    
    pMvcCtx->bIsInit = 1;

    return VF_OK;
}


/*!
************************************************************************
*   ����ԭ�� :  MVC_DEC_Destroy(MVC_CTX_S *pMvcCtx)
*   �������� :  ����mvc������
*   ����˵�� :  ��
*   ����ֵ   �� ��
************************************************************************
*/
VOID MVC_DEC_Destroy(MVC_CTX_S *pMvcCtx)
{
    UINT32 i;
    UINT32 usrdat_idx;

    MVC_ClearCurrPic(pMvcCtx);
    MVC_ClearAllNal(pMvcCtx);

    for (i=0; i<pMvcCtx->TotalUsrDatNum; i++)
    {
        if (NULL != pMvcCtx->pUsrDatArray[i])
        {
            FreeUsdByDec(pMvcCtx->ChanID,  pMvcCtx->pUsrDatArray[i]);
            pMvcCtx->pUsrDatArray[i] = NULL;
        }
    }

    for (i=0; i<MVC_MAX_FRAME_STORE; i++)
    {
        for (usrdat_idx=0; usrdat_idx <4; usrdat_idx++)
        {
            if (NULL != pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx])
            {
                FreeUsdByDec(pMvcCtx->ChanID, pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx]);
                pMvcCtx->FrameStore[i].fs_image.p_usrdat[usrdat_idx] = NULL;
            }
        }
    }

    dprint(PRN_CTRL, "MvcDec destroy\n");

    return;
}


/*!
************************************************************************
*      ����ԭ�� :  SINT32 MVC_DEC_DecodePacket(MVC_STREAM_PACKET *pPacket)
*      �������� :  ��ȡһƬ���������жϸ�Ƭ�����Ƿ���������nal
*      ����˵�� :  ��
*      ����ֵ   �� ��
************************************************************************
*/
SINT32 MVC_DEC_DecodePacket(MVC_CTX_S *pMvcCtx, MVC_STREAM_PACKET *pPacket)
{
    SINT32 ret = 0;
    UINT32 nal_header;  //y00226912 test 32����mvc����ȷ
    UINT32 nal_unit_type;
	SINT32 Ret;
    SINT32 RefNum, ReadNum, NewNum;

    if (pMvcCtx->bIsInit != 1)
    {
    	dprint(PRN_FATAL, "MvcCtx not init!\n");
        return MVC_ERR;
    }
    
    RefNum = ReadNum = NewNum = 0;
    CHECK_NULL_PTR_ReturnValue(pPacket, MVC_ERR);

    pMvcCtx->advanced_cfg  = (pMvcCtx->pstExtraData->s32DecMode << 16) & 0x30000;
    pMvcCtx->ref_error_thr = pMvcCtx->pstExtraData->s32RefErrThr;
    pMvcCtx->out_error_thr = pMvcCtx->pstExtraData->s32OutErrThr;
    pMvcCtx->SeErrFlg      = 0;

    // ���յ�һ����������ƴ��һ��������Nalu, ע��MVC_RecivePacket����������ֵ
    if (1 != pMvcCtx->HaveNalNotDecoded)
    {
        if((1 == pMvcCtx->pstExtraData->stChanOption.u32DynamicFrameStoreAllocEn) && \
           (1 == pMvcCtx->pstExtraData->s32WaitFsFlag) && \
           (1 == pMvcCtx->u32SCDWrapAround))
        {
            pMvcCtx->pCurrNal = pMvcCtx->pLastNal;  
            pMvcCtx->u32SCDWrapAround = 0;
        }
        else
        {
            ret = MVC_ReceivePacket(pMvcCtx, pPacket);
            pMvcCtx->pLastNal = pMvcCtx->pCurrNal;
        }
        //ret = MVC_ReceivePacket(pMvcCtx, pPacket);
    }
    else
    {
        ret = (NULL != pMvcCtx->pCurrNal) ? MVC_OK : MVC_ERR;
    }
    pMvcCtx->HaveNalNotDecoded = 0;  //��Ϊ��NALһ���ᱻ�õ�����������new_pic_flagʱ�����д���

    // �յ�һ��������Nalu��ʼ����
    if (MVC_OK == ret)
    {
        //ȡһ��NALͷ�����ж��Ƿ�new pic
        pMvcCtx->pCurrNal->nal_bitoffset = 0;
        BsInit(&(pMvcCtx->BS), (UINT8 *)((pMvcCtx->pCurrNal->stream[0].streamBuffer)), pMvcCtx->pCurrNal->stream[0].bitstream_length);

        nal_header = BsGet(&(pMvcCtx->BS), 32);     // ȡ��һ��32bit, 0x00 0x00 0x01 0xxx
        pMvcCtx->pCurrNal->nal_unit_type = nal_unit_type = (UINT32)(nal_header & 0x1F);
        pMvcCtx->pCurrNal->nal_ref_idc   = (UINT32)((nal_header >> 5) & 3);

             /* MVC */
            pMvcCtx->pCurrNal->svc_extension_flag = -1;
            if(MVC_NALU_TYPE_SLICEEXT==pMvcCtx->pCurrNal->nal_unit_type)
            {
                nal_header = BsGet(&(pMvcCtx->BS), 24);
                pMvcCtx->pCurrNal->svc_extension_flag = (nal_header >> 23)  & 1;
                if(0==pMvcCtx->pCurrNal->svc_extension_flag)
                {
                    pMvcCtx->CurrNalMvcInfo.is_valid = 1;
                    //pMvcCtx->CurrNalMvcInfo.mvcinfo_flag = 1;
                    pMvcCtx->CurrNalMvcInfo.non_idr_flag = (nal_header >> 22) & 1;
                    //pMvcCtx->CurrNalMvcInfo.priority_id = (nal_header >> 16) & 0x3f;
                    pMvcCtx->CurrNalMvcInfo.view_id = (nal_header >> 6) & 0x3ff;
                    //pMvcCtx->CurrNalMvcInfo.temporal_id = (nal_header >> 3) & 7;
                    pMvcCtx->CurrNalMvcInfo.anchor_pic_flag = (nal_header >> 2) & 1;
                    pMvcCtx->CurrNalMvcInfo.inter_view_flag = (nal_header >> 1) & 1;
                    //pMvcCtx->CurrNalMvcInfo.reserved_one_bit = nal_header & 1;
                    pMvcCtx->pCurrNal->nal_unit_type = nal_unit_type = pMvcCtx->CurrNalMvcInfo.non_idr_flag ? MVC_NALU_TYPE_SLICE : MVC_NALU_TYPE_IDR;
                }
            }
            /* End */
            pMvcCtx->SeErrFlg = 0;
        if ( (MVC_NALU_TYPE_SLICE==nal_unit_type) || (MVC_NALU_TYPE_IDR==nal_unit_type) )
        {
            SINT32  MVC_IsNewPic, IsSizeChanged;
            if ( MVC_OK == MVC_InquireSliceProperty(pMvcCtx, &MVC_IsNewPic, &IsSizeChanged) )
            { 
                pMvcCtx->PicCounter += (1 == MVC_IsNewPic);  /* 32λ����ζ�źܶ�֡��30fps�ɲ�4.4�꣬��˲�����32λ������� */
				if (1 == MVC_IsNewPic)
				{
                    if (pMvcCtx->CurrSlice.field_pic_flag == 1)
                    {
                        pMvcCtx->NewPicCounter = pMvcCtx->NewPicCounter + 1;
                    }
                    else
                    {
                        pMvcCtx->NewPicCounter = pMvcCtx->NewPicCounter + 2;
                    }
				}
                if ( (1 == MVC_IsNewPic) && (MVC_OK == MVC_HaveSliceToDec(pMvcCtx)) )  //�������Sliceδ���룬˵����δ����MVC_NALU_TYPE_EOPIC������������һ��MVC_NALU_TYPE_EOPIC
                {
                    pMvcCtx->pCurrNal->nal_bitoffset = 0;
                    BsInit(&(pMvcCtx->BS), (UINT8 *)(MVC_g_NalTypeEOPIC), sizeof(MVC_g_NalTypeEOPIC));
                    MVC_DecOneNal(pMvcCtx, 1);
                    if (pMvcCtx->CurrSlice.field_pic_flag == 1)
                    {
                        pMvcCtx->NewPicCounter = pMvcCtx->NewPicCounter - 1;
                    }
                    else
                    {
                        pMvcCtx->NewPicCounter = pMvcCtx->NewPicCounter - 2;
                    }
                    pMvcCtx->HaveNalNotDecoded = 1;  //��ס�ղ���һ��NALδ����
                    return MVC_NOTDEC;
                }

                /* ���ͼ��ߴ��ڴ�slice�����˱仯��Ӧ�õ�֮ǰ�ѽ����ͼ��ȫ������󼶺��ټ����³ߴ�ͼ��Ľ��룬����
                   �ϳߴ��ͼ����ܻ��н϶�Ķ��������²���Ч�����ѡ�z56361, 2011-11-1 */
                if (1 == IsSizeChanged)
                {
		      		Ret = MVC_FlushDPB(pMvcCtx, -1);
		            if (MVC_OK != Ret)
		            {
		                dprint(PRN_DBG, "line %d: flush dpb return %d\n", __LINE__, ret);
		            }
                      
                    VCTRL_GetChanImgNum(pMvcCtx->ChanID, &RefNum, &ReadNum, &NewNum);
			   
                    if (pMvcCtx->pstExtraData->stChanOption.s32ReRangeEn == 1)
                    {
                     #ifdef PRODUCT_KIRIN
                        if (NewNum > 0)
                        {
                            pMvcCtx->HaveNalNotDecoded = 1;  //��ס�ղ���һ��NALδ����(��ԭ�ߴ��֡����)
                            return HEVC_NOTDEC;
                        }
                     #else
                        if (ReadNum + NewNum > 0)
                        {
                            pMvcCtx->HaveNalNotDecoded = 1;  //��ס�ղ���һ��NALδ����(��ԭ�ߴ��֡����)
                            return HEVC_NOTDEC;
                        }
                     #endif
                    }
                }
            }
            else
            {
                MVC_ClearCurrNal(pMvcCtx);
                REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
                return MVC_ERR;
            }
        }
		
        // ȥ��β�㺯��
        ret = MVC_FindTrailZeros(pMvcCtx);
		
        // ��g_BS�е�ֵ, BS�ĳ�ʼ����Ҫ��word�����λ�ÿ�ʼ
        pMvcCtx->pCurrNal->nal_bitoffset = 0;
        BsInit(&(pMvcCtx->BS), (UINT8 *)((pMvcCtx->pCurrNal->stream[0].streamBuffer)), pMvcCtx->pCurrNal->stream[0].bitstream_length);
        ret = MVC_DecOneNal(pMvcCtx, 0);
        if(MVC_NOTDEC == ret)
        {
            return ret;
        }
        else if (MVC_OK != ret)
        {
            REPORT_DECSYNTAX_ERR(pMvcCtx->ChanID);
            return MVC_ERR;
        }

        if (MVC_OK != ret)
        {
            dprint(PRN_DBG, "MVC_DecOneNal ERR\n");
            return MVC_ERR;
        }
        else
        {
            return MVC_OK;
        }
    }

    return MVC_OK;    
}


/*!
************************************************************************
*   ����ԭ�� :  VOID MVC_DEC_RecycleImage(MVC_CTX_S *pMvcCtx, UINT32 PicID)
*   �������� :  MVC ����VO�Ѿ���ʾ��ͼ���λ
*   ����˵�� :  
*   ����ֵ   �� 
************************************************************************
*/
SINT32 MVC_DEC_RecycleImage(MVC_CTX_S *pMvcCtx, UINT32 PicID)
{
    UINT32 i;
    FSP_LOGIC_FS_S *pstLogicFs;

    dprint(PRN_CTRL, "MVC recycle image\n");

    /* �ҵ�PicID����Ӧ���߼�֡�棬�Ӷ��ҵ�image�ṹ�壬���������ͷ�image��������userdata */
    pstLogicFs = FSP_GetLogicFs(pMvcCtx->ChanID, PicID);
    if (NULL != pstLogicFs)
    {
        /* �������߼�֡���Ƿ����ڵȴ���ʾ�ͷ� */
        if (pstLogicFs->s32DispState == FS_DISP_STATE_NOT_USED)
        {
            return MVC_ERR;
        }

        /* �ͷ�userdata */
        for (i = 0; i < 4; i++)
        {
            if (NULL != pstLogicFs->stDispImg.p_usrdat[i])
            {
                FreeUsdByDec(pMvcCtx->ChanID, pstLogicFs->stDispImg.p_usrdat[i]);
                pstLogicFs->stDispImg.p_usrdat[i] = NULL;
            }
        }

        /* ��ͼ��'PicID'���Ϊ"��ʾ����" */
        FSP_SetDisplay(pMvcCtx->ChanID, PicID, 0);
        return MVC_OK;
    }
    return MVC_ERR;
}


UINT32 MVC_DEC_VDMPostProc( MVC_CTX_S *pMvcCtx, SINT32 ErrRatio, SINT32 CalledByISR )
{
    SINT32 ret;

    if (0 == pMvcCtx->StartMode)
    {
        pMvcCtx->CurrPic.err_level = ErrRatio;
        MVC_ClearAllSlice(pMvcCtx);

        ret = MVC_StorePicInDpb(pMvcCtx);
        if (MVC_OK != ret)
        {
            FSP_ClearLogicFs(pMvcCtx->ChanID, pMvcCtx->CurrPic.frame_store->logic_fs_id, 1);
            dprint(PRN_ERROR, "store pic err, ret = %d\n", ret);

            MVC_ClearCurrPic(pMvcCtx);
            MVC_ClearDPB(pMvcCtx,-1);
            pMvcCtx->CurrPic.frame_store = NULL;
            return MVC_ERR;
        }

        pMvcCtx->AllowStartDec = 1;
    }
    else  // (1 == pMvcCtx->StartMode)
    {
        // ͳ�Ƶ�ǰPic������������ж��Ƿ���Ҫ��g_CurrPic.err_level?
        pMvcCtx->CurrPic.err_level = ErrRatio;
        MVC_ClearAllSlice(pMvcCtx);

        ret = MVC_StorePicInDpb(pMvcCtx);
        if (MVC_OK != ret)
        {
            dprint(PRN_ERROR, "MVC_IMODE nal store pic err, ret = %d\n", ret);
        }
        ret = MVC_FlushDPB(pMvcCtx,-1);
        if (MVC_OK != ret)
        {
            dprint(PRN_ERROR, "MVC_IMODE nal flush dpb err, ret = %d\n", ret);
        }

        pMvcCtx->AllowStartDec = 0;
        MVC_InitOldSlice(pMvcCtx);
    }

    return MVC_OK;
}


SINT32 MVC_DEC_GetImageBuffer( MVC_CTX_S *pMvcCtx )
{
    UINT32 i;
    SINT32 ReadImgNum, NewImgNum;
#ifdef VFMW_CFG_CAP_HD_SIMPLE
    SINT32  LogicFsID = 0;
    FSP_LOGIC_FS_S *pstLFs = NULL;
#endif

    if (pMvcCtx->TotalFsNum < 1)
    {
        return 1;
    }
#ifdef VFMW_CFG_CAP_HD_SIMPLE
    if(1 == pMvcCtx->PrevPic.FrameStoreFlag)
    {
        LogicFsID = pMvcCtx->PrevPic.image_id;
        pstLFs = FSP_GetLogicFs(pMvcCtx->ChanID, LogicFsID);
        if ((NULL != pstLFs)&&(FS_DISP_STATE_ALLOCED == pstLFs->s32DispState))
        {
            return 1;
        }
    }
#endif
    // allocate frame_store
    for (i=0; i<pMvcCtx->TotalFsNum; i++)
    {
        if ((0 == pMvcCtx->FrameStore[i].is_used) && (1 == MVC_IsOutDPB(pMvcCtx, &pMvcCtx->FrameStore[i])))
        {
            /* ֡��ṹ��ȡ���ˣ���Ҫ��ѯFSP�Ƿ������֡����� */
            if (FSP_IsNewFsAvalible(pMvcCtx->ChanID) == 1)
            {
                return 1;
            }
            else if (FSP_IsNewFsAvalible(pMvcCtx->ChanID) == -1)
            {
                FSP_ClearNotInVoQueue(pMvcCtx->ChanID, &(pMvcCtx->ImageQue));
                return 0;
            }
			else
			{
			    return 0;
			}
        }        
    }

    /* ���֡���Ƿ�й© */
    GetQueueImgNum(&pMvcCtx->ImageQue, &ReadImgNum, &NewImgNum);
    dprint(PRN_FATAL, "cann't find FrameStore\n");
    dprint(PRN_FATAL, "========== MVC FrameStore state(is_used, is_in_dpb, MVC_IsOutDPB) ========\n");
    for (i=0; i<pMvcCtx->TotalFsNum; i++)
    {
        dprint(PRN_FATAL, "%02d: %d %d %d\n",i, pMvcCtx->FrameStore[i].is_used, pMvcCtx->FrameStore[i].is_in_dpb,
                          MVC_IsOutDPB(pMvcCtx, &pMvcCtx->FrameStore[i]));
    }   
    dprint(PRN_FATAL, "ReadImgNum = %d, NewImgNum = %d\n", ReadImgNum, NewImgNum);
    if ((ReadImgNum+NewImgNum) < (SINT32)(pMvcCtx->TotalFsNum-pMvcCtx->DPB.used_size-2))
    {
        dprint(PRN_FATAL, "FrameStore leak, MVC_ClearAll\n");
        MVC_ClearAll(pMvcCtx,1);
    }

    return 0;
}


#endif // #ifdef MVC_ENABLE
