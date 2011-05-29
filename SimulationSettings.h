#if !defined(AFX_SIMULATIONSETTINGS_H__FB128C03_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
#define AFX_SIMULATIONSETTINGS_H__FB128C03_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SimulationSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SimulationSettings dialog

class SimulationSettings : public CDialog
{
// Construction
public:
	BOOL Create ();
	SimulationSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SimulationSettings)
	enum { IDD = SETTINGS_DIALOG };
	CScrollBar	m_LipidRemovalScroll;
	CScrollBar	m_LipidScroll;
	CScrollBar	m_NumSMCScroll;
	CScrollBar	m_NumMacroScroll;
	int		m_NumMacrophageContactsPerCycle;
	int		m_MacroScrollValue;
	int		m_NumSMCContactsPerCycle;
	int		m_SMCMacroKillRate;
	int		m_LipidRemovalRate;
	int		m_LipidArrivalRate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SimulationSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SimulationSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeNoofmacrohittingwalldisplay();
	afx_msg void OnChangeNoofsmchittingwalldisplay();
	afx_msg void OnRecordsimulationinfo();
	afx_msg void OnHelp();
	afx_msg void OnChangeSmcmacrokillrate();
	afx_msg void OnChangeLipidarrivalratedisplay();
	afx_msg void OnChangeLipiddeathratedisplay();
	afx_msg void OnColours();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

void StopSimualationUpdate();
void StartSimualationUpdate();
void RestartApp();


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMULATIONSETTINGS_H__FB128C03_C7C0_11D1_8BD5_C70EEF9E9032__INCLUDED_)
