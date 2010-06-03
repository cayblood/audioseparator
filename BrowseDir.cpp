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
int BrowseDialog(char *strPath)
{
    BOOL retStatus = FALSE;
    BROWSEINFO bi;
    LPITEMIDLIST pidl;
    LPMALLOC pMalloc;    // set a pointer to the shell's IMalloc interface

    if ( ! (SUCCEEDED(SHGetMalloc(&pMalloc) ))) // USC : No memory for shell
        return -1;

    // Populate browse info structure
    ZeroMemory(&bi,sizeof(bi));
    bi.hwndOwner = NULL;
    bi.pidlRoot  = NULL;
    bi.pszDisplayName = NULL;
    bi.lpszTitle = "Select a new working directory";
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


I invoke the BrowseDialog from CMainFrame to change the cwd, however do with it what you wish...

void CMainFrame::OnFileSetCwd()
{
    char strPath[MAX_PATH];

    // Display the Browse folder dialog, if OK change directory
    if ( ::BrowseDialog(strPath) == TRUE)
        _chdir(strPath);
}
