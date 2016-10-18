/******************************************************************************

                 ��Ȩ���� (C), 2001-2015, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : soc_cs_tpiu_interface.h
  �� �� ��   : ����
  ��    ��   : Excel2Code
  ��������   : 2015-11-20 11:31:16
  ����޸�   :
  ��������   : �ӿ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��20��
    ��    ��   : l00352922
    �޸�����   : �ӡ�Hi3660V100 SOC�Ĵ����ֲ�_CS_TPIU.xml���Զ�����

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_CS_TPIU_INTERFACE_H__
#define __SOC_CS_TPIU_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/1) TPIU
 ****************************************************************************/
/* �Ĵ���˵����TPIU �ӿ�λ��ָʾ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_SUPPORTED_PORT_SIZES_UNION */
#define SOC_CS_TPIU_TPIU_SUPPORTED_PORT_SIZES_ADDR(base) ((base) + (0x000))

/* �Ĵ���˵����TPIU ��ǰ�ӿ�λ��ָʾ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_CURRENT_PORT_SIZES_UNION */
#define SOC_CS_TPIU_TPIU_CURRENT_PORT_SIZES_ADDR(base) ((base) + (0x004))

/* �Ĵ���˵����TPIU triggerģʽָʾ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_UNION */
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_ADDR(base) ((base) + (0x100))

/* �Ĵ���˵����TPIU trigger������ֵ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_TRIGGER_COUNTER_VALUE_UNION */
#define SOC_CS_TPIU_TPIU_TRIGGER_COUNTER_VALUE_ADDR(base) ((base) + (0x104))

/* �Ĵ���˵����TPIU trigger�������üĴ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_UNION */
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_ADDR(base) ((base) + (0x108))

/* �Ĵ���˵����TPIU test patternģʽָʾ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_UNION */
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_ADDR(base) ((base) + (0x200))

/* �Ĵ���˵����TPIU test patternģʽ���üĴ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_UNION */
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_ADDR(base) ((base) + (0x204))

/* �Ĵ���˵����TPIU test pattern�������Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_TPRCR_UNION */
#define SOC_CS_TPIU_TPIU_TPRCR_ADDR(base)             ((base) + (0x208))

/* �Ĵ���˵����TPIU formatter and flush״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_FFSR_UNION */
#define SOC_CS_TPIU_TPIU_FFSR_ADDR(base)              ((base) + (0x300))

/* �Ĵ���˵����TPIU formatter and flush���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_FFCR_UNION */
#define SOC_CS_TPIU_TPIU_FFCR_ADDR(base)              ((base) + (0x304))

/* �Ĵ���˵����TPIU Formatterͬ���������Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_FSCR_UNION */
#define SOC_CS_TPIU_TPIU_FSCR_ADDR(base)              ((base) + (0x308))

/* �Ĵ���˵����TPIU EXCTL In Port�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_EXTCTL_IN_PORT_UNION */
#define SOC_CS_TPIU_TPIU_EXTCTL_IN_PORT_ADDR(base)    ((base) + (0x400))

/* �Ĵ���˵����TPIU EXCTL Out Port�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_EXTCTL_OUT_PORT_UNION */
#define SOC_CS_TPIU_TPIU_EXTCTL_OUT_PORT_ADDR(base)   ((base) + (0x404))

/* �Ĵ���˵����TPIU�������ʼĴ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_LAR_UNION */
#define SOC_CS_TPIU_TPIU_LAR_ADDR(base)               ((base) + (0xFB0))

/* �Ĵ���˵����TPIU����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TPIU_TPIU_LSR_UNION */
#define SOC_CS_TPIU_TPIU_LSR_ADDR(base)               ((base) + (0xFB4))





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
                     (1/1) TPIU
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_SUPPORTED_PORT_SIZES_UNION
 �ṹ˵��  : TPIU_SUPPORTED_PORT_SIZES �Ĵ����ṹ���塣��ַƫ����:0x000����ֵ:0x0000FFFF�����:32
 �Ĵ���˵��: TPIU �ӿ�λ��ָʾ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  port_size_s : 16; /* bit[0-15] : Indicates the TPIU supports port size of 1-bit~16-bit. */
        unsigned int  reserved    : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_SUPPORTED_PORT_SIZES_UNION;
#endif
#define SOC_CS_TPIU_TPIU_SUPPORTED_PORT_SIZES_port_size_s_START  (0)
#define SOC_CS_TPIU_TPIU_SUPPORTED_PORT_SIZES_port_size_s_END    (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_CURRENT_PORT_SIZES_UNION
 �ṹ˵��  : TPIU_CURRENT_PORT_SIZES �Ĵ����ṹ���塣��ַƫ����:0x004����ֵ:0x00000001�����:32
 �Ĵ���˵��: TPIU ��ǰ�ӿ�λ��ָʾ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  port_size_c : 16; /* bit[0-15] : ���õ�ǰ��Ҫ��port size����n bit����Ϊ1��ʾport sizeΪn+1.���磬bit[15]����Ϊ1����port size����Ϊ16. */
        unsigned int  reserved    : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_CURRENT_PORT_SIZES_UNION;
#endif
#define SOC_CS_TPIU_TPIU_CURRENT_PORT_SIZES_port_size_c_START  (0)
#define SOC_CS_TPIU_TPIU_CURRENT_PORT_SIZES_port_size_c_END    (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_UNION
 �ṹ˵��  : TPIU_SUPPORTED_TRIGGER_MODES �Ĵ����ṹ���塣��ַƫ����:0x100����ֵ:0x0000011F�����:32
 �Ĵ���˵��: TPIU triggerģʽָʾ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mult2     : 1;  /* bit[0]    : Indicates multiplying the trigger counter by 2 is supported. */
        unsigned int  mult4     : 1;  /* bit[1]    : Indicates multiplying the trigger counter by 4 is supported. */
        unsigned int  mult16    : 1;  /* bit[2]    : Indicates multiplying the trigger counter by 16 is supported. */
        unsigned int  mult256   : 1;  /* bit[3]    : Indicates multiplying the trigger counter by 256 is supported. */
        unsigned int  mult64k   : 1;  /* bit[4]    : Indicates multiplying the trigger counter by 65536 is supported. */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : Reserved */
        unsigned int  tcount8   : 1;  /* bit[8]    : Indicates an 8-bit wide counter register is implemented. */
        unsigned int  reserved_1: 7;  /* bit[9-15] : Reserved */
        unsigned int  triggered : 1;  /* bit[16]   : A trigger has occurred and the counter has reached 0.
                                                     0 Trigger has not occurred.
                                                     1 Trigger has occurred */
        unsigned int  trgrun    : 1;  /* bit[17]   : A trigger has occurred but the counter is not at 0.
                                                     0 Either a trigger has not occurred or the counter is at 0.
                                                     1 A trigger has occurred but the counter is not at 0. */
        unsigned int  reserved_2: 14; /* bit[18-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_UNION;
#endif
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult2_START      (0)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult2_END        (0)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult4_START      (1)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult4_END        (1)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult16_START     (2)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult16_END       (2)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult256_START    (3)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult256_END      (3)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult64k_START    (4)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_mult64k_END      (4)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_tcount8_START    (8)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_tcount8_END      (8)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_triggered_START  (16)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_triggered_END    (16)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_trgrun_START     (17)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TRIGGER_MODES_trgrun_END       (17)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_TRIGGER_COUNTER_VALUE_UNION
 �ṹ˵��  : TPIU_TRIGGER_COUNTER_VALUE �Ĵ����ṹ���塣��ַƫ����:0x104����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU trigger������ֵ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trigcount : 8;  /* bit[0-7] : 8-bit����ֵ�������ӳ�ָ������words�������trigger��д�����ᵼ��trigger������reload������������Ԥ��ֵ�������ǵ�ǰ����ֵ�� */
        unsigned int  reserved  : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_TRIGGER_COUNTER_VALUE_UNION;
#endif
#define SOC_CS_TPIU_TPIU_TRIGGER_COUNTER_VALUE_trigcount_START  (0)
#define SOC_CS_TPIU_TPIU_TRIGGER_COUNTER_VALUE_trigcount_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_UNION
 �ṹ˵��  : TPIU_TRIGGER_MULTIPLIER �Ĵ����ṹ���塣��ַƫ����:0x108����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU trigger�������üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mult2    : 1;  /* bit[0]   : Multiply the Trigger Counter by 2 (2^1).
                                                   0 Multiplier disabled.
                                                   1 Multiplier enabled */
        unsigned int  mult4    : 1;  /* bit[1]   : Multiply the Trigger Counter by 4 (2^2).
                                                   0 Multiplier disabled.
                                                   1 Multiplier enabled. */
        unsigned int  mult16   : 1;  /* bit[2]   : Multiply the Trigger Counter by 16 (2^4).
                                                   0 Multiplier disabled.
                                                   1 Multiplier enabled. */
        unsigned int  mult256  : 1;  /* bit[3]   : Multiply the Trigger Counter by 256 (2^8).
                                                   0 Multiplier disabled.
                                                   1 Multiplier enabled */
        unsigned int  mult64k  : 1;  /* bit[4]   : Multiply the Trigger Counter by 65536 (2^16).
                                                   0 Multiplier disabled.
                                                   1 Multiplier enabled. */
        unsigned int  reserved : 27; /* bit[5-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_UNION;
#endif
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult2_START     (0)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult2_END       (0)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult4_START     (1)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult4_END       (1)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult16_START    (2)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult16_END      (2)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult256_START   (3)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult256_END     (3)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult64k_START   (4)
#define SOC_CS_TPIU_TPIU_TRIGGER_MULTIPLIER_mult64k_END     (4)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_UNION
 �ṹ˵��  : TPIU_SUPPORTED_TEST_PATTERN_MODE �Ĵ����ṹ���塣��ַƫ����:0x200����ֵ:0x0003000F�����:32
 �Ĵ���˵��: TPIU test patternģʽָʾ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  patw1    : 1;  /* bit[0]    : Indicates the walking 1s pattern is supported as output over the trace port. */
        unsigned int  patw0    : 1;  /* bit[1]    : Indicates the walking 0s pattern is supported as output over the trace port. */
        unsigned int  pata5    : 1;  /* bit[2]    : Indicates the AA/55 pattern is supported as output over the trace port. */
        unsigned int  patf0    : 1;  /* bit[3]    : Indicates the FF/00 pattern is supported as output over the trace port. */
        unsigned int  reserved_0: 12; /* bit[4-15] : Reserved */
        unsigned int  ptimeen  : 1;  /* bit[16]   : Indicates timed mode is supported. */
        unsigned int  pcouten  : 1;  /* bit[17]   : Indicates continuous mode is supported. */
        unsigned int  reserved_1: 14; /* bit[18-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_UNION;
#endif
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_patw1_START     (0)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_patw1_END       (0)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_patw0_START     (1)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_patw0_END       (1)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_pata5_START     (2)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_pata5_END       (2)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_patf0_START     (3)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_patf0_END       (3)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_ptimeen_START   (16)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_ptimeen_END     (16)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_pcouten_START   (17)
#define SOC_CS_TPIU_TPIU_SUPPORTED_TEST_PATTERN_MODE_pcouten_END     (17)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_UNION
 �ṹ˵��  : TPIU_CURRENT_TEST_PATTERN_MODE �Ĵ����ṹ���塣��ַƫ����:0x204����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU test patternģʽ���üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  patw1    : 1;  /* bit[0]    : Indicates whether the walking 1s pattern is enabled as output over the Trace Port.
                                                    0 Pattern disabled.
                                                    1 Pattern enabled. */
        unsigned int  patw0    : 1;  /* bit[1]    : Indicates whether the walking 0s pattern is enabled as output over the Trace Port.
                                                    0 Pattern disabled.
                                                    1 Pattern enabled. */
        unsigned int  pata5    : 1;  /* bit[2]    : Indicates whether the AA/55 pattern is enabled as output over the Trace Port.
                                                    0 Pattern disabled.
                                                    1 Pattern enabled. */
        unsigned int  patf0    : 1;  /* bit[3]    : Indicates whether the FF/00 pattern is enabled as output over the Trace Port.
                                                    0 Pattern disabled.
                                                    1 Pattern enabled. */
        unsigned int  reserved_0: 12; /* bit[4-15] : Reserved */
        unsigned int  ptimeen  : 1;  /* bit[16]   : Indicates whether Timed Mode is enabled.
                                                    0 Mode disabled.
                                                    1 Mode enabled. */
        unsigned int  pcouten  : 1;  /* bit[17]   : Indicates whether Continuous Mode is enabled.
                                                    0 Mode disabled.
                                                    1 Mode enabled. */
        unsigned int  reserved_1: 14; /* bit[18-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_UNION;
#endif
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_patw1_START     (0)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_patw1_END       (0)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_patw0_START     (1)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_patw0_END       (1)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_pata5_START     (2)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_pata5_END       (2)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_patf0_START     (3)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_patf0_END       (3)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_ptimeen_START   (16)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_ptimeen_END     (16)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_pcouten_START   (17)
#define SOC_CS_TPIU_TPIU_CURRENT_TEST_PATTERN_MODE_pcouten_END     (17)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_TPRCR_UNION
 �ṹ˵��  : TPIU_TPRCR �Ĵ����ṹ���塣��ַƫ����:0x208����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU test pattern�������Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pattcount : 8;  /* bit[0-7] : 8-bit counter value to indicate the number of traceclkin cycles for which a pattern runs before it 
                                                    switches to the next pattern. */
        unsigned int  reserved  : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_TPRCR_UNION;
#endif
#define SOC_CS_TPIU_TPIU_TPRCR_pattcount_START  (0)
#define SOC_CS_TPIU_TPIU_TPRCR_pattcount_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_FFSR_UNION
 �ṹ˵��  : TPIU_FFSR �Ĵ����ṹ���塣��ַƫ����:0x300����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU formatter and flush״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flinprog  : 1;  /* bit[0]   : Flush in progress.
                                                    0 afvalids is LOW.
                                                    1 afvalids is HIGH */
        unsigned int  ftstopped : 1;  /* bit[1]   : The formatter has received a stop request signal and all trace data and post-amble is sent. Any additional trace data on the ATB interface is ignored and atreadys goes HIGH.
                                                    0 Formatter has not stopped.
                                                    1 Formatter has stopped. */
        unsigned int  tcpresent : 1;  /* bit[2]   : Indicates whether the TRACECTL pin is available for use.
                                                    0 TRACECTL pin not present.
                                                    1 TRACECTL pin present. */
        unsigned int  reserved  : 29; /* bit[3-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_FFSR_UNION;
#endif
#define SOC_CS_TPIU_TPIU_FFSR_flinprog_START   (0)
#define SOC_CS_TPIU_TPIU_FFSR_flinprog_END     (0)
#define SOC_CS_TPIU_TPIU_FFSR_ftstopped_START  (1)
#define SOC_CS_TPIU_TPIU_FFSR_ftstopped_END    (1)
#define SOC_CS_TPIU_TPIU_FFSR_tcpresent_START  (2)
#define SOC_CS_TPIU_TPIU_FFSR_tcpresent_END    (2)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_FFCR_UNION
 �ṹ˵��  : TPIU_FFCR �Ĵ����ṹ���塣��ַƫ����:0x304����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU formatter and flush���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enftc    : 1;  /* bit[0]    : Do not embed triggers into the formatted stream. Trace disable cycles and triggers are indicated 
                                                    by tracectl, where present.
                                                    0 Formatting disabled.
                                                    1 Formatting enabled. */
        unsigned int  encont   : 1;  /* bit[1]    : Is embedded in trigger packets and indicates that no cycle is using sync packets.
                                                    0 Continuous formatting disabled.
                                                    1 Continuous formatting enabled. */
        unsigned int  reserved_0: 2;  /* bit[2-3]  : Reserved */
        unsigned int  fonflin  : 1;  /* bit[4]    : Enables the use of the flushin connection.
                                                    0 Disable generation of flush using the flushin interface.
                                                    1 Enable generation of flush using the flushin interface */
        unsigned int  fontrig  : 1;  /* bit[5]    : Initiates a manual flush of data in the system when a trigger event occurs.
                                                    A trigger event occurs when the trigger counter reaches 0, or, if the trigger counter is 0, when 
                                                    trigin is HIGH.
                                                    0 Disable generation of flush when a Trigger Event occurs.
                                                    1 Enable generation of flush when a Trigger Event occurs. */
        unsigned int  fonman   : 1;  /* bit[6]    : Generates a flush. This bit is set to 0 when this flush is serviced.
                                                    0 Manual flush is not initiated.
                                                    1 Manual flush is initiated. */
        unsigned int  reserved_1: 1;  /* bit[7]    : Reserved */
        unsigned int  trigin   : 1;  /* bit[8]    : Indicates a trigger when trigin is asserted.
                                                    0 Disable trigger indication when trigin is asserted.
                                                    1 Enable trigger indication when trigin is asserted. */
        unsigned int  trigevt  : 1;  /* bit[9]    : Indicates a trigger on a trigger event.
                                                    0 Disable trigger indication on a trigger event.
                                                    1 Enable trigger indication on a trigger event */
        unsigned int  trigfl   : 1;  /* bit[10]   : Indicates a trigger when flush completion on afreadys is returned.
                                                    0 Disable trigger indication on return of afreadys.
                                                    1 Enable trigger indication on return of afreadys. */
        unsigned int  reserved_2: 1;  /* bit[11]   : Reserved */
        unsigned int  stopfl   : 1;  /* bit[12]   : Forces the FIFO to drain off any part-completed packets.
                                                    0 Disable stopping the formatter on return of afreadys.
                                                    1 Enable stopping the formatter on return of afreadys */
        unsigned int  stoptrig : 1;  /* bit[13]   : Stops the formatter after a trigger event is observed. Reset to disabled or 0.
                                                    0 Disable stopping the formatter after a trigger event is observed.
                                                    1 Enable stopping the formatter after a trigger event is observed. */
        unsigned int  reserved_3: 18; /* bit[14-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_FFCR_UNION;
#endif
#define SOC_CS_TPIU_TPIU_FFCR_enftc_START     (0)
#define SOC_CS_TPIU_TPIU_FFCR_enftc_END       (0)
#define SOC_CS_TPIU_TPIU_FFCR_encont_START    (1)
#define SOC_CS_TPIU_TPIU_FFCR_encont_END      (1)
#define SOC_CS_TPIU_TPIU_FFCR_fonflin_START   (4)
#define SOC_CS_TPIU_TPIU_FFCR_fonflin_END     (4)
#define SOC_CS_TPIU_TPIU_FFCR_fontrig_START   (5)
#define SOC_CS_TPIU_TPIU_FFCR_fontrig_END     (5)
#define SOC_CS_TPIU_TPIU_FFCR_fonman_START    (6)
#define SOC_CS_TPIU_TPIU_FFCR_fonman_END      (6)
#define SOC_CS_TPIU_TPIU_FFCR_trigin_START    (8)
#define SOC_CS_TPIU_TPIU_FFCR_trigin_END      (8)
#define SOC_CS_TPIU_TPIU_FFCR_trigevt_START   (9)
#define SOC_CS_TPIU_TPIU_FFCR_trigevt_END     (9)
#define SOC_CS_TPIU_TPIU_FFCR_trigfl_START    (10)
#define SOC_CS_TPIU_TPIU_FFCR_trigfl_END      (10)
#define SOC_CS_TPIU_TPIU_FFCR_stopfl_START    (12)
#define SOC_CS_TPIU_TPIU_FFCR_stopfl_END      (12)
#define SOC_CS_TPIU_TPIU_FFCR_stoptrig_START  (13)
#define SOC_CS_TPIU_TPIU_FFCR_stoptrig_END    (13)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_FSCR_UNION
 �ṹ˵��  : TPIU_FSCR �Ĵ����ṹ���塣��ַƫ����:0x308����ֵ:0x00000040�����:32
 �Ĵ���˵��: TPIU Formatterͬ���������Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cyccount : 12; /* bit[0-11] : 12-bit����ֵ�����������ԵĲ���ͬ��������ʼֵΪ0x40����ÿ64֡���ݺ����ͬ������ */
        unsigned int  reserved : 20; /* bit[12-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_FSCR_UNION;
#endif
#define SOC_CS_TPIU_TPIU_FSCR_cyccount_START  (0)
#define SOC_CS_TPIU_TPIU_FSCR_cyccount_END    (11)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_EXTCTL_IN_PORT_UNION
 �ṹ˵��  : TPIU_EXTCTL_IN_PORT �Ĵ����ṹ���塣��ַƫ����:0x400����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU EXCTL In Port�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  extctlin : 8;  /* bit[0-7] : EXTCTL inputs. */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_EXTCTL_IN_PORT_UNION;
#endif
#define SOC_CS_TPIU_TPIU_EXTCTL_IN_PORT_extctlin_START  (0)
#define SOC_CS_TPIU_TPIU_EXTCTL_IN_PORT_extctlin_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_EXTCTL_OUT_PORT_UNION
 �ṹ˵��  : TPIU_EXTCTL_OUT_PORT �Ĵ����ṹ���塣��ַƫ����:0x404����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU EXCTL Out Port�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  extctlout : 8;  /* bit[0-7] : EXTCTL outputs. */
        unsigned int  reserved  : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TPIU_TPIU_EXTCTL_OUT_PORT_UNION;
#endif
#define SOC_CS_TPIU_TPIU_EXTCTL_OUT_PORT_extctlout_START  (0)
#define SOC_CS_TPIU_TPIU_EXTCTL_OUT_PORT_extctlout_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_LAR_UNION
 �ṹ˵��  : TPIU_LAR �Ĵ����ṹ���塣��ַƫ����:0xFB0����ֵ:0x00000000�����:32
 �Ĵ���˵��: TPIU�������ʼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  key : 32; /* bit[0-31]: д��0xC5ACCE55���н�����Ȼ��ſ����������������Ĵ�����д�������κ�ֵ���ٴ������� */
    } reg;
} SOC_CS_TPIU_TPIU_LAR_UNION;
#endif
#define SOC_CS_TPIU_TPIU_LAR_key_START  (0)
#define SOC_CS_TPIU_TPIU_LAR_key_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TPIU_TPIU_LSR_UNION
 �ṹ˵��  : TPIU_LSR �Ĵ����ṹ���塣��ַƫ����:0xFB4����ֵ:0x00000003�����:32
 �Ĵ���˵��: TPIU����״̬�Ĵ�����
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
} SOC_CS_TPIU_TPIU_LSR_UNION;
#endif
#define SOC_CS_TPIU_TPIU_LSR_sli_START       (0)
#define SOC_CS_TPIU_TPIU_LSR_sli_END         (0)
#define SOC_CS_TPIU_TPIU_LSR_slk_START       (1)
#define SOC_CS_TPIU_TPIU_LSR_slk_END         (1)
#define SOC_CS_TPIU_TPIU_LSR_ntt_START       (2)
#define SOC_CS_TPIU_TPIU_LSR_ntt_END         (2)






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

#endif /* end of soc_cs_tpiu_interface.h */
