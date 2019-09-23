////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes_script.cpp
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "base_client_classes.h"
#include "base_client_classes_wrappers.h"
#include "../feel_sound.h"
#include "../fbasicvisual.h"
#include "../skeletonanimated.h"
#include "ai/stalker/ai_stalker.h"
#include "../../xrNetServer/net_utils.h"
#include "../ResourceManager.h"
#include "../device.h"
#include "../Render.h"
#include "Actor.h"
#include "Explosive.h"
#include "inventory_item.h"
#include "script_game_object.h"
#include "xrServer_Space.h"
#include "xrServer_Objects_ALife.h"
#include "../lua_tools.h"
#include <typeinfo>


struct CGlobalFlags { };

using namespace luabind;

#pragma optimize("s",on)
void SLargeInteger::script_register(lua_State *L)
{
	module(L)
		[
			class_<SLargeInteger>("Int64")
			.def_readwrite("low"			,				&SLargeInteger::LowPart)
			.def_readwrite("high"			,				&SLargeInteger::HighPart)
			.def("to_string"				,				&SLargeInteger::to_string)
		];		
}

void DLL_PureScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<DLL_Pure,CDLL_PureWrapper>("DLL_Pure")
			.def(constructor<>())
			.def("_construct",&DLL_Pure::_construct,&CDLL_PureWrapper::_construct_static)
	];
}

/*
void ISpatialScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ISpatial,CISpatialWrapper>("ISpatial")
			.def(constructor<>())
			.def("spatial_register",	&ISpatial::spatial_register,	&CISpatialWrapper::spatial_register_static)
			.def("spatial_unregister",	&ISpatial::spatial_unregister,	&CISpatialWrapper::spatial_unregister_static)
			.def("spatial_move",		&ISpatial::spatial_move,		&CISpatialWrapper::spatial_move_static)
			.def("spatial_sector_point",&ISpatial::spatial_sector_point,&CISpatialWrapper::spatial_sector_point_static)
			.def("dcast_CObject",		&ISpatial::dcast_CObject,		&CISpatialWrapper::dcast_CObject_static)
			.def("dcast_FeelSound",		&ISpatial::dcast_FeelSound,		&CISpatialWrapper::dcast_FeelSound_static)
			.def("dcast_Renderable",	&ISpatial::dcast_Renderable,	&CISpatialWrapper::dcast_Renderable_static)
			.def("dcast_Light",			&ISpatial::dcast_Light,			&CISpatialWrapper::dcast_Light_static)
	];
}
*/


void set_object_schedule(ISheduled* obj, u32 t_min, u32 t_max)
{
	obj->shedule.t_min = t_min;
	obj->shedule.t_max = t_max;
}

u32 get_schedule_max(ISheduled* obj) { return obj->shedule.t_max; }
u32 get_schedule_min(ISheduled* obj) { return obj->shedule.t_min; }


void ISheduledScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ISheduled,CISheduledWrapper>("ISheduled")
		.def_readwrite("updated_times",			&ISheduled::updated_times)		 	 // может потребоватьс€ сброс счетчиков после сверки	
		.property("schedule_max",				&get_schedule_max)
		.property("schedule_min",				&get_schedule_min)
		.def("set_schedule",					&set_object_schedule)                // дл€ подбора оптимального интервала апдейта объекта, например в зависимости дистанции до актора
		
//			.def(constructor<>())
//			.def("shedule_Scale",		&ISheduled::shedule_Scale,		&CISheduledWrapper::shedule_Scale_static)
//			.def("shedule_Update",		&ISheduled::shedule_Update,		&CISheduledWrapper::shedule_Update_static)
	];
}

void IRenderableScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<IRenderable,CIRenderableWrapper>("IRenderable")
//			.def(constructor<>())
//			.def("renderable_Render",&IRenderable::renderable_Render,&CIRenderableWrapper::renderable_Render_static)
//			.def("renderable_ShadowGenerate",&IRenderable::renderable_ShadowGenerate,&CIRenderableWrapper::renderable_ShadowGenerate_static)
//			.def("renderable_ShadowReceive",&IRenderable::renderable_ShadowReceive,&CIRenderableWrapper::renderable_ShadowReceive_static)
	];
}

void ICollidableScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ICollidable>("ICollidable")
			.def(constructor<>())
	];
}


IC u32 get_inventory_item_flags(CGameObject* O)
{
	CInventoryItem *II = smart_cast<CInventoryItem*> (O);
	if (II) return II->m_flags.flags;
	return 0;
}

IC void set_inventory_item_flags(CGameObject* O, u16 flags)
{
	CInventoryItem *II = smart_cast<CInventoryItem*> (O);
	if (II) II->m_flags.assign ( (u16) flags);	
}

LPCSTR get_obj_class_name(CGameObject *obj) { return obj->CppClassName(); }

IC LPCSTR get_class_id(CGameObject *obj)

{
	static string16 result;
	CLSID2TEXT(obj->CLS_ID, result);
	return result;
}

DLL_API LPCSTR raw_lua_class_name(lua_State *L)
{
	using namespace luabind::detail;
	for (int i = lua_gettop(L) - 1; i > 0; i--) // обычно аргумент (-2) ,    int(CSE_ALifeObject:: объект
	if (lua_isuserdata(L, i))
	{
		object_rep *rep = is_class_object(L, i); 
		if (rep)			
			return rep->crep()->name();		
	}
	return "?";
}


LPCSTR get_lua_class_name(luabind::object O)
{			
	lua_State *L = O.lua_state();				
	if (L)
		return raw_lua_class_name(L);	
	return "?";
}


Fvector rotation_get_dir(SRotation *R, bool v_inverse)
{
	Fvector result;
	if (v_inverse)
		result.setHP(R->yaw, -R->pitch);
	else
		result.setHP(R->yaw, R->pitch);
	return result;
}

void	rotation_set_dir(SRotation *R, const Fvector &dir, bool v_inverse)
{
	R->yaw   = dir.getH(); 
	if (v_inverse)
		R->pitch = -dir.getP();
	else
		R->pitch =  dir.getP();
	R->roll  = 0;
}

void    rotation_copy(SRotation *R, SRotation *src) { memcpy(R, src, sizeof(SRotation)); }
void	rotation_init(SRotation *R, float y, float p, float r)
{
	R->pitch = p;
	R->roll = r;
	R->yaw = y;
}

void CRotationScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<SRotation>("SRotation")
			.def ( constructor<>() )
			.def ( constructor<float, float, float>() )
			.def_readwrite("pitch"				,				&SRotation::pitch)	
			.def_readwrite("yaw"				,				&SRotation::yaw)	
			.def_readwrite("roll"				,				&SRotation::roll)				
			.def("get_dir"						,				&rotation_get_dir)
			.def("set_dir"						,				&rotation_set_dir)
			.def("set"							,				&rotation_copy)
			.def("set"							,				&rotation_init)
		];
}


void CObjectScript::script_register		(lua_State *L)
{
	module(L)
	[
//		class_<CObject,bases<DLL_Pure,ISheduled,ICollidable,IRenderable>,CObjectWrapper>("CObject")
//			.def(constructor<>())
//			.def("_construct",			&CObject::_construct,&CObjectWrapper::_construct_static)
/*			
			.def("spatial_register",	&CObject::spatial_register,	&CObjectWrapper::spatial_register_static)
			.def("spatial_unregister",	&CObject::spatial_unregister,	&CObjectWrapper::spatial_unregister_static)
			.def("spatial_move",		&CObject::spatial_move,		&CObjectWrapper::spatial_move_static)
			.def("spatial_sector_point",&CObject::spatial_sector_point,&CObjectWrapper::spatial_sector_point_static)
			.def("dcast_FeelSound",		&CObject::dcast_FeelSound,		&CObjectWrapper::dcast_FeelSound_static)
			.def("dcast_Light",			&CObject::dcast_Light,			&CObjectWrapper::dcast_Light_static)
*/			
//			.def("shedule_Scale",		&CObject::shedule_Scale,		&CObjectWrapper::shedule_Scale_static)
//			.def("shedule_Update",		&CObject::shedule_Update,		&CObjectWrapper::shedule_Update_static)

//			.def("renderable_Render"		,&CObject::renderable_Render,&CObjectWrapper::renderable_Render_static)
//			.def("renderable_ShadowGenerate",&CObject::renderable_ShadowGenerate,&CObjectWrapper::renderable_ShadowGenerate_static)
//			.def("renderable_ShadowReceive",&CObject::renderable_ShadowReceive,&CObjectWrapper::renderable_ShadowReceive_static)
//			.def("Visual",					&CObject::Visual)

		class_<CGameObject,bases<DLL_Pure,ISheduled,ICollidable,IRenderable>,CGameObjectWrapper>("CGameObject")
			.def(constructor<>())
			.def("_construct",			&CGameObject::_construct,&CGameObjectWrapper::_construct_static)
			.def("Visual",				&CGameObject::Visual)			
/*
			.def("spatial_register",	&CGameObject::spatial_register,	&CGameObjectWrapper::spatial_register_static)
			.def("spatial_unregister",	&CGameObject::spatial_unregister,	&CGameObjectWrapper::spatial_unregister_static)
			.def("spatial_move",		&CGameObject::spatial_move,		&CGameObjectWrapper::spatial_move_static)
			.def("spatial_sector_point",&CGameObject::spatial_sector_point,&CGameObjectWrapper::spatial_sector_point_static)
			.def("dcast_FeelSound",		&CGameObject::dcast_FeelSound,		&CGameObjectWrapper::dcast_FeelSound_static)
			.def("dcast_Light",			&CGameObject::dcast_Light,			&CGameObjectWrapper::dcast_Light_static)
*/
//			.def("shedule_Scale",		&CGameObject::shedule_Scale,		&CGameObjectWrapper::shedule_Scale_static)
//			.def("shedule_Update",		&CGameObject::shedule_Update,		&CGameObjectWrapper::shedule_Update_static)

//			.def("renderable_Render"		,&CGameObject::renderable_Render,&CGameObjectWrapper::renderable_Render_static)
//			.def("renderable_ShadowGenerate",&CGameObject::renderable_ShadowGenerate,&CGameObjectWrapper::renderable_ShadowGenerate_static)
//			.def("renderable_ShadowReceive",&CGameObject::renderable_ShadowReceive,&CGameObjectWrapper::renderable_ShadowReceive_static)

			.def("net_Export",			&CGameObject::net_Export,		&CGameObjectWrapper::net_Export_static)
			.def("net_Import",			&CGameObject::net_Import,		&CGameObjectWrapper::net_Import_static)
			.def("net_Spawn",			&CGameObject::net_Spawn,		&CGameObjectWrapper::net_Spawn_static)

			.def("use",					&CGameObject::use,	&CGameObjectWrapper::use_static)

//			.def("setVisible",			&CGameObject::setVisible)
			.def("getVisible",			&CGameObject::getVisible)
			.def("getEnabled",			&CGameObject::getEnabled)
			
			// alpet: дополнительные свойства 
			
			.def("test_server_flag"			,					&CGameObject::TestServerFlag)			
			.property	 ("se_object"		,					&CGameObject::alife_object)
			.property	 ("item_flags"		,					&get_inventory_item_flags,  &set_inventory_item_flags)     // ???! проблемы с кастингом, не позвол€ют оставить это свойство в CInventoryItem
			.property	 ("class_name"		,					&get_lua_class_name)
			.property	 ("class_id"		,					&get_class_id)
			
			.def("load_config"				,					&CGameObject::Load)
#ifdef   OBJECTS_RADIOACTIVE
		.def_readwrite("radiation_accum_factor"			,			&CGameObject::m_fRadiationAccumFactor)
		.def_readwrite("radiation_accum_limit"			,			&CGameObject::m_fRadiationAccumLimit)
		.def_readwrite("radiation_restore_speed"		,			&CGameObject::m_fRadiationRestoreSpeed)
#endif
//			.def("setEnabled",			&CGameObject::setEnabled)
			,
			
			class_<CGlobalFlags>("global_flags")  // дл€ оптимальности доступа, предполагаетс€ в скриптах скопировать элементы этого "класса" в пространство имен _G 
			.enum_("explosion")
			[
				value("flExploding"				,				int(1)),
				value("flExplodEventSent"		,				int(2)),
				value("flReadyToExplode"		,				int(4)),
				value("flExploded"				,				int(8))
			]
			.enum_("inventory_item")
			[
				value("FdropManual"				,				int(CInventoryItem::EIIFlags::FdropManual)),
				value("FCanTake"				,				int(CInventoryItem::EIIFlags::FCanTake)),
				value("FCanTrade"				,				int(CInventoryItem::EIIFlags::FCanTrade)),
				value("Fbelt"					,				int(CInventoryItem::EIIFlags::Fbelt)),
				value("Fruck"					,				int(CInventoryItem::EIIFlags::Fruck)),
				value("FRuckDefault"			,				int(CInventoryItem::EIIFlags::FRuckDefault)),
				value("FUsingCondition"			,				int(CInventoryItem::EIIFlags::FUsingCondition)),
				value("FAllowSprint"			,				int(CInventoryItem::EIIFlags::FAllowSprint)),
				value("Fuseful_for_NPC"			,				int(CInventoryItem::EIIFlags::Fuseful_for_NPC)),
				value("FInInterpolation"		,				int(CInventoryItem::EIIFlags::FInInterpolation)),
				value("FInInterpolate"			,				int(CInventoryItem::EIIFlags::FInInterpolate)),
				value("FIsQuestItem"			,				int(CInventoryItem::EIIFlags::FIsQuestItem)),
				value("FIAlwaysTradable"		,				int(CInventoryItem::EIIFlags::FIAlwaysTradable)),
				value("FIAlwaysUntradable"		,				int(CInventoryItem::EIIFlags::FIAlwaysUntradable)),
				value("FIUngroupable"			,				int(CInventoryItem::EIIFlags::FIUngroupable)),
				value("FIManualHighlighting"	,				int(CInventoryItem::EIIFlags::FIManualHighlighting))
			]
			.enum_("se_object_flags")
			[
				value("flUseSwitches"			,				int(CSE_ALifeObject:: flUseSwitches)),
				value("flSwitchOnline"			,				int(CSE_ALifeObject:: flSwitchOnline)),
				value("flSwitchOffline"			,				int(CSE_ALifeObject:: flSwitchOffline)),
				value("flInteractive"			,				int(CSE_ALifeObject:: flInteractive)),
				value("flVisibleForAI"			,				int(CSE_ALifeObject:: flVisibleForAI)),
				value("flUsefulForAI"			,				int(CSE_ALifeObject:: flUsefulForAI)),
				value("flOfflineNoMove"			,				int(CSE_ALifeObject:: flOfflineNoMove)),
				value("flUsedAI_Locations"		,				int(CSE_ALifeObject:: flUsedAI_Locations)),
				value("flGroupBehaviour"		,				int(CSE_ALifeObject:: flGroupBehaviour)),
				value("flCanSave"				,				int(CSE_ALifeObject:: flCanSave)),
				value("flVisibleForMap"			,				int(CSE_ALifeObject:: flVisibleForMap)),
				value("flUseSmartTerrains"		,				int(CSE_ALifeObject:: flUseSmartTerrains)),
				value("flCheckForSeparator"		,				int(CSE_ALifeObject:: flCheckForSeparator))
			]


//		,class_<CPhysicsShellHolder,CGameObject>("CPhysicsShellHolder")
//			.def(constructor<>())

//		,class_<CEntity,CPhysicsShellHolder,CEntityWrapper>("CEntity")
//			.def(constructor<>())
//			.def("HitSignal",&CEntity::HitSignal,&CEntityWrapper::HitSignal_static)
//			.def("HitImpulse",&CEntity::HitImpulse,&CEntityWrapper::HitImpulse_static)

//		,class_<CEntityAlive,CEntity>("CEntityAlive")
//			.def(constructor<>())

//		,class_<CCustomMonster,CEntityAlive>("CCustomMonster")
//			.def(constructor<>())

//		,class_<CAI_Stalker,CCustomMonster>("CAI_Stalker")
	];
}


// alpet ======================== SCRIPT_TEXTURE_CONTROL BEGIN =========== 

IRender_Visual	*visual_get_child(IRender_Visual	*v, u32 n_child)
{

	if (!v) return NULL; // not have visual
	CKinematics *k = dynamic_cast<CKinematics*> (v);
	if (!k) return NULL;
	if (n_child >= k->children.size()) 		return NULL;
	return k->children.at(n_child);
}


// alpet: выборка текстуры из визуала по индексу
CTexture* visual_get_texture(IRender_Visual	*child_v, int n_texture)
{
	if (!child_v) return NULL; // not have visual
	
	CTexture* list[256] = { 0 };
	int tex_count = 0;

	n_texture = (n_texture > 255) ? 255 : n_texture;
	list[n_texture] = NULL;

	u32 verbose = 1;

	// визуал выстраиваетс€ иерархически - есть потомки и предки
	
	Shader* s = child_v->shader_ref._get();

	if (verbose > 5) Msg("# Shader *S = 0x%p name = <%s> ", s, child_v->shader_name); // shader расшарен дл€ всех визуалов с одинаковыми текстурами и моделью

	if (s && s->E[0]._get()) // обычно в первом элементе находитс€ исчерпывающий список текстур 
	{
		ShaderElement* E = s->E[0]._get();
		if (verbose > 5) Msg("#  ShaderElement *E = 0x%p", E);
		for (u32 p = 0; p < E->passes.size(); p++)
		if (E->passes[p]._get())
		{
			SPass* pass = E->passes[p]._get();
			if (verbose > 5) Msg("#   SPass *pass = 0x%p", pass);
			STextureList* tlist = pass->T._get();
			if (!tlist) continue;
			if (verbose > 5) Msg("#   STextureList *tlist = 0x%p, size = %d ", tlist, tlist->size());

			for (u32 t = 0; t < tlist->size() && tex_count <= n_texture; t++)
				list[tex_count++] = tlist->at(t).second._get();
		}
	}



	return list[n_texture];
}

void visual_configure (IRender_Visual	*child_v, LPCSTR new_shader, LPCSTR new_texture)
{
	if ((new_shader  && child_v->textures.size()) ||
		(new_texture && child_v->shader_name.size()))
	{
		// здесь производитс€ замена текстуры и/или шейдера визуала
		s32 last_skinning = Render->m_skinning;

		Render->shader_option_skinning(1); // черт значит зачем нужно замен€ть флаг, но при дефолтном -1 вместо наложени€ текстуры будет покраска визуала  серым 
		child_v->shader_ref.destroy();

		LPCSTR sh_name = new_shader ? new_shader : *child_v->shader_name;
		LPCSTR tx_name = new_texture ? new_texture : *child_v->textures;

		child_v->shader_ref.create(sh_name, tx_name);
		child_v->shader_name = sh_name;
		child_v->textures = tx_name;

		Render->shader_option_skinning(last_skinning);
	}
}

void visual_set_texture(IRender_Visual *v, LPCSTR replace) { visual_configure(v, NULL, replace); }

void visual_set_shader(IRender_Visual *v,  LPCSTR replace) { visual_configure(v, replace, NULL); }

void IRender_VisualScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<IRender_Visual>("IRender_Visual")
			.def(constructor<>())
			.def("dcast_PKinematicsAnimated", &IRender_Visual::dcast_PKinematicsAnimated)
			.def("dcast_PKinematics",		  &IRender_Visual::dcast_PKinematics)
			.def("child",					  &visual_get_child)
			.def("configure",				  &visual_configure)
			
			.def("get_texture",				  &visual_get_texture)
			

			.def("get_shader_name",			  &IRender_Visual::get_shader_name)
			.def("get_texture_name",		  &IRender_Visual::get_texture_name)			
			.def("set_shader_name",			  &visual_set_shader)
			.def("set_texture_name",		  &visual_set_texture)

		];
}


void CKinematicsAnimated_PlayCycle(CKinematicsAnimated* sa, LPCSTR anim)
{
	sa->PlayCycle(anim);
}

u16 get_bone_id(CKinematics *K, LPCSTR bone_name) { return K->LL_BoneID(bone_name); }


Fvector CalcBonePosition(CKinematics *K, const Fvector &pos, LPCSTR bone_name)
{
	u16					bone_id;
	if (xr_strlen(bone_name))
		bone_id			= K->LL_BoneID(bone_name);
	else
		bone_id			= K->LL_GetBoneRoot();

	Fmatrix				matrix;
	Fmatrix				XF;
	XF.identity();
	XF.translate_over (pos);
	matrix.mul_43		(XF,  K->LL_GetTransform(bone_id));
	return				(matrix.c);
}

const SLargeInteger get_bones_visible(CKinematics *K)
{	
	SLargeInteger r;
	r.QuadPart = K->LL_GetBonesVisible();
	return r;
}

bool get_bone_visible_by_id(CKinematics *K, u16 bone_id)
{
	if (bone_id != BI_NONE)	return K->LL_GetBoneVisible(bone_id);
	return false;
}

bool get_bone_visible_by_name(CKinematics *K, LPCSTR bone_name)
{
	u16 bone_id = K->LL_BoneID (bone_name);
	return get_bone_visible_by_id(K, bone_id);	
}

void set_bone_visible_by_id   (CKinematics *K, u16 bone_id, bool visible, bool recursive = true)
{ 
	if (bone_id != BI_NONE) K->LL_SetBoneVisible(bone_id, visible, recursive); 
}
void set_bone_visible_by_name (CKinematics *K, LPCSTR bone_name, bool visible, bool recursive = true) 
{ 
	u16 bone_id = K->LL_BoneID (bone_name);	
	K->LL_SetBoneVisible(bone_id, visible, recursive); 
}

void set_bones_visible(CKinematics *K, const SLargeInteger &vset)
{	
	K->LL_SetBonesVisible(vset.QuadPart);	
}


void CKinematicsAnimatedScript::script_register		(lua_State *L)
{
	module(L)
	[   // базовые методы и свойства дл€ скриптовой анимации
		class_<CBoneInstance>("CBoneInstance")
		.def_readonly("mTransform"				,				&CBoneInstance::mTransform)
		.def_readonly("mRenderTransform"		,				&CBoneInstance::mRenderTransform)
		.def("get_param"						,				&CBoneInstance::get_param)
		.def("set_param"						,				&CBoneInstance::set_param)		
		,
		class_<CKinematics>("CKinematics")
		.def("LL_BoneCount"						,				&CKinematics::LL_BoneCount)
		.def("LL_BoneID"						,				&get_bone_id)
		.def("LL_BoneName"						,				&CKinematics::LL_BoneName_dbg)
		.def("LL_GetBoneInstance"				,				&CKinematics::LL_GetBoneInstance)		
		.def("LL_GetBoneRoot"					,				&CKinematics::LL_GetBoneRoot)	
		.def("LL_GetBoneVisible"				,				&get_bone_visible_by_id)
		.def("LL_GetBoneVisible"				,				&get_bone_visible_by_name)
		.def("LL_SetBoneVisible"				,				&set_bone_visible_by_id)
		.def("LL_SetBoneVisible"				,				&set_bone_visible_by_name)
		.def("LL_GetBonesVisible"				,				&get_bones_visible)
		.def("LL_SetBonesVisible"				,				&set_bones_visible)
		.def("bone_position"					,				&CalcBonePosition)
		,
		class_<CKinematicsAnimated, CKinematics>("CKinematicsAnimated")
		.def("PlayCycle"						,				&CKinematicsAnimated_PlayCycle)		
	];
}

void CBlendScript::script_register		(lua_State *L)
{
	module(L)
		[
			class_<CBlend>("CBlend")
			//			.def(constructor<>())
		];
}

// alpet ======================== SCRIPT_TEXTURE_CONTROL EXPORTS 2 =========== 

void child_visual_configure(IRender_Visual	*v, u32 n_child, LPCSTR new_shader, LPCSTR new_texture)
{

	if (!v)	return; // not have visual

	CKinematics *k = dynamic_cast<CKinematics*> (v);
	if (!k)	return;

	if (n_child >= k->children.size()) return;

	IRender_Visual*  child_v = k->children.at(n_child);

	visual_configure(child_v, new_shader, new_texture);
}


void script_object_set_texture (CScriptGameObject *script_obj, u32 nc, LPCSTR replace)
{
	IRender_Visual *v = script_obj->object().Visual();
	child_visual_configure (v, nc, NULL, replace);
}

void script_object_set_shader(CScriptGameObject *script_obj, u32 nc, LPCSTR replace)
{
	IRender_Visual *v = script_obj->object().Visual();
	child_visual_configure(v, nc, replace, NULL);
}

CTexture* script_object_get_texture (CScriptGameObject *script_obj, u32 n_child, u32 n_texture) 
{
	IRender_Visual *v = script_obj->object().Visual();
	v = visual_get_child (v, n_child);
	return visual_get_texture (v, n_texture);
}


CTexture* script_texture_create(LPCSTR name)
{
	return Device.Resources->_CreateTexture(name);
}

CTexture* script_texture_find(LPCSTR name) 
{
	return Device.Resources->_FindTexture(name);
}

void script_texture_delete(CTexture *t)
{
	xr_delete(t);	
}

LPCSTR script_texture_getname(CTexture *t) 
{
	return t->cName.c_str();
}

void script_texture_setname (CTexture *t, LPCSTR name)
{
	t->SetName(name);
}


void script_texture_load(CTexture *t)
{
	t->Load();
}

void script_texture_unload(CTexture *t)
{
	t->Unload();
}

void script_texture_reload(CTexture *t)
{
	t->Unload();
	t->Load();
}

void script_texture_cast(lua_State *L)
{
	using namespace luabind::detail;
	CTexture *result = (CTexture*) lua_topointer(L, 1);
	if (!IsBadReadPtr(result, sizeof(CTexture)))
		convert_to_lua<CTexture*> (L, result); // for default 
	else
		lua_pushnil(L);
}



void CTextureScript::script_register(lua_State *L)
{
	// added by alpet 10.07.14
	module(L)
		[
			class_<CTexture>("CTexture")			
			.def("delete",				&script_texture_delete)
			.def("find",				&script_texture_find)
			.def("load",				&script_texture_load)
			.def("reload",				&script_texture_reload)
			.def("unload",				&script_texture_unload)
			.def("get_name",			&script_texture_getname)
			.def("set_name",			&script_texture_setname)
			.def("get_surface",			&CTexture::surface_get)
			.def("cast_texture",		&script_texture_cast, raw(_1))	
			.def_readonly("ref_count",  &CTexture::dwReference)
			
		];
}


__declspec(dllexport) CResourceManager *get_resource_manager() // also can be used in luaicp
{
	return Device.Resources;
}

void cast_ptr_CTexture(lua_State *L)
{
	CTexture *t = (CTexture*) (lua_topointer(L, 1));
	if (t)
		luabind::detail::convert_to_lua<CTexture*>(L, t);
	else
		lua_pushnil(L);
}

#ifdef LUAICP_COMPAT
DLL_API void *get_textures_map(CResourceManager *mgr)
{
	if (mgr) return &mgr->textures();
	return NULL;
}
#endif

void get_loaded_textures(CResourceManager *mgr, lua_State *L)
{
	CResourceManager::map_Texture &map = mgr->textures();

	CResourceManager::map_Texture::iterator I = map.begin();
	CResourceManager::map_Texture::iterator E = map.end();
	
	lua_createtable(L, 0, 0);
	int t = lua_gettop(L);
	LPCSTR pattern = NULL;
	if (t > 1)
		pattern = lua_tostring(L, 2);

	for ( ; I != E; ++I)
	if (I->first)
	{		
		LPCSTR name = I->first;
		if ( pattern && !strstr(name, pattern) ) continue;				
		lua_pushlightuserdata (L, (void*)I->second); // alpet: дешевле вставл€ть указатель, и в скриптах кастовать его через cast_ptr_CTexture только дл€ нужных объектов			
		lua_setfield(L, t, name);
	}
}


void CResourceManagerScript::script_register(lua_State *L)
{
	// added by alpet 10.07.14
	module(L) [ 
		// added by alpet
		class_<CResourceManager>("CResourceManager")
		.def("get_loaded_textures",  &get_loaded_textures, raw(_2))
		,
		def("cast_ptr_CTexture",	&cast_ptr_CTexture, raw(_1)),
		def("get_resource_manager", &get_resource_manager),
		def("texture_create",		&script_texture_create),
		def("texture_delete",		&script_texture_delete),
		def("texture_find",			&script_texture_find),
		def("texture_from_object",  &script_object_get_texture),
		def("texture_from_visual",  &visual_get_texture),
		def("texture_to_object",	&script_object_set_texture),
		def("texture_to_visual",	&visual_set_texture),
		def("texture_load",			&script_texture_load),
		def("texture_unload",		&script_texture_unload),
		def("texture_get_name",		&script_texture_getname),
		def("texture_set_name",		&script_texture_setname)
	];
}

// alpet ======================== SCRIPT_TEXTURE_CONTROL END =========== 

// alpet ======================== CAMERA SCRIPT OBJECT =================

ENGINE_API extern float psHUD_FOV;

CCameraBase* actor_camera(u16 index)
{
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
	if (!pA) return NULL;
	return pA->cam_ByIndex(index);
}


float global_fov(float new_fov = 0)
{
	if (new_fov > 0.1)
		g_fov = new_fov;
	return g_fov;
}

float hud_fov(float new_fov = 0)
{
	if (new_fov > 0.1)
		psHUD_FOV = new_fov;
	return psHUD_FOV;

}

void CCameraBaseScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<CCameraBase>("CCameraBase")
			.def_readwrite("aspect",		&CCameraBase::f_aspect)
			.def_readonly ("direction",		&CCameraBase::vDirection)
			.def_readwrite("fov",			&CCameraBase::f_fov)
			.def_readwrite("position",		&CCameraBase::vPosition)

			.def_readwrite("lim_yaw",		&CCameraBase::lim_yaw)
			.def_readwrite("lim_pitch",		&CCameraBase::lim_pitch)
			.def_readwrite("lim_roll",		&CCameraBase::lim_roll)

			.def_readwrite("yaw",			&CCameraBase::yaw)
			.def_readwrite("pitch",			&CCameraBase::pitch)
			.def_readwrite("roll",			&CCameraBase::roll),


			def("actor_camera",				&actor_camera),
			def("get_global_fov",			&global_fov),
			def("set_global_fov",			&global_fov),
			def("get_hud_fov",				&hud_fov),
			def("set_hud_fov",				&hud_fov)




		];
}
// alpet ======================== CAMERA SCRIPT OBJECT =================



/*
void CKinematicsScript::script_register		(lua_State *L)
{
	module(L)
		[
			class_<CKinematics, FHierrarhyVisual>("CKinematics")
			//			.def(constructor<>())
		];
}

void FHierrarhyVisualScript::script_register		(lua_State *L)
{
	module(L)
		[
			class_<FHierrarhyVisual, IRender_Visual>("FHierrarhyVisual")
			//			.def(constructor<>())
		];
}
*/
