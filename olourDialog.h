#if !defined(AFX_OLOURDIALOG_H__5F6D1CA3_FD8D_11D5_95CD_BBD1F1D0E936__INCLUDED_)
#define AFX_OLOURDIALOG_H__5F6D1CA3_FD8D_11D5_95CD_BBD1F1D0E936__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// olourDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ColourDialog dialog

class ColourDialog : public CDialog
{
// Construction
public:
	ColourDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ColourDialog)
	enum { IDD = IDD_COLOURDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ColourDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ColourDialog)
	afx_msg void OnLipidmax();
	afx_msg void OnLipidmin();
	afx_msg void OnMacro1cell();
	afx_msg void OnMacromaxcells();
	afx_msg void OnSmc1cell();
	afx_msg void OnSmcmaxcells();
	afx_msg void OnRestoredefaultcols();
	afx_msg void OnBackground();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLOURDIALOG_H__5F6D1CA3_FD8D_11D5_95CD_BBD1F1D0E936__INCLUDED_)
