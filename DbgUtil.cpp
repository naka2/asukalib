#include "DbgUtil.h"

VOID DebugWrite(LPSTR format, ...)
{
	FILE *fp = NULL;
	va_list ap = NULL;
	DWORD time = 0;
	CHAR string[256];;

	time = timeGetTime();

	if(!string || 0 == (fopen_s(&fp,"debug_report.txt","a+")))
		return;

	va_start(ap,format);
	sprintf_s(string,ARRAYSIZE(string),"[%010d]: %s",time,format);
	vfprintf(fp,string,ap);
	va_end(ap);

	fclose(fp);
}
