
// ClipBoardPlusDlg.h : header file
//

#pragma once
#include "ClipButton.h"
#include "NetHelper.h"
#include "ClipEditorDlg.h"
#include "SysHelper.h"
#include "ReminderDlg.h"
#include "mmsystem.h"


#define MAXCLIPS 5
#define WM_TRAY_MESSAGE (WM_USER + 1)
#define MIN_PASSWORD_SZ 16

// CClipBoardPlusDlg dialog
class CClipBoardPlusDlg : public CDialogEx
{
// Construction
public:
	CClipBoardPlusDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	CString m_Clips[MAXCLIPS];
	CClipButton m_ClipButton[MAXCLIPS];
	BOOL m_PasswordMode, m_TopmostMode;
	BOOL m_IsClipBoardPlusEvent;
	int m_RightClickedButton;
	CMenu m_MenuPopup, m_MenuTray;
	NOTIFYICONDATA m_TrayData;
	BOOL m_bMinimized;
	int m_CBPVersionMaj, m_CBPVersionMin;
	CNetHelper m_NetHelper;
	CClipEditorDlg *m_EdDlg;
	CReminderDlg m_RemDlg;
	UINT m_uRemTimer;


	void InitClips();
	void GetClip();
	CString GetClipboardText();
	BOOL SetClipboardText(CString text);
	void ScrollClips(int nclip);
	void ClearClip(int nclip);
	BOOL IsPassword(CString clip);
	void MaskClips();
	void SaveClips(CString str);
	void OnTrayLButtonDown(CPoint pt);
	void OnTrayRButtonDown(CPoint pt);
	void SetupMinimizeToTray();
	void MinimizeToTray();
	void MaximizeFromTray();
	void DisplayNotesDlg(BOOL isnotes, CString add);
	
	void SaveToBackup();
	void LoadFromBackup();
	BOOL ParseClips(CString notes);

	void InitReminders();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIPBOARDPLUS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClipboardUpdate();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnMenuExit();
	afx_msg void OnMenuClear();
	afx_msg void OnMenuClearall();
	afx_msg void OnMenuPasswordmode();
	afx_msg void OnMenuClearclipboard();
	afx_msg void OnMenuSave();
	afx_msg void OnMenuSaveall();
	afx_msg void OnMenuHelp();
	afx_msg void OnMenuMinimizeto();
	afx_msg void OnTrayRestore();
	afx_msg void OnTrayExit();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMenuCheckforupdates();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuEdit();
	afx_msg void OnMenuOpenlink();
	afx_msg void OnMenuGetmorefreeapps();
	afx_msg void OnMenuAdd();
	afx_msg void OnMenuStickynotes();
	afx_msg LRESULT RestoreHandler(WPARAM wp, LPARAM lp);

	afx_msg void OnMenuRemainontop();
	afx_msg void OnMenuReminders();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSaveDeleteautosavedclips();
};
