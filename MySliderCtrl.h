#if !defined(AFX_MYSLIDERCTRL_H__13139E74_7DA3_4935_821E_F7AE1B80DB37__INCLUDED_)
#define AFX_MYSLIDERCTRL_H__13139E74_7DA3_4935_821E_F7AE1B80DB37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MySliderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMySliderCtrl window

class CMySliderCtrl : public CSliderCtrl
{
// Construction
public:
	CMySliderCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySliderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMySliderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMySliderCtrl)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSLIDERCTRL_H__13139E74_7DA3_4935_821E_F7AE1B80DB37__INCLUDED_)
