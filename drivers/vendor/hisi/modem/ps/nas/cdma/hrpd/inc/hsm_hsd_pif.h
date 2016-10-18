/** ****************************************************************************

                    Huawei Technologies Sweden AB (C), 2001-2015

 ********************************************************************************
 * @author    Automatically generated by DAISY
 * @version
 * @date      2015-06-15
 * @file
 * @brief
 *
 * @copyright Huawei Technologies Sweden AB
 *******************************************************************************/
#ifndef HSM_HSD_PIF_H
#define HSM_HSD_PIF_H

/*******************************************************************************
 1. Other files included
*******************************************************************************/

#include "vos.h"
#include "cas_hrpd_airlinkmgmt_nas_pif.h"
#include "cas_hrpd_overheadmsg_nas_pif.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#pragma pack(4)

/*******************************************************************************
 2. Macro definitions
*******************************************************************************/

#define HSD_HSM_SECOND_COLORCODE_MAX_LENGTH                 ( 7 )
#define HSD_HSM_SUBNET_ID_MAX_LENGTH                        ( 16 )

/*******************************************************************************
 3. Enumerations declarations
*******************************************************************************/

/** ****************************************************************************
 * Name        : HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum HSM_HSD_PIF_MSG_TYPE_ENUM
{
    ID_HSD_HSM_START_REQ                                    = 0x0000, /**< @sa HSD_HSM_START_REQ_STRU */
    ID_HSM_HSD_START_CNF                                    = 0x0001, /**< @sa HSM_HSD_START_CNF_STRU */
    ID_HSD_HSM_POWEROFF_REQ                                 = 0x0002, /**< @sa HSD_HSM_POWEROFF_REQ_STRU */
    ID_HSM_HSD_POWEROFF_CNF                                 = 0x0003, /**< @sa HSM_HSD_POWEROFF_CNF_STRU */
    ID_HSD_HSM_POWER_SAVE_REQ                               = 0x0004, /**< @sa HSD_HSM_POWER_SAVE_REQ_STRU */
    ID_HSM_HSD_POWER_SAVE_CNF                               = 0x0005, /**< @sa HSM_HSD_POWER_SAVE_CNF_STRU */
    ID_HSD_HSM_SYSTEM_ACQUIRED_IND                          = 0x0006, /**< @sa HSD_HSM_SYSTEM_ACQUIRED_IND_STRU */
    ID_HSD_HSM_OVERHEAD_MSG_IND                             = 0x0007, /**< @sa HSD_HSM_OVERHEAD_MSG_IND_STRU */
    ID_HSD_HSM_OHM_NOT_CURRENT_IND                          = 0x0008, /**< @sa HSD_HSM_OHM_NOT_CURRENT_IND_STRU */
    ID_HSM_HSD_SESSION_NEG_RESULT_IND                       = 0x0009, /**< @sa HSM_HSD_SESSION_NEG_RESULT_IND_STRU */
    ID_HSM_HSD_SESSION_OPEN_OR_NEG_END_IND                  = 0x000A, /**< @sa HSM_HSD_SESSION_OPEN_OR_NEG_END_IND_STRU */
    ID_HSM_HSD_SESSION_OPEN_OR_NEG_START_IND                = 0x000B, /**< @sa HSM_HSD_SESSION_OPEN_OR_NEG_START_IND_STRU */
    ID_HSM_HSD_CAS_STATUS_IND                               = 0x000C, /**< @sa HSM_HSD_CAS_STATUS_IND_STRU */
    ID_HSM_HSD_CONN_OPEN_IND                                = 0x000D, /**< @sa HSM_HSD_CONN_OPEN_IND_STRU */
    ID_HSD_HSM_SUSPEND_IND                                  = 0x000E, /**< @sa HSD_HSM_SUSPEND_IND_STRU */
    ID_HSD_HSM_RESUME_IND                                   = 0x000F, /**< @sa HSD_HSM_RESUME_IND_STRU */
    ID_HSD_HSM_IRAT_FROM_LTE_IND                            = 0x0010, /**< @sa HSD_HSM_IRAT_FROM_LTE_IND_STRU */
    ID_HSD_HSM_IRAT_TO_LTE_IND                              = 0x0011, /**< @sa HSD_HSM_IRAT_TO_LTE_IND_STRU */
    ID_HSD_HSM_PILOT_SEARCH_FAIL_NTF                        = 0x0012, /**< @sa HSD_HSM_PILOT_SEARCH_FAIL_NTF_STRU */
    ID_HSM_HSD_PIF_MSG_TYPE_ENUM_BUTT                       = 0xFFFF
};
typedef VOS_UINT16 HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16;

/** ****************************************************************************
 * Name        : HSM_HSD_SESSION_NEG_RSLT_ENUM_UINT8
 *
 * Description :
 *******************************************************************************/
enum HSM_HSD_SESSION_NEG_RSLT_ENUM
{
    HSM_HSD_SESSION_NEG_RSLT_SUCCESS                        = 0x00,
    HSM_HSD_SESSION_NEG_RSLT_FAIL                           = 0x01,
    HSM_HSD_SESSION_NEG_RSLT_TIMEOUT                        = 0x02,
    HSM_HSD_SESSION_NEG_RSLT_SESSION_CLOSE                  = 0x03,
    HSM_HSD_SESSION_NEG_RSLT_ENUM_BUTT                      = 0x04
};
typedef VOS_UINT8 HSM_HSD_SESSION_NEG_RSLT_ENUM_UINT8;

/** ****************************************************************************
 * Name        : HSM_HSD_SESSION_RELEASE_TYPE_ENUM_UINT8
 *
 *******************************************************************************/
enum HSM_HSD_SESSION_RELEASE_TYPE_ENUM
{
    HSM_HSD_SESSION_RELEASE_TYPE_0                          = 0x00,
    HSM_HSD_SESSION_RELEASE_TYPE_A                          = 0x01,
    HSM_HSD_SESSION_RELEASE_TYPE_ENUM_BUTT                  = 0x02
};
typedef VOS_UINT8 HSM_HSD_SESSION_RELEASE_TYPE_ENUM_UINT8;

/** ****************************************************************************
 * Name        : HSM_HSD_HRPD_CAS_STATUS_ENUM_UINT16
 *
 * Description :
 *******************************************************************************/
enum HSM_HSD_HRPD_CAS_STATUS_ENUM
{
    HSM_HSD_HRPD_CAS_STATUS_NONE                            = 0x0000,
    HSM_HSD_HRPD_CAS_STATUS_INIT                            = 0x0001,
    HSM_HSD_HRPD_CAS_STATUS_IDLE                            = 0x0002,
    HSM_HSD_HRPD_CAS_STATUS_CONN                            = 0x0003,
    HSM_HSD_HRPD_CAS_STATUS_ENUM_BUTT                       = 0x0004
};
typedef VOS_UINT16 HSM_HSD_HRPD_CAS_STATUS_ENUM_UINT16;

/** ****************************************************************************
 * Name        : HSM_HSD_HRPD_CONN_OPEN_RSLT_ENUM_UINT8
 *
 *******************************************************************************/
enum HSM_HSD_HRPD_CONN_OPEN_RSLT_ENUM
{
    HSM_HSD_HRPD_CONN_OPEN_RSLT_SUCC                        = 0x00,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_FAIL                        = 0x01,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_PERSIST_FAIL                = 0x02,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_MAX_PROBE                   = 0x03,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_AUTH_BILLING_FAIL           = 0x04,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_BS_BUSY_OR_GENERAL          = 0x05,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_PREFER_CH_NOT_VALID         = 0x06,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_TCA_RTC_ACK_TIMEOUT         = 0x07,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_NO_RF                       = 0x08,
    HSM_HSD_HRPD_CONN_OPEN_RSLT_ENUM_BUTT                   = 0x09
};
typedef VOS_UINT8 HSM_HSD_HRPD_CONN_OPEN_RSLT_ENUM_UINT8;

/** ****************************************************************************
 * Name        : HSD_HSM_SYSTEM_ACQUIRE_TYPE_ENUM
 *
 *******************************************************************************/
enum HSD_HSM_SYSTEM_ACQUIRE_TYPE_ENUM
{
    HSD_HSM_SYSTEM_ACQUIRE_TYPE_NORMAL                      = 0x00,
    HSD_HSM_SYSTEM_ACQUIRE_TYPE_ABNORMAL                    = 0x01,

    HSD_HSM_SYSTEM_ACQUIRE_TYPE_BUTT                        = 0x02
};
typedef VOS_UINT8 HSD_HSM_SYSTEM_ACQUIRE_TYPE_ENUM_UINT8;

/*******************************************************************************
 4. Message Header declaration
*******************************************************************************/

/*******************************************************************************
 5. Message declaration
*******************************************************************************/

/*******************************************************************************
 6. STRUCT and UNION declaration
*******************************************************************************/

/*******************************************************************************
 7. OTHER declarations
*******************************************************************************/

/** ****************************************************************************
 * Name        : HSD_HSM_START_REQ_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_START_REQ_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_START_CNF_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSM_HSD_START_CNF_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_POWEROFF_REQ_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_POWEROFF_REQ_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_POWEROFF_CNF_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSM_HSD_POWEROFF_CNF_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_POWER_SAVE_REQ_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_POWER_SAVE_REQ_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_POWER_SAVE_CNF_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSM_HSD_POWER_SAVE_CNF_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_SYSTEM_ACQUIRED_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16                        enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                                              usOpId;
    HSD_HSM_SYSTEM_ACQUIRE_TYPE_ENUM_UINT8                  enSysAcqType;
    VOS_UINT8                                               aucResv[3];
    VOS_UINT32                                              aulSystemTime[2];
} HSD_HSM_SYSTEM_ACQUIRED_IND_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_OVERHEAD_MSG_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
    VOS_UINT16                          usBandClass;
    VOS_UINT16                          usFreq;
    VOS_UINT16                          usCountryCode;
    VOS_UINT8                           ucColorCode;
    VOS_UINT8                           ucSubNetMask;
    VOS_UINT8                           aucSectorId[HSD_HSM_SUBNET_ID_MAX_LENGTH];
    VOS_UINT8                           ucSecondaryColorCodeCount;
    VOS_UINT8                           aucSecondaryColorCode[HSD_HSM_SECOND_COLORCODE_MAX_LENGTH];
    VOS_INT32                           lLongitude;
    VOS_INT32                           lLatitude;
} HSD_HSM_OVERHEAD_MSG_IND_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_OHM_NOT_CURRENT_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_OHM_NOT_CURRENT_IND_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_SESSION_NEG_RESULT_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16                        enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                                              usOpId;
    HSM_HSD_SESSION_NEG_RSLT_ENUM_UINT8                     enNegResult;
    VOS_UINT8                                               ucIsNewSession;
    HSM_HSD_SESSION_RELEASE_TYPE_ENUM_UINT8                 enSessionRelType;
    VOS_UINT8                                               ucIsEhrpdSupport;   /* 表示当前Session类型是HRPD或EHRPD，当NegResult为success时有效 */
} HSM_HSD_SESSION_NEG_RESULT_IND_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_SESSION_OPEN_OR_NEG_END_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSM_HSD_SESSION_OPEN_OR_NEG_END_IND_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_SESSION_OPEN_OR_NEG_START_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSM_HSD_SESSION_OPEN_OR_NEG_START_IND_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_CAS_STATUS_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
    HSM_HSD_HRPD_CAS_STATUS_ENUM_UINT16 enCasStatus;
    VOS_UINT8                           aucReserved[2];
} HSM_HSD_CAS_STATUS_IND_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_CONN_OPEN_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16                        enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                                              usOpId;
    HSM_HSD_HRPD_CONN_OPEN_RSLT_ENUM_UINT8                  enRslt;
    VOS_UINT8                                               aucReserved[3];
} HSM_HSD_CONN_OPEN_IND_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_SUSPEND_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_SUSPEND_IND_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_RESUME_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_RESUME_IND_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_IRAT_FROM_LTE_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_IRAT_FROM_LTE_IND_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_IRAT_TO_LTE_IND_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_IRAT_TO_LTE_IND_STRU;

/** ****************************************************************************
 * Name        : HSD_HSM_PILOT_SEARCH_FAIL_NTF_STRU
 *
 * Description :
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;            /* _H2ASN_Skip */
    VOS_UINT16                          usOpId;
} HSD_HSM_PILOT_SEARCH_FAIL_NTF_STRU;

/** ****************************************************************************
 * Name        : HSM_HSD_PIF_MSG_DATA
 * Description : H2ASN top level message structure definition
 *******************************************************************************/
typedef struct
{
    HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16    enMsgId;     /* _H2ASN_MsgChoice_Export HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16 */
    VOS_UINT8                           aucMsgBlock[2];

    /* _H2ASN_MsgChoice_When_Comment HSM_HSD_PIF_MSG_TYPE_ENUM_UINT16 */
}HSM_HSD_PIF_MSG_DATA;

/* _H2ASN_Length UINT32*/
/** ****************************************************************************
 * Name        : HSM_HSD_PIF
 * Description : H2ASN top level message structure definition
 *******************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    HSM_HSD_PIF_MSG_DATA                stMsgData;
}HSM_HSD_PIF;


/*******************************************************************************
 8. Global  declaration
*******************************************************************************/

/*******************************************************************************
 9. Function declarations
*******************************************************************************/

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif
