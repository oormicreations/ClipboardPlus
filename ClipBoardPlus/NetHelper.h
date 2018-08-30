#pragma once
#include "wininet.h"

class CNetHelper
{
public:
	CNetHelper();
	~CNetHelper();

	void Checkforupdates(int appVerMaj, int appVerMin, CString updateFile, CString downloadUrl, CString userAgent);
	CString Fetch(CString url, CString useragent);
	void ReportUsage(CString regSection, int ver);
};

