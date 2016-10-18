﻿/*!
 *****************************************************************************
 *
 * @File       memmgr_api_rpc.h
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 * 
 * The contents of this file are subject to the MIT license as set out below.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * Alternatively, the contents of this file may be used under the terms of the 
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above. 
 * 
 * If you wish to allow use of your version of this file only under the terms 
 * of GPL, and not to allow others to use your version of this file under the 
 * terms of the MIT license, indicate your decision by deleting the provisions 
 * above and replace them with the notice and other provisions required by GPL 
 * as set out in the file called "GPLHEADER" included in this distribution. If 
 * you do not delete the provisions above, a recipient may use your version of 
 * this file under the terms of either the MIT license or GPL.
 * 
 * This License is also included in this distribution in the file called 
 * "MIT_COPYING".
 *
 *****************************************************************************/

#ifndef __MEMMGR_RPC_H__
#define __MEMMGR_RPC_H__

#include "img_defs.h"
#include "sysbrg_api.h"
#include "memmgr_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	WriteMemRefToMemRef_ID,
	TOPAZKM_MMUMAllocateHeapDeviceMemory_ID,
	TOPAZKM_StreamMMUMAllocateHeapDeviceMemory_ID,
	TOPAZKM_MMUMFreeDeviceMemory_ID,
	TOPAZKM_MMUFlushMMUTableCache_ID,
	TOPAZKM_MapExternal_ID,
	TOPAZKM_UnMapExternal_ID,
	TOPAZKM_MMCopyTiledBuffer_ID,

} MEMMGR_eFuncId;

typedef struct
{
	MEMMGR_eFuncId	eFuncId;
    union
	{
	
		struct
		{
			 sysbrg_user_pointer hDeviceMem;
                          		 IMG_UINT32 ui32Offset;
                          		 IMG_UINT32 ui32ManglerFuncIdExt;
                          		 sysbrg_user_pointer hRefDeviceMem;
                          		 IMG_UINT32 ui32RefOffset;
                          
		} sWriteMemRefToMemRefCmd;
	
		struct
		{
			 IMG_UINT32 ui32Size;
                          		 IMG_UINT32 ui32Alignment;
                          		 IMG_UINT32 ui32Heap;
                          		 IMG_BOOL bSaveRestore;
                          		 sysbrg_user_pointer ppMemInfo;
                          		 IMG_BOOL tileSensetive;
                          
		} sTOPAZKM_MMUMAllocateHeapDeviceMemoryCmd;
	
		struct
		{
			 IMG_UINT32 ui32StreamId;
                          		 IMG_UINT32 ui32Size;
                          		 IMG_UINT32 ui32Alignment;
                          		 IMG_UINT32 ui32Heap;
                          		 IMG_BOOL bSaveRestore;
                          		 sysbrg_user_pointer ppMemInfo;
                          		 IMG_BOOL tileSensetive;
                          
		} sTOPAZKM_StreamMMUMAllocateHeapDeviceMemoryCmd;
	
		struct
		{
			 sysbrg_user_pointer pMemInfo;
                          
		} sTOPAZKM_MMUMFreeDeviceMemoryCmd;
	
		struct
		{
			 IMG_UINT32 ui32BufLen;
                          		 IMG_UINT32 ui32PallocId;
                          		 IMG_UINT32 ui32Heap;
                          		 IMG_UINT32 ui32Alignment;
                          		 sysbrg_user_pointer memInfo;
                          
		} sTOPAZKM_MapExternalCmd;
	
		struct
		{
			 sysbrg_user_pointer memInfo;
                          
		} sTOPAZKM_UnMapExternalCmd;
	
		struct
		{
			 sysbrg_user_pointer pMemoryInfo;
                          		 sysbrg_user_pointer pcBuffer;
                          		 IMG_UINT32 ui32Size;
                          		 IMG_UINT32 ui32Offset;
                          		 IMG_BOOL bToMemory;
                          
		} sTOPAZKM_MMCopyTiledBufferCmd;
	
	} sCmd;
} MEMMGR_sCmdMsg;

typedef struct
{
    union
	{
	
		struct
		{
			IMG_BOOL		xTOPAZKM_MMUMAllocateHeapDeviceMemoryResp;
		} sTOPAZKM_MMUMAllocateHeapDeviceMemoryResp;
            
		struct
		{
			IMG_BOOL		xTOPAZKM_StreamMMUMAllocateHeapDeviceMemoryResp;
		} sTOPAZKM_StreamMMUMAllocateHeapDeviceMemoryResp;
            
		struct
		{
			IMG_BOOL		xTOPAZKM_MMUMFreeDeviceMemoryResp;
		} sTOPAZKM_MMUMFreeDeviceMemoryResp;
            
		struct
		{
			IMG_BOOL		xTOPAZKM_MMUFlushMMUTableCacheResp;
		} sTOPAZKM_MMUFlushMMUTableCacheResp;
            
		struct
		{
			IMG_BOOL		xTOPAZKM_MapExternalResp;
		} sTOPAZKM_MapExternalResp;
            
		struct
		{
			IMG_BOOL		xTOPAZKM_UnMapExternalResp;
		} sTOPAZKM_UnMapExternalResp;
            
		struct
		{
			IMG_BOOL		xTOPAZKM_MMCopyTiledBufferResp;
		} sTOPAZKM_MMCopyTiledBufferResp;
            
	} sResp;
} MEMMGR_sRespMsg;



extern IMG_VOID MEMMGR_dispatch(SYSBRG_sPacket __user *psPacket);

#ifdef __cplusplus
}
#endif

#endif
