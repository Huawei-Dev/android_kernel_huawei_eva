/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7omB5iYtHEGPqMDW7TpB3q4yvDckVri1GoBL4gKI3k0M86wMC5hMD5fQLRyjt
hU2r3yQXcIINBcRSz0F8v8CbwCrzxgkEx9NK71P19fOZet+FWYGNLPKu/a3oZvUZ1trUHId3
somLKKCmElDY+x+9WKe9pi86BhtF6QfOHFEuNbom639gN8OyFwQSBBcI2+0hww==*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/***********************************************************************
*
* Copyright (c) 2009 HUAWEI - All Rights Reserved
*
* File: $real8_vfmw.c$
* Date: $2009/07/13$
* Revision: $v1.0$
* Purpose: real8 decoder file
*
*
* Change History:
*
* Date             Author            Change
* ====             ======            ====== 
* 2009/07/13       q45134            Original
*
* Dependencies:
*
************************************************************************/


#include    "syntax.h"
#include    "real8.h"
#include    "public.h"
#include    "bitstream.h"
#include    "vfmw.h"

#include    "vdm_hal.h"
#include    "postprocess.h"
#include    "vfmw_ctrl.h"
#include    "fsp.h"


#ifdef REAL8_ENABLE

#define RV8_CLEARREFFLAG do {                                       \
    FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->BwdFsID, 1);      \
    FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->FwdFsID, 1);      \
    pRv8CodecInfo->FwdFsID = 0;                                     \
    pRv8CodecInfo->BwdFsID = 0;                                     \
    pRv8CodecInfo->FwdFsID=pRv8CodecInfo->BwdFsID=FSP_GetNullLogicFs(pCtx->ChanID);\
} while(0)

#define RV8_CLEARREFNUM do {pRv8CodecInfo->RefNum=0;} while (0)


/* Bit stream field sizes */
#define FIELDLEN_PSC 22
static const UINT32 FIELDLEN_TR = 8;       /* temporal reference */
static const UINT32 FIELDLEN_TR_RV = 13;

static const UINT32 FIELDLEN_PTYPE = 13;   /* picture type */
static const UINT32 FIELDLEN_PTYPE_CONST = 2;
static const UINT32 FIELDLEN_PTYPE_SPLIT = 1;
static const UINT32 FIELDLEN_PTYPE_DOC = 1;
static const UINT32 FIELDLEN_PTYPE_RELEASE = 1;
static const UINT32 FIELDLEN_PTYPE_SRCFORMAT = 3;
static const UINT32 FIELDLEN_PTYPE_CODINGTYPE = 1;
static const UINT32 FIELDLEN_PTYPE_UMV = 1;
static const UINT32 FIELDLEN_PTYPE_AP = 1;
static const UINT32 FIELDLEN_PTYPE_PB = 1;

static const UINT32 FIELDLEN_PPTYPE_UFEP = 3;
static const UINT32 FIELDLEN_OPTIONAL_EPTYPE_RESERVED  =  3;
static const UINT32 FIELDLEN_MANDATORY_EPTYPE_RESERVED =  2;

static const UINT32 FIELDLEN_MPPTYPE_SRCFORMAT = 3;
static const UINT32 FIELDLEN_MPPTYPE_RPR = 1;
static const UINT32 FIELDLEN_MPPTYPE_RRU = 1;
static const UINT32 FIELDLEN_MPPTYPE_ROUNDING = 1;

static const UINT32 FIELDLEN_CSFMT_PARC = 4;
static const UINT32 FIELDLEN_CSFMT_FWI = 9;
static const UINT32 FIELDLEN_CSFMT_CONST = 1;
static const UINT32 FIELDLEN_CSFMT_FHI = 9;

static const UINT32 FIELDLEN_EPAR_WIDTH = 8;
static const UINT32 FIELDLEN_EPAR_HEIGHT = 8;

static const UINT32 FIELDLEN_PQUANT = 5;   /* picture quant value */
static const UINT32 FIELDLEN_CPM = 1;      /* continuous presence multipoint indicator */
static const UINT32 FIELDLEN_TRB = 3;      /* temporal reference for B frames */
static const UINT32 FIELDLEN_DBQUANT = 2;  /* B frame differential quant value */
static const UINT32 FIELDLEN_PSPARE = 8;   /* spare information */

#define FIELDLEN_GBSC 17    /* Group of blocks start code */
static const UINT32 FIELDLEN_GN = 5;       /* GOB number. */
static const UINT32 FIELDLEN_GLCI = 2; /* GOB logical channel indicator */
static const UINT32 FIELDLEN_GFID = 2; /* GOB Frame ID */
static const UINT32 FIELDLEN_GQUANT = 5;   /* GQUANT */
static const UINT32 FIELDLEN_SQUANT = 5;   /* SQUANT */

#define FIELDLEN_SSC 24         /* Slice start code */
#define FIELDLEN_SSC_RV8 17     /* Slice start code */

static const UINT32 FIELDLEN_WDA = 2;      /* RPR warping displacement accuracy */
static const UINT32 FIELDLEN_RPR_FILL_MODE = 2;
static const UINT32 FIELDLEN_RPR_COLOR = 8;

static const UINT32 FIELDLEN_PWIDTH_RV = 8;
static const UINT32 FIELDLEN_PHEIGHT_RV = 8;
/* Bit stream field values */

static const UINT32 FIELDVAL_PSC   = (0x00008000 >> (32 - FIELDLEN_PSC));
static const UINT32 FIELDVAL_SSC   = (0x1d1c105e >> (32 - FIELDLEN_SSC));
static const UINT32 FIELDVAL_SSC_RV8   = (0x00008000 >> (32 - FIELDLEN_SSC_RV8));
static const UINT32 FIELDVAL_GBSC = (0x00008000 >> (32 - FIELDLEN_GBSC));
static const UINT32 FIELDVAL_EPTYPE_RESERVED   =  0;

/* Bitstream Version Information */
/* */
/* Starting with RV_FID_REALVIDEO30, for RealVideo formats we embed the */
/* minor bitstream version number in the slice header. */
/* The encoder only ever produces one bitstream format.  But the decoder */
/* must be backwards compatible, and able to decode any prior minor */
/* bitstream version number. */
/* */
/* It is assumed that if the bitstream major version number changes, */
/* then a new RV_FID is introduced.  So, only the minor version number */
/* is present in the bitstream. */
/* */
/* The minor version number is encoded in the bitstream using 3 bits. */
/* RV89Combo's first bitstream minor version number is "2", which is encoded */
/* as all 0's.  The following table maps the bitstream value to the */
/* actual minor version number. */

static const UINT32 FIELDLEN_RV_BITSTREAM_VERSION = 3;
#define UNSUPPORTED_RV_BITSTREAM_VERSION 9999
#define ENCODERS_CURRENT_RV_BITSTREAM_VERSION 2
    /* This should match RV_BITSTREAM_MINOR_VERSION in "hiverv.h" */

static const UINT32 s_RVVersionEncodingToMinorVersion[8] =
{
    2,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION
};

#define NUMBER_OF_RV_BITSTREAM_VERSIONS \
                   (sizeof(s_RVVersionEncodingToMinorVersion) \
                   / sizeof(s_RVVersionEncodingToMinorVersion[0]))


UINT32 Real8_CB_GetBits(BS *pBs, UINT32 nBits)
{
    UINT32 Data;

    /* make sure that the number of bits given is <= 25 */
    
    Data = BsShow(pBs, nBits);
    BsSkip(pBs, nBits);

    return Data;
}


UINT32 Real8_CB_Get1Bit(BS *pBs)
{
    UINT32 Data;
    
    Data = BsShow(pBs, 1);
    BsSkip(pBs, 1);

    return Data;
}


/* gs_VLCDecodeTable maps the leading 8-bits of the unread bitstream */
/* into the length and info of the VLC code at the head of those 8 bits. */
/* The table's contents can be summarized as follows, where p, q, r, s */
/* and '*' are arbitrary binary digits {0, 1}. */
/* */
/*  8 Bits      Length  Info */
/*  ========    ======  ==== */
/*  0p0q0r0s    8       pqrs combined with VLC at head of next 8 bits */
/*  0p0q0r1*    7       pqr */
/*  0p0q1***    5       pq */
/*  0p1*****    3       p */
/*  1*******    1       0 */
/* */
/* Note that when the length is 8, or even, we need to read another 8 bits */
/* from the bitstream, look up these 8 bits in the same table, and combine */
/* the length and info with the previously decoded bits. */
/* */
/* For a given 8-bit chunk of bitstream bits, the maximum length is 8, */
/* and the maximum info value is contained in 4 bits.  Thus, we can pack */
/* the length and info in a U8.  The info is in the least significant */
/* 4 bits, and the length is in the most significant 4 bits. */
#define PACK_LENGTH_AND_INFO(length, info) (((length) << 4) | (info))

static const UINT8 gs_VLCDecodeTable[256] =
{
    PACK_LENGTH_AND_INFO(8,  0),   /* 00000000 */
    PACK_LENGTH_AND_INFO(8,  1),   /* 00000001 */
    PACK_LENGTH_AND_INFO(7,  0),   /* 00000010 */
    PACK_LENGTH_AND_INFO(7,  0),   /* 00000011 */
    PACK_LENGTH_AND_INFO(8,  2),   /* 00000100 */
    PACK_LENGTH_AND_INFO(8,  3),   /* 00000101 */
    PACK_LENGTH_AND_INFO(7,  1),   /* 00000110 */
    PACK_LENGTH_AND_INFO(7,  1),   /* 00000111 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001000 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001001 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001010 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001011 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001100 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001101 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001110 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001111 */
    PACK_LENGTH_AND_INFO(8,  4),   /* 00010000 */
    PACK_LENGTH_AND_INFO(8,  5),   /* 00010001 */
    PACK_LENGTH_AND_INFO(7,  2),   /* 00010010 */
    PACK_LENGTH_AND_INFO(7,  2),   /* 00010011 */
    PACK_LENGTH_AND_INFO(8,  6),   /* 00010100 */
    PACK_LENGTH_AND_INFO(8,  7),   /* 00010101 */
    PACK_LENGTH_AND_INFO(7,  3),   /* 00010110 */
    PACK_LENGTH_AND_INFO(7,  3),   /* 00010111 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011000 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011001 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011010 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011011 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011100 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011101 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011110 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111111 */
    PACK_LENGTH_AND_INFO(8,  8),   /* 01000000 */
    PACK_LENGTH_AND_INFO(8,  9),   /* 01000001 */
    PACK_LENGTH_AND_INFO(7,  4),   /* 01000010 */
    PACK_LENGTH_AND_INFO(7,  4),   /* 01000011 */
    PACK_LENGTH_AND_INFO(8, 10),   /* 01000100 */
    PACK_LENGTH_AND_INFO(8, 11),   /* 01000101 */
    PACK_LENGTH_AND_INFO(7,  5),   /* 01000110 */
    PACK_LENGTH_AND_INFO(7,  5),   /* 01000111 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001000 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001001 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001010 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001011 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001100 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001101 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001110 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001111 */
    PACK_LENGTH_AND_INFO(8, 12),   /* 01010000 */
    PACK_LENGTH_AND_INFO(8, 13),   /* 01010001 */
    PACK_LENGTH_AND_INFO(7,  6),   /* 01010010 */
    PACK_LENGTH_AND_INFO(7,  6),   /* 01010011 */
    PACK_LENGTH_AND_INFO(8, 14),   /* 01010100 */
    PACK_LENGTH_AND_INFO(8, 15),   /* 01010101 */
    PACK_LENGTH_AND_INFO(7,  7),   /* 01010110 */
    PACK_LENGTH_AND_INFO(7,  7),   /* 01010111 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011000 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011001 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011010 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011011 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011100 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011101 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011110 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111110 */
    PACK_LENGTH_AND_INFO(1,  0)    /* 11111111 */
};


UINT32 Real8_CB_GetVLCBits(BS *pBs, UINT32 *pInfo)
{
    UINT32 bits, info = 0, length = 0;
    UINT32 lengthAndInfo;
    UINT32 thisChunksLength;

    do
    {
        /* Extract the leading 8-bits into 'bits' */    
        bits = BsShow(pBs, 8);
    
        lengthAndInfo = gs_VLCDecodeTable[bits];
        thisChunksLength = lengthAndInfo >> 4;

        info = (info << (thisChunksLength >> 1)) | (lengthAndInfo & 0xf);
        length += thisChunksLength;

        BsSkip(pBs, thisChunksLength);  
    }
    while (!(length & 1));    /* We're done when length is an odd number */

    *pInfo = info;

    return length;
}


/*!
************************************************************************
*    ����ԭ�� :  Real8_GetImageBuffer()
*    �������� :  ȡһ��֡��
*    ����˵�� :  **p_vobuf ����ָ��ǰ����֡���ָ��ĵ�ַ
*    ����ֵ   :  0: �޷��ҵ�����֡��;1: �ܹ��ҵ�����֡��
************************************************************************
*/
UINT32 Real8_GetImageBuffer(RV8_CTX_S *pCtx)
{
    SINT32 ret = RV8_VFMW_FALSE;
    RV8_CODECINF *pRv8CodecInfo;
    RV8_VOBUFINF *pVOBufInf;

    pRv8CodecInfo = &pCtx->CodecInfo;
    pVOBufInf = pCtx->CodecInfo.VOBufInf;

    pRv8CodecInfo->CurFsID = FSP_NewLogicFs(pCtx->ChanID, 1);
    if (pRv8CodecInfo->CurFsID < 0)
    {
        /* ��ȡ֡��ʧ�� */
        dprint(PRN_FATAL,"get frame store fail!\n");
        RV8_CLEARREFFLAG;
        RV8_CLEARREFNUM;		
		#if 0
		ResetVoQueue(&pCtx->ImageQue);
		FSP_EmptyInstance(pCtx->ChanID);
		#else
		FSP_ClearNotInVoQueue(pCtx->ChanID, &(pCtx->ImageQue));
		#endif

        return 0;
    }

    ret = RV8_VFMW_TRUE;
    {
        FSP_LOGIC_FS_S *pLf = FSP_GetLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID);
        if (NULL == pLf)
        {
            dprint(PRN_DBG, "line: %d, pImage is NULL!\n", __LINE__);
            return RV8_VFMW_FALSE;
        }

        dprint(PRN_PIC, "get image buffer ok: LogicFsID = %d\n", pRv8CodecInfo->CurFsID);               
        if ( (NULL!=pLf->pstDecodeFs) && (NULL!=pLf->pstDispOutFs) && (NULL!=pLf->pstTfOutFs) )
        {
            dprint(PRN_PIC, "decode %p, disp %p, tf %p\n", pLf->pstDecodeFs->PhyAddr, pLf->pstDispOutFs->PhyAddr, pLf->pstTfOutFs->PhyAddr);
        }
        if(pRv8CodecInfo->CurFsID == 2)
        {
            pRv8CodecInfo->CurFsID = pRv8CodecInfo->CurFsID;
        }
    }

    return ret;
}


/*!
************************************************************************
*    ����ԭ�� :  Real8_ArrangeVHBMem()
*    �������� :  ������Ϣ�غ�֡��ռ䲢��ʼ������֡��Ķ���
*    ����˵�� :  ImageNum ��Ҫ�����֡��
*    ����ֵ   :  0: ����ʧ�� 1: ����ɹ�
************************************************************************
*/
UINT32 Real8_ArrangeVHBMem(SINT32 ImageNum, RV8_CTX_S *pCtx, RV8_PictureHeader *pPictureHeader)
{
    //SINT32 i;
    //UINT32 SlotWidth;
    //UINT32 BankOfst;
    //UINT32 ImgWidth, ImgHeight;

    RV8_CODECINF *pRv8CodecInfo;
    FSP_INST_CFG_S  FspInstCfg;
    SINT32 PicWidth, PicHeight, DecFsNum, DispFsNum, PmvNum;

    pRv8CodecInfo = &pCtx->CodecInfo;

    if( pPictureHeader->pic_width_in_mb < 2 || pPictureHeader->pic_height_in_mb < 2 )
    {
        dprint(PRN_FATAL, "image size abnormal (%dx%d) in MB\n", pPictureHeader->pic_width_in_mb, pPictureHeader->pic_height_in_mb);
        return RV8_VFMW_FALSE;
    }
	
    if ((pCtx->pstExtraData->stChanOption.s32ReRangeEn == 0)&&(pCtx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION))
    {
        if ((pPictureHeader->pic_width_in_pixel) > pCtx->pstExtraData->stChanOption.s32MaxWidth || (pPictureHeader->pic_height_in_pixel) > pCtx->pstExtraData->stChanOption.s32MaxHeight)
        {
            dprint(PRN_FATAL, "real8 actual frame size(%dx%d) exeed max config(%dx%d)\n", pPictureHeader->pic_width_in_pixel, pPictureHeader->pic_height_in_pixel,
				pCtx->pstExtraData->stChanOption.s32MaxWidth, pCtx->pstExtraData->stChanOption.s32MaxHeight);

			return RV8_VFMW_FALSE;
		}

        /* ��������£�ͼ��ߴ�̶��ذ�������Ϣ�� */
        PicWidth  = pCtx->pstExtraData->stChanOption.s32MaxWidth;
        PicHeight = pCtx->pstExtraData->stChanOption.s32MaxHeight;
		
        if (pRv8CodecInfo->Imgbuf_allocated == RV8_VFMW_TRUE)
        {
            return RV8_VFMW_TRUE;
		}
	    /*Ϊ�˾����ܵؾ�ȷ������֡����������չ�ʽ ��֡�����= s32MaxRefFrameNum + s32DisplayFrameNum + 1����ǰ����Ϊ...+2�����ǵ�ǰֻ֡ռһ���Ϳ�����*/		
		DecFsNum = pCtx->pstExtraData->stChanOption.s32MaxRefFrameNum + PLUS_FS_NUM;
        if (0 == pCtx->pstExtraData->stChanOption.s32SupportBFrame)
        {
            PmvNum = 1;
        }
        else
        {
            PmvNum = pCtx->pstExtraData->stChanOption.s32MaxRefFrameNum + 1;
			PmvNum = MIN(PmvNum,17);
        }		
		DispFsNum = pCtx->pstExtraData->stChanOption.s32DisplayFrameNum;
	}	
	else if (pCtx->pstExtraData->eCapLevel != CAP_LEVEL_USER_DEFINE_WITH_OPTION)
	{
        PicWidth  = pPictureHeader->pic_width_in_pixel;
        PicHeight = pPictureHeader->pic_height_in_pixel;	
		DecFsNum = 4;
		PmvNum = 2;
		DispFsNum = 5;		
	}
	else
	{
        if ((pPictureHeader->pic_width_in_pixel) > pCtx->pstExtraData->stChanOption.s32MaxWidth || (pPictureHeader->pic_height_in_pixel) > pCtx->pstExtraData->stChanOption.s32MaxHeight)
        {
            dprint(PRN_FATAL, "real8 actual frame size(%dx%d) exeed max config(%dx%d)\n", (pPictureHeader->pic_width_in_pixel), (pPictureHeader->pic_height_in_pixel),
				pCtx->pstExtraData->stChanOption.s32MaxWidth, pCtx->pstExtraData->stChanOption.s32MaxHeight);

			return RV8_VFMW_FALSE;
		}
        PicWidth  = (pPictureHeader->pic_width_in_pixel);
        PicHeight = (pPictureHeader->pic_height_in_pixel);
	    /*Ϊ�˾����ܵؾ�ȷ������֡����������չ�ʽ ��֡�����= s32MaxRefFrameNum + s32DisplayFrameNum + 1����ǰ����Ϊ...+2�����ǵ�ǰֻ֡ռһ���Ϳ�����*/		
		DecFsNum = pCtx->pstExtraData->stChanOption.s32MaxRefFrameNum + PLUS_FS_NUM;
        if (0 == pCtx->pstExtraData->stChanOption.s32SupportBFrame)
        {
            PmvNum = 1;
        }
        else
        {
            PmvNum = pCtx->pstExtraData->stChanOption.s32MaxRefFrameNum + 1;
			PmvNum = MIN(PmvNum,17);
        }		
		DispFsNum = pCtx->pstExtraData->stChanOption.s32DisplayFrameNum;
	}

    memset(&FspInstCfg, 0, sizeof(FSP_INST_CFG_S));
    FspInstCfg.s32DecFsWidth        = PicWidth;
    FspInstCfg.s32DecFsHeight       = PicHeight;
    FspInstCfg.s32ExpectedDecFsNum  = (pCtx->pstExtraData->eCapLevel == CAP_LEVEL_SINGLE_IFRAME_FHD)? 0: DecFsNum;
    FspInstCfg.s32ExpectedDispFsNum = DispFsNum;
    FspInstCfg.s32ExpectedPmvNum    = (pCtx->pstExtraData->eCapLevel == CAP_LEVEL_SINGLE_IFRAME_FHD)? 1: PmvNum;
	
    /* ����FSPʵ�������ָ�֡�� */
    if (FSP_OK == FSP_ConfigInstance(pCtx->ChanID, &FspInstCfg))
    {
        SINT32 Ret, UsedMemSize;
        Ret = FSP_PartitionFsMemory(pCtx->ChanID, pCtx->pstExtraData->s32SyntaxMemAddr, 
              pCtx->pstExtraData->s32SyntaxMemSize, &UsedMemSize);
        if (Ret != FSP_OK)
        {
            // �ָ�֡��ʧ�ܣ�����
            return RV8_VFMW_FALSE;
        }
    }
    else
    {
        return RV8_VFMW_FALSE;
    }
    pRv8CodecInfo->Imgbuf_allocated = RV8_VFMW_TRUE;
	
    return RV8_VFMW_TRUE;
}


SINT32 Real8_CB_GetCPFMT(BS *pBs, RV8_PictureHeader *pPictureHeader)
{
    pPictureHeader->pixel_aspect_ratio = Real8_CB_GetBits(pBs, FIELDLEN_CSFMT_PARC);
    pPictureHeader->pic_width_in_pixel = (Real8_CB_GetBits(pBs, FIELDLEN_CSFMT_FWI) + 1) << 2;

    /* Bit 14 must be "1" to prevent start code emulation */
    if (Real8_CB_Get1Bit(pBs) != 1)
    {
        return RV8_VFMW_FALSE;
    }
    /* Frame height indication */
    /* The number of lines is given by FHI*4 */
    pPictureHeader->pic_height_in_pixel = Real8_CB_GetBits(pBs, FIELDLEN_CSFMT_FHI) << 2;

    return RV8_VFMW_TRUE;
}


/* --------------------------------------------------------------------------- */
/*  Real8_CB_SetDimensions() */
/*      use in the decoder to initialize size, m_nGOBs, m_nMBs, and m_mbaSize */
/* --------------------------------------------------------------------------- */
/* for PutSliceHeader              SQCIF, QCIF, CIF, 4CIF, 16CIF, 2048x1152 */
static const UINT32 MBA_NumMBs[]     = { 47,   98, 395, 1583,  6335, 9215 };
static const UINT32 MBA_FieldWidth[] = {  6,    7,   9,   11,    13,   14 };
static const UINT32 MBA_Widths = sizeof( MBA_FieldWidth ) / sizeof( MBA_FieldWidth[0] );

VOID Real8_CB_SetDimensions(RV8_PictureHeader *pPictureHeader)
{
    UINT32 i, j;
    RV8_ParsePicParam *pRv8ParsePicParam = &pPictureHeader->Rv8ParsePicParam;
    /* Set m_nMBs and m_nGOBs based on the image dimensions. */
    /* TBD, there's probably a formula that can be used here instead */
    /* of this nested if-then-else sequence. */

    if (pPictureHeader->pic_width_in_pixel == 128 && pPictureHeader->pic_height_in_pixel == 96)
    {
        pRv8ParsePicParam->m_nMBs   = 8;
        pRv8ParsePicParam->m_nGOBs  = 6;
    }
    else if (pPictureHeader->pic_width_in_pixel == 176 && pPictureHeader->pic_height_in_pixel == 144)
    {
        pRv8ParsePicParam->m_nMBs   = 11;
        pRv8ParsePicParam->m_nGOBs  = 9;
    }
    else if (pPictureHeader->pic_width_in_pixel == 352 && pPictureHeader->pic_height_in_pixel == 288)
    {
        pRv8ParsePicParam->m_nMBs   = 22;
        pRv8ParsePicParam->m_nGOBs  = 18;
    }
    else if (pPictureHeader->pic_width_in_pixel == 704 && pPictureHeader->pic_height_in_pixel == 576)
    {
        pRv8ParsePicParam->m_nMBs   = 44;
        pRv8ParsePicParam->m_nGOBs  = 36;
    }
    else if (pPictureHeader->pic_width_in_pixel == 1408 && pPictureHeader->pic_height_in_pixel == 1152)
    {
        pRv8ParsePicParam->m_nMBs   = 88;
        pRv8ParsePicParam->m_nGOBs  = 72;
    }
    else
    {
        pRv8ParsePicParam->m_nMBs  = (pPictureHeader->pic_width_in_pixel + 15) >> 4;
        pRv8ParsePicParam->m_nGOBs = (pPictureHeader->pic_height_in_pixel + 15) >> 4;
    }

    /* initialize m_mbaSize */

    j = pRv8ParsePicParam->m_nGOBs * pRv8ParsePicParam->m_nMBs - 1;
    for (i = 0; i < (MBA_Widths - 1) && MBA_NumMBs[i] <  j; i++);

    pRv8ParsePicParam->m_mbaSize = MBA_FieldWidth[i];
}


/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::GetPicSize() */
/*  Gets the picture size from the bitstream. */
/* --------------------------------------------------------------------------- */
VOID Real8_CB_GetPicSize(BS *pBs, RV8_ParsePicParam *pRv8ParsePicParam, UINT32 *width, UINT32 *height)
{
    UINT32 code;
    UINT32 w, h;

    const UINT32 code_width[8] = {160, 176, 240, 320, 352, 640, 704, 0};
    const UINT32 code_height1[8] = {120, 132, 144, 240, 288, 480, 0, 0};
    const UINT32 code_height2[4] = {180, 360, 576, 0};

    /* If we're decoding a P or B frame, get 1 bit that signals */
    /* difference in size from previous reference frame. */
    if (pRv8ParsePicParam->ptype == RV8_ORG_INTERPIC || pRv8ParsePicParam->ptype == RV8_ORG_TRUEBPIC)
    {
        /* no diff in size */
        if ( Real8_CB_Get1Bit(pBs) )
        {
            *width = pRv8ParsePicParam->pwidth_prev;
            *height = pRv8ParsePicParam->pheight_prev;
            return;
        }
    }
    /* width */
    code = Real8_CB_GetBits(pBs, 3);
    w = code_width[code];
    if (w == 0)
    {
        do
        {
            code = Real8_CB_GetBits(pBs, 8);
            w += (code << 2);
        }
        while (code == 255);
    }

    /* height */
    code = Real8_CB_GetBits(pBs, 3);
    h = code_height1[code];
    if (h == 0)
    {
        code <<= 1;
        code |= Real8_CB_Get1Bit(pBs);
        code &= 3;
        h = code_height2[code];
        if (h == 0)
        {
            do
            {
                code = Real8_CB_GetBits(pBs, 8);
                h += (code << 2);
            }
            while (code == 255);
        }
    }
    *width = w;
    *height = h;
}


/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::GetBits() */
/*      Reads bits from buffer.  Supports up to 25 bits. */
/*      nbits   : number of bits to be read */
/* --------------------------------------------------------------------------- */
static const UINT32 GetBitsMask[25] =
{
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
    0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
    0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
    0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff
};


SINT32 Real8_CB_FindNextSliceStartCode(BS *pBs)
{
    SINT32 i, RemainByte;

    BsToNextByte(pBs);

    RemainByte = (BsResidBits(pBs) / 8) - 2;
    for (i = 0; i < RemainByte; i++)
    {
        if (FIELDVAL_SSC_RV8 == (UINT32)BsShow(pBs, FIELDLEN_SSC_RV8))
        {
            return RV8_VFMW_TRUE;
        }

        BsSkip(pBs, 8);
    }

    return RV8_VFMW_FALSE;
}


/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::SearchBits_IgnoreOnly0() */
/*      Searches for a code with known number of bits.  Skips only zero bits. */
/*      Search fails if code has not been found and non-zero bit encountered. */
/*      nbits       : number of bits in the code */
/*      code        : code to search for */
/*      lookahead   : maximum number of bits to parse for the code */
/* --------------------------------------------------------------------------- */
SINT32 Real8_CB_SearchBits_IgnoreOnly0(
    BS *pBs,
    const UINT32   nbits,
    const UINT32   code,
    const UINT32   lookahead)
{
    UINT32 w, n, msb;

    msb = 1 << (nbits - 1);

    w = Real8_CB_GetBits(pBs, nbits);

    for (n = 0; w != code && n < lookahead; n++)
    {
        if (w & msb)
        { 
            break; 
        }
        w = ((w << 1) & GetBitsMask[nbits]) | Real8_CB_Get1Bit(pBs);
    }

    if (w == code)
    {
        return RV8_VFMW_TRUE;
    }
    else
    {
        return RV8_VFMW_FALSE;
    }
}


VOID Real8_CB_SetRPRSizes(RV8_PictureHeader *pPictureHeader, UINT32 num_sizes, UINT32 *sizes)
{
    UINT32 size[9] = {0,1,1,2,2,3,3,3,3};

    pPictureHeader->Rv8ParsePicParam.m_numSizes = num_sizes;
    pPictureHeader->Rv8ParsePicParam.m_pctszSize = size[num_sizes];
    pPictureHeader->Rv8ParsePicParam.m_pSizes = sizes;
}


SINT32 Real8_CB_GetSSC(BS *pBs, RV8_PictureHeader *pPictureHeader)
{
    //const UINT32 MAX_SSC_LOOKAHEAD_NUMBER = 7;

    if (RV8_FID_REALVIDEO30 == pPictureHeader->Fid)
    {
#if 0
        UINT32 offset, i, deltaofs;

        offset = Real8_CB_GetBsOffset(t);
        RVAssert(offset >= SliceBitOffset(0) &&
                    offset < (Real8_CB_GetMaxBsSize(t) << 3));
        /* locate current slice, i - 1 */
        for (i = 1; i < t->m_nSlices && offset >= SliceBitOffset(i); i ++){};
        /* current slice must be valid at this point */
        RVAssert(SliceFlag(i - 1));
        /* check if just beginning to decode the current slice */
        if (offset == SliceBitOffset(i - 1))
        {
            return RV8_VFMW_TRUE;
        }
        /* there is no more slice header in the bitstream */
        if (i == t->m_nSlices)
        {
            return RV8_VFMW_FALSE;
        }
        /* skip stuffing if any */
        deltaofs = SliceBitOffset(i) - offset;
        if (deltaofs < 8 && Real8_CB_SearchBits(pBs,deltaofs, 0, 0))
        { 
            return RV8_VFMW_TRUE;
        }
#endif
    }
    else  //(RV8_FID_RV89COMBO == pPictureHeader->Fid)
    {
        if ( Real8_CB_FindNextSliceStartCode(pBs) )
        { 
            return RV8_VFMW_TRUE;
        }
        //GetSSC - SSC not found;
    }

    return RV8_VFMW_FALSE;
}


SINT32 Real8_CB_GetSliceHeader(
    BS *pBs,
    UINT32 bFirstSlice,
    SINT32 *iSliceMBA,
    SINT32 *iSliceGFID,
    SINT32 *iSQUANT,
    RV8_PictureHeader *pPictureHeader)
{
    RV8_ParsePicParam *pRv8ParsePicParam = &pPictureHeader->Rv8ParsePicParam;
    UINT32 versionEncoding;

    if (RV8_FID_REALVIDEO30 == pPictureHeader->Fid)
    {
        /* skip if this is the first slice called from IAPass1ProcessFrame */
        /* otherwise read the slice header */
        if (!bFirstSlice)
        {
            /* this was called from GetPicHeader */
            if (pRv8ParsePicParam->mba == -1)
            {
                bFirstSlice = RV8_VFMW_TRUE;
            }

            /* Bitstream version */
            versionEncoding = Real8_CB_GetBits(pBs, FIELDLEN_RV_BITSTREAM_VERSION);
            if (versionEncoding >= NUMBER_OF_RV_BITSTREAM_VERSIONS
                || s_RVVersionEncodingToMinorVersion[versionEncoding]
                > ENCODERS_CURRENT_RV_BITSTREAM_VERSION )
            {
                return RV8_VFMW_FALSE;
            }

            /* RealNetworks Slice Header */

            /* PTYPE */
            if (bFirstSlice)
            {
                pRv8ParsePicParam->ptype = (EnumRV8PicCodType)Real8_CB_GetBits(pBs, 2);
            }
            else
            {
                if ( (EnumRV8PicCodType)Real8_CB_GetBits(pBs, 2) != pRv8ParsePicParam->ptype )
                {
                    return RV8_VFMW_FALSE;
                }
            }

            /* ECC */
            if (Real8_CB_Get1Bit(pBs) != 0)
            {
                return RV8_VFMW_FALSE;
            }

            /* PQUANT/SQUANT */
            pRv8ParsePicParam->pquant = Real8_CB_GetBits(pBs, FIELDLEN_SQUANT);
            /* 0-31 are legitimate QP values for RV89Combo */
            if (pRv8ParsePicParam->pquant > 31)
            {
                return RV8_VFMW_FALSE;
            }
            /* In-loop deblocking */
            pRv8ParsePicParam->deblocking_filter_passthrough = 
                Real8_CB_GetBits(pBs, 1) ? 1 : 0;

            if (bFirstSlice)
            {
                pRv8ParsePicParam->tr = Real8_CB_GetBits(pBs, FIELDLEN_TR_RV);
            }
            else
            {
                if (Real8_CB_GetBits(pBs, FIELDLEN_TR_RV) != pRv8ParsePicParam->tr)
                {
                    return RV8_VFMW_FALSE;
                }
            }

            /* PCTSZ (RealVideo RPR picture size) */
            if (pRv8ParsePicParam->m_pctszSize > 0)
            {
                if (bFirstSlice)
                {
                    UINT32 width, height;
                    pRv8ParsePicParam->pctsz = Real8_CB_GetBits(pBs, pRv8ParsePicParam->m_pctszSize);

                    if (pRv8ParsePicParam->m_pSizes != NULL)
                    {
                        width = pRv8ParsePicParam->m_pSizes[2*pRv8ParsePicParam->pctsz];
                        height = pRv8ParsePicParam->m_pSizes[2*pRv8ParsePicParam->pctsz+1];
                        pRv8ParsePicParam->pwidth = width;
                        pRv8ParsePicParam->pheight = height;
                        /* TBD, why is parc set here? */
                        pPictureHeader->pic_width_in_pixel = width;
                        pPictureHeader->pic_height_in_pixel = height;

                        Real8_CB_SetDimensions(pPictureHeader);
                    }
                    else
                    {
                        return RV8_VFMW_FALSE;
                    }
                }
                else
                {
                    if (Real8_CB_GetBits(pBs, pRv8ParsePicParam->m_pctszSize) != pRv8ParsePicParam->pctsz)
                    {
                        return RV8_VFMW_FALSE;
                    }
                }
            }

            /* MBA */
            pRv8ParsePicParam->mba = Real8_CB_GetBits(pBs, pRv8ParsePicParam->m_mbaSize);
            /* RTYPE */
            if (bFirstSlice)
            {
                pRv8ParsePicParam->rtype = Real8_CB_GetBits(pBs, FIELDLEN_MPPTYPE_ROUNDING);
            }
            else
            {
                if (Real8_CB_GetBits(pBs, FIELDLEN_MPPTYPE_ROUNDING) != pRv8ParsePicParam->rtype)
                {
                    return RV8_VFMW_FALSE;
                }
            }
        }
        *iSliceMBA = pRv8ParsePicParam->mba;
        *iSQUANT = pRv8ParsePicParam->pquant;
        *iSliceGFID = 0;
    }
    else  //(RV8_FID_RV89COMBO == pPictureHeader->Fid)
    {
        /* The SSC either does not exist because this is the first slice */
        /* after the picture start code, or the SSC has already been read */
        /* in IAPass1. */
        if (!bFirstSlice)
        {
            /* Must be 1 to prevent start code emulation (SEPB1) */
            if (Real8_CB_Get1Bit(pBs) != 1)
            {
                return RV8_VFMW_FALSE;
            }
            /* No SSBI since CPM can not be "1". */
            /* Get macro-block address */
            *iSliceMBA = Real8_CB_GetBits(pBs, pRv8ParsePicParam->m_mbaSize);
            if (pRv8ParsePicParam->m_mbaSize > 11)
            {
                /* Must be 1 to prevent start code emulation (SEPB2) */
                if (Real8_CB_Get1Bit(pBs) != 1)
                {
                    return RV8_VFMW_FALSE;
                }
            }
            /* Get SQUANT */
            *iSQUANT = Real8_CB_GetBits(pBs, FIELDLEN_SQUANT);
            /* 0-31 are legitimate QP values for RV89Combo */
            if (*iSQUANT > 31)
            {
                return RV8_VFMW_FALSE;
            }
            /* No SWI field. */
            /* Must be 1 to prevent start code emulation (SEPB3) */
            if (Real8_CB_Get1Bit(pBs) != 1)
            {
                return RV8_VFMW_FALSE;
            }
            /* Get GOB frame ID. */
            *iSliceGFID = Real8_CB_GetBits(pBs, FIELDLEN_GFID);
        }
        else
        {
            *iSliceMBA = 0;
            *iSQUANT = -1;
        }
    }

    return RV8_VFMW_TRUE;
}


SINT32 Real8_CB_GetPictureHeader(BS *pBs, RV8_PictureHeader *pPictureHeader)
{
    SINT32 Status = VF_OK;
    SINT32 gfid, mba, squant;
    SINT32 tmp;
    RV8_ParsePicParam *pRv8ParsePicParam = &pPictureHeader->Rv8ParsePicParam;

    if (RV8_FID_REALVIDEO30 == pPictureHeader->Fid)
    {
        /* get the first slice header */
        /* set m_rv.mba to -1 to indicate this is a call to GetSliceHeader from  */
        /* GetPicHeader */
        pRv8ParsePicParam->mba = -1;
        if ( RV8_VFMW_TRUE == Real8_CB_GetSliceHeader(pBs, RV8_VFMW_FALSE, &mba, &gfid, &squant, pPictureHeader) )
        {
            switch (pRv8ParsePicParam->ptype)
            {
                case RV8_ORG_FORCED_INTRAPIC:
                    pPictureHeader->PicCodType = RV8_INTRAPIC;
                    pRv8ParsePicParam->m_isForcedKey = RV8_VFMW_TRUE;
                    break;
                case RV8_ORG_INTRAPIC:
                    pPictureHeader->PicCodType = RV8_INTRAPIC;
                    pRv8ParsePicParam->m_isForcedKey = RV8_VFMW_FALSE;
                    break;
                case RV8_ORG_INTERPIC:
                    pPictureHeader->PicCodType = RV8_INTERPIC;
                    break;
                case RV8_ORG_TRUEBPIC:
                    pPictureHeader->PicCodType = RV8_TRUEBPIC;
                    break;
                default:
                    break;
            }

            pPictureHeader->Rounding = pRv8ParsePicParam->rtype;
            pPictureHeader->Deblocking_Filter_Passthrough = pRv8ParsePicParam->deblocking_filter_passthrough;
            pPictureHeader->PQUANT = pRv8ParsePicParam->pquant;
            pPictureHeader->TR = pRv8ParsePicParam->tr;
            pPictureHeader->TRB = pRv8ParsePicParam->trb;
            pPictureHeader->DBQUANT = pRv8ParsePicParam->dbq;

            /* initialize m_nGOBS, m_nMBs, and m_mbaSize */
            if ( ( (pPictureHeader->pic_width_in_pixel<48) || (pPictureHeader->pic_width_in_pixel>MAX_HOR_SIZE) ) || 
                 ( (pPictureHeader->pic_height_in_pixel<48) || (pPictureHeader->pic_height_in_pixel>MAX_VER_SIZE) ) )
            {
                return VF_ERR_PARAM;
            }
            Real8_CB_SetDimensions(pPictureHeader);
        }
        else
        {
            Status = VF_ERR_PARAM;
        }
    }
    else  //(RV8_FID_RV89COMBO == pPictureHeader->Fid)
    {
        UINT32 length, info;
        const UINT32 PSC_LEN = 31;

        memset(pPictureHeader, 0, sizeof(*pPictureHeader));

        /* SUPER_VLC needs longer start code to avoid emulation */
        tmp = Real8_CB_GetBits(pBs, 24);
        if (tmp != 1)
        {
            return VF_ERR_PARAM;
        }
        length = Real8_CB_GetVLCBits(pBs, &info);
        if (length < PSC_LEN)
        {
            return VF_ERR_PARAM;
        }
        if (info & 1)
        {
            return VF_ERR_PARAM;
        }
        if (!((info>>1) & 1))
        {
            /* QCIF; */
            pPictureHeader->pic_width_in_pixel = 176;
            pPictureHeader->pic_height_in_pixel = 144;
        }
        else 
        {
            pPictureHeader->pic_width_in_pixel = 0;
            pPictureHeader->pic_height_in_pixel = 0;
            /* Read dimensions from the bitstream, below */
        }
        pPictureHeader->PQUANT = (info>>2) & 31;
        pPictureHeader->TR = (info>>(2+5)) & 255;

        /* Picture type */
        length = Real8_CB_GetVLCBits(pBs, &info);
        if (length == 1)
        {
            pPictureHeader->PicCodType = RV8_INTERPIC;
        }
        else if (length == 3 && info == 1)
        {
            pPictureHeader->PicCodType = RV8_INTRAPIC;
        }
        else if (length == 5 && info == 0)
        {
            pPictureHeader->PicCodType = RV8_TRUEBPIC;
        }
        else
        {
            return VF_ERR_PARAM;
        }
        /* Non-QCIF frame size */
        if (pPictureHeader->pic_width_in_pixel == 0)
        {
            if ( RV8_VFMW_FALSE == Real8_CB_GetCPFMT(pBs, pPictureHeader) )
            {
                return VF_ERR_PARAM;
            }
        }

        if ( ( (pPictureHeader->pic_width_in_pixel<48) || (pPictureHeader->pic_width_in_pixel>MAX_HOR_SIZE) ) || 
             ( (pPictureHeader->pic_height_in_pixel<48) || (pPictureHeader->pic_height_in_pixel>MAX_VER_SIZE) ) )
        {
            return VF_ERR_PARAM;
        }
        Real8_CB_SetDimensions(pPictureHeader);
    }

    pPictureHeader->pic_width_in_mb = (pPictureHeader->pic_width_in_pixel + 15) / 16;
    pPictureHeader->pic_height_in_mb = (pPictureHeader->pic_height_in_pixel + 15) / 16;
    pPictureHeader->total_mbs = pPictureHeader->pic_width_in_mb * pPictureHeader->pic_height_in_mb;

    return Status;
}


VOID Real8_SetImgFormat(RV8_CTX_S *pCtx)
{
    UINT32 codtype, csp, vfmt, sfmt, fldflg, fldfst = 0, dar;
    RV8_CODECINF *pRv8CodecInfo;
    RV8_PictureHeader *pPictureHeader;
    IMAGE *pImg;

    pRv8CodecInfo = &pCtx->CodecInfo;
    pPictureHeader = &pCtx->CodecInfo.PictureHeader;

    pImg = FSP_GetFsImagePtr(pCtx->ChanID, pRv8CodecInfo->CurFsID);
    if (NULL == pImg)
    {
        return;
    }

    codtype = pPictureHeader->PicCodType;
    csp = 0;
    vfmt = 5;
    sfmt = 0;
    fldflg = (pImg->format>>10)&0x3;
    fldflg |= 3;  // frame
    fldfst = 1;  //��������ָ����������ʾ
    dar = 0;

    pImg->format = ((dar&7)<<14)|((fldfst&0x3)<<12)|((fldflg&3)<<10)|((sfmt&3)<<8)|((vfmt&3)<<5)|((csp&7)<<2)|(codtype&3);
    pImg->top_fld_type = codtype&3;
    pImg->bottom_fld_type = codtype&3;
    pImg->is_fld_save = 0;
    pImg->image_width = pPictureHeader->pic_width_in_pixel;
    pImg->image_height = pPictureHeader->pic_height_in_pixel;

	SetAspectRatio(pImg,(VDEC_DAR_E)dar);
	
    if ((pCtx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION))
	{
	    SINT32  Stride = pImg->image_stride;//(pImg->image_width + 63) & (~63);
		SINT32  ChromOfst = Stride * ((pImg->image_height + 15)/16)*16;

		pImg->chrom_phy_addr = pImg->luma_phy_addr + ChromOfst;		
	    pImg->top_luma_phy_addr  = pImg->luma_phy_addr;
	    pImg->top_chrom_phy_addr = pImg->top_luma_phy_addr + ChromOfst;
	    pImg->btm_luma_phy_addr  = pImg->top_luma_phy_addr + Stride;
	    pImg->btm_chrom_phy_addr = pImg->top_chrom_phy_addr + Stride;
	    pImg->luma_2d_phy_addr   = pImg->luma_phy_addr;
		pImg->chrom_2d_phy_addr  = pImg->top_chrom_phy_addr;
	} 
}


VOID Real8_WriteSliceMsg(RV8_CTX_S *pCtx, SINT32 HoldSliceNum)
{
    RV8_DEC_PARAM_S *pRv8DecParam;
    RV8_CODECINF *pRv8CodecInfo;
    RV8_PictureHeader *pPictureHeader;
    RV8_SliceHeader *pSliceHeader;
    RV8_SLC_PARAM_S *pSliceParam;

    pRv8DecParam = &pCtx->Rv8DecParam;
    pRv8CodecInfo = &pCtx->CodecInfo;
    pPictureHeader = &pCtx->CodecInfo.PictureHeader;
    pSliceHeader = &pCtx->CodecInfo.SliceHeader;

    pSliceParam = &pRv8DecParam->SliceParam[HoldSliceNum];

    pSliceParam->StreamPhyAddr[0] = pSliceHeader->bit_stream_addr_0;
    pSliceParam->StreamBitOffset[0] = pSliceHeader->bit_offset_0;
    pSliceParam->StreamLength[0] = pSliceHeader->bit_len_0;

    pSliceParam->StreamPhyAddr[1] = pSliceHeader->bit_stream_addr_1;
    pSliceParam->StreamBitOffset[1] = pSliceHeader->bit_offset_1;
    pSliceParam->StreamLength[1] = pSliceHeader->bit_len_1;

    pSliceParam->dblk_filter_passthrough = pSliceHeader->dblk_filter_passthrough;
    pSliceParam->osvquant = pSliceHeader->osvquant;
    pSliceParam->sliceqp = pSliceHeader->SliceQP;
    pSliceParam->first_mb_in_slice = pSliceHeader->first_mb_in_slice;
    pSliceParam->last_mb_in_slice = pSliceHeader->last_mb_in_slice;///20141129

    pRv8DecParam->TotalSliceNum = HoldSliceNum + 1;

    return;
}


VOID Real8_WritePicMsg(RV8_CTX_S *pCtx)
{
    RV8_DEC_PARAM_S *pRv8DecParam;
    RV8_CODECINF *pRv8CodecInfo;
    RV8_PictureHeader *pPictureHeader;

    pRv8DecParam = &pCtx->Rv8DecParam;
    pRv8CodecInfo = &pCtx->CodecInfo;
    pPictureHeader = &pCtx->CodecInfo.PictureHeader;

    pRv8DecParam->PicCodingType = pPictureHeader->PicCodType;
    pRv8DecParam->PicWidthInMb = pPictureHeader->pic_width_in_mb;
    pRv8DecParam->PicHeightInMb = pPictureHeader->pic_height_in_mb;
    pRv8DecParam->Ratio0 = pRv8CodecInfo->Ratio0;
    pRv8DecParam->Ratio1 = pRv8CodecInfo->Ratio1;

    pRv8DecParam->PQUANT = pPictureHeader->PQUANT;
    pRv8DecParam->PrevPicQP = pRv8CodecInfo->PrevPicQP;
    pRv8DecParam->PrevPicMb0QP = pRv8CodecInfo->PrevPicMb0QP;

    if (RV8_TRUEBPIC != pPictureHeader->PicCodType)
    {
        pRv8CodecInfo->PrevPicQP = pPictureHeader->PQUANT;
    }

    return;
}


VOID Real8_WriteReg(RV8_CTX_S *pCtx, UINT32 fst_slc_grp)
{
    RV8_DEC_PARAM_S *pRv8DecParam;

    pRv8DecParam = &pCtx->Rv8DecParam;
    pRv8DecParam->FstSlcGrp = fst_slc_grp;

    return;
}


SINT32 Real8_ModifySliceMsg(RV8_CTX_S *pCtx)
{
    RV8_DEC_PARAM_S *pRv8DecParam;
    UADDR stream_base_addr;
    UINT32 BytePos0, BytePos1;
    SINT32 i, TotalMbnMinus1;

    pRv8DecParam = &pCtx->Rv8DecParam;

    TotalMbnMinus1 = pRv8DecParam->PicWidthInMb * pRv8DecParam->PicHeightInMb - 1;

    if (0 != pRv8DecParam->SliceParam[0].first_mb_in_slice)
    {
        return RV8_VFMW_FALSE;
    }

    if (pRv8DecParam->TotalSliceNum > 2)
    {
        for (i=1; i<pRv8DecParam->TotalSliceNum-1; i++)
        {
            if ( (pRv8DecParam->SliceParam[i].first_mb_in_slice <= 0) || (pRv8DecParam->SliceParam[i].first_mb_in_slice >= TotalMbnMinus1) )
            {
                return RV8_VFMW_FALSE;
            }
        }
    }

    if (pRv8DecParam->TotalSliceNum > 1)
    {
        for (i=0; i<pRv8DecParam->TotalSliceNum-1; i++)
        {
            if (pRv8DecParam->SliceParam[i].first_mb_in_slice >= pRv8DecParam->SliceParam[i+1].first_mb_in_slice)
            {
                return RV8_VFMW_FALSE;
            }
        }

        if ( (pRv8DecParam->SliceParam[pRv8DecParam->TotalSliceNum-1].first_mb_in_slice <= 0) || 
        	  (pRv8DecParam->SliceParam[pRv8DecParam->TotalSliceNum-1].first_mb_in_slice > TotalMbnMinus1) )
        {
            return RV8_VFMW_FALSE;
        }
    }

    for (i=0; i<pRv8DecParam->TotalSliceNum; i++)
    {
        if (pRv8DecParam->TotalSliceNum-1 == i)
        {
            pRv8DecParam->SliceParam[i].last_mb_in_slice = TotalMbnMinus1;
        }
        else
        {
            pRv8DecParam->SliceParam[i].last_mb_in_slice = pRv8DecParam->SliceParam[i+1].first_mb_in_slice - 1;
        }

        if ( (pRv8DecParam->SliceParam[i].first_mb_in_slice > pRv8DecParam->SliceParam[i].last_mb_in_slice) ||
              ( (pRv8DecParam->SliceParam[i].first_mb_in_slice < 0) || (pRv8DecParam->SliceParam[i].first_mb_in_slice > TotalMbnMinus1) ) ||
              ( (pRv8DecParam->SliceParam[i].last_mb_in_slice < 0) || (pRv8DecParam->SliceParam[i].last_mb_in_slice > TotalMbnMinus1) )
           )
        {
            return RV8_VFMW_FALSE;
        }
    }

    // find stream_base_addr
    stream_base_addr = pRv8DecParam->SliceParam[0].StreamPhyAddr[0];

    for (i=0; i<pRv8DecParam->TotalSliceNum; i++)
    {
        if (pRv8DecParam->SliceParam[i].StreamPhyAddr[0] < stream_base_addr)
        {
            stream_base_addr = pRv8DecParam->SliceParam[i].StreamPhyAddr[0];
        }

        if ( (pRv8DecParam->SliceParam[i].StreamPhyAddr[1]<stream_base_addr) && (0!=pRv8DecParam->SliceParam[i].StreamLength[1]) )
        {
            stream_base_addr = pRv8DecParam->SliceParam[i].StreamPhyAddr[1];
        }
    }

    pRv8DecParam->StreamBaseAddr = stream_base_addr & 0xFFFFFFF0;

    for (i=0; i<pRv8DecParam->TotalSliceNum; i++)
    {
        if (0 == pRv8DecParam->SliceParam[i].StreamPhyAddr[1])  //ֻ��һ������
        {
            BytePos0 = pRv8DecParam->SliceParam[i].StreamPhyAddr[0] - pRv8DecParam->StreamBaseAddr 
                         + pRv8DecParam->SliceParam[i].StreamBitOffset[0]/8;

            pRv8DecParam->SliceParam[i].StreamPhyAddr[0] = BytePos0 & 0xFFFFFFF0;
            pRv8DecParam->SliceParam[i].StreamBitOffset[0] = 8*(BytePos0 & 0x0000000F)
                         + pRv8DecParam->SliceParam[i].StreamBitOffset[0]%8;
        }
        else  //��������
        {
            BytePos0 = pRv8DecParam->SliceParam[i].StreamPhyAddr[0] - pRv8DecParam->StreamBaseAddr 
                         + pRv8DecParam->SliceParam[i].StreamBitOffset[0]/8;
            pRv8DecParam->SliceParam[i].StreamPhyAddr[0] = BytePos0 & 0xFFFFFFF0;
            pRv8DecParam->SliceParam[i].StreamBitOffset[0] = 8*(BytePos0 & 0x0000000F)
                         + pRv8DecParam->SliceParam[i].StreamBitOffset[0]%8;

            BytePos1 = pRv8DecParam->SliceParam[i].StreamPhyAddr[1] - pRv8DecParam->StreamBaseAddr 
                         + pRv8DecParam->SliceParam[i].StreamBitOffset[1]/8;
            pRv8DecParam->SliceParam[i].StreamPhyAddr[1] = BytePos1 & 0xFFFFFFF0;
            pRv8DecParam->SliceParam[i].StreamBitOffset[1] = 8*(BytePos1 & 0x0000000F)
                         + pRv8DecParam->SliceParam[i].StreamBitOffset[1]%8;
        }
    }

    return RV8_VFMW_TRUE;
}


SINT32 REAL8DEC_Init(RV8_CTX_S *pCtx, SYNTAX_EXTRA_DATA_S *pstExtraData)
{
    RV8_CODECINF *pRv8CodecInfo;
    RV8_PictureHeader *pPictureHeader;
    SINT32 ImgQueRstMagic;

    //��ʼ��
    ImgQueRstMagic = pCtx->ImageQue.ResetMagicWord;
    memset(pCtx, 0, sizeof(RV8_CTX_S));
    pCtx->ImageQue.ResetMagicWord = ImgQueRstMagic;
    ResetVoQueue(&pCtx->ImageQue);
        
    pCtx->pstExtraData = pstExtraData;
    pRv8CodecInfo = &pCtx->CodecInfo;
    pPictureHeader = &pCtx->CodecInfo.PictureHeader;

    pRv8CodecInfo->PrevPicQP = 0x0F;
    pRv8CodecInfo->PrevPicMb0QP = 0x0F;
    pRv8CodecInfo->UsedDecMode = pCtx->pstExtraData->s32DecMode;
    pRv8CodecInfo->Imgbuf_allocated = RV8_VFMW_FALSE;

    pPictureHeader->Fid = RV8_FID_REALVIDEO30;
    pPictureHeader->m_uTRWrap = RV8_TR_WRAP_RV;
    pPictureHeader->pixel_aspect_ratio = RV8_PARC_SQUARE;
    
    pCtx->ChanID = VCTRL_GetChanIDByCtx(pCtx);
    if ( -1 == pCtx->ChanID )
    {
        dprint(PRN_FATAL, "-1 == VCTRL_GetChanIDByCtx() Err! \n");
        return VF_ERR_SYS;
    }

    return 0;
}


VOID REAL8DEC_Destroy(RV8_CTX_S *pCtx)
{
    RV8_CODECINF *pRv8CodecInfo;

    pRv8CodecInfo = &pCtx->CodecInfo;
    dprint(PRN_CTRL, "Decoding quits at frm %d\n", pRv8CodecInfo->FrmCnt);

    return;
}

SINT32 REAL8DEC_Decode(RV8_CTX_S *pCtx, DEC_STREAM_PACKET_S *pPacket)
{
    RV8_PictureHeader  TmpPictureHeader  = {0};
    RV8_PictureHeader *pTmpPictureHeader = &TmpPictureHeader;
    RV8_PictureHeader *pPictureHeader = NULL;
    RV8_SliceHeader   *pSliceHeader   = NULL;
    UINT32 Ratio0, Ratio1;
    SINT32 trb, trd;
    SINT32 mbn, mbx, mby, SliceNum, HoldSliceNum;
    UINT32 bFirstSlice;
    SINT32 iSliceMBA, iSliceGFID, iSQUANT;
    SINT32 found_ssc;
    UINT32 QP, EntropyQP, quant_prev;
    UINT32 fst_slc_grp, last_slc_grp;
    SINT32 PrevHeaderBytes = 0, PrevOffsetBytes = 0;
    UINT32 i, SliceByteOffset;
    UINT32 Num_RPR_Sizes = 0;
    UINT32 RPR_Sizes[2 * RV8_MAX_NUM_RPR_SIZES] = {0};
    BS    *pBs  = NULL;
    IMAGE *pImg = NULL;
    FSP_LOGIC_FS_S *pCur, *pBwd, *pFwd;
    RV8_DEC_PARAM_S *pRv8DecParam  = NULL;
    RV8_CODECINF    *pRv8CodecInfo = NULL;
    
    pRv8DecParam = &pCtx->Rv8DecParam;
    pRv8CodecInfo = &pCtx->CodecInfo;
    pBs = &pCtx->Bs;
    pPictureHeader = &pCtx->CodecInfo.PictureHeader;
    pSliceHeader = &pCtx->CodecInfo.SliceHeader;

    pPictureHeader->Rv8ParsePicParam.pwidth_prev = pPictureHeader->Rv8ParsePicParam.pwidth;
    pPictureHeader->Rv8ParsePicParam.pheight_prev = pPictureHeader->Rv8ParsePicParam.pheight;
    pTmpPictureHeader->Rv8ParsePicParam.pwidth_prev = pPictureHeader->Rv8ParsePicParam.pwidth;
    pTmpPictureHeader->Rv8ParsePicParam.pheight_prev = pPictureHeader->Rv8ParsePicParam.pheight;
    pTmpPictureHeader->Rv8ParsePicParam.pwidth = pPictureHeader->Rv8ParsePicParam.pwidth;
    pTmpPictureHeader->Rv8ParsePicParam.pheight = pPictureHeader->Rv8ParsePicParam.pheight;

    pPictureHeader->Fid = RV8_FID_REALVIDEO30;
    pPictureHeader->m_uTRWrap = RV8_TR_WRAP_RV;

    TmpPictureHeader.Fid = pPictureHeader->Fid;
    TmpPictureHeader.m_uTRWrap = pPictureHeader->m_uTRWrap;

    if (NULL == pPacket)
    {
        dprint(PRN_ERROR, "ERROR: frm %d, Invalid packet pointer!\n", pRv8CodecInfo->FrmCnt);
        return RV8_VFMW_FALSE;
    }

    memset( pRv8DecParam, 0, sizeof(RV8_DEC_PARAM_S) );

    //if start new pic, length1 should be reserved for VFMW searching and parsing
    pRv8CodecInfo->BsOffset = 0;
    pRv8CodecInfo->PacketTail = pPacket->StreamPack[0].LenInByte;

    //adjust bs start address from updated offset
    pRv8CodecInfo->BsPhyAddr = pPacket->StreamPack[0].PhyAddr + pRv8CodecInfo->BsOffset;
    pRv8CodecInfo->BsVirAddr = pPacket->StreamPack[0].VirAddr + pRv8CodecInfo->BsOffset;
    pRv8CodecInfo->BsLength  = pPacket->StreamPack[0].LenInByte - pRv8CodecInfo->BsOffset;
    pRv8CodecInfo->IsLastSeg = pPacket->StreamPack[0].IsLastSeg;

    if (1 != pPacket->StreamPack[0].IsLastSeg)
    {
        pRv8CodecInfo->BsPhyAddr1 =  pPacket->StreamPack[1].PhyAddr;
        pRv8CodecInfo->BsVirAddr1 =  pPacket->StreamPack[1].VirAddr;
        pRv8CodecInfo->BsLength1 =  pPacket->StreamPack[1].LenInByte;
        pRv8CodecInfo->IsLastSeg1 = pPacket->StreamPack[1].IsLastSeg;
        if (1 != pPacket->StreamPack[1].IsLastSeg)
        {
            SM_ReleaseStreamSeg(pCtx->ChanID, pPacket->StreamPack[0].StreamID);
            SM_ReleaseStreamSeg(pCtx->ChanID, pPacket->StreamPack[1].StreamID);

            return RV8_VFMW_FALSE;
        }   
    }
    else
    {
        pRv8CodecInfo->BsPhyAddr1 = 0;
        pRv8CodecInfo->BsVirAddr1 = 0;
        pRv8CodecInfo->BsLength1  = 0;
        pRv8CodecInfo->IsLastSeg1 = 0;
    }

    if ( (pRv8CodecInfo->BsLength < 75) || (pRv8CodecInfo->BsLength >= 3*1024*1024) )
    {
        return RV8_VFMW_FALSE;
    }
    
    /*���������ʼ��*/
    if (NULL == pRv8CodecInfo->BsVirAddr)
    {
        return RV8_VFMW_FALSE;
    }
    BsInit(pBs, pRv8CodecInfo->BsVirAddr, pRv8CodecInfo->BsLength);
    Num_RPR_Sizes = Real8_CB_GetBits(pBs, 8);
    if (Num_RPR_Sizes > 8)
    {
        return RV8_VFMW_FALSE;
    }

    for (i=0; i<2*RV8_MAX_NUM_RPR_SIZES; i++)
    {
        RPR_Sizes[i] = Real8_CB_GetBits(pBs, 32);
        RPR_Sizes[i] = REVERSE_ENDIAN32( RPR_Sizes[i] );
    }

    pRv8CodecInfo->TotalSliceNum = Real8_CB_GetBits(pBs, 8) + 1;
    if ( (pRv8CodecInfo->TotalSliceNum > 256) || (pRv8CodecInfo->TotalSliceNum < 1) )
    {
        return RV8_VFMW_FALSE;
    }

    for (i=0; i<pRv8CodecInfo->TotalSliceNum; i++)
    {
        BsSkip(pBs, 32);
        SliceByteOffset = Real8_CB_GetBits(pBs, 32);
        pRv8CodecInfo->SliceByteOffset[i] = REVERSE_ENDIAN32( SliceByteOffset );
        if (pRv8CodecInfo->SliceByteOffset[i] >= 3*1024*1024)
        {
            return RV8_VFMW_FALSE;
        }
    }

    for (i=0; i<pRv8CodecInfo->TotalSliceNum-1; i++)
    {
        if (pRv8CodecInfo->SliceByteOffset[i+1] <= pRv8CodecInfo->SliceByteOffset[i])
        {
            return RV8_VFMW_FALSE;
        }
    }

    /*Picture Header ����*/
    PrevHeaderBytes = (pRv8CodecInfo->TotalSliceNum*8+1) + (2*4*RV8_MAX_NUM_RPR_SIZES+1);
    PrevOffsetBytes = PrevHeaderBytes + pRv8CodecInfo->SliceByteOffset[0];

    if ( (PrevHeaderBytes >= 3*1024*1024) || (PrevOffsetBytes >= 3*1024*1024) )
    {
        return RV8_VFMW_FALSE;
    }
    if ( (PrevOffsetBytes < (2*4*RV8_MAX_NUM_RPR_SIZES+1)+9) || (pRv8CodecInfo->BsLength-PrevOffsetBytes < 1) )
    {
        return RV8_VFMW_FALSE;
    }
    BsInit(pBs, pRv8CodecInfo->BsVirAddr+PrevOffsetBytes, pRv8CodecInfo->BsLength-PrevOffsetBytes);

    Real8_CB_SetRPRSizes(&TmpPictureHeader, Num_RPR_Sizes, &RPR_Sizes[0]);
    if ( VF_OK != Real8_CB_GetPictureHeader(pBs, &TmpPictureHeader) )
    {
        dprint(PRN_ERROR, "Real8_CB_GetPictureHeader Error!\n");
        return RV8_VFMW_FALSE;  //Error
    }
        
    //VHB ��ַ����
    if (RV8_VFMW_FALSE == pRv8CodecInfo->Imgbuf_allocated)
    {
        //δ������ߴ�С�仯��,��Ҫ�޸�VO����ṹ�ڵ���Ϣ
        REPORT_IMGSIZE_CHANGE(pCtx->ChanID, pPictureHeader->pic_width_in_mb, pPictureHeader->pic_height_in_mb,\
            pTmpPictureHeader->pic_width_in_mb, pTmpPictureHeader->pic_height_in_mb);
        RV8_CLEARREFFLAG;
        RV8_CLEARREFNUM; //��ղο�ͼ��������������������ȫ�������� 
        FSP_ClearNotInVoQueue(pCtx->ChanID, &(pCtx->ImageQue));
		
		if((pTmpPictureHeader->pic_width_in_mb > MAX_IMG_WIDTH_IN_MB) ||
			(pTmpPictureHeader->pic_height_in_mb > MAX_IMG_HEIGHT_IN_MB))
		{
			dprint(PRN_PIC, "pic width/height to large,MbWidth = %d, MbHeight = %d",
				pTmpPictureHeader->pic_width_in_mb,
				pTmpPictureHeader->pic_height_in_mb);
			return RV8_VFMW_FALSE;
		}
		
        if ((pCtx->pstExtraData->stChanOption.s32ReRangeEn == 0)&&(pCtx->pstExtraData->eCapLevel == CAP_LEVEL_USER_DEFINE_WITH_OPTION))
        {
            if (pRv8CodecInfo->Imgbuf_allocated == RV8_VFMW_FALSE)
            {
                ResetVoQueue(&pCtx->ImageQue);
            }
        }
        else
        {
            //δ������ߴ�С�仯��,��Ҫ�޸�VO����ṹ�ڵ���Ϣ        
            ResetVoQueue(&pCtx->ImageQue);
        }
        dprint(PRN_ERROR, "new seq size %d * %d\n", pTmpPictureHeader->pic_width_in_mb, pTmpPictureHeader->pic_height_in_mb);
		
        if(RV8_VFMW_FALSE == Real8_ArrangeVHBMem(REAL8_MAXIMUM_IMAGE_NUM, pCtx, pTmpPictureHeader))
        {
			dprint(PRN_ERROR, "Real8_ArrangeVHBMem err\n"); 
			return RV8_VFMW_FALSE;
        }
    }

    memcpy( pPictureHeader, pTmpPictureHeader, sizeof(RV8_PictureHeader) );

    bFirstSlice = RV8_VFMW_TRUE;

    //fixed 20130603
    pRv8CodecInfo->UsedDecMode = pCtx->pstExtraData->s32DecMode;
   
    //��ȡ֡��
    if ( RV8_VFMW_FALSE == Real8_GetImageBuffer(pCtx) )
    {
        dprint(PRN_FATAL, "Fatal Error: frm%d, get image buffer failed\n", pRv8CodecInfo->FrmCnt);
        return RV8_VFMW_FALSE;
    }

    /* PTS */
    pImg = FSP_GetFsImagePtr(pCtx->ChanID, pRv8CodecInfo->CurFsID);
    if (NULL != pImg)
    {
        pImg->PTS = pCtx->pstExtraData->pts;
        pImg->Usertag = pCtx->pstExtraData->Usertag;
        pImg->DispTime = pCtx->pstExtraData->DispTime;
        pImg->DispEnableFlag = pCtx->pstExtraData->DispEnableFlag;
        pImg->DispFrameDistance = pCtx->pstExtraData->DispFrameDistance;
        pImg->DistanceBeforeFirstFrame = pCtx->pstExtraData->DistanceBeforeFirstFrame;
        pImg->GopNum = pCtx->pstExtraData->GopNum;

        pCtx->pstExtraData->pts = (UINT64)(-1);    
	    dprint(PRN_PTS,"dec_pts: %lld Usertag = %lld\n",pImg->PTS, pImg->Usertag);
    }
    else
    {
        FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
        return RV8_VFMW_FALSE;
    }

    /* �ο�ά�� */
    if (RV8_TRUEBPIC != pPictureHeader->PicCodType)
    {
        if ( (RV8_INTERPIC == pPictureHeader->PicCodType) && (pRv8CodecInfo->RefNum < 1) )
        {
            return RV8_VFMW_FALSE;
        }
        pRv8CodecInfo->RefNum = pRv8CodecInfo->RefNum>0 ? 2 : 1;
        if (pRv8CodecInfo->FwdFsID != pRv8CodecInfo->BwdFsID)
        {
            //�����ͷŵĲ�����Ч�ο�ͼ
            dprint(PRN_REF, "---------- unref %d\n", pRv8CodecInfo->FwdFsID);
            FSP_SetRef(pCtx->ChanID, pRv8CodecInfo->FwdFsID, 0);
        }
        pRv8CodecInfo->FwdFsID = pRv8CodecInfo->BwdFsID;
        pRv8CodecInfo->BwdFsID = pRv8CodecInfo->CurFsID;
        FSP_SetRef(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);

        FSP_RemovePmv(pCtx->ChanID, pRv8CodecInfo->FwdFsID);
        dprint(PRN_REF, "++++++++++ ref %d\n", pRv8CodecInfo->CurFsID);
    }
    else
    {
        if (pRv8CodecInfo->RefNum < 2)
        {
            FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
            return RV8_VFMW_FALSE;
        }
    }

    pCur = FSP_GetLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID);
    pFwd = FSP_GetLogicFs(pCtx->ChanID, pRv8CodecInfo->FwdFsID);
    pBwd = FSP_GetLogicFs(pCtx->ChanID, pRv8CodecInfo->BwdFsID);
    
    if ((NULL== pCur) || (NULL== pFwd) || (NULL== pBwd))
    {
    	dprint(PRN_DBG, "%s %d unknow error!!\n",__FUNCTION__,__LINE__);
    	return RV8_VFMW_FALSE;
    }

    if ( (NULL == pCur->pstDecodeFs) || (NULL == pCur->pstTfOutFs) || (NULL == pCur->pstDispOutFs) )
    {
        FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
        dprint(PRN_FATAL, "CurFsID abnormal!\n");
        return RV8_VFMW_FALSE;
    }
    else
    {
        pRv8DecParam->BwdRefPhyAddr   = (NULL != pBwd->pstDecodeFs)? pBwd->pstDecodeFs->PhyAddr: pCur->pstDecodeFs->PhyAddr;
        pRv8DecParam->FwdRefPhyAddr   = (NULL != pFwd->pstDecodeFs)? pFwd->pstDecodeFs->PhyAddr: pCur->pstDecodeFs->PhyAddr;
        pRv8DecParam->CurPicPhyAddr   = pCur->pstDecodeFs->PhyAddr;
        pRv8DecParam->CurrPmvPhyAddr = pCur->PmvAddr;

        pRv8DecParam->ColPmvPhyAddr = pBwd->PmvAddr;
        pRv8DecParam->DispFramePhyAddr = pCur->pstDispOutFs->PhyAddr;
    }

    dprint(PRN_REF, "dec addr: cur,cur2d,fwd,bwd,currpmv,colpmv = %p,%p,%p,%p,%p,%p\n", pRv8DecParam->CurPicPhyAddr, pRv8DecParam->DispFramePhyAddr, 
           pRv8DecParam->FwdRefPhyAddr, pRv8DecParam->BwdRefPhyAddr, pRv8DecParam->CurrPmvPhyAddr, pRv8DecParam->ColPmvPhyAddr);

    pRv8DecParam->DDRStride = pCur->pstDispOutFs->Stride;

    //PrevPicQP
    //����TRB, TRD
    if (RV8_TRUEBPIC == pPictureHeader->PicCodType)
    {
        /* Compute ratios needed for direct mode */
        trb = (SINT32)pPictureHeader->TR - (SINT32)pFwd->TR;
        trd = (SINT32)pBwd->TR - (SINT32)pFwd->TR;

        if (trb  < 0)
        {
            trb += pPictureHeader->m_uTRWrap;
        }
        if (trd < 0)
        {
            trd += pPictureHeader->m_uTRWrap;
        }        
        if (trb > trd)      /* just in case TR's are incorrect */
        {
            trb = 0;
        }
        if (trd > 0)
        {
            Ratio0 = (trb << RV8_TR_SHIFT) / trd;
            Ratio1 = ((trd-trb) << RV8_TR_SHIFT) / trd;
        }
        else
        {
            Ratio0 = Ratio1 = 0;
        }

        pRv8CodecInfo->trb = trb;
        pRv8CodecInfo->trd = trd;

        pRv8CodecInfo->Ratio0 = Ratio0;
        pRv8CodecInfo->Ratio1 = Ratio1;
    }

    pCur->TR = pPictureHeader->TR;

    QP = pPictureHeader->PQUANT;
    quant_prev = QP;
    EntropyQP = QP;

    Real8_WritePicMsg(pCtx);

    mbn = 0;
    mbx = 0;
    mby = 0;
    fst_slc_grp = 1;
    SliceNum = 0;
    HoldSliceNum = 0;

    // Cfg Slice Msg Start
    pSliceHeader->bit_offset_0 = BsPos(pBs)%8;
    if (1 == pRv8CodecInfo->TotalSliceNum)
    {
        pSliceHeader->bit_len_0 = (pRv8CodecInfo->BsLength - PrevHeaderBytes - pRv8CodecInfo->SliceByteOffset[0])*8 - BsPos(pBs);  //CurrSliceBitLen;
    }
    else
    {
        pSliceHeader->bit_len_0 = (pRv8CodecInfo->SliceByteOffset[1] - pRv8CodecInfo->SliceByteOffset[0])*8 - BsPos(pBs);  //CurrSliceBitLen;
    }
    pSliceHeader->bit_stream_addr_0 = pRv8CodecInfo->BsPhyAddr + PrevOffsetBytes + BsPos(pBs)/8;   //(CurrSliceStartBitPos/8) & 0xFFFFFFF0;

    pSliceHeader->bit_offset_1 = 0;
    pSliceHeader->bit_len_1 = 0;
    pSliceHeader->bit_stream_addr_1 = 0;

    pSliceHeader->SliceQP = QP;
    pSliceHeader->osvquant = pPictureHeader->OSVQUANT;
    pSliceHeader->dblk_filter_passthrough = pPictureHeader->Deblocking_Filter_Passthrough;
    pSliceHeader->first_mb_in_slice = mbn;
    Real8_WriteSliceMsg(pCtx, HoldSliceNum);
    SliceNum++;
    HoldSliceNum++;

    /*Slice Header����*/
    for (i=1; i<pRv8CodecInfo->TotalSliceNum; i++)
    {
        PrevOffsetBytes = PrevHeaderBytes + pRv8CodecInfo->SliceByteOffset[i];
        if (PrevOffsetBytes >= 3*1024*1024)
        {
            FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
            return RV8_VFMW_FALSE;
        }
        if ( (PrevOffsetBytes < (2*4*RV8_MAX_NUM_RPR_SIZES+1)+9) || (pRv8CodecInfo->BsLength-PrevOffsetBytes < 1) )
        {
            FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
            return RV8_VFMW_FALSE;
        }
        BsInit(pBs, pRv8CodecInfo->BsVirAddr+PrevOffsetBytes, pRv8CodecInfo->BsLength-PrevOffsetBytes);
        
        bFirstSlice = RV8_VFMW_FALSE;  //RV8_VFMW_TRUE;
        found_ssc = Real8_CB_GetSliceHeader(pBs, bFirstSlice, &iSliceMBA, &iSliceGFID, &iSQUANT, pPictureHeader);
        if (RV8_VFMW_TRUE != found_ssc)
        {
            continue;
        }

        mbn = iSliceMBA;
		//�˴���������
        if (pPictureHeader->pic_width_in_mb == 0)
        {
            FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
            return RV8_VFMW_FALSE;
        }
        mby = mbn / pPictureHeader->pic_width_in_mb;
        mbx = mbn % pPictureHeader->pic_width_in_mb;
        
        if (iSQUANT >= 0)
        {
            QP = quant_prev = iSQUANT;
        }
        EntropyQP = QP;
        
        // Cfg Slice Msg Start
        pSliceHeader->bit_offset_0 = BsPos(pBs)%8;
        if (i == (pRv8CodecInfo->TotalSliceNum-1) )
        {
            pSliceHeader->bit_len_0 = (pRv8CodecInfo->BsLength - PrevHeaderBytes - pRv8CodecInfo->SliceByteOffset[i])*8 - BsPos(pBs);
        }
        else
        {
             if (i > (RV8_MAX_SLC_PARAM_NUM - 2))
             {
                 dprint(PRN_DBG, "line:%d! i > (RV8_MAX_SLC_PARAM_NUM - 2)\n",__LINE__);
                 return RV8_VFMW_FALSE;
             }
             pSliceHeader->bit_len_0 = (pRv8CodecInfo->SliceByteOffset[i+1] - pRv8CodecInfo->SliceByteOffset[i])*8 - BsPos(pBs);
        }
        
        pSliceHeader->bit_stream_addr_0 = pRv8CodecInfo->BsPhyAddr + PrevOffsetBytes + BsPos(pBs)/8;
        pSliceHeader->bit_offset_1 = 0;
        pSliceHeader->bit_len_1 = 0;
        pSliceHeader->bit_stream_addr_1 = 0;
        
        pSliceHeader->SliceQP = QP;
        pSliceHeader->osvquant = pPictureHeader->OSVQUANT;
        pSliceHeader->dblk_filter_passthrough = pPictureHeader->Deblocking_Filter_Passthrough;
        pSliceHeader->first_mb_in_slice = mbn;
        Real8_WriteSliceMsg(pCtx, HoldSliceNum);
        SliceNum++;
        HoldSliceNum++;
        
        if (HoldSliceNum >= REAL8_SLC_DN_MSG_SLOT_FULL_THRESHOLD)
        {        
            if ( RV8_VFMW_TRUE != Real8_ModifySliceMsg(pCtx) )
            {
                FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
                return RV8_VFMW_FALSE;
            }
            pCtx->pRv8DecParam = &pCtx->Rv8DecParam;

            if( NULL == pCtx->pRv8DecParam )
            {
                dprint(PRN_DBG, "%s %d NULL == pCtx->pRv8DecParam!!\n",__FUNCTION__,__LINE__);
                return RV8_VFMW_FALSE;
            }

            if ( (1 == pCtx->pstExtraData->stDisParams.s32Mode)&& 
                 (RV8_TRUEBPIC == pCtx->pRv8DecParam->PicCodingType) )
            {
                if (NULL != pCtx->pRv8DecParam)
                {
                    pCtx->pRv8DecParam = NULL;
                    FSP_SetDisplay(pCtx->ChanID, pCtx->CodecInfo.CurFsID, 0);
                    ReleasePacket(VFMW_REAL8, pCtx);
                    return RV8_VFMW_FALSE;
                }
            }
            else if (2 == pCtx->pstExtraData->stDisParams.s32Mode)
            {
                if ((pCtx->pstExtraData->stDisParams.s32DisNums > 0) && (NULL != pCtx->pRv8DecParam)
                    && (RV8_TRUEBPIC == pCtx->pRv8DecParam->PicCodingType))
                {
                    pCtx->pRv8DecParam = NULL;
                    FSP_SetDisplay(pCtx->ChanID, pCtx->CodecInfo.CurFsID, 0);
                    pCtx->pstExtraData->stDisParams.s32DisNums--;
                    ReleasePacket(VFMW_REAL8, pCtx);
                    return RV8_VFMW_FALSE;
                }
            }
            //pCtx->pRv8DecParam = &pCtx->Rv8DecParam;
        }
        // Cfg Slice Msg End
    }    


    last_slc_grp = 1;
    Real8_WriteReg(pCtx, fst_slc_grp);
    fst_slc_grp = 0;
    HoldSliceNum = 0;
    if(NULL == pCtx->pRv8DecParam)
    {
        // Start VDM
        if ( RV8_VFMW_TRUE != Real8_ModifySliceMsg(pCtx) )
        {
            FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
            return RV8_VFMW_FALSE;
        }
        pCtx->pRv8DecParam = &pCtx->Rv8DecParam;
    }
    pCtx->Rv8DecParam.Compress_en = pCtx->pstExtraData->s32Compress_en;

    return RV8_VFMW_TRUE;
}

SINT32 REAL8DEC_Decode_User(RV8_CTX_S *pCtx, DEC_STREAM_PACKET_S *pPacket)
{
    RV8_PictureHeader  TmpPictureHeader = {0};
    RV8_PictureHeader *pTmpPictureHeader = &TmpPictureHeader;
    RV8_PictureHeader *pPictureHeader = NULL;
    RV8_SliceHeader   *pSliceHeader   = NULL;
    UINT32 i;
    UINT32 Ratio0, Ratio1;
    SINT32 trb, trd;
    SINT32 HoldSliceNum;
    UINT32 fst_slc_grp;
    SINT32 TotalSliceBits = 0;
    BS    *pBs  = NULL;
    IMAGE *pImg = NULL;
    FSP_LOGIC_FS_S  *pCur, *pBwd, *pFwd;
    RV8_DEC_PARAM_S *pRv8DecParam  = NULL;
    RV8_CODECINF    *pRv8CodecInfo = NULL;
    CB_PicHdrEnc_S  *pPicHdrEnc    = NULL;
    CB_SlcHdrEnc_S  *pSlcHdrEnc    = NULL;
    
    pRv8DecParam = &pCtx->Rv8DecParam;
    pRv8CodecInfo = &pCtx->CodecInfo;
    pBs = &pCtx->Bs;
    pPictureHeader = &pCtx->CodecInfo.PictureHeader;
    pSliceHeader = &pCtx->CodecInfo.SliceHeader;

    pPictureHeader->Rv8ParsePicParam.pwidth_prev = pPictureHeader->Rv8ParsePicParam.pwidth;
    pPictureHeader->Rv8ParsePicParam.pheight_prev = pPictureHeader->Rv8ParsePicParam.pheight;
    pTmpPictureHeader->Rv8ParsePicParam.pwidth_prev = pPictureHeader->Rv8ParsePicParam.pwidth;
    pTmpPictureHeader->Rv8ParsePicParam.pheight_prev = pPictureHeader->Rv8ParsePicParam.pheight;
    pTmpPictureHeader->Rv8ParsePicParam.pwidth = pPictureHeader->Rv8ParsePicParam.pwidth;
    pTmpPictureHeader->Rv8ParsePicParam.pheight = pPictureHeader->Rv8ParsePicParam.pheight;

    pPictureHeader->Fid = RV8_FID_REALVIDEO30;
    pPictureHeader->m_uTRWrap = RV8_TR_WRAP_RV;
    TmpPictureHeader.Fid = pPictureHeader->Fid;
    TmpPictureHeader.m_uTRWrap = pPictureHeader->m_uTRWrap;
    
    if (NULL == pPacket)
    {
        dprint(PRN_ERROR, "ERROR: frm %d, Invalid packet pointer!\n", pRv8CodecInfo->FrmCnt);
        return RV8_VFMW_FALSE;
    }

    memset( pRv8DecParam, 0, sizeof(RV8_DEC_PARAM_S) );

    //if start new pic, length1 should be reserved for VFMW searching and parsing
    pRv8CodecInfo->BsOffset = 0;
    pRv8CodecInfo->PacketTail = pPacket->StreamPack[0].LenInByte;

    //init pre parse pic header & slice header
    //for IsLastSeg always be 1 in SM_CopyRawToSeg, so we don't care StreamPack[1]
    pPicHdrEnc = (CB_PicHdrEnc_S *)pPacket->StreamPack[0].VirAddr;
    pRv8CodecInfo->BsOffset += sizeof(CB_PicHdrEnc_S);
    pSlcHdrEnc = (CB_SlcHdrEnc_S *)(pPacket->StreamPack[0].VirAddr + pRv8CodecInfo->BsOffset);
    pRv8CodecInfo->BsOffset += sizeof(CB_SlcHdrEnc_S)*pPicHdrEnc->TotalSliceNum;

    if (pRv8CodecInfo->BsOffset >= pRv8CodecInfo->PacketTail)
    {
        dprint(PRN_ERROR, "%s ERROR: BsOffset(%d) >= PacketTail(%d)\n", __func__, pRv8CodecInfo->BsOffset, pRv8CodecInfo->PacketTail);
        return RV9_VFMW_FALSE;
    }
    
    //adjust bs start address from updated offset
    pRv8CodecInfo->BsPhyAddr = pPacket->StreamPack[0].PhyAddr + pRv8CodecInfo->BsOffset;
    
    pRv8CodecInfo->BsVirAddr = pPacket->StreamPack[0].VirAddr + pRv8CodecInfo->BsOffset;
    pRv8CodecInfo->BsLength  = pPacket->StreamPack[0].LenInByte - pRv8CodecInfo->BsOffset;
    pRv8CodecInfo->IsLastSeg = pPacket->StreamPack[0].IsLastSeg;

    if (1 != pPacket->StreamPack[0].IsLastSeg)
    {
        pRv8CodecInfo->BsPhyAddr1 =  pPacket->StreamPack[1].PhyAddr;
        pRv8CodecInfo->BsVirAddr1 =  pPacket->StreamPack[1].VirAddr;
        pRv8CodecInfo->BsLength1 =  pPacket->StreamPack[1].LenInByte;
        pRv8CodecInfo->IsLastSeg1 = pPacket->StreamPack[1].IsLastSeg;
        if (1 != pPacket->StreamPack[1].IsLastSeg)
        {
            SM_ReleaseStreamSeg(pCtx->ChanID, pPacket->StreamPack[0].StreamID);
            SM_ReleaseStreamSeg(pCtx->ChanID, pPacket->StreamPack[1].StreamID);

            return RV8_VFMW_FALSE;
        } 
    }
    else
    {
        pRv8CodecInfo->BsPhyAddr1 = 0;
        pRv8CodecInfo->BsVirAddr1 = 0;
        pRv8CodecInfo->BsLength1  = 0;
        pRv8CodecInfo->IsLastSeg1 = 0;
    }

    if ( (pRv8CodecInfo->BsLength < 40) || (pRv8CodecInfo->BsLength >= 3*1024*1024) )   //40�Ƿ����?
    {
        dprint(PRN_ERROR, "pRv8CodecInfo->BsLength = %d\n", pRv8CodecInfo->BsLength);
        return RV8_VFMW_FALSE;
    }
        
    pRv8CodecInfo->TotalSliceNum = pPicHdrEnc->TotalSliceNum;
    if ( (pRv8CodecInfo->TotalSliceNum > 256) || (pRv8CodecInfo->TotalSliceNum < 1) )
    {
        return RV8_VFMW_FALSE;
    }
    
    pPictureHeader->PicCodType = pPicHdrEnc->PicCodType;
    pPictureHeader->pic_width_in_mb = (pPicHdrEnc->PicWidthInPixel+15)/16;
    pPictureHeader->pic_height_in_mb = (pPicHdrEnc->PicHeightInPixel+15)/16;
    pPictureHeader->pic_width_in_pixel= pPictureHeader->pic_width_in_mb*16;
    pPictureHeader->pic_height_in_pixel = pPictureHeader->pic_height_in_mb*16;
    pPictureHeader->Rounding = pPicHdrEnc->Rounding;
    pPictureHeader->stream_base_addr = pRv8CodecInfo->BsPhyAddr & 0xFFFFFFF0;
    pPictureHeader->total_mbs = pPictureHeader->pic_width_in_mb * pPictureHeader->pic_height_in_mb;
        
    //VHB ��ַ����
    if (RV8_VFMW_FALSE == pRv8CodecInfo->Imgbuf_allocated)
    {
        RV8_CLEARREFFLAG;
        RV8_CLEARREFNUM;
        FSP_ClearNotInVoQueue(pCtx->ChanID, &(pCtx->ImageQue));
        if(RV8_VFMW_FALSE == Real8_ArrangeVHBMem(REAL8_MAXIMUM_IMAGE_NUM, pCtx, pPictureHeader))
        {
			dprint(PRN_ERROR, "Real8_ArrangeVHBMem err\n"); 
			return RV8_VFMW_FALSE;
        }
        //pRv8CodecInf->pToQue = NULL;
    }

    //fixed 20130603
    pRv8CodecInfo->UsedDecMode = pCtx->pstExtraData->s32DecMode;
   
    //��ȡ֡��
    if ( RV8_VFMW_FALSE == Real8_GetImageBuffer(pCtx) )
    {
        dprint(PRN_FATAL, "Fatal Error: frm%d, get image buffer failed\n", pRv8CodecInfo->FrmCnt);
        return RV8_VFMW_FALSE;
    }

    /* PTS */
    pImg = FSP_GetFsImagePtr(pCtx->ChanID, pRv8CodecInfo->CurFsID);
    if (NULL != pImg)
    {
        pImg->PTS = pCtx->pstExtraData->pts;
        pImg->Usertag = pCtx->pstExtraData->Usertag;
        pImg->DispTime = pCtx->pstExtraData->DispTime;
        pImg->DispEnableFlag = pCtx->pstExtraData->DispEnableFlag;
        pImg->DispFrameDistance = pCtx->pstExtraData->DispFrameDistance;
        pImg->DistanceBeforeFirstFrame = pCtx->pstExtraData->DistanceBeforeFirstFrame;
        pImg->GopNum = pCtx->pstExtraData->GopNum;

        pCtx->pstExtraData->pts = (UINT64)(-1);    
	    dprint(PRN_PTS,"dec_pts: %lld Usertag = %lld\n",pImg->PTS, pImg->Usertag);
    }
    else
    {
        FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
        return RV8_VFMW_FALSE;
    }

    /* �ο�ά�� */
    if (RV8_TRUEBPIC != pPictureHeader->PicCodType)
    {
        if ( (RV8_INTERPIC == pPictureHeader->PicCodType) && (pRv8CodecInfo->RefNum < 1) )
        {
            return RV8_VFMW_FALSE;
        }
        pRv8CodecInfo->RefNum = pRv8CodecInfo->RefNum>0 ? 2 : 1;
        if (pRv8CodecInfo->FwdFsID != pRv8CodecInfo->BwdFsID)
        {
            //�����ͷŵĲ�����Ч�ο�ͼ
            dprint(PRN_REF, "---------- unref %d\n", pRv8CodecInfo->FwdFsID);
            FSP_SetRef(pCtx->ChanID, pRv8CodecInfo->FwdFsID, 0);
        }
        pRv8CodecInfo->FwdFsID = pRv8CodecInfo->BwdFsID;
        pRv8CodecInfo->BwdFsID = pRv8CodecInfo->CurFsID;
        FSP_SetRef(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);

        FSP_RemovePmv(pCtx->ChanID, pRv8CodecInfo->FwdFsID);
        dprint(PRN_REF, "++++++++++ ref %d\n", pRv8CodecInfo->CurFsID);
    }
    else
    {
        if (pRv8CodecInfo->RefNum < 2)
        {
            FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
            return RV8_VFMW_FALSE;
        }
    }

    pCur = FSP_GetLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID);
    pFwd = FSP_GetLogicFs(pCtx->ChanID, pRv8CodecInfo->FwdFsID);
    pBwd = FSP_GetLogicFs(pCtx->ChanID, pRv8CodecInfo->BwdFsID);
    
    if ((NULL== pCur) || (NULL== pFwd) || (NULL== pBwd))
    {
    	dprint(PRN_DBG, "%s %d unknow error!!\n",__FUNCTION__,__LINE__);
    	return RV8_VFMW_FALSE;
    }

    if ( (NULL == pCur->pstDecodeFs) || (NULL == pCur->pstTfOutFs) || (NULL == pCur->pstDispOutFs) )
    {
        FSP_ClearLogicFs(pCtx->ChanID, pRv8CodecInfo->CurFsID, 1);
        dprint(PRN_FATAL, "CurFsID abnormal!\n");
        return RV8_VFMW_FALSE;
    }
    else
    {
        pRv8DecParam->BwdRefPhyAddr   = (NULL != pBwd->pstDecodeFs)? pBwd->pstDecodeFs->PhyAddr: pCur->pstDecodeFs->PhyAddr;
        pRv8DecParam->FwdRefPhyAddr   = (NULL != pFwd->pstDecodeFs)? pFwd->pstDecodeFs->PhyAddr: pCur->pstDecodeFs->PhyAddr;
        pRv8DecParam->CurPicPhyAddr   = pCur->pstDecodeFs->PhyAddr;
        pRv8DecParam->CurrPmvPhyAddr = pCur->PmvAddr;

        pRv8DecParam->ColPmvPhyAddr = pBwd->PmvAddr;
        pRv8DecParam->DispFramePhyAddr = pCur->pstDispOutFs->PhyAddr;
    }

    dprint(PRN_REF, "dec addr: cur,cur2d,fwd,bwd,currpmv,colpmv = %p,%p,%p,%p,%p,%p\n", pRv8DecParam->CurPicPhyAddr, pRv8DecParam->DispFramePhyAddr, 
           pRv8DecParam->FwdRefPhyAddr, pRv8DecParam->BwdRefPhyAddr, pRv8DecParam->CurrPmvPhyAddr, pRv8DecParam->ColPmvPhyAddr);

    pRv8DecParam->DDRStride = pCur->pstDispOutFs->Stride;
    pRv8DecParam->StreamBaseAddr = pPictureHeader->stream_base_addr;
    
    //����TRB, TRD
    if (RV8_TRUEBPIC == pPictureHeader->PicCodType)
    {

        pRv8CodecInfo->trb = pPicHdrEnc->Trb;
        pRv8CodecInfo->trd = pPicHdrEnc->Trd;
    
        /* Compute ratios needed for direct mode */
        trb = pRv8CodecInfo->trb;
        trd = pRv8CodecInfo->trd;
      
        if (trb > trd)      /* just in case TR's are incorrect */
        {
            trb = 0;
        }
        if (trd > 0)
        {
            Ratio0 = (trb << RV8_TR_SHIFT) / trd;
            Ratio1 = ((trd-trb) << RV8_TR_SHIFT) / trd;
        }
        else
        {
            Ratio0 = Ratio1 = 0;
        }

        pRv8CodecInfo->Ratio0 = Ratio0;
        pRv8CodecInfo->Ratio1 = Ratio1;
    }

    pPictureHeader->PQUANT = pSlcHdrEnc[0].SliceQP;
    
    Real8_WritePicMsg(pCtx);

    HoldSliceNum = 0;
    
    for(i=0; i<pPicHdrEnc->TotalSliceNum; i++)
    {
        pSliceHeader->bit_offset_0 = (TotalSliceBits + pSlcHdrEnc[i].BitOffset) % 128;
        pSliceHeader->bit_len_0 = pSlcHdrEnc[i].BitLen - pSlcHdrEnc[i].BitOffset;
        pSliceHeader->bit_stream_addr_0 = (TotalSliceBits/8) & 0xFFFFFFF0;
        pSliceHeader->bit_offset_1 = 0;
        pSliceHeader->bit_len_1 = 0;
        pSliceHeader->bit_stream_addr_1 = 0;
        TotalSliceBits += pSlcHdrEnc[i].BitLen;;
        
        pSliceHeader->SliceQP = pSlcHdrEnc[i].SliceQP;
        pSliceHeader->osvquant = pSlcHdrEnc[i].Osvquant;
        pSliceHeader->dblk_filter_passthrough = pSlcHdrEnc[i].DblkFilterPassThrough;
        pSliceHeader->first_mb_in_slice = pSlcHdrEnc[i].FirstMbInSlice;

        if(i < pPicHdrEnc->TotalSliceNum-1)
        {
            pSliceHeader->last_mb_in_slice = pSlcHdrEnc[i+1].FirstMbInSlice - 1;
        }
        else
        {
            pSliceHeader->last_mb_in_slice = pPictureHeader->pic_width_in_mb*pPictureHeader->pic_height_in_mb - 1;
        }

        Real8_WriteSliceMsg(pCtx, HoldSliceNum);
        HoldSliceNum++;
    }
    
    fst_slc_grp = 1;
    Real8_WriteReg(pCtx, fst_slc_grp);
    
    pCtx->pRv8DecParam = &pCtx->Rv8DecParam;
    pCtx->Rv8DecParam.Compress_en = pCtx->pstExtraData->s32Compress_en;
        
    return RV8_VFMW_TRUE;
}

SINT32 REAL8DEC_RecycleImage(RV8_CTX_S *pCtx, UINT32 ImgID)
{
    RV8_CODECINF *pRv8CodecInfo;
    IMAGE *pImg;

    pRv8CodecInfo = &pCtx->CodecInfo;

    FSP_SetDisplay(pCtx->ChanID, ImgID, 0);
    pImg = FSP_GetFsImagePtr(pCtx->ChanID, ImgID);
    if (NULL != pImg)
    {
        //FreeUsdByDec(pCtx->ChanID, pImg->p_usrdat[0]);
        //pImg->p_usrdat[0] = NULL;
        //FreeUsdByDec(pCtx->ChanID, pImg->p_usrdat[1]);
        //pImg->p_usrdat[1] = NULL;
    }

    return RV8_VFMW_TRUE;
}


SINT32 REAL8DEC_GetRemainImg(RV8_CTX_S *pCtx)
{
    RV8_CODECINF *pRv8CodecInfo;
    IMAGE *pImg = NULL;
    SINT32 display_flag;
    SINT32 ret = LAST_OUTPUT_FAIL;

    pRv8CodecInfo = &pCtx->CodecInfo;

    if (OUTPUT_IN_DEC == pCtx->pstExtraData->s32DecOrderOutput)
    {
        ret = LAST_ALREADY_OUT;
    }
    else
    {
        pImg = FSP_GetFsImagePtr(pCtx->ChanID, pRv8CodecInfo->BwdFsID);
        if (NULL != pImg)
        {
    	   display_flag = FSP_GetDisplay(pCtx->ChanID, pRv8CodecInfo->BwdFsID);
	   if (FS_DISP_STATE_DEC_COMPLETE == display_flag)
	   {
             Real8_SetImgFormat(pCtx);
	       pImg->last_frame = 1;

              FSP_SetDisplay(pCtx->ChanID, pRv8CodecInfo->BwdFsID, 1);
              if (VF_OK != InsertImgToVoQueue(pCtx->ChanID, VFMW_REAL8, pCtx, &pCtx->ImageQue, pImg) )
              {
                  FSP_SetDisplay(pCtx->ChanID, pRv8CodecInfo->BwdFsID, 0);
                  //FreeUsdByDec(pCtx->ChanID, pImg->p_usrdat[0]);
                  //pImg->p_usrdat[0] = NULL;
                  //FreeUsdByDec(pCtx->ChanID, pImg->p_usrdat[1]);
                  //pImg->p_usrdat[1] = NULL;
                  ret = LAST_OUTPUT_FAIL;
              }
	       else if ( pImg->error_level > 100 || pImg->image_width < 32 || pImg->image_height < 32 )        // ��������·��ص�VF_OK����ûʵ�ʲ嵽������
              {
                  ret = LAST_OUTPUT_FAIL;
	       }
	       else   // �ɹ����������
	       {
                  ret = LAST_OUTPUT_OK;
	       }
          }// ֮ǰ�Ѿ��嵽������
	   else
	   {
              ret = LAST_ALREADY_OUT;
	   }
       }
    }

    if (LAST_OUTPUT_OK != ret)
    {
        ret = GetVoLastImageID(&pCtx->ImageQue);
    }

    return ret;
}


UINT32 REAL8DEC_VDMPostProc(RV8_CTX_S *pCtx, SINT32 ErrRatio, UINT32 Mb0QpInCurrPic)
{
    RV8_CODECINF *pRv8CodecInfo;
    IMAGE *pCurImg, *pToQueImg;

    pRv8CodecInfo = &pCtx->CodecInfo;

    dprint(PRN_DBG, "ErrRatio = %d\n", ErrRatio);

    ReleasePacket(VFMW_REAL8, pCtx);

    pCurImg = FSP_GetFsImagePtr(pCtx->ChanID, pRv8CodecInfo->CurFsID);

    if (NULL == pCurImg)
    {
         return RV8_VFMW_FALSE;
    }

    pRv8CodecInfo->LastDecPicCodingType = pRv8CodecInfo->PictureHeader.PicCodType;
    if (RV8_TRUEBPIC != pRv8CodecInfo->PictureHeader.PicCodType)
    {
        pRv8CodecInfo->PrevPicMb0QP = Mb0QpInCurrPic & 0x1F;
    }

    /* ����err_level */
    pCurImg->error_level = ErrRatio;
    pCurImg->error_level = CLIP1(100, (SINT32)(pCurImg->error_level));

    /* ��ͼ�������VO���� */
    dprint(PRN_DBG, "DEBUG: DEC over\n");

    Real8_SetImgFormat(pCtx);

    if ( I_MODE == pRv8CodecInfo->UsedDecMode )
    {
        pRv8CodecInfo->ToQueFsID = pRv8CodecInfo->CurFsID;
    }
    else
    {
        if (OUTPUT_IN_DEC != pCtx->pstExtraData->s32DecOrderOutput) /* if(��ʾ�����) */
        {
            if (RV8_TRUEBPIC != pRv8CodecInfo->PictureHeader.PicCodType)
            {
                pRv8CodecInfo->ToQueFsID = (pRv8CodecInfo->RefNum > 1)? pRv8CodecInfo->FwdFsID: -1;
            }
            else
            {
                pRv8CodecInfo->ToQueFsID = pRv8CodecInfo->CurFsID;
            }
        }
        else
        {
            pRv8CodecInfo->ToQueFsID = pRv8CodecInfo->CurFsID;
        }
    }

    if ( -1 != pRv8CodecInfo->ToQueFsID )
    {
        pToQueImg = FSP_GetFsImagePtr(pCtx->ChanID, pRv8CodecInfo->ToQueFsID);
        dprint(PRN_QUEUE, "insert buf %d ref %d\n", pRv8CodecInfo->ToQueFsID, FSP_GetRef(pCtx->ChanID, pRv8CodecInfo->ToQueFsID));

        FSP_SetDisplay(pCtx->ChanID, pRv8CodecInfo->ToQueFsID, 1);
        if (VF_OK != InsertImgToVoQueue(pCtx->ChanID, VFMW_REAL8, pCtx, &pCtx->ImageQue, pToQueImg))
        {
            FSP_SetDisplay(pCtx->ChanID, pRv8CodecInfo->ToQueFsID, 0);
            return RV8_VFMW_FALSE;
        }
    }

    pRv8CodecInfo->FrmCnt++;

    return RV8_VFMW_TRUE;
}


SINT32 REAL8DEC_GetImageBuffer( RV8_CTX_S *pCtx )
{
    RV8_CODECINF *pRv8CodecInfo;
    RV8_VOBUFINF *pVOBufInf;

    pRv8CodecInfo = &pCtx->CodecInfo;
    pVOBufInf = pCtx->CodecInfo.VOBufInf;

        /* ���FSP�����µ�֡����Ի�ȡ������TRUE */
    if (FSP_IsNewFsAvalible(pCtx->ChanID) == 1)
    {
        return 1;
    }
    else if (FSP_IsNewFsAvalible(pCtx->ChanID) == -1)
    {
        FSP_ClearNotInVoQueue(pCtx->ChanID, &(pCtx->ImageQue));
    }

    return 0;
}

VOID REAL8DEC_Support(SINT32 *flag)
{
    *flag = 1;
    return;
}
#endif //REAL8_VFMW_ENABLE
