#include "pch_script.h"
#include "torch.h"
#include "script_game_object.h"
#include "inventory_item_object.h"
#include "../LightAnimLibrary.h"

using namespace luabind;


#define  CLASS_IMPL		CTorch
#define  target_0		light_render
#define  target_1		light_omni
#define	 target_2		glow_render
#include "light_ext.inc"
#undef   target_0
#undef	 target_1
#undef	 target_2 
#undef   CLASS_IMPL


CTorch *get_torch(CScriptGameObject *script_obj)
{
	CGameObject *obj = &script_obj->object();
	CTorch* t = smart_cast<CTorch*> (obj);
	if (t) return t;
	script_obj = script_obj->GetObjectByName("device_torch");
	if (script_obj) return 
		get_torch (script_obj); // рекурсия
	return NULL;
}



#pragma optimize("s",on)
void CTorch::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CTorch,CInventoryItemObject>("CTorch")
			.def(constructor<>())
			// alpet: управление параметрами света
			.def_readonly("on"		,		&CTorch::m_switched_on)			
			.def("enable"			,		(void (CTorch::*)(bool)) (&CTorch::Switch))
			.def("switch"			,		(void (CTorch::*)())	 (&CTorch::Switch))			
			.def("get_light"		,		&CTorch::GetLight)
			.def("set_animation"	,		&CTorch::SetAnimation)
			.def("set_angle"		,		&CTorch::SetAngle)
			.def("set_brightness"	,		&CTorch::SetBrightness)
			.def("set_color"		,		&CTorch::SetColor)
			.def("set_rgb"			,		&CTorch::SetRGB)
			.def("set_range"		,		&CTorch::SetRange)			
			.def("set_texture"		,		&CTorch::SetTexture)
			.def("set_virtual_size" ,		&CTorch::SetVirtualSize)
			// работа с ПНВ
			.def_readonly("nvd_on"		,		&CTorch::m_bNightVisionOn)
			.def("enable_nvd"			,		(void (CTorch::*)(bool)) (&CTorch::SwitchNightVision))
			.def("switch_nvd"			,		(void (CTorch::*)()) (&CTorch::SwitchNightVision))
			
			,			
			def("get_torch_obj"		,		&get_torch) 
	];
}
