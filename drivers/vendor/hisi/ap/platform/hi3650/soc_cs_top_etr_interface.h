/******************************************************************************

                 ��Ȩ���� (C), 2001-2015, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : soc_cs_top_etr_interface.h
  �� �� ��   : ����
  ��    ��   : Excel2Code
  ��������   : 2015-11-20 11:31:10
  ����޸�   :
  ��������   : �ӿ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��20��
    ��    ��   : l00352922
    �޸�����   : �ӡ�Hi3660V100 SOC�Ĵ����ֲ�_CS_TOP_ETR.xml���Զ�����

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#ifndef __SOC_CS_TOP_ETR_INTERFACE_H__
#define __SOC_CS_TOP_ETR_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/

/****************************************************************************
                     (1/1) TOP_ETR
 ****************************************************************************/
/* �Ĵ���˵����ETR RAM size�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_RSZ_UNION */
#define SOC_CS_TOP_ETR_ETR_RSZ_ADDR(base)             ((base) + (0x004))

/* �Ĵ���˵����ETR״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_STS_UNION */
#define SOC_CS_TOP_ETR_ETR_STS_ADDR(base)             ((base) + (0x00C))

/* �Ĵ���˵����ETR RAM�����ݼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_RRD_UNION */
#define SOC_CS_TOP_ETR_ETR_RRD_ADDR(base)             ((base) + (0x010))

/* �Ĵ���˵����ETR RAM��ָ��Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_RRP_UNION */
#define SOC_CS_TOP_ETR_ETR_RRP_ADDR(base)             ((base) + (0x014))

/* �Ĵ���˵����ETR RAMдָ��Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_RWP_UNION */
#define SOC_CS_TOP_ETR_ETR_RWP_ADDR(base)             ((base) + (0x018))

/* �Ĵ���˵����ETR trigger�������Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_TRG_UNION */
#define SOC_CS_TOP_ETR_ETR_TRG_ADDR(base)             ((base) + (0x01C))

/* �Ĵ���˵����ETR���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_CTL_UNION */
#define SOC_CS_TOP_ETR_ETR_CTL_ADDR(base)             ((base) + (0x020))

/* �Ĵ���˵����ETR RAMд���ݼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_RWD_UNION */
#define SOC_CS_TOP_ETR_ETR_RWD_ADDR(base)             ((base) + (0x024))

/* �Ĵ���˵����ETR ����ģʽ���üĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_MODE_UNION */
#define SOC_CS_TOP_ETR_ETR_MODE_ADDR(base)            ((base) + (0x028))

/* �Ĵ���˵����ETR buffer�������ˮ�߼Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_LBUFLEVEL_UNION */
#define SOC_CS_TOP_ETR_ETR_LBUFLEVEL_ADDR(base)       ((base) + (0x02C))

/* �Ĵ���˵����ETR buffer��ǰˮ�߼Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_CBUFLEVEL_UNION */
#define SOC_CS_TOP_ETR_ETR_CBUFLEVEL_ADDR(base)       ((base) + (0x030))

/* �Ĵ���˵����ETR bufferˮ�����üĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_BUFWM_UNION */
#define SOC_CS_TOP_ETR_ETR_BUFWM_ADDR(base)           ((base) + (0x034))

/* �Ĵ���˵����ETR RAM��ָ���λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_RRPHI_UNION */
#define SOC_CS_TOP_ETR_ETR_RRPHI_ADDR(base)           ((base) + (0x038))

/* �Ĵ���˵����ETR RAMдָ���λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_RWPHI_UNION */
#define SOC_CS_TOP_ETR_ETR_RWPHI_ADDR(base)           ((base) + (0x03C))

/* �Ĵ���˵����ETR AXI�ӿڿ��ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_AXICTL_UNION */
#define SOC_CS_TOP_ETR_ETR_AXICTL_ADDR(base)          ((base) + (0x110))

/* �Ĵ���˵����ETR����buffer��ַ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_DBALO_UNION */
#define SOC_CS_TOP_ETR_ETR_DBALO_ADDR(base)           ((base) + (0x118))

/* �Ĵ���˵����ETR����buffer��ַ��λ�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_DBAHO_UNION */
#define SOC_CS_TOP_ETR_ETR_DBAHO_ADDR(base)           ((base) + (0x11C))

/* �Ĵ���˵����ETR formatter and flush״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_FFSR_UNION */
#define SOC_CS_TOP_ETR_ETR_FFSR_ADDR(base)            ((base) + (0x300))

/* �Ĵ���˵����ETR formatter and flush���ƼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_FFCR_UNION */
#define SOC_CS_TOP_ETR_ETR_FFCR_ADDR(base)            ((base) + (0x304))

/* �Ĵ���˵����ETR Formatterͬ���������Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_FSCR_UNION */
#define SOC_CS_TOP_ETR_ETR_FSCR_ADDR(base)            ((base) + (0x308))

/* �Ĵ���˵����ETR�������ʼĴ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_LAR_UNION */
#define SOC_CS_TOP_ETR_ETR_LAR_ADDR(base)             ((base) + (0xFB0))

/* �Ĵ���˵����ETR����״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_LSR_UNION */
#define SOC_CS_TOP_ETR_ETR_LSR_ADDR(base)             ((base) + (0xFB4))

/* �Ĵ���˵����ETR��Ȩ״̬�Ĵ�����
   λ����UNION�ṹ:  SOC_CS_TOP_ETR_ETR_AUTHSTATUS_UNION */
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_ADDR(base)      ((base) + (0xFB8))





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
                     (1/1) TOP_ETR
 ****************************************************************************/
/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_RSZ_UNION
 �ṹ˵��  : ETR_RSZ �Ĵ����ṹ���塣��ַƫ����:0x004����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR RAM size�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rsz      : 31; /* bit[0-30]: trace buffer�Ĵ�С����32-bit wordΪ��λ����1KBʱ���Ĵ���ֵΪ0x00000100 */
        unsigned int  reserved : 1;  /* bit[31]  : Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_RSZ_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_RSZ_rsz_START       (0)
#define SOC_CS_TOP_ETR_ETR_RSZ_rsz_END         (30)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_STS_UNION
 �ṹ˵��  : ETR_STS �Ĵ����ṹ���塣��ַƫ����:0x00C����ֵ:0x0000000C�����:32
 �Ĵ���˵��: ETR״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  full      : 1;  /* bit[0]   : This flag is set when the RAM write pointer wraps around the top of the buffer, and remains set until the TraceCaptEn bit is cleared and set. */
        unsigned int  triggered : 1;  /* bit[1]   : The Triggered bit is set when trace capture is in progress and the TMC has detected a Trigger Event.  */
        unsigned int  tmcready  : 1;  /* bit[2]   : Trace capture has stopped and all internal pipelines and buffers have drained and AXI interface is not busy */
        unsigned int  ftempty   : 1;  /* bit[3]   : This bit is set when trace capture has stopped, and all internal pipelines and buffers have drained.  */
        unsigned int  empty     : 1;  /* bit[4]   : This bit indicates that the TMC does not contain any valid trace data in the trace memory.
                                                    This bit is valid only when TraceCaptEn is HIGH. This bit reads as zero when TraceCaptEn is LOW. */
        unsigned int  memerr    : 1;  /* bit[5]   : This bit indicates that an error has occurred on the AXI master interface. The error could be because of an error 
                                                    response from an AXI slave or because of the status of the Authentication interface inputs */
        unsigned int  reserved  : 26; /* bit[6-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_STS_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_STS_full_START       (0)
#define SOC_CS_TOP_ETR_ETR_STS_full_END         (0)
#define SOC_CS_TOP_ETR_ETR_STS_triggered_START  (1)
#define SOC_CS_TOP_ETR_ETR_STS_triggered_END    (1)
#define SOC_CS_TOP_ETR_ETR_STS_tmcready_START   (2)
#define SOC_CS_TOP_ETR_ETR_STS_tmcready_END     (2)
#define SOC_CS_TOP_ETR_ETR_STS_ftempty_START    (3)
#define SOC_CS_TOP_ETR_ETR_STS_ftempty_END      (3)
#define SOC_CS_TOP_ETR_ETR_STS_empty_START      (4)
#define SOC_CS_TOP_ETR_ETR_STS_empty_END        (4)
#define SOC_CS_TOP_ETR_ETR_STS_memerr_START     (5)
#define SOC_CS_TOP_ETR_ETR_STS_memerr_END       (5)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_RRD_UNION
 �ṹ˵��  : ETR_RRD �Ĵ����ṹ���塣��ַƫ����:0x010����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR RAM�����ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rrd : 32; /* bit[0-31]: Reads return data from Trace buffer */
    } reg;
} SOC_CS_TOP_ETR_ETR_RRD_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_RRD_rrd_START  (0)
#define SOC_CS_TOP_ETR_ETR_RRD_rrd_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_RRP_UNION
 �ṹ˵��  : ETR_RRP �Ĵ����ṹ���塣��ַƫ����:0x014����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR RAM��ָ��Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rrp : 32; /* bit[0-31]: The contents of this register represents the lower 32 bits of the 
                                              40-bit AXI address used to access trace memory.  */
    } reg;
} SOC_CS_TOP_ETR_ETR_RRP_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_RRP_rrp_START  (0)
#define SOC_CS_TOP_ETR_ETR_RRP_rrp_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_RWP_UNION
 �ṹ˵��  : ETR_RWP �Ĵ����ṹ���塣��ַƫ����:0x018����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR RAMдָ��Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rwp : 32; /* bit[0-31]: The contents of this register represents the lower 32 bits of the 
                                              40-bit AXI address used to access trace memory.  */
    } reg;
} SOC_CS_TOP_ETR_ETR_RWP_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_RWP_rwp_START  (0)
#define SOC_CS_TOP_ETR_ETR_RWP_rwp_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_TRG_UNION
 �ṹ˵��  : ETR_TRG �Ĵ����ṹ���塣��ַƫ����:0x01C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR trigger�������Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  trg : 32; /* bit[0-31]: This count represents the number of 32-bit words between a TRIGIN or trigger packet and a Trigger Event.  */
    } reg;
} SOC_CS_TOP_ETR_ETR_TRG_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_TRG_trg_START  (0)
#define SOC_CS_TOP_ETR_ETR_TRG_trg_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_CTL_UNION
 �ṹ˵��  : ETR_CTL �Ĵ����ṹ���塣��ַƫ����:0x020����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR���ƼĴ�����
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
} SOC_CS_TOP_ETR_ETR_CTL_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_CTL_tracecapten_START  (0)
#define SOC_CS_TOP_ETR_ETR_CTL_tracecapten_END    (0)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_RWD_UNION
 �ṹ˵��  : ETR_RWD �Ĵ����ṹ���塣��ַƫ����:0x024����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR RAMд���ݼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rwd : 32; /* bit[0-31]: Data written to this register is placed in the Trace RAM. */
    } reg;
} SOC_CS_TOP_ETR_ETR_RWD_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_RWD_rwd_START  (0)
#define SOC_CS_TOP_ETR_ETR_RWD_rwd_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_MODE_UNION
 �ṹ˵��  : ETR_MODE �Ĵ����ṹ���塣��ַƫ����:0x028����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR ����ģʽ���üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : Selects the operating mode. 
                                                   2/3 =Reserved 
                                                   1 = Software FIFO mode
                                                   0 = Circular Buffer mode  */
        unsigned int  reserved : 30; /* bit[2-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_MODE_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_MODE_mode_START      (0)
#define SOC_CS_TOP_ETR_ETR_MODE_mode_END        (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_LBUFLEVEL_UNION
 �ṹ˵��  : ETR_LBUFLEVEL �Ĵ����ṹ���塣��ַƫ����:0x02C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR buffer�������ˮ�߼Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lbuflevel : 32; /* bit[0-31]: Indicates the maximum fill level of the trace memory in 32-bit words since this register was last read. */
    } reg;
} SOC_CS_TOP_ETR_ETR_LBUFLEVEL_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_LBUFLEVEL_lbuflevel_START  (0)
#define SOC_CS_TOP_ETR_ETR_LBUFLEVEL_lbuflevel_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_CBUFLEVEL_UNION
 �ṹ˵��  : ETR_CBUFLEVEL �Ĵ����ṹ���塣��ַƫ����:0x030����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR buffer��ǰˮ�߼Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cbuflevel : 32; /* bit[0-31]: Indicates the current fill level of the trace memory in 32-bit words */
    } reg;
} SOC_CS_TOP_ETR_ETR_CBUFLEVEL_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_CBUFLEVEL_cbuflevel_START  (0)
#define SOC_CS_TOP_ETR_ETR_CBUFLEVEL_cbuflevel_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_BUFWM_UNION
 �ṹ˵��  : ETR_BUFWM �Ĵ����ṹ���塣��ַƫ����:0x034����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR bufferˮ�����üĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bufwm : 32; /* bit[0-31]: Indicates the required threshold vacancy level in 32-bit words in the trace memory */
    } reg;
} SOC_CS_TOP_ETR_ETR_BUFWM_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_BUFWM_bufwm_START  (0)
#define SOC_CS_TOP_ETR_ETR_BUFWM_bufwm_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_RRPHI_UNION
 �ṹ˵��  : ETR_RRPHI �Ĵ����ṹ���塣��ַƫ����:0x038����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR RAM��ָ���λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rrphi    : 8;  /* bit[0-7] : Bits [39:32] of the read pointer */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_RRPHI_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_RRPHI_rrphi_START     (0)
#define SOC_CS_TOP_ETR_ETR_RRPHI_rrphi_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_RWPHI_UNION
 �ṹ˵��  : ETR_RWPHI �Ĵ����ṹ���塣��ַƫ����:0x03C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR RAMдָ���λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rwphi    : 8;  /* bit[0-7] : Bits [39:32] of the write pointer */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_RWPHI_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_RWPHI_rwphi_START     (0)
#define SOC_CS_TOP_ETR_ETR_RWPHI_rwphi_END       (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_AXICTL_UNION
 �ṹ˵��  : ETR_AXICTL �Ĵ����ṹ���塣��ַƫ����:0x110����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR AXI�ӿڿ��ƼĴ�����
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
        unsigned int  reserved_1        : 20; /* bit[12-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_AXICTL_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_AXICTL_protctrlbit0_START       (0)
#define SOC_CS_TOP_ETR_ETR_AXICTL_protctrlbit0_END         (0)
#define SOC_CS_TOP_ETR_ETR_AXICTL_protctrlbit1_START       (1)
#define SOC_CS_TOP_ETR_ETR_AXICTL_protctrlbit1_END         (1)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit0_START      (2)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit0_END        (2)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit1_START      (3)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit1_END        (3)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit2_START      (4)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit2_END        (4)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit3_START      (5)
#define SOC_CS_TOP_ETR_ETR_AXICTL_cachectrlbit3_END        (5)
#define SOC_CS_TOP_ETR_ETR_AXICTL_scattergathermode_START  (7)
#define SOC_CS_TOP_ETR_ETR_AXICTL_scattergathermode_END    (7)
#define SOC_CS_TOP_ETR_ETR_AXICTL_wrburstlen_START         (8)
#define SOC_CS_TOP_ETR_ETR_AXICTL_wrburstlen_END           (11)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_DBALO_UNION
 �ṹ˵��  : ETR_DBALO �Ĵ����ṹ���塣��ַƫ����:0x118����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR����buffer��ַ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bufaddrlo : 32; /* bit[0-31]: ��Ϊtrace buffer��ϵͳ�ڴ�ĵ�32-bit��ַ */
    } reg;
} SOC_CS_TOP_ETR_ETR_DBALO_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_DBALO_bufaddrlo_START  (0)
#define SOC_CS_TOP_ETR_ETR_DBALO_bufaddrlo_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_DBAHO_UNION
 �ṹ˵��  : ETR_DBAHO �Ĵ����ṹ���塣��ַƫ����:0x11C����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR����buffer��ַ��λ�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bufaddrhi : 8;  /* bit[0-7] : Holds the upper eight bits of the 40-bit address used to locate the trace buffer in system memory */
        unsigned int  reserved  : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_DBAHO_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_DBAHO_bufaddrhi_START  (0)
#define SOC_CS_TOP_ETR_ETR_DBAHO_bufaddrhi_END    (7)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_FFSR_UNION
 �ṹ˵��  : ETR_FFSR �Ĵ����ṹ���塣��ַƫ����:0x300����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR formatter and flush״̬�Ĵ�����
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
} SOC_CS_TOP_ETR_ETR_FFSR_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_FFSR_flinprog_START   (0)
#define SOC_CS_TOP_ETR_ETR_FFSR_flinprog_END     (0)
#define SOC_CS_TOP_ETR_ETR_FFSR_ftstopped_START  (1)
#define SOC_CS_TOP_ETR_ETR_FFSR_ftstopped_END    (1)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_FFCR_UNION
 �ṹ˵��  : ETR_FFCR �Ĵ����ṹ���塣��ַƫ����:0x304����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR formatter and flush���ƼĴ�����
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
        unsigned int  reserved_3    : 18; /* bit[14-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_FFCR_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_FFCR_enft_START           (0)
#define SOC_CS_TOP_ETR_ETR_FFCR_enft_END             (0)
#define SOC_CS_TOP_ETR_ETR_FFCR_enti_START           (1)
#define SOC_CS_TOP_ETR_ETR_FFCR_enti_END             (1)
#define SOC_CS_TOP_ETR_ETR_FFCR_fonflin_START        (4)
#define SOC_CS_TOP_ETR_ETR_FFCR_fonflin_END          (4)
#define SOC_CS_TOP_ETR_ETR_FFCR_fontrig_START        (5)
#define SOC_CS_TOP_ETR_ETR_FFCR_fontrig_END          (5)
#define SOC_CS_TOP_ETR_ETR_FFCR_flushman_START       (6)
#define SOC_CS_TOP_ETR_ETR_FFCR_flushman_END         (6)
#define SOC_CS_TOP_ETR_ETR_FFCR_trigontrigin_START   (8)
#define SOC_CS_TOP_ETR_ETR_FFCR_trigontrigin_END     (8)
#define SOC_CS_TOP_ETR_ETR_FFCR_trigontrigevt_START  (9)
#define SOC_CS_TOP_ETR_ETR_FFCR_trigontrigevt_END    (9)
#define SOC_CS_TOP_ETR_ETR_FFCR_trigonfl_START       (10)
#define SOC_CS_TOP_ETR_ETR_FFCR_trigonfl_END         (10)
#define SOC_CS_TOP_ETR_ETR_FFCR_stoponfl_START       (12)
#define SOC_CS_TOP_ETR_ETR_FFCR_stoponfl_END         (12)
#define SOC_CS_TOP_ETR_ETR_FFCR_stopontrigevt_START  (13)
#define SOC_CS_TOP_ETR_ETR_FFCR_stopontrigevt_END    (13)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_FSCR_UNION
 �ṹ˵��  : ETR_FSCR �Ĵ����ṹ���塣��ַƫ����:0x308����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR Formatterͬ���������Ĵ�����
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
} SOC_CS_TOP_ETR_ETR_FSCR_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_FSCR_pscount_START   (0)
#define SOC_CS_TOP_ETR_ETR_FSCR_pscount_END     (4)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_LAR_UNION
 �ṹ˵��  : ETR_LAR �Ĵ����ṹ���塣��ַƫ����:0xFB0����ֵ:0x00000000�����:32
 �Ĵ���˵��: ETR�������ʼĴ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  key : 32; /* bit[0-31]: д��0xC5ACCE55���н�����Ȼ��ſ����������������Ĵ�����д�������κ�ֵ���ٴ������� */
    } reg;
} SOC_CS_TOP_ETR_ETR_LAR_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_LAR_key_START  (0)
#define SOC_CS_TOP_ETR_ETR_LAR_key_END    (31)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_LSR_UNION
 �ṹ˵��  : ETR_LSR �Ĵ����ṹ���塣��ַƫ����:0xFB4����ֵ:0x00000003�����:32
 �Ĵ���˵��: ETR����״̬�Ĵ�����
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
} SOC_CS_TOP_ETR_ETR_LSR_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_LSR_sli_START       (0)
#define SOC_CS_TOP_ETR_ETR_LSR_sli_END         (0)
#define SOC_CS_TOP_ETR_ETR_LSR_slk_START       (1)
#define SOC_CS_TOP_ETR_ETR_LSR_slk_END         (1)
#define SOC_CS_TOP_ETR_ETR_LSR_ntt_START       (2)
#define SOC_CS_TOP_ETR_ETR_LSR_ntt_END         (2)


/*****************************************************************************
 �ṹ��    : SOC_CS_TOP_ETR_ETR_AUTHSTATUS_UNION
 �ṹ˵��  : ETR_AUTHSTATUS �Ĵ����ṹ���塣��ַƫ����:0xFB8����ֵ:0x00000022�����:32
 �Ĵ���˵��: ETR��Ȩ״̬�Ĵ�����
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nsid     : 2;  /* bit[0-1] : Indicates the security level for non-secure invasive debug.
                                                   0x2 Functionality disabled. This return value occurs when DBGEN is LOW.
                                                   0x3 Functionality enabled. This return value occurs when DBGEN is HIGH. */
        unsigned int  nsnid    : 2;  /* bit[2-3] : Indicates non-secure non-invasive debug is not implemented. */
        unsigned int  sid      : 2;  /* bit[4-5] : Indicates the security level for secure invasive debug.
                                                   0x2 Functionality disabled. This return value occurs when SPIDEN is LOW or DBGEN is LOW.
                                                   0x3 Functionality enabled. This return value occurs when SPIDEN and DBGEN are HIGH. */
        unsigned int  snid     : 2;  /* bit[6-7] : Indicates secure non-invasive debug is not implemented. */
        unsigned int  reserved : 24; /* bit[8-31]: Reserved */
    } reg;
} SOC_CS_TOP_ETR_ETR_AUTHSTATUS_UNION;
#endif
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_nsid_START      (0)
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_nsid_END        (1)
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_nsnid_START     (2)
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_nsnid_END       (3)
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_sid_START       (4)
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_sid_END         (5)
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_snid_START      (6)
#define SOC_CS_TOP_ETR_ETR_AUTHSTATUS_snid_END        (7)






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

#endif /* end of soc_cs_top_etr_interface.h */
