/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : TLPsBastetInterface.h
  版 本 号   : 初稿
  作    者   : y00151394
  生成日期   : 2015年6月25日
  最近修改   :
  功能描述   : TL PS同BASTET模块接口定义
  函数列表   :
  修改历史   :
  1.日    期   : 2015年6月25日
    作    者   : yechaoling
    修改内容   : 创建文件

******************************************************************************/

#ifndef __TL_PS_BASTET_INTERFACE_H__
#define __TL_PS_BASTET_INTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif


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

/*****************************************************************************
 函数名    : LRLC_UlGetRetxRatio
 功能描述  : 获取LTE RLC层重传比例
 输入参数  : VOS_UINT32 *pulTxPduNum  LTE RLC发送PDU个数, 为累计值
             VOS_UINT32 *pulRetxNum   LTE RLC重传PDU个数, 为累计值
 输出参数  :
 返回值    :

 修改历史     :
  1.日    期  :
    作    者  :
    修改内容  : 新生成函数
*****************************************************************************/
VOS_UINT32 LRLC_UlGetRetxRatio(VOS_UINT32 *pulTxPduNum, VOS_UINT32 *pulRetxNum);

/*****************************************************************************
 函数名    : LRLC_UlGetBufferSize
 功能描述  : 获取LTE 当前缓存数据量
 输入参数  : VOS_UINT32 *pulBufferSize  LTE 当前缓存数据量

 输出参数  :
 返回值    :

 修改历史     :
  1.日    期  :
    作    者  :
    修改内容  : 新生成函数
*****************************************************************************/
VOS_UINT32 LRLC_UlGetBufferSize(VOS_UINT32 *pulBufferSize);

/*****************************************************************************
 函数名    : LRRC_GetServiceCellInfo
 功能描述  : 获取TDS RLC重传比例
 输入参数  : VOS_UINT32 *pulTxPduNum  TDS RLC发送PDU个数, 为累计值
             VOS_UINT32 *pulRetxNum   TDS RLC重传PDU个数, 为累计值
 输出参数  :
 返回值    :

 修改历史     :
  1.日    期  :
    作    者  :
    修改内容  : 新生成函数
*****************************************************************************/
VOS_UINT32 TRLC_UlGetRetxRatio(VOS_UINT32 *pulTxPduNum, VOS_UINT32 *pulRetxNum);

/*****************************************************************************
 函数名    : TRLC_UlGetBufferSize
 功能描述  : 获取TDS当前缓存数据量
 输入参数  : VOS_UINT32 *pulBufferSize  TDS当前缓存数据量

 输出参数  :
 返回值    :

 修改历史     :
  1.日    期  :
    作    者  :
    修改内容  : 新生成函数
*****************************************************************************/
VOS_UINT32 TRLC_UlGetBufferSize(VOS_UINT32 *pulBufferSize);



#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif

