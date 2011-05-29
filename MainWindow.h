#if !defined(AFX_MAINWINDOW_H__FB128C05_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
#define AFX_MAINWINDOW_H__FB128C05_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_

#include "SimulationSettings.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MainWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainWindow dialog


class CMainWindow : public CDialog
{
// Construction
public:
	HICON m_hIcon;
	POINT XSectionEnd;
	POINT XSectionStart;
	POINT Adjust3DStart;
	bool XSectionMode;
	bool Adjust3DView;
	bool Adjust3DZoom;
	CMainWindow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMainWindow)
	enum { IDD = IDD_MAINDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMainWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnMenuhelp();
	afx_msg void OnXsection();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnAbout();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINWINDOW_H__FB128C05_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
