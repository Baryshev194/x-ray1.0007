////////////////////////////////////////////////////////////////////////////
//	Module 		: script_render_device_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 29.07.2014
//	Author		: Dmitriy Iassenev
//	Description : Script render device script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_render_device.h"
#include "Level.h"

using namespace luabind;

bool is_device_paused(CRenderDevice* d)
{
	return !!Device.Paused();
}

void set_device_paused(CRenderDevice* d, bool b)
{
	Device.Pause(b, TRUE, FALSE,"set_device_paused_script");
}

/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
// получить текущий fov
float get_fov()
{
	return g_fov;
}
// установить текущий fov
void set_fov(CRenderDevice* d, float fov)
{
	g_fov = fov;
}
/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/

extern ENGINE_API BOOL g_appLoaded;
bool is_app_ready()
{
	return !!g_appLoaded;
}

u32 time_global(const CRenderDevice *self)
{
	THROW		(self);
	return		(self->dwTimeGlobal);
}

#pragma optimize("s",on)
void CScriptRenderDevice::script_register(lua_State *L)
{
	module(L)
	[
		class_<CRenderDevice>("render_device")
			.def_readonly("width",					&CRenderDevice::dwWidth)
			.def_readonly("height",					&CRenderDevice::dwHeight)
			.def_readonly("time_delta",				&CRenderDevice::dwTimeDelta)
			.def_readonly("f_time_delta",			&CRenderDevice::fTimeDelta)
			.def_readonly("cam_pos",				&CRenderDevice::vCameraPosition)
			.def_readonly("cam_dir",				&CRenderDevice::vCameraDirection)
			.def_readonly("cam_top",				&CRenderDevice::vCameraTop)
			.def_readonly("cam_right",				&CRenderDevice::vCameraRight)
//			.def_readonly("view",					&CRenderDevice::mView)
//			.def_readonly("projection",				&CRenderDevice::mProject)
//			.def_readonly("full_transform",			&CRenderDevice::mFullTransform)
			.def_readwrite("fov",					&CRenderDevice::fFOV)
			.def_readonly("aspect_ratio",			&CRenderDevice::fASPECT)
			.def("time_global",						&time_global)
			.def_readonly("precache_frame",			&CRenderDevice::dwPrecacheFrame)
			.def_readonly("frame",					&CRenderDevice::dwFrame)
			.def ("frame_elapsed",					&CRenderDevice::frame_elapsed)
			/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
			.def("get_fov",							&get_fov)
			.def("set_fov",							&set_fov)
			/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/
			.def("is_paused",						&is_device_paused)
			.def("pause",							&set_device_paused),
			
			def("app_ready",						&is_app_ready)
	];
}
