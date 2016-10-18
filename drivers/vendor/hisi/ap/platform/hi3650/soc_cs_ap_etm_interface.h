/******************************************************************************

                 ��Ȩ���� (C), 2001-2015, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : soc_cs_ap_etm_interface.h
  �� �� ��   : ����
  ��    ��   : Excel2Code
  ��������   : 2015-11-20 11:30:53
  ����޸�   :
  ��������   : �ӿ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��20��
    ��    ��   : l00352922
    �޸�����   : �ӡ�Hi3660V100 SOC�Ĵ����ֲ�_CS_AP_ETM.xml���Զ�����

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_CS_AP_ETM_INTERFACE_H__
#define __SOC_CS_AP_ETM_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/1) ETM
 ****************************************************************************/
/* �Ĵ���˵����ETM��̿��ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCPRGCTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCPRGCTLR_ADDR(base)       ((base) + (0x004))

/* �Ĵ���˵����ETM״ָ̬ʾ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSTATR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSTATR_ADDR(base)         ((base) + (0x00C))

/* �Ĵ���˵����ETM���ÿ��ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCONFIGR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_ADDR(base)       ((base) + (0x010))

/* �Ĵ���˵����ETM�������ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCAUXCTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_ADDR(base)       ((base) + (0x018))

/* �Ĵ���˵����ETM�¼�����0�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_UNION */
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_ADDR(base)    ((base) + (0x020))

/* �Ĵ���˵����ETM�¼�����1�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_UNION */
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_ADDR(base)    ((base) + (0x024))

/* �Ĵ���˵����ETM stall���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_ADDR(base)     ((base) + (0x02C))

/* �Ĵ���˵����ETM timestamp���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCTSCTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCTSCTLR_ADDR(base)        ((base) + (0x030))

/* �Ĵ���˵����ETMͬ�����ڼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSYNCPR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSYNCPR_ADDR(base)        ((base) + (0x034))

/* �Ĵ���˵����ETM���ڼ������ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCCCTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCCCTLR_ADDR(base)        ((base) + (0x038))

/* �Ĵ���˵����ETM��֧�㲥���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCBBCTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCBBCTLR_ADDR(base)        ((base) + (0x03C))

/* �Ĵ���˵����ETM traceid���üĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCTRACEIDR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCTRACEIDR_ADDR(base)      ((base) + (0x040))

/* �Ĵ���˵����ETMָ��trace���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCVICTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_ADDR(base)        ((base) + (0x080))

/* �Ĵ���˵����ETMָ��trace Include/Exclude���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCVIIECTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCVIIECTLR_ADDR(base)      ((base) + (0x084))

/* �Ĵ���˵����ETMָ��trace Start/Stop���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCVISSCTLR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCVISSCTLR_ADDR(base)      ((base) + (0x088))

/* �Ĵ���˵����ETM sequencer0״̬ת�����ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSEQEVR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_ADDR(base)       ((base) + (0x100))

/* �Ĵ���˵����ETM sequencer1״̬ת�����ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSEQEVR1_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_ADDR(base)       ((base) + (0x104))

/* �Ĵ���˵����ETM sequencer2״̬ת�����ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSEQEVR2_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_ADDR(base)       ((base) + (0x108))

/* �Ĵ���˵����ETM sequencer��λ���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_ADDR(base)     ((base) + (0x118))

/* �Ĵ���˵����ETM sequencer״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSEQSTR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSEQSTR_ADDR(base)        ((base) + (0x11C))

/* �Ĵ���˵����ETM�ⲿ����ѡ��Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCEXTINSELR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_ADDR(base)     ((base) + (0x120))

/* �Ĵ���˵����ETM counter0 reload�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCNTRLDVR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCNTRLDVR0_ADDR(base)     ((base) + (0x140))

/* �Ĵ���˵����ETM counter1 reload�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCNTRLDVR1_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCNTRLDVR1_ADDR(base)     ((base) + (0x144))

/* �Ĵ���˵����ETM counter0���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_ADDR(base)      ((base) + (0x150))

/* �Ĵ���˵����ETM counter1���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_ADDR(base)      ((base) + (0x154))

/* �Ĵ���˵����ETM counter0����ֵ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCNTVR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCNTVR0_ADDR(base)        ((base) + (0x160))

/* �Ĵ���˵����ETM counter1����ֵ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCNTVR1_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCNTVR1_ADDR(base)        ((base) + (0x164))

/* �Ĵ���˵����ETM resourceѡ����ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCRSCTLRn_UNION */
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_ADDR(base, n)    ((base) + (0x200+(4*(n))))

/* �Ĵ���˵����ETM single-shot�Ƚ������ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSSCCR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSSCCR0_ADDR(base)        ((base) + (0x280))

/* �Ĵ���˵����ETM single-shot�Ƚ���״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCSSCSR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_ADDR(base)        ((base) + (0x2A0))

/* �Ĵ���˵����ETM OS Lock�������ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCOSLAR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCOSLAR_ADDR(base)         ((base) + (0x300))

/* �Ĵ���˵����ETM OS Lock����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCOSLSR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_ADDR(base)         ((base) + (0x304))

/* �Ĵ���˵����ETM�µ���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCPDCR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCPDCR_ADDR(base)          ((base) + (0x310))

/* �Ĵ���˵����ETM�µ�״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCPDSR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCPDSR_ADDR(base)          ((base) + (0x314))

/* �Ĵ���˵����ETM��ַ�Ƚ�ֵ��λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCACVRLm_UNION */
#define SOC_CS_AP_ETM_ETM_TRCACVRLm_ADDR(base, m)     ((base) + (0x400+(8*(m))))

/* �Ĵ���˵����ETM��ַ�Ƚ�ֵ��λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCACVRHm_UNION */
#define SOC_CS_AP_ETM_ETM_TRCACVRHm_ADDR(base, m)     ((base) + (0x404+(8*(m))))

/* �Ĵ���˵����ETM��ַ�Ƚ������ͼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCACATRm_UNION */
#define SOC_CS_AP_ETM_ETM_TRCACATRm_ADDR(base, m)     ((base) + (0x480+(8*(m))))

/* �Ĵ���˵����ETM context ID�Ƚ�ֵ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCIDCVR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCIDCVR0_ADDR(base)       ((base) + (0x600))

/* �Ĵ���˵����ETM VMID�Ƚ�ֵ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCVMIDCVR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCVMIDCVR0_ADDR(base)      ((base) + (0x640))

/* �Ĵ���˵����ETM context ID�Ƚ������ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCCIDCCTLR0_UNION */
#define SOC_CS_AP_ETM_ETM_TRCCIDCCTLR0_ADDR(base)     ((base) + (0x680))

/* �Ĵ���˵����ETM�������ʼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCLAR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCLAR_ADDR(base)           ((base) + (0xFB0))

/* �Ĵ���˵����ETM����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCLSR_UNION */
#define SOC_CS_AP_ETM_ETM_TRCLSR_ADDR(base)           ((base) + (0xFB4))

/* �Ĵ���˵����ETM��Ȩ״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_UNION */
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_ADDR(base)    ((base) + (0xFB8))





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
                     (1/1) ETM
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCPRGCTLR_UNION
 �ṹ˵��  : ETM_TRCPRGCTLR �Ĵ����ṹ���塣��ַƫ����:0x004����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM��̿��ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  en       : 1;  /* bit[0]   : Trace program enable:
                                                   0 The ETM trace unit interface in the processor is disabled, and clocks are enabled only when necessary to process APB accesses, or drain any already generated trace. This is the reset value.
                                                   1 The ETM trace unit interface in the processor is enabled, and clocks are enabled. Writes to most trace registers are ignored. */
        unsigned int  reserved : 31; /* bit[1-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCPRGCTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCPRGCTLR_en_START        (0)
#define SOC_CS_AP_ETM_ETM_TRCPRGCTLR_en_END          (0)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSTATR_UNION
 �ṹ˵��  : ETM_TRCSTATR �Ĵ����ṹ���塣��ַƫ����:0x00C����ֵ:0x00000003�����:32
 �Ĵ���˵��: ETM״ָ̬ʾ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  idle     : 1;  /* bit[0]   : Idle status:
                                                   0 The ETM trace unit is not idle.
                                                   1 The ETM trace unit is idle. */
        unsigned int  pmstable : 1;  /* bit[1]   : Indicates whether the ETM trace unit registers are stable and can be read:
                                                   0 The programmers model is not stable.
                                                   1 The programmers model is stable. */
        unsigned int  reserved : 30; /* bit[2-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSTATR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSTATR_idle_START      (0)
#define SOC_CS_AP_ETM_ETM_TRCSTATR_idle_END        (0)
#define SOC_CS_AP_ETM_ETM_TRCSTATR_pmstable_START  (1)
#define SOC_CS_AP_ETM_ETM_TRCSTATR_pmstable_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCONFIGR_UNION
 �ṹ˵��  : ETM_TRCCONFIGR �Ĵ����ṹ���塣��ַƫ����:0x010����ֵ:0x00000001�����:32
 �Ĵ���˵��: ETM���ÿ��ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 1;  /* bit[0]    : Reserved */
        unsigned int  reserved_1: 2;  /* bit[1-2]  : Reserved */
        unsigned int  bb       : 1;  /* bit[3]    : Enables branch broadcast mode. The possible values are:
                                                    0 Disables branch broadcast mode.
                                                    1 Enables branch broadcast mode. */
        unsigned int  cci      : 1;  /* bit[4]    : Enables cycle counting instruction trace. The possible values are:
                                                    0 Disables cycle counting instruction trace.
                                                    1 Enables cycle counting instruction trace. */
        unsigned int  reserved_2: 1;  /* bit[5]    : Reserved */
        unsigned int  cid      : 1;  /* bit[6]    : Enables context ID tracing. The possible values are:
                                                    0 Disables context ID tracing.
                                                    1 Enables context ID tracing. */
        unsigned int  vmid     : 1;  /* bit[7]    : Enables VMID tracing. The possible values are:
                                                    0 Disables VMID tracing.
                                                    1 Enables VMID tracing. */
        unsigned int  reserved_3: 3;  /* bit[8-10] : Reserved */
        unsigned int  ts       : 1;  /* bit[11]   : Enables global timestamp tracing. The possible values are:
                                                    0 Disables global timestamp tracing.
                                                    1 Enables global timestamp tracing. */
        unsigned int  rs       : 1;  /* bit[12]   : Enables the return stack. The possible values are:
                                                    0 Disables the return stack.
                                                    1 Enables the return stack. */
        unsigned int  reserved_4: 19; /* bit[13-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCONFIGR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_bb_START        (3)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_bb_END          (3)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_cci_START       (4)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_cci_END         (4)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_cid_START       (6)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_cid_END         (6)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_vmid_START      (7)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_vmid_END        (7)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_ts_START        (11)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_ts_END          (11)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_rs_START        (12)
#define SOC_CS_AP_ETM_ETM_TRCCONFIGR_rs_END          (12)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCAUXCTLR_UNION
 �ṹ˵��  : ETM_TRCAUXCTLR �Ĵ����ṹ���塣��ַƫ����:0x018����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM�������ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  afready     : 1;  /* bit[0]   : Always respond to AFREADY immediately. Does not have any interaction with FIFO draining, 
                                                      even in WFI state. The possible values are:
                                                      0 ETM trace unit AFREADYM output is asserted only when the ETM trace unit 
                                                      is in idle state or when all the trace bytes in FIFO before a flush request are output.
                                                      1 ETM trace unit AFREADYM output is always asserted HIGH. When this bit 
                                                      is set to 1, trace unit behavior deviates from architecturally-specified behavior. */
        unsigned int  idleack     : 1;  /* bit[1]   : Force idle-drain acknowledge high, CPU does not wait for trace to drain before entering WFX state. The possible values are:
                                                      0 ETM trace unit idle acknowledge is asserted only when the ETM trace unit is in idle state.
                                                      1 ETM trace unit idle acknowledge is asserted irrespective of the ETM trace unit idle state. */
        unsigned int  ovflw       : 1;  /* bit[2]   : Force an overflow if synchronization is not completed when second synchronization becomes due. The possible values are:
                                                      0 No FIFO overflow when SYNC packets are delayed.
                                                      1 Forces FIFO overflow when SYNC packets are delayed. */
        unsigned int  syncdelay   : 1;  /* bit[3]   : Delay periodic synchronization if FIFO is more than half-full. The possible values are:
                                                      0 SYNC packets are inserted into FIFO only when trace activity is low.
                                                      1 SYNC packets are inserted into FIFO irrespective of trace activity. */
        unsigned int  tsnodelay   : 1;  /* bit[4]   : Do not delay timestamp insertion based on FIFO depth. The possible values are:
                                                      0 Timestamp packets are inserted into FIFO only when trace activity is LOW.
                                                      1 Timestamp packets are inserted into FIFO irrespective of trace activity */
        unsigned int  authnoflush : 1;  /* bit[5]   : Do not flush trace on de-assertion of authentication inputs. The possible values are:
                                                      0 ETM trace unit FIFO is flushed and ETM trace unit enters idle state when 
                                                      DBGEN or NIDEN is LOW.
                                                      1 ETM trace unit FIFO is not flushed and ETM trace unit does not enter idle state when DBGEN or NIDEN is LOW */
        unsigned int  reserved_0  : 1;  /* bit[6]   : Reserved */
        unsigned int  coreifen    : 1;  /* bit[7]   : Keep core interface enabled regardless of trace enable register state. The possible values are:
                                                      0 Core interface enabled is set by trace enable register state.
                                                      1 Enable core interface, regardless of trace enable register state. */
        unsigned int  reserved_1  : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCAUXCTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_afready_START      (0)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_afready_END        (0)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_idleack_START      (1)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_idleack_END        (1)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_ovflw_START        (2)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_ovflw_END          (2)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_syncdelay_START    (3)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_syncdelay_END      (3)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_tsnodelay_START    (4)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_tsnodelay_END      (4)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_authnoflush_START  (5)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_authnoflush_END    (5)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_coreifen_START     (7)
#define SOC_CS_AP_ETM_ETM_TRCAUXCTLR_coreifen_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_UNION
 �ṹ˵��  : ETM_TRCEVENTCTL0R �Ĵ����ṹ���塣��ַƫ����:0x020����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM�¼�����0�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sel0     : 4;  /* bit[0-3]  : Selects the resource number, based on the value of TYPE0:
                                                    When TYPE0 is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When TYPE0 is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0: 3;  /* bit[4-6]  : Reserved */
        unsigned int  type0    : 1;  /* bit[7]    : Selects the resource type for trace event 0:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  sel1     : 4;  /* bit[8-11] : Selects the resource number, based on the value of TYPE1:
                                                    When TYPE1 is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When TYPE1 is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_1: 3;  /* bit[12-14]: Reserved */
        unsigned int  type1    : 1;  /* bit[15]   : Selects the resource type for trace event 1:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  sel2     : 4;  /* bit[16-19]: Selects the resource number, based on the value of TYPE2:
                                                    When TYPE2 is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When TYPE2 is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_2: 3;  /* bit[20-22]: Reserved */
        unsigned int  type2    : 1;  /* bit[23]   : Selects the resource type for trace event 2:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  sel3     : 4;  /* bit[24-27]: Selects the resource number, based on the value of TYPE3:
                                                    When TYPE3 is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When TYPE3 is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_3: 3;  /* bit[28-30]: Reserved */
        unsigned int  type3    : 1;  /* bit[31]   : Selects the resource type for trace event 3:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel0_START      (0)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel0_END        (3)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type0_START     (7)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type0_END       (7)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel1_START      (8)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel1_END        (11)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type1_START     (15)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type1_END       (15)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel2_START      (16)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel2_END        (19)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type2_START     (23)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type2_END       (23)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel3_START      (24)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_sel3_END        (27)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type3_START     (31)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL0R_type3_END       (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_UNION
 �ṹ˵��  : ETM_TRCEVENTCTL1R �Ĵ����ṹ���塣��ַƫ����:0x024����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM�¼�����1�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  en         : 4;  /* bit[0-3]  : One bit per event, to enable generation of an event element in the instruction trace stream when the 
                                                      selected event occurs:
                                                      0 Event does not cause an event element.
                                                      1 Event causes an event element. */
        unsigned int  reserved_0 : 7;  /* bit[4-10] : Reserved */
        unsigned int  atb        : 1;  /* bit[11]   : ATB trigger enable:
                                                      0 ATB trigger disabled.
                                                      1 ATB trigger enabled. */
        unsigned int  lpoverride : 1;  /* bit[12]   : Low power state behavior override:
                                                      0 Low power state behavior unaffected.
                                                      1 Low power state behavior overridden. The resources and Event trace generation are unaffected by entry to a low power state. */
        unsigned int  reserved_1 : 19; /* bit[13-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_en_START          (0)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_en_END            (3)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_atb_START         (11)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_atb_END           (11)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_lpoverride_START  (12)
#define SOC_CS_AP_ETM_ETM_TRCEVENTCTL1R_lpoverride_END    (12)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_UNION
 �ṹ˵��  : ETM_TRCSTALLCTLR �Ĵ����ṹ���塣��ַƫ����:0x02C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM stall���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 2;  /* bit[0-1] : Reserved */
        unsigned int  level    : 2;  /* bit[2-3] : Threshold level field. The field can support 4 monotonic levels from 0b00 to 0b11, where:
                                                   0b00 Zero invasion. This setting has a greater risk of an ETM trace unit FIFO overflow.
                                                   0b11 Maximum invasion occurs but there is less risk of a FIFO overflow. */
        unsigned int  reserved_1: 4;  /* bit[4-7] : Reserved */
        unsigned int  istall   : 1;  /* bit[8]   : Instruction stall bit. Controls if the trace unit can stall the processor when the instruction trace buffer space is less than LEVEL:
                                                   0 The trace unit does not stall the processor.
                                                   1 The trace unit can stall the processor.  */
        unsigned int  reserved_2: 23; /* bit[9-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_level_START     (2)
#define SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_level_END       (3)
#define SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_istall_START    (8)
#define SOC_CS_AP_ETM_ETM_TRCSTALLCTLR_istall_END      (8)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCTSCTLR_UNION
 �ṹ˵��  : ETM_TRCTSCTLR �Ĵ����ṹ���塣��ַƫ����:0x030����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM timestamp���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sel      : 4;  /* bit[0-3] : Identifies the resource selector to use */
        unsigned int  reserved_0: 3;  /* bit[4-6] : Reserved */
        unsigned int  type     : 1;  /* bit[7]   : Single or combined resource selector */
        unsigned int  reserved_1: 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCTSCTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCTSCTLR_sel_START       (0)
#define SOC_CS_AP_ETM_ETM_TRCTSCTLR_sel_END         (3)
#define SOC_CS_AP_ETM_ETM_TRCTSCTLR_type_START      (7)
#define SOC_CS_AP_ETM_ETM_TRCTSCTLR_type_END        (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSYNCPR_UNION
 �ṹ˵��  : ETM_TRCSYNCPR �Ĵ����ṹ���塣��ַƫ����:0x034����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETMͬ�����ڼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  period   : 5;  /* bit[0-4] : Defines the number of bytes of trace between synchronization requests.The number of bytes is 2^N where N is the value of this field.
                                                   ? A value of zero disables these periodic synchronization requests, but does not disable other 
                                                   synchronization requests.
                                                   ? The minimum value that can be programmed, other than zero, is 8, providing a minimum synchronization 
                                                   period of 256 bytes.
                                                   ? The maximum value is 20, providing a maximum synchronization period of 2^20 bytes. */
        unsigned int  reserved : 27; /* bit[5-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSYNCPR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSYNCPR_period_START    (0)
#define SOC_CS_AP_ETM_ETM_TRCSYNCPR_period_END      (4)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCCCTLR_UNION
 �ṹ˵��  : ETM_TRCCCCTLR �Ĵ����ṹ���塣��ַƫ����:0x038����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM���ڼ������ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  threshold : 12; /* bit[0-11] : Instruction trace cycle count threshold. */
        unsigned int  reserved  : 20; /* bit[12-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCCCTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCCCTLR_threshold_START  (0)
#define SOC_CS_AP_ETM_ETM_TRCCCCTLR_threshold_END    (11)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCBBCTLR_UNION
 �ṹ˵��  : ETM_TRCBBCTLR �Ĵ����ṹ���塣��ַƫ����:0x03C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM��֧�㲥���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  range    : 8;  /* bit[0-7] : Address range field. 
                                                   Selects which address range comparator pairs are in use with branch broadcasting. Each bit represents an address range comparator pair, so bit[n] controls the selection of address range comparator pair n. If bit[n] is:
                                                   0 The address range that address range comparator pair n defines, is not selected.
                                                   1 The address range that address range comparator pair n defines, is selected. */
        unsigned int  mode     : 1;  /* bit[8]   : Mode bit:
                                                   0 Exclude mode. Branch broadcasting is not enabled in the address range that RANGE defines.
                                                   If RANGE==0 then branch broadcasting is enabled for the entire memory map.
                                                   1 Include mode. Branch broadcasting is enabled in the address range that RANGE defines.
                                                   If RANGE==0 then the behavior of the trace unit is constrained UNPREDICTABLE. That is, the trace unit might or might not consider any instructions to be in a branch broadcast region. */
        unsigned int  reserved : 23; /* bit[9-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCBBCTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCBBCTLR_range_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCBBCTLR_range_END       (7)
#define SOC_CS_AP_ETM_ETM_TRCBBCTLR_mode_START      (8)
#define SOC_CS_AP_ETM_ETM_TRCBBCTLR_mode_END        (8)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCTRACEIDR_UNION
 �ṹ˵��  : ETM_TRCTRACEIDR �Ĵ����ṹ���塣��ַƫ����:0x040����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM traceid���üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  traceid  : 7;  /* bit[0-6] : Trace ID value.
                                                   When only instruction tracing is enabled, this provides the trace ID. */
        unsigned int  reserved : 25; /* bit[7-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCTRACEIDR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCTRACEIDR_traceid_START   (0)
#define SOC_CS_AP_ETM_ETM_TRCTRACEIDR_traceid_END     (6)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCVICTLR_UNION
 �ṹ˵��  : ETM_TRCVICTLR �Ĵ����ṹ���塣��ַƫ����:0x080����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETMָ��trace���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sel        : 4;  /* bit[0-3]  : Selects the resource number to use for the viewinst event, based on the value of TYPE:
                                                      When TYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                      When TYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0 : 3;  /* bit[4-6]  : Reserved */
        unsigned int  type       : 1;  /* bit[7]    : Selects the resource type for the viewinst event:
                                                      0 Single selected resource.
                                                      1 Boolean combined resource pair. */
        unsigned int  reserved_1 : 1;  /* bit[8]    : Reserved */
        unsigned int  ssstatus   : 1;  /* bit[9]    : Indicates the current status of the start/stop logic:
                                                      0 Start/stop logic is in the stopped state.
                                                      1 Start/stop logic is in the started state. */
        unsigned int  trcreset   : 1;  /* bit[10]   : Selects whether a reset exception must always be traced:
                                                      0 Reset exception is traced only if the instruction or exception immediately before the reset 
                                                      exception is traced.
                                                      1 Reset exception is always traced regardless of the value of ViewInst. */
        unsigned int  trcerr     : 1;  /* bit[11]   : Selects whether a system error exception must always be traced:
                                                      0 System error exception is traced only if the instruction or exception immediately before 
                                                      the system error exception is traced.
                                                      1 System error exception is always traced regardless of the value of ViewInst */
        unsigned int  reserved_2 : 4;  /* bit[12-15]: Reserved */
        unsigned int  exlevel_s  : 4;  /* bit[16-19]: In Secure state, each bit controls whether instruction tracing is enabled for the corresponding exception level:
                                                      0 Trace unit generates instruction trace, in Secure state, for exception level n.
                                                      1 Trace unit does not generate instruction trace, in Secure state, for exception level n.
                                                      The exception levels are:
                                                      Bit[16] Exception level 0.
                                                      Bit[17] Exception level 1.
                                                      Bit[18] RAZ/WI.
                                                      Bit[19] Exception level 3. */
        unsigned int  exlevel_ns : 4;  /* bit[20-23]: In Non-secure state, each bit controls whether instruction tracing is enabled for the corresponding 
                                                      exception level:
                                                      0 Trace unit generates instruction trace, in Non-secure state, for exception level n.
                                                      1 Trace unit does not generate instruction trace, in Non-secure state, for exception level n.
                                                      The exception levels are:
                                                      Bit[20] Exception level 0.
                                                      Bit[21] Exception level 1.
                                                      Bit[22] Exception level 2.
                                                      Bit[23] RAZ/WI.  */
        unsigned int  reserved_3 : 8;  /* bit[24-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCVICTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_sel_START         (0)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_sel_END           (3)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_type_START        (7)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_type_END          (7)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_ssstatus_START    (9)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_ssstatus_END      (9)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_trcreset_START    (10)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_trcreset_END      (10)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_trcerr_START      (11)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_trcerr_END        (11)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_exlevel_s_START   (16)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_exlevel_s_END     (19)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_exlevel_ns_START  (20)
#define SOC_CS_AP_ETM_ETM_TRCVICTLR_exlevel_ns_END    (23)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCVIIECTLR_UNION
 �ṹ˵��  : ETM_TRCVIIECTLR �Ĵ����ṹ���塣��ַƫ����:0x084����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETMָ��trace Include/Exclude���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  include  : 4;  /* bit[0-3]  : Defines the address range comparators for ViewInst include control.One bit is provided for each implemented Address Range Comparator.  */
        unsigned int  reserved_0: 12; /* bit[4-15] : Reserved */
        unsigned int  exclude  : 4;  /* bit[16-19]: Defines the address range comparators for ViewInst exclude control. One bit is provided for each 
                                                    implemented Address Range Comparator. */
        unsigned int  reserved_1: 12; /* bit[20-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCVIIECTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCVIIECTLR_include_START   (0)
#define SOC_CS_AP_ETM_ETM_TRCVIIECTLR_include_END     (3)
#define SOC_CS_AP_ETM_ETM_TRCVIIECTLR_exclude_START   (16)
#define SOC_CS_AP_ETM_ETM_TRCVIIECTLR_exclude_END     (19)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCVISSCTLR_UNION
 �ṹ˵��  : ETM_TRCVISSCTLR �Ĵ����ṹ���塣��ַƫ����:0x088����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETMָ��trace Start/Stop���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  start    : 8;  /* bit[0-7]  : Defines the single address comparators to start trace with the ViewInst Start/Stop control.
                                                    One bit is provided for each implemented single address comparator.  */
        unsigned int  reserved_0: 8;  /* bit[8-15] : Reserved */
        unsigned int  stop     : 8;  /* bit[16-23]: Defines the single address comparators to stop trace with the ViewInst Start/Stop control.
                                                    One bit is provided for each implemented single address comparator.  */
        unsigned int  reserved_1: 8;  /* bit[24-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCVISSCTLR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCVISSCTLR_start_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCVISSCTLR_start_END       (7)
#define SOC_CS_AP_ETM_ETM_TRCVISSCTLR_stop_START      (16)
#define SOC_CS_AP_ETM_ETM_TRCVISSCTLR_stop_END        (23)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSEQEVR0_UNION
 �ṹ˵��  : ETM_TRCSEQEVR0 �Ĵ����ṹ���塣��ַƫ����:0x100����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM sequencer0״̬ת�����ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  f_sel    : 4;  /* bit[0-3]  : Selects the resource number, based on the value of F TYPE:
                                                    When F TYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When F TYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0: 3;  /* bit[4-6]  : Reserved */
        unsigned int  f_type   : 1;  /* bit[7]    : Selects the resource type to move forwards from this state to the next state:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  b_sel    : 4;  /* bit[8-11] : Selects the resource number, based on the value of B TYPE:
                                                    When B TYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When B TYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_1: 3;  /* bit[12-14]: Reserved */
        unsigned int  b_type   : 1;  /* bit[15]   : Selects the resource type to move backwards to this state from the next state:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  reserved_2: 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSEQEVR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_f_sel_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_f_sel_END       (3)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_f_type_START    (7)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_f_type_END      (7)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_b_sel_START     (8)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_b_sel_END       (11)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_b_type_START    (15)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR0_b_type_END      (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSEQEVR1_UNION
 �ṹ˵��  : ETM_TRCSEQEVR1 �Ĵ����ṹ���塣��ַƫ����:0x104����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM sequencer1״̬ת�����ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  f_sel    : 4;  /* bit[0-3]  : Selects the resource number, based on the value of F TYPE:
                                                    When F TYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When F TYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0: 3;  /* bit[4-6]  : Reserved */
        unsigned int  f_type   : 1;  /* bit[7]    : Selects the resource type to move forwards from this state to the next state:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  b_sel    : 4;  /* bit[8-11] : Selects the resource number, based on the value of B TYPE:
                                                    When B TYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When B TYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_1: 3;  /* bit[12-14]: Reserved */
        unsigned int  b_type   : 1;  /* bit[15]   : Selects the resource type to move backwards to this state from the next state:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  reserved_2: 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSEQEVR1_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_f_sel_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_f_sel_END       (3)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_f_type_START    (7)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_f_type_END      (7)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_b_sel_START     (8)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_b_sel_END       (11)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_b_type_START    (15)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR1_b_type_END      (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSEQEVR2_UNION
 �ṹ˵��  : ETM_TRCSEQEVR2 �Ĵ����ṹ���塣��ַƫ����:0x108����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM sequencer2״̬ת�����ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  f_sel    : 4;  /* bit[0-3]  : Selects the resource number, based on the value of F TYPE:
                                                    When F TYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When F TYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0: 3;  /* bit[4-6]  : Reserved */
        unsigned int  f_type   : 1;  /* bit[7]    : Selects the resource type to move forwards from this state to the next state:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  b_sel    : 4;  /* bit[8-11] : Selects the resource number, based on the value of B TYPE:
                                                    When B TYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When B TYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_1: 3;  /* bit[12-14]: Reserved */
        unsigned int  b_type   : 1;  /* bit[15]   : Selects the resource type to move backwards to this state from the next state:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  reserved_2: 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSEQEVR2_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_f_sel_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_f_sel_END       (3)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_f_type_START    (7)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_f_type_END      (7)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_b_sel_START     (8)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_b_sel_END       (11)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_b_type_START    (15)
#define SOC_CS_AP_ETM_ETM_TRCSEQEVR2_b_type_END      (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_UNION
 �ṹ˵��  : ETM_TRCSEQRSTEVR �Ĵ����ṹ���塣��ַƫ����:0x118����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM sequencer��λ���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  resetsel  : 4;  /* bit[0-3] : Selects the resource number, based on the value of RESETTYPE:
                                                    When RESETTYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When RESETTYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0: 3;  /* bit[4-6] : Reserved */
        unsigned int  resettype : 1;  /* bit[7]   : Selects the resource type to move back to state 0:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  reserved_1: 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_resetsel_START   (0)
#define SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_resetsel_END     (3)
#define SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_resettype_START  (7)
#define SOC_CS_AP_ETM_ETM_TRCSEQRSTEVR_resettype_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSEQSTR_UNION
 �ṹ˵��  : ETM_TRCSEQSTR �Ĵ����ṹ���塣��ַƫ����:0x11C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM sequencer״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  state    : 2;  /* bit[0-1] : Current sequencer state:
                                                   0b00 State 0.
                                                   0b01 State 1.
                                                   0b10 State 2.
                                                   0b11 State 3. */
        unsigned int  reserved : 30; /* bit[2-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSEQSTR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSEQSTR_state_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCSEQSTR_state_END       (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCEXTINSELR_UNION
 �ṹ˵��  : ETM_TRCEXTINSELR �Ĵ����ṹ���塣��ַƫ����:0x120����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM�ⲿ����ѡ��Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sel0     : 5;  /* bit[0-4]  : Selects an event from the external input bus for External Input Resource 0. */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : Reserved */
        unsigned int  sel1     : 5;  /* bit[8-12] : Selects an event from the external input bus for External Input Resource 1. */
        unsigned int  reserved_1: 3;  /* bit[13-15]: Reserved */
        unsigned int  sel2     : 5;  /* bit[16-20]: Selects an event from the external input bus for External Input Resource 2. */
        unsigned int  reserved_2: 3;  /* bit[21-23]: Reserved */
        unsigned int  sel3     : 5;  /* bit[24-28]: Selects an event from the external input bus for External Input Resource 3. */
        unsigned int  reserved_3: 3;  /* bit[29-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCEXTINSELR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel0_START      (0)
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel0_END        (4)
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel1_START      (8)
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel1_END        (12)
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel2_START      (16)
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel2_END        (20)
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel3_START      (24)
#define SOC_CS_AP_ETM_ETM_TRCEXTINSELR_sel3_END        (28)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCNTRLDVR0_UNION
 �ṹ˵��  : ETM_TRCCNTRLDVR0 �Ĵ����ṹ���塣��ַƫ����:0x140����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM counter0 reload�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  value    : 16; /* bit[0-15] : Defines the reload value for the counter. This value is loaded into the counter each time the reload event occurs. */
        unsigned int  reserved : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCNTRLDVR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCNTRLDVR0_value_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCCNTRLDVR0_value_END       (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCNTRLDVR1_UNION
 �ṹ˵��  : ETM_TRCCNTRLDVR1 �Ĵ����ṹ���塣��ַƫ����:0x144����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM counter1 reload�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  value    : 16; /* bit[0-15] : Defines the reload value for the counter. This value is loaded into the counter each time the reload event occurs. */
        unsigned int  reserved : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCNTRLDVR1_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCNTRLDVR1_value_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCCNTRLDVR1_value_END       (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_UNION
 �ṹ˵��  : ETM_TRCCNTCTLR0 �Ĵ����ṹ���塣��ַƫ����:0x150����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM counter0���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cntsel   : 4;  /* bit[0-3]  : Selects the resource number, based on the value of CNTTYPE:
                                                    When CNTTYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When CNTTYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0: 3;  /* bit[4-6]  : Reserved */
        unsigned int  cnttype  : 1;  /* bit[7]    : Selects the resource type for the counter:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  rldsel   : 4;  /* bit[8-11] : Selects the resource number, based on the value of RLDTYPE:
                                                    When RLDTYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When RLDTYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_1: 3;  /* bit[12-14]: Reserved */
        unsigned int  rldtype  : 1;  /* bit[15]   : Selects the resource type for the reload:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  rldself  : 1;  /* bit[16]   : Defines whether the counter reloads when it reaches zero:
                                                    0 The counter does not reload when it reaches zero. The counter only reloads based on 
                                                    RLDTYPE and RLDSEL.
                                                    1 The counter reloads when it reaches zero and the resource selected by CNTTYPE and CNTSEL is also active. The counter also reloads based on RLDTYPE and RLDSEL. */
        unsigned int  reserved_2: 15; /* bit[17-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_cntsel_START    (0)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_cntsel_END      (3)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_cnttype_START   (7)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_cnttype_END     (7)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_rldsel_START    (8)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_rldsel_END      (11)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_rldtype_START   (15)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_rldtype_END     (15)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_rldself_START   (16)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR0_rldself_END     (16)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_UNION
 �ṹ˵��  : ETM_TRCCNTCTLR1 �Ĵ����ṹ���塣��ַƫ����:0x154����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM counter1���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cntsel   : 4;  /* bit[0-3]  : Selects the resource number, based on the value of CNTTYPE:
                                                    When CNTTYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When CNTTYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_0: 3;  /* bit[4-6]  : Reserved */
        unsigned int  cnttype  : 1;  /* bit[7]    : Selects the resource type for the counter:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  rldsel   : 4;  /* bit[8-11] : Selects the resource number, based on the value of RLDTYPE:
                                                    When RLDTYPE is 0, selects a single selected resource from 0-15 defined by bits[3:0].
                                                    When RLDTYPE is 1, selects a Boolean combined resource pair from 0-7 defined by bits[2:0]. */
        unsigned int  reserved_1: 3;  /* bit[12-14]: Reserved */
        unsigned int  rldtype  : 1;  /* bit[15]   : Selects the resource type for the reload:
                                                    0 Single selected resource.
                                                    1 Boolean combined resource pair. */
        unsigned int  rldself  : 1;  /* bit[16]   : Defines whether the counter reloads when it reaches zero:
                                                    0 The counter does not reload when it reaches zero. The counter only reloads based on 
                                                    RLDTYPE and RLDSEL.
                                                    1 The counter reloads when it reaches zero and the resource selected by CNTTYPE and CNTSEL is also active. The counter also reloads based on RLDTYPE and RLDSEL. */
        unsigned int  cntchain : 1;  /* bit[17]   : Defines whether the counter decrements when the counter reloads. This enables two counters to be used in 
                                                    combination to provide a larger counter:
                                                    0 The counter operates independently from the counter. The counter only decrements based 
                                                    on CNTTYPE and CNTSEL.
                                                    1 The counter decrements when the counter reloads. The counter also decrements when the 
                                                    resource selected by CNTTYPE and CNTSEL is active. */
        unsigned int  reserved_2: 14; /* bit[18-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_cntsel_START    (0)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_cntsel_END      (3)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_cnttype_START   (7)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_cnttype_END     (7)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_rldsel_START    (8)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_rldsel_END      (11)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_rldtype_START   (15)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_rldtype_END     (15)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_rldself_START   (16)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_rldself_END     (16)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_cntchain_START  (17)
#define SOC_CS_AP_ETM_ETM_TRCCNTCTLR1_cntchain_END    (17)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCNTVR0_UNION
 �ṹ˵��  : ETM_TRCCNTVR0 �Ĵ����ṹ���塣��ַƫ����:0x160����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM counter0����ֵ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  value    : 16; /* bit[0-15] : Contains the current counter value. */
        unsigned int  reserved : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCNTVR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCNTVR0_value_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCCNTVR0_value_END       (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCNTVR1_UNION
 �ṹ˵��  : ETM_TRCCNTVR1 �Ĵ����ṹ���塣��ַƫ����:0x164����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM counter1����ֵ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  value    : 16; /* bit[0-15] : Contains the current counter value. */
        unsigned int  reserved : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCNTVR1_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCNTVR1_value_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCCNTVR1_value_END       (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCRSCTLRn_UNION
 �ṹ˵��  : ETM_TRCRSCTLRn �Ĵ����ṹ���塣��ַƫ����:0x200+(4*(n))����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM resourceѡ����ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  select   : 8;  /* bit[0-7]  : Selects one or more resources from the required group. One bit is provided for each resource from the group. */
        unsigned int  reserved_0: 8;  /* bit[8-15] : Reserved */
        unsigned int  group    : 3;  /* bit[16-18]: Selects a group of resources.  */
        unsigned int  reserved_1: 1;  /* bit[19]   : Reserved */
        unsigned int  inv      : 1;  /* bit[20]   : Inverts the selected resources:
                                                    0 Resource is not inverted.
                                                    1 Resource is inverted. */
        unsigned int  pairinv  : 1;  /* bit[21]   : Inverts the result of a combined pair of resources. 
                                                    This bit is implemented only on the lower register for a pair of resource selectors. */
        unsigned int  reserved_2: 10; /* bit[22-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCRSCTLRn_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_select_START    (0)
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_select_END      (7)
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_group_START     (16)
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_group_END       (18)
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_inv_START       (20)
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_inv_END         (20)
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_pairinv_START   (21)
#define SOC_CS_AP_ETM_ETM_TRCRSCTLRn_pairinv_END     (21)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSSCCR0_UNION
 �ṹ˵��  : ETM_TRCSSCCR0 �Ĵ����ṹ���塣��ַƫ����:0x280����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM single-shot�Ƚ������ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sac      : 8;  /* bit[0-7]  : Selects one or more single address comparators for single-shot control. One bit is provided for each implemented single address comparator. */
        unsigned int  reserved_0: 8;  /* bit[8-15] : Reserved */
        unsigned int  arc      : 4;  /* bit[16-19]: Selects one or more address range comparators for single-shot control. One bit is provided for each implemented address range comparator. */
        unsigned int  reserved_1: 4;  /* bit[20-23]: Reserved */
        unsigned int  rst      : 1;  /* bit[24]   : Controls whether the single-shot comparator resource is reset when it fires. The possible values are:
                                                    0 The single-shot comparator resource is not reset when it fires.
                                                    1 The single-shot comparator resource is reset when it fires. This enables the single-shot comparator
                                                    resource to fire multiple times. */
        unsigned int  reserved_2: 7;  /* bit[25-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSSCCR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSSCCR0_sac_START       (0)
#define SOC_CS_AP_ETM_ETM_TRCSSCCR0_sac_END         (7)
#define SOC_CS_AP_ETM_ETM_TRCSSCCR0_arc_START       (16)
#define SOC_CS_AP_ETM_ETM_TRCSSCCR0_arc_END         (19)
#define SOC_CS_AP_ETM_ETM_TRCSSCCR0_rst_START       (24)
#define SOC_CS_AP_ETM_ETM_TRCSSCCR0_rst_END         (24)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCSSCSR0_UNION
 �ṹ˵��  : ETM_TRCSSCSR0 �Ĵ����ṹ���塣��ַƫ����:0x2A0����ֵ:0x00000001�����:32
 �Ĵ���˵��: ETM single-shot�Ƚ���״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inst     : 1;  /* bit[0]   : Instruction address comparator support:
                                                   1 Single-shot instruction address comparisons supported. */
        unsigned int  da       : 1;  /* bit[1]   : Data address comparator support:
                                                   0 Single-shot data address comparisons not supported.  */
        unsigned int  dv       : 1;  /* bit[2]   : Data value comparator support:
                                                   0 Single-shot data value comparisons not supported. */
        unsigned int  reserved : 28; /* bit[3-30]: Reserved */
        unsigned int  status   : 1;  /* bit[31]  : Single-shot status. This indicates whether any of the selected comparators have matched:
                                                   0 Match has not occurred.
                                                   1 Match has occurred at least once. */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCSSCSR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_inst_START      (0)
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_inst_END        (0)
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_da_START        (1)
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_da_END          (1)
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_dv_START        (2)
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_dv_END          (2)
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_status_START    (31)
#define SOC_CS_AP_ETM_ETM_TRCSSCSR0_status_END      (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCOSLAR_UNION
 �ṹ˵��  : ETM_TRCOSLAR �Ĵ����ṹ���塣��ַƫ����:0x300����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM OS Lock�������ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  oslk     : 1;  /* bit[0]   : OS Lock key value:
                                                   0 Unlock the OS Lock.
                                                   1 Lock the OS Lock. */
        unsigned int  reserved : 31; /* bit[1-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCOSLAR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCOSLAR_oslk_START      (0)
#define SOC_CS_AP_ETM_ETM_TRCOSLAR_oslk_END        (0)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCOSLSR_UNION
 �ṹ˵��  : ETM_TRCOSLSR �Ĵ����ṹ���塣��ַƫ����:0x304����ֵ:0x0000000A�����:32
 �Ĵ���˵��: ETM OS Lock����״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  oslm_0   : 1;  /* bit[0]   : OS Lock model bit[0]. The value of this field is always 0b10, indicating that the OS Lock is implemented. */
        unsigned int  oslk     : 1;  /* bit[1]   : OS Lock status bit:
                                                   0 OS Lock is unlocked.
                                                   1 OS Lock is locked. */
        unsigned int  ntt      : 1;  /* bit[2]   : This bit is RAZ, that indicates that software must perform a 32-bit write to update the TRCOSLAR. */
        unsigned int  oslm_1   : 1;  /* bit[3]   : OS Lock model bit[1].  */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCOSLSR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_oslm_0_START    (0)
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_oslm_0_END      (0)
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_oslk_START      (1)
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_oslk_END        (1)
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_ntt_START       (2)
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_ntt_END         (2)
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_oslm_1_START    (3)
#define SOC_CS_AP_ETM_ETM_TRCOSLSR_oslm_1_END      (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCPDCR_UNION
 �ṹ˵��  : ETM_TRCPDCR �Ĵ����ṹ���塣��ַƫ����:0x310����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM�µ���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0: 3;  /* bit[0-2] : Reserved */
        unsigned int  pu       : 1;  /* bit[3]   : Powerup request, to request that power to the ETM trace unit and access to the trace registers is 
                                                   maintained:
                                                   0 Power not requested.
                                                   1 Power requested. */
        unsigned int  reserved_1: 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCPDCR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCPDCR_pu_START        (3)
#define SOC_CS_AP_ETM_ETM_TRCPDCR_pu_END          (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCPDSR_UNION
 �ṹ˵��  : ETM_TRCPDSR �Ĵ����ṹ���塣��ַƫ����:0x314����ֵ:0x00000021�����:32
 �Ĵ���˵��: ETM�µ�״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  power    : 1;  /* bit[0]   : Indicates the ETM trace unit is powered:
                                                   0 ETM trace unit is not powered. The trace registers are not accessible and they all return an error response.
                                                   1 ETM trace unit is powered. All registers are accessible. */
        unsigned int  stickypd : 1;  /* bit[1]   : Sticky power down state. 
                                                   0 Trace register power has not been removed since the TRCPDSR was last read.
                                                   1 Trace register power has been removed since the TRCPDSR was last read. */
        unsigned int  reserved_0: 3;  /* bit[2-4] : Reserved */
        unsigned int  oslk     : 1;  /* bit[5]   : OS lock status.
                                                   0 The OS Lock is unlocked.
                                                   1 The OS Lock is locked. */
        unsigned int  reserved_1: 26; /* bit[6-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCPDSR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCPDSR_power_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCPDSR_power_END       (0)
#define SOC_CS_AP_ETM_ETM_TRCPDSR_stickypd_START  (1)
#define SOC_CS_AP_ETM_ETM_TRCPDSR_stickypd_END    (1)
#define SOC_CS_AP_ETM_ETM_TRCPDSR_oslk_START      (5)
#define SOC_CS_AP_ETM_ETM_TRCPDSR_oslk_END        (5)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCACVRLm_UNION
 �ṹ˵��  : ETM_TRCACVRLm �Ĵ����ṹ���塣��ַƫ����:0x400+(8*(m))����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM��ַ�Ƚ�ֵ��λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  address_low : 32; /* bit[0-31]: The bit[31:0] address value to compare against. */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCACVRLm_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCACVRLm_address_low_START  (0)
#define SOC_CS_AP_ETM_ETM_TRCACVRLm_address_low_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCACVRHm_UNION
 �ṹ˵��  : ETM_TRCACVRHm �Ĵ����ṹ���塣��ַƫ����:0x404+(8*(m))����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM��ַ�Ƚ�ֵ��λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  address_hi : 32; /* bit[0-31]: The bit[63:32] address value to compare against. */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCACVRHm_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCACVRHm_address_hi_START  (0)
#define SOC_CS_AP_ETM_ETM_TRCACVRHm_address_hi_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCACATRm_UNION
 �ṹ˵��  : ETM_TRCACATRm �Ĵ����ṹ���塣��ַƫ����:0x480+(8*(m))����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM��ַ�Ƚ������ͼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  type        : 2;  /* bit[0-1]  : Controls what type of comparison the trace unit performs. This value is:
                                                       0b00 Instruction address, RES0. */
        unsigned int  contexttype : 2;  /* bit[2-3]  : Controls whether the trace unit performs a Context ID comparison, a VMID comparison, or both
                                                       comparisons. The possible values are:
                                                       0b00 The trace unit does not perform a Context ID comparison.
                                                       0b01 The trace unit performs a Context ID comparison using the Context ID comparator that the
                                                       CONTEXT field specifies, and signals a match if both the Context ID comparator matches and the address comparator match.
                                                       0b10 The trace unit performs a VMID comparison using the VMID comparator that the CONTEXT field specifies, and signals a match if both the VMID comparator and the address comparator
                                                       match.
                                                       0b11 The trace unit performs a Context ID comparison and a VMID comparison using the comparators that the CONTEXT field specifies, and signals a match if the Context ID
                                                       comparator matches, the VMID comparator matches, and the address comparator matches. */
        unsigned int  reserved_0  : 4;  /* bit[4-7]  : Reserved */
        unsigned int  exlevel_s   : 4;  /* bit[8-11] : Each bit controls whether a comparison can occur in Secure state for the corresponding Exception level.
                                                       The possible values are:
                                                       0 The trace unit can perform a comparison, in Secure state, for Exception level n.
                                                       1 The trace unit does not perform a comparison, in Secure state, for Exception level n.
                                                       The Exception levels are:
                                                       Bit[8] Exception level 0.
                                                       Bit[9] Exception level 1.
                                                       Bit[10] Always RES0.
                                                       Bit[11] Exception level 3. */
        unsigned int  exlevel_ns  : 4;  /* bit[12-15]: Each bit controls whether a comparison can occur in Non-secure state for the corresponding Exception
                                                       level. The possible values are:
                                                       0 The trace unit can perform a comparison, in Non-secure state, for Exception level n.
                                                       1 The trace unit does not perform a comparison, in Non-secure state, for Exception level n.
                                                       The exception levels are:
                                                       Bit[12] Exception level 0.
                                                       Bit[13] Exception level 1.
                                                       Bit[14] Exception level 2.
                                                       Bit[15] Always RES0. */
        unsigned int  reserved_1  : 16; /* bit[16-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCACATRm_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCACATRm_type_START         (0)
#define SOC_CS_AP_ETM_ETM_TRCACATRm_type_END           (1)
#define SOC_CS_AP_ETM_ETM_TRCACATRm_contexttype_START  (2)
#define SOC_CS_AP_ETM_ETM_TRCACATRm_contexttype_END    (3)
#define SOC_CS_AP_ETM_ETM_TRCACATRm_exlevel_s_START    (8)
#define SOC_CS_AP_ETM_ETM_TRCACATRm_exlevel_s_END      (11)
#define SOC_CS_AP_ETM_ETM_TRCACATRm_exlevel_ns_START   (12)
#define SOC_CS_AP_ETM_ETM_TRCACATRm_exlevel_ns_END     (15)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCIDCVR0_UNION
 �ṹ˵��  : ETM_TRCCIDCVR0 �Ĵ����ṹ���塣��ַƫ����:0x600����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM context ID�Ƚ�ֵ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  value : 32; /* bit[0-31]: Context ID value. */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCIDCVR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCIDCVR0_value_START  (0)
#define SOC_CS_AP_ETM_ETM_TRCCIDCVR0_value_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCVMIDCVR0_UNION
 �ṹ˵��  : ETM_TRCVMIDCVR0 �Ĵ����ṹ���塣��ַƫ����:0x640����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM VMID�Ƚ�ֵ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  value    : 8;  /* bit[0-7] : The VMID value. */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCVMIDCVR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCVMIDCVR0_value_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCVMIDCVR0_value_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCCIDCCTLR0_UNION
 �ṹ˵��  : ETM_TRCCIDCCTLR0 �Ĵ����ṹ���塣��ַƫ����:0x680����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM context ID�Ƚ������ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  comp0    : 4;  /* bit[0-3] : Controls the mask value that the trace unit applies to TRCCIDCVR0. Each bit in this field corresponds to a byte in TRCCIDCVR0. When a bit is:
                                                   0 The trace unit includes the relevant byte in TRCCIDCVR0 when it performs the Context ID comparison.
                                                   1 The trace unit ignores the relevant byte in TRCCIDCVR0 when it performs the Context ID comparison. */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCCIDCCTLR0_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCCIDCCTLR0_comp0_START     (0)
#define SOC_CS_AP_ETM_ETM_TRCCIDCCTLR0_comp0_END       (3)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCLAR_UNION
 �ṹ˵��  : ETM_TRCLAR �Ĵ����ṹ���塣��ַƫ����:0xFB0����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETM�������ʼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  key : 32; /* bit[0-31]: д��0xC5ACCE55���н�����Ȼ��ſ����������������Ĵ�����д�������κ�ֵ���ٴ������� */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCLAR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCLAR_key_START  (0)
#define SOC_CS_AP_ETM_ETM_TRCLAR_key_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCLSR_UNION
 �ṹ˵��  : ETM_TRCLSR �Ĵ����ṹ���塣��ַƫ����:0xFB4����ֵ:0x00000003�����:32
 �Ĵ���˵��: ETM����״̬�Ĵ�����
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
} SOC_CS_AP_ETM_ETM_TRCLSR_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCLSR_sli_START       (0)
#define SOC_CS_AP_ETM_ETM_TRCLSR_sli_END         (0)
#define SOC_CS_AP_ETM_ETM_TRCLSR_slk_START       (1)
#define SOC_CS_AP_ETM_ETM_TRCLSR_slk_END         (1)
#define SOC_CS_AP_ETM_ETM_TRCLSR_ntt_START       (2)
#define SOC_CS_AP_ETM_ETM_TRCLSR_ntt_END         (2)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_UNION
 �ṹ˵��  : ETM_TRCAUTHSTATUS �Ĵ����ṹ���塣��ַƫ����:0xFB8����ֵ:0x00000088�����:32
 �Ĵ���˵��: ETM��Ȩ״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nsid     : 2;  /* bit[0-1] : Always 0b00 Non-Secure invasive debug is not implemented */
        unsigned int  nsnid    : 2;  /* bit[2-3] : Non-secure Non-invasive Debug:
                                                   0b10 Non-secure Non-invasive Debug implemented but disabled, NIDEN=0.
                                                   0b11 Non-secure Non-invasive Debug implemented and enabled, NIDEN=1 */
        unsigned int  sid      : 2;  /* bit[4-5] : Always 0b00 Secure invasive debug is not implemented */
        unsigned int  snid     : 2;  /* bit[6-7] : Secure Non-invasive Debug:
                                                   0b10 Secure Non-invasive Debug implemented but disabled.
                                                   0b11 Secure Non-invasive Debug implemented and enabled. */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_UNION;
#endif
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_nsid_START      (0)
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_nsid_END        (1)
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_nsnid_START     (2)
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_nsnid_END       (3)
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_sid_START       (4)
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_sid_END         (5)
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_snid_START      (6)
#define SOC_CS_AP_ETM_ETM_TRCAUTHSTATUS_snid_END        (7)






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

#endif /* end of soc_cs_ap_etm_interface.h */
