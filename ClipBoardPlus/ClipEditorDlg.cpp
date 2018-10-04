// ClipEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClipBoardPlus.h"
#include "ClipEditorDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include "mmsystem.h"

// CClipEditorDlg dialog

IMPLEMENT_DYNAMIC(CClipEditorDlg, CDialog)

CClipEditorDlg::CClipEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_EDIT, pParent)
{
	m_BkBrush = 0;
	m_EdFont = NULL;
	m_RemCount = 0;

	for (int i = 0; i < MAX_REMINDERS; i++) m_Timer[i] = 0;
}

CClipEditorDlg::~CClipEditorDlg()
{
}

void CClipEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CLIP, m_ClipEd);
	DDX_Control(pDX, IDC_SLIDER_BROWSE, m_SliderBrowse);
}


BEGIN_MESSAGE_MAP(CClipEditorDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CClipEditorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ADDNOTE, &CClipEditorDlg::OnBnClickedAddnote)
	ON_BN_CLICKED(IDC_PRENOTE, &CClipEditorDlg::OnBnClickedPrenote)
	ON_BN_CLICKED(IDC_NEXTNOTE, &CClipEditorDlg::OnBnClickedNextnote)
	ON_BN_CLICKED(IDC_DELNOTE, &CClipEditorDlg::OnBnClickedDelnote)
	ON_BN_CLICKED(IDCANCEL, &CClipEditorDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_COPYNOTE, &CClipEditorDlg::OnBnClickedCopynote)
	ON_EN_CHANGE(IDC_EDIT_CLIP, &CClipEditorDlg::OnEnChangeEditClip)
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_EDIT_CLIP, &CClipEditorDlg::OnEnKillfocusEditClip)
END_MESSAGE_MAP()


// CClipEditorDlg message handlers


BOOL CClipEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CWnd *pwnd = GetParent();
	CRect wrect;
	pwnd->GetWindowRect(&wrect);
	int ht = wrect.Height();
	wrect.top = wrect.top + ht;
	wrect.bottom = wrect.bottom + ht;
	MoveWindow(wrect);

	//GetDlgItem(IDC_EDIT_NOTECOUNT)->MoveWindow(14, 0, wrect.Width()/2, 14);
	//GetDlgItem(IDC_EDIT_NOTEINFO)->MoveWindow(wrect.Width() / 2, 0, wrect.Width()-14, 14);

	SetNotesFont();
	m_HasChanged = FALSE;

	m_SliderBrowse.SetRange(0, 100);

	SetToolTips();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CClipEditorDlg::ShowNotesButtons(int show)
{
	GetDlgItem(IDC_ADDNOTE)->ShowWindow(show);
	GetDlgItem(IDC_DELNOTE)->ShowWindow(show);
	//GetDlgItem(IDC_PRENOTE)->ShowWindow(show);
	//GetDlgItem(IDC_NEXTNOTE)->ShowWindow(show);
	GetDlgItem(IDC_COPYNOTE)->ShowWindow(show);
	GetDlgItem(IDCANCEL)->ShowWindow(!show);
	m_SliderBrowse.ShowWindow(show);
}

void CClipEditorDlg::SetNotesFont()
{
	CString fontname = _T("Georgia");
	int fontsz = 16;
	if (!m_SysHelper.IsFontInstalled(fontname))
	{
		fontname = _T("Courier New");
		fontsz = 16;
	}

	 m_EdFont = new CFont;
	 m_EdFont->CreateFontW(fontsz, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, fontname);

	m_ClipEd.SetFont(m_EdFont);
}

void CClipEditorDlg::OnBnClickedOk()
{
	if (!m_IsStickyNote)
	{
		m_ClipEd.GetWindowText(m_ClipText);
		m_SysHelper.SetClipboardText(m_ClipText);
	}
	else
	{
		//ShowChangedNotice();
		if (m_HasChanged)
		{
			if (m_NoteCount > 0)
			{
				CString notes = m_VerStr;
				for (int n = 0; n < m_NoteCount; n++)
				{
					if (!m_Notes[n].IsEmpty()) notes = notes + m_Notes[n] + _T("←");
				}

				m_SysHelper.SaveString(m_SysHelper.m_FileName, notes);
			}
		}

	}

	CDialog::OnOK();
}

void CClipEditorDlg::ShowChangedNotice()
{
	if (m_HasChanged)
	{
		int res = AfxMessageBox(_T("Do you wish to save the edited clip?"), MB_YESNO);
		if (res == IDYES)
		{
			OnBnClickedAddnote();
		}
		else m_HasChanged = FALSE;
	}
}

BOOL CClipEditorDlg::ReadStickyNotes()
{
	m_DispNote = 0;
	m_NoteCount = 0;

	m_SysHelper.m_FileName = m_SysHelper.GetAppFileName(CBP_SNOTES_FILE);
	
	if (!m_SysHelper.m_FileName.IsEmpty())
	{
		BOOL res = m_SysHelper.CreateFileAndInit(m_SysHelper.m_FileName, m_VerStr);

		if (res)
		{
			CString notes = m_SysHelper.ReadStringFromFile(m_SysHelper.m_FileName);

			if (!notes.IsEmpty())
			{
				if (ParseNotes(notes))
				{
					m_DispNote = m_NoteCount - 1;
					DisplayNote();
				}
				else
				{
					m_ClipEd.SetWindowText(_T("How does it work?\r\n\r\n\
C : Copy Clip\r\n+ : New Clip\r\n- : Delete note\r\n✓ : Close\r\nSlider: Browse\
\r\nset reminder 11:30 call someone\r\nset reminder 2h 15m go jogging and drop trash\r\nPress add + button to set a reminder as above"));
				}

				m_SliderBrowse.SetRange(1, m_NoteCount);
				m_SliderBrowse.SetPos(m_NoteCount);

				return TRUE;
			}

			if (!m_AddThis.IsEmpty())
			{
				m_ClipEd.SetWindowText(m_AddThis);
				OnBnClickedAddnote();
			}


		}
	}

	return FALSE;
}

BOOL CClipEditorDlg::ParseNotes(CString notes)
{
	m_NoteCount = 0;
	int start = 0;
	CString token;
	CString separator = _T("←");
	token = notes.Tokenize(separator, start);

	token = token + separator;

	//Disable ver check for this ver
	//if (token.Compare(m_VerStr))
	//{
	//	AfxMessageBox(_T("The sticky clips file has some problems."));
	//	return FALSE;
	//}

	while (start >= 0)
	{
		m_Notes[m_NoteCount] = notes.Tokenize(separator, start);
		m_NoteCount++;
		if (m_NoteCount > MAX_NOTES)break;
	}
	m_NoteCount--;

	if(m_NoteCount<1)return FALSE;
	return TRUE;
}


void CClipEditorDlg::OnBnClickedAddnote()
{
	if (m_NoteCount >= MAX_NOTES)
	{
		AfxMessageBox(_T("You have too many notes. Delete some to make room."));
		return;
	}

	m_ClipEd.SetWindowText(_T(""));

	CString note, str;

	CString time = CTime::GetCurrentTime().Format("%Y-%m-%d  %H:%M:%S");

	m_Notes[m_NoteCount] = note + _T("⧖") + time;
	m_NoteCount++;
	m_DispNote = m_NoteCount - 1;

	m_SliderBrowse.SetRange(1, m_NoteCount);
	m_SliderBrowse.SetPos(m_NoteCount);

	DisplayNote();
}


void CClipEditorDlg::OnBnClickedPrenote()
{
	ShowChangedNotice();

	if (m_DispNote > 0)
	{
		m_DispNote--;
		m_ClipEd.SetWindowText(m_Notes[m_DispNote]);
		DisplayNote();
	}
}


void CClipEditorDlg::OnBnClickedNextnote()
{
	ShowChangedNotice();

	if (m_DispNote < m_NoteCount - 1)
	{
		m_DispNote++;
		m_ClipEd.SetWindowText(m_Notes[m_DispNote]);
		DisplayNote();
	}
}


void CClipEditorDlg::OnBnClickedDelnote()
{
	if (m_NoteCount < 1)return;

	m_Notes[m_DispNote] = _T("Deleted");

	CString notes = m_VerStr;
	for (int n = 0; n < m_NoteCount; n++)
	{
		if(m_Notes[n] != _T("Deleted")) notes = notes + m_Notes[n] + _T("←");
	}

	m_SysHelper.SaveString(m_SysHelper.m_FileName, notes);

	for (int n = m_DispNote; n < m_NoteCount-1; n++)
	{
		m_Notes[n] = m_Notes[n+1];
	}

	m_NoteCount--;

	if (m_NoteCount < m_DispNote + 1)
	{
		m_DispNote = m_NoteCount - 1;
	}

	if (m_NoteCount < 1)
	{
		m_NoteCount = 0;
		m_Notes[m_NoteCount] = _T("");
		m_DispNote = 0;
	}

	m_SliderBrowse.SetRange(1, m_NoteCount);

	DisplayNote();
}


void CClipEditorDlg::OnBnClickedCancel()
{
	if (m_IsStickyNote)
	{
		m_ClipEd.SetWindowText(_T(""));
		SetDlgItemText(IDC_EDIT_NOTEINFO, _T(""));
		SetDlgItemText(IDC_EDIT_NOTECOUNT, _T(""));
	}
	else
	{
		CDialog::OnCancel();
	}
}


HBRUSH CClipEditorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	int id = pWnd->GetDlgCtrlID();

	if ((id == IDC_EDIT_CLIP))// || (id == IDC_EDIT_NOTECOUNT) || (id == IDC_EDIT_NOTEINFO))
	{
		COLORREF bkc = RGB(255, 255, 220);
		pDC->SetTextColor(RGB(0, 0, 170));
		pDC->SetBkColor(bkc);

		if (!m_BkBrush) m_BkBrush = CreateSolidBrush(bkc);
		hbr = m_BkBrush;
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CClipEditorDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_BkBrush != NULL)
	{
		DeleteObject(m_BkBrush);
		m_BkBrush = NULL;
	}

	if(m_EdFont!=NULL) delete m_EdFont;
	if (m_ToolTip) delete m_ToolTip;

	//its modeless so delete manually
	delete this;
}


void CClipEditorDlg::DisplayNote()
{
	CString str;
	if (m_NoteCount > 0)
	{
		str.Format(_T("%03d | %03d"), m_DispNote + 1, m_NoteCount);
	}
	SetDlgItemText(IDC_EDIT_NOTECOUNT, str);

	int pos = m_Notes[m_DispNote].Find(_T("⧖"));
	str = m_Notes[m_DispNote].Right(m_Notes[m_DispNote].GetLength() - pos - 1);
	SetDlgItemText(IDC_EDIT_NOTEINFO, str);

	str = m_Notes[m_DispNote].Left(pos);
	SetDlgItemText(IDC_EDIT_CLIP, str);
}



void CClipEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages

	//CRect rect;
	//GetClientRect(&rect);
	//dc.FillSolidRect(rect, RGB(255, 255, 190));
}


void CClipEditorDlg::OnBnClickedCopynote()
{
	CString str;
	GetDlgItemText(IDC_EDIT_CLIP, str);
	m_SysHelper.SetClipboardText(str);
}


void CClipEditorDlg::OnEnChangeEditClip()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_HasChanged = TRUE;
}

void CClipEditorDlg::Process(CString note)
{
	CString str, hr, min;
	BOOL isduration = FALSE;
	int pos = 0;

	////test
	//note = _T("Set reminder 12:52 x:yz");
	//note = _T("Set reminder 2h 12m abch xyzm ");
	pos = note.Find(_T("⧖"));
	note = note.Left(pos);

	note.MakeLower();
	note.Replace(_T("\r\n"), _T(" "));
	note.Replace(_T("  "), _T(" "));
	note.Trim();

	str = _T("set reminder");
	pos = note.Find(str);
	if (pos == 0)
	{
		note.Replace(str, _T(""));

		pos = note.Find(_T(":"));
		if ((pos > 0) && (pos < 5))
		{
			hr = note.Left(pos);
			note.Replace(hr, _T(""));
			hr.Trim();

			pos = note.Find(_T(" "));
			if ((pos > 0) && (pos < 4))
			{
				min = note.Left(pos);
				note.Replace(min, _T(""));
				min.Trim();
				min.Trim(':');
			}

			note.Trim();
		}
		else
		{
			pos = note.Find(_T("h "));
			if ((pos > 0) && (pos < 5))
			{
				hr = note.Left(pos);
				note.Replace(hr + _T("h "), _T(""));
				hr.Trim();
			}

			pos = note.Find(_T("m "));
			if ((pos > 0) && (pos < 4))
			{
				min = note.Left(pos);
				note.Replace(min + _T("m "), _T(""));
				min.Trim();
			}

			isduration = TRUE;
		}

		int h = _ttoi(hr);
		int m = _ttoi(min);
		long duration = 0;

		CTime time = CTime::GetCurrentTime();
		int h1 = time.GetHour();
		int m1 = time.GetMinute();
		int totalmin = h1 * 60 + m1;
		int remmin = h * 60 + m;

		if (isduration) duration = (h * 60 + m);
		else
		{
			if ((h > 23) || (m > 59))
			{
				AfxMessageBox(_T("Hour/min are out of range (23,59)"), MB_ICONERROR);
				return;
			}


			if (remmin > totalmin)
			{
				duration = (remmin - totalmin);
			}
			else
			{
				AfxMessageBox(_T("Error: Reminder is in past."), MB_ICONERROR);
				return;
			}
		}

		if (duration < 1)
		{
			AfxMessageBox(_T("Error: Time duration was not found."), MB_ICONERROR);
			return;
		}
		else
		{
			if (isduration)
			{
				h = h + h1;
				if (h > 23)
				{
					h = h - 24;
				}
				m = m + m1;
				if (m > 59)
				{
					h++;
					m = m - 60;
				}
			}

			int h3 = duration / 60;
			int m3 = duration % 60;

			if (m_RemCount >= MAX_REMINDERS)
			{
				m_RemCount = 0;
			}

			m_RemText[m_RemCount] = note;
			if (m_RemText[m_RemCount].GetLength()>150) m_RemText[m_RemCount].Truncate(150);

			str.Format(_T("Reminder set for\r\n\r\n%s ...\r\n\r\nAt time %02d:%02d. Due in %d hours %d minutes"), m_RemText[m_RemCount], h, m, h3, m3);
		}
		//str.Format(_T("Reminder set for\n%s\non %d:%d\n%d"), note, h, m, duration);

		KillTimer(m_Timer[m_RemCount]);
		m_Timer[m_RemCount] = SetTimer(WM_USER + m_RemCount + 1, duration * 60000, NULL);
		m_RemCount++;

		AfxMessageBox(str, MB_ICONINFORMATION);

	}

	//else m_RemText.Empty();


}

void CClipEditorDlg::OnTimer(UINT_PTR nIDEvent)
{

	if ((nIDEvent < WM_USER + m_RemCount + 1) && (nIDEvent > WM_USER))
	{
		UINT nrem = nIDEvent - WM_USER - 1;
		
		KillTimer(m_Timer[nrem]);
		CString remalertfile = m_SysHelper.GetAppFileName(CBP_ALERT_FILE);
		if (!remalertfile.IsEmpty()) PlaySound(remalertfile, NULL, SND_FILENAME);

		CWnd *parent = GetParent();
		if (parent)	parent->SendMessage(WM_CBP_RESTORE, 0, 0);

		MessageBox(m_RemText[nrem] + _T(" ...\r\n\r\n(Check the sticky clip for more.)"), _T("ClipBoard Plus Reminder"), MB_ICONINFORMATION);
		m_RemText[nrem].Empty();

	}

	CDialog::OnTimer(nIDEvent);
}




void CClipEditorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	if (pScrollBar == (CScrollBar*)&m_SliderBrowse)
	{
		//if ((nSBCode == SB_THUMBPOSITION) || (nSBCode == SB_THUMBTRACK))
		//if (nSBCode != SB_ENDSCROLL)
		//{
			m_DispNote = m_SliderBrowse.GetPos() - 1; //nPos - 1;
			if (m_DispNote < 0) m_DispNote = 0;
			if (m_DispNote >= MAX_NOTES)m_DispNote = MAX_NOTES - 1;
			if (m_DispNote >= m_NoteCount)m_DispNote = m_NoteCount - 1;
			DisplayNote();
		//}
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CClipEditorDlg::SetToolTips()
{
	m_ToolTip = new CToolTipCtrl();
	m_ToolTip->Create(this);

	int buttonids[] = { IDCANCEL, IDC_COPYNOTE, IDC_ADDNOTE, IDC_DELNOTE, IDOK, IDC_SLIDER_BROWSE };
	CString tips[] = { _T("Cancel"), _T("Copy Clip"),  _T("Add New Clip"),  _T("Delete Clip"),  _T("Save and Close"),  _T("Browse") };
	int nbuttons = 6;

	for (int i = 0; i < nbuttons; i++)
	{
		CWnd* pWnd = GetDlgItem(buttonids[i]);
		m_ToolTip->AddTool(pWnd, tips[i]);
	}

	m_ToolTip->Activate(TRUE);
}

BOOL CClipEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_ToolTip != NULL)
	{
		m_ToolTip->RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CClipEditorDlg::OnEnKillfocusEditClip()
{
	if (m_HasChanged)
	{
		CString note;
		m_ClipEd.GetWindowText(note);
		if (!note.IsEmpty())
		{
			CString time = CTime::GetCurrentTime().Format("%Y-%m-%d  %H:%M:%S");
			m_Notes[m_DispNote] = note + _T("⧖") + time;
			Process(m_Notes[m_DispNote]);
		}

	}
}
