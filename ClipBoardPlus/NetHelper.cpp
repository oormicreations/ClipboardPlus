#include "stdafx.h"
#include "NetHelper.h"


CNetHelper::CNetHelper()
{
}


CNetHelper::~CNetHelper()
{
}



UINT UsageProc(LPVOID param)
{
	int ver = *(int*)param;

	//static CHAR frmdata[] = ("appinstall=cbp_install&ver=1.0.0");
	static CHAR frmdata[30];
	sprintf_s(frmdata, "appinstall=cbp_install&ver=%d", ver);
	frmdata[29] = 0;

	static TCHAR hdrs[] = (_T("Content-Type: application/x-www-form-urlencoded"));
	LPCTSTR accept[2] = { _T("*/*"), NULL };

	HINTERNET hInternet = InternetOpen(_T("Oormi Usage"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	HINTERNET hSession = InternetConnect(hInternet, _T("oormi.in"), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);

	HINTERNET hReq = HttpOpenRequest(hSession, _T("POST"), _T("software/selftalkmsg/selftalkstat01.php"), NULL, NULL, accept, INTERNET_FLAG_SECURE, 1);

	//not working with unicode version
	//BOOL res = HttpSendRequest(hReq, _T("Content-Type: application/x-www-form-urlencoded"), _tcslen(hdrs), _T("appinstall=install&ver=1.0.0"), _tcslen(frmdata));
	BOOL res = HttpSendRequestA(hReq, ("Content-Type: application/x-www-form-urlencoded"), _tcslen(hdrs), frmdata, strlen(frmdata));

	InternetCloseHandle(hReq);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
	return 0;
}

void CNetHelper::ReportUsage(CString regSection, int ver)
{
	unsigned char *c;// = new char;
	UINT i;

	if (!AfxGetApp()->GetProfileBinary(regSection, _T("VERSION"), (LPBYTE*)&c, &i))
	{
		CWinThread* hTh1 = AfxBeginThread(UsageProc, (LPVOID)&ver/*UsageProc receives this as param */, THREAD_PRIORITY_NORMAL);
		AfxGetApp()->WriteProfileBinary(regSection, _T("VERSION"), (LPBYTE)&ver, sizeof(ver));
	}
	
	//char x = c[0];
	delete [] c;
	c = NULL;
}

CString CNetHelper::Fetch(CString url, CString useragent)
{
	CString content = _T("");
	DWORD flags;
	BOOL connected = InternetGetConnectedState(&flags, NULL);
	if (!connected)
	{
		return content;
	}

	HINTERNET hInet = InternetOpen(useragent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	HINTERNET hUrl = InternetOpenUrl(hInet, url, NULL, -1L,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE |
		INTERNET_FLAG_NO_CACHE_WRITE | WININET_API_FLAG_ASYNC, NULL);

	if (hUrl)
	{
		CHAR szBuffer[1024] = { 0 };

		DWORD dwRead;
		while (InternetReadFile(hUrl, szBuffer, sizeof(szBuffer) - 1, &dwRead) == TRUE)
		{
			if (dwRead > 0)
			{
				szBuffer[dwRead] = 0;
				CString tstr(CA2CT(szBuffer, CP_UTF8));
				content += tstr;
			}
			else break;
		}
	}

	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInet);

	return content;
}


void CNetHelper::Checkforupdates(int appVerMaj, int appVerMin, CString updateFile, CString downloadUrl, CString userAgent)
{
	CString content = Fetch(updateFile, userAgent);

	if (!content.IsEmpty())
	{
		CString ver1, ver2, durl;

		AfxExtractSubString(ver1, content, 0, '|'); //major ver num
		AfxExtractSubString(ver2, content, 1, '|'); //minor ver num
		AfxExtractSubString(durl, content, 2, '|'); //downlaod url

		int oldver = appVerMaj * 10 + appVerMin;
		int newver = _ttoi(ver1) * 10 + _ttoi(ver2);

		if (newver > oldver)
		{
			int res = AfxMessageBox(_T("A new version of Clipboard Plus is available for download. Would you like to download it now?"), MB_YESNO);
			if (res == IDYES)
			{
				ShellExecute(NULL, _T("open"), durl, NULL, NULL, SW_SHOWNORMAL);
			}
			else
			{
				AfxMessageBox(_T("Clipboard Plus can be updated from ") + downloadUrl);
			}
		}
		else
		{
			AfxMessageBox(_T("Your version of Clipboard Plus is up to date!"));
		}
	}

	else
	{
		AfxMessageBox(_T("Failed to connect to the internet!"));
	}
}

