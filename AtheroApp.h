#if !defined(AFX_ATHEROAPP_H__FB128C04_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
#define AFX_ATHEROAPP_H__FB128C04_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AtheroApp.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// AtheroApp thread

class AtheroApp : public CWinApp
{
public:
	AtheroApp();           
	virtual ~AtheroApp();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AtheroApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	

	// Generated message map functions
	//{{AFX_MSG(AtheroApp)
	afx_msg void OnInfo();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

extern AtheroApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATHEROAPP_H__FB128C04_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
