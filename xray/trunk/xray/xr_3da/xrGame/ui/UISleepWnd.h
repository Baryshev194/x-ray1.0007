// UISleepWnd.h:  ������ ��� ������ ����, ������� ����� ��� �����
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

	// �������� ������� ������������� ����� ������ �� ������-��������
	void				ModifyRestTime		(s8 dHours, s8 dMinutes);
	// ���������� ����� � 0
	void				ResetTime			()								{ SetRestTime(0, 0); }
	
protected:
	// ������������� �� ����������� ����� ��� ���
	void				SetRestTime			(u8 hours, u8 minutes);
	// ������� ���������� ����� ������
	s8					m_Hours, m_Minutes;

	// ��������
	CUIStatic*			UIStaticRestAmount;
	//********
	CUIButton*			UIPlusBtn;
	CUIButton*			UIMinusBtn;
	//********
	CUIButton*			UISleepBtn; // ������ - �����
	CUIButton*			UIWaitBtn;	//������ - �������
	//CUIButton*		UICloseBtn;
};