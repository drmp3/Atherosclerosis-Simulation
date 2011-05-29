#if !defined(AFX_SPLASHSCREENDLG_H__02119061_9137_11D2_AE30_0020182F1487__INCLUDED_)
#define AFX_SPLASHSCREENDLG_H__02119061_9137_11D2_AE30_0020182F1487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashScreenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashScreenDlg dialog

class CSplashScreenDlg : public CDialog
{
// Construction
public:
	CSplashScreenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSplashScreenDlg)
	enum { IDD = IDD_STARTUPDLG };
	CStatic	m_Splashbmp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashScreenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSplashScreenDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHSCREENDLG_H__02119061_9137_11D2_AE30_0020182F1487__INCLUDED_)
