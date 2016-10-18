/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NasCcMnccProc.h
  版 本 号   : 初稿
  作    者   : 丁庆 49431
  生成日期   : 2007年8月31日
  最近修改   : 2007年8月31日
  功能描述   : 接收和处理来自上层的MNCC原语
  函数列表   :
  修改历史   :
  1.日    期   : 2007年8月31日
    作    者   : 丁庆 49431
    修改内容   : 创建文件
******************************************************************************/
#ifndef  NAS_CC_MNCC_PROC_H
#define  NAS_CC_MNCC_PROC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"


/*****************************************************************************
  2 函数声明
*****************************************************************************/
/*****************************************************************************
 函 数 名  : NAS_CC_ProcMnccPrimitive
 功能描述  : 处理MNCC原语。该函数将MNCC原语分发到各自的处理函数去处理。
 输入参数  : pMsg - 来自上层的MNCC原语消息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月14日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_ProcMnccPrimitive(
    VOS_VOID                            *pMsg
);

/*****************************************************************************
 函 数 名  : NAS_CC_GetMsgUus1Info
 功能描述  : 判断该条消息是否激活UUS1信息
 输入参数  : entityId       :处理该原语的CC实体的ID
             enMsgType      :   消息类型
 输出参数  : pstUuieInfo    :   该消息的UUIE内容
 返 回 值  : VOS_OK         :   UUS1消息激活,需携带UUIE
             VOS_ERR        :   UUS1消息未激活,不携带UUIE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年7月27日
    作    者   : zhoujun /40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_GetMsgUus1Info(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_MSG_TYPE_ENUM_U8             enMsgType,
    NAS_CC_IE_USER_USER_STRU            *pstUuieInfo
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

#endif /* NAS_CC_MNCC_PROC_H */

