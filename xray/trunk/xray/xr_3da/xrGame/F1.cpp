#include "pch_script.h"
#include "f1.h"


CF1::CF1(void) {
	m_flags.set				(Fbelt, TRUE);
	m_weight				= .1f;
	SetSlot (GRENADE_SLOT);
}

CF1::~CF1(void) {
}

using namespace luabind;

#pragma optimize("s",on)
void CF1::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CF1,CGrenade>("CF1")
			.def(constructor<>())
	];
}
