/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : Ps_Math.h
  版 本 号   : 初稿
  作    者   : 韩磊
  生成日期   : 2008年9月23日
  最近修改   :
  功能描述   : 数学库头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2008年9月23日
    作    者   : 韩磊
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PS_MATH_H__
#define __PS_MATH_H__




/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include <math.h>                                                               /*_H2ASN_Skip*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define PS_FLOAT_PRECISION              ( 0.001 )                               /* 协议栈使用的浮点数精度,用于浮点数比较时参考 */

#if(VOS_VXWORKS == VOS_OS_VER)
#define PS_NULL_FLOAT                   ( (PS_FLOAT)(HUGE_VAL) )                /* 浮点数无效值, 用于浮点数初始化时使用 */
#else
#define PS_NULL_FLOAT                   ( (PS_FLOAT)(0XFFFFFFFF) )
#endif

typedef float   PS_FLOAT;
typedef double  PS_DOUBLE;

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
#define PS_FloatAdd( f1, f2 )           ( (f1) + (f2) )
#define PS_FloatSub( f1, f2 )           ( (f1) - (f2) )
#define PS_FloatMul( f1, f2 )           ( (f1) * (f2) )
#define PS_FloatDiv( f1, f2 )           ( (f1) / (f2) )
#define PS_FloatAbs( f )                ( fabs(f) )
#define PS_FloatPow( f1, f2 )           ( pow(f1, f2) )
#define PS_FloatSqrt( f )               ( sqrt(f) )
#define PS_FloatExp( f )                ( exp(f) )
#define PS_FloatLog( f )                ( log(f) )
#define PS_FloatLog10( f )              ( log10(f) )
#define PS_FloatCeil( f )               ( ceil(f) )
#define PS_FloatFloor( f )              ( floor(f) )

#define PS_FloatGreat( f1, f2 )         ( ((f1)>((f2)+(PS_FLOAT_PRECISION))) ? PS_TRUE : PS_FALSE )
#define PS_FloatLess( f1, f2 )          ( ((f1)<((f2)-(PS_FLOAT_PRECISION))) ? PS_TRUE : PS_FALSE )
#define PS_FloatEqual( f1, f2 )         ( (fabs((f1)-(f2))<=(PS_FLOAT_PRECISION)) ? PS_TRUE : PS_FALSE )
#define PS_FloatNotLess(  f1, f2  )     ( PS_FloatLess((f1), (f2) ) ?  PS_FALSE :  PS_TRUE )
#define PS_FloatNotGreat(  f1, f2  )    ( PS_FloatGreat((f1), (f2) )?  PS_FALSE :  PS_TRUE )

#define PS_Int2Float( n )               ( (float)(n) )


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

#endif /* end of Ps_Math.h */
