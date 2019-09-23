#include "stdafx.h"
#pragma hdrstop

#include <time.h>
#include "resource.h"
#include "log.h"
#include "../../build_config_defines.h"

BOOL						LogExecCB		= TRUE;
static string_path			logFName		= "engine.log";
static BOOL 				no_log			= TRUE;

str_container				verbosity_filters; // набор фильтров дл€ вывода регул€рных сообщений
 u32						verbosity_level = 3;

#define	LOG_TIME_PRECISE
bool __declspec(dllexport) force_flush_log = false;	// alpet: выставить в true если лог все-же записываетс€ плохо при вылете. —лишком часта€ запись лога вредит SSD и снижает произволительность.

#ifdef PROFILE_CRITICAL_SECTIONS
	static xrCriticalSection	logCS(MUTEX_PROFILE_ID(log));
#else // PROFILE_CRITICAL_SECTIONS
	static xrCriticalSection	logCS;
#endif // PROFILE_CRITICAL_SECTIONS
xr_vector<shared_str>*		LogFile			= NULL;
static LogCallback			LogCB			= 0;

IWriter *LogWriter;

size_t cached_log = 0;

void FlushLog			(LPCSTR file_name)
{
	if (LogWriter)
		LogWriter->flush();
	cached_log = 0;
}

void FlushLog			()
{
	FlushLog		(logFName);
}

#pragma optimize("gyt", off)

void __cdecl InitVerbosity (const char *filters)
{
	string4096 tmp;
	strcpy_s(tmp, 4095, filters);
	tmp[sizeof(tmp) - 1] = 0;
	char *t = strtok(tmp, ",");
	while (NULL != t)
	{
		if (xr_strlen(t) > 0)
			verbosity_filters.dock(t)->dwReference++;
		t = strtok(NULL, ",");
	}

}

extern bool shared_str_initialized;


void AddOne				(const char *split) 
{
	if(!LogFile)		
						return;

	logCS.Enter			();

#ifdef DEBUG
	OutputDebugString	(split);
	OutputDebugString	("\n");
#endif

	//exec CallBack
	if (LogExecCB&&LogCB)LogCB(split);

//	DUMP_PHASE;
	{
		if (shared_str_initialized)
		{
			shared_str			temp = shared_str(split);
			LogFile->push_back(temp);
		}

		//+RvP, alpet
		if (LogWriter)
		{				
			switch (*split)
			{
			case 0x21:
			case 0x23:
			case 0x25:
				split ++; // пропустить первый символ, т.к. это веро€тно цветовой тег
				break;
			}

			char buf[64];
#ifdef	LOG_TIME_PRECISE 
			SYSTEMTIME lt;
			GetLocalTime(&lt);
			
			sprintf_s(buf, 64, "[%02d.%02d.%02d %02d:%02d:%02d.%03d] ", lt.wDay, lt.wMonth, lt.wYear % 100, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);						
			LogWriter->w_printf("%s%s\r\n", buf, split);
			cached_log += xr_strlen(buf);
			cached_log += xr_strlen(split) + 2;
#else
			time_t t = time(NULL);
			tm* ti = localtime(&t);
			
			strftime(buf, 64, "[%x %X]\t", ti);

			LogWriter->wprintf("%s %s\r\n", buf, split);
#endif
			if (force_flush_log || cached_log >= 32768)
				FlushLog();
		}
		//-RvP
	}


	logCS.Leave				();
}

void Log				(const char *s) 
{
	int		i,j;
	char	split[1024];

	for (i=0,j=0; s[i]!=0; i++) {
		if (s[i]=='\n') {
			split[j]=0;	// end of line
			if (split[0]==0) { split[0]=' '; split[1]=0; }
			AddOne(split);
			j=0;
		} else {
			split[j++]=s[i];
		}
	}
	split[j]=0;
	AddOne(split);
}


void __cdecl LogVAList(const char *format, va_list &mark)
{
	string4096	buf; // alpet: размер буфера лучше сделать побольше, чтобы избежать вылетов invalid parameter handler при выводе стеков вызова
	int sz		= _vsnprintf(buf, sizeof(buf)-1, format, mark); buf[sizeof(buf)-1]=0;
    va_end		(mark);
	if (sz)		Log(buf);
}

void __cdecl Msg		( const char *format, ...)
{
	va_list mark;	
	va_start	(mark, format );
	LogVAList   (format, mark);
}

void __cdecl	MsgCB (LPCSTR format, ...) // alpet: вывод сообщений только в колбек (дл€ отладки и передачи данных в перехватчик)
{
	static string4096 ctx_ring[16];   // кольцевой буфер дл€ сохранени€ данных контекста выполнени€ (выводитс€ при сбое, или по необходимости)
	static u32 ctx_index = 0;

	va_list mark;
	string4096	buf;
	va_start(mark, format);
	int sz = _vsnprintf(buf, sizeof(buf)-1, format, mark); buf[sizeof(buf)-1] = 0;
	va_end(mark);
	// функци€ двойного назначени€: может использоватьс€ дл€ вотчинга произвольных переменных в местах потенциальных сбоев
	if (strstr(buf, "#CONTEXT:"))
	{
		SYSTEMTIME lt;
		GetLocalTime(&lt);
		LPSTR dest = ctx_ring[ctx_index & 15];
		ctx_index++;
		// copy-paste forever	
		sprintf_s(dest, sizeof(buf) - 1, "[%02d:%02d:%02d.%03d]. %s", lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds, buf);						
		return;
	}
	if (strstr(buf, "#DUMP_CONTEXT"))
	{
		Log("#DEBUG CONTEXT DUMP:");
		for (u32 i = 15; i > 0; i--)
			Msg("# %s", ctx_ring[(ctx_index + i) & 15]);

		return;
	}

	if (NULL != LogCB && sz)		LogCB(buf);
}


void __cdecl MsgV (const char *verbosity, const char *format, ...)
{
	if (!verbosity) return;	
	bool b_show = (verbosity[0] != '!');
	if (!b_show)
		verbosity++;

	if (verbosity[0] >= '0' && verbosity[0] <= '9')
	{
		u32 msg_level = verbosity[0] - '0';
		if (msg_level > verbosity_level) return;
		verbosity++;
	}
	str_value *f = verbosity_filters.dock(verbosity);
	if ( (f->dwReference > 0) == b_show )
	{		
		va_list mark;
		va_start(mark, format);		
		LogVAList (format, mark);
	}
	// f->dwReference--;
}

void Log				(const char *msg, const char *dop) {
	char buf[1024];

	if (dop)	sprintf_s(buf,sizeof(buf),"%s %s",msg,dop);
	else		sprintf_s(buf,sizeof(buf),"%s",msg);

	Log		(buf);
}

void Log				(const char *msg, u32 dop) {
	char buf[1024];

	sprintf_s	(buf,sizeof(buf),"%s %d",msg,dop);
	Log			(buf);
}

void Log				(const char *msg, int dop) {
	char buf[1024];

	sprintf_s	(buf, sizeof(buf),"%s %d",msg,dop);
	Log		(buf);
}

void Log				(const char *msg, float dop) {
	char buf[1024];

	sprintf_s	(buf, sizeof(buf),"%s %f",msg,dop);
	Log		(buf);
}

void Log				(const char *msg, const Fvector &dop) {
	char buf[1024];

	sprintf_s	(buf,sizeof(buf),"%s (%f,%f,%f)",msg,dop.x,dop.y,dop.z);
	Log		(buf);
}

void Log				(const char *msg, const Fmatrix &dop)	{
	char	buf	[1024];

	sprintf_s	(buf,sizeof(buf),"%s:\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",msg,dop.i.x,dop.i.y,dop.i.z,dop._14_
																				,dop.j.x,dop.j.y,dop.j.z,dop._24_
																				,dop.k.x,dop.k.y,dop.k.z,dop._34_
																				,dop.c.x,dop.c.y,dop.c.z,dop._44_);
	Log		(buf);
}

void LogWinErr			(const char *msg, long err_code)	{
	Msg					("%s: %s",msg,Debug.error2string(err_code)	);
}

typedef void (WINAPI *OFFSET_UPDATER)(LPCSTR key, u32 ofs);


void	LogXrayOffset(LPCSTR key, LPVOID base, LPVOID pval)
{
#ifdef LUAICP_COMPAT
	u32 ofs = (u32)pval - (u32)base;
	MsgV	  ("XRAY_OFFSET", "XRAY_OFFSET: %30s = 0x%x base = 0x%p, pval = 0x%p ", key, ofs, base, pval);
	static OFFSET_UPDATER cbUpdater = NULL;
	HMODULE hDLL = GetModuleHandle("luaicp.dll");
	if (!cbUpdater && hDLL)
		cbUpdater = (OFFSET_UPDATER) GetProcAddress(hDLL, "UpdateXrayOffset");

	if (cbUpdater)
		cbUpdater(key, ofs);
#endif
}


void SetLogCB			(LogCallback cb)
{
	LogCB				= cb;
}

LogCallback  GetLogCB()
{
	return LogCB;
}


LPCSTR log_name			()
{
	return				(logFName);
}

void InitLog()
{
	R_ASSERT			(LogFile==NULL);
	LogFile				= xr_new< xr_vector<shared_str> >();
}

void CreateLog			(BOOL nl)
{
    no_log				= nl;
	strconcat			(sizeof(logFName),logFName,Core.ApplicationName,"_",Core.UserName,".log");
	if (FS.path_exist("$logs$"))
		FS.update_path	(logFName,"$logs$",logFName);
	if (!no_log){

		LogWriter = FS.w_open	(logFName);
        if (LogWriter == NULL){
        	MessageBox	(NULL,"Can't create log file.","Error",MB_ICONERROR);
        	abort();
        }		
	
		time_t t = time(NULL);
		tm* ti = localtime(&t);
		char buf[64];
		strftime(buf, 64, "[%x %X]\t", ti);

        for (u32 it=0; it<LogFile->size(); it++)	{
			LPCSTR		s	= *((*LogFile)[it]);
			LogWriter->w_printf("%s%s\n", buf, s?s:"");
		}
		LogWriter->flush();
    }
	LogFile->reserve		(128);
}

void CloseLog(void)
{
	if(LogWriter){
		FS.w_close(LogWriter);
	}

	FlushLog		();
 	LogFile->clear	();
	xr_delete		(LogFile);
}
