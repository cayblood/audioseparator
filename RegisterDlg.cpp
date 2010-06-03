// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AudioSeparator.h"
#include "RegisterDlg.h"
#include "md5class.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog


CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterDlg)
	m_strKey = _T("");
	m_strName = _T("");
	m_strInstructions = _T("");
	//}}AFX_DATA_INIT
}


void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDlg)
	DDX_Text(pDX, IDC_EDIT_KEY, m_strKey);
	DDV_MaxChars(pDX, m_strKey, 32);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_INSTRUCTIONS, m_strInstructions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	//{{AFX_MSG_MAP(CRegisterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg message handlers

void CRegisterDlg::OnOK() 
{
	CMD5 crypt;
	CString strTemp;

	UpdateData();

	m_strName.TrimLeft();
	m_strName.TrimRight();
	m_strKey.TrimLeft();
	m_strKey.TrimRight();
	strTemp = m_strName.Left(32);
	strTemp += MD5_HASH_KEY;

	crypt.setPlainText(strTemp);
	if (!m_strKey.Compare(crypt.getMD5Digest()))
	{
		CWinApp* pApp = AfxGetApp();

		UpdateData();

		pApp->WriteProfileString("Options", "Name", m_strName);
		pApp->WriteProfileString("Options", "Key", m_strKey);

		MessageBox("Registration key accepted!  Thank you for registering.", "Success");
	}
	else
	{
		MessageBox("Invalid registration key.  Please try again.", "Error");
		return;
	}
	
	CDialog::OnOK();
}

BOOL CRegisterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_strInstructions.Format(
		"Thank you for your interest in AudioSeparator!  "
		"At present the program will only produce the first "
		"%d segments of a WAV file.  To enable full functionality "
		"and have this limit removed, please register online "
		"for $14.95 at:\n\n\thttp://audioseparator.youngbloods.org"
		"\n\nAfter receiving your registration key, "
		"please enter the following information and click OK:",
		MAX_SPLITS_FOR_UNREGISTERED_USERS);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
