#pragma once
#include "afxdtctl.h"
#include "Reminder.h"



// CReminderDlg dialog

class CReminderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReminderDlg)

public:
	CReminderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReminderDlg();

	CString m_sVer;
	//CString m_sReminderDescription;
	//CString m_sDate;
	//CString m_sTime;

	CDateTimeCtrl m_dtDate;
	CDateTimeCtrl m_dtTime;
	CListBox m_lbRemList;

	BOOL LoadReminders();
	BOOL SaveReminders();
	BOOL ParseRems(CString sRems);
	void SetList();

	CReminder m_rReminders[MAX_REMINDERS_NEW];
	UINT m_uRemCount;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetrem();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDelrem();
	afx_msg void OnLbnSelchangeListRem();
	afx_msg void OnDestroy();
};
