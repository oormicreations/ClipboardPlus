#include "stdafx.h"
#include "Reminder.h"


CReminder::CReminder()
{
}


CReminder::~CReminder()
{
}

void CReminder::SetRemDateTime(CString sToken)
{
	int iStart = 0, iCount = 0;
	CString token;
	CString separator = _T("|");
	CTime curTime = CTime::GetCurrentTime();

	while (iStart >= 0)
	{
		token = sToken.Tokenize(separator, iStart);
		token.Trim();
		if (!token.IsEmpty())
		{
			if (iCount == 0) m_tRemTime = _tcstoui64(token, NULL, 10);
			if (iCount == 1)
			{
				m_sStatus = token;
				if (m_tRemTime <= curTime)
				{
					if (m_sStatus == _T("Active"))
					{
						m_sStatus = _T("Missed");
					}
				}

			}
			if (iCount == 2) m_sRemDesc = token;
			iCount++;
		}

		if (iCount >= 3)break;
	}

	m_sDispStr = m_tRemTime.Format(_T("%Y-%m-%d | %H:%M | ")) + m_sStatus + m_sRemDesc;

}

CString CReminder::GetNotificationStr()
{
	return (m_tRemTime.Format(_T("%Y-%m-%d | %H:%M | ")) + m_sStatus + _T(" |\r\n\r\n") + m_sRemDesc);
}