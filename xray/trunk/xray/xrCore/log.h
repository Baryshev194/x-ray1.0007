#ifndef logH
#define logH

//+RvP
#include <Windows.h>
#include <fstream>
#include <ctime>
//-RvP

#define VPUSH(a)	a.x,a.y,a.z

extern  XRCORE_API  u32				verbosity_level;

void	XRCORE_API  __cdecl			InitVerbosity(const char *filters);

void 	XRCORE_API	__cdecl			Msg			(LPCSTR format, ...);
void	XRCORE_API	__cdecl			MsgV		(const char *verbosity, const char *format, ...);    // alpet: вывод фильтруемых сообщений
void 	XRCORE_API	__cdecl			MsgCB		(LPCSTR format, ...);	// alpet: вывод сообщений только в колбек (для отладки) или в контекстный буфер

void 	XRCORE_API		Log			(LPCSTR msg);
void 	XRCORE_API		Log			(LPCSTR msg);
void 	XRCORE_API		Log			(LPCSTR msg, LPCSTR			dop);
void 	XRCORE_API		Log			(LPCSTR msg, u32			dop);
void 	XRCORE_API		Log			(LPCSTR msg, int  			dop);
void 	XRCORE_API		Log			(LPCSTR msg, float			dop);
void 	XRCORE_API		Log			(LPCSTR msg, const Fvector& dop);
void 	XRCORE_API		Log			(LPCSTR msg, const Fmatrix& dop);
void 	XRCORE_API		LogWinErr	(LPCSTR msg, long 			err_code);

void	XRCORE_API		LogXrayOffset(LPCSTR key, LPVOID base, LPVOID pval);


typedef void	( * LogCallback)	(LPCSTR string);
void	XRCORE_API				SetLogCB	(LogCallback cb);
LogCallback XRCORE_API			GetLogCB	();
void 							CreateLog	(BOOL no_log=FALSE);
void 							InitLog		();
void 							CloseLog	();
void	XRCORE_API				FlushLog	();

extern 	XRCORE_API	xr_vector<shared_str>*		LogFile;
extern 	XRCORE_API	BOOL						LogExecCB;


u32   XRCORE_API		SimpleExceptionFilter(PEXCEPTION_POINTERS pExPtrs);
#define SIMPLE_FILTER	SimpleExceptionFilter(GetExceptionInformation())



#endif

