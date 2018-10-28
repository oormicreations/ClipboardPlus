
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
	m_EdDlg = NULL;
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
	ON_COMMAND(ID_MENU_EDIT, &CClipBoardPlusDlg::OnMenuEdit)
	ON_COMMAND(ID_MENU_OPENLINK, &CClipBoardPlusDlg::OnMenuOpenlink)
	ON_COMMAND(ID_MENU_GETMOREFREEAPPS, &CClipBoardPlusDlg::OnMenuGetmorefreeapps)
	ON_COMMAND(ID_MENU_ADD, &CClipBoardPlusDlg::OnMenuAdd)
	ON_COMMAND(ID_MENU_STICKYNOTES, &CClipBoardPlusDlg::OnMenuStickynotes)
	ON_MESSAGE(WM_CBP_RESTORE, RestoreHandler)
	ON_COMMAND(ID_MENU_REMAINONTOP, &CClipBoardPlusDlg::OnMenuRemainontop)
	ON_COMMAND(ID_MENU_REMINDERS, &CClipBoardPlusDlg::OnMenuReminders)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CClipBoardPlusDlg message handlers
LRESULT CClipBoardPlusDlg::RestoreHandler(WPARAM wp, LPARAM lp)
{
	MaximizeFromTray();
	return 0;
}

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

	m_CBPVersionMaj = 1;
	m_CBPVersionMin = 3;

	m_PasswordMode = TRUE;
	m_IsClipBoardPlusEvent = FALSE;
	m_TopmostMode = FALSE;
	m_RightClickedButton = -1;

	m_MenuPopup.LoadMenu(IDR_MENU_CBP);

	SetupMinimizeToTray();
	InitClips();
	GetClip(); //get already present contents

	InitReminders();

	m_NetHelper.ReportUsage(_T("ClipboardPlus"), m_CBPVersionMaj*10 + m_CBPVersionMin);



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

	LoadFromBackup();
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

	SaveToBackup();//persistant loading
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

	KillTimer(m_uRemTimer);

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
	if (m_RightClickedButton >= 0)
	{
		ClearClip(m_RightClickedButton);
		SaveToBackup();
	}
}


void CClipBoardPlusDlg::OnMenuClearall()
{
	for (int i = 0; i < MAXCLIPS; i++)
	{
		ClearClip(i);
	}
	SaveToBackup();
}


void CClipBoardPlusDlg::OnMenuPasswordmode()
{
	m_PasswordMode = !m_PasswordMode;
	m_MenuPopup.CheckMenuItem(ID_MENU_PASSWORDMODE, (MF_CHECKED * (UINT)m_PasswordMode) | MF_BYCOMMAND);

	MaskClips();
}

BOOL CClipBoardPlusDlg::IsPassword(CString clip)
{
	if (clip.IsEmpty()) return FALSE;

	//should not be more than one word
	if (clip.Find(' ') >= 0) return FALSE;

	//should not be too long, like a link etc
	if (clip.GetLength() > MIN_PASSWORD_SZ) return FALSE;

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
		str = str + "\r\n-------------------------------------------------------------------------------\r\n" + m_Clips[i];
	}
	SaveClips(str);
}

void CClipBoardPlusDlg::SaveClips(CString str)
{
	CString fname = CTime::GetCurrentTime().Format("Clips-%Y%m%d-%H%M%S.txt");

	CSysHelper shelp;
	CString idir = shelp.GetUserDocumentPath(CBP_USER_FOLDER);

	CFileDialog ResFileOpenDialog(false, _T("txt"), fname, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	ResFileOpenDialog.m_ofn.lpstrInitialDir = idir;

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
	ShellExecute(NULL, _T("open"), _T("https://oormi.in/software/cbp/help110.html"), NULL, NULL, SW_SHOWNORMAL);
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
	m_NetHelper.Checkforupdates(m_CBPVersionMaj, m_CBPVersionMin,
		_T("https://oormi.in/software/cbp/updatecbp.txt"),
		_T(" https://github.com/oormicreations/ClipboardPlus/releases"), _T("Clipboard Plus App"));

}



void CClipBoardPlusDlg::OnMenuEdit()
{
	if ((m_RightClickedButton >= 0) && (!m_Clips[m_RightClickedButton].IsEmpty()))
	{
		DisplayNotesDlg(FALSE, m_Clips[m_RightClickedButton]);
		SaveToBackup();
	}
}


void CClipBoardPlusDlg::OnMenuOpenlink()
{
	if (m_RightClickedButton >= 0)
	{
		if (m_Clips[m_RightClickedButton].Find(_T("http")) >= 0)
		{
			ShellExecute(NULL, _T("open"), m_Clips[m_RightClickedButton], NULL, NULL, SW_SHOWNORMAL);
		}
	}
}


void CClipBoardPlusDlg::OnMenuGetmorefreeapps()
{
	ShellExecute(NULL, _T("open"), _T("https://github.com/oormicreations?tab=repositories"), NULL, NULL, SW_SHOWNORMAL);
}


void CClipBoardPlusDlg::OnMenuAdd()
{
	if (!m_Clips[m_RightClickedButton].IsEmpty())
	{
		DisplayNotesDlg(TRUE, m_Clips[m_RightClickedButton]);
	}
}


void CClipBoardPlusDlg::OnMenuStickynotes()
{
	DisplayNotesDlg(TRUE, _T(""));
}


void CClipBoardPlusDlg::DisplayNotesDlg(BOOL isnotes, CString add)
{
	if (m_EdDlg == NULL)
	{
		m_EdDlg = new CClipEditorDlg(this);;
		m_EdDlg->m_AddThis = _T("");
		m_EdDlg->m_IsStickyNote = isnotes;
		m_EdDlg->m_VerStr.Format(_T("CBP Sticky Clips Ver %d.%d←"), m_CBPVersionMaj, m_CBPVersionMin);
		m_EdDlg->Create(IDD_DIALOG_EDIT, this);
	}

	m_EdDlg->m_AddThis = _T("");
	m_EdDlg->m_IsStickyNote = isnotes;
	
	if (isnotes)
	{
		m_EdDlg->m_AddThis = add;
		m_EdDlg->ShowNotesButtons(SW_SHOW);
		m_EdDlg->SetWindowText(_T("Sticky Clips"));
		m_EdDlg->ReadStickyNotes();
	}
	else
	{
		m_EdDlg->ShowNotesButtons(SW_HIDE);
		m_EdDlg->SetWindowText(_T("Edit Clip"));
		m_EdDlg->m_ClipEd.SetWindowText(add);
	}


	//if (!m_EdDlg->AnimateWindow(400, AW_SLIDE | AW_VER_POSITIVE))
	//{
		m_EdDlg->ShowWindow(SW_SHOWNORMAL);
	//}

}

void CClipBoardPlusDlg::OnMenuRemainontop()
{
	m_TopmostMode = !m_TopmostMode;
	m_MenuPopup.CheckMenuItem(ID_MENU_REMAINONTOP, (MF_CHECKED * (UINT)m_TopmostMode) | MF_BYCOMMAND);

	if(m_TopmostMode) SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CClipBoardPlusDlg::SaveToBackup()
{
	CString sClips;
	sClips.Format(_T("CBP Ver %d.%d"), m_CBPVersionMaj, m_CBPVersionMin);

	for (int i = 0; i < MAXCLIPS; i++)
	{
		if(!m_Clips[i].IsEmpty()) sClips = sClips + _T("←") + m_Clips[i];
	}

	CSysHelper SysHelper;
	SysHelper.SaveString(SysHelper.GetAppFileName(CBP_BACKUP_FILE), sClips);
}

void CClipBoardPlusDlg::LoadFromBackup()
{
	CSysHelper SysHelper;

	SysHelper.m_FileName = SysHelper.GetAppFileName(CBP_BACKUP_FILE);

	if (!SysHelper.m_FileName.IsEmpty())
	{
		CString sClips = SysHelper.ReadStringFromFile(SysHelper.m_FileName, FALSE);

		if (!sClips.IsEmpty())
		{
			if (ParseClips(sClips))
			{
				for (int i = 0; i < MAXCLIPS; i++)
				{
					m_ClipButton[i].SetText(m_Clips[i]);
				}

				m_ClipButton[0].SetSkin(IDB_BITMAP_CLIP_HOT, IDB_BITMAP_CLIP_HOT, RGB(30, 30, 30));

				MaskClips();
				return;
			}
		}
	}

}

BOOL CClipBoardPlusDlg::ParseClips(CString clips)
{
	int iCount = 0;
	int iStart = 0;
	CString token;
	CString separator = _T("←");
	token = clips.Tokenize(separator, iStart);

	CString sVer;
	sVer.Format(_T("CBP Ver %d.%d←"), m_CBPVersionMaj, m_CBPVersionMin);

	token = token + separator;
	if (token.Compare(sVer))
	{
		//no issues for 1.3, bypass check
		//AfxMessageBox(_T("The backup file has some problems."));
		//return FALSE;
	}

	while (iStart >= 0)
	{
		m_Clips[iCount] = clips.Tokenize(separator, iStart);
		iCount++;
		if (iCount >= MAXCLIPS)break;
	}
	iCount--;

	if (iCount<1)return FALSE;
	return TRUE;
}


void CClipBoardPlusDlg::OnMenuReminders()
{
	m_RemDlg.DoModal();
}

void CClipBoardPlusDlg::InitReminders()
{
	m_RemDlg.m_sVer.Format(_T("CBP Ver %d.%d←"), m_CBPVersionMaj, m_CBPVersionMin);
	m_RemDlg.LoadReminders();
	m_uRemTimer = SetTimer(WM_TIMER + 1001, 30000, NULL);
}

void CClipBoardPlusDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_uRemTimer)
	{
		CTime curTime = CTime::GetCurrentTime();
		for (int t = 0; t < m_RemDlg.m_uRemCount; t++)
		{
			if (m_RemDlg.m_rReminders[t].m_sStatus == _T("Expired")) continue;
			if (m_RemDlg.m_rReminders[t].m_tRemTime == 0) continue;
			if (m_RemDlg.m_rReminders[t].m_tRemTime <= curTime)
			{
				SendMessage(WM_CBP_RESTORE, 0, 0);

				CSysHelper sysHelper;
				CString remalertfile = sysHelper.GetAppFileName(CBP_ALERT_FILE);
				if (!remalertfile.IsEmpty()) PlaySound(remalertfile, NULL, SND_FILENAME);
				MessageBox(m_RemDlg.m_rReminders[t].GetNotificationStr(), _T("ClipBoard Plus Reminder"), MB_ICONINFORMATION);

				m_RemDlg.m_rReminders[t].m_sStatus = _T("Expired");
				m_RemDlg.SaveReminders();
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}
