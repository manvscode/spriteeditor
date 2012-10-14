#ifndef _MEMLEAKCHECK_H_
#define _MEMLEAKCHECK_H_

#define MEMLEAK_CHECK \
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); \
	flag |= _CRTDBG_LEAK_CHECK_DF; \
	_CrtSetDbgFlag(flag);


#endif