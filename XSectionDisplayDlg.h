#if !defined(AFX_XSECTIONDISPLAYDLG_H__8DC048E6_A4C4_11D2_AE30_0020182F1487__INCLUDED_)
#define AFX_XSECTIONDISPLAYDLG_H__8DC048E6_A4C4_11D2_AE30_0020182F1487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XSectionDisplayDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXSectionDisplayDlg dialog

class CXSectionDisplayDlg : public CDialog
{
// Construction
public:
	POINT m_Endpoint;
	POINT m_Startpoint;
	CXSectionDisplayDlg(CWnd* pParent = NULL, POINT *pstartpoint=NULL, POINT *pendpoint=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CXSectionDisplayDlg)
	enum { IDD = IDD_XSECTIONDISPLAYDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXSectionDisplayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CXSectionDisplayDlg)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XSECTIONDISPLAYDLG_H__8DC048E6_A4C4_11D2_AE30_0020182F1487__INCLUDED_)
