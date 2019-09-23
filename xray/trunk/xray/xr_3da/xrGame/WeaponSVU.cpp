#include "pch_script.h"
#include "weaponsvu.h"

CWeaponSVU::CWeaponSVU(void) : CWeaponCustomPistol("SVU")
{
	SetSlot(RIFLE_SLOT); 
}

CWeaponSVU::~CWeaponSVU(void)
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponSVU::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponSVU, CWeaponMagazined>("CWeaponSVU")
			.def(constructor<>())
	];
}
