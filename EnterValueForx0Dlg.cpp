// EnterValueForx0Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "athero.h"
#include "EnterValueForx0Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnterValueForx0Dlg dialog


CEnterValueForx0Dlg::CEnterValueForx0Dlg(CWnd* pParent /*=NULL*/,FUNCTIONADJUSTERINFO *pFuncAdjInfo)
	: CDialog(CEnterValueForx0Dlg::IDD, pParent)
{
	lpFunctionAdjusterInfo = pFuncAdjInfo;

	//{{AFX_DATA_INIT(CEnterValueForx0Dlg)
	m_UsingThisValue = FALSE;
	m_Value = 0.0;
	//}}AFX_DATA_INIT

//	if (pFuncAdjInfo->UseValueForx0)
	//{
		//m_Value = (float)pFuncAdjInfo->ValueForx0  / (double)((unsigned)-1);
//		m_UsingThisValue = true;
	//}
	//else
	//{
		m_Value = pFuncAdjInfo->yValuesDataForGraph[0];
		//m_UsingThisValue = false;
	//}
	
}


void CEnterValueForx0Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterValueForx0Dlg)
	DDX_Check(pDX, IDC_CHECK2, m_UsingThisValue);
	DDX_Text(pDX, IDC_VALUEFORX0, m_Value);
	DDV_MinMaxDouble(pDX, m_Value, 0., 100.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnterValueForx0Dlg, CDialog)
	//{{AFX_MSG_MAP(CEnterValueForx0Dlg)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterValueForx0Dlg message handlers

void CEnterValueForx0Dlg::OnOk() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK ();
	//lpFunctionAdjusterInfo->ValueForx0 = unsigned (m_Value* ((unsigned) -1) );
	lpFunctionAdjusterInfo->yValuesDataForGraph[0] = m_Value;
	GenerateyValuesForEveryxValue (lpFunctionAdjusterInfo);
	//lpFunctionAdjusterInfo->UseValueForx0 = m_UsingThisValue;
}
