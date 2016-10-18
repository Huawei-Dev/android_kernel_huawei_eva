/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : wal_linux_atcmdsrv.h
  �� �� ��   : ����
  ��    ��   : zhangheng
  ��������   : 2012��12��10��
  ����޸�   :
  ��������   : wal_linux_atcmdsrv.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��10��10��
    ��    ��   : zhangxiang
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __WAL_LINUX_ATCMDSRV_H__
#define __WAL_LINUX_ATCMDSRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "oal_ext_if.h"
#include "wlan_types.h"
#include "wlan_spec.h"
#include "mac_vap.h"
#include "hmac_ext_if.h"
#include "wal_ext_if.h"
#include "wal_config.h"
#include "dmac_alg_if.h"
#include "wal_linux_ioctl.h"
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_ATCMDSRV_H
/*****************************************************************************
  2 �궨��
*****************************************************************************/
#if ((_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102_HOST))
/*atcmdsrv˽������궨��*/
#define WAL_ATCMDSRV_IOCTL_DBB_LEN               12
#define WAL_ATCMDSRV_IOCTL_MAC_LEN               6
#define WAL_ATCMDSRV_IOCTL_VERIFY_CODE           1102
#define WAL_ATCMDSRV_IOCTL_MODE_NUM              12
#define WAL_ATCMDSRV_IOCTL_DATARATE_NUM          14
#define WAL_ATCMDSRB_IOCTL_AL_TX_LEN             2000
#define WAL_ATCMDSRB_DBB_NUM_TIME                (5 * OAL_TIME_HZ)
#define WAL_ATCMDSRB_CHECK_FEM_PA                (5 * OAL_TIME_HZ)
#define WAL_ATCMDSRB_GET_RX_PCKT                 (5 * OAL_TIME_HZ)
#define WAL_ATCMDSRV_GET_HEX_CHAR(tmp) (((tmp) > 9)? ((tmp-10) + 'A') : ((tmp) + '0'))
#define WLAN_HT_ONLY_MODE_2G                     WLAN_HT_ONLY_MODE + 3


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/*atcmdsrvö������*/
enum WAL_ATCMDSRV_IOCTL_CMD
{
    WAL_ATCMDSRV_IOCTL_CMD_WI_FREQ_SET=0,              /*  */
    WAL_ATCMDSRV_IOCTL_CMD_WI_POWER_SET,               /*  */
    WAL_ATCMDSRV_IOCTL_CMD_MODE_SET,                   /*  */
    WAL_ATCMDSRV_IOCTL_CMD_DATARATE_SET,
    WAL_ATCMDSRV_IOCTL_CMD_BAND_SET,
    WAL_ATCMDSRV_IOCTL_CMD_ALWAYS_TX_SET,
    WAL_ATCMDSRV_IOCTL_CMD_DBB_GET,
    WAL_ATCMDSRV_IOCTL_CMD_FEM_PA_STATUS_GET,
    WAL_ATCMDSRV_IOCTL_CMD_ALWAYS_RX_SET,
    WAL_ATCMDSRV_IOCTL_CMD_HW_ADDR_SET,
    WAL_ATCMDSRV_IOCTL_CMD_RX_PCKG_GET,
    WAL_ATCMDSRV_IOCTL_CMD_PM_SWITCH,
    WAL_ATCMDSRV_IOCTL_CMD_RX_RSSI,
    WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_SET,
    WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_RESULT,
    WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_TIME,
    WAL_ATCMDSRV_IOCTL_CMD_UART_LOOP_SET,
    WAL_ATCMDSRV_IOCTL_CMD_SDIO_LOOP_SET,
    HWIFI_IOCTL_CMD_TEST_BUTT


};

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
/* 1102 ʹ��atcmdsrv �·����� */
typedef struct wal_atcmdsrv_wifi_priv_cmd {
    /* У��λ,ȡֵ1102,������ƽ̨������ */
    oal_int32 l_verify;
    oal_int32   ul_cmd;                                          /* ����� */
    union
    {
    oal_int32 l_freq;
    oal_int32 l_pow;
    oal_int32 l_mode;
    oal_int32 l_datarate;
    oal_int32 l_bandwidth;
    oal_int32 l_awalys_tx;
    oal_int32 l_fem_pa_status;
    oal_int32 l_awalys_rx;
    oal_int32 l_rx_pkcg;
    oal_int32 l_pm_switch;
    oal_int32 l_rx_rssi;
    oal_int32 l_chipcheck_result;
    oal_uint64 l_chipcheck_time;
    oal_int32 l_uart_loop_set;
    oal_int32 l_sdio_loop_set;
    oal_uint8 auc_mac_addr[WAL_ATCMDSRV_IOCTL_MAC_LEN];
    oal_int8  auc_dbb[WAL_ATCMDSRV_IOCTL_DBB_LEN];

    }pri_data;

}wal_atcmdsrv_wifi_priv_cmd_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/

extern oal_int32 wal_atcmdsrv_wifi_priv_cmd(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, oal_int32 ul_cmd);
extern oal_void  wal_atcmsrv_ioctl_get_fem_pa_status(oal_net_device_stru *pst_net_dev, oal_int32 *pl_fem_pa_status);
extern oal_int32 wlan_device_mem_check(void);
extern oal_int32 wlan_device_mem_check_result(unsigned long long *time);
extern oal_int32 conn_test_uart_loop(char *param);
extern oal_int32 conn_test_sdio_loop(char *param);

#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of wal_linux_ioctl.h */

