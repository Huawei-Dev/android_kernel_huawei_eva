/******************************************************************************

                 ��Ȩ���� (C), 2001-2015, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : soc_cs_ap_etf_interface.h
  �� �� ��   : ����
  ��    ��   : Excel2Code
  ��������   : 2015-11-20 13:57:03
  ����޸�   :
  ��������   : �ӿ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��20��
    ��    ��   : l00352922
    �޸�����   : �ӡ�Hi3660V100 SOC�Ĵ����ֲ�_CS_AP_ETF.xml���Զ�����

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_CS_AP_ETF_INTERFACE_H__
#define __SOC_CS_AP_ETF_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/1) AP_ETF
 ****************************************************************************/
/* �Ĵ���˵����ETF RAM size�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_RSZ_UNION */
#define SOC_CS_AP_ETF_ETF_RSZ_ADDR(base)              ((base) + (0x004))

/* �Ĵ���˵����ETF״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_STS_UNION */
#define SOC_CS_AP_ETF_ETF_STS_ADDR(base)              ((base) + (0x00C))

/* �Ĵ���˵����ETF RAM�����ݼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_RRD_UNION */
#define SOC_CS_AP_ETF_ETF_RRD_ADDR(base)              ((base) + (0x010))

/* �Ĵ���˵����ETF RAM��ָ��Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_RRP_UNION */
#define SOC_CS_AP_ETF_ETF_RRP_ADDR(base)              ((base) + (0x014))

/* �Ĵ���˵����ETF RAMдָ��Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_RWP_UNION */
#define SOC_CS_AP_ETF_ETF_RWP_ADDR(base)              ((base) + (0x018))

/* �Ĵ���˵����ETF trigger�������Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_TRG_UNION */
#define SOC_CS_AP_ETF_ETF_TRG_ADDR(base)              ((base) + (0x01C))

/* �Ĵ���˵����ETF���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_CTL_UNION */
#define SOC_CS_AP_ETF_ETF_CTL_ADDR(base)              ((base) + (0x020))

/* �Ĵ���˵����ETF RAMд���ݼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_RWD_UNION */
#define SOC_CS_AP_ETF_ETF_RWD_ADDR(base)              ((base) + (0x024))

/* �Ĵ���˵����ETF ����ģʽ���üĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_MODE_UNION */
#define SOC_CS_AP_ETF_ETF_MODE_ADDR(base)             ((base) + (0x028))

/* �Ĵ���˵����ETF buffer�������ˮ�߼Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_LBUFLEVEL_UNION */
#define SOC_CS_AP_ETF_ETF_LBUFLEVEL_ADDR(base)        ((base) + (0x02C))

/* �Ĵ���˵����ETF buffer��ǰˮ�߼Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_CBUFLEVEL_UNION */
#define SOC_CS_AP_ETF_ETF_CBUFLEVEL_ADDR(base)        ((base) + (0x030))

/* �Ĵ���˵����ETF bufferˮ�����üĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_BUFWM_UNION */
#define SOC_CS_AP_ETF_ETF_BUFWM_ADDR(base)            ((base) + (0x034))

/* �Ĵ���˵����ETF formatter and flush״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_FFSR_UNION */
#define SOC_CS_AP_ETF_ETF_FFSR_ADDR(base)             ((base) + (0x300))

/* �Ĵ���˵����ETF formatter and flush���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_FFCR_UNION */
#define SOC_CS_AP_ETF_ETF_FFCR_ADDR(base)             ((base) + (0x304))

/* �Ĵ���˵����ETF Formatterͬ���������Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_FSCR_UNION */
#define SOC_CS_AP_ETF_ETF_FSCR_ADDR(base)             ((base) + (0x308))

/* �Ĵ���˵����ETF�������ʼĴ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_LAR_UNION */
#define SOC_CS_AP_ETF_ETF_LAR_ADDR(base)              ((base) + (0xFB0))

/* �Ĵ���˵����ETF����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_AP_ETF_ETF_LSR_UNION */
#define SOC_CS_AP_ETF_ETF_LSR_ADDR(base)              ((base) + (0xFB4))





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
                     (1/1) AP_ETF
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_RSZ_UNION
 �ṹ˵��  : ETF_RSZ �Ĵ����ṹ���塣��ַƫ����:0x004����ֵ:0x00001000�����:32
 �Ĵ���˵��: ETF RAM size�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rsz      : 31; /* bit[0-30]: ָʾTOP ETF trace buffer�Ĵ�СΪ16KB�� */
        unsigned int  reserved : 1;  /* bit[31]  : Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_RSZ_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_RSZ_rsz_START       (0)
#define SOC_CS_AP_ETF_ETF_RSZ_rsz_END         (30)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_STS_UNION
 �ṹ˵��  : ETF_STS �Ĵ����ṹ���塣��ַƫ����:0x00C����ֵ:0x0000000C�����:32
 �Ĵ���˵��: ETF״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  full      : 1;  /* bit[0]   : This bit can help to determine how much of the trace buffer contains valid data.
                                                    
                                                    Circular Buffer mode:
                                                    This flag is set when the RAM write pointer wraps around the top of the buffer, and remains set until the TraceCaptEn bit is cleared and set.
                                                    
                                                    Software FIFO mode and Hardware FIFO mode:
                                                    This flag indicates that the current space in the trace memory is less than or equal to the value programmed in the BUFWM Register, that is, Fill level >= MEM_SIZE - BUFWM.  */
        unsigned int  triggered : 1;  /* bit[1]   : The Triggered bit is set when trace capture is in progress and the TMC has detected a Trigger Event.  */
        unsigned int  tmcready  : 1;  /* bit[2]   : Trace capture has stopped and all internal pipelines and buffers have drained and AXI interface is not busy */
        unsigned int  ftempty   : 1;  /* bit[3]   : This bit is set when trace capture has stopped, and all internal pipelines and buffers have drained.  */
        unsigned int  empty     : 1;  /* bit[4]   : This bit indicates that the TMC does not contain any valid trace data in the trace memory.
                                                    This bit is valid only when TraceCaptEn is HIGH. This bit reads as zero when TraceCaptEn is LOW. */
        unsigned int  reserved  : 27; /* bit[5-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_STS_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_STS_full_START       (0)
#define SOC_CS_AP_ETF_ETF_STS_full_END         (0)
#define SOC_CS_AP_ETF_ETF_STS_triggered_START  (1)
#define SOC_CS_AP_ETF_ETF_STS_triggered_END    (1)
#define SOC_CS_AP_ETF_ETF_STS_tmcready_START   (2)
#define SOC_CS_AP_ETF_ETF_STS_tmcready_END     (2)
#define SOC_CS_AP_ETF_ETF_STS_ftempty_START    (3)
#define SOC_CS_AP_ETF_ETF_STS_ftempty_END      (3)
#define SOC_CS_AP_ETF_ETF_STS_empty_START      (4)
#define SOC_CS_AP_ETF_ETF_STS_empty_END        (4)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_RRD_UNION
 �ṹ˵��  : ETF_RRD �Ĵ����ṹ���塣��ַƫ����:0x010����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF RAM�����ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rrd : 32; /* bit[0-31]: Reads return data from Trace buffer */
    } reg;
} SOC_CS_AP_ETF_ETF_RRD_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_RRD_rrd_START  (0)
#define SOC_CS_AP_ETF_ETF_RRD_rrd_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_RRP_UNION
 �ṹ˵��  : ETF_RRP �Ĵ����ṹ���塣��ַƫ����:0x014����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF RAM��ָ��Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rrp      : 12; /* bit[0-11] : This value represents the location in trace memory that is accessed on a subsequent RRD read */
        unsigned int  reserved : 20; /* bit[12-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_RRP_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_RRP_rrp_START       (0)
#define SOC_CS_AP_ETF_ETF_RRP_rrp_END         (11)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_RWP_UNION
 �ṹ˵��  : ETF_RWP �Ĵ����ṹ���塣��ַƫ����:0x018����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF RAMдָ��Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rwp      : 12; /* bit[0-11] : This value represents the location in trace memory that are accessed on a subsequent write to the 
                                                    trace memory */
        unsigned int  reserved : 20; /* bit[12-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_RWP_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_RWP_rwp_START       (0)
#define SOC_CS_AP_ETF_ETF_RWP_rwp_END         (11)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_TRG_UNION
 �ṹ˵��  : ETF_TRG �Ĵ����ṹ���塣��ַƫ����:0x01C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF trigger�������Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trg : 32; /* bit[0-31]: This count represents the number of 32-bit words between a TRIGIN or trigger packet and a Trigger Event.  */
    } reg;
} SOC_CS_AP_ETF_ETF_TRG_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_TRG_trg_START  (0)
#define SOC_CS_AP_ETF_ETF_TRG_trg_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_CTL_UNION
 �ṹ˵��  : ETF_CTL �Ĵ����ṹ���塣��ַƫ����:0x020����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tracecapten : 1;  /* bit[0]   : Controls trace capture.  */
        unsigned int  reserved    : 31; /* bit[1-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_CTL_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_CTL_tracecapten_START  (0)
#define SOC_CS_AP_ETF_ETF_CTL_tracecapten_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_RWD_UNION
 �ṹ˵��  : ETF_RWD �Ĵ����ṹ���塣��ַƫ����:0x024����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF RAMд���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rwd : 32; /* bit[0-31]: Data written to this register is placed in the Trace RAM. */
    } reg;
} SOC_CS_AP_ETF_ETF_RWD_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_RWD_rwd_START  (0)
#define SOC_CS_AP_ETF_ETF_RWD_rwd_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_MODE_UNION
 �ṹ˵��  : ETF_MODE �Ĵ����ṹ���塣��ַƫ����:0x028����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF ����ģʽ���üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : Selects the operating mode. 
                                                   3 = Reserved 
                                                   2 = Hardware FIFO mode
                                                   1 = Software FIFO mode
                                                   0 = Circular Buffer mode  */
        unsigned int  reserved : 30; /* bit[2-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_MODE_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_MODE_mode_START      (0)
#define SOC_CS_AP_ETF_ETF_MODE_mode_END        (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_LBUFLEVEL_UNION
 �ṹ˵��  : ETF_LBUFLEVEL �Ĵ����ṹ���塣��ַƫ����:0x02C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF buffer�������ˮ�߼Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lbuflevel : 13; /* bit[0-12] : Indicates the maximum fill level of the trace memory in 32-bit words since this register was last 
                                                     read */
        unsigned int  reserved  : 19; /* bit[13-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_LBUFLEVEL_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_LBUFLEVEL_lbuflevel_START  (0)
#define SOC_CS_AP_ETF_ETF_LBUFLEVEL_lbuflevel_END    (12)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_CBUFLEVEL_UNION
 �ṹ˵��  : ETF_CBUFLEVEL �Ĵ����ṹ���塣��ַƫ����:0x030����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF buffer��ǰˮ�߼Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cbuflevel : 13; /* bit[0-12] : Indicates the current fill level of the trace memory in 32-bit words */
        unsigned int  reserved  : 19; /* bit[13-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_CBUFLEVEL_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_CBUFLEVEL_cbuflevel_START  (0)
#define SOC_CS_AP_ETF_ETF_CBUFLEVEL_cbuflevel_END    (12)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_BUFWM_UNION
 �ṹ˵��  : ETF_BUFWM �Ĵ����ṹ���塣��ַƫ����:0x034����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF bufferˮ�����üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  protctrlbit0      : 1;  /* bit[0]    : This bit controls the value driven on ARPROTM[0] or AWPROTM[0] on the AXI master interface when performing AXI transfers.
                                                             0 Normal access.
                                                             1 Privileged access. */
        unsigned int  protctrlbit1      : 1;  /* bit[1]    : This bit controls the value driven on ARPROTM[1] or AWPROTM[1] on the AXI master interface when performing AXI transfers.
                                                             0 Secure access.
                                                             1 Non-secure access. */
        unsigned int  cachectrlbit0     : 1;  /* bit[2]    : This bit controls the value driven on the ARCACHEM[0] or AWCACHEM[0] signal on the AXI master interface when performing AXI transfers.
                                                             0 Non-bufferable.
                                                             1 Bufferable. */
        unsigned int  cachectrlbit1     : 1;  /* bit[3]    : This bit controls the value driven on the ARCACHEM[1] or AWCACHEM[1] signal on the AXI master interface when performing AXI transfers.
                                                             0 Non-cacheable.
                                                             1 Cacheable. */
        unsigned int  cachectrlbit2     : 1;  /* bit[4]    : This bit controls the value driven on the ARCACHEM[2] or AWCACHEM[2] signal on the AXI master interface when performing AXI transfers. If CacheCtrlBit1 is LOW, this bit must also be LOW to comply with the AXI protocol. Setting this bit to HIGH when the CacheCtrlBit1 is LOW 
                                                             results in Unpredictable behavior.
                                                             0 Do not cache allocate on reads.
                                                             1 Cache allocate on reads. */
        unsigned int  cachectrlbit3     : 1;  /* bit[5]    : This bit controls the value driven on the ARCACHEM[3] or AWCACHEM[3] signal on the AXI master interface when performing AXI transfers. If CacheCtrlBit1 is LOW, this bit must also be LOW to comply with the AXI protocol. Setting this bit to HIGH when the CacheCtrlBit1 is LOW 
                                                             results in Unpredictable behavior.
                                                             0 Do not cache allocate on writes.
                                                             1 Cache allocate on writes. */
        unsigned int  reserved_0        : 1;  /* bit[6]    : Reserved */
        unsigned int  scattergathermode : 1;  /* bit[7]    : This bit indicates whether trace memory is accessed as a single buffer in system memory or as a linked-list based scatter-gather memory. This bit is ignored when in Disabled state.
                                                             0 Trace memory is a single contiguous block of system memory.
                                                             1 Trace memory is spread over multiple blocks of system memory based on a linked-list mechanism.  */
        unsigned int  wrburstlen        : 4;  /* bit[8-11] : This field indicates the maximum number of data transfers that can occur within each burst.
                                                             It is recommended that this value be set to no more than half the write buffer depth. Also, it is recommended that this value be set to enable an AXI burst of at least one frame of trace data.
                                                             0x0 One data transfer per burst. 
                                                             0x1 Maximum of two data transfers per burst.
                                                             ...
                                                             0xF Maximum of 16 data transfers per burst. */
        unsigned int  bufwm             : 1;  /* bit[12]   : Indicates the required threshold vacancy level in 32-bit words in the trace memory */
        unsigned int  reserved_1        : 19; /* bit[13-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_BUFWM_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_BUFWM_protctrlbit0_START       (0)
#define SOC_CS_AP_ETF_ETF_BUFWM_protctrlbit0_END         (0)
#define SOC_CS_AP_ETF_ETF_BUFWM_protctrlbit1_START       (1)
#define SOC_CS_AP_ETF_ETF_BUFWM_protctrlbit1_END         (1)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit0_START      (2)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit0_END        (2)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit1_START      (3)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit1_END        (3)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit2_START      (4)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit2_END        (4)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit3_START      (5)
#define SOC_CS_AP_ETF_ETF_BUFWM_cachectrlbit3_END        (5)
#define SOC_CS_AP_ETF_ETF_BUFWM_scattergathermode_START  (7)
#define SOC_CS_AP_ETF_ETF_BUFWM_scattergathermode_END    (7)
#define SOC_CS_AP_ETF_ETF_BUFWM_wrburstlen_START         (8)
#define SOC_CS_AP_ETF_ETF_BUFWM_wrburstlen_END           (11)
#define SOC_CS_AP_ETF_ETF_BUFWM_bufwm_START              (12)
#define SOC_CS_AP_ETF_ETF_BUFWM_bufwm_END                (12)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_FFSR_UNION
 �ṹ˵��  : ETF_FFSR �Ĵ����ṹ���塣��ַƫ����:0x300����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF formatter and flush״̬�Ĵ�����
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
        unsigned int  ftstopped : 1;  /* bit[1]   : This bit behaves in the same way as the FtEmpty bit in the STS Register, 0x00C */
        unsigned int  reserved  : 30; /* bit[2-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_FFSR_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_FFSR_flinprog_START   (0)
#define SOC_CS_AP_ETF_ETF_FFSR_flinprog_END     (0)
#define SOC_CS_AP_ETF_ETF_FFSR_ftstopped_START  (1)
#define SOC_CS_AP_ETF_ETF_FFSR_ftstopped_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_FFCR_UNION
 �ṹ˵��  : ETF_FFCR �Ĵ����ṹ���塣��ַƫ����:0x304����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF formatter and flush���ƼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  enft          : 1;  /* bit[0]    : If this bit is set, formatting is enabled.
                                                         0 Formatting disabled.
                                                         1 Formatting enabled. */
        unsigned int  enti          : 1;  /* bit[1]    : Setting this bit enables the insertion of triggers in the formatted trace stream.
                                                         0 Triggers are not embedded into the formatted trace stream.
                                                         1 Embed triggers into formatted trace stream. */
        unsigned int  reserved_0    : 2;  /* bit[2-3]  : Reserved */
        unsigned int  fonflin       : 1;  /* bit[4]    : Setting this bit enables the detection of transitions on the FLUSHIN input by the TMC. If this bit is set and the Formatter has not already stopped, a rising edge on FLUSHIN initiates a flush request.
                                                         0 Flush-on-FLUSHIN feature is disabled.
                                                         1 Flush-on-FLUSHIN feature is enabled. */
        unsigned int  fontrig       : 1;  /* bit[5]    : Setting this bit generates a flush when a Trigger Event occurs. If StopOnTrigEvt is set, this bit is ignored. 
                                                         0 Flush-on-trigger-event disabled.
                                                         1 Flush-on-trigger-event enabled. */
        unsigned int  flushman      : 1;  /* bit[6]    : Manually generate a flush of the system. Setting this bit causes a flush to be generated.
                                                         0 Manual flush is not initiated.
                                                         1 Manual flush is initiated. */
        unsigned int  reserved_1    : 1;  /* bit[7]    : Reserved */
        unsigned int  trigontrigin  : 1;  /* bit[8]    : Indicates a trigger when trigin is asserted.
                                                         0 Disable trigger indication when trigin is asserted.
                                                         1 Enable trigger indication when trigin is asserted. */
        unsigned int  trigontrigevt : 1;  /* bit[9]    : If this bit is set, a trigger is indicated on the output trace stream when a Trigger Event occurs. 
                                                         0 A trigger is not indicated on the trace stream when a Trigger Event occurs.
                                                         1 A trigger is indicated on the trace stream when a Trigger Event occurs. */
        unsigned int  trigonfl      : 1;  /* bit[10]   : If this bit is set, a trigger is indicated on the trace stream when a flush completes. 
                                                         0 A trigger is not indicated on the trace stream when a flush completes.
                                                         1 A trigger is indicated on the trace stream when a flush completes. */
        unsigned int  reserved_2    : 1;  /* bit[11]   : Reserved */
        unsigned int  stoponfl      : 1;  /* bit[12]   : If this bit is set, the formatter is stopped on completion of a flush operation.
                                                         0 Trace capture is not stopped when FLUSH is completed.
                                                         1 Trace capture is stopped when FLUSH is completed.  */
        unsigned int  stopontrigevt : 1;  /* bit[13]   : Stops the formatter after a trigger event is observed. Reset to disabled or 0.
                                                         0 Trace capture is not stopped when a Trigger Event is observed.
                                                         1 Trace capture is stopped when a Trigger Event is observed. */
        unsigned int  drainbuffer   : 1;  /* bit[14]   : This bit is used to enable draining of the trace data through the ATB master interface after the formatter has stopped.
                                                         0 Trace data is not drained through ATB master interface.
                                                         1 Trace data is drained through ATB master interface. */
        unsigned int  reserved_3    : 17; /* bit[15-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_FFCR_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_FFCR_enft_START           (0)
#define SOC_CS_AP_ETF_ETF_FFCR_enft_END             (0)
#define SOC_CS_AP_ETF_ETF_FFCR_enti_START           (1)
#define SOC_CS_AP_ETF_ETF_FFCR_enti_END             (1)
#define SOC_CS_AP_ETF_ETF_FFCR_fonflin_START        (4)
#define SOC_CS_AP_ETF_ETF_FFCR_fonflin_END          (4)
#define SOC_CS_AP_ETF_ETF_FFCR_fontrig_START        (5)
#define SOC_CS_AP_ETF_ETF_FFCR_fontrig_END          (5)
#define SOC_CS_AP_ETF_ETF_FFCR_flushman_START       (6)
#define SOC_CS_AP_ETF_ETF_FFCR_flushman_END         (6)
#define SOC_CS_AP_ETF_ETF_FFCR_trigontrigin_START   (8)
#define SOC_CS_AP_ETF_ETF_FFCR_trigontrigin_END     (8)
#define SOC_CS_AP_ETF_ETF_FFCR_trigontrigevt_START  (9)
#define SOC_CS_AP_ETF_ETF_FFCR_trigontrigevt_END    (9)
#define SOC_CS_AP_ETF_ETF_FFCR_trigonfl_START       (10)
#define SOC_CS_AP_ETF_ETF_FFCR_trigonfl_END         (10)
#define SOC_CS_AP_ETF_ETF_FFCR_stoponfl_START       (12)
#define SOC_CS_AP_ETF_ETF_FFCR_stoponfl_END         (12)
#define SOC_CS_AP_ETF_ETF_FFCR_stopontrigevt_START  (13)
#define SOC_CS_AP_ETF_ETF_FFCR_stopontrigevt_END    (13)
#define SOC_CS_AP_ETF_ETF_FFCR_drainbuffer_START    (14)
#define SOC_CS_AP_ETF_ETF_FFCR_drainbuffer_END      (14)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_FSCR_UNION
 �ṹ˵��  : ETF_FSCR �Ĵ����ṹ���塣��ַƫ����:0x308����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF Formatterͬ���������Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pscount  : 5;  /* bit[0-4] : Determines the reload value of the Synchronization Counter. The reload value takes effect the next time the 
                                                   counter reaches zero. Reads from this register return the reload value programmed into this register. This register is set to 0xA on reset, corresponding to a synchronization period of 1024 bytes.
                                                   0x0 Synchronization disabled.
                                                   0x1-0x6 Reserved.
                                                   0x7-0x1B Synchronization period is 2PSCount bytes. For example, a value of 0x7 gives a synchronization 
                                                   period of 128 bytes.
                                                   0x1C-0x1F Reserved. */
        unsigned int  reserved : 27; /* bit[5-31]: Reserved */
    } reg;
} SOC_CS_AP_ETF_ETF_FSCR_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_FSCR_pscount_START   (0)
#define SOC_CS_AP_ETF_ETF_FSCR_pscount_END     (4)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_LAR_UNION
 �ṹ˵��  : ETF_LAR �Ĵ����ṹ���塣��ַƫ����:0xFB0����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETF�������ʼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  key : 32; /* bit[0-31]: д��0xC5ACCE55���н�����Ȼ��ſ����������������Ĵ�����д�������κ�ֵ���ٴ������� */
    } reg;
} SOC_CS_AP_ETF_ETF_LAR_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_LAR_key_START  (0)
#define SOC_CS_AP_ETF_ETF_LAR_key_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_AP_ETF_ETF_LSR_UNION
 �ṹ˵��  : ETF_LSR �Ĵ����ṹ���塣��ַƫ����:0xFB4����ֵ:0x00000003�����:32
 �Ĵ���˵��: ETF����״̬�Ĵ�����
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
} SOC_CS_AP_ETF_ETF_LSR_UNION;
#endif
#define SOC_CS_AP_ETF_ETF_LSR_sli_START       (0)
#define SOC_CS_AP_ETF_ETF_LSR_sli_END         (0)
#define SOC_CS_AP_ETF_ETF_LSR_slk_START       (1)
#define SOC_CS_AP_ETF_ETF_LSR_slk_END         (1)
#define SOC_CS_AP_ETF_ETF_LSR_ntt_START       (2)
#define SOC_CS_AP_ETF_ETF_LSR_ntt_END         (2)






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

#endif /* end of soc_cs_ap_etf_interface.h */
