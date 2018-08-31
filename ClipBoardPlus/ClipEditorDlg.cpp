// ClipEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClipBoardPlus.h"
#include "ClipEditorDlg.h"
#include "afxdialogex.h"


// CClipEditorDlg dialog

IMPLEMENT_DYNAMIC(CClipEditorDlg, CDialog)

CClipEditorDlg::CClipEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_EDIT, pParent)
{
	m_BkBrush = 0;
}

CClipEditorDlg::~CClipEditorDlg()
{
}

void CClipEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CLIP, m_ClipEd);
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
END_MESSAGE_MAP()


// CClipEditorDlg message handlers


BOOL CClipEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CWnd *pwnd = GetParent();
	CRect wrect;
	pwnd->GetWindowRect(&wrect);
	MoveWindow(wrect);

	SetNotesFont();

	if (m_IsStickyNote)
	{
		m_DispNote = 0;
		m_NoteCount = 0;

		GetDlgItem(IDC_ADDNOTE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DELNOTE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_PRENOTE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_NEXTNOTE)->ShowWindow(SW_SHOW);	

		this->SetWindowText(_T("Sticky Notes"));

		ReadStickyNotes();

	}
	else
	{
		m_ClipEd.SetWindowText(m_ClipText);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CClipEditorDlg::SetNotesFont()
{
	CString fontname = _T("Segoe Print");
	int fontsz = 24;
	if (!m_SysHelper.IsFontInstalled(fontname))
	{
		fontname = _T("Courier New");
		fontsz = 16;
	}

	CFont *font = new CFont;
	font->CreateFontW(fontsz, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, fontname);

	m_ClipEd.SetFont(font);
}

void CClipEditorDlg::OnBnClickedOk()
{
	m_ClipEd.GetWindowText(m_ClipText);

	CDialog::OnOK();
}


BOOL CClipEditorDlg::ReadStickyNotes()
{
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
					m_ClipEd.SetWindowText(m_Notes[m_DispNote]);
				}
				else
				{
					m_ClipEd.SetWindowText(_T("How does it work?\r\n✕ : Clear\r\n+ : Save note\r\n← : Next note\r\n→ : Previous note\r\n- : Delete note\r\n✓ : Close"));
				}
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
	if (token.Compare(m_VerStr))
	{
		AfxMessageBox(_T("The sticky notes file has some problems."));
		return FALSE;
	}

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

	CString note;
	m_ClipEd.GetWindowText(note);

	if (note == m_Notes[m_DispNote])
	{
		AfxMessageBox(_T("Its the same note, change it to add!"));
		return;
	}

	if (!note.IsEmpty())
	{
		CString time = CTime::GetCurrentTime().Format("%Y-%m-%d  %H:%M:%S");

		m_Notes[m_NoteCount] = note;
		m_SysHelper.SaveStringAppend(m_SysHelper.m_FileName, m_Notes[m_NoteCount] + _T("@@") + time + _T("←"));
		m_NoteCount++;
		m_DispNote = m_NoteCount-1;

		SetDlgItemText(IDC_EDIT_NOTEINFO, time);
		DisplayCount();
	}
}


void CClipEditorDlg::OnBnClickedPrenote()
{
	if (m_DispNote > 0)
	{
		m_DispNote--;
		m_ClipEd.SetWindowText(m_Notes[m_DispNote]);
	}
	DisplayCount();
}


void CClipEditorDlg::OnBnClickedNextnote()
{
	if (m_DispNote < m_NoteCount - 1)
	{
		m_DispNote++;
		m_ClipEd.SetWindowText(m_Notes[m_DispNote]);
		DisplayCount();
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

	m_ClipEd.SetWindowText(m_Notes[m_DispNote]);
	DisplayCount();
}


void CClipEditorDlg::OnBnClickedCancel()
{
	if (m_IsStickyNote)
	{
		m_ClipEd.SetWindowText(_T(""));
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

	if (id == IDC_EDIT_CLIP)
	{
		COLORREF bkc = RGB(255, 255, 190);
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
}

void CClipEditorDlg::DisplayCount()
{
	CString str;
	str.Format(_T("%03d | %03d"), m_DispNote+1, m_NoteCount);
	SetDlgItemText(IDC_EDIT_NOTECOUNT, str);
}
