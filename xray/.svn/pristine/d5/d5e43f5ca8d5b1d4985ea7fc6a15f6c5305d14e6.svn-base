// UISleepWnd.h:  окошко для выбора того, сколько спать или ждать
// Red_Virus
//////////////////////////////////////////////////////////////////////

#pragma once

#include "uiwindow.h"

class CUIButton;
class CUIStatic;
//class CUITrackButton;

class CUISleepWnd: public CUIWindow
{
private:
	typedef			CUIWindow inherited;
public:
						CUISleepWnd			();
	virtual				~CUISleepWnd		();

	void				Init				();
	virtual void		Draw				();
	virtual void		Show				();
	virtual void		Hide				();
	void				SendMessage			(CUIWindow *pWnd, s16 msg, void *pData);

	// Изменяем текущее установленное время отдыха на дельта-значения
	void				ModifyRestTime		(s8 dHours, s8 dMinutes);
	// Сбрасываем время в 0
	void				ResetTime			()								{ SetRestTime(0, 0); }
	
protected:
	// Устанавливаем на отображение время для сна
	void				SetRestTime			(u8 hours, u8 minutes);
	// Текущее запоменное время отдыха
	s8					m_Hours, m_Minutes;

	// Контролы
	CUIStatic*			UIStaticRestAmount;
	//********
	CUIButton*			UIPlusBtn;
	CUIButton*			UIMinusBtn;
	//********
	CUIButton*			UISleepBtn; // кнопка - спать
	CUIButton*			UIWaitBtn;	//кнопка - ожидать
	//CUIButton*		UICloseBtn;
};