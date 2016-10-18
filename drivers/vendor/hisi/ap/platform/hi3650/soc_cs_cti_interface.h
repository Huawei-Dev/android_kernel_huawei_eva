/******************************************************************************

                 ��Ȩ���� (C), 2001-2015, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : soc_cs_cti_interface.h
  �� �� ��   : ����
  ��    ��   : Excel2Code
  ��������   : 2015-11-20 11:31:00
  ����޸�   :
  ��������   : �ӿ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��20��
    ��    ��   : l00352922
    �޸�����   : �ӡ�Hi3660V100 SOC�Ĵ����ֲ�_CS_CTI.xml���Զ�����

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_CS_CTI_INTERFACE_H__
#define __SOC_CS_CTI_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/1) CTI
 ****************************************************************************/
/* �Ĵ���˵����CTI���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_CONTROL_UNION */
#define SOC_CS_CTI_CTI_CONTROL_ADDR(base)             ((base) + (0x000))

/* �Ĵ���˵����CTI�ж�ȷ�ϼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INTACK_UNION */
#define SOC_CS_CTI_CTI_INTACK_ADDR(base)              ((base) + (0x010))

/* �Ĵ���˵����CTI trigger set�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_APPSET_UNION */
#define SOC_CS_CTI_CTI_APPSET_ADDR(base)              ((base) + (0x014))

/* �Ĵ���˵����CTI trigger clear�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_APPCLEAR_UNION */
#define SOC_CS_CTI_CTI_APPCLEAR_ADDR(base)            ((base) + (0x018))

/* �Ĵ���˵����CTI trigger pulse�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_APPPULSE_UNION */
#define SOC_CS_CTI_CTI_APPPULSE_ADDR(base)            ((base) + (0x01C))

/* �Ĵ���˵����CTI trigger0��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN0_UNION */
#define SOC_CS_CTI_CTI_INEN0_ADDR(base)               ((base) + (0x020))

/* �Ĵ���˵����CTI trigger1��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN1_UNION */
#define SOC_CS_CTI_CTI_INEN1_ADDR(base)               ((base) + (0x024))

/* �Ĵ���˵����CTI trigger2��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN2_UNION */
#define SOC_CS_CTI_CTI_INEN2_ADDR(base)               ((base) + (0x028))

/* �Ĵ���˵����CTI trigger3��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN3_UNION */
#define SOC_CS_CTI_CTI_INEN3_ADDR(base)               ((base) + (0x02C))

/* �Ĵ���˵����CTI trigger4��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN4_UNION */
#define SOC_CS_CTI_CTI_INEN4_ADDR(base)               ((base) + (0x030))

/* �Ĵ���˵����CTI trigger5��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN5_UNION */
#define SOC_CS_CTI_CTI_INEN5_ADDR(base)               ((base) + (0x034))

/* �Ĵ���˵����CTI trigger6��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN6_UNION */
#define SOC_CS_CTI_CTI_INEN6_ADDR(base)               ((base) + (0x038))

/* �Ĵ���˵����CTI trigger7��channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_INEN7_UNION */
#define SOC_CS_CTI_CTI_INEN7_ADDR(base)               ((base) + (0x03C))

/* �Ĵ���˵����CTI channel��trigger0ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN0_UNION */
#define SOC_CS_CTI_CTI_OUTEN0_ADDR(base)              ((base) + (0x0A0))

/* �Ĵ���˵����CTI channel��trigger1ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN1_UNION */
#define SOC_CS_CTI_CTI_OUTEN1_ADDR(base)              ((base) + (0x0A4))

/* �Ĵ���˵����CTI channel��trigger2ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN2_UNION */
#define SOC_CS_CTI_CTI_OUTEN2_ADDR(base)              ((base) + (0x0A8))

/* �Ĵ���˵����CTI channel��trigger3ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN3_UNION */
#define SOC_CS_CTI_CTI_OUTEN3_ADDR(base)              ((base) + (0x0AC))

/* �Ĵ���˵����CTI channel��trigger4ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN4_UNION */
#define SOC_CS_CTI_CTI_OUTEN4_ADDR(base)              ((base) + (0x0B0))

/* �Ĵ���˵����CTI channel��trigger5ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN5_UNION */
#define SOC_CS_CTI_CTI_OUTEN5_ADDR(base)              ((base) + (0x0B4))

/* �Ĵ���˵����CTI channel��trigger6ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN6_UNION */
#define SOC_CS_CTI_CTI_OUTEN6_ADDR(base)              ((base) + (0x0B8))

/* �Ĵ���˵����CTI channel��trigger7ʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_OUTEN7_UNION */
#define SOC_CS_CTI_CTI_OUTEN7_ADDR(base)              ((base) + (0x0BC))

/* �Ĵ���˵����CTI trigger����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_TRIGINSTATUS_UNION */
#define SOC_CS_CTI_CTI_TRIGINSTATUS_ADDR(base)        ((base) + (0x130))

/* �Ĵ���˵����CTI trigger���״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_TRIGOUTSTATUS_UNION */
#define SOC_CS_CTI_CTI_TRIGOUTSTATUS_ADDR(base)       ((base) + (0x134))

/* �Ĵ���˵����CTI channel����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_CHINSTATUS_UNION */
#define SOC_CS_CTI_CTI_CHINSTATUS_ADDR(base)          ((base) + (0x138))

/* �Ĵ���˵����CTI channel���״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_CHOUTSTATUS_UNION */
#define SOC_CS_CTI_CTI_CHOUTSTATUS_ADDR(base)         ((base) + (0x13C))

/* �Ĵ���˵����CTI channelʹ�ܼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_GATE_UNION */
#define SOC_CS_CTI_CTI_GATE_ADDR(base)                ((base) + (0x140))

/* �Ĵ���˵����CTI�������ʼĴ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_LAR_UNION */
#define SOC_CS_CTI_CTI_LAR_ADDR(base)                 ((base) + (0xFB0))

/* �Ĵ���˵����CTI����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_LSR_UNION */
#define SOC_CS_CTI_CTI_LSR_ADDR(base)                 ((base) + (0xFB4))

/* �Ĵ���˵����CTI��Ȩ״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_CTI_CTI_AUTHSTATUS_UNION */
#define SOC_CS_CTI_CTI_AUTHSTATUS_ADDR(base)          ((base) + (0xFB8))





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
                     (1/1) CTI
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_CONTROL_UNION
 �ṹ˵��  : CTI_CONTROL �Ĵ����ṹ���塣��ַƫ����:0x000����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  glben    : 1;  /* bit[0]   : Enables or disables the CTI.
                                                   0��disabled
                                                   1��enabled */
        unsigned int  reserved : 31; /* bit[1-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_CONTROL_UNION;
#endif
#define SOC_CS_CTI_CTI_CONTROL_glben_START     (0)
#define SOC_CS_CTI_CTI_CONTROL_glben_END       (0)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INTACK_UNION
 �ṹ˵��  : CTI_INTACK �Ĵ����ṹ���塣��ַƫ����:0x010����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI�ж�ȷ�ϼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  intack   : 8;  /* bit[0-7] : Acknowledges the corresponding ctitrigout output. There is one bit of the register for each ctitrigout 
                                                   output. When a 1 is written to a bit in this register, the corresponding ctitrigout is acknowledged, causing 
                                                   it to be cleared. */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INTACK_UNION;
#endif
#define SOC_CS_CTI_CTI_INTACK_intack_START    (0)
#define SOC_CS_CTI_CTI_INTACK_intack_END      (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_APPSET_UNION
 �ṹ˵��  : CTI_APPSET �Ĵ����ṹ���塣��ַƫ����:0x014����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger set�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  appset   : 4;  /* bit[0-3] : Setting a bit HIGH generates a channel event for the selected channel. There is one bit of the 
                                                   register for each channel. 
                                                   Reads as follows: 
                                                   0 Application trigger is inactive.
                                                   1 Application trigger is active.
                                                   Writes as follows: 
                                                   0 No effect.
                                                   1 Generate channel event. */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_APPSET_UNION;
#endif
#define SOC_CS_CTI_CTI_APPSET_appset_START    (0)
#define SOC_CS_CTI_CTI_APPSET_appset_END      (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_APPCLEAR_UNION
 �ṹ˵��  : CTI_APPCLEAR �Ĵ����ṹ���塣��ַƫ����:0x018����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger clear�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  appclear : 4;  /* bit[0-3] : Sets the corresponding bits in the CTIAPPSET to 0. There is one bit of the register for each 
                                                   channel. On writes, for each bit: 
                                                   0 Has no effect.
                                                   1 Clears the corresponding channel event. */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_APPCLEAR_UNION;
#endif
#define SOC_CS_CTI_CTI_APPCLEAR_appclear_START  (0)
#define SOC_CS_CTI_CTI_APPCLEAR_appclear_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_APPPULSE_UNION
 �ṹ˵��  : CTI_APPPULSE �Ĵ����ṹ���塣��ַƫ����:0x01C����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger pulse�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  appulse  : 4;  /* bit[0-3] : Setting a bit HIGH generates a channel event pulse for the selected channel. There is one bit of the register for each channel. On writes, for each bit:
                                                   0 Has no effect.
                                                   1 Generate an event pulse on the corresponding channel. */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_APPPULSE_UNION;
#endif
#define SOC_CS_CTI_CTI_APPPULSE_appulse_START   (0)
#define SOC_CS_CTI_CTI_APPPULSE_appulse_END     (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN0_UNION
 �ṹ˵��  : CTI_INEN0 �Ĵ����ṹ���塣��ַƫ����:0x020����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger0��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin0�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[0]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[0]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN0_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN0_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN0_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN1_UNION
 �ṹ˵��  : CTI_INEN1 �Ĵ����ṹ���塣��ַƫ����:0x024����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger1��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin1�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[1]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[1]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN1_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN1_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN1_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN2_UNION
 �ṹ˵��  : CTI_INEN2 �Ĵ����ṹ���塣��ַƫ����:0x028����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger2��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin2�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[2]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[2]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN2_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN2_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN2_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN3_UNION
 �ṹ˵��  : CTI_INEN3 �Ĵ����ṹ���塣��ַƫ����:0x02C����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger3��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin3�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[3]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[3]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN3_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN3_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN3_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN4_UNION
 �ṹ˵��  : CTI_INEN4 �Ĵ����ṹ���塣��ַƫ����:0x030����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger4��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin4�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[4]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[4]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN4_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN4_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN4_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN5_UNION
 �ṹ˵��  : CTI_INEN5 �Ĵ����ṹ���塣��ַƫ����:0x034����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger5��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin5�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[5]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[5]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN5_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN5_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN5_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN6_UNION
 �ṹ˵��  : CTI_INEN6 �Ĵ����ṹ���塣��ַƫ����:0x038����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger6��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin6�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[6]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[6]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN6_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN6_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN6_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_INEN7_UNION
 �ṹ˵��  : CTI_INEN7 �Ĵ����ṹ���塣��ַƫ����:0x03C����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger7��channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginen : 4;  /* bit[0-3] : ʹ��triggerin7�¼�����Ӧchannel��ӳ��
                                                   0��disable CTITRIGIN[7]����Ӧchannel��ӳ��
                                                   1��enable CTITRIGIN[7]����Ӧchannel��ӳ�� */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_INEN7_UNION;
#endif
#define SOC_CS_CTI_CTI_INEN7_triginen_START  (0)
#define SOC_CS_CTI_CTI_INEN7_triginen_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN0_UNION
 �ṹ˵��  : CTI_OUTEN0 �Ĵ����ṹ���塣��ַƫ����:0x0A0����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger0ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout0�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[0]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[0]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN0_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN0_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN0_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN1_UNION
 �ṹ˵��  : CTI_OUTEN1 �Ĵ����ṹ���塣��ַƫ����:0x0A4����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger1ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout1�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[1]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[1]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN1_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN1_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN1_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN2_UNION
 �ṹ˵��  : CTI_OUTEN2 �Ĵ����ṹ���塣��ַƫ����:0x0A8����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger2ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout2�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[2]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[2]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN2_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN2_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN2_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN3_UNION
 �ṹ˵��  : CTI_OUTEN3 �Ĵ����ṹ���塣��ַƫ����:0x0AC����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger3ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout3�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[3]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[3]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN3_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN3_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN3_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN4_UNION
 �ṹ˵��  : CTI_OUTEN4 �Ĵ����ṹ���塣��ַƫ����:0x0B0����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger4ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout4�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[4]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[4]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN4_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN4_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN4_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN5_UNION
 �ṹ˵��  : CTI_OUTEN5 �Ĵ����ṹ���塣��ַƫ����:0x0B4����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger5ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout5�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[5]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[5]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN5_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN5_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN5_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN6_UNION
 �ṹ˵��  : CTI_OUTEN6 �Ĵ����ṹ���塣��ַƫ����:0x0B8����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger6ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout6�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[6]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[6]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN6_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN6_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN6_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_OUTEN7_UNION
 �ṹ˵��  : CTI_OUTEN7 �Ĵ����ṹ���塣��ַƫ����:0x0BC����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel��trigger7ʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigouten : 4;  /* bit[0-3] : ʹ����Ӧchannel��triggerout7�¼���ӳ��
                                                    0��disable ��Ӧchannel��CTITRIGOUT[7]��ӳ��
                                                    1��enable ��Ӧchannel��CTITRIGOUT[7]��ӳ�� */
        unsigned int  reserved  : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_OUTEN7_UNION;
#endif
#define SOC_CS_CTI_CTI_OUTEN7_trigouten_START  (0)
#define SOC_CS_CTI_CTI_OUTEN7_trigouten_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_TRIGINSTATUS_UNION
 �ṹ˵��  : CTI_TRIGINSTATUS �Ĵ����ṹ���塣��ַƫ����:0x130����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger����״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  triginstatus : 8;  /* bit[0-7] : Shows the status of the ctitrigin inputs. There is one bit of the field for each trigger input.
                                                       1 ctitrigin is active.
                                                       0 ctitrigin is inactive. */
        unsigned int  reserved     : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_TRIGINSTATUS_UNION;
#endif
#define SOC_CS_CTI_CTI_TRIGINSTATUS_triginstatus_START  (0)
#define SOC_CS_CTI_CTI_TRIGINSTATUS_triginstatus_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_TRIGOUTSTATUS_UNION
 �ṹ˵��  : CTI_TRIGOUTSTATUS �Ĵ����ṹ���塣��ַƫ����:0x134����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI trigger���״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigoutstatus : 8;  /* bit[0-7] : Shows the status of the ctitrigout outputs. There is one bit of the field for each trigger output.
                                                        1 ctitrigout is active.
                                                        0 ctitrigout is inactive */
        unsigned int  reserved      : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_TRIGOUTSTATUS_UNION;
#endif
#define SOC_CS_CTI_CTI_TRIGOUTSTATUS_trigoutstatus_START  (0)
#define SOC_CS_CTI_CTI_TRIGOUTSTATUS_trigoutstatus_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_CHINSTATUS_UNION
 �ṹ˵��  : CTI_CHINSTATUS �Ĵ����ṹ���塣��ַƫ����:0x138����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel����״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  chinstatus : 4;  /* bit[0-3] : Shows the status of the ctichin inputs. There is one bit of the field for each channel input.
                                                     0 ctichin is inactive.
                                                     1 ctichin is active. */
        unsigned int  reserved   : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_CHINSTATUS_UNION;
#endif
#define SOC_CS_CTI_CTI_CHINSTATUS_chinstatus_START  (0)
#define SOC_CS_CTI_CTI_CHINSTATUS_chinstatus_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_CHOUTSTATUS_UNION
 �ṹ˵��  : CTI_CHOUTSTATUS �Ĵ����ṹ���塣��ַƫ����:0x13C����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI channel���״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  choutstatus : 4;  /* bit[0-3] : Shows the status of the ctichout outputs. There is one bit of the field for each channel output.
                                                      0 ctichout is inactive.
                                                      1 ctichout is active. */
        unsigned int  reserved    : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_CHOUTSTATUS_UNION;
#endif
#define SOC_CS_CTI_CTI_CHOUTSTATUS_choutstatus_START  (0)
#define SOC_CS_CTI_CTI_CHOUTSTATUS_choutstatus_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_GATE_UNION
 �ṹ˵��  : CTI_GATE �Ĵ����ṹ���塣��ַƫ����:0x140����ֵ:0x0000000F�����:32
 �Ĵ���˵��: CTI channelʹ�ܼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ctigateen0 : 1;  /* bit[0]   : ʹ��ͨ��0
                                                     0��disable CTICHOUT0
                                                     1��enable CTICHOUT0 */
        unsigned int  ctigateen1 : 1;  /* bit[1]   : ʹ��ͨ��1
                                                     0��disable CTICHOUT1
                                                     1��enable CTICHOUT1 */
        unsigned int  ctigateen2 : 1;  /* bit[2]   : ʹ��ͨ��2
                                                     0��disable CTICHOUT2
                                                     1��enable CTICHOUT2 */
        unsigned int  ctigateen3 : 1;  /* bit[3]   : ʹ��ͨ��3
                                                     0��disable CTICHOUT3
                                                     1��enable CTICHOUT3 */
        unsigned int  reserved   : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_GATE_UNION;
#endif
#define SOC_CS_CTI_CTI_GATE_ctigateen0_START  (0)
#define SOC_CS_CTI_CTI_GATE_ctigateen0_END    (0)
#define SOC_CS_CTI_CTI_GATE_ctigateen1_START  (1)
#define SOC_CS_CTI_CTI_GATE_ctigateen1_END    (1)
#define SOC_CS_CTI_CTI_GATE_ctigateen2_START  (2)
#define SOC_CS_CTI_CTI_GATE_ctigateen2_END    (2)
#define SOC_CS_CTI_CTI_GATE_ctigateen3_START  (3)
#define SOC_CS_CTI_CTI_GATE_ctigateen3_END    (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_LAR_UNION
 �ṹ˵��  : CTI_LAR �Ĵ����ṹ���塣��ַƫ����:0xFB0����ֵ:0x00000000�����:32
 �Ĵ���˵��: CTI�������ʼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  key : 32; /* bit[0-31]: д��0xC5ACCE55���н�����Ȼ��ſ����������������Ĵ�����д�������κ�ֵ���ٴ������� */
    } reg;
} SOC_CS_CTI_CTI_LAR_UNION;
#endif
#define SOC_CS_CTI_CTI_LAR_key_START  (0)
#define SOC_CS_CTI_CTI_LAR_key_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_LSR_UNION
 �ṹ˵��  : CTI_LSR �Ĵ����ṹ���塣��ַƫ����:0xFB4����ֵ:0x00000003�����:32
 �Ĵ���˵��: CTI����״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sli      : 1;  /* bit[0]   : ָʾ�ڼ��Ƿ�֧���������ƣ�
                                                   0�������֧����������
                                                   1�����֧���������� */
        unsigned int  slk      : 1;  /* bit[1]   : ����ĵ�ǰ����״̬��
                                                   0���ѽ���������д����
                                                   1����������������д�������ɶ� */
        unsigned int  ntt      : 1;  /* bit[2]   : ָʾ�������ʼĴ���Ϊ32-bit */
        unsigned int  reserved : 29; /* bit[3-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_LSR_UNION;
#endif
#define SOC_CS_CTI_CTI_LSR_sli_START       (0)
#define SOC_CS_CTI_CTI_LSR_sli_END         (0)
#define SOC_CS_CTI_CTI_LSR_slk_START       (1)
#define SOC_CS_CTI_CTI_LSR_slk_END         (1)
#define SOC_CS_CTI_CTI_LSR_ntt_START       (2)
#define SOC_CS_CTI_CTI_LSR_ntt_END         (2)


/*****************************************************************************
 �ṹ��    : SOC_CS_CTI_CTI_AUTHSTATUS_UNION
 �ṹ˵��  : CTI_AUTHSTATUS �Ĵ����ṹ���塣��ַƫ����:0xFB8����ֵ:0x0000000A�����:32
 �Ĵ���˵��: CTI��Ȩ״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nsid     : 2;  /* bit[0-1] : Indicates the security level for non-secure invasive debug:
                                                   0b10 Disabled.
                                                   0b11 Enabled. */
        unsigned int  nsnid    : 2;  /* bit[2-3] : Indicates the security level for non-secure non-invasive debug:
                                                   0b10 Disabled.
                                                   0b11 Enabled. */
        unsigned int  sid      : 2;  /* bit[4-5] : Always 0b00 Secure invasive debug is not implemented */
        unsigned int  snid     : 2;  /* bit[6-7] : Always 0b00. The security level for secure non-invasive debug is not implemented */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_CTI_CTI_AUTHSTATUS_UNION;
#endif
#define SOC_CS_CTI_CTI_AUTHSTATUS_nsid_START      (0)
#define SOC_CS_CTI_CTI_AUTHSTATUS_nsid_END        (1)
#define SOC_CS_CTI_CTI_AUTHSTATUS_nsnid_START     (2)
#define SOC_CS_CTI_CTI_AUTHSTATUS_nsnid_END       (3)
#define SOC_CS_CTI_CTI_AUTHSTATUS_sid_START       (4)
#define SOC_CS_CTI_CTI_AUTHSTATUS_sid_END         (5)
#define SOC_CS_CTI_CTI_AUTHSTATUS_snid_START      (6)
#define SOC_CS_CTI_CTI_AUTHSTATUS_snid_END        (7)






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

#endif /* end of soc_cs_cti_interface.h */
