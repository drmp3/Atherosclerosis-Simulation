// AtheroApp.cpp : implementation file
//

#include "stdafx.h"
#include "athero.h"
#include "resource.h"
#include "MainWindow.h"
#include "AtheroApp.h"
#include "SimulationInfo.h"
#include "SplashScreenDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AtheroApp


BEGIN_MESSAGE_MAP(AtheroApp, CWinApp)
	//{{AFX_MSG_MAP(AtheroApp)
	ON_COMMAND(CM_INFO, OnInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



AtheroApp::AtheroApp()
{
}

AtheroApp theApp;

AtheroApp::~AtheroApp()
{
}


BOOL AtheroApp::InitInstance()
{
		// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	Enable3dControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	CMainWindow dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int AtheroApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// AtheroApp message handlers

void AtheroApp::OnInfo() 
{
	// TODO: Add your command handler code here
	SimulationInfoDialog.ShowWindow (SW_SHOWNOACTIVATE);
}


