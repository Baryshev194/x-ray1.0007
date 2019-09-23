///////////////////////////////////////////////////////////////
// Phrase.h
// �����, ����������� ����� (������� �������)
///////////////////////////////////////////////////////////////

#pragma once

#include "PhraseScript.h"

class CPhraseDialog;
class CGameObject;

class CPhrase
{
private:
	friend CPhraseDialog;
public:
	CPhrase(void);
	virtual ~CPhrase(void);

	void				SetText			(LPCSTR text)		{m_text = text;}
	LPCSTR				GetText			()	const;

	void				SetID		(const shared_str& id)			{m_ID = id;}
	const shared_str&	GetID		()	const						{return m_ID;}

	int					GoodwillLevel	()	const			{return m_iGoodwillLevel;}

	bool				IsDummy			()	const;
	CPhraseScript*		GetPhraseScript	()					{return &m_PhraseScript;};

protected:
	//���������� ������ � ������ ���� �������
	shared_str		m_ID;
	//��������� ������������� �����
	xr_string		m_text;
	
	//����������� ������� ���������������, ����������� ��� ����
	//���� ����� ����� ���� �������
	int				m_iGoodwillLevel;
	
	//��� ������ ���������� �������
	CPhraseScript	m_PhraseScript;
};