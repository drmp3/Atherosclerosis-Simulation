// SimulationInfo.cpp : implementation file
//

#include "athero.h"
#include "SimulationInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SimulationInfo dialog


SimulationInfo::SimulationInfo(CWnd* pParent /*=NULL*/)
	: CDialog(SimulationInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(SimulationInfo)
	m_NumCycles = 0;
	m_NumMacrophages = 0;
	//}}AFX_DATA_INIT
}


void SimulationInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SimulationInfo)
	DDX_Text(pDX, IDC_NUMBEROFCYCLES, m_NumCycles);
	DDX_Text(pDX, IDC_NUMMACROPHAGES, m_NumMacrophages);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SimulationInfo, CDialog)
	//{{AFX_MSG_MAP(SimulationInfo)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_STOPATCYCLE, OnChangeStopatcycle)
	ON_WM_PAINT()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL SimulationInfo::Create() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::Create(IDD);
}

void SimulationInfo::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow (SW_HIDE);
}



void SimulationInfo::OnChangeStopatcycle() 
{
	// TODO: Add your control notification handler code here
	StopAtCycle=GetDlgItemInt (IDC_STOPATCYCLE);	//doesnt matter if its 0 - it wont stop
}

BOOL SimulationInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetDlgItemInt (IDC_STOPATCYCLE,0);
	SetDlgItemText (IDC_RECORDING,"Not Recording");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SimulationInfo::OnPaint() 
{
	UpdateData (FALSE);
	EnterCriticalSection (&LockCritSection);		
	{
		CPaintDC dc(this); // device context for painting
	}
	LeaveCriticalSection (&LockCritSection);		
}
