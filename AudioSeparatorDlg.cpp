// AudioSeparatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MySliderCtrl.h"
#include "AudioSeparator.h"
#include "AudioSeparatorDlg.h"
#include "ProgressWnd.h"
#include "riff.h"
#include "RegisterDlg.h"
#include "md5class.h"
#include "AudioSeparatorFile.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char BEGIN_WITH[] = "Track";
const char DEFAULT_THRESHOLD[] = "0.20 %";
const int SLIDER_RANGE = 300;
const int MAX_MILLISECONDS = 5000;
const int MIN_SILENCELENGTH = 1;
const int DEFAULT_SILENCELENGTH = 500;
const int MAX_TRACKLENGTH = 3600;
const int MIN_TRACKLENGTH = 1;
const int DEFAULT_TRACKLENGTH = 30;
const float MAX_THRESHOLD = 5.0;
static const char MD5_HASH_KEY[] = "j1gab00";
BOOLEAN g_bRegisteredUser = FALSE;

// additional functionality for directory browser

#include <SHLOBJ.H>		// req'd for Browse dialog

/* ============================================================================
 * Function     : DirBrowseCallbackProc
 * Purpose      : Callback procedure for DirBrowseDialog
 *              : 1) Initialise to cwd on startup
 *              : 2) Set status text to the selected directory
 * ==========================================================================*/
static int __stdcall BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
    TCHAR szDir[MAX_PATH] = {NULL};

    if (uMsg == BFFM_INITIALIZED)
    {
    //  Browse dialog initialised - set it to the cwd
        if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir))
            SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
    }
    else if (uMsg == BFFM_SELCHANGED)
    {
     // Fail if non filesystem  ie "//server" selected : BIF_RETURNONLYFSDIRS
        if ( ! SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
            EnableWindow(GetDlgItem(hwnd, IDOK), FALSE); // disable the OK button

     // Set the status window to the currently selected path (or NULL).
        SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
    }

    return 0;
}
/* ============================================================================
 * Function      : DirBrowseDialog
 * Purpose       : Display a directory browser
 * Parameters    : char * strPath : path selected
 * Return        : -1    : memory allocation for shell failed
 *               : TRUE  : path selected
 *                 FALSE : if cancelled or unsuccessful
 * ==========================================================================*/
int BrowseDialog(char *strPath, HWND parentHWND = NULL)
{
    BOOL retStatus = FALSE;
    BROWSEINFO bi;
    LPITEMIDLIST pidl;
    LPMALLOC pMalloc;    // set a pointer to the shell's IMalloc interface

    if ( ! (SUCCEEDED(SHGetMalloc(&pMalloc) ))) // USC : No memory for shell
        return -1;

    // Populate browse info structure
    ZeroMemory(&bi,sizeof(bi));
    bi.hwndOwner = parentHWND;
    bi.pidlRoot  = NULL;
    bi.pszDisplayName = NULL;
    bi.lpszTitle = "Select a directory in which to write split WAV files";
    bi.ulFlags   = BIF_RETURNONLYFSDIRS; // Only "directories"
    bi.ulFlags  |= BIF_STATUSTEXT;       // Show the status window
    bi.lpfn      = BrowseCallbackProc;

    // Display browse dialog
    pidl = SHBrowseForFolder(&bi);

    if (pidl != NULL) // pidl == NULL if cancelled the selection
    {
        // FALSE if "//server" selected - but this is caught in the BrowseCallbackProc
        // FALSE if cancelled
        retStatus = SHGetPathFromIDList(pidl, strPath);

        pMalloc->Free(pidl);    // Free the PIDL returned by SHBrowseForFolder.
    }

    pMalloc->Release();         // Release the shell's allocator.

    return retStatus ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strRegInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strRegInfo = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_REGINFO, m_strRegInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioSeparatorDlg dialog

CAudioSeparatorDlg::CAudioSeparatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioSeparatorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAudioSeparatorDlg)
	m_strThreshText = _T("");
	m_strThreshMaxText = _T("");
	m_strBeginWith = _T("");
	m_strFileToSplit = _T("");
	m_strOutputDir = _T("");
	m_nMinTrackLength = 0;
	m_nSilenceLength = 0;
	m_nAppendSilence = 0;
	m_bTrimSilence = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAudioSeparatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioSeparatorDlg)
	DDX_Control(pDX, IDC_SLIDER_THRESH, m_ctrlSlider);
	DDX_Text(pDX, IDC_STATIC_THRESHTEXT, m_strThreshText);
	DDX_Text(pDX, IDC_STATIC_MAXTHRESHOLD, m_strThreshMaxText);
	DDX_Text(pDX, IDC_EDIT_BEGINWITH, m_strBeginWith);
	DDV_MaxChars(pDX, m_strBeginWith, 100);
	DDX_Text(pDX, IDC_EDIT_FILETOSPLIT, m_strFileToSplit);
	DDV_MaxChars(pDX, m_strFileToSplit, 250);
	DDX_Text(pDX, IDC_EDIT_OUTPUTDIR, m_strOutputDir);
	DDV_MaxChars(pDX, m_strOutputDir, 250);
	DDX_Text(pDX, IDC_EDIT_MINTRACKLENGTH, m_nMinTrackLength);
	DDV_MinMaxUInt(pDX, m_nMinTrackLength, 1, 7200);
	DDX_Text(pDX, IDC_EDIT_SILENCELENGTH, m_nSilenceLength);
	DDV_MinMaxUInt(pDX, m_nSilenceLength, 1, 5000);
	DDX_Text(pDX, IDC_EDIT_APPENDSILENCE, m_nAppendSilence);
	DDV_MinMaxUInt(pDX, m_nAppendSilence, 0, 5000);
	DDX_Check(pDX, IDC_CHECK_TRIMSILENCE, m_bTrimSilence);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAudioSeparatorDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioSeparatorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnButtonBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnButtonBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, OnButtonRegister)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_EN_KILLFOCUS(IDC_EDIT_BEGINWITH, OnKillfocusEditBeginwith)
	ON_EN_KILLFOCUS(IDC_EDIT_MINTRACKLENGTH, OnKillfocusEditMintracklength)
	ON_EN_KILLFOCUS(IDC_EDIT_SILENCELENGTH, OnKillfocusEditSilencelength)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioSeparatorDlg message handlers

BOOL CAudioSeparatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	CWinApp* pApp = AfxGetApp();

	m_bTrimSilence = pApp->GetProfileInt("Options", "TrimSilence", 0);
	m_nUseAverageMethod = pApp->GetProfileInt("Options", "UseAverageMethod", 0);
	m_nAppendSilence = pApp->GetProfileInt("Options", "AppendSilence", 0);
	m_nMinTrackLength = pApp->GetProfileInt("Options", "MinTrackLength", DEFAULT_TRACKLENGTH);
	m_nSilenceLength = pApp->GetProfileInt("Options", "SilenceLength", DEFAULT_SILENCELENGTH);
	m_strBeginWith = pApp->GetProfileString("Options", "BeginWith", BEGIN_WITH);
	m_strThreshMaxText.Format("%.0f %%", MAX_THRESHOLD);
	m_strThreshText = pApp->GetProfileString("Options", "Threshold", DEFAULT_THRESHOLD);
	CheckRegistration();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// initialize threshold slider bar
	CSliderCtrl *pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_THRESH);
	pSliderCtrl->SetRange(0, SLIDER_RANGE);
	pSliderCtrl->SetTicFreq(SLIDER_RANGE / (int)MAX_THRESHOLD);
	float nTemp = atof(m_strThreshText.Left(m_strThreshText.GetLength() - 2));
	pSliderCtrl->SetPos((int)((float)SLIDER_RANGE * (nTemp / MAX_THRESHOLD)));

	// initialize spin controls
	CSpinButtonCtrl *pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_APPENDSILENCE);
	pSpinCtrl->SetRange(0, MAX_MILLISECONDS);
	pSpinCtrl->SetPos(m_nAppendSilence);

	pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SILENCELENGTH);
	pSpinCtrl->SetRange(MIN_SILENCELENGTH, MAX_MILLISECONDS);
	pSpinCtrl->SetPos(m_nSilenceLength);

	pSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MINTRACKLENGTH);
	pSpinCtrl->SetRange(MIN_TRACKLENGTH, MAX_TRACKLENGTH);
	pSpinCtrl->SetPos(m_nMinTrackLength);

	if (m_nUseAverageMethod)
	{
		CButton *pRadio = (CButton*)GetDlgItem(IDC_RADIO1);
		pRadio->SetCheck(1);
	}
	else
	{
		CButton *pRadio = (CButton*)GetDlgItem(IDC_RADIO2);
		pRadio->SetCheck(1);
	}

	if (m_bTrimSilence)
	{
		CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_TRIMSILENCE);
		pCheck->SetCheck(1);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAudioSeparatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAudioSeparatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAudioSeparatorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAudioSeparatorDlg::OnButtonBrowse1()
{
	static char BASED_CODE szFilter[] = "WAV Files (*.wav)|*.wav||";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, GetParent());
	dlg.DoModal();
	m_strFileToSplit = dlg.GetPathName();
	UpdateData(FALSE);
}

void CAudioSeparatorDlg::OnButtonBrowse2()
{
    char strPath[MAX_PATH];

    // Display the Browse folder dialog, if OK change directory
    if ( ::BrowseDialog(strPath, m_hWnd) == TRUE)
		m_strOutputDir = strPath;
	UpdateData(FALSE);
}

void CAudioSeparatorDlg::OnOK()
{
	CAudioSeparatorFile inFile;

	UpdateData();

	double nThreshold = atof(m_strThreshText.Left(m_strThreshText.GetLength() - 2)) / 100;
	if (nThreshold == 0)
	{
		MessageBox("Silence threshold must be greater than zero.", "Error", MB_ICONERROR);
		return;
	}

	// attempt to open WAV file
	AUDSEPRET returnval = inFile.Open(m_strFileToSplit,
	                                  m_nSilenceLength,
	                                  m_nMinTrackLength,
									  nThreshold,
									  (unsigned long)m_nAppendSilence,
									  (m_nUseAverageMethod ? METHOD_AVERAGE_NOISE : METHOD_MAXIMUM_NOISE),
									  m_bTrimSilence,
									  (LPCTSTR)m_strBeginWith,
	                                  this);
	if (returnval == AUD_FILE_OPEN_FAILED)
	{
		CString msg;

		if (!m_strFileToSplit.GetLength())
			msg = "Invalid file name.";
		else
			msg.Format("Failed to open %s "
			           "for reading, either due to a "
					   "system file error or because "
					   "it is in an unrecognized "
					   "format.  Your WAV file must be "
					   "encoded in a sampling rate of "
					   "either 8 or 16 bits in stereo or "
					   "mono mode.", m_strFileToSplit);
		MessageBox(msg, "Error", MB_ICONERROR);
		return;
	}

	if (!SetCurrentDirectory(m_strOutputDir))
	{
		MessageBox("Invalid output directory.", "Error", MB_ICONERROR);
		return;
	}

	if (returnval == AUD_TOO_SHORT_FOR_SPLIT)
	{
		MessageBox("WAV file isn't long enough to split.", "Error", MB_ICONERROR);
		return;
	}

	inFile.AnalyzeAudio();

	return;
	//CDialog::OnOK();
}

void CAudioSeparatorDlg::OnCancel()
{
	CWinApp* pApp = AfxGetApp();

	UpdateData();

	if (m_bTrimSilence)
	{
		pApp->WriteProfileInt("Options", "TrimSilence", 1);
	}
	else
	{
		pApp->WriteProfileInt("Options", "TrimSilence", 0);
	}
	pApp->WriteProfileInt("Options", "UseAverageMethod", m_nUseAverageMethod);
	pApp->WriteProfileInt("Options", "AppendSilence", m_nAppendSilence);
	pApp->WriteProfileInt("Options", "MinTrackLength", m_nMinTrackLength);
	pApp->WriteProfileInt("Options", "SilenceLength", m_nSilenceLength);
	pApp->WriteProfileString("Options", "BeginWith", m_strBeginWith);
	pApp->WriteProfileString("Options", "Threshold", m_strThreshText);

	CDialog::OnCancel();
}

void CAudioSeparatorDlg::OnButtonRegister()
{
	CRegisterDlg dlg;

	int nResponse = dlg.DoModal();
	CheckRegistration();
}

BOOL CAudioSeparatorDlg::CheckRegistration()
{
	CWinApp* pApp = AfxGetApp();
	CString strKey, strName, strTemp;
	strName = pApp->GetProfileString("Options", "Name");
	strKey = pApp->GetProfileString("Options", "Key");

	if (strName.GetLength() > 0 && strKey.GetLength() > 0)
	{
		CButton *pRegisterButton = (CButton*)GetDlgItem(IDC_BUTTON_REGISTER);
		CMD5 crypt;

		strTemp = strName.Left(32) + MD5_HASH_KEY;
		crypt.setPlainText(strTemp);
		if (!strKey.Compare(crypt.getMD5Digest()))
		{
			pRegisterButton->ShowWindow(FALSE);
			g_bRegisteredUser = true;
			return TRUE;
		}
		else
		{
			pRegisterButton->ShowWindow(TRUE);
		}
	}

	g_bRegisteredUser = false;
	return FALSE;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (g_bRegisteredUser)
	{
		CWinApp* pApp = AfxGetApp();
		CString strName = pApp->GetProfileString("Options", "Name");

		m_strRegInfo = "Registered to ";
		m_strRegInfo += strName;
	}
	else
	{
		m_strRegInfo = "Unregistered copy.  Press the \"Register\" "
			           "button for more information.";
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAudioSeparatorDlg::OnRadio1()
{
	CButton *pRadio = (CButton*)GetDlgItem(IDC_RADIO1);
	if (pRadio->GetCheck() == 1)
		m_nUseAverageMethod = 1;
	else
		m_nUseAverageMethod = 0;
}

void CAudioSeparatorDlg::OnRadio2()
{
	CButton *pRadio = (CButton*)GetDlgItem(IDC_RADIO2);
	if (pRadio->GetCheck() == 1)
		m_nUseAverageMethod = 0;
	else
		m_nUseAverageMethod = 1;
}

void CAudioSeparatorDlg::OnKillfocusEditBeginwith()
{
	UpdateData();
}

void CAudioSeparatorDlg::OnKillfocusEditMintracklength()
{
	UpdateData();
}

void CAudioSeparatorDlg::OnKillfocusEditSilencelength()
{
	UpdateData();
}

void CAudioSeparatorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	int test = 0;

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAudioSeparatorDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	int test = 0;

	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CAudioSeparatorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl *pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_THRESH);

	m_strThreshText.Format("%.2f %%", (float)pSliderCtrl->GetPos() / SLIDER_RANGE * MAX_THRESHOLD);
	UpdateData(FALSE);

	return;
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}