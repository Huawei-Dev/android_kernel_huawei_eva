/******************************************************************************

                 ��Ȩ���� (C), 2001-2015, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : soc_cs_top_funnel_interface.h
  �� �� ��   : ����
  ��    ��   : Excel2Code
  ��������   : 2015-11-20 11:31:13
  ����޸�   :
  ��������   : �ӿ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��20��
    ��    ��   : l00352922
    �޸�����   : �ӡ�Hi3660V100 SOC�Ĵ����ֲ�_CS_TOP_FUNNEL.xml���Զ�����

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_CS_TOP_FUNNEL_INTERFACE_H__
#define __SOC_CS_TOP_FUNNEL_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/1) TOP_FUNNEL
 ****************************************************************************/
/* �Ĵ���˵����Funnel���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_UNION */
#define SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_ADDR(base)  ((base) + (0x000))

/* �Ĵ���˵����Funnel���ȼ����ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_UNION */
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_ADDR(base) ((base) + (0x004))

/* �Ĵ���˵����Funnel�������ʼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_FUNNEL_FUNNEL_LAR_UNION */
#define SOC_CS_TOP_FUNNEL_FUNNEL_LAR_ADDR(base)       ((base) + (0xFB0))

/* �Ĵ���˵����Funnel����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_FUNNEL_FUNNEL_LSR_UNION */
#define SOC_CS_TOP_FUNNEL_FUNNEL_LSR_ADDR(base)       ((base) + (0xFB4))





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
                     (1/1) TOP_FUNNEL
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_UNION
 �ṹ˵��  : FUNNEL_CTRL_REG �Ĵ����ṹ���塣��ַƫ����:0x000����ֵ:0x00000300�����:32
 �Ĵ���˵��: Funnel���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enable   : 7;  /* bit[0-6]  : bit[0]~bit[6]�ֱ��Ӧport0~port6��
                                                    0��slave port disabled
                                                    1��slave port enabled
                                                    port��Ӧ��ϵ���£������˿�����ʹ�ܡ�
                                                    port0:AP
                                                    port1:reserved
                                                    port2:reserved
                                                    port3:modem_a9
                                                    port4:stm-500
                                                    port5:isp_a7
                                                    port6:iomcu_m7 */
        unsigned int  reserved_0: 1;  /* bit[7]    : Reserved */
        unsigned int  holdtime : 4;  /* bit[8-11] : Funnel ATB slave�˿�һ�����ٽ��յ�transaction��������hold time��Ϊ�˽��Ͷ˿�Ƶ���л���trace���ܴ�����Ӱ�졣���ٽ��յ�transaction��Ϊhold timeֵ��1��ȡֵ��Χb0000~b1110��b1111������ */
        unsigned int  reserved_1: 20; /* bit[12-31]: Reserved */
    } reg;
} SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_UNION;
#endif
#define SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_enable_START    (0)
#define SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_enable_END      (6)
#define SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_holdtime_START  (8)
#define SOC_CS_TOP_FUNNEL_FUNNEL_CTRL_REG_holdtime_END    (11)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_UNION
 �ṹ˵��  : FUNNEL_PRIORITY_CTRL_REG �Ĵ����ṹ���塣��ַƫ����:0x004����ֵ:0x00000000�����:32
 �Ĵ���˵��: Funnel���ȼ����ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  priport0 : 3;  /* bit[0-2]  : �˿�0���ȼ����� */
        unsigned int  priport1 : 3;  /* bit[3-5]  : �˿�1���ȼ����� */
        unsigned int  priport2 : 3;  /* bit[6-8]  : �˿�2���ȼ����� */
        unsigned int  priport3 : 3;  /* bit[9-11] : �˿�3���ȼ����� */
        unsigned int  priport4 : 3;  /* bit[12-14]: �˿�4���ȼ����� */
        unsigned int  priport5 : 3;  /* bit[15-17]: �˿�5���ȼ����� */
        unsigned int  priport6 : 3;  /* bit[18-20]: �˿�6���ȼ����ã���ֵԽ�ͣ����ȼ�Խ�ߣ�������ֵ��ͬʱ�����С�Ķ˿����ȼ��ϸߡ����ܵ�ȡֵ���£�
                                                    b000�����ȼ�0
                                                    b001�����ȼ�1
                                                    b010�����ȼ�2
                                                    b011�����ȼ�3
                                                    b100�����ȼ�4
                                                    b101�����ȼ�5
                                                    b110�����ȼ�6
                                                    b111�����ȼ�7 */
        unsigned int  reserved : 11; /* bit[21-31]: Reserved */
    } reg;
} SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_UNION;
#endif
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport0_START  (0)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport0_END    (2)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport1_START  (3)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport1_END    (5)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport2_START  (6)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport2_END    (8)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport3_START  (9)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport3_END    (11)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport4_START  (12)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport4_END    (14)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport5_START  (15)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport5_END    (17)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport6_START  (18)
#define SOC_CS_TOP_FUNNEL_FUNNEL_PRIORITY_CTRL_REG_priport6_END    (20)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_FUNNEL_FUNNEL_LAR_UNION
 �ṹ˵��  : FUNNEL_LAR �Ĵ����ṹ���塣��ַƫ����:0xFB0����ֵ:0x00000000�����:32
 �Ĵ���˵��: Funnel�������ʼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  key : 32; /* bit[0-31]: д��0xC5ACCE55���н�����Ȼ��ſ����������������Ĵ�����д�������κ�ֵ���ٴ������� */
    } reg;
} SOC_CS_TOP_FUNNEL_FUNNEL_LAR_UNION;
#endif
#define SOC_CS_TOP_FUNNEL_FUNNEL_LAR_key_START  (0)
#define SOC_CS_TOP_FUNNEL_FUNNEL_LAR_key_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_FUNNEL_FUNNEL_LSR_UNION
 �ṹ˵��  : FUNNEL_LSR �Ĵ����ṹ���塣��ַƫ����:0xFB4����ֵ:0x00000003�����:32
 �Ĵ���˵��: Funnel����״̬�Ĵ�����
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
} SOC_CS_TOP_FUNNEL_FUNNEL_LSR_UNION;
#endif
#define SOC_CS_TOP_FUNNEL_FUNNEL_LSR_sli_START       (0)
#define SOC_CS_TOP_FUNNEL_FUNNEL_LSR_sli_END         (0)
#define SOC_CS_TOP_FUNNEL_FUNNEL_LSR_slk_START       (1)
#define SOC_CS_TOP_FUNNEL_FUNNEL_LSR_slk_END         (1)
#define SOC_CS_TOP_FUNNEL_FUNNEL_LSR_ntt_START       (2)
#define SOC_CS_TOP_FUNNEL_FUNNEL_LSR_ntt_END         (2)






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

#endif /* end of soc_cs_top_funnel_interface.h */
