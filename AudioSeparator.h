// AudioSeparator.h : main header file for the AUDIOSEPARATOR application
//

#if !defined(AFX_AUDIOSEPARATOR_H__FFBE90DB_5EFC_410B_9A51_62AFBE87C906__INCLUDED_)
#define AFX_AUDIOSEPARATOR_H__FFBE90DB_5EFC_410B_9A51_62AFBE87C906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAudioSeparatorApp:
// See AudioSeparator.cpp for the implementation of this class
//

class CAudioSeparatorApp : public CWinApp
{
public:
	CAudioSeparatorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioSeparatorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAudioSeparatorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOSEPARATOR_H__FFBE90DB_5EFC_410B_9A51_62AFBE87C906__INCLUDED_)
