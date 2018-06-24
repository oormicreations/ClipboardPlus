#include "stdafx.h"
#include "ClipButton.h"
#include "resource.h"		// main symbols


CClipButton::CClipButton()
{
	m_IsHot = FALSE;
}


CClipButton::~CClipButton()
{
}

BEGIN_MESSAGE_MAP(CClipButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


BOOL CClipButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CButton::OnEraseBkgnd(pDC);
}


void CClipButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	this->Invalidate();
}


void CClipButton::SetSkin(UINT uiNormal, UINT uiDisabled, COLORREF clrTextColor)
{
	//TRACE(TEXT("CGuiButton::SetSkin\n"));
	// --------------------------------------
	// Free previous allocated bitmaps
	// --------------------------------------
	m_NormalBitmapDC.DeleteObject();
	m_DisabledBitmapDC.DeleteObject();

	// --------------------------------------
	// Load bitmaps corresponding to states
	// --------------------------------------
	if (uiNormal>0)
		m_NormalBitmapDC.LoadBitmap(uiNormal);
	if (uiDisabled>0)
		m_DisabledBitmapDC.LoadBitmap(uiDisabled);

	m_TextColor = clrTextColor;
}

void CClipButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	ASSERT(lpDrawItemStruct);

	CDC btnDC;
	btnDC.Attach(lpDrawItemStruct->hDC);	// get device context

	//CBitmap* pbitmapDC = NULL;
	BITMAP bmpStruct;

	CRect btnRect;
	GetClientRect(&btnRect);

	// --------------------------------------
	// Draw the bitmap
	// --------------------------------------

	// Get the correct Bitmap
	//if (!IsWindowEnabled())
	//	pbitmapDC = &m_DisabledBitmapDC;
	//else
	//	pbitmapDC = &m_NormalBitmapDC;

	// Select the bitmap into a Compatible DC
	CDC *bmpDC = new CDC();
	bmpDC->CreateCompatibleDC(&btnDC);
	bmpDC->SelectObject(m_NormalBitmapDC);
//	bmpDC->SelectObject(pbitmapDC);

//	if (pbitmapDC != NULL)
	{
//		pbitmapDC->GetBitmap(&bmpStruct);
		m_NormalBitmapDC.GetBitmap(&bmpStruct);
		const int padding = 0;
		btnDC.StretchBlt(padding, padding, btnRect.Width() - (2 * padding), btnRect.Height() - (2 * padding), bmpDC, 0, 0, bmpStruct.bmWidth, bmpStruct.bmHeight, SRCCOPY);
	}

	// --------------------------------------
	// Draw the Text
	// --------------------------------------

	CRect textRect;
	textRect = lpDrawItemStruct->rcItem;
	textRect.left = textRect.left + 10;

	int       oldMode = btnDC.SetBkMode(TRANSPARENT);
	COLORREF oldColor = btnDC.SetTextColor(m_TextColor);

	CSize sz;
	sz = btnDC.GetTextExtent(m_BtnText);

	BOOL bNoOfLines = FALSE;
	UINT uiDrawTextFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;

	int iOffset = btnDC.DrawText(m_BtnText, textRect, uiDrawTextFormat);

	btnDC.DeleteDC();
	bmpDC->DeleteDC();
	//oldbm->DeleteObject();
	//pbitmapDC->DeleteObject();
	delete bmpDC;

}


void CClipButton::OnDestroy()
{
	CButton::OnDestroy();

	m_NormalBitmapDC.DeleteObject();
	m_DisabledBitmapDC.DeleteObject();
}

void CClipButton::SetText(CString btext)
{
	btext.Replace(_T("\r\n"), _T(" "));

	int iLength = btext.GetLength();
	if (iLength > 45)
	{
		btext.Truncate(45);
		btext = btext + _T(" ...");
	}

	m_BtnText = btext;
	this->Invalidate();

}

void CClipButton::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetSkin(IDB_BITMAP_CLIP_HOT, IDB_BITMAP_CLIP_HOT, RGB(30, 30, 30));
	Invalidate();

	CButton::OnMouseHover(nFlags, point);
}


void CClipButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_IsHot)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 100;
		tme.hwndTrack = this->GetSafeHwnd();
		TrackMouseEvent(&tme);
		m_IsHot = TRUE;
		//TRACE("track\n");
	}
	CButton::OnMouseMove(nFlags, point);
}


void CClipButton::OnMouseLeave()
{
	if (m_IsHot)
	{
		SetSkin(IDB_BITMAP_CLIP_COLD, IDB_BITMAP_CLIP_COLD, RGB(60, 60, 60));
		Invalidate();
		m_IsHot = FALSE;
	}

	CButton::OnMouseLeave();
}
