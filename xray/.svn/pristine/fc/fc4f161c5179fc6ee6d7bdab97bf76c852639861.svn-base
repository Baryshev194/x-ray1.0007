#include "pch_script.h"
#include "HangingLamp.h"
#include "../LightAnimLibrary.h"
#include "../xr_collide_form.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "PHElement.h"
#include "../skeletonanimated.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangingLamp::CHangingLamp	()
{
	Init();
}

CHangingLamp::~CHangingLamp	()
{
}

void CHangingLamp::Init()
{
	fHealth					= 100.f;
	light_bone				= BI_NONE;
	ambient_bone			= BI_NONE;
	lanim					= 0;
	ambient_power			= 0.f;
	light_render			= 0;
	light_ambient			= 0;
	glow_render				= 0;
}

void CHangingLamp::RespawnInit()
{
	Init();
	if(Visual()){
		CKinematics* K = smart_cast<CKinematics*>(Visual());
		K->LL_SetBonesVisible(u64(-1));
		K->CalculateBones_Invalidate();
		K->CalculateBones	();
	}
}

void CHangingLamp::Center	(Fvector& C) const 
{ 
	if (renderable.visual){
		renderable.xform.transform_tiny(C,renderable.visual->vis.sphere.P);	
	}else{
		C.set	(XFORM().c);
	}
}

float CHangingLamp::Radius	() const 
{ 
	return (renderable.visual)?renderable.visual->vis.sphere.R:EPS;
}

void CHangingLamp::Load		(LPCSTR section)
{
	inherited::Load			(section);
	LPCSTR imm_sect = READ_IF_EXISTS (pSettings, r_string, section, "immunities_sect", NULL);
	if (imm_sect)
		CHitImmunity::LoadImmunities(imm_sect, pSettings);
}

void CHangingLamp::net_Destroy()
{
	light_render.destroy	();
	light_ambient.destroy	();
	glow_render.destroy		();
	RespawnInit				();
	if(Visual())CPHSkeleton::RespawnInit();
	inherited::net_Destroy	();
}

BOOL CHangingLamp::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract			*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectHangingLamp	*lamp	= smart_cast<CSE_ALifeObjectHangingLamp*>(e);
	R_ASSERT				(lamp);
	inherited::net_Spawn	(DC);
	Fcolor					clr;

	// set bone id
//	CInifile* pUserData		= K->LL_UserData(); 
//	R_ASSERT3				(pUserData,"Empty HangingLamp user data!",lamp->get_visual());
	xr_delete(collidable.model);
	if (Visual()){
		CKinematics* K		= smart_cast<CKinematics*>(Visual());
		R_ASSERT			(Visual()&&smart_cast<CKinematics*>(Visual()));
		light_bone			= K->LL_BoneID	(*lamp->light_main_bone);	VERIFY(light_bone!=BI_NONE);
		ambient_bone		= K->LL_BoneID	(*lamp->light_ambient_bone);VERIFY(ambient_bone!=BI_NONE);
		collidable.model	= xr_new<CCF_Skeleton>				(this);
		// alpet: загрузка иммунитетов из спавн-конфига
		CInifile* ini=K->LL_UserData();
		if(ini && ini->section_exist("immunities"))		CHitImmunity::LoadImmunities("immunities",ini);
	}
	fBrightness				= lamp->brightness;
	clr.set					(lamp->color);						clr.a = 1.f;
	clr.mul_rgb				(fBrightness);

	light_render			= ::Render->light_create();
	light_render->set_shadow(!!lamp->flags.is(CSE_ALifeObjectHangingLamp::flCastShadow));
	light_render->set_type	(lamp->flags.is(CSE_ALifeObjectHangingLamp::flTypeSpot)?IRender_Light::SPOT:IRender_Light::POINT);
	light_render->set_range	(lamp->range);
	light_render->set_color	(clr);
	light_render->set_cone	(lamp->spot_cone_angle);
	light_render->set_texture(*lamp->light_texture);
	light_render->set_virtual_size(lamp->m_virtual_size);

	if (lamp->glow_texture.size())	{
		glow_render				= ::Render->glow_create();
		glow_render->set_texture(*lamp->glow_texture);
		glow_render->set_color	(clr);
		glow_render->set_radius	(lamp->glow_radius);
	}

	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPointAmbient)){
		ambient_power			= lamp->m_ambient_power;
		light_ambient			= ::Render->light_create();
		light_ambient->set_type	(IRender_Light::POINT);
		light_ambient->set_shadow(false);
		clr.mul_rgb				(ambient_power);
		light_ambient->set_range(lamp->m_ambient_radius);
		light_ambient->set_color(clr);
		light_ambient->set_texture(*lamp->m_ambient_texture);
		light_ambient->set_virtual_size(lamp->m_virtual_size);
	}

	fHealth					= lamp->m_health;

	lanim					= LALib.FindItem(*lamp->color_animator);

	CPHSkeleton::Spawn(e);
	if (smart_cast<CKinematicsAnimated*>(Visual()))	smart_cast<CKinematicsAnimated*>	(Visual())->PlayCycle("idle");
	if (smart_cast<CKinematics*>(Visual())){
		smart_cast<CKinematics*>			(Visual())->CalculateBones_Invalidate	();
		smart_cast<CKinematics*>			(Visual())->CalculateBones();
		//.intepolate_pos
	}
	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic)&&!Visual())
		Msg("! WARNING: lamp, obj name [%s],flag physics set, but has no visual",*cName());
//.	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic)&&Visual()&&!guid_physic_bone)	fHealth=0.f;
	if (Alive())			TurnOn	();
	else{
		processing_activate		();	// temporal enable
		TurnOff					();	// -> and here is disable :)
	}
	
	setVisible					((BOOL)!!Visual());
	setEnabled					((BOOL)!!collidable.model);

	return						(TRUE);
}


void	CHangingLamp::SpawnInitPhysics	(CSE_Abstract	*D)	
{
	CSE_ALifeObjectHangingLamp	*lamp	= smart_cast<CSE_ALifeObjectHangingLamp*>(D);	
	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic))		CreateBody(lamp);
	if (smart_cast<CKinematics*>(Visual())){
		smart_cast<CKinematics*>			(Visual())->CalculateBones_Invalidate	();
		smart_cast<CKinematics*>			(Visual())->CalculateBones();
		//.intepolate_pos
	}
}

void	CHangingLamp::CopySpawnInit		()
{
	CPHSkeleton::CopySpawnInit();
	CKinematics* K=smart_cast<CKinematics*>(Visual());
	if(!K->LL_GetBoneVisible(light_bone))
		TurnOff();
}
void	CHangingLamp::net_Save			(NET_Packet& P)	
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState(P);
}

BOOL	CHangingLamp::net_SaveRelevant	()
{
	return (inherited::net_SaveRelevant() || BOOL(PPhysicsShell()!=NULL));
}

void CHangingLamp::shedule_Update	(u32 dt)
{
	CPHSkeleton::Update(dt);


	inherited::shedule_Update		(dt);
}

void CHangingLamp::UpdateCL	()
{
	inherited::UpdateCL		();

	if(m_pPhysicsShell)
		m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());

	if (Alive() && light_render->get_active()){
		if(Visual())	PKinematics(Visual())->CalculateBones();

		// update T&R from light (main) bone
		Fmatrix xf;
		if (light_bone!=BI_NONE){
			Fmatrix& M = smart_cast<CKinematics*>(Visual())->LL_GetTransform(light_bone);
			xf.mul		(XFORM(),M);
			VERIFY(!fis_zero(DET(xf)));
		}else{
			xf.set		(XFORM());
		}
		light_render->set_rotation	(xf.k,xf.i);
		light_render->set_position	(xf.c);
		if (glow_render)glow_render->set_position	(xf.c);

		// update T&R from ambient bone
		if (light_ambient){	
			if (ambient_bone!=light_bone){
				if (ambient_bone!=BI_NONE){
					Fmatrix& M = smart_cast<CKinematics*>(Visual())->LL_GetTransform(ambient_bone);
					xf.mul		(XFORM(),M);
					VERIFY(!fis_zero(DET(xf)));
				}else{
					xf.set		(XFORM());
				}
			}
			light_ambient->set_rotation	(xf.k,xf.i);
			light_ambient->set_position	(xf.c);
		}
		
		if (lanim){
			int frame;
			u32 clr					= lanim->CalculateBGR(Device.fTimeGlobal,frame); // возвращает в формате BGR
			Fcolor					fclr;
			fclr.set				((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb			(fBrightness/255.f);
			light_render->set_color	(fclr);
			if (glow_render)		glow_render->set_color	(fclr);
			if (light_ambient) {
				fclr.mul_rgb		(ambient_power);
				light_ambient->set_color(fclr);
			}
		}
	}
}

void CHangingLamp::TurnOn	()
{
	light_render->set_active						(true);
	if (glow_render)	glow_render->set_active		(true);
	if (light_ambient)	light_ambient->set_active	(true);
	if (Visual()){
		CKinematics* K				= smart_cast<CKinematics*>(Visual());
		K->LL_SetBoneVisible		(light_bone, TRUE, TRUE);
		K->CalculateBones_Invalidate();
		K->CalculateBones			();
	}
	processing_activate		();
}

void CHangingLamp::TurnOff	()
{
	light_render->set_active						(false);
	if (glow_render)	glow_render->set_active		(false);
	if (light_ambient)	light_ambient->set_active	(false);
	if (Visual())		smart_cast<CKinematics*>(Visual())->LL_SetBoneVisible(light_bone, FALSE, TRUE);
	if(!PPhysicsShell())//if we have physiccs_shell it will call processing deactivate when disable
		processing_deactivate	();
}



#define  CLASS_IMPL		CHangingLamp

CSE_ALifeObjectHangingLamp *get_se_lamp(CHangingLamp *lobj)
{
	CSE_ALifeDynamicObject *e = lobj->lua_game_object()->alife_object();
	if (!e) return NULL;
	return smart_cast<CSE_ALifeObjectHangingLamp*>(e);
}

void		CHangingLamp::SetDirection(const Fvector &v, float bank)
{
	lua_game_object()->SetDirection(v, bank);		
}
void		CHangingLamp::SetPosition(const Fvector &v) 
{
	lua_game_object()->SetPosition(v);	
}

void		CHangingLamp::Synchronize() // alpet: сохранение данных в серверный объект
{
	CSE_ALifeObjectHangingLamp *lamp = get_se_lamp(this);
	if (!lamp) return;
	lamp->position() = XFORM().c;	
	XFORM().getXYZ(lamp->angle());	 
	lamp->brightness = fBrightness;
	lamp->spot_cone_angle = light_render->get_cone();
	lamp->range = light_render->get_range();
	Fcolor clr = light_render->get_color();
	if (fBrightness > 0)
		clr.mul_rgb( 255.f / fBrightness);
	lamp->color = clr.get();

	if (light_ambient)
	{
		lamp->m_ambient_radius = light_ambient->get_range();		
	}	

	if (lanim)
		lamp->color_animator = lanim->cName;
}

#define  target_0		light_render
#define  target_1		light_ambient
#define	 target_2		glow_render		
#include "light_ext.inc"
#undef   target_0
#undef	 target_1
#undef	 target_2 
#undef CLASS_IMPL

//void CHangingLamp::Hit(float P,Fvector &dir, CObject* who,s16 element,
//					   Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
void	CHangingLamp::Hit					(SHit* pHDS)
{
	SHit	HDS = *pHDS;
	callback(GameObject::eHit)(
		lua_game_object(), 
		HDS.power,
		HDS.dir,
		smart_cast<const CGameObject*>(HDS.who)->lua_game_object(),
		HDS.bone()
		);

#ifdef HLAMP_AFFECT_IMMUNITIES
	HDS.power = CHitImmunity::AffectHit(HDS.power,HDS.hit_type);	
	inherited::Hit(pHDS);
#endif
	BOOL	bWasAlive		= Alive		() || light_render->get_active();

	if(m_pPhysicsShell) 
	   m_pPhysicsShell->applyHit(pHDS->p_in_bone_space,pHDS->dir,pHDS->impulse,pHDS->boneID,pHDS->hit_type);
	
	if (!bWasAlive) return;

	if (pHDS->boneID==light_bone)
	    SetHealth ( 0.f );
	else
	{
		float damage = pHDS->damage() * 100.f;
		Msg("DEBUG: %s health = %.3f, damage = %.3f", Name_script(), GetHealth(), damage);
		SetHealth(GetHealth() - damage);
	}
	if (bWasAlive && (!Alive()))		TurnOff	();
}

static BONE_P_MAP bone_map=BONE_P_MAP();
void CHangingLamp::CreateBody(CSE_ALifeObjectHangingLamp	*lamp)
{
	if (!Visual())			return;
	if (m_pPhysicsShell)	return;
	
	CKinematics* pKinematics= smart_cast<CKinematics*>	(Visual());

	m_pPhysicsShell			= P_create_Shell();

	bone_map					.clear();
	LPCSTR	fixed_bones=*lamp->fixed_bones;
	if(fixed_bones){
		int count =					_GetItemCount(fixed_bones);
		for (int i=0 ;i<count; ++i){
			string64					fixed_bone							;
			_GetItem					(fixed_bones,i,fixed_bone)			;
			u16 fixed_bone_id=pKinematics->LL_BoneID(fixed_bone)			;
			R_ASSERT2(BI_NONE!=fixed_bone_id,"wrong fixed bone")			;
			bone_map.insert(mk_pair(fixed_bone_id,physicsBone()))			;
		}
	}else{
		bone_map.insert(mk_pair(pKinematics->LL_GetBoneRoot(),physicsBone()))			;
	}



	m_pPhysicsShell->build_FromKinematics(pKinematics,&bone_map);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true);//,
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	m_pPhysicsShell->SetAirResistance();//0.0014f,1.5f

/////////////////////////////////////////////////////////////////////////////
	BONE_P_PAIR_IT i=bone_map.begin(),e=bone_map.end();
	for(;i!=e;i++){
		CPhysicsElement* fixed_element=i->second.element;
		///R_ASSERT2(fixed_element,"fixed bone has no physics");
		if(fixed_element)fixed_element->Fix();
	}

	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(Visual())->LL_UserData());
	m_pPhysicsShell->set_DisableParams(disable_params);
	ApplySpawnIniToPhysicShell(&lamp->spawn_ini(),m_pPhysicsShell,fixed_bones[0]!='\0');
}

void CHangingLamp::net_Export(NET_Packet& P)
{
	VERIFY					(Local());
}

void CHangingLamp::net_Import(NET_Packet& P)
{
	VERIFY					(Remote());
}

BOOL CHangingLamp::UsedAI_Locations()
{
	return					(FALSE);
}

#pragma optimize("s",on)
void CHangingLamp::script_register(lua_State *L)
{
	luabind::module(L)
	[
		luabind::class_<CHangingLamp,CGameObject,CHitImmunity>("hanging_lamp")
			.def(luabind::constructor<>())
			.def("turn_on",		&CHangingLamp::TurnOn)
			.def("turn_off",	&CHangingLamp::TurnOff)
			// alpet: управление параметрами света			
			.def("get_light",		&CHangingLamp::GetLight) 
			.def("set_animation",	&CHangingLamp::SetAnimation)
			.def("set_brightness",	&CHangingLamp::SetBrightness)
			.def("set_direction",   &CHangingLamp::SetDirection)   
			.def("set_position",    &CHangingLamp::SetPosition)  
			.def("set_angle",	&CHangingLamp::SetAngle)
			.def("set_color",	&CHangingLamp::SetColor)
			.def("set_rgb",		&CHangingLamp::SetRGB)
			.def("set_range",	&CHangingLamp::SetRange)
			.def("set_texture", &CHangingLamp::SetTexture)
			.def("set_virtual_size", &CHangingLamp::SetVirtualSize)
			.def("synchronize", &CHangingLamp::Synchronize)		
			,
		luabind::class_<IRender_Light>("IRender_Light")
			.def("get_active", &IRender_Light::get_active)
			.def("get_angle",  &IRender_Light::get_cone)			
			.def("get_color",  &IRender_Light::get_color)
			.def("get_range",  &IRender_Light::get_range)
			.def("get_virtual_size", &IRender_Light::get_virtual_size)

			.def("set_active", &IRender_Light::set_active)
			.def("set_angle",  &IRender_Light::set_cone)
			.def("set_color", (void (IRender_Light::*)(const Fcolor&)) (&IRender_Light::set_color) )
			.def("set_range",  &IRender_Light::set_range)
			.def("set_texture", &IRender_Light::set_texture)
			.def("set_virtual_size", &IRender_Light::set_virtual_size)

	];
}
