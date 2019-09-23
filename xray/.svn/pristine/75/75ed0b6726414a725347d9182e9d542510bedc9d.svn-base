#include "pch_script.h"
#include "Explosive.h"
#include "Grenade.h"
#include "Missile.h"

using namespace luabind;

#pragma optimize("s",on)
void CExplosive::script_register(lua_State *L)
{
	module(L)
	[
			class_<CExplosive>("explosive")
			.def_readwrite("blast"						,			&CExplosive::m_fBlastHit)
			.def_readwrite("blast_impulse"				,			&CExplosive::m_fBlastHitImpulse)
			.def_readwrite("blast_r"					,			&CExplosive::m_fBlastRadius)
			.def_readwrite("current_parent"				,			&CExplosive::m_iCurrentParentID)
			.def_readonly ("explode_pos"				,			&CExplosive::m_vExplodePos)
			.def_readonly ("explode_duration"			,			&CExplosive::m_fExplodeDuration)
			.def_readwrite("explode_duration_max"		,			&CExplosive::m_fExplodeDurationMax)
			.def_readonly ("explosion_flags"			,			&CExplosive::m_explosion_flags)

			.def_readwrite("fragment_speed"				,			&CExplosive::m_fFragmentSpeed)
			.def_readwrite("frags"						,			&CExplosive::m_iFragsNum)
			.def_readwrite("frags_hit"					,			&CExplosive::m_fFragHit)
			.def_readwrite("frags_hit_impulse"			,			&CExplosive::m_fFragHitImpulse)
			.def_readwrite("frags_r"					,			&CExplosive::m_fFragsRadius)
			
			.def_readonly ("light_color"				,			&CExplosive::m_LightColor)
			.def_readwrite("light_range"				,			&CExplosive::m_fLightRange)
			.def_readwrite("light_time"					,			&CExplosive::m_fLightTime)

			.def("explode",					(&CExplosive::Explode))
			,
			class_<CGrenade, bases<CExplosive, CMissile>>("CGrenade")
	];
}

