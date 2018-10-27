#include "stdafx.h"
#include "Reminder.h"


CReminder::CReminder()
{
	m_bExpired = FALSE;
}


CReminder::~CReminder()
{
}

void CReminder::SetRemDateTime(CString sToken)
{
	int iStart = sToken.Find(_T("|"));
	if (iStart >= 0) m_sRemDesc = sToken.Right(sToken.GetLength() - iStart -1);
	m_tRemTime = _tcstoui64(sToken, NULL, 10);

	CTime curTime = CTime::GetCurrentTime();
	if (m_tRemTime < curTime)m_bExpired = TRUE;

	m_sDispStr = m_tRemTime.Format(_T("%Y-%m-%d | %H:%M | ")) + m_sRemDesc;

}