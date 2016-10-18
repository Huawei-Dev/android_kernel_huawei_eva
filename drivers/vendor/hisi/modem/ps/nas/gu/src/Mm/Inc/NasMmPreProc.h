/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NasMmPreProc.h
  版 本 号   : 初稿
  作    者   : w00176964
  生成日期   : 2014年4月16日
  最近修改   :
  功能描述   : NasMmPreProc.c的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年4月16日
    作    者   : W00176964
    修改内容   : 创建文件
******************************************************************************/
#ifndef _NAS_MM_PREPROC_H_
#define _NAS_MM_PREPROC_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*******************************************************************************
  3 枚举定义
*******************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/
/*模块名＋意义＋HEADER
 */


/*****************************************************************************
  6 消息定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
VOS_UINT8   NAS_MM_RcvMmcMmSysInfoInd_PreProc(
    VOS_VOID                           *pRcvMsg
);

VOS_UINT8 NAS_MM_IsNeedDiscardMmcMmSysInfoInd( VOS_VOID );

VOS_VOID NAS_MM_ProcMmcMmSysInfoInd_CsNotSupport_PreProc( VOS_VOID );

VOS_VOID NAS_MM_ProcMmcMmSysInfoInd_NoImsi_PreProc( VOS_VOID );

#if (FEATURE_ON == FEATURE_ECALL)
VOS_UINT8 NAS_MM_ProcMmcMmSysInfoInd_ECallOnly_PreProc(
    VOS_VOID                           *pRcvMsg
);

#endif
VOS_UINT8 NAS_MM_RcvMmcCoverageLostInd_PreProc(
    VOS_VOID                           *pRcvMsg
);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasMmPreProc.h*/

