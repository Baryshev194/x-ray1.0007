#pragma once

class CBlender_SSLR:public IBlender
{
public:
	virtual LPCSTR getComment() { return "EXT: SSLR"; }
	virtual BOOL canBeDetailed() { return FALSE; }
	virtual BOOL canBeLMAPped() { return FALSE; }

	virtual void Compile(CBlender_Compile& C);

	CBlender_SSLR();
	virtual ~CBlender_SSLR();
};