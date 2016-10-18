/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : TafApsSndPpp.h
  版 本 号   : 初稿
  作    者   : h00246512
  生成日期   : 2014年10月21日
  最近修改   :
  功能描述   : TafApsSndPpp.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年10月21日
    作    者   : h00246512
    修改内容   : 创建文件

******************************************************************************/

#ifndef __TAFAPSSNDPPP_H__
#define __TAFAPSSNDPPP_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "taf_aps_ppp_pif.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


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


/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_VOID  TAF_APS_SndPppActReq(
    VOS_UINT8                           ucPdpId
);
VOS_VOID  TAF_APS_SndPppDeactInd(
    VOS_UINT8                           ucPdpId
);
VOS_VOID  TAF_APS_SndPppDeactReq(
    VOS_UINT8                           ucPdpId
);
VOS_VOID  TAF_APS_SndPppModeChangeNotify(
    VOS_UINT8                           ucPdpId
);

VOS_VOID  TAF_APS_SndPppDialModeNotify(
    VOS_UINT16                          usClientId
);

VOS_VOID TAF_APS_SndPppLinkStatusNotify(
    VOS_UINT8                           ucPdpId,
    TAF_APS_PPP_LINK_STATUS_ENUM_UINT8  enLinkStatus
);




#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TafApsSndPpp.h */
