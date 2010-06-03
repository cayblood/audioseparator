// MySliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AudioSeparator.h"
#include "MySliderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySliderCtrl

CMySliderCtrl::CMySliderCtrl()
{
}

CMySliderCtrl::~CMySliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CMySliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CMySliderCtrl)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMySliderCtrl message handlers

void CMySliderCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CWnd *pParent = (CWnd*)pScrollBar->GetParentOwner();
	pParent->PostMessage(WM_HSCROLL);	
	
	CSliderCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}
