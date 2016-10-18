/******************************************************************************

                 ��Ȩ���� (C), 2001-2015, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : soc_cs_tsgen_interface.h
  �� �� ��   : ����
  ��    ��   : Excel2Code
  ��������   : 2015-11-20 11:31:19
  ����޸�   :
  ��������   : �ӿ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��20��
    ��    ��   : l00352922
    �޸�����   : �ӡ�Hi3660V100 SOC�Ĵ����ֲ�_CS_TSGEN.xml���Զ�����

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_CS_TSGEN_INTERFACE_H__
#define __SOC_CS_TSGEN_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/1) TSGEN
 ****************************************************************************/
/* �Ĵ���˵����timestamp�������ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_TSGEN_TS_CNTCR_UNION */
#define SOC_CS_TSGEN_TS_CNTCR_ADDR(base)              ((base) + (0x0000))

/* �Ĵ���˵����timestamp����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TSGEN_TS_CNTSR_UNION */
#define SOC_CS_TSGEN_TS_CNTSR_ADDR(base)              ((base) + (0x0004))

/* �Ĵ���˵����timestamp��ǰ����ֵ��λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TSGEN_TS_CNTCVLW_UNION */
#define SOC_CS_TSGEN_TS_CNTCVLW_ADDR(base)            ((base) + (0x0008))

/* �Ĵ���˵����timestamp��ǰ����ֵ��λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TSGEN_TS_CNTCVUP_UNION */
#define SOC_CS_TSGEN_TS_CNTCVUP_ADDR(base)            ((base) + (0x000C))

/* �Ĵ���˵����timestamp����Ƶ�ʼĴ�����
   λ����UNION�ṹ:  SOC_CS_TSGEN_TS_CNTFID0_UNION */
#define SOC_CS_TSGEN_TS_CNTFID0_ADDR(base)            ((base) + (0x0020))

/* �Ĵ���˵����timestamp��ǰ����ֵ��λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TSGEN_TSCNTCVLW_RO_UNION */
#define SOC_CS_TSGEN_TSCNTCVLW_RO_ADDR(base)          ((base) + (0x1000))

/* �Ĵ���˵����timestamp��ǰ����ֵ��λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TSGEN_TSCNTCVUP_RO_UNION */
#define SOC_CS_TSGEN_TSCNTCVUP_RO_ADDR(base)          ((base) + (0x1004))





/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/



/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/



/*****************************************************************************
  6 STRUCT����
*****************************************************************************/



/*****************************************************************************
  7 UNION����
*****************************************************************************/

/****************************************************************************
                     (1/1) TSGEN
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_CS_TSGEN_TS_CNTCR_UNION
 �ṹ˵��  : TS_CNTCR �Ĵ����ṹ���塣��ַƫ����:0x0000����ֵ:0x00000000�����:32
 �Ĵ���˵��: timestamp�������ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  en       : 1;  /* bit[0]   : ʹ�ܿ���
                                                   0��������disabled
                                                   1��������enabled */
        unsigned int  hdbg     : 1;  /* bit[1]   : Halt on debug����
                                                   0��HLTDBG�źŶԼ�������Ӱ��
                                                   1��HLTDBG�ź���Чʱ����ͣ���� */
        unsigned int  reserved : 30; /* bit[2-31]: Reserved */
    } reg;
} SOC_CS_TSGEN_TS_CNTCR_UNION;
#endif
#define SOC_CS_TSGEN_TS_CNTCR_en_START        (0)
#define SOC_CS_TSGEN_TS_CNTCR_en_END          (0)
#define SOC_CS_TSGEN_TS_CNTCR_hdbg_START      (1)
#define SOC_CS_TSGEN_TS_CNTCR_hdbg_END        (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_TSGEN_TS_CNTSR_UNION
 �ṹ˵��  : TS_CNTSR �Ĵ����ṹ���塣��ַƫ����:0x0004����ֵ:0x00000000�����:32
 �Ĵ���˵��: timestamp����״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 1;  /* bit[0]   : Reserved */
        unsigned int  dbgh     : 1;  /* bit[1]   : Debug Halted. */
        unsigned int  reserved_1: 30; /* bit[2-31]: Reserved */
    } reg;
} SOC_CS_TSGEN_TS_CNTSR_UNION;
#endif
#define SOC_CS_TSGEN_TS_CNTSR_dbgh_START      (1)
#define SOC_CS_TSGEN_TS_CNTSR_dbgh_END        (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_TSGEN_TS_CNTCVLW_UNION
 �ṹ˵��  : TS_CNTCVLW �Ĵ����ṹ���塣��ַƫ����:0x0008����ֵ:0x00000000�����:32
 �Ĵ���˵��: timestamp��ǰ����ֵ��λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cntcvlw : 32; /* bit[0-31]: ��ǰ����ֵ��bit[31:0] */
    } reg;
} SOC_CS_TSGEN_TS_CNTCVLW_UNION;
#endif
#define SOC_CS_TSGEN_TS_CNTCVLW_cntcvlw_START  (0)
#define SOC_CS_TSGEN_TS_CNTCVLW_cntcvlw_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TSGEN_TS_CNTCVUP_UNION
 �ṹ˵��  : TS_CNTCVUP �Ĵ����ṹ���塣��ַƫ����:0x000C����ֵ:0x00000000�����:32
 �Ĵ���˵��: timestamp��ǰ����ֵ��λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cntcvup : 32; /* bit[0-31]: ��ǰ����ֵ��bit[63:32] */
    } reg;
} SOC_CS_TSGEN_TS_CNTCVUP_UNION;
#endif
#define SOC_CS_TSGEN_TS_CNTCVUP_cntcvup_START  (0)
#define SOC_CS_TSGEN_TS_CNTCVUP_cntcvup_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TSGEN_TS_CNTFID0_UNION
 �ṹ˵��  : TS_CNTFID0 �Ĵ����ṹ���塣��ַƫ����:0x0020����ֵ:0x00000000�����:32
 �Ĵ���˵��: timestamp����Ƶ�ʼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  freq : 32; /* bit[0-31]: Frequency in number of ticks per second. For example, for a 50 MHz 
                                               clock, program 0x02FAF080. */
    } reg;
} SOC_CS_TSGEN_TS_CNTFID0_UNION;
#endif
#define SOC_CS_TSGEN_TS_CNTFID0_freq_START  (0)
#define SOC_CS_TSGEN_TS_CNTFID0_freq_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TSGEN_TSCNTCVLW_RO_UNION
 �ṹ˵��  : TSCNTCVLW_RO �Ĵ����ṹ���塣��ַƫ����:0x1000����ֵ:0x00000000�����:32
 �Ĵ���˵��: timestamp��ǰ����ֵ��λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cntcvlw_ro : 32; /* bit[0-31]: ��ǰ����ֵ��bit[31:0]��ֻ������ */
    } reg;
} SOC_CS_TSGEN_TSCNTCVLW_RO_UNION;
#endif
#define SOC_CS_TSGEN_TSCNTCVLW_RO_cntcvlw_ro_START  (0)
#define SOC_CS_TSGEN_TSCNTCVLW_RO_cntcvlw_ro_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TSGEN_TSCNTCVUP_RO_UNION
 �ṹ˵��  : TSCNTCVUP_RO �Ĵ����ṹ���塣��ַƫ����:0x1004����ֵ:0x00000000�����:32
 �Ĵ���˵��: timestamp��ǰ����ֵ��λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cntcvup_ro : 32; /* bit[0-31]: ��ǰ����ֵ��bit[63:32]��ֻ������ */
    } reg;
} SOC_CS_TSGEN_TSCNTCVUP_RO_UNION;
#endif
#define SOC_CS_TSGEN_TSCNTCVUP_RO_cntcvup_ro_START  (0)
#define SOC_CS_TSGEN_TSCNTCVUP_RO_cntcvup_ro_END    (31)






/*****************************************************************************
  8 OTHERS����
*****************************************************************************/



/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_cs_tsgen_interface.h */
