#include "stdafx.h"
#include "SysHelper.h"


CSysHelper::CSysHelper()
{
}


CSysHelper::~CSysHelper()
{
}


CString CSysHelper::GetClipboardText()
{
	// Try opening the clipboard
	if (!OpenClipboard(0))
	{
		//AfxMessageBox(_T("<Failed to access the clipboard>"));
		//MS Word or Excel will cause problems by locking up the clipbroad many times. 
		//So return last clip in case of failure.
		return _T("");
	}

	// Get handle of clipboard object for unicode text
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr)
	{
		//AfxMessageBox(_T("<No data in the clipboard>"));
		CloseClipboard();
		return _T("");
	}

	// Lock the handle to get the actual text pointer
	TCHAR * pszText = static_cast<TCHAR*>(GlobalLock(hData));
	if (pszText == nullptr)
	{
		//AfxMessageBox(_T("<The clipboard has no text>"));
		// Release the lock
		GlobalUnlock(hData);

		// Release the clipboard
		CloseClipboard();

		return _T("");
	}

	// Save text in a string class instance
	CString text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return text;
}


BOOL CSysHelper::SetClipboardText(CString text)
{
	// Try opening the clipboard
	if (!OpenClipboard(0))
	{
		return FALSE;
	}

	LPTSTR  lptstrCopy;
	HGLOBAL hglbCopy;
	int iSize = (text.GetLength() + 1) * sizeof(TCHAR);

	hglbCopy = GlobalAlloc(GMEM_MOVEABLE, iSize);
	if (hglbCopy == NULL)
	{
		CloseClipboard();
		return FALSE;
	}

	// Lock the handle and copy the text to the buffer. 
	lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
	ZeroMemory(lptstrCopy, iSize); // adds null character also
	memcpy(lptstrCopy, text.GetBuffer(), text.GetLength() * sizeof(TCHAR));

	//lptstrCopy[iSize] = /*(TCHAR)*/0;    // null character 
	GlobalUnlock(hglbCopy);

	EmptyClipboard();
	HANDLE hData = SetClipboardData(CF_UNICODETEXT, hglbCopy);
	if (hData == nullptr)
	{
		CloseClipboard();
		return FALSE;
	}

	// Release the clipboard
	CloseClipboard();

	return TRUE;
}

CString CSysHelper::ReadStringFromFile(CString filename, BOOL prompt)
{
	CFile file;
	if (!file.Open(filename, CFile::modeRead))
	{
		if (prompt) AfxMessageBox(_T("Failed to open the file: ") + filename, MB_ICONERROR);
		return _T("");
	}

	UINT len = (UINT)file.GetLength();
	char * buf = new char[len + 1];

	file.Read(buf, len);
	file.Close();
	
	buf[len] = 0;


	CString uni;
	int cc = 0;
	// get length (cc) of the new widechar excluding the \0 terminator first
	if ((cc = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0) - 1) > 0)
	{
		// convert
		wchar_t *buf1 = uni.GetBuffer(cc);
		if (buf1) MultiByteToWideChar(CP_UTF8, 0, buf, -1, buf1, cc);
		uni.ReleaseBuffer();
	}

	delete[] buf;
	return uni;
}

CString CSysHelper::GetFileContent()
{
	CFileDialog DataFileOpenDialog(true, _T("rtf"), _T(""), OFN_HIDEREADONLY, _T("Rich text Files (*.rtf)|*.rtf|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	DataFileOpenDialog.m_ofn.lpstrTitle = _T("Load an RTF or Text File ...");
	//DataFileOpenDialog.m_ofn.lpstrInitialDir = GetUserDocumentPath(PREDICTED_FOLDER);
	INT_PTR res = DataFileOpenDialog.DoModal();
	if (res == IDCANCEL) return _T("");
	m_FileName = DataFileOpenDialog.GetPathName();
	if (m_FileName.IsEmpty()) return _T("");
	m_FileExt = DataFileOpenDialog.GetFileExt();

	m_FileTitle = DataFileOpenDialog.GetFileTitle();
	if (m_FileTitle.GetLength() > 10)
	{
		m_FileTitle.Truncate(10);
		m_FileTitle = m_FileTitle + _T("...");
	}

	return ReadStringFromFile(m_FileName);
}

BOOL CSysHelper::GetSaveFileNameType()
{
	CFileDialog DataFileOpenDialog(false, _T("rtf"), _T(""), OFN_HIDEREADONLY, _T("Rich text Files (*.rtf)|*.rtf|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	DataFileOpenDialog.m_ofn.lpstrTitle = _T("Save an RTF or Text File ...");
	//DataFileOpenDialog.m_ofn.lpstrInitialDir = GetUserDocumentPath(PREDICTED_FOLDER);

	INT_PTR res = DataFileOpenDialog.DoModal();
	if (res == IDCANCEL) return FALSE;
	m_FileName = DataFileOpenDialog.GetPathName();
	if (m_FileName.IsEmpty()) return FALSE;
	m_FileExt = DataFileOpenDialog.GetFileExt();

	m_FileTitle = DataFileOpenDialog.GetFileTitle();
	if (m_FileTitle.GetLength() > 10)
	{
		m_FileTitle.Truncate(10);
		m_FileTitle = m_FileTitle + _T("...");
	}

	return TRUE;
}

BOOL CSysHelper::SetFileContent(CString content)
{
	if (m_FileName.IsEmpty())
	{
		GetSaveFileNameType();
	}

	if(!SaveString(m_FileName, content)) m_FileName = _T("");
	
	return TRUE;

}

BOOL CSysHelper::SaveString(CString filename, CString content)
{
	if (filename.IsEmpty()) return FALSE;

	CFile file;
	BOOL res1 = file.Open(filename, CFile::modeWrite|CFile::modeCreate);
	if (!res1)
	{
		AfxMessageBox(_T("Error : Failed to save the file"));
		return FALSE;
	}

	UINT len = (UINT)content.GetLength() * sizeof(TCHAR);
	char * outputString = (char*)malloc(len+1);
	ZeroMemory(outputString, len+1);

	len = WideCharToMultiByte(CP_UTF8, NULL, content, -1, outputString, len, NULL, NULL);

	file.Write(outputString, ::strlen(outputString));
	file.Close();
	free(outputString);

	return TRUE;

}


BOOL CSysHelper::SaveStringAppend(CString filename, CString content)
{
	if (filename.IsEmpty()) return FALSE;

	CFile file;
	BOOL res1 = file.Open(filename, CFile::modeWrite);
	if (!res1)
	{
		AfxMessageBox(_T("Error : Failed to append the file"));
		return FALSE;
	}

	UINT len = (UINT)content.GetLength() * sizeof(TCHAR);
	char * outputString = (char*)malloc(len + 1);
	ZeroMemory(outputString, len + 1);

	len = WideCharToMultiByte(CP_UTF8, NULL, content, -1, outputString, len, NULL, NULL);

	file.SeekToEnd();
	file.Write(outputString, ::strlen(outputString));
	file.Close();
	free(outputString);

	return TRUE;

}


CString CSysHelper::GetAppFileName(UINT type)
{
	CString path = GetUserDocumentPath(CBP_USER_FOLDER);

	if (!path.IsEmpty())
	{
		CString fname;
		if (type == CBP_SNOTES_FILE)	fname = _T("\\CBP_StickyClips.txt");
		if (type == CBP_ALERT_FILE)		fname = _T("\\rem.wav");
		if (type == CBP_BACKUP_FILE)	fname = _T("\\CBP_Backup.txt");
		if (type == CBP_REM_FILE)		fname = _T("\\CBP_Reminders.txt");
		if (fname.IsEmpty()) return _T("");

		path = path + fname;
		return path;
	}

	return _T("");
}

CString CSysHelper::GetUserDocumentPath(UINT type)
{
	TCHAR my_documents[MAX_PATH];
	for (int i = 0; i < MAX_PATH; i++)my_documents[i] = 0;

	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

	if (result != S_OK)
	{
		return _T("");
	}
	else
	{
		CString str;
		str.SetString(my_documents);

		if (type == CBP_USER_FOLDER)
		{
			str = str + _T("\\Oormi Creations\\Clipboard Plus");
		}

		if (GetFileAttributes(str) == INVALID_FILE_ATTRIBUTES)
		{
			//path doesn't exist, attempt creation
			if (SHCreateDirectoryEx(NULL, str, NULL) == ERROR_SUCCESS)
			{
				return str;
			}
			else
			{
				return _T("");
			}
		}

		//path is valid
		return str;
	}

}

BOOL CSysHelper::CreateFileAndInit(CString filename, CString content)
{
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(filename) && GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		SaveString(filename, content);
		return TRUE;
	}
	else return TRUE;

	return FALSE;
}

static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* /*lpelfe*/, NEWTEXTMETRICEX* /*lpntme*/, int /*FontType*/, LPARAM lParam)
{
	LPARAM* l = (LPARAM*)lParam;
	*l = TRUE;
	return TRUE;
}

bool CSysHelper::IsFontInstalled(LPCTSTR lpszFont)
{
	// Get the screen DC
	CDC dc;
	if (!dc.CreateCompatibleDC(NULL))
	{
		return false;
	}
	LOGFONT lf = { 0 };
	// Any character set will do
	lf.lfCharSet = DEFAULT_CHARSET;
	// Set the facename to check for
	_tcscpy_s(lf.lfFaceName, lpszFont);
	LPARAM lParam = 0;
	// Enumerate fonts
	::EnumFontFamiliesEx(dc.GetSafeHdc(), &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)&lParam, 0);
	return lParam ? true : false;
}

void CSysHelper::SelectMultipleFiles(CString * files, int maxfiles)
{
	CFileDialog DataFileOpenDialog(true, _T("rtf"), _T(""), OFN_ALLOWMULTISELECT, _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	DataFileOpenDialog.m_ofn.lpstrTitle = _T("Select Files ...");
	//DataFileOpenDialog.m_ofn.lpstrInitialDir = GetUserDocumentPath(PREDICTED_FOLDER);

	int nLnBuff = maxfiles*256;
	TCHAR* pBuffFileSelect = new TCHAR[nLnBuff];

	memset(pBuffFileSelect, 0, nLnBuff * sizeof(TCHAR));

	DataFileOpenDialog.m_ofn.lpstrFile = pBuffFileSelect;
	DataFileOpenDialog.m_ofn.nMaxFile = nLnBuff;

	INT_PTR res = DataFileOpenDialog.DoModal();
	if (res == IDOK)
	{

		int f = 0;
		POSITION pos(DataFileOpenDialog.GetStartPosition());
		while (pos)
		{
			CString filename = DataFileOpenDialog.GetNextPathName(pos);

			files[f] = filename;
			f++;
			if (f >= maxfiles) break;
		}
	}

	delete[] pBuffFileSelect;
}
