// ReminderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClipBoardPlus.h"
#include "ReminderDlg.h"
#include "afxdialogex.h"


// CReminderDlg dialog

IMPLEMENT_DYNAMIC(CReminderDlg, CDialogEx)

CReminderDlg::CReminderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_REM, pParent)
{
	m_uRemCount = 0;

}

CReminderDlg::~CReminderDlg()
{
}

void CReminderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dtDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_dtTime);
	DDX_Control(pDX, IDC_LIST_REM, m_lbRemList);
}


BEGIN_MESSAGE_MAP(CReminderDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SETREM, &CReminderDlg::OnBnClickedButtonSetrem)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CReminderDlg::OnDtnDatetimechangeDatetimepicker1)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, &CReminderDlg::OnDtnDatetimechangeDatetimepicker2)
	ON_BN_CLICKED(IDC_BUTTON_DELREM, &CReminderDlg::OnBnClickedButtonDelrem)
	ON_LBN_SELCHANGE(IDC_LIST_REM, &CReminderDlg::OnLbnSelchangeListRem)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CReminderDlg message handlers

BOOL CReminderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_dtDate.SetFormat(_T("yyyy-MM-dd"));
	m_dtTime.SetFormat(_T("HH:mm"));

	CTime start = CTime::GetCurrentTime();
	CTime end = CTime(start.GetYear() + 10, 12, 31, start.GetHour(), start.GetMinute(), 0);
	m_dtDate.SetRange(&start, &end);
	m_dtTime.SetRange(&start, &end);

	SetList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CReminderDlg::OnBnClickedButtonSetrem()
{
	UINT rCount = 0;
	if (m_uRemCount < MAX_REMINDERS_NEW - 1)
	{
		rCount = m_uRemCount;
		m_uRemCount++;
	}
	else //overwrite expired one
	{
		for (int t = 0; t < MAX_REMINDERS_NEW; t++)
		{
			if (m_rReminders[t].m_bExpired)
			{
				rCount = t;
				break;
			}
		}
	}

	if ((rCount < 1) && (m_uRemCount >= MAX_REMINDERS_NEW))
	{
		UINT res = AfxMessageBox(_T("Maximum limit for reminders has been reached. Please free up some space by deleting old reminders.\r\nClick Yes to proceed anyway. No to cancel"), MB_YESNO);
		if (res == IDNO) return;
	}

	//GetDlgItemText(IDC_DATETIMEPICKER1, m_rReminders[rCount].m_sRemDate);
	//GetDlgItemText(IDC_DATETIMEPICKER2, m_rReminders[rCount].m_sRemTime);

	GetDlgItemText(IDC_EDIT_REMTEXT, m_rReminders[rCount].m_sRemDesc);
	if (m_rReminders[rCount].m_sRemDesc.IsEmpty())
	{
		AfxMessageBox(_T("Please enter a description or to do text."));
		return;
	}

	CTime dt1, dt2;
	m_dtDate.GetTime(dt1);
	m_dtTime.GetTime(dt2);

	m_rReminders[rCount].m_tRemTime = CTime(dt1.GetYear(), dt1.GetMonth(), dt1.GetDay(), dt2.GetHour(), dt2.GetMinute(), 0);
	m_rReminders[rCount].m_sDispStr = m_rReminders[rCount].m_tRemTime.Format(_T("%Y-%m-%d | %H:%M | ")) + m_rReminders[rCount].m_sRemDesc;

	m_lbRemList.AddString(m_rReminders[rCount].m_sDispStr);
}


void CReminderDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	//GetDlgItemText(IDC_DATETIMEPICKER1, m_sDate);
	*pResult = 0;
}


void CReminderDlg::OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	//GetDlgItemText(IDC_DATETIMEPICKER2, m_sTime);
	*pResult = 0;
}


void CReminderDlg::OnBnClickedButtonDelrem()
{
	int iID = m_lbRemList.GetCurSel();
	m_lbRemList.DeleteString(iID);
}


void CReminderDlg::OnLbnSelchangeListRem()
{
	CString str;
	int iID = m_lbRemList.GetCurSel();
	SetDlgItemText(IDC_EDIT_REMTEXT, m_rReminders[iID].m_sRemDesc);

	m_dtDate.SetTime(&m_rReminders[iID].m_tRemTime);
	m_dtTime.SetTime(&m_rReminders[iID].m_tRemTime);
}

void CReminderDlg::SetList()
{
	for (int i = 0; i < m_uRemCount; i++)
	{
		m_lbRemList.AddString(m_rReminders[i].m_sDispStr);
	}
}

BOOL CReminderDlg::LoadReminders()
{
	CSysHelper SysHelper;
	SysHelper.m_FileName = SysHelper.GetAppFileName(CBP_REM_FILE);

	if (!SysHelper.m_FileName.IsEmpty())
	{
		CString sRems = SysHelper.ReadStringFromFile(SysHelper.m_FileName, FALSE);

		if (!sRems.IsEmpty())
		{
			return ParseRems(sRems);
		}
	}
	return 0;
}

BOOL CReminderDlg::ParseRems(CString sRems)
{
	int iCount = 0;
	int iStart = 0;
	CString token;
	CString separator = _T("←");
	token = sRems.Tokenize(separator, iStart);

	token = token + separator;
	if (token.Compare(m_sVer))
	{
		AfxMessageBox(_T("The reminders file has some problems."));
		return FALSE;
	}

	while (iStart >= 0)
	{
		token = sRems.Tokenize(separator, iStart);
		token.Trim();
		if (!token.IsEmpty())
		{
			m_rReminders[iCount].SetRemDateTime(token);
			iCount++;
		}

		if (iCount >= MAX_REMINDERS_NEW)break;
	}
	//iCount--;
	m_uRemCount = iCount;

	if (iCount<1)return FALSE;
	return TRUE;
}

BOOL CReminderDlg::SaveReminders()
{
	CString str;
	CString sRems = m_sVer;
	int iCount = m_lbRemList.GetCount();
	if (iCount >= MAX_REMINDERS_NEW) iCount = MAX_REMINDERS_NEW;

	for (int i = 0; i < iCount; i++)
	{
		LONGLONG t = m_rReminders[i].m_tRemTime.GetTime();
		str.Format(_T("%I64d|%s"), t, m_rReminders[i].m_sRemDesc);
		sRems = sRems + _T("\r\n") + str + _T("←");
	}

	CSysHelper SysHelper;
	return SysHelper.SaveString(SysHelper.GetAppFileName(CBP_REM_FILE), sRems);
}

void CReminderDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	SaveReminders();
}
