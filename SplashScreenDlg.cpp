// SplashScreenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "athero.h"
#include "resource.h"
#include "SplashScreenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashScreenDlg dialog

void CALLBACK EXPORT TimerProc(HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
)
{
	SendMessage (hWnd,WM_CLOSE,0,0);
	KillTimer (hWnd,nIDEvent);
}



CSplashScreenDlg::CSplashScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashScreenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashScreenDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSplashScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashScreenDlg)
	DDX_Control(pDX, IDC_SPLASHBMP, m_Splashbmp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashScreenDlg, CDialog)
	//{{AFX_MSG_MAP(CSplashScreenDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashScreenDlg message handlers

BOOL CSplashScreenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int dlgxsize = 320;
	int dlgysize = 220;

	int xsize = GetSystemMetrics (SM_CXFULLSCREEN);
	int ysize = GetSystemMetrics (SM_CYFULLSCREEN);
	SetWindowPos (NULL, (xsize-dlgxsize)/2, (ysize-dlgysize)/2, dlgxsize, dlgysize, 0);

	m_Splashbmp.SetWindowPos (NULL,0,0,dlgxsize,dlgysize,0);

	//::ShowWindow(AtheroWindow, SW_MAXIMIZE);
	
	// TODO: Add extra initialization here
	SetTimer (100,2000,&TimerProc);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
