#if !defined(AFX_ENTERVALUEFORX0DLG_H__71EF3743_8221_11D2_AE30_0020182F1487__INCLUDED_)
#define AFX_ENTERVALUEFORX0DLG_H__71EF3743_8221_11D2_AE30_0020182F1487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnterValueForx0Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnterValueForx0Dlg dialog

class CEnterValueForx0Dlg : public CDialog
{
// Construction
public:
	FUNCTIONADJUSTERINFO* lpFunctionAdjusterInfo;
	CEnterValueForx0Dlg(CWnd* pParent = NULL, FUNCTIONADJUSTERINFO * pFuncAdjInfo=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnterValueForx0Dlg)
	enum { IDD = IDD_ENTERNUMDLG };
	BOOL	m_UsingThisValue;
	float	m_Value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterValueForx0Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnterValueForx0Dlg)
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTERVALUEFORX0DLG_H__71EF3743_8221_11D2_AE30_0020182F1487__INCLUDED_)
