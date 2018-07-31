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
END_MESSAGE_MAP()


// CClipEditorDlg message handlers


BOOL CClipEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_ClipEd.SetWindowText(m_ClipText);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CClipEditorDlg::OnBnClickedOk()
{
	m_ClipEd.GetWindowText(m_ClipText);

	CDialog::OnOK();
}
