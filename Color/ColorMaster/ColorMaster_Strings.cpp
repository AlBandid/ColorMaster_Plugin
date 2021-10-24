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

#include "ColorMaster.h"

typedef struct {
	A_u_long	index;
	A_char		str[256];
} TableString;



TableString		g_strs[StrID_NUMTYPES] = {
	StrID_NONE,							"",
	StrID_Name,							"ColorMaster",
	StrID_Description,					"A coloring and color inverting effect,\rfor your colorful work.\rAleksandra Koziol.",
	StrID_Color_Check_Param_Name,		"Change tint?",
	StrID_Color_Param_Name,				"Color",
	StrID_Invert_Hue_Param_Name,		"Invert HUE",
	StrID_Invert_Saturation_Param_Name,	"Invert SAT",
	StrID_Invert_Brightness_Param_Name,	"Invert BRI",
	StrID_Gamma_Param_Name,				"Gamma",
};


char* GetStringPtr(int strNum)
{
	return g_strs[strNum].str;
}
