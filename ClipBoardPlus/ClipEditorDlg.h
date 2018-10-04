#pragma once
#include "afxwin.h"
#include "SysHelper.h"
#include "afxcmn.h"

#define MAX_NOTES 300
#define MAX_REMINDERS 20

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
	int m_NoteCount, m_DispNote;
	CString m_Notes[MAX_NOTES];
	CString m_VerStr;
	HBRUSH m_BkBrush;
	CString m_AddThis;
	CFont *m_EdFont;
	BOOL m_HasChanged;
	UINT m_Timer[MAX_REMINDERS];
	UINT m_RemCount;
	CString m_RemText[MAX_REMINDERS];
	CSliderCtrl m_SliderBrowse;
	CToolTipCtrl* m_ToolTip;


	BOOL ReadStickyNotes();
	BOOL ParseNotes(CString notes);
	void SetNotesFont();
	void DisplayNote();
	void ShowNotesButtons(int show);
	void ShowChangedNotice();
	void Process(CString note);
	void SetToolTips();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAddnote();
	afx_msg void OnBnClickedPrenote();
	afx_msg void OnBnClickedNextnote();
	afx_msg void OnBnClickedDelnote();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCopynote();
	afx_msg void OnEnChangeEditClip();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnKillfocusEditClip();
};
