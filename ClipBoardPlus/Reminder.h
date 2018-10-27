#pragma once

#include "SysHelper.h"

#define MAX_REMINDERS_NEW 200

class CReminder
{
public:
	CReminder();
	~CReminder();

	CString m_sRemDate;
	CString m_sRemTime;
	CString m_sRemDesc;
	CString m_sDispStr;
	CTime m_tRemTime;
	BOOL m_bExpired;

	void SetRemDateTime(CString sToken);
};

