// AudioSeparatorDlg.h : header file
//

#if !defined(AFX_AUDIOSEPARATORDLG_H__F6E0D3AD_AD51_435A_8BDF_F8D2757846A3__INCLUDED_)
#define AFX_AUDIOSEPARATORDLG_H__F6E0D3AD_AD51_435A_8BDF_F8D2757846A3__INCLUDED_

#include "MySliderCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAudioSeparatorDlg dialog

class CAudioSeparatorDlg : public CDialog
{
// Construction
public:
	UINT m_nUseAverageMethod;
	CAudioSeparatorDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL CheckRegistration();

// Dialog Data
	//{{AFX_DATA(CAudioSeparatorDlg)
	enum { IDD = IDD_AUDIOSEPARATOR_DIALOG };
	CMySliderCtrl	m_ctrlSlider;
	CString	m_strThreshText;
	CString	m_strThreshMaxText;
	CString	m_strBeginWith;
	CString	m_strFileToSplit;
	CString	m_strOutputDir;
	UINT	m_nMinTrackLength;
	UINT	m_nSilenceLength;
	UINT	m_nAppendSilence;
	BOOL	m_bTrimSilence;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioSeparatorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAudioSeparatorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonBrowse1();
	afx_msg void OnButtonBrowse2();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonRegister();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnKillfocusEditBeginwith();
	afx_msg void OnKillfocusEditMintracklength();
	afx_msg void OnKillfocusEditSilencelength();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOSEPARATORDLG_H__F6E0D3AD_AD51_435A_8BDF_F8D2757846A3__INCLUDED_)
