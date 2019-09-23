#include "pch_script.h"
#include "weaponvintorez.h"

CWeaponVintorez::CWeaponVintorez(void) : CWeaponMagazined("VINTOREZ",SOUND_TYPE_WEAPON_SNIPERRIFLE)
{
	m_weight = 1.5f;
	SetSlot (RIFLE_SLOT);
}

CWeaponVintorez::~CWeaponVintorez(void)
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponVintorez::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponVintorez, CWeaponMagazined>("CWeaponVintorez")
			.def(constructor<>())
	];
}
