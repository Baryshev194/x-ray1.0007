#include "pch_script.h"
#include "WeaponLR300.h"

CWeaponLR300::CWeaponLR300		() : CWeaponMagazined("LR300",SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
	SetSlot(RIFLE_SLOT);
}

CWeaponLR300::~CWeaponLR300		()
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponLR300::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponLR300, CWeaponMagazined>("CWeaponLR300")
			.def(constructor<>())
	];
}
