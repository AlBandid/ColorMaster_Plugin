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
//nanana master

static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	AEGP_SuiteHandler suites(in_data->pica_basicP); // this is the suite handler, he acquires and disposes of needed suites, magic!
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"%s v%d.%d\r%s",
											STR(StrID_Name), 
											MAJOR_VERSION, 
											MINOR_VERSION, 
											STR(StrID_Description));
	return PF_Err_NONE;
}

static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);

	out_data->out_flags =  PF_OutFlag_DEEP_COLOR_AWARE;	// just 16bpc, not 32bpc
	
	return PF_Err_NONE;
}

static PF_Err 
ParamsSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOXX(	STR(StrID_Color_Check_Param_Name),
						COLORMASTER_COLOR_CHECK_DEF,
						NULL,
						COLOR_CHECK_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_COLOR(	STR(StrID_Color_Param_Name), 
					0,
					PF_MAX_CHAN8,
					PF_MAX_CHAN8,
					COLOR_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_FLOAT_SLIDERX(	STR(StrID_Gamma_Param_Name),
							COLORMASTER_GAMMA_MIN,
							COLORMASTER_GAMMA_MAX,
							COLORMASTER_GAMMA_MIN,
							COLORMASTER_GAMMA_MAX,
							COLORMASTER_GAMMA_DEF,
							PF_Precision_THOUSANDTHS,
							0,
							0,
							GAMMA_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOXX(	STR(StrID_Invert_Hue_Param_Name),
						COLORMASTER_INV_HUE_DEF,
						NULL,
						INV_HUE_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOXX(	STR(StrID_Invert_Saturation_Param_Name),
						COLORMASTER_INV_SAT_DEF,
						NULL,
						INV_SAT_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOXX(	STR(StrID_Invert_Brightness_Param_Name),
						COLORMASTER_INV_BRI_DEF,
						NULL,
						INV_BRI_DISK_ID);
	
	out_data->num_params = COLORMASTER_NUM_PARAMS;

	return err;
}

static PF_Err convertRGBtoHLS(PF_Pixel* in_pix, PF_HLS_Pixel out_pixHLS)
{
	PF_Err		err = PF_Err_NONE;

	float hue{}, sat{}, bri{}, Cmax{}, Cmin{}, delta{};
	float r = in_pix->red / 255.0;
	float g = in_pix->green / 255.0;
	float b = in_pix->blue / 255.0;

	Cmin = MIN(r, MIN(g, b));
	Cmax = MAX(r, MAX(g, b));

	delta = Cmax - Cmin;

	// lightness conversion
	bri = (Cmax + Cmin) / 2;

	// hue conversion
	if (delta == 0)
	{
		hue = 0;
		sat = 0;
	}
	else
	{
		if (bri < 50) { sat = delta / (Cmax + Cmin); }
		else { sat = delta / (2 - Cmax - Cmin); }
		if (Cmax == r) { hue = 60 * fmodf(((g - b) / delta), 6); }
		if (Cmax == g) { hue = 60 * ((b - r) / delta + 2); }
		if (Cmax == b) { hue = 60 * ((r - g) / delta + 4); }
	}

	if (hue < 0)
	{
		hue += 360;
	}
	if (hue > 360)
	{
		hue = fmodf(hue, 360);
	}

	out_pixHLS[0] = hue;
	out_pixHLS[1] = bri * 100;
	out_pixHLS[2] = sat * 100;

	return err;
}
static PF_Err convertRGBtoHLS2(PF_Pixel in_pix, PF_HLS_Pixel out_pixHLS)
{
	PF_Err		err = PF_Err_NONE;

	float hue{}, sat{}, bri{}, Cmax{}, Cmin{}, delta{};
	float r = in_pix.red / 255.0;
	float g = in_pix.green / 255.0;
	float b = in_pix.blue / 255.0;

	Cmin = MIN(r, MIN(g, b));
	Cmax = MAX(r, MAX(g, b));

	delta = Cmax - Cmin;

	// lightness conversion
	bri = (Cmax + Cmin) / 2;

	// hue conversion
	if (delta == 0)
	{
		hue = 0;
		sat = 0;
	}
	else
	{
		if (bri < 50) { sat = delta / (Cmax + Cmin); }
		else { sat = delta / (2 - Cmax - Cmin); }
		if (Cmax == r) { hue = 60 * fmodf(((g - b) / delta), 6); }
		if (Cmax == g) { hue = 60 * ((b - r) / delta + 2); }
		if (Cmax == b) { hue = 60 * ((r - g) / delta + 4); }
	}

	if (hue < 0)
	{
		hue += 360;
	}
	if (hue > 360)
	{
		hue = fmodf(hue, 360);
	}

	out_pixHLS[0] = hue;
	out_pixHLS[1] = bri * 100;
	out_pixHLS[2] = sat * 100;

	return err;
}

static PF_Err convertHLStoRGB(PF_HLS_Pixel in_pixHLS, PF_Pixel8* out_pix8)
{
	PF_Err		err = PF_Err_NONE;

	// convert back to rgb code here
	float C, X, m, hue = in_pixHLS[0], sat = in_pixHLS[2], bri = in_pixHLS[1];
	C = (100 - ABS(2 * bri - 100)) * sat / 100;
	X = C * (1 - ABS(fmodf((hue / 60), 2) - 1));
	m = bri - C / 2;
	float r;
	float g;
	float b;

	if (hue >= 0 && hue < 60)
	{
		r = C;
		g = X;
		b = 0;
	}
	else if (hue >= 60 && hue < 120)
	{
		r = X;
		g = C;
		b = 0;
	}
	else if (hue >= 120 && hue < 180)
	{
		r = 0;
		g = C;
		b = X;
	}
	else if (hue >= 180 && hue < 240)
	{
		r = 0;
		g = X;
		b = C;
	}
	else if (hue >= 240 && hue < 300)
	{
		r = X;
		g = 0;
		b = C;
	}
	else if (hue >= 300 && hue < 360)
	{
		r = C;
		g = 0;
		b = X;
	}

	out_pix8->red = (r + m) * 255 / 100;
	out_pix8->green = (g + m) * 255 / 100;
	out_pix8->blue = (b + m) * 255 / 100;

	return err;
}
static PF_Err convertHLStoRGB2(PF_HLS_Pixel in_pixHLS, PF_Pixel8 out_pix8)
{
	PF_Err		err = PF_Err_NONE;

	// convert back to rgb code here
	float C, X, m, hue = in_pixHLS[0], sat = in_pixHLS[2], bri = in_pixHLS[1];
	C = (100 - ABS(2 * bri - 100)) * sat / 100;
	X = C * (1 - ABS(fmodf((hue / 60), 2) - 1));
	m = bri - C / 2;
	float r;
	float g;
	float b;

	if (hue >= 0 && hue < 60)
	{
		r = C;
		g = X;
		b = 0;
	}
	else if (hue >= 60 && hue < 120)
	{
		r = X;
		g = C;
		b = 0;
	}
	else if (hue >= 120 && hue < 180)
	{
		r = 0;
		g = C;
		b = X;
	}
	else if (hue >= 180 && hue < 240)
	{
		r = 0;
		g = X;
		b = C;
	}
	else if (hue >= 240 && hue < 300)
	{
		r = X;
		g = 0;
		b = C;
	}
	else if (hue >= 300 && hue < 360)
	{
		r = C;
		g = 0;
		b = X;
	}

	out_pix8.red = (r + m) * 255 / 100;
	out_pix8.green = (g + m) * 255 / 100;
	out_pix8.blue = (b + m) * 255 / 100;

	return err;
}

PF_Pixel16 convertBPC8to16(PF_Pixel8 in_pix8)
{
	PF_Pixel16 out_pix16;
	out_pix16.alpha = in_pix8.alpha * 128.498;
	out_pix16.red = in_pix8.red * 128.498;
	out_pix16.green = in_pix8.green * 128.498;
	out_pix16.blue = in_pix8.blue * 128.498;

	return out_pix16;
}

PF_Pixel8 convertBPC16to8(PF_Pixel16* in_pix16)
{
	PF_Pixel8 out_pix8;
	out_pix8.alpha = in_pix16->alpha * .00778;
	out_pix8.red = in_pix16->red * .00778;
	out_pix8.green = in_pix16->green * .00778;
	out_pix8.blue = in_pix16->blue * .00778;

	return out_pix8;
}
PF_Pixel8 convertBPC16to8v2(PF_Pixel16 in_pix16)
{
	PF_Pixel8 out_pix8;
	out_pix8.alpha = in_pix16.alpha * .00778;
	out_pix8.red = in_pix16.red * .00778;
	out_pix8.green = in_pix16.green * .00778;
	out_pix8.blue = in_pix16.blue * .00778;

	return out_pix8;
}

static PF_Err
ColorFunction8(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel8* inP,
	PF_Pixel8* outP)
{
	PF_Err		err = PF_Err_NONE;

	ColorInfo* ciP = reinterpret_cast<ColorInfo*>(refcon);
	PF_Pixel8 color = ciP->color8;
	PF_HLS_Pixel in_hls;
	PF_HLS_Pixel color_hls;
	convertRGBtoHLS(inP, in_hls);
	convertRGBtoHLS2(color, color_hls);
	in_hls[0] = color_hls[0];

	convertHLStoRGB(in_hls, outP);
	outP->alpha = inP->alpha;

	return err;
}

static PF_Err
ColorFunction16(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel16* inP,
	PF_Pixel16* outP)
{
	PF_Err		err = PF_Err_NONE;

	ColorInfo* ciP = reinterpret_cast<ColorInfo*>(refcon);
	PF_Pixel16 color = ciP->color16;
	PF_Pixel8 inP8 = convertBPC16to8(inP);
	PF_Pixel8 outP8;
	PF_Pixel8 color8 = convertBPC16to8v2(color);
	PF_HLS_Pixel in_hls;
	PF_HLS_Pixel color_hls;
	convertRGBtoHLS2(inP8, in_hls);
	convertRGBtoHLS2(color8, color_hls);
	in_hls[0] = color_hls[0];

	convertHLStoRGB2(in_hls, outP8);
	outP8.alpha = inP8.alpha;

	outP->red = outP8.red * 128.498;
	outP->green = outP8.green * 128.498;
	outP->blue = outP8.blue * 128.498;
	outP->alpha = outP8.alpha * 128.498;

	return err;
}

static PF_Err
GammaFunction8(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel8* inP,
	PF_Pixel8* outP)
{
	PF_Err		err = PF_Err_NONE;

	ColorInfo* ciP = reinterpret_cast<ColorInfo*>(refcon);
	double	gammaF = ciP->gammaF;

	double red{}, green{}, blue{};

	//red
	red = pow(inP->red, gammaF); // mno¿enie przez gamma
	if (red > 255) { red = 255; } // zabezpieczenie przed wyjsciem poza zakres

	//green
	green = pow(inP->green, gammaF);
	if (green > 255) { green = 255; }

	//blue
	blue = pow(inP->blue, gammaF);
	if (blue > 255) { blue = 255; }

	if (isnan(red)) { red = 0; }
	if (isnan(green)) { green = 0; }
	if (isnan(blue)) { blue = 0; }

	outP->red = red;
	outP->green = green;
	outP->blue = blue;
	outP->alpha = inP->alpha;

	return err;
}

static PF_Err
GammaFunction16(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel16* inP,
	PF_Pixel16* outP)
{
	PF_Err		err = PF_Err_NONE;

	ColorInfo* ciP = reinterpret_cast<ColorInfo*>(refcon);
	double	gammaF = ciP->gammaF;

	double red, green, blue;

	//red
	red = pow(inP->red * .00778, gammaF) * 128.498;
	if (red > 32767) { red = 32767; } // zabezpieczenie przed wyjsciem poza zakres

	//green
	green = pow(inP->green * .00778, gammaF) * 128.498;
	if (green > 32767) { green = 32767; }

	//blue
	blue = pow(inP->blue * .00778, gammaF) * 128.498;
	if (blue > 32767) { blue = 32767; }

	if (isnan(red)) { red = 0; }	// isnan -> is not a number
	if (isnan(green)) { green = 0; }
	if (isnan(blue)) { blue = 0; }

	outP->red = red;
	outP->green = green;
	outP->blue = blue;
	outP->alpha = inP->alpha;

	return err;
}

static PF_Err
Invert8(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel8* inP,
	PF_Pixel8* out_pix8)
{
	PF_Err		err = PF_Err_NONE;

	ColorInfo* ciP = reinterpret_cast<ColorInfo*>(refcon);

	PF_HLS_Pixel hls;
	convertRGBtoHLS(inP, hls);

	A_long hue = hls[0], sat = hls[2], bri = hls[1];

	if (ciP->hueB == TRUE)
	{
		hue = (hue + 180) % 360;
	}
	if (ciP->briB == TRUE)
	{
		bri = 100 - bri;
	}
	if (ciP->satB == TRUE)
	{
		sat = 100 - sat;
	}

	hls[0] = hue;
	hls[1] = bri;
	hls[2] = sat;

	convertHLStoRGB(hls, out_pix8);

	return err;
}

static PF_Err
Invert16(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel16* inP,
	PF_Pixel16* out_pix16)
{
	PF_Err		err = PF_Err_NONE;

	ColorInfo* ciP = reinterpret_cast<ColorInfo*>(refcon);
	A_long Cmax{}, Cmin{}, delta{}, C{}, X{}, m{};

	PF_Pixel8 tmp8 = convertBPC16to8(inP);
	PF_HLS_Pixel hls;
	convertRGBtoHLS2(tmp8, hls);

	A_long hue = hls[0], sat = hls[2], bri = hls[1];

	if (ciP->hueB == TRUE)
	{
		hue = (hue + 180) % 360;
	}
	if (ciP->briB == TRUE)
	{
		bri = 100 - bri;
	}
	if (ciP->satB == TRUE)
	{
		sat = 100 - sat;
	}

	hls[0] = hue;
	hls[1] = bri;
	hls[2] = sat;

	convertHLStoRGB2(hls, tmp8);

	out_pix16->red = tmp8.red * 128.498;
	out_pix16->green = tmp8.green * 128.498;
	out_pix16->blue = tmp8.blue * 128.498;
	out_pix16->alpha = inP->alpha;

	return err;
}

static PF_Err
Render(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output)
{
	PF_Err				err = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);
	PF_Handle infoH = suites.HandleSuite1()->host_new_handle(sizeof(ColorInfo));

	ColorInfo			ciP;
	AEFX_CLR_STRUCT(ciP);
	A_long				linesL = 0;

	linesL = output->extent_hint.bottom - output->extent_hint.top;	//wylicza nam maksymalny Y warstwy 

	//przyjmujemy parametry u¿ytkownika z UI do naszej struktury
	ciP.color8 = params[COLORMASTER_COLOR]->u.cd.value;
	ciP.color16 = convertBPC8to16(ciP.color8);
	ciP.gammaF = params[COLORMASTER_GAMMA]->u.fs_d.value;
	ciP.colorB = params[COLORMASTER_COLOR_CHECK]->u.bd.value;
	ciP.hueB = params[COLORMASTER_INV_HUE]->u.bd.value;
	ciP.satB = params[COLORMASTER_INV_SAT]->u.bd.value;
	ciP.briB = params[COLORMASTER_INV_BRI]->u.bd.value;


	if (PF_WORLD_IS_DEEP(output)) {
		
		if (ciP.colorB)
		{
			ERR(suites.Iterate16Suite1()->iterate(	in_data,
													0,									// progress base
													linesL,								// progress final
													&params[COLORMASTER_INPUT]->u.ld,	// src 
													NULL,								// area - null for all pixels
													(void*)&ciP,						// refcon - your custom data pointer
													ColorFunction16,					// pixel function pointer
													output));
			ERR(suites.Iterate16Suite1()->iterate(in_data,
				0,									// progress base
				linesL,								// progress final
				output,	// src 
				NULL,								// area - null for all pixels
				(void*)&ciP,						// refcon - your custom data pointer
				GammaFunction16,					// pixel function pointer
				output));
		}
		else
		{
			ERR(suites.Iterate16Suite1()->iterate(in_data,
				0,									// progress base
				linesL,								// progress final
				&params[COLORMASTER_INPUT]->u.ld,	// src 
				NULL,								// area - null for all pixels
				(void*)&ciP,						// refcon - your custom data pointer
				GammaFunction16,					// pixel function pointer
				output));
		}
		
		if (ciP.hueB || ciP.satB || ciP.briB)
		{
			ERR(suites.Iterate16Suite1()->iterate(	in_data,
													0,									// progress base
													linesL,								// progress final
													output,	// src 
													NULL,								// area - null for all pixels
													(void*)&ciP,						// refcon - your custom data pointer
													Invert16,							// pixel function pointer
													output));
		}
	}
	else {

		if (ciP.colorB)
		{
			ERR(suites.Iterate8Suite1()->iterate(	in_data,
													0,									// progress base
													linesL,								// progress final
													&params[COLORMASTER_INPUT]->u.ld,	// src 
													NULL,								// area - null for all pixels
													(void*)&ciP,						// refcon - your custom data pointer
													ColorFunction8,					// pixel function pointer
													output));
			ERR(suites.Iterate8Suite1()->iterate(in_data,
				0,									// progress base
				linesL,								// progress final
				output,								// src 
				NULL,								// area - null for all pixels
				(void*)&ciP,						// refcon - your custom data pointer
				GammaFunction8,						// pixel function pointer
				output));
		}
		else
		{
			ERR(suites.Iterate8Suite1()->iterate(in_data,
				0,									// progress base
				linesL,								// progress final
				&params[COLORMASTER_INPUT]->u.ld,	// src 
				NULL,								// area - null for all pixels
				(void*)&ciP,						// refcon - your custom data pointer
				GammaFunction8,						// pixel function pointer
				output));
		}
		

		if (ciP.hueB || ciP.satB || ciP.briB)
		{
			ERR(suites.Iterate8Suite1()->iterate(	in_data,
													0,									// progress base
													linesL,								// progress final
													output,								// src 
													NULL,								// area - null for all pixels
													(void*)&ciP,						// refcon - your custom data pointer
													Invert8,							// pixel function pointer
													output));
		}
	}

	return err;
}


DllExport	
PF_Err 
EntryPointFunc (
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;
	
	try {
		switch (cmd) {
			case PF_Cmd_ABOUT:

				err = About(in_data,
							out_data,
							params,
							output);
				break;
				
			case PF_Cmd_GLOBAL_SETUP:

				err = GlobalSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_PARAMS_SETUP:

				err = ParamsSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_RENDER:

				err = Render(	in_data,
								out_data,
								params,
								output);
				break;
		}
	}
	catch(PF_Err &thrown_err){
		err = thrown_err;
	}
	return err;
}

