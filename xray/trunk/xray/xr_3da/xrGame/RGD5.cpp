#include "pch_script.h"
#include "rgd5.h"

CRGD5::CRGD5(void)
{
	m_flags.set				(Fbelt, TRUE);
	m_weight				= .1f;
	SetSlot (GRENADE_SLOT);
}

CRGD5::~CRGD5(void)
{
}

using namespace luabind;

#pragma optimize("s",on)
void CRGD5::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CRGD5,CGrenade>("CRGD5")
			.def(constructor<>())
	];
}
