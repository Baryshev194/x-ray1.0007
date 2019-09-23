// UISleepWnd.cpp:  окошко дл€ выбора того, сколько спать или ждать
// Red_Virus
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pch_script.h"
#include "UIWindow.h"
#include "UISleepWnd.h"
#include "../alife_space.h"
#include "../actor.h"
#include "../script_engine.h"
#include "../script_engine_space.h"
#include "../script_process.h"
#include "../ai_space.h"
#include "UIXmlInit.h"
#include "UIButton.h"

CUISleepWnd::CUISleepWnd()
	: m_Hours		(0),
	  m_Minutes		(0)
{
}

CUISleepWnd::~CUISleepWnd()
{
}


void CUISleepWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "sleep_wnd.xml");
	R_ASSERT2(xml_result, "xml file not found sleep_dialog_new.xml");

	CUIXmlInit	xml_init;
	
	xml_init.InitWindow					(uiXml, "main", 0, this);
	
	UIStaticRestAmount		= xr_new<CUIStatic>();UIStaticRestAmount->SetAutoDelete(true);
	AttachChild				(UIStaticRestAmount);
	xml_init.InitStatic		(uiXml, "sleep_wait_amount_static", 0, UIStaticRestAmount);

	UIPlusBtn				= xr_new<CUIButton>();UIPlusBtn->SetAutoDelete(true);
	AttachChild				(UIPlusBtn);
	xml_init.InitButton		(uiXml, "plus_button", 0, UIPlusBtn);

	UIMinusBtn				= xr_new<CUIButton>();UIMinusBtn->SetAutoDelete(true);
	AttachChild(UIMinusBtn);
	xml_init.InitButton(uiXml, "minus_button", 0, UIMinusBtn);

	UISleepBtn				= xr_new<CUIButton>();UISleepBtn->SetAutoDelete(true);
	AttachChild(UISleepBtn);
	xml_init.InitButton(uiXml, "sleep_button", 0, UISleepBtn);
	
	UIWaitBtn				= xr_new<CUIButton>();UIWaitBtn->SetAutoDelete(true);
	AttachChild(UIWaitBtn);
	xml_init.InitButton(uiXml, "wait_button", 0, UIWaitBtn);

	ResetTime();
}


void CUISleepWnd::Draw()
{
	CUIWindow::Draw						();
}

void CUISleepWnd::Show() 
{ 
	inherited::Show(true);
}

void CUISleepWnd::Hide()
{
	inherited::Show(false);
}

void CUISleepWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	const s8 deltaMinutes = 15;
	

	if(pWnd == UISleepBtn && msg == BUTTON_CLICKED)
	{
		u32 restMsec = (m_Hours * 3600 + m_Minutes * 60) * 1000;
		if (restMsec != 0){

				luabind::functor<void>	sleep;
				if (ai().script_engine().functor("sleep_manager.sleep",sleep))
					sleep(restMsec);
			
		}
	}
	else if(pWnd == UIWaitBtn && msg == BUTTON_CLICKED)
	{
		u32 restMsec = (m_Hours * 3600 + m_Minutes * 60) * 1000;
		if (restMsec != 0){
				luabind::functor<void>	wait;
				if (ai().script_engine().functor("sleep_manager.wait",wait))
					wait(restMsec);
		}
	}
	else if(pWnd == UIPlusBtn && msg == BUTTON_CLICKED)
	{
		// Add fixed amount of minutes and hours
		ModifyRestTime(0, deltaMinutes);
	}
	else if(pWnd == UIMinusBtn && msg == BUTTON_CLICKED)
	{
		// Add fixed amount of minutes and hours
		ModifyRestTime(0, -deltaMinutes);
	}
	else if ((UIPlusBtn == pWnd || UIMinusBtn == pWnd || UISleepBtn == pWnd || UIWaitBtn == pWnd) && BUTTON_DOWN == msg)
	{
		CUIButton *pBtn = smart_cast<CUIButton*>(pWnd);
		R_ASSERT(pBtn);

		pBtn->EnableTextHighlighting(false);
	}
	
	if ((UIPlusBtn == pWnd || UIMinusBtn == pWnd || UISleepBtn == pWnd || UIWaitBtn == pWnd) && BUTTON_CLICKED == msg)
	{
		CUIButton *pBtn = smart_cast<CUIButton*>(pWnd);
		R_ASSERT(pBtn);

		pBtn->EnableTextHighlighting(true);
	}

	inherited::SendMessage(pWnd, msg, pData);
}

void CUISleepWnd::ModifyRestTime(s8 dHours, s8 dMinutes)
{

	// ѕроверка на влидный интервал
	R_ASSERT(dMinutes < 61);

	s8 oldMin = m_Minutes;

	// ”величиваем минуты
	m_Minutes = m_Minutes + dMinutes;

	// ѕровер€ем выходы за пределы допуска
	if (m_Minutes > 59)
	{
		++m_Hours;
		m_Minutes = m_Minutes - 60;
	}
	else if (m_Minutes < 0)
	{
		--m_Hours;
		m_Minutes = m_Minutes + 60;
	}

	// “оже самое и дл€ часов
	m_Hours = m_Hours + dHours;
	if (m_Hours > 99)
	{
		m_Hours = 99;
		m_Minutes = oldMin;
	}
	else if (m_Hours < 0)
	{
		m_Minutes = oldMin;
		m_Hours = 0;
	}

	SetRestTime(m_Hours, m_Minutes);
}

void CUISleepWnd::SetRestTime(u8 hours, u8 minutes)
{
	string32					buf;
	static s8 _h				= pSettings->r_s8("actor", "max_sleep_hours");
	m_Minutes					= minutes;
	m_Hours						= hours;
	if(m_Hours>=_h)				m_Minutes = 0;
	clamp						(m_Hours,s8(0),_h);
	sprintf_s						(buf, "%02i:%02i", m_Hours, m_Minutes);
	UIStaticRestAmount->SetText	(buf);
}