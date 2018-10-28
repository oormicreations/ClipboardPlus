#pragma once

#include "SysHelper.h"

#define MAX_REMINDERS_NEW 200

class CReminder
{
public:
	CReminder();
	~CReminder();

	CString m_sStatus;
	CString m_sRemDesc;
	CString m_sDispStr;
	CTime m_tRemTime;

	void SetRemDateTime(CString sToken);
	CString GetNotificationStr();

};

                                                                                                                                                                                                                                                                                                                                                       