#pragma once
#define CBP_USER_FOLDER 1
#define CBP_SNOTES_FILE 1
#define CBP_ALERT_FILE 2
#define CBP_BACKUP_FILE 3

#define TWIPS_PER_PT 20
#define WM_CBP_RESTORE WM_USER + 100

class CSysHelper
{
public:
	CSysHelper();
	~CSysHelper();

	CString m_FileName, m_FileTitle, m_FileExt;

	CString GetClipboardText();
	BOOL SetClipboardText(CString text);
	CString GetFileContent();
	BOOL SetFileContent(CString content);
	BOOL SaveString(CString filename, CString content);
	BOOL SaveStringAppend(CString filename, CString content);
	CString GetAppFileName(UINT type);
	CString GetUserDocumentPath(UINT type);
	BOOL CreateFileAndInit(CString filename, CString content);
	CString ReadStringFromFile(CString filename, BOOL prompt = TRUE);
	bool IsFontInstalled(LPCTSTR lpszFont);
	void SelectMultipleFiles(CString * files, int maxfiles);
	BOOL GetSaveFileNameType();
};

