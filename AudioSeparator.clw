; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAudioSeparatorDlg
LastTemplate=CSliderCtrl
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "AudioSeparator.h"

ClassCount=5
Class1=CAudioSeparatorApp
Class2=CAudioSeparatorDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_AUDIOSEPARATOR_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Class4=CRegisterDlg
Class5=CMySliderCtrl
Resource4=IDD_DIALOG_REGISTER

[CLS:CAudioSeparatorApp]
Type=0
HeaderFile=AudioSeparator.h
ImplementationFile=AudioSeparator.cpp
Filter=N
LastObject=CAudioSeparatorApp

[CLS:CAudioSeparatorDlg]
Type=0
HeaderFile=AudioSeparatorDlg.h
ImplementationFile=AudioSeparatorDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CAudioSeparatorDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=AudioSeparatorDlg.h
ImplementationFile=AudioSeparatorDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_STATIC_REGINFO

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC_REGINFO,static,1342308352

[DLG:IDD_AUDIOSEPARATOR_DIALOG]
Type=1
Class=CAudioSeparatorDlg
ControlCount=32
Control1=IDC_STATIC,static,1342308354
Control2=IDC_EDIT_FILETOSPLIT,edit,1350631552
Control3=IDC_BUTTON_BROWSE1,button,1342242816
Control4=IDC_STATIC,static,1342308354
Control5=IDC_EDIT_OUTPUTDIR,edit,1350631552
Control6=IDC_BUTTON_BROWSE2,button,1342242816
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,static,1342308352
Control11=IDC_SLIDER_THRESH,msctls_trackbar32,1342242817
Control12=IDC_STATIC_THRESHTEXT,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC_MAXTHRESHOLD,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_SILENCELENGTH,edit,1350639744
Control17=IDC_SPIN_SILENCELENGTH,msctls_updown32,1342177462
Control18=IDC_STATIC,static,1342308352
Control19=IDC_RADIO1,button,1342373897
Control20=IDC_RADIO2,button,1342242825
Control21=IDC_STATIC,static,1342308352
Control22=IDC_EDIT_MINTRACKLENGTH,edit,1350639744
Control23=IDC_SPIN_MINTRACKLENGTH,msctls_updown32,1342177462
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_EDIT_BEGINWITH,edit,1350631552
Control27=IDC_STATIC,static,1342308352
Control28=IDC_EDIT_APPENDSILENCE,edit,1350639744
Control29=IDC_SPIN_APPENDSILENCE,msctls_updown32,1342177462
Control30=IDC_STATIC,static,1342308352
Control31=IDC_CHECK_TRIMSILENCE,button,1342242819
Control32=IDC_BUTTON_REGISTER,button,1342242816

[DLG:IDD_DIALOG_REGISTER]
Type=1
Class=CRegisterDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC_INSTRUCTIONS,static,1342308352
Control4=IDC_EDIT_NAME,edit,1350631552
Control5=IDC_EDIT_KEY,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352

[CLS:CRegisterDlg]
Type=0
HeaderFile=RegisterDlg.h
ImplementationFile=RegisterDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_EDIT_KEY
VirtualFilter=dWC

[CLS:CMySliderCtrl]
Type=0
HeaderFile=MySliderCtrl.h
ImplementationFile=MySliderCtrl.cpp
BaseClass=CSliderCtrl
Filter=W
VirtualFilter=KWC

