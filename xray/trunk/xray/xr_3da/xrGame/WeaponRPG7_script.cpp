#include "pch_script.h"
#include "WeaponRPG7.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponRPG7::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponRPG7, CWeaponMagazined>("CWeaponRPG7")
			.def(constructor<>())
	];
}
