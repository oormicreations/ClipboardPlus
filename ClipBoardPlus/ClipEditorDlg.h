#pragma once
#include "afxwin.h"
#include "SysHelper.h"

#define MAX_NOTES 100

// CClipEditorDlg dialog

class CClipEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CClipEditorDlg)

public:
	CClipEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClipEditorDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSysHelper m_SysHelper;
	BOOL m_IsStickyNote;
	CEdit m_ClipEd;
	CString m_ClipText;
	int m_NoteCount;
	CString m_Notes[MAX_NOTES];
	CString m_VerStr;

	BOOL ReadStickyNotes();
	BOOL ParseNotes(CString notes);

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAddnote();
};
