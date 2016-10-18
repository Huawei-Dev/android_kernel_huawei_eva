/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7omB5iYtHEGPqMDW7TpB3q4yvDckVri1GoBL4gKI3k0M86wMC5hMD5fQLRyjt
hU2r3+wdNBVAaWSiemyLf8l47mSbvsHIny7YPxN17cSVi5cBtC4TaJu2EsMB5CsZ8x/mxAHh
7y7DbUs6C91ZJZRdEHbBQTNJfyLx0iCj7UCH6At3hVND8qFdRMBNJxkvmM21/A==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
   
#include "fsp.h"
#include "mem_manage.h"
#include "syntax.h"
#include "vfmw_ctrl.h"

FSP_INST_S* s_pFspInst[MAX_CHAN_NUM];

extern VFMW_CHAN_S  *s_pstVfmwChan[MAX_CHAN_NUM];
extern UINT32 g_u32DFSMaxAllocTime;


/***********************************************************************
   OMX ֡��ص��ӿ�
 ***********************************************************************/
static FSP_OMX_INTF_S g_FspOMXIntf;


/***********************************************************************
    OMX bypassģʽʱ������֡��ص��ӿ�
 ***********************************************************************/
SINT32 FSP_SetOMXInterface(FSP_OMX_INTF_S *pIntf)
{
	if (NULL == pIntf->pfnBufferHandler)
	{
		dprint(PRN_FATAL, "%s FrameIntf invalid\n", __func__);
		return FSP_ERR;
	}

	memcpy(&g_FspOMXIntf, pIntf, sizeof(FSP_OMX_INTF_S));

	return FSP_OK;
}
   
/***********************************************************************
    ��ѯ�Ƿ���OMX bypass mode
 ***********************************************************************/
inline SINT32 FSP_IsOmxBypassMode(SINT32 InstID)
{         
    if(1 == s_pFspInst[InstID]->IsOmxPath
    && 1 == s_pstVfmwChan[InstID]->stSynExtraData.stChanOption.u32OmxBypassMode)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/***********************************************************************
    OMX bypassģʽʱ���ⲿ֡��ص�
 ***********************************************************************/
inline SINT32 FSP_ExtBufHandler(SINT32 InstID, VDEC_BC_TYPE_E type, VOID *pParam)
{     
	if (NULL == g_FspOMXIntf.pfnBufferHandler)
	{
		dprint(PRN_FATAL, "%s: BufferHandler func null\n");
		return 0;
	}

	return g_FspOMXIntf.pfnBufferHandler(InstID, type, pParam);
}


/***********************************************************************
    ȫ�ֳ�ʼ��
 ***********************************************************************/
VOID FSP_Init(VOID)
{ 
    memset(s_pFspInst, 0, MAX_CHAN_NUM*sizeof(FSP_INST_S *));
    
    return;
}

/***********************************************************************
    ȫ���˳�
 ***********************************************************************/
VOID  FSP_Exit(VOID)
{
    SINT32 i;
        
    for (i = 0; i < MAX_CHAN_NUM; i++)
    {
        if (NULL != s_pFspInst[i] && FSP_INST_STATE_NULL != s_pFspInst[i]->eFspInstState)
        {
            FSP_DisableInstance(i);
        }
    }
       
    return;
}

/***********************************************************************
	����ͨ��ʱ����֡��ʵ����ȫ������
 ***********************************************************************/
SINT32 FSP_ConfigAttribute(SINT32 InstID, FSP_ATTR_S *pstAttr)  
{
    FSP_INST_S *pstInst;

    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(s_pFspInst[InstID]->eFspInstState == FSP_INST_STATE_INUSE, "fsp instance not in used");

    pstInst = s_pFspInst[InstID];

    /* ���õͻ���ģʽ��������֡�����ʱ��ʵ����Ҫ���ֶ��������ֵ */
    pstInst->IsOmxPath = pstAttr->IsOmxPath;

    return FSP_OK;
}

/***********************************************************************
	ʵ����Ϣ����������Էֽ��߿�ʼ���¶��ᱻ����
 ***********************************************************************/
SINT32 FSP_ClearContext(FSP_INST_S *pFspInst)  // ONE TEST
{
	SINT8             s8TmpInstID;
    SINT8             s8TmpIsOmxPath;
	FSP_INST_STATE_E  eTmpFspInstState;
	FSP_PRE_INF_S     stTmpPreInfo;
	
    FSP_ASSERT_RET(pFspInst != NULL, "pFspInst = NULL");

    s8TmpInstID = pFspInst->s32InstID;
    s8TmpIsOmxPath  = pFspInst->IsOmxPath;
	eTmpFspInstState = pFspInst->eFspInstState;
    memcpy(&stTmpPreInfo, &(pFspInst->stPreInfo), sizeof(FSP_PRE_INF_S));
	
    memset(pFspInst, 0, sizeof(FSP_INST_S));
	
	pFspInst->s32InstID = s8TmpInstID;
    pFspInst->IsOmxPath =  s8TmpIsOmxPath;
	pFspInst->eFspInstState =  eTmpFspInstState;
    memcpy(&(pFspInst->stPreInfo), &stTmpPreInfo, sizeof(FSP_PRE_INF_S));

    return FSP_OK;
}

/***********************************************************************
    ʹ��ʵ��(��״֡��ʵ��ʹ�ܺ����������)
    �ɹ�����ʵ��FSP_OK��ʧ�ܷ���FSP_ERR
 ***********************************************************************/
SINT32  FSP_EnableInstance(SINT32  InstID)
{    
    FSP_INST_S *pstInst;
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");

    pstInst = s_pFspInst[InstID];
    if (FSP_INST_STATE_NULL == pstInst->eFspInstState)
    {
        memset( pstInst, 0, sizeof(FSP_INST_S) );
	    pstInst->eFspInstState = FSP_INST_STATE_INUSE;
        pstInst->eFspPartitionState = FSP_PARTITION_STATE_FIRST;
	    return FSP_OK;
    }
    
    return FSP_ERR;
}

/***********************************************************************
    ����ʵ��
 ***********************************************************************/
VOID  FSP_DisableInstance(SINT32 InstID)
{    
    FSP_INST_S *pstInst;
    FSP_ASSERT(InstID >= 0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");

    pstInst = s_pFspInst[InstID];
    if (FSP_INST_STATE_INUSE == pstInst->eFspInstState)
    {
        pstInst->eFspInstState = FSP_INST_STATE_NULL;
    }
	
    return;
}


/***********************************************************************
    ��λʵ��
    �ɹ�����ʵ��FSP_OK��ʧ�ܷ���FSP_ERR
 ***********************************************************************/
SINT32  FSP_ResetInstance(SINT32  InstID)
{  
    FSP_INST_S *pFspInst = s_pFspInst[InstID];
    
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");

    FSP_ClearContext(pFspInst);

    return FSP_OK;
}

/***********************************************************************
    ���ʵ��
    �ڼ�������£��������������ش���Э�����ܻ�Ҫ��������н�������
    �ģ����а������FSPʵ����������֡�涼���Ϊ���ο�������ʾ���ȴ���һ��
    ����ʹ�á�
    ��FSP_ResetInstance()��ͬ���ǣ������������������Ϣ��Ҳ�����֡��ķ���
    ��Ϣ�������������֡��ռ����Ϣ��
    �ɹ�����ʵ��FSP_OK��ʧ�ܷ���FSP_ERR
 ***********************************************************************/
SINT32  FSP_EmptyInstance(SINT32  InstID)
{
    FSP_INST_S *pstInst;
    SINT32 i;

    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");

    pstInst = s_pFspInst[InstID];

    /* ����߼�֡��ռ�� */
    memset( &pstInst->stLogicFs[0], 0, sizeof(FSP_LOGIC_FS_S)*MAX_FRAME_NUM);

    /* �������֡��ռ�� */
    for (i = 0; i < pstInst->s32DecFsNum; i++)
    {
        pstInst->stDecFs[i].IsDecRef = 0;
        pstInst->stDecFs[i].DispState = FS_DISP_STATE_NOT_USED;
        pstInst->stDecFs[i].u32Tick = 0;
    }
    /* �����ʾռ�� */
    for (i = 0; i < pstInst->s32DispFsNum; i++)
    {
        pstInst->stDispFs[i].IsDecRef = 0;
        pstInst->stDispFs[i].DispState = FS_DISP_STATE_NOT_USED;
        pstInst->stDispFs[i].u32Tick = 0;
    }
    /* ���PMVռ�� */
    for (i = 0; i < pstInst->s32PmvNum; i++)
    {
        pstInst->IsPmvInUse[i] = 0;
    }

    /* ʵ��tick���� */
    pstInst->u32InstTick = 0;

    return FSP_OK;
}


/***********************************************************************
    ���û�б��ŵ�VO QUEUE ���߼�֡
 ***********************************************************************/
SINT32 FSP_ClearNotInVoQueue(SINT32 InstID,IMAGE_VO_QUEUE *pstVoQue)
{
    FSP_INST_S *pstInst;
    IMAGE *p_image;
    SINT32 i;

    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(pstVoQue != NULL, "pstVoQue is NULL");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");
    
    pstInst = s_pFspInst[InstID];

    for(i= 0; i < MAX_FRAME_NUM; i++)
    {
        p_image = &(pstInst->stLogicFs[i].stDispImg);   
        FSP_SetRef(InstID, p_image->image_id, 0);

        if( SYN_OK != IsImageInVoQueue(pstVoQue, p_image))
        {
            FSP_SetDisplay(InstID,p_image->image_id, 0);   
        }
    }

    return FSP_OK;

}


/***********************************************************************
    ����ʵ��
 ***********************************************************************/
SINT32  FSP_ConfigInstance( SINT32 InstID,  FSP_INST_CFG_S *pstFspInstCfg )
{
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(pstFspInstCfg != NULL, "pstFspInstCfg = NULL");
    FSP_ASSERT_RET((NULL != s_pFspInst[InstID] && s_pFspInst[InstID]->eFspInstState == FSP_INST_STATE_INUSE), "This inst is not active");
        
    /* ������Ϣ���� */
    pstFspInstCfg->s32DispLoopEnable = 0;    /* ����Ҫ����֡��  */
    
    /* ��ȻFSP���ܶ�̬�޸����ã�������ʱ���ʵ��������Ҳ�Ǻ���� */
    FSP_ClearContext(pstInst);
    
    /* ����������Ϣ */
    memcpy( &pstInst->stInstCfg, pstFspInstCfg, sizeof(FSP_INST_CFG_S) );

    return FSP_OK;
}


/***********************************************************************
    ֡��ָ�
 ***********************************************************************/
SINT32  FSP_PartitionFsMemory ( SINT32 InstID, UADDR MemPhyAddr, SINT32 MemSize, SINT32 *UsedMemSize )
{
    SINT32  i, Ret;
    SINT32  MinFsNum;
    UADDR   PhyAddr;
    SINT32  Size;
    SINT32  ExpectedRefNum = 0;
    FSP_INST_S *pstInst;
    FSP_INST_CFG_S *pstCfg;
    VDMHAL_MEM_ARRANGE_S MemArrange;
    VDEC_CHAN_CAP_LEVEL_E  eChanCapLevel;
    extern SINT32 CapItem[][7];
    memset(&MemArrange, 0, sizeof(VDMHAL_MEM_ARRANGE_S));
        
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");

	pstInst = s_pFspInst[InstID];

	pstCfg  = &pstInst->stInstCfg;
	PhyAddr = MemPhyAddr;
	Size    = MemSize;

    if(0 == s_pstVfmwChan[InstID]->stSynExtraData.stChanOption.u32DynamicFrameStoreAllocEn)// check only when dfs disable	
    {
        /* �ж�ͼ���С�Ƿ����ͨ����������������������������䣬����ʧ�� */
        eChanCapLevel = VCTRL_GetChanCapLevel(InstID);
        if  (eChanCapLevel >= CAP_LEVEL_BUTT || 
            (((pstCfg->s32DecFsWidth * pstCfg->s32DecFsHeight) > (CapItem[eChanCapLevel][0] * CapItem[eChanCapLevel][1]))&&(eChanCapLevel != CAP_LEVEL_USER_DEFINE_WITH_OPTION)) )
        {
            if (eChanCapLevel >= CAP_LEVEL_BUTT)
            {
                dprint(PRN_FATAL, "eChanCapLevel = %d is not expected!\n", eChanCapLevel);
            }
            else
            {
                dprint(PRN_FATAL, "decode channel CapLevel is %d(w=%d,h=%d), this stream (w=%d,h=%d) can NOT be decoded!\n",
                eChanCapLevel, CapItem[eChanCapLevel][0], CapItem[eChanCapLevel][1],
                pstCfg->s32DecFsWidth, pstCfg->s32DecFsHeight);
            }
            pstInst->s8TotalValidFsNum = 0;
            return FSP_ERR;
        }
    }

    /* �ж��Ƿ���2D scaler�����û����ͳһ��������֡��(dec,disp,tf��������֡���С����һ����). */ 
	if ((pstCfg->s32DispFsHeight == 0 || pstCfg->s32DispFsWidth == 0) || (0 == pstCfg->s32DispLoopEnable) ||
		((pstCfg->s32DecFsHeight == pstCfg->s32DispFsHeight) && (pstCfg->s32DecFsWidth  == pstCfg->s32DispFsWidth)))
	{
        MinFsNum = pstCfg->s32ExpectedDecFsNum + pstCfg->s32DispLoopEnable*4;
        if ((1 != (s_pstVfmwChan[InstID]->stSynExtraData.stChanOption.u32DynamicFrameStoreAllocEn)) || \
           ((1 == (s_pstVfmwChan[InstID]->stSynExtraData.stChanOption.u32DynamicFrameStoreAllocEn)) && \
           ((FSP_PARTITION_STATE_FIRST == s_pstVfmwChan[InstID]->FspInst.eFspPartitionState) || \
           (FSP_PARTITION_STATE_SUCCESS == s_pstVfmwChan[InstID]->FspInst.eFspPartitionState))))
		{
            if (1 == (s_pstVfmwChan[InstID]->stSynExtraData.stChanOption.u32DynamicFrameStoreAllocEn))
            {
                // ʹ�ܶ�̬֡�棬ֻҪ��ʵ��֡�����
                ExpectedRefNum = pstCfg->s32ExpectedDecFsNum;
            }
            else
            {
                // ��ʹ�ܶ�̬֡�棬�ж����ö���
                ExpectedRefNum = 40;
            }
		    Ret = VDMHAL_ArrangeMem(PhyAddr, Size, pstCfg->s32DecFsWidth, pstCfg->s32DecFsHeight, pstCfg->s32ExpectedPmvNum, ExpectedRefNum, InstID, &MemArrange);  /* PmvNum = 0 */
        }
        else if(FSP_PARTITION_STATE_WAIT == s_pstVfmwChan[InstID]->FspInst.eFspPartitionState)
        {
            return FSP_OK;
        }
        else
        {
            pstInst->s8TotalValidFsNum = 0;
            dprint(PRN_ERROR,"DFS, Wrong Partition State. State: 0x%x\n", s_pstVfmwChan[InstID]->FspInst.eFspPartitionState); 
            return FSP_ERR;
        }
        
        if (VDMHAL_NOT_ARRANGE == Ret)
        {
            pstInst->s8TotalValidFsNum = MemArrange.ValidFrameNum;
            s_pstVfmwChan[InstID]->stSynExtraData.s32WaitFsFlag = 1;
            dprint(PRN_FS,"DFS, Chan %d change to FSP_WAIT_ARRANGE state.\n", InstID); 
            return FSP_WAIT_ARRANGE;
        }
        else if(VDMHAL_OK == Ret)
        {
            pstInst->s8TotalValidFsNum = MemArrange.ValidFrameNum;
            s_pstVfmwChan[InstID]->stSynExtraData.s32WaitFsFlag = 0;
            if(1 == (s_pstVfmwChan[InstID]->stSynExtraData.stChanOption.u32DynamicFrameStoreAllocEn))
            {
                s_pstVfmwChan[InstID]->stDynamicFSTimestamp.u32AllTime = VFMW_OSAL_GetTimeInMs() - s_pstVfmwChan[InstID]->stDynamicFSTimestamp.u32StartTime;
                if((s_pstVfmwChan[InstID]->stDynamicFSTimestamp.u32AllTime) > g_u32DFSMaxAllocTime)
                {
                    g_u32DFSMaxAllocTime = s_pstVfmwChan[InstID]->stDynamicFSTimestamp.u32AllTime;
                }
            }
            dprint(PRN_FS, "DFS, Mem Element Fill Successed! Ready to Decode!\n");
        }
        else
        {
            pstInst->s8TotalValidFsNum = 0;
            s_pstVfmwChan[InstID]->stSynExtraData.s32WaitFsFlag = 0;
            dprint(PRN_FATAL, "Fsp arrange failed, return %d\n", Ret);
            return FSP_ERR;
        }
        
		if (MemArrange.ValidFrameNum >= MinFsNum)
		{
			pstInst->s32DispFsNum = 0;
			pstInst->s32TfFsNum = 0;

			/* ��������֡��: ��������·��ʹ������֡�涼������ã�����Ҫ�ֿ����� */
			if (0 == pstCfg->s32DispLoopEnable)
			{
                if (pstInst->IsOmxPath && 1 != s_pstVfmwChan[InstID]->stSynExtraData.stChanOption.u32DynamicFrameStoreAllocEn)
                {
                    //+1Ϊ�˼��ݵ���˫����+3��VPSS��DEI����
                    pstInst->s32DecFsNum = MIN(MemArrange.ValidFrameNum, pstCfg->s32ExpectedDecFsNum+1+3);
                }
                else
                {
                    pstInst->s32DecFsNum = MIN(MemArrange.ValidFrameNum, 40); 
                }
			}
			else
			{
            #ifdef VFMW_CFG_CAP_HD_SIMPLE
                pstInst->s32DecFsNum = MIN(MemArrange.ValidFrameNum, 40);
            #else
                pstInst->s32DecFsNum = pstCfg->s32ExpectedDecFsNum;
            #endif
			}

			/* ��ַ�������ʼ��Ϊ��Чֵ����Ч���߼�֡���ַ��ʼ��Ϊ�������ֵ */
			for (i = 0; i < MAX_FRAME_NUM; i++)
			{
			    if (i < pstInst->s32DecFsNum)
			    {
				    pstInst->stDecFs[i].PhyFsID     = i;
		            pstInst->stDecFs[i].PhyAddr     = MemArrange.FrameAddr[i];
			    #ifdef VFMW_MODULE_LOWDLY_SUPPORT
					pstInst->stDecFs[i].LineNumAddr = MemArrange.LineNumAddr[i];
				#endif
					pstInst->stDecFs[i].Stride      = MemArrange.VahbStride;
					pstInst->stDecFs[i].FsWidth     = pstCfg->s32DecFsWidth;
					pstInst->stDecFs[i].FsHeight    = pstCfg->s32DecFsHeight;
		            pstInst->stDecFs[i].ChromOffset = MemArrange.ChromOffset;
					pstInst->stDecFs[i].IsDecRef    = 0;
					pstInst->stDecFs[i].DispState   = FS_DISP_STATE_NOT_USED;
			    }
				else
				{
				    pstInst->stDecFs[i].PhyFsID     = 0;
		            pstInst->stDecFs[i].PhyAddr     = MemArrange.FrameAddr[0];
					pstInst->stDecFs[i].Stride      = MemArrange.VahbStride;
					pstInst->stDecFs[i].FsWidth     = pstCfg->s32DecFsWidth;
					pstInst->stDecFs[i].FsHeight    = pstCfg->s32DecFsHeight;
		            pstInst->stDecFs[i].ChromOffset = MemArrange.ChromOffset;
					pstInst->stDecFs[i].IsDecRef    = 0;
					pstInst->stDecFs[i].DispState   = FS_DISP_STATE_NOT_USED;

				}
			}
			
            /* TF����֡��(TF����ȷ�������ã���TF�����ڴ���DB���ã�Ӳ��������) */
			pstInst->s32TfFsNum = 1;
		    pstInst->stTfFs[0].PhyFsID     = 0;
            pstInst->stTfFs[0].PhyAddr     = MemArrange.FrameAddr[pstInst->s32DecFsNum];
			pstInst->stTfFs[0].Stride      = MemArrange.VahbStride;
			pstInst->stTfFs[0].FsWidth     = pstCfg->s32DecFsWidth;
			pstInst->stTfFs[0].FsHeight    = pstCfg->s32DecFsHeight;
            pstInst->stTfFs[0].ChromOffset = MemArrange.ChromOffset;
			pstInst->stTfFs[0].IsDecRef    = 0;
			pstInst->stTfFs[0].DispState   = FS_DISP_STATE_NOT_USED;

            /* ��ʾ����֡�� */
			if (0 == pstCfg->s32DispLoopEnable)
			{
                pstInst->s32DispFsNum = 0;
			}
            else
            {
                pstInst->s32DispFsNum = MIN((MemArrange.ValidFrameNum - pstInst->s32DecFsNum - pstInst->s32TfFsNum), 25);
				//if (pstInst->s32DispFsNum < pstCfg->s32ExpectedDispFsNum + pstCfg->s32ExpectedDecFsNum)
				if (pstInst->s32DispFsNum < pstCfg->s32ExpectedDispFsNum)
				{
                    pstInst->s8TotalValidFsNum = 0;
                    dprint(PRN_FATAL, "disp fs(=%d) less than expected(=%d+%d)!\n", pstInst->s32DispFsNum, pstCfg->s32ExpectedDecFsNum, pstCfg->s32ExpectedDispFsNum);
					return FSP_ERR;
				}
			}
			for (i = 0; i < MAX_FRAME_NUM; i++)
			{
				if (i < pstInst->s32DispFsNum)
				{
				    pstInst->stDispFs[i].PhyFsID     = i;
		            pstInst->stDispFs[i].PhyAddr     = MemArrange.FrameAddr[pstInst->s32DecFsNum + pstInst->s32TfFsNum + i];
					pstInst->stDispFs[i].Stride      = MemArrange.VahbStride;
					pstInst->stDispFs[i].FsWidth     = pstCfg->s32DispFsWidth;
					pstInst->stDispFs[i].FsHeight    = pstCfg->s32DispFsHeight;
		            pstInst->stDispFs[i].ChromOffset = MemArrange.ChromOffset;
					pstInst->stDispFs[i].IsDecRef    = 0;
					pstInst->stDispFs[i].DispState   = FS_DISP_STATE_NOT_USED;
				}
				else
				{
				    pstInst->stDispFs[i].PhyFsID     = 0;
		            pstInst->stDispFs[i].PhyAddr     = MemArrange.FrameAddr[pstInst->s32DecFsNum + pstInst->s32TfFsNum + 0];
					pstInst->stDispFs[i].Stride      = MemArrange.VahbStride;
					pstInst->stDispFs[i].FsWidth     = pstCfg->s32DispFsWidth;
					pstInst->stDispFs[i].FsHeight    = pstCfg->s32DispFsHeight;
		            pstInst->stDispFs[i].ChromOffset = MemArrange.ChromOffset;
					pstInst->stDispFs[i].IsDecRef    = 0;
					pstInst->stDispFs[i].DispState   = FS_DISP_STATE_NOT_USED;
				}
			}

			/* PMV �洢�� */
			pstInst->s32PmvNum  = MIN(MemArrange.ValidPMVNum, MAX_PMV_NUM);
			for (i = 0; i < MAX_PMV_NUM; i++ )
			{
				if (i < pstInst->s32PmvNum)
				{
	                pstInst->PmvAddr[i] = MemArrange.PMVAddr[i];
					pstInst->IsPmvInUse[i] = 0;
				}
				else
				{
	                pstInst->PmvAddr[i] = MemArrange.PMVAddr[0];
					pstInst->IsPmvInUse[i] = 0;
				}
			}
            pstInst->s32HalfPmvOffset = MemArrange.HafPmvOffset;

            /* α����֡��ʼ�� (H264��gap�����õ�α����֡) */
			pstInst->stDummyPhyFs.PhyFsID     = 0;
			pstInst->stDummyPhyFs.PhyAddr     = pstInst->stDecFs[0].PhyAddr; /* ��ַ����ʹ�ã�Ϊ����ȫ��㸳��ֵ���� */
			pstInst->stDummyPhyFs.Stride      = MemArrange.VahbStride;
			pstInst->stDummyPhyFs.FsWidth     = pstCfg->s32DecFsWidth;
			pstInst->stDummyPhyFs.FsHeight    = pstCfg->s32DecFsHeight;
			pstInst->stDummyPhyFs.ChromOffset = MemArrange.ChromOffset;
			pstInst->stDummyPhyFs.IsDecRef    = 0;
			pstInst->stDummyPhyFs.DispState   = 0;

             /* ��ѯ�ֱ����Ƿ�仯�����ǣ��¼��ϱ� */
             if (pstCfg->s32DecFsWidth != pstInst->stPreInfo.s32PreWidth || pstCfg->s32DecFsHeight != pstInst->stPreInfo.s32PreHeight || pstInst->s32DecFsNum != pstInst->stPreInfo.s32PreDecNum)
             {
                 if (pstInst->stPreInfo.s32PreWidth != 0 && pstInst->stPreInfo.s32PreHeight != 0 && pstInst->stPreInfo.s32PreDecNum != 0)  //�ų���һ�ηָ��Ĵ����ж�
                 {
                 #ifdef VFMW_CFG_CAP_HD_SIMPLE
                     pstInst->s8SizeChangeFlag = 1;
                     REPORT_RESOLUTION_CHANGE(InstID);
                 #endif
                 }
                                 
                 pstInst->stPreInfo.s32PreWidth = pstCfg->s32DecFsWidth;
                 pstInst->stPreInfo.s32PreHeight = pstCfg->s32DecFsHeight;
                 pstInst->stPreInfo.s32PreDecNum = pstInst->s32DecFsNum;
                 pstInst->stPreInfo.s32PreDecSlotLen = pstInst->stDecFs[1].PhyAddr - pstInst->stDecFs[0].PhyAddr;
             }
                     
			/* ��������ռ�õ��ڴ��� */
			*UsedMemSize = MemArrange.TotalMemUsed;
			return FSP_OK;
		}
		else
		{
            dprint(PRN_FATAL, "arrange ret %d, min = %d, actual = %d\n", Ret, MinFsNum, MemArrange.ValidFrameNum);
		}
	}
	else
	{
        /* ����2D scaler�����Ȼ���dec��tf֡�棬�ٷ�disp֡��. ����2d scaler����ʵ�� */
        dprint(PRN_ERROR, "%s %d err return!\n", __func__, __LINE__); 
	}
	
    return FSP_ERR;
}


/***********************************************************************
    ��ȡ����֡��ַ��
    VDH���ý���֡��ַʱ��Ҫ��"����֡��ַ�� + ֡ID"�ķ�ʽ��
 ***********************************************************************/
VOID  FSP_GetDecodeFrameTable(SINT32 InstID, UADDR PhyAddr[], SINT32 *pFsNum)
{
    SINT32 i;
    FSP_INST_S *pstInst;
    
    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");	
    FSP_ASSERT(PhyAddr != NULL, "PhyAddr is NULL\n");
    FSP_ASSERT(pFsNum != NULL, "pFsNum is NULL\n");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");

    pstInst = s_pFspInst[InstID];
    *pFsNum = pstInst->s32DecFsNum;
    for (i = 0; i < pstInst->s32DecFsNum; i++)
    {
        PhyAddr[i] = pstInst->stDecFs[i].PhyAddr;
    }
	
    return;
}


/***********************************************************************
    ��ȡ����PMV��ַ��
    VDH����PMV��ַʱ��Ҫ��"PMV��ַ�� + PMV ID"�ķ�ʽ��
 ***********************************************************************/
VOID  FSP_GetDecodePmvTable(SINT32 InstID, UADDR PhyAddr[], SINT32 *pPmvNum)
{
    SINT32 i;
    FSP_INST_S *pstInst;
    
    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");	
    FSP_ASSERT(PhyAddr != NULL, "PhyAddr is NULL\n");
    FSP_ASSERT(pPmvNum != NULL, "pFsNum is NULL\n");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");

    pstInst = s_pFspInst[InstID];
    *pPmvNum = pstInst->s32PmvNum;
    for (i = 0; i < pstInst->s32PmvNum; i++)
    {
        PhyAddr[i] = pstInst->PmvAddr[i];
    }
	
    return;
}

/***********************************************************************
    �ж��Ƿ������뵽��֡
    ��������뵽������1�����򷵻�0
 ***********************************************************************/
SINT32  FSP_IsNewFsAvalible(SINT32 InstID)
{
    SINT8  flag = 0;
    SINT32 i;
    FSP_INST_S *pstInst;

    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");	
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");	

    pstInst = s_pFspInst[InstID];

    /* 0. ���PartitionFsMemory������δִ�У��̶�����1�����������ò���������Ҳ��û�л���⵽SEQ */
    if (0 == pstInst->s32DecFsNum)
    {
        return 1;
    }

    /* 1. �н���֡��? */
    for (i = 0; i < pstInst->s32DecFsNum; i++)
    {
        if ((pstInst->stDecFs[i].IsDecRef == 0) && (pstInst->stDecFs[i].DispState == FS_DISP_STATE_NOT_USED))
        {
            if (FSP_IsOmxBypassMode(InstID) && (FSP_ExtBufHandler(InstID, BC_CHK_BUF, &pstInst->stDecFs[i].PhyAddr) != 1))
            {
                flag = 1;
                continue;
            }
            else
            {
                break;
            }
        }
    }
	
    if (i >= pstInst->s32DecFsNum)
    {
        goto fs_leak_detect;
    }

    return 1;


fs_leak_detect:

    if (FSP_IsFsLeak(InstID))
    {
        return -1;
    }

    if (flag)
    {
        return 2;
    }

    return 0;
}

/* ����Ƿ�����֡��(��Ҫ����ʾ֡��)й© */
SINT32 FSP_IsFsLeak(SINT32 InstID)
{
    SINT32 i;
    SINT32  RefNum, NewNum, ReadNum;    
    SINT32  TotalFsNum;
    FSP_INST_S *pstInst;

    pstInst = s_pFspInst[InstID];
    if (NULL == pstInst)
    {
       dprint(PRN_ERROR, "FSP_IsFsLeak: fsp is not inited \n");
			return FSP_ERR;
    }            
    
    TotalFsNum = pstInst->s32DecFsNum;
    for (i = 0; i < pstInst->s32DecFsNum; i++)
    {
        if ((pstInst->stDecFs[i].IsDecRef == 0) && (pstInst->stDecFs[i].DispState == FS_DISP_STATE_NOT_USED))
        {
            break;
	  }
    }

    /* ���û�п���֡�棬Ҫ�ж��Ƿ��ڶ����� */
    if (i >= TotalFsNum)
    {
        RefNum = ReadNum = NewNum = 0;
        VCTRL_GetChanImgNum(InstID, &RefNum, &ReadNum, &NewNum);
        if (RefNum + ReadNum + NewNum + 2 < TotalFsNum)
        {
            dprint(PRN_FATAL,"No free fs: ref=%d,read=%d,new=%d,total=%d\n",RefNum, ReadNum, NewNum, TotalFsNum);
            FSP_PrintContext(InstID);
            return 1;
        }
    }

    return 0;
}


/* ��һ��tick��С������֡�棬����֡��� */
SINT32 FindLeastTickPhyFs(FSP_PHY_FS_S *pstPhyFs, SINT32 PhyNum)
{
    SINT32 i, LeastFsID = 0;
    UINT32 LeastTick;

    LeastTick = 0xffffffff;
    for (i = 0; i < PhyNum; i++)
    {
        if (pstPhyFs->u32Tick < LeastTick)
        {
              LeastTick = pstPhyFs->u32Tick;
          	LeastFsID = i;
        }
        pstPhyFs++;   //yyc change
    }

    return LeastFsID;
}

/***********************************************************************
    ��ȡ�µ��߼�֡
    InstID: ʵ��ID
    NeedPhyFs: ��־�Ƿ���Ҫ��������֡�档H.264�ڽ���gapʱҲҪ����֡�棬
               ������֡�治��Ҫ����ʵ������֡���Դ�����.
               
    �ɹ������߼�֡��ID��ʧ�ܷ���-1
 ***********************************************************************/
#define  PHYFS_USED(pFs) \
 ((pFs != NULL) && (((pFs)->DispState != FS_DISP_STATE_NOT_USED) || ((pFs)->IsDecRef != 0) || ((pFs)->IsTfRef != 0)))
 
SINT32 FSP_NewLogicFs ( SINT32 InstID, SINT32 NeedPhyFs )
{
    SINT32 i;
    FSP_INST_S *pstInst;
    FSP_LOGIC_FS_S *pstLogicFs = NULL;
    FSP_PHY_FS_S *pstDecFs, *pstDispFs, *pstTfFs;
    IMAGE *pstImg;
 
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");
     
    VFMW_OSAL_SpinLock(G_SPINLOCK_FSP);
    
    pstInst = s_pFspInst[InstID];

	/* Ѱ��һ�����е��߼�֡�� */
	pstLogicFs = NULL;

    for (i = 0; i < MAX_FRAME_NUM; i++)
    {
        /* Ϊ��ֹ�߼�֡��й©����pstDecodeFs��pstDispOutFs��pstTfOutFsȫ����Ч���߼�֡��Ϊ�����߼�֡ */
        if (pstInst->stLogicFs[i].s32IsRef == 0 && pstInst->stLogicFs[i].s32DispState == FS_DISP_STATE_NOT_USED)
        {
              pstLogicFs = &pstInst->stLogicFs[i];
              pstLogicFs->s32LogicFsID = i;
              pstLogicFs->IsDummyFs = 0;
              break;
        }
	}
    
    if (NULL == pstLogicFs)
    {
        dprint(PRN_FATAL, "no logic frame store!!!\n");
        VFMW_OSAL_SpinUnLock(G_SPINLOCK_FSP);
        /* ��ϴ֡��ز����������� */
        return -1;
	}
	
    pstLogicFs->pstDecodeFs  = NULL;
	pstLogicFs->pstDispOutFs = NULL;
	pstLogicFs->pstTfOutFs   = NULL;
	pstLogicFs->PmvAddr      = 0;

    if (0 == NeedPhyFs)
    {
        /* ֻ���߼�֡�������������֡. ֻ��H.264��GAP��ʱ��Ż����˷�֧ */
		pstLogicFs->pstDecodeFs  = &pstInst->stDummyPhyFs;
		pstLogicFs->pstDispOutFs = &pstInst->stDummyPhyFs;
		pstLogicFs->pstTfOutFs   = &pstInst->stDummyPhyFs;
		pstLogicFs->PmvAddr      = pstInst->PmvAddr[0];   /* pmv��ַ��ID�����õ���Ϊ��ȫ����ֵ���� */
		pstLogicFs->PmvIdc       = pstInst->s32PmvNum - 1;
		pstLogicFs->IsDummyFs    = 1;
    }
	else
	{
        /*** �������֡�� ***/	
		for (i=0; i < pstInst->s32DecFsNum; i++)
		{
            if (pstInst->stDecFs[i].IsDecRef == 0 && pstInst->stDecFs[i].DispState == FS_DISP_STATE_NOT_USED)
            {
                if (FSP_IsOmxBypassMode(InstID) && (FSP_ExtBufHandler(InstID, BC_CHK_BUF, &pstInst->stDecFs[i].PhyAddr) != 1))
                {
                    continue;
                }
                else
                {
                    pstLogicFs->pstDecodeFs = &pstInst->stDecFs[i];
                    break;
                }
            }
		}
		if (NULL == pstLogicFs->pstDecodeFs)
	    {
		    dprint(PRN_FATAL, "%s no decode phy fs\n", __func__);
    		for (i = 0; i < pstInst->s32DecFsNum; i++)
    		{
    		    dprint(PRN_FATAL,"%d%d ",pstInst->stDecFs[i].IsDecRef,pstInst->stDecFs[i].DispState);
    		}
    		dprint(PRN_FATAL,"\n");

            if (pstInst->s8TotalValidFsNum > 0)
            {
    			/* �ݴ�: ��һ����"��"�Ľ���֡ */
    			i = FindLeastTickPhyFs(&pstInst->stDecFs[0], pstInst->s32DecFsNum);
    			pstLogicFs->pstDecodeFs = &pstInst->stDecFs[i];
            }
            else
            {
                dprint(PRN_FATAL, "%s Not partition yet!\n", __func__);
                VFMW_OSAL_SpinUnLock(G_SPINLOCK_FSP);
                return -1;
            }
		}

		/*** ����TF֡�� ***/
		pstLogicFs->pstTfOutFs = &pstInst->stTfFs[0];

        /* ����PMV�洢�� */
		if (pstInst->s32PmvNum > 0)
  		{
            for (i = 0; i < pstInst->s32PmvNum; i++)
            {
                if (0 == pstInst->IsPmvInUse[i])
                {
                    pstLogicFs->PmvIdc  = i;
                    pstLogicFs->PmvAddr = pstInst->PmvAddr[i];
                    pstLogicFs->HalfPmvOffset = pstInst->s32HalfPmvOffset;
                    break;
                }
            }

            if (i >= pstInst->s32PmvNum)
            {
                for (i = 0; i < pstInst->s32PmvNum; i++)
                {
                    pstInst->IsPmvInUse[i] = 0;
                }

                pstLogicFs->PmvIdc  = 0;
                pstLogicFs->PmvAddr = pstInst->PmvAddr[0];
                pstLogicFs->HalfPmvOffset = pstInst->s32HalfPmvOffset;
                dprint(PRN_FATAL, "no pmv memory\n");
            }
	    }

		/*** ������ʾ֡�� ***/
		if (0 == pstInst->stInstCfg.s32DispLoopEnable)
		{
            pstLogicFs->pstDispOutFs = pstLogicFs->pstDecodeFs;
		}
	}
	
    /* ��ʾ֡����Ϊ"Ԥռ��"��ֹ�ظ�ʹ�� */
	pstLogicFs->s32DispState = FS_DISP_STATE_ALLOCED;
	pstLogicFs->pstDispOutFs->DispState = FS_DISP_STATE_ALLOCED;
	pstLogicFs->eLfsPmvState            = LFS_PMV_STATE_ALLOCATED;	

    /* ������֡�����"ʱ���"����ֹ�Ժ����ڲ������ϵ�ԭ�������� */
	pstLogicFs->u32Tick               = pstInst->u32InstTick;
	pstLogicFs->pstDecodeFs->u32Tick  = pstLogicFs->u32Tick;
	pstLogicFs->pstDispOutFs->u32Tick = pstLogicFs->u32Tick;
	pstInst->u32InstTick++;

    /* ��дIMAGE�ṹ����Ӧ��Ϣ */
	pstDecFs = pstLogicFs->pstDecodeFs;
	pstDispFs = pstLogicFs->pstDispOutFs;
	pstTfFs = pstLogicFs->pstTfOutFs;
	pstImg = &pstLogicFs->stDispImg;

	memset(pstImg, 0, sizeof(IMAGE));
	pstImg->image_id           = pstLogicFs->s32LogicFsID;
    pstImg->image_width        = pstDecFs->FsWidth;
	pstImg->image_height       = pstDecFs->FsHeight;
	pstImg->image_stride       = pstDispFs->Stride;

	pstImg->luma_phy_addr      = pstDecFs->PhyAddr;
    pstImg->chrom_phy_addr     = pstImg->luma_phy_addr + pstDecFs->ChromOffset;
	pstImg->top_luma_phy_addr  = pstDispFs->PhyAddr;
	pstImg->top_chrom_phy_addr = pstImg->top_luma_phy_addr + pstDispFs->ChromOffset;
	pstImg->btm_luma_phy_addr  = pstImg->top_luma_phy_addr + pstDispFs->Stride;
	pstImg->btm_chrom_phy_addr = pstImg->top_chrom_phy_addr + pstDispFs->Stride;

	// ��дͼ�������TF�õļ�����ַ
	pstImg->luma_2d_phy_addr   = pstDispFs->PhyAddr;
    pstImg->chrom_2d_phy_addr  = pstImg->luma_2d_phy_addr + pstDispFs->ChromOffset;

    VFMW_OSAL_SpinUnLock(G_SPINLOCK_FSP);

    return pstLogicFs->s32LogicFsID;
}


/***********************************************************************
    ɾ��ָ���߼�֡��PMV��
    PMV����ÿһ������֡�󶨣��ڽ�һ֡���Ϊ�ο�ʱ������PMV��Ҳ����������
    ����ÿһ���ο�֡����ҪPMV��(ǰ��ο�֡��PMV���豣��)��������һ���ο�
    ֡��PMV�鲻��Ҫ����ʱ����ɾ�����ɱ�֤���̶ȵؽ�ʡ�ڴ档
    ���PMV���㹻(����MPEG2Э����3��PMV��)����������ʹ��Ҳ������������.
 ***********************************************************************/
VOID  FSP_RemovePmv ( SINT32 InstID, SINT32 LogicFrameID )
{
    FSP_LOGIC_FS_S *pstLogicFs;
    FSP_INST_S *pstInst = s_pFspInst[InstID];

    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT(LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM, "LogicFrameID out of range");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");

    pstLogicFs = &pstInst->stLogicFs[LogicFrameID];

    if (LFS_PMV_STATE_VALID == pstLogicFs->eLfsPmvState)
    {
        if (pstLogicFs->PmvIdc >= 0 && pstLogicFs->PmvIdc < MAX_PMV_NUM)
        {
            pstInst->IsPmvInUse[pstLogicFs->PmvIdc] = 0;
        }
        pstLogicFs->eLfsPmvState = LFS_PMV_STATE_NOT_USE;
    }

    return;
}


/***********************************************************************
    �����߼�֡�Ĳο�����, ��������֡��TF�ο�֡
    IsRef = 1���Ϊ���ο�����֮���߼�֡���Ϊ���ο�
 ***********************************************************************/
VOID  FSP_SetRef ( SINT32 InstID, SINT32 LogicFrameID, SINT32 IsRef )
{
    FSP_LOGIC_FS_S *pstLogicFs = NULL;
    FSP_INST_S *pstInst = NULL;
    
    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT(LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM, "LogicFrameID out of range");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");
    
    pstInst = s_pFspInst[InstID];

    if(NULL == pstInst)
    {
    	dprint(PRN_DBG, "%s %d NULL == pstInst!!\n",__FUNCTION__,__LINE__);
    	return;
    }
    
    pstLogicFs = &pstInst->stLogicFs[LogicFrameID];
    pstLogicFs->s32IsRef = IsRef;

    if (pstLogicFs->IsDummyFs)   /* �����gap֡������ָ������֡�������壬�������� */
    {
        return;
    }

    /*** 1. ����ο�����(��������֡��PMV) ***/
    /* tick�����ȣ�˵������֡��Ӧ�ñ����ᣬ�������߼�֡���ˣ�������»����vdec�ⲿ֡��й© */
    if ((NULL != pstLogicFs->pstDecodeFs) && (pstLogicFs->pstDecodeFs->u32Tick == pstLogicFs->u32Tick))
    {
        pstLogicFs->pstDecodeFs->IsDecRef = IsRef;
    }
    // PMV��
    if ((pstLogicFs->PmvIdc >= 0) && (pstLogicFs->PmvIdc < MAX_PMV_NUM))
    {
        if ((1 == IsRef) && (LFS_PMV_STATE_ALLOCATED == pstLogicFs->eLfsPmvState))
        {
            if( NULL == pstInst)
            {
                dprint(PRN_DBG, "%s %d pstInst is null!!\n",__FUNCTION__,__LINE__);
                return;
            }
            pstInst->IsPmvInUse[pstLogicFs->PmvIdc] = 1;

            pstLogicFs->eLfsPmvState = LFS_PMV_STATE_VALID;
        }
        else if ((0 == IsRef) && (LFS_PMV_STATE_VALID == pstLogicFs->eLfsPmvState))
        {
            if( NULL == pstInst)
            {
                dprint(PRN_DBG, "%s %d pstInst is null!!\n",__FUNCTION__,__LINE__);
                return;
            }
            pstInst->IsPmvInUse[pstLogicFs->PmvIdc] = 0;
            pstLogicFs->PmvAddr = 0;
            pstLogicFs->eLfsPmvState = LFS_PMV_STATE_NOT_USE;
        }
    }
    else
    {
        dprint(PRN_FATAL, "fsp.c, line %d: pstLogicFs->PmvIdc out of range(=%d)\n", __LINE__,pstLogicFs->PmvIdc);
    }
    
    /* ������Ϊ"���ο�"����Ҫ���һЩ��Ϣ */
    if (0 == IsRef)
    {
    	if (NULL != pstLogicFs->pstDispOutFs && pstLogicFs->pstDispOutFs->DispState == FS_DISP_STATE_ALLOCED)
    	{
          pstLogicFs->pstDispOutFs->DispState = FS_DISP_STATE_NOT_USED;
    	}
    	//pstLogicFs->pstDecodeFs = NULL;
    }

    /* ������Ϊ"�ο�"��˵����һ֡������ϣ���Ӧ����ʾ����֡Ҫ���Ϊ"�������" */
    if ((NULL != pstLogicFs->pstDispOutFs) && (pstLogicFs->pstDispOutFs->u32Tick == pstLogicFs->u32Tick))
    {
        if ((1 == IsRef) && FS_DISP_STATE_ALLOCED == pstLogicFs->pstDispOutFs->DispState)
        {
           pstLogicFs->pstDispOutFs->DispState = FS_DISP_STATE_DEC_COMPLETE;
           pstLogicFs->s32DispState = FS_DISP_STATE_DEC_COMPLETE;
    	}
    }

    if (0 == IsRef)
    {
    	pstLogicFs->pstDecodeFs = NULL;
    }

    return;
}

/***********************************************************************
    ��ȡ�߼�֡�Ľ���ο�����
 ***********************************************************************/
SINT32  FSP_GetRef ( SINT32 InstID, SINT32 LogicFrameID )
{
    FSP_LOGIC_FS_S *pstLogicFs;
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "FSP_GetRef: InstID out of range");
    FSP_ASSERT_RET(LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM, "FSP_GetRef: LogicFrameID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "FSP_GetRef: Fsp is not inited");
    
    pstLogicFs = &pstInst->stLogicFs[LogicFrameID];

    return pstLogicFs->s32IsRef;
}


/***********************************************************************
    �����߼�֡�Ƿ�ȴ���ʾ
 ***********************************************************************/
VOID  FSP_SetDisplay ( SINT32 InstID, SINT32 LogicFrameID, SINT32 IsWaitDisp )
{
    FSP_LOGIC_FS_S *pstLogicFs;
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT(LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM, "LogicFrameID out of range");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");
    
    VFMW_OSAL_SpinLock(G_SPINLOCK_FSP);
   
    pstLogicFs = &pstInst->stLogicFs[LogicFrameID];

    pstLogicFs->s32DispState = (0 == IsWaitDisp)? FS_DISP_STATE_NOT_USED : FS_DISP_STATE_WAIT_DISP;

    if (pstLogicFs->IsDummyFs || NULL == pstLogicFs->pstDispOutFs)
    {
        VFMW_OSAL_SpinUnLock(G_SPINLOCK_FSP);
        return;
    }
	
    if (pstLogicFs->pstDispOutFs->u32Tick != pstLogicFs->u32Tick)
    {
        dprint(PRN_FATAL,"FSP_SetDisplay: LogicTick=%d, DispTick=%d!\n",pstLogicFs->u32Tick,pstLogicFs->pstDispOutFs->u32Tick);       
        VFMW_OSAL_SpinUnLock(G_SPINLOCK_FSP);
        return;
    }
 
    if (0 == IsWaitDisp)
    {
        pstLogicFs->s32DispState = FS_DISP_STATE_NOT_USED;
        pstLogicFs->pstDispOutFs->DispState = FS_DISP_STATE_NOT_USED;
    }
    else
    {
        pstLogicFs->s32DispState = FS_DISP_STATE_WAIT_DISP;
        pstLogicFs->pstDispOutFs->DispState = FS_DISP_STATE_WAIT_DISP;
    }

    if (0 == IsWaitDisp)
    {
        pstLogicFs->pstDispOutFs = NULL;
    }
    
    VFMW_OSAL_SpinUnLock(G_SPINLOCK_FSP);

    return;
}

/***********************************************************************
    ��ȡ�߼�֡�Ƿ�ȴ���ʾ
 ***********************************************************************/
SINT32  FSP_GetDisplay ( SINT32 InstID, SINT32 LogicFrameID )
{   
    FSP_LOGIC_FS_S *pstLogicFs;
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");
   
    pstLogicFs = &pstInst->stLogicFs[LogicFrameID];
    
    return pstLogicFs->s32DispState;
}

/***********************************************************************
    ��������֡��Ĵ洢��ʽ(��֡�洢/�����洢)
 ***********************************************************************/
SINT32  FSP_SetStoreType( SINT32 InstID, FSP_PHY_FS_TYPE_E ePhyFsType, SINT32 PhyFsID, FSP_PHY_FS_STORE_TYPE_E eStoreType)
{
    SINT32 Ret = FSP_ERR;
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");

    if (FSP_PHY_FS_TYPE_DECODE == ePhyFsType)
    {
        if (PhyFsID < pstInst->s32DecFsNum)
        {
            pstInst->stDecFs[PhyFsID].eStoreType = eStoreType;
            Ret = FSP_OK;
        }
    }
    else if (FSP_PHY_FS_TYPE_DISPLAY == ePhyFsType)
    {
        if (PhyFsID < pstInst->s32DispFsNum)
        {
            pstInst->stDispFs[PhyFsID].eStoreType = eStoreType;
            Ret = FSP_OK;
        }
    }
    
    return Ret;
}

/***********************************************************************
    ��ȡ����֡��Ĵ洢��ʽ(��֡�洢/�����洢)
 ***********************************************************************/
FSP_PHY_FS_STORE_TYPE_E  FSP_GetStoreType( SINT32 InstID, FSP_PHY_FS_TYPE_E ePhyFsType, SINT32 PhyFsID)
{
    FSP_PHY_FS_STORE_TYPE_E eStoreType = FSP_PHY_FS_STORE_TYPE_BUTT;   
    FSP_INST_S *pstInst = s_pFspInst[InstID]; 

    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");

    if (FSP_PHY_FS_TYPE_DECODE == ePhyFsType)
    {
        if (PhyFsID < pstInst->s32DecFsNum)
        {
            eStoreType = pstInst->stDecFs[PhyFsID].eStoreType;
        }
    }
    else if (FSP_PHY_FS_TYPE_DISPLAY == ePhyFsType)
    {
        if (PhyFsID < pstInst->s32DispFsNum)
        {
            eStoreType = pstInst->stDispFs[PhyFsID].eStoreType;
        }
    }

    return eStoreType;
}



/***********************************************************************
    ����߼�֡
    ���Ҫ���ⶪ��һ���Ѿ����뵽���߼�֡�����Էֱ��������Ϊ������ο���
    ������ʾ������TF�ο��ȣ�����õ��ô˺�����ʹ�ü����ܱ�֤������߼�
    ֡�Ϲ�������Դ��ȫ�ͷŵ�
    HoldDispFs: ָʾ�Ƿ���Ҫ��������ʾ����ʾ֡�棬1��ʾ����߼�֡�ڵ���ʾ
                ֡���Ѳ�����ʾ��������������������߼�֡���������
                ����HoldDispFsΪ��ֵ����ǰ�߼�֡�Ĳο�����һ���ᱻ���.
 ***********************************************************************/
VOID FSP_ClearLogicFs( SINT32 InstID, SINT32 LogicFrameID, SINT32 HoldDispFs)
{
    FSP_LOGIC_FS_S *pstLogic;
    FSP_INST_S *pstInst = s_pFspInst[InstID];

    if ((InstID>=0 && InstID < MAX_CHAN_NUM) && (LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM) && NULL != s_pFspInst[InstID])
    {
        FSP_SetRef(InstID, LogicFrameID, 0);
        pstLogic = &pstInst->stLogicFs[LogicFrameID];
        
        if (FS_DISP_STATE_WAIT_DISP != pstLogic->s32DispState)
        {
            FSP_SetDisplay(InstID, LogicFrameID, 0);
        }
    }

    return ;
}


/***********************************************************************
    ��ȡ�߼�֡
 ***********************************************************************/
FSP_LOGIC_FS_S*  FSP_GetLogicFs( SINT32 InstID, SINT32 LogicFrameID)
{
    FSP_LOGIC_FS_S *pstLogicFs;
    
    if ((InstID>=0 && InstID < MAX_CHAN_NUM) && (LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM) && NULL != s_pFspInst[InstID])
    {        
        pstLogicFs = &s_pFspInst[InstID]->stLogicFs[LogicFrameID];
        
        return (pstLogicFs);
    }

    return NULL;
}

/***********************************************************************
    ��ȡ�߼�֡�����ͼ��(ָ��IMAGE�ṹ���ָ��)
 ***********************************************************************/
IMAGE*  FSP_GetFsImagePtr( SINT32 InstID, SINT32 LogicFrameID)
{
    FSP_LOGIC_FS_S *pstLogicFs;
    
    if ((InstID>=0 && InstID < MAX_CHAN_NUM) && (LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM) && NULL != s_pFspInst[InstID])
    {      
        pstLogicFs = &s_pFspInst[InstID]->stLogicFs[LogicFrameID];
           
        return (&pstLogicFs->stDispImg);
    }

    return NULL;
}


/***********************************************************************
    ��ȡ��������֡��
 ***********************************************************************/
FSP_PHY_FS_S*  FSP_GetDecPhyFs( SINT32 InstID, SINT32 LogicFrameID)
{
    FSP_LOGIC_FS_S *pstLogicFs;
    
    if ((InstID>=0 && InstID < MAX_CHAN_NUM) && (LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM) && NULL != s_pFspInst[InstID])
    {  
        pstLogicFs = &s_pFspInst[InstID]->stLogicFs[LogicFrameID];
           
        return (pstLogicFs->pstDecodeFs);
    }

    return NULL;
}


/***********************************************************************
    ��ȡ�������֡��
 ***********************************************************************/
FSP_PHY_FS_S*  FSP_GetDispPhyFs( SINT32 InstID, SINT32 LogicFrameID)
{
    FSP_LOGIC_FS_S *pstLogicFs;
    
    if ((InstID>=0 && InstID < MAX_CHAN_NUM) && (LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM) && NULL != s_pFspInst[InstID])
    {  
        pstLogicFs = &s_pFspInst[InstID]->stLogicFs[LogicFrameID];
        return (pstLogicFs->pstDispOutFs);
    }
	
    return NULL;
}


/***********************************************************************
    ��ȡPMV��ַ
 ***********************************************************************/
UADDR FSP_GetPmvAddr( SINT32 InstID, SINT32 LogicFrameID )
{
    FSP_LOGIC_FS_S *pstLogicFs;
    
    if ((InstID>=0 && InstID < MAX_CHAN_NUM) && (LogicFrameID>=0 && LogicFrameID < MAX_FRAME_NUM) && NULL != s_pFspInst[InstID])
    {  
        pstLogicFs = &s_pFspInst[InstID]->stLogicFs[LogicFrameID];
        return (pstLogicFs->PmvAddr);
    }

    return 0;
}

/***********************************************************************
    ��ȡDecFs��ַ��
    264Э���õ�ַ��+ID�ķ�ʽ���߼����õ�ַ��Ϣ��������Ҫ��ȡ����֡���ַ��
 ***********************************************************************/
VOID  FSP_GetDecFsAddrTab( SINT32 InstID, SINT32 *DecFsNum, SINT32 *DecFsAddrTab ,SINT32 *LineNumAddrTab)
{
    FSP_PHY_FS_S *pstDecFs;
    SINT32 i;
    
    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");	
    FSP_ASSERT(DecFsAddrTab != NULL, "DecFsAddrTab is NULL\n");
    FSP_ASSERT(DecFsNum != NULL, "DecFsNum is NULL\n");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");
    
    *DecFsNum = s_pFspInst[InstID]->s32DecFsNum;
    pstDecFs = &s_pFspInst[InstID]->stDecFs[0];
    for (i = 0; i < *DecFsNum; i++)
    {
        DecFsAddrTab[i] = pstDecFs[i].PhyAddr;
    #ifdef VFMW_MODULE_LOWDLY_SUPPORT
        LineNumAddrTab[i] = pstDecFs[i].LineNumAddr;
    #endif
    }

    return;
}


/***********************************************************************
    ��ȡPMV��ַ��
    264Э���õ�ַ��+ID�ķ�ʽ���߼�����PMV��ַ��Ϣ��������Ҫ��ȡPMV��ַ��
 ***********************************************************************/
VOID  FSP_GetPmvAddrTab( SINT32 InstID, SINT32 *PmvNum, SINT32 *PmvAddrTab )
{
    SINT32 i;
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");	
    FSP_ASSERT(PmvAddrTab != NULL, "PmvAddrTab is NULL\n");
    FSP_ASSERT(PmvNum != NULL, "PmvNum is NULL\n");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");
    
    *PmvNum = pstInst->s32PmvNum;
    for (i = 0; i < pstInst->s32PmvNum; i++)
    {
        PmvAddrTab[i] = pstInst->PmvAddr[i];
    }

    return;
}

VOID  FSP_GetInstInfo( SINT32 InstID, FSP_INST_INFO_S *pFspInstInfo )
{
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");
   
    pFspInstInfo->s32DecFsNum  = pstInst->s32DecFsNum;
    pFspInstInfo->s32DecFsSize = pstInst->stDecFs[1].PhyAddr - pstInst->stDecFs[0].PhyAddr;
    pFspInstInfo->s32DispFsNum = pstInst->s32DispFsNum;
    pFspInstInfo->s32TfFsNum   = pstInst->s32TfFsNum;
    pFspInstInfo->s32InstID    = pstInst->s32InstID;	
    pFspInstInfo->s32PmvNum    = pstInst->s32PmvNum;	
    memcpy(&pFspInstInfo->stDecFs[0], &pstInst->stDecFs[0], sizeof(FSP_PHY_FS_S)*MAX_FRAME_NUM);
 
    return;
}

SINT32  FSP_GetLogicIdByPhyAddr( SINT32 InstID, UADDR PhyAddr)
{
    SINT32 i;
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");

    for (i=0; i<MAX_FRAME_NUM; i++)
    {
        if (pstInst->stLogicFs[i].pstDecodeFs == NULL)
        {
            continue;
        }
        if (PhyAddr == pstInst->stLogicFs[i].pstDecodeFs->PhyAddr)
        {
            return i;
        }
    }
        
    return -1;
}

/***********************************************************************
    ��ӡFSP�ڲ�״̬
    ����ʵʱ����ʱ�������ض�����̽��FSP�ڲ�״̬
 ***********************************************************************/
VOID FSP_PrintContext(SINT32 InstID)
{
    SINT32 i;
    FSP_INST_S *pstInst;
    FSP_PHY_FS_S *pstPhy;
    FSP_LOGIC_FS_S *pstLogic;
    
    FSP_ASSERT(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT(NULL != s_pFspInst[InstID], "fsp is not inited");

    pstInst = s_pFspInst[InstID];    
    if (NULL == pstInst)
    {
        dprint(PRN_FATAL,"%s s_pFspInst[%d]=NULL\n", __func__, InstID);
        return;
    }

    dprint(PRN_FATAL,"FSP CONTEXT:\n");
    dprint(PRN_FATAL,"%-35s :%d\n", "FspInstState",      pstInst->eFspInstState);
    dprint(PRN_FATAL,"%-35s :%d\n", "DecFsWidth",        pstInst->stInstCfg.s32DecFsWidth);
    dprint(PRN_FATAL,"%-35s :%d\n", "DecFsHeight",       pstInst->stInstCfg.s32DecFsHeight);
    dprint(PRN_FATAL,"%-35s :%d\n", "DispFsWidth",       pstInst->stInstCfg.s32DispFsWidth);
    dprint(PRN_FATAL,"%-35s :%d\n", "DispFsHeight",      pstInst->stInstCfg.s32DispFsHeight);
    dprint(PRN_FATAL,"%-35s :%d\n", "ExpectedDecFsNum",  pstInst->stInstCfg.s32ExpectedDecFsNum);
    dprint(PRN_FATAL,"%-35s :%d\n", "ExpectedDispFsNum", pstInst->stInstCfg.s32ExpectedDispFsNum);
    dprint(PRN_FATAL,"%-35s :%d\n", "ExpectedPmvNum",    pstInst->stInstCfg.s32ExpectedPmvNum);
    dprint(PRN_FATAL,"%-35s :%d\n", "DecFsNum",          pstInst->s32DecFsNum);
    dprint(PRN_FATAL,"%-35s :%d\n", "DispFsNum",         pstInst->s32DispFsNum);
    dprint(PRN_FATAL,"%-35s :%d\n", "PmvNum",            pstInst->s32PmvNum);

    dprint(PRN_FATAL,"logic fs: %d\n", MAX_FRAME_NUM);
    for (i = 0; i < MAX_FRAME_NUM; i++)
    {
        pstLogic = &pstInst->stLogicFs[i];
        if ((i != 0) && ((i&7) == 0) )
        {
            dprint(PRN_FATAL,"\n");
        }
        dprint(PRN_FATAL,"%d%d ", pstLogic->s32IsRef, pstLogic->s32DispState);
    }
    dprint(PRN_FATAL,"\ndec fs: %d\n", pstInst->s32DecFsNum);
    for (i = 0; i < pstInst->s32DecFsNum; i++)
    {
        pstPhy = &pstInst->stDecFs[i];
        if ((i != 0) && ((i&7) == 0) )
        {
            dprint(PRN_FATAL,"\n");
        }
        dprint(PRN_FATAL,"%d%d ", pstPhy->IsDecRef, pstPhy->DispState);
    }
    dprint(PRN_FATAL,"\ndisp fs: %d\n", pstInst->s32DispFsNum);
    for (i = 0; i < pstInst->s32DispFsNum; i++)
    {
        pstPhy = &pstInst->stDispFs[i];
        if ((i != 0) && ((i&7) == 0) )
        {
            dprint(PRN_FATAL,"\n");
        }
        dprint(PRN_FATAL,"%d%d ", pstPhy->IsDecRef, pstPhy->DispState);
    }
    dprint(PRN_FATAL,"\npmv: %d\n", pstInst->s32PmvNum);
    for (i = 0; i < pstInst->s32PmvNum; i++)
    {
        if ((i != 0) && ((i&15) == 0) )
        {
            dprint(PRN_FATAL,"\n");
        }
        dprint(PRN_FATAL,"%d ", pstInst->IsPmvInUse[i]);
    }
    dprint(PRN_FATAL,"\n\n");

    return;
}


/***********************************************************************
    ��ȡ�����߼�֡��ID
    �ο�֡��λʱ��
 ***********************************************************************/
SINT32 FSP_GetNullLogicFs(SINT32 InstID)
{
    SINT32 i;
    SINT32 ret_id = 0;
    
    FSP_INST_S *pstInst = s_pFspInst[InstID];
    
    for (i=MAX_FRAME_NUM-1; i>=0; i--)
    {
        if (FS_DISP_STATE_NOT_USED == pstInst->stLogicFs[i].s32DispState)
        {
            ret_id = i;
            break;
        }
    }
    
    pstInst->stLogicFs[ret_id].pstDecodeFs = NULL;
    pstInst->stLogicFs[ret_id].pstDispOutFs = NULL;
    
    return ret_id;
}

/***********************************************************************
    ��ѯ�Ƿ���֡�����޵����ռ�
 ***********************************************************************/
SINT32 FSP_GetTotalValidFsNum(SINT32 InstID)
{     
    FSP_ASSERT_RET(InstID>=0 && InstID < MAX_CHAN_NUM, "InstID out of range");
    FSP_ASSERT_RET(NULL != s_pFspInst[InstID], "fsp is not inited");
 
    return s_pFspInst[InstID]->s8TotalValidFsNum;
}


