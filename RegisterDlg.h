#if !defined(AFX_REGISTERDLG_H__8007A4C4_B072_48EB_A336_2ED28AFCCFC1__INCLUDED_)
#define AFX_REGISTERDLG_H__8007A4C4_B072_48EB_A336_2ED28AFCCFC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterDlg.h : header file
//

#include "AudioSeparatorFile.h"

extern const char MD5_HASH_KEY[];

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog

class CRegisterDlg : public CDialog
{
// Construction
public:
	CRegisterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterDlg)
	enum { IDD = IDD_DIALOG_REGISTER };
	CString	m_strKey;
	CString	m_strName;
	CString	m_strInstructions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDLG_H__8007A4C4_B072_48EB_A336_2ED28AFCCFC1__INCLUDED_)
