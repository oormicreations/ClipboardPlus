
// ClipBoardPlusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClipBoardPlus.h"
#include "ClipBoardPlusDlg.h"
#include "afxdialogex.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClipBoardPlusDlg dialog


CClipBoardPlusDlg::CClipBoardPlusDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIPBOARDPLUS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClipBoardPlusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_ClipButton[0]);
	DDX_Control(pDX, IDC_BUTTON2, m_ClipButton[1]);
	DDX_Control(pDX, IDC_BUTTON3, m_ClipButton[2]);
	DDX_Control(pDX, IDC_BUTTON4, m_ClipButton[3]);
	DDX_Control(pDX, IDC_BUTTON5, m_ClipButton[4]);
}

BEGIN_MESSAGE_MAP(CClipBoardPlusDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CClipBoardPlusDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CClipBoardPlusDlg::OnBnClickedButton2)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CClipBoardPlusDlg::OnBnClickedOk)
	ON_WM_CLIPBOARDUPDATE()
	ON_BN_CLICKED(IDC_BUTTON3, &CClipBoardPlusDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CClipBoardPlusDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CClipBoardPlusDlg::OnBnClickedButton5)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MENU_EXIT, &CClipBoardPlusDlg::OnMenuExit)
	ON_COMMAND(ID_MENU_CLEAR, &CClipBoardPlusDlg::OnMenuClear)
	ON_COMMAND(ID_MENU_CLEARALL, &CClipBoardPlusDlg::OnMenuClearall)
	ON_COMMAND(ID_MENU_PASSWORDMODE, &CClipBoardPlusDlg::OnMenuPasswordmode)
	ON_COMMAND(ID_MENU_CLEARCLIPBOARD, &CClipBoardPlusDlg::OnMenuClearclipboard)
	ON_COMMAND(ID_MENU_SAVE, &CClipBoardPlusDlg::OnMenuSave)
	ON_COMMAND(ID_MENU_SAVEALL, &CClipBoardPlusDlg::OnMenuSaveall)
	ON_COMMAND(ID_MENU_HELP, &CClipBoardPlusDlg::OnMenuHelp)
	ON_MESSAGE(WM_TRAY_MESSAGE, OnTrayNotify)
	ON_COMMAND(ID_MENU_MINIMIZETO, &CClipBoardPlusDlg::OnMenuMinimizeto)
	ON_COMMAND(ID_TRAY_RESTORE, &CClipBoardPlusDlg::OnTrayRestore)
	ON_COMMAND(ID_TRAY_EXIT, &CClipBoardPlusDlg::OnTrayExit)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_MENU_CHECKFORUPDATES, &CClipBoardPlusDlg::OnMenuCheckforupdates)
	ON_COMMAND(ID_MENU_ABOUT, &CClipBoardPlusDlg::OnMenuAbout)
END_MESSAGE_MAP()


// CClipBoardPlusDlg message handlers

BOOL CClipBoardPlusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	if (!AddClipboardFormatListener(this->GetSafeHwnd()))
	{
		AfxMessageBox(_T("AddClipboardFormatListener failed!"));
	}

	m_CBPVersion = 1;
	m_PasswordMode = TRUE;
	m_IsClipBoardPlusEvent = FALSE;
	m_RightClickedButton = -1;
	m_MenuPopup.LoadMenu(IDR_MENU_CBP);
	
	SetupMinimizeToTray();
	InitClips();
	GetClip(); //get already present contents

	m_NetHelper.ReportUsage(_T("ClipboardPlus"), _T("INST"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClipBoardPlusDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClipBoardPlusDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClipBoardPlusDlg::InitClips()
{
	for (int i = 0; i < MAXCLIPS; i++)
	{
		m_ClipButton[i].SetSkin(IDB_BITMAP_CLIP_COLD, IDB_BITMAP_CLIP_COLD, RGB(60, 60, 60));
		m_Clips[i] = _T("");
	}
}

void CClipBoardPlusDlg::ClearClip(int nclip)
{
	m_Clips[nclip] = _T("");
	m_ClipButton[nclip].SetText(_T(""));
}

void CClipBoardPlusDlg::MaskClips()
{
	for (int i = 0; i < MAXCLIPS; i++)
	{
		if (IsPassword(m_Clips[i]) && m_PasswordMode)
		{
			m_ClipButton[i].SetText(_T("* * * * * * * *"));
		}
		else m_ClipButton[i].SetText(m_Clips[i]);
	}

}

void CClipBoardPlusDlg::GetClip()
{
	CString tmpClip;
	CString newClip = GetClipboardText();

	if (m_Clips[0] == newClip)
	{
		return;
	}

	if (!m_Clips[0].IsEmpty())
	{
		for (int i = MAXCLIPS - 1; i > 0; i--)
		{
			tmpClip = m_Clips[i - 1];
			m_Clips[i] = tmpClip;
			m_ClipButton[i].SetText(m_Clips[i]);
		}
	}

	m_Clips[0] = newClip;
	m_ClipButton[0].SetText(m_Clips[0]);
	m_ClipButton[0].SetSkin(IDB_BITMAP_CLIP_HOT, IDB_BITMAP_CLIP_HOT, RGB(30, 30, 30));

	MaskClips();
}


void CClipBoardPlusDlg::ScrollClips(int nclip)
{
	CString tmpClip;
	CString hotClip = m_Clips[nclip];

	for (int i = nclip; i > 0; i--)
	{
		tmpClip = m_Clips[i - 1];
		m_Clips[i] = tmpClip;
		m_ClipButton[i].SetText(m_Clips[i]);
	}

	m_Clips[0] = hotClip;
	m_ClipButton[0].SetText(m_Clips[0]);
	m_ClipButton[0].SetSkin(IDB_BITMAP_CLIP_HOT, IDB_BITMAP_CLIP_HOT, RGB(30, 30, 30));

	MaskClips();

}


CString CClipBoardPlusDlg::GetClipboardText()
{
	// Try opening the clipboard
	if (!OpenClipboard())
	{
		//AfxMessageBox(_T("<Failed to access the clipboard>"));
		//MS Word or Excel will cause problems by locking up the clipbroad many times. 
		//So return last clip in case of failure.
		return m_Clips[0]; 
	}

	// Get handle of clipboard object for unicode text
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr)
	{
		//AfxMessageBox(_T("<No data in the clipboard>"));
		CloseClipboard();
		return m_Clips[0];
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

		return m_Clips[0];
	}

	// Save text in a string class instance
	CString text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return text;
}


BOOL CClipBoardPlusDlg::SetClipboardText(CString text)
{
	//if(text.IsEmpty()) return FALSE;

	m_IsClipBoardPlusEvent = TRUE;

	// Try opening the clipboard
	if (!OpenClipboard())
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

void CClipBoardPlusDlg::OnDestroy()
{
	if (!m_bMinimized) //store window position only if not minimized to tray
	{
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		AfxGetApp()->WriteProfileBinary(_T("ClipboardPlus"), _T("WP"), (LPBYTE)&wp, sizeof(wp));
	}

	CDialogEx::OnDestroy();

	if (m_bMinimized)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_TrayData);
	}

	// Unload the menu resources
	m_MenuTray.DestroyMenu();
	m_MenuPopup.DestroyMenu();
}


void CClipBoardPlusDlg::OnBnClickedOk()
{
	if (!RemoveClipboardFormatListener(this->GetSafeHwnd()))
	{
		AfxMessageBox(_T("RemoveClipboardFormatListener failed!"));
	}

	CDialogEx::OnOK();
}


void CClipBoardPlusDlg::OnClipboardUpdate()
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.

	if(!m_IsClipBoardPlusEvent) GetClip();
	m_IsClipBoardPlusEvent = FALSE;

	CDialogEx::OnClipboardUpdate();
}


void CClipBoardPlusDlg::OnBnClickedButton1()
{

}

void CClipBoardPlusDlg::OnBnClickedButton2()
{
	SetClipboardText(m_Clips[1]);
	ScrollClips(1);
}

void CClipBoardPlusDlg::OnBnClickedButton3()
{
	SetClipboardText(m_Clips[2]);
	ScrollClips(2);
}


void CClipBoardPlusDlg::OnBnClickedButton4()
{
	SetClipboardText(m_Clips[3]);
	ScrollClips(3);
}


void CClipBoardPlusDlg::OnBnClickedButton5()
{
	SetClipboardText(m_Clips[4]);
	ScrollClips(4);
}


void CClipBoardPlusDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu *pSubMenu = m_MenuPopup.GetSubMenu(0);
	ASSERT(pSubMenu);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	m_RightClickedButton = -1;
	CRect brect;
	for (int i = 0; i < MAXCLIPS; i++)
	{
		m_ClipButton[i].GetWindowRect(&brect);
		if (brect.PtInRect(point))
		{
			m_RightClickedButton = i;
			break;
		}
	}
	
	//TRACE("b=%d\n", m_RightClickedButton);
}


void CClipBoardPlusDlg::OnMenuExit()
{
	OnBnClickedOk();
}


void CClipBoardPlusDlg::OnMenuClear()
{
	if (m_RightClickedButton >= 0) ClearClip(m_RightClickedButton);
}


void CClipBoardPlusDlg::OnMenuClearall()
{
	for (int i = 0; i < MAXCLIPS; i++)
	{
		ClearClip(i);
	}
}


void CClipBoardPlusDlg::OnMenuPasswordmode()
{
	m_PasswordMode = !m_PasswordMode;
	m_MenuPopup.CheckMenuItem(ID_MENU_PASSWORDMODE, (MF_CHECKED * (UINT)m_PasswordMode) | MF_BYCOMMAND);

	MaskClips();
}

BOOL CClipBoardPlusDlg::IsPassword(CString clip)
{
	//should not be more than one word
	if (clip.Find(' ') >= 0) return FALSE;

	//should not be too long, like a link etc
	if (clip.GetLength() > 10) return FALSE;

	TCHAR nums[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
		L'०', L'१', L'२', L'३', L'४', L'५', L'६', L'७', L'८', L'९'}; //L'x' is necessary

	TCHAR spl[] = { '~', '`', '!', '@', '#', '$', '%', '^', '&', '*', 
		'(', ')', '-', '_', '+', '=', '|', '\\', '{', '[', '}', ']',
		':', ';', '\"', '\'', '<', ',', '>', '.', '?', '/'}; //32

	BOOL hasnum = FALSE;
	BOOL hasspl = FALSE;

	for (int i = 0; i < 20; i++)
	{
		if (clip.Find(nums[i]) >= 0)
		{
			hasnum = TRUE;
			break;
		}
	}

	for (int i = 0; i < 32; i++)
	{
		if (clip.Find(spl[i]) >= 0)
		{
			hasspl = TRUE;
			break;
		}
	}

	return hasnum && hasspl;

}




void CClipBoardPlusDlg::OnMenuClearclipboard()
{
	////EmptyClipboard causes problems
	//if (OpenClipboard())
	//{
	//	EmptyClipboard();
	//	CloseClipboard();
	//}

	m_Clips[0] = _T("");
	m_ClipButton[0].SetText(m_Clips[0]);
	SetClipboardText(m_Clips[0]);

}


void CClipBoardPlusDlg::OnMenuSave()
{
	if (m_RightClickedButton >= 0) SaveClips(m_Clips[m_RightClickedButton]);
}


void CClipBoardPlusDlg::OnMenuSaveall()
{
	CString str;
	for (int i = 0; i < MAXCLIPS; i++)
	{
		str = str + "\r\n---------------------\r\n" + m_Clips[i];
	}
	SaveClips(str);
}

void CClipBoardPlusDlg::SaveClips(CString str)
{
	CString fname = CTime::GetCurrentTime().Format("Clips-%Y%m%d-%H%M%S.txt");

	CFileDialog ResFileOpenDialog(false, _T("txt"), fname, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	//ResFileOpenDialog.m_ofn.lpstrInitialDir = docpath;
	INT_PTR res = ResFileOpenDialog.DoModal();
	if (res == IDOK)
	{
		fname = ResFileOpenDialog.GetPathName();
		CFile txtfile;
		BOOL res = txtfile.Open(fname, CFile::modeCreate | CFile::modeWrite);
		if (!res)
		{
			AfxMessageBox(_T("Error : Failed to create the file"));
			return;
		}
		txtfile.Write(str, str.GetLength()*sizeof(TCHAR));
		txtfile.Close();
	}

}

void CClipBoardPlusDlg::OnMenuHelp()
{
	ShellExecute(NULL, _T("open"), _T("https://oormi.in/software/cbp/help.html"), NULL, NULL, SW_SHOWNORMAL);
}

afx_msg LRESULT CClipBoardPlusDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID;
	UINT uMsg;
	CPoint pt;

	uID = (UINT)wParam;
	uMsg = (UINT)lParam;

	if (uID != 1)
	{
		return 0;
	}

	switch (uMsg)
	{
		case WM_LBUTTONDOWN:
			GetCursorPos(&pt);
			ClientToScreen(&pt);
			OnTrayLButtonDown(pt);
			break;

		case WM_RBUTTONDOWN:

		case WM_CONTEXTMENU:
			GetCursorPos(&pt);
			OnTrayRButtonDown(pt);
			break;

	}

	return 0;
}

void CClipBoardPlusDlg::OnTrayLButtonDown(CPoint pt)
{
	MaximizeFromTray();
}

void CClipBoardPlusDlg::OnTrayRButtonDown(CPoint pt)
{
	m_MenuTray.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, this);
}


void CClipBoardPlusDlg::OnMenuMinimizeto()
{
	MinimizeToTray();
}

void CClipBoardPlusDlg::SetupMinimizeToTray()
{
	m_TrayData.cbSize = sizeof(NOTIFYICONDATA);
	m_TrayData.hWnd = this->m_hWnd;
	m_TrayData.uID = 1;
	m_TrayData.uCallbackMessage = WM_TRAY_MESSAGE;
	m_TrayData.hIcon = this->m_hIcon;

	wcscpy_s(m_TrayData.szTip, _T("Clipboard Plus"));
	m_TrayData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	BOOL bSuccess = m_MenuTray.LoadMenu(IDR_MENU_TRAY);
	if (!(bSuccess))
	{
		MessageBox(_T("Unabled to load menu"), _T("Error"));
	}

	m_bMinimized = FALSE;
}

void CClipBoardPlusDlg::MinimizeToTray()
{
	BOOL bSuccess = Shell_NotifyIcon(NIM_ADD, &m_TrayData);
	if (!(bSuccess))
	{
		MessageBox(_T("Unable to set tray icon"), _T("Error"));
	}

	this->ShowWindow(SW_MINIMIZE);
	this->ShowWindow(SW_HIDE);

	m_bMinimized = TRUE;
}

void CClipBoardPlusDlg::MaximizeFromTray()
{
	this->ShowWindow(SW_SHOW);
	this->ShowWindow(SW_RESTORE);

	Shell_NotifyIcon(NIM_DELETE, &m_TrayData);

	m_bMinimized = FALSE;

	SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); // restore the minimize window
	SetForegroundWindow();
	SetActiveWindow();
	SetWindowPos(0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	RedrawWindow(0, 0, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN); 

}

void CClipBoardPlusDlg::OnTrayRestore()
{
	MaximizeFromTray();
}


void CClipBoardPlusDlg::OnTrayExit()
{
	OnBnClickedOk();
}


void CClipBoardPlusDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	static bool bOnce = true;

	if (bShow && !IsWindowVisible()	&& bOnce)
	{
		bOnce = false;

		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(_T("ClipboardPlus"), _T("WP"), (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
}


void CClipBoardPlusDlg::OnMenuAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CClipBoardPlusDlg::OnMenuCheckforupdates()
{
	m_NetHelper.Checkforupdates(m_CBPVersion, _T("https://oormi.in/software/cbp/updatecbp.txt"),
		_T(" https://github.com/oormicreations/ClipboardPlus"), _T("Clipboard Plus App"));

}

