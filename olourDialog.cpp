// olourDialog.cpp : implementation file
//

#include "stdafx.h"
#include "athero.h"
#include "olourDialog.h"
#include "resource.h"
#include "ddrawx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int ARTERY_RED  = 190;
int ARTERY_GREEN  = 0;
int ARTERY_BLUE  = 0;

int COLOUR_FOR_MAX_NO_MACROS_R =255;
int COLOUR_FOR_MAX_NO_MACROS_G =210;
int COLOUR_FOR_MAX_NO_MACROS_B =0;

int COLOUR_FOR_1_MACRO_R =90;
int COLOUR_FOR_1_MACRO_G =50;
int COLOUR_FOR_1_MACRO_B =0;

int COLOUR_FOR_MAX_NO_SMCS_R =1;
int COLOUR_FOR_MAX_NO_SMCS_G =239;
int COLOUR_FOR_MAX_NO_SMCS_B =57;

int COLOUR_FOR_1_SMC_R =1;
int COLOUR_FOR_1_SMC_G =51;
int COLOUR_FOR_1_SMC_B =46;

int COLOUR_FOR_MAX_NO_LIPIDS_R =255;
int COLOUR_FOR_MAX_NO_LIPIDS_G =255;
int COLOUR_FOR_MAX_NO_LIPIDS_B =255;

int COLOUR_FOR_1_LIPID_R  = 100;
int COLOUR_FOR_1_LIPID_G = 100;
int COLOUR_FOR_1_LIPID_B = 100;



/////////////////////////////////////////////////////////////////////////////
// ColourDialog dialog


ColourDialog::ColourDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ColourDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(ColourDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ColourDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ColourDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ColourDialog, CDialog)
	//{{AFX_MSG_MAP(ColourDialog)
	ON_BN_CLICKED(IDC_LIPIDMAX, OnLipidmax)
	ON_BN_CLICKED(IDC_LIPIDMIN, OnLipidmin)
	ON_BN_CLICKED(IDC_MACRO1CELL, OnMacro1cell)
	ON_BN_CLICKED(IDC_MACROMAXCELLS, OnMacromaxcells)
	ON_BN_CLICKED(IDC_SMC1CELL, OnSmc1cell)
	ON_BN_CLICKED(IDC_SMCMAXCELLS, OnSmcmaxcells)
	ON_BN_CLICKED(IDC_RESTOREDEFAULTCOLS, OnRestoredefaultcols)
	ON_BN_CLICKED(IDC_BACKGROUND, OnBackground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// ColourDialog message handlers

void CalculateColours ()
{
	CalculateMacrophageColours ();
	CalculateSMCColours ();
	CalculateLipidColours ();
	DrawMacrophageBuffer ();
	InvalidateRect (AtheroWindow,NULL,false);
}

void ColourDialog::OnLipidmax() 
{
	
	CColorDialog cdlg (RGB(COLOUR_FOR_MAX_NO_LIPIDS_R, COLOUR_FOR_MAX_NO_LIPIDS_G, COLOUR_FOR_MAX_NO_LIPIDS_B), CC_FULLOPEN, this);
	if (cdlg.DoModal () == IDOK) {
		COLORREF res = cdlg.GetColor ();
		COLOUR_FOR_MAX_NO_LIPIDS_R =GetRValue (res);
	    COLOUR_FOR_MAX_NO_LIPIDS_G =GetGValue (res);
		COLOUR_FOR_MAX_NO_LIPIDS_B =GetBValue (res);
		CalculateColours ();
	}
	
}

void ColourDialog::OnLipidmin() 
{
	
	CColorDialog cdlg (RGB(COLOUR_FOR_1_LIPID_R, COLOUR_FOR_1_LIPID_G, COLOUR_FOR_1_LIPID_B), CC_FULLOPEN, this);
	if (cdlg.DoModal () == IDOK) {
		COLORREF res = cdlg.GetColor ();
		COLOUR_FOR_1_LIPID_R =GetRValue (res);
	    COLOUR_FOR_1_LIPID_G =GetGValue (res);
		COLOUR_FOR_1_LIPID_B =GetBValue (res);
		CalculateColours ();
	}	
}

void ColourDialog::OnMacro1cell() 
{
	
	CColorDialog cdlg (RGB(COLOUR_FOR_1_MACRO_R, COLOUR_FOR_1_MACRO_G, COLOUR_FOR_1_MACRO_B), CC_FULLOPEN, this);
	if (cdlg.DoModal () == IDOK) {
		COLORREF res = cdlg.GetColor ();
		COLOUR_FOR_1_MACRO_R =GetRValue (res);
	    COLOUR_FOR_1_MACRO_G =GetGValue (res);
		COLOUR_FOR_1_MACRO_B =GetBValue (res);
		CalculateColours ();
	}

}

void ColourDialog::OnMacromaxcells() 
{
	
	CColorDialog cdlg (RGB(COLOUR_FOR_MAX_NO_MACROS_R, COLOUR_FOR_MAX_NO_MACROS_G, COLOUR_FOR_MAX_NO_MACROS_B), CC_FULLOPEN, this);
	if (cdlg.DoModal () == IDOK) {
		COLORREF res = cdlg.GetColor ();
		COLOUR_FOR_MAX_NO_MACROS_R =GetRValue (res);
	    COLOUR_FOR_MAX_NO_MACROS_G =GetGValue (res);
		COLOUR_FOR_MAX_NO_MACROS_B =GetBValue (res);
		CalculateColours ();
	}
}

void ColourDialog::OnSmc1cell() 
{
	CColorDialog cdlg (RGB(COLOUR_FOR_1_SMC_R, COLOUR_FOR_1_SMC_G, COLOUR_FOR_1_SMC_B), CC_FULLOPEN, this);
	if (cdlg.DoModal () == IDOK) {
		COLORREF res = cdlg.GetColor ();
		COLOUR_FOR_1_SMC_R =GetRValue (res);
	    COLOUR_FOR_1_SMC_G =GetGValue (res);
		COLOUR_FOR_1_SMC_B =GetBValue (res);
		CalculateColours ();
	}	
}

void ColourDialog::OnSmcmaxcells() 
{
	
	CColorDialog cdlg (RGB(COLOUR_FOR_MAX_NO_SMCS_R, COLOUR_FOR_MAX_NO_SMCS_G, COLOUR_FOR_MAX_NO_SMCS_B), CC_FULLOPEN, this);
	if (cdlg.DoModal () == IDOK) {
		COLORREF res = cdlg.GetColor ();
		COLOUR_FOR_MAX_NO_SMCS_R =GetRValue (res);
	    COLOUR_FOR_MAX_NO_SMCS_G =GetGValue (res);
		COLOUR_FOR_MAX_NO_SMCS_B =GetBValue (res);
		CalculateColours ();
	}
}

void ColourDialog::OnRestoredefaultcols() 
{
	
	COLOUR_FOR_MAX_NO_MACROS_R =255;
	COLOUR_FOR_MAX_NO_MACROS_G =210;
	COLOUR_FOR_MAX_NO_MACROS_B =0;
	
	COLOUR_FOR_1_MACRO_R =90;
	COLOUR_FOR_1_MACRO_G =50;
	COLOUR_FOR_1_MACRO_B =0;
	
	COLOUR_FOR_MAX_NO_SMCS_R =1;
	COLOUR_FOR_MAX_NO_SMCS_G =239;
	COLOUR_FOR_MAX_NO_SMCS_B =57;
	
	COLOUR_FOR_1_SMC_R =1;
	COLOUR_FOR_1_SMC_G =51;
	COLOUR_FOR_1_SMC_B =46;
	
	COLOUR_FOR_MAX_NO_LIPIDS_R =255;
	COLOUR_FOR_MAX_NO_LIPIDS_G =255;
	COLOUR_FOR_MAX_NO_LIPIDS_B =255;
	
	COLOUR_FOR_1_LIPID_R  = 100;
	COLOUR_FOR_1_LIPID_G = 100;
	COLOUR_FOR_1_LIPID_B = 100;

	ARTERY_RED  = 190;
	ARTERY_GREEN  = 0;
	ARTERY_BLUE  = 0;

	CalculateColours ();

	MessageBox ("Default Colours Restored");
	
}

void ColourDialog::OnBackground() 
{
	CColorDialog cdlg (RGB(ARTERY_RED, ARTERY_GREEN, ARTERY_BLUE), CC_FULLOPEN, this);
	if (cdlg.DoModal () == IDOK) {
		COLORREF res = cdlg.GetColor ();
		ARTERY_RED =GetRValue (res);
	    ARTERY_GREEN =GetGValue (res);
		ARTERY_BLUE =GetBValue (res);
		CalculateColours ();
	}
}
