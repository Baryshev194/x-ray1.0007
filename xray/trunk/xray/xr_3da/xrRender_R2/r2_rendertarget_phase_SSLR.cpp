#include "stdafx.h"

void CRenderTarget::phase_SSLR()
{
	u32 Offset = 0;
	Fvector2 p0,p1;

	struct v_SQ
	{
		Fvector4 p;
		Fvector2 uv0;
	};

	float _w = float(Device.dwWidth);
	float _h = float(Device.dwHeight);
	p0.set(.5f/_w, .5f/_h);
	p1.set((_w + .5f)/_w, (_h + .5f)/_h);

	/////////////////////////////////////////////////////////////////////////////////////
	// phase SSLR
	u_setrt(rt_SSLR_0, 0, 0, HW.pBaseZB);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//
	v_SQ* pv = (v_SQ*) RCache.Vertex.Lock(4, g_SQ->vb_stride, Offset);

	pv->p.set(EPS, float(_h + EPS), EPS, 1.f);
	pv->uv0.set(p0.x, p1.y);
	pv++;

	pv->p.set(EPS, EPS, EPS, 1.f);
	pv->uv0.set(p0.x, p0.y);
	pv++;

	pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f);
	pv->uv0.set(p1.x, p1.y);
	pv++;

	pv->p.set(float(_w + EPS), EPS, EPS, 1.f);
	pv->uv0.set(p1.x, p0.y);
	pv++;

	RCache.Vertex.Unlock(4,g_SQ->vb_stride);

	//
	RCache.set_Element(s_SSLR->E[0]);

	Fmatrix m_inv_v;
	m_inv_v.invert(Device.mView);
	RCache.set_c("m_inv_v", m_inv_v);
	RCache.set_c("SSLR_params", ps_ext_SSLR_L, ps_ext_SSLR_error, 1.f, 1.f);

	RCache.set_Geometry(g_SQ);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	/////////////////////////////////////////////////////////////////////////////////////
	// hblur
	u_setrt(rt_SSLR_1, 0, 0, HW.pBaseZB);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//
	pv = (v_SQ*) RCache.Vertex.Lock(4, g_SQ->vb_stride, Offset);

	pv->p.set(EPS, float(_h + EPS), EPS, 1.f);
	pv->uv0.set(p0.x, p1.y);
	pv++;

	pv->p.set(EPS, EPS, EPS, 1.f);
	pv->uv0.set(p0.x, p0.y);
	pv++;

	pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f);
	pv->uv0.set(p1.x, p1.y);
	pv++;

	pv->p.set(float(_w + EPS), EPS, EPS, 1.f);
	pv->uv0.set(p1.x, p0.y);
	pv++;

	RCache.Vertex.Unlock(4,g_SQ->vb_stride);

	//
	RCache.set_Element(s_SSLR->E[1]);

	RCache.set_c("blur_params", ps_ext_SSLR_blur, 0.f, _w, _h);

	RCache.set_Geometry(g_SQ);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	/////////////////////////////////////////////////////////////////////////////////////
	// vblur
	u_setrt(rt_SSLR_0, 0, 0, HW.pBaseZB);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//
	pv = (v_SQ*) RCache.Vertex.Lock(4, g_SQ->vb_stride, Offset);

	pv->p.set(EPS, float(_h + EPS), EPS, 1.f);
	pv->uv0.set(p0.x, p1.y);
	pv++;

	pv->p.set(EPS, EPS, EPS, 1.f);
	pv->uv0.set(p0.x, p0.y);
	pv++;

	pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f);
	pv->uv0.set(p1.x, p1.y);
	pv++;

	pv->p.set(float(_w + EPS), EPS, EPS, 1.f);
	pv->uv0.set(p1.x, p0.y);
	pv++;

	RCache.Vertex.Unlock(4,g_SQ->vb_stride);

	//
	RCache.set_Element(s_SSLR->E[2]);

	RCache.set_c("blur_params", 0.f, ps_ext_SSLR_blur, _w, _h);

	RCache.set_Geometry(g_SQ);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	//
	RCache.set_Stencil(FALSE);
}