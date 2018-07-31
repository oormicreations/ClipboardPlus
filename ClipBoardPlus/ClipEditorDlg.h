#pragma once
#include "afxwin.h"


// CClipEditorDlg dialog

class CClipEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CClipEditorDlg)

public:
	CClipEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClipEditorDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ClipEd;
	CString m_ClipText;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
