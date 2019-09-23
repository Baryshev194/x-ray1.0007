#include "stdafx.h"
#pragma hdrstop

#include "Blender_SSLR.h"

CBlender_SSLR::CBlender_SSLR() { description.CLS = 0; }
CBlender_SSLR::~CBlender_SSLR() {}

void CBlender_SSLR::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch(C.iElement)
	{
	case 0:
		C.r_Pass("null", "SSLR", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_rtf("s_normal", r2_RT_N);
		C.r_Sampler_clf("s_image", r2_RT_generic0);
		C.r_End();
		break;
	}
}