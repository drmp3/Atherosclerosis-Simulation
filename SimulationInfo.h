#if !defined(AFX_SIMULATIONINFO_H__FB128C02_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
#define AFX_SIMULATIONINFO_H__FB128C02_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SimulationInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SimulationInfo dialog

class SimulationInfo : public CDialog
{
// Construction
public:
	SimulationInfo(CWnd* pParent = NULL);   // standard constructor
	BOOL Create();
// Dialog Data
	//{{AFX_DATA(SimulationInfo)
	enum { IDD = IDD_SIMULATIONINFO };
	int		m_NumCycles;
	int		m_NumMacrophages;
	int		m_macroKillRate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SimulationInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SimulationInfo)
	afx_msg void OnClose();
	afx_msg void OnChangeStopatcycle();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMULATIONINFO_H__FB128C02_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
