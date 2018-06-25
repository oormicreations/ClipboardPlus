#pragma once
#include "wininet.h"

class CNetHelper
{
public:
	CNetHelper();
	~CNetHelper();

	void Checkforupdates(int appVer, CString updateFile, CString downloadUrl, CString userAgent);
	CString Fetch(CString url, CString useragent);
	void ReportUsage(CString regSection, CString regKey);
};

