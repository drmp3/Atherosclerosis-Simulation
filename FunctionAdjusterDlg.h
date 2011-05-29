#if !defined(AFX_FUNCTIONADJUSTERDLG_H__71EF3741_8221_11D2_AE30_0020182F1487__INCLUDED_)
#define AFX_FUNCTIONADJUSTERDLG_H__71EF3741_8221_11D2_AE30_0020182F1487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FunctionAdjusterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFunctionAdjusterDlg dialog

class CFunctionAdjusterDlg : public CDialog
{
// Construction
public:
	FUNCTIONADJUSTERINFO* lpFunctionAdjusterInfo;
	CFunctionAdjusterDlg(CWnd* pParent = NULL,FUNCTIONADJUSTERINFO *pFuncAdjInfo = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFunctionAdjusterDlg)
	enum { IDD = IDD_FUNCTIONADJ };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFunctionAdjusterDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFunctionAdjusterDlg)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfirstxvalue();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUNCTIONADJUSTERDLG_H__71EF3741_8221_11D2_AE30_0020182F1487__INCLUDED_)
