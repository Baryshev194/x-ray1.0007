#include "stdafx.h"
#pragma hdrstop

#include "WeaponMounted.h"
#include "xrServer_Objects_ALife.h"
#include "camerafirsteye.h"
#include "actor.h"
#include "weaponammo.h"


#include "actoreffector.h"
#include "effectorshot.h"
#include "ai_sounds.h"
#include "level.h"
#include "xr_level_controller.h"
#include "../skeletoncustom.h"
#include "game_object_space.h"

//----------------------------------------------------------------------------------------

void CWeaponMounted::BoneCallbackX(CBoneInstance *B)
{
	CWeaponMounted	*P = static_cast<CWeaponMounted*>(B->Callback_Param);

	if (P->Owner()){
		Fmatrix rX;		rX.rotateX		(P->camera->pitch+P->m_dAngle.y);
		B->mTransform.mulB_43(rX);
		// alpet: сохранение ориентации после детача
		static int method = 0;
		Fmatrix &m = P->PPhysicsShell()->get_Element(1)->mXFORM;
		Fvector pos = m.c;
		float h, p, b;
		m.getHPB (h, p, b);	// желательно не затереть горизонтальный угол 	
		m.setHPB (h, -P->camera->pitch, b).c = pos;
	}
}

void CWeaponMounted::BoneCallbackY(CBoneInstance *B)
{
	CWeaponMounted	*P = static_cast<CWeaponMounted*>(B->Callback_Param);

	if (P->Owner()){
		Fmatrix rY;		rY.rotateY		(P->camera->yaw+P->m_dAngle.x);
		B->mTransform.mulB_43(rY);
		// alpet: сохранение ориентации после детача		
		Fmatrix &m = P->PPhysicsShell()->get_Element(1)->mXFORM;		
		Fvector pos = m.c;		
		float h, p, b;
		m.getHPB(h, p, b);	// желательно не затереть вертикальный угол 	
		m.setHPB(-P->camera->yaw, p, b).c = pos;		
	}
}
//----------------------------------------------------------------------------------------

CWeaponMounted::CWeaponMounted()
{
	camera		= xr_new<CCameraFirstEye>	(this, CCameraBase::flRelativeLink|CCameraBase::flPositionRigid|CCameraBase::flDirectionRigid); 
	camera->Load	("mounted_weapon_cam");
}

CWeaponMounted::~CWeaponMounted()
{
	xr_delete(camera);
}

void	CWeaponMounted::Load(LPCSTR section)
{
	inherited::Load(section);
	CShootingObject::Load	(section);

	HUD_SOUND::LoadSound(section,"snd_shoot", sndShot, SOUND_TYPE_WEAPON_SHOOTING);

	//тип используемых патронов
	m_sAmmoType = pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load(*m_sAmmoType, 0);

	//подбрасывание камеры во время отдачи
	camMaxAngle			= pSettings->r_float		(section,"cam_max_angle"	); 
	camMaxAngle			= deg2rad					(camMaxAngle);
	camRelaxSpeed		= pSettings->r_float		(section,"cam_relax_speed"	); 
	camRelaxSpeed		= deg2rad					(camRelaxSpeed);

}

BOOL	CWeaponMounted::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMountedWeapon	*mw	= smart_cast<CSE_ALifeMountedWeapon*>(e);
	R_ASSERT				(mw);

	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	R_ASSERT				(Visual() && smart_cast<CKinematics*>(Visual()));

	CKinematics* K			= smart_cast<CKinematics*>(Visual());
	CInifile* pUserData		= K->LL_UserData(); 

	R_ASSERT3				(pUserData,"Empty MountedWeapon user data!",mw->get_visual());

	fire_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","fire_bone"));
	actor_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","actor_bone"));
	rotate_x_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_x_bone"));
	rotate_y_bone			= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","rotate_y_bone"));
	camera_bone				= K->LL_BoneID	(pUserData->r_string("mounted_weapon_definition","camera_bone"));

	CBoneData& bdX			= K->LL_GetData(rotate_x_bone); VERIFY(bdX.IK_data.type==jtJoint);
	camera->lim_pitch.set	(bdX.IK_data.limits[0].limit.x,bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY			= K->LL_GetData(rotate_y_bone); VERIFY(bdY.IK_data.type==jtJoint);
	camera->lim_yaw.set		(bdY.IK_data.limits[1].limit.x,bdY.IK_data.limits[1].limit.y);

	U16Vec fixed_bones;
	fixed_bones.push_back	(K->LL_GetBoneRoot());
	PPhysicsShell()			= P_build_Shell(this,false,fixed_bones);
	K						->CalculateBones_Invalidate();
	K						->CalculateBones();
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());

	CShootingObject::Light_Create();

	setVisible	(TRUE);
	setEnabled	(TRUE);



	return TRUE;
}

void	CWeaponMounted::net_Destroy()
{
	CShootingObject::Light_Destroy();

	inherited::net_Destroy();
	xr_delete(m_pPhysicsShell);
}

void	CWeaponMounted::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
}

void	CWeaponMounted::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
}

void	CWeaponMounted::UpdateCL()
{
	inherited::UpdateCL	();
	if (Owner()){
		CKinematics* K		= smart_cast<CKinematics*>(Visual());
		K->CalculateBones	();
		// update fire pos & fire_dir
		fire_bone_xform		= K->LL_GetTransform(fire_bone);
		fire_bone_xform.mulA_43			(XFORM());
		fire_pos.set					(0,0,0); 
		fire_bone_xform.transform_tiny	(fire_pos);
		fire_dir.set					(0,0,1); 
		fire_bone_xform.transform_dir	(fire_dir);

		UpdateFire			();

		if(OwnerActor() && OwnerActor()->IsMyCamera()) 
		{
			cam_Update(Device.fTimeDelta, g_fov);
			OwnerActor()->Cameras().Update(Camera());
			OwnerActor()->Cameras().ApplyDevice(VIEWPORT_NEAR);
		}
	}
}

void	CWeaponMounted::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

void	CWeaponMounted::renderable_Render()
{
	//нарисовать подсветку
	RenderLight();
	inherited::renderable_Render	();
}

void	CWeaponMounted::OnMouseMove			(int dx, int dy)
{
	if (Remote())	return;

	CCameraBase* C	= camera;
	float scale		= (C->f_fov/g_fov)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d		= float(dx)*scale;
		C->Move		((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d		= ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*3.f/4.f;
		C->Move		((d>0)?kUP:kDOWN, _abs(d));
	}
}
void	CWeaponMounted::OnKeyboardPress		(int dik)
{
	if (Remote())							return;

	switch (dik)	
	{
	case kWPN_FIRE:					
		FireStart();
		break;
	};

}
void	CWeaponMounted::OnKeyboardRelease	(int dik)
{
	if (Remote())							return;
	switch (dik)	
	{
	case kWPN_FIRE:
		FireEnd();
		break;
	};
}
void	CWeaponMounted::OnKeyboardHold		(int dik)
{
	if (Remote())							return;

//	switch(dik)
//	{
//	}
}

void	CWeaponMounted::cam_Update			(float dt, float fov)
{
	Fvector							P,Da;
	Da.set							(0,0,0);
	Camera()->f_fov = fov;

	CKinematics* K					= smart_cast<CKinematics*>(Visual());
	K->CalculateBones_Invalidate	();
	K->CalculateBones				();
	const Fmatrix& C				= K->LL_GetTransform(camera_bone);
	XFORM().transform_tiny			(P,C.c);

	CActor *A = OwnerActor();
	if(A){
		// rotate head
		CCameraBase *cam = Camera();
		// weapon corrections
		static float y_corr = 1;
		static float p_corr = 1;
		static float b_corr = 1;
		// camera apply coef
		static float y_coef = 1;
		static float p_coef = 1;
		static float b_coef = 1;
		// actor apply coefs
		static float ay_coef = 1;
		static float ap_coef = 1;
		static float ab_coef = 1;
		static int   method = 1;
		float h, p, b;
		bool dump = IsDebuggerPresent() && (0 != (GetAsyncKeyState(VK_CONTROL) & 0xC000));

		Fmatrix rot = XFORM(); // weapon rotation+position read
		Fmatrix full;
		rot.c.set(0, 0, 0);
		rot.getHPB(h, p, b);
		if (dump)
			Msg("original device orientation =~C0F %.3f %.3f %.3f~C07, cam orientation =~C0E %.3f %.3f %.3f~C07 ", h, p, b, cam->yaw, cam->pitch, cam->roll);

		// некоммутативность умножения матриц здесь
		
		full.setHPB(cam->yaw * y_coef, cam->pitch * p_coef, cam->roll  * b_coef);	   // set camera rotation 

		if (method & 2)
			full.mulB_43(rot);
		else
		{   // метод с похожим результатом: 
			rot.invert(); // равноценно rot.setHPB(-h, -p, -b);							   // разворот вокруг осей Y, X и Z			 						
			full.mulA_43(rot);															   // apply weapon rotation to camera rotation matrix	
		}
		full.getHPB(h, p, b);

		if (dump)
			Msg("target light orientation    =~C0B %.3f %.3f %.3f~C07", h, p, b);

		A->Orientation().yaw			=  h;
		A->Orientation().pitch		= -p; // alpet: попытка исправить вертикальную инверсию, заметную при игре от 3-го лица
		A->Orientation().roll		=  b;
		CCameraBase *fe = A->cam_FirstEye();
		if (fe)
		{   // alpet: поворот камеры актора, направляющий заодно и свет от фонаря
			fe->yaw   = h * ay_coef;
			fe->pitch = p * ap_coef;
			fe->roll  = b * ab_coef;
			Fvector pv = P;
			// pv.y -= A->Radius() *2.f / 3.f; // TODO: здесь нужно ортогональный базис "вниз" от точки камеры, а просто вычитание Y
			A->XFORM().c = pv;
		}
		
	}
	Camera()->Update							(P,Da);
	Level().Cameras().Update					(Camera());
}

void CWeaponMounted::UpdateXFORM(const Fmatrix &upd)
{
	m_pPhysicsShell->EnabledCallbacks(TRUE); // для пересчета позиций всех костей
	inherited::UpdateXFORM(upd);
}

bool	CWeaponMounted::Use					(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)
{
	return !Owner();
}

// extern float g_fov;

bool	CWeaponMounted::attach_Actor		(CGameObject* actor)
{	
	m_dAngle.set(0.0f,0.0f);
	CHolderCustom::attach_Actor(actor);
	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	// убрать оружие из рук	
	// disable shell callback
	m_pPhysicsShell->EnabledCallbacks(FALSE);
	// enable actor rotate callback
	CBoneInstance& biX		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_x_bone);	
	biX.set_callback		(bctCustom,BoneCallbackX,this);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(rotate_y_bone);	
	biY.set_callback		(bctCustom,BoneCallbackY,this);
	// set actor to mounted position
	const Fmatrix& A	= K->LL_GetTransform(actor_bone);
	Fvector ap;
	XFORM().transform_tiny	(ap,A.c);
	Fmatrix AP; AP.translate(ap);
	if(OwnerActor()) OwnerActor()->SetPhPosition	(AP);
	processing_activate		();
	return true;
}
void	CWeaponMounted::detach_Actor		()
{
	CActor *A = OwnerActor();
	if (A)
	{   // alpet: устранения крена, после пользования наклоненным пулеметом
		A->Orientation().roll = 0;  
		CCameraBase *fe = A->cam_FirstEye();
		if (fe)
			fe->roll = 0;
	}

	CKinematics *K = PKinematics(Visual());
	CHolderCustom::detach_Actor();
	// disable actor rotate callback
	CBoneInstance& biX		= K->LL_GetBoneInstance(rotate_x_bone);	
	biX.reset_callback		();
	CBoneInstance& biY		= K->LL_GetBoneInstance(rotate_y_bone);	
	biY.reset_callback		();
	// enable shell callback
	m_pPhysicsShell->EnabledCallbacks(TRUE);
	//закончить стрельбу
	FireEnd();

	processing_deactivate		();

	
	
}

Fvector	CWeaponMounted::ExitPosition		()
{
	return XFORM().c;
}

CCameraBase*	CWeaponMounted::Camera				()
{
	return camera;
}


void CWeaponMounted::FireStart()
{
	m_dAngle.set(0.0f,0.0f);
	CShootingObject::FireStart();
}
void CWeaponMounted::FireEnd()
{
	m_dAngle.set(0.0f,0.0f);
	CShootingObject::FireEnd();
	StopFlameParticles	();
	RemoveShotEffector ();
}


void CWeaponMounted::OnShot		()
{
	VERIFY(Owner());

	FireBullet(get_CurrentFirePoint(),fire_dir, 
		fireDispersionBase,
		m_CurrentAmmo, Owner()->ID(),ID(), SendHitAllowed(Owner()));

	StartShotParticles			();

	if(m_bLightShotEnabled) 
		Light_Start			();

	StartFlameParticles();
	StartSmokeParticles(fire_pos, zero_vel);
	OnShellDrop(fire_pos, zero_vel);

	bool b_hud_mode = (Level().CurrentEntity() == smart_cast<CObject*>(Owner()));
	HUD_SOUND::PlaySound(sndShot, fire_pos, Owner(), b_hud_mode);

	//добавить эффектор стрельбы
	AddShotEffector		();
	m_dAngle.set(	::Random.randF(-fireDispersionBase,fireDispersionBase),
					::Random.randF(-fireDispersionBase,fireDispersionBase));
}

void CWeaponMounted::UpdateFire()
{
	fTime -= Device.fTimeDelta;
	

	CShootingObject::UpdateFlameParticles();
	CShootingObject::UpdateLight();

	if(!IsWorking()){
		if(fTime<0) fTime = 0.f;
		return;
	}

	if(fTime<=0){
		OnShot();
		fTime += fTimeToFire;
	}else{
		angle_lerp		(m_dAngle.x,0.f,5.f,Device.fTimeDelta);
		angle_lerp		(m_dAngle.y,0.f,5.f,Device.fTimeDelta);
	}
}

const Fmatrix&	 CWeaponMounted::get_ParticlesXFORM	()
{
	return fire_bone_xform;
}

void CWeaponMounted::AddShotEffector				()
{
	if(OwnerActor())
	{
		CCameraShotEffector* S	= smart_cast<CCameraShotEffector*>(OwnerActor()->Cameras().GetCamEffector(eCEShot)); 
		if (!S)	S				= (CCameraShotEffector*)OwnerActor()->Cameras().AddCamEffector(xr_new<CCameraShotEffector> (camMaxAngle,camRelaxSpeed, 0.25f, 0.01f, 0.7f));
		R_ASSERT				(S);
		S->Shot					(0.01f);
	}
}

void  CWeaponMounted::RemoveShotEffector	()
{
	if(OwnerActor())
		OwnerActor()->Cameras().RemoveCamEffector	(eCEShot);
}