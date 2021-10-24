/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

/*
	ColorMaster.h
*/

#pragma once

#ifndef COLORMASTER_H
#define COLORMASTER_H

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		u_int16;
typedef unsigned long		u_long;
typedef short int			int16;
#define PF_TABLE_BITS	12
#define PF_TABLE_SZ_16	4096

#define PF_DEEP_COLOR_AWARE 1	// make sure we get 16bpc pixels; 
								// AE_Effect.h checks for this.

#include "AEConfig.h"

#ifdef AE_OS_WIN
typedef unsigned short PixelType;
#include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"

#include "ColorMaster_Strings.h"

/* Versioning information */

#define	MAJOR_VERSION	1
#define	MINOR_VERSION	0
#define	BUG_VERSION		0
#define	STAGE_VERSION	PF_Stage_DEVELOP
#define	BUILD_VERSION	1


/* Parameter defaults */

#define COLORMASTER_GAMMA_MIN		0.000
#define COLORMASTER_GAMMA_MAX		3.000
#define COLORMASTER_GAMMA_DEF		1.000
#define COLORMASTER_COLOR_CHECK_DEF	TRUE
#define	COLORMASTER_INV_HUE_DEF		FALSE
#define	COLORMASTER_INV_SAT_DEF		FALSE
#define	COLORMASTER_INV_BRI_DEF		FALSE
#define COLORMASTER_8_TO_16_MULT	32767.0/255.0
#define COLORMASTER_16_TO_8_MULT	255.0/32767.0

enum {
	COLORMASTER_INPUT = 0,
	COLORMASTER_COLOR_CHECK,
	COLORMASTER_COLOR,
	COLORMASTER_GAMMA,
	COLORMASTER_INV_HUE,
	COLORMASTER_INV_SAT,
	COLORMASTER_INV_BRI,
	COLORMASTER_NUM_PARAMS
};

enum {
	COLOR_CHECK_DISK_ID = 1,
	COLOR_DISK_ID,
	GAMMA_DISK_ID,
	INV_HUE_DISK_ID,
	INV_SAT_DISK_ID,
	INV_BRI_DISK_ID
};

typedef struct ColorInfo {
	PF_Pixel8	color8;
	PF_Pixel16	color16;
	PF_Pixel32	color32;
	PF_Boolean	colorB,
				hueB,
				satB,
				briB;
	PF_FpLong	gammaF;
} ColorInfo, * ColorInfoP, ** ColorInfoH;

#ifdef __cplusplus
extern "C" {
#endif

	DllExport	PF_Err
		EntryPointFunc(
			PF_Cmd			cmd,
			PF_InData* in_data,
			PF_OutData* out_data,
			PF_ParamDef* params[],
			PF_LayerDef* output,
			void* extra);

#ifdef __cplusplus
}
#endif

#endif // COLORMASTER_H