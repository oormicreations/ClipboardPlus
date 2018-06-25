// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClipBoardPlus.h"
#include "AboutDlg.h"
#include "afxdialogex.h"


// CAboutDlg dialog

IMPLEMENT_DYNAMIC(CAboutDlg, CDialogEx)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ABOUTBOX, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAboutDlg message handlers
