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
END_MESSAGE_MAP()


// CClipEditorDlg message handlers


BOOL CClipEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//m_IsStickyNote = FALSE;

	if (m_IsStickyNote)
	{
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
					m_ClipEd.SetWindowText(m_Notes[m_NoteCount-1]);
				}
			}

		}
	}

	return FALSE;
}

BOOL CClipEditorDlg::ParseNotes(CString notes)
{
	return FALSE;
}


void CClipEditorDlg::OnBnClickedAddnote()
{
	CString note;
	m_ClipEd.GetWindowText(note);
	if (!note.IsEmpty())
	{
		m_SysHelper.SaveStringAppend(m_SysHelper.m_FileName, note);
	}
}
