#pragma once
#include "afxwin.h"
class CClipButton :
	public CButton
{
public:
	CClipButton();
	~CClipButton();

	CBitmap m_NormalBitmapDC;
	CBitmap m_DisabledBitmapDC;
	COLORREF m_TextColor;
	CString m_BtnText;
	BOOL m_IsHot;

	void SetSkin(UINT uiNormal, UINT uiDisabled, COLORREF clrTextColor);
	void SetText(CString btext);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};

