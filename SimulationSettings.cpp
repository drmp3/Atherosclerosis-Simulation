// SimulationSettings.cpp : implementation file
//

#include "athero.h"
#include "SimulationSettings.h"
#include "FunctionAdjusterDlg.h"
#include "olourDialog.h"
#include "Macrophg.h"
#include "MacroGame.h"
#include "ddrawx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// SimulationSettings dialog

HWND MacroArrivalGraphWindow,MacroDeathGraphWindow,SMCArrivalGraphWindow;
DWORD SimulationThreadID;
HANDLE SimulationThreadHandle;
DWORD g_LastUpdateTime;
DWORD g_RenderTime;
bool  g_DoMacroUpdate;
bool  g_Restart;



SimulationSettings::SimulationSettings(CWnd* pParent /*=NULL*/)
	: CDialog(SimulationSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(SimulationSettings)
	m_NumMacrophageContactsPerCycle = 0;
	m_MacroScrollValue = 0;
	m_NumSMCContactsPerCycle = 0;
	m_SMCMacroKillRate = 0;
	m_LipidRemovalRate = 0;
	m_LipidArrivalRate = 0;
	//}}AFX_DATA_INIT
}


void SimulationSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SimulationSettings)
	DDX_Control(pDX, IDC_LIPIDREMOVALRATE, m_LipidRemovalScroll);
	DDX_Control(pDX, IDC_LIPIDARRIVALRATE, m_LipidScroll);
	DDX_Control(pDX, IDC_NOOFSMCHITTINGWALLSCROLL, m_NumSMCScroll);
	DDX_Control(pDX, IDC_NOOFMACROHITTINGWALLSCROLL, m_NumMacroScroll);
	DDX_Text(pDX, IDC_NOOFMACROHITTINGWALLDISPLAY, m_NumMacrophageContactsPerCycle);
	DDV_MinMaxInt(pDX, m_NumMacrophageContactsPerCycle, 0, MAX_MACRO_CONTACTS_PER_CYCLE);
	DDX_Scroll(pDX, IDC_NOOFMACROHITTINGWALLSCROLL, m_MacroScrollValue);
	DDX_Text(pDX, IDC_NOOFSMCHITTINGWALLDISPLAY, m_NumSMCContactsPerCycle);
	DDX_Text(pDX, IDC_SMCMACROKILLRATE, m_SMCMacroKillRate);
	DDV_MinMaxInt(pDX, m_SMCMacroKillRate, 0, 100);
	DDX_Text(pDX, IDC_LIPIDDEATHRATEDISPLAY, m_LipidRemovalRate);
	DDV_MinMaxInt(pDX, m_LipidRemovalRate, 0, 100000);
	DDX_Text(pDX, IDC_LIPIDARRIVALRATEDISPLAY, m_LipidArrivalRate);
	DDV_MinMaxInt(pDX, m_LipidArrivalRate, 0, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SimulationSettings, CDialog)
	//{{AFX_MSG_MAP(SimulationSettings)
	ON_WM_CANCELMODE()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_NOOFMACROHITTINGWALLDISPLAY, OnChangeNoofmacrohittingwalldisplay)
	ON_EN_CHANGE(IDC_NOOFSMCHITTINGWALLDISPLAY, OnChangeNoofsmchittingwalldisplay)
	ON_BN_CLICKED(IDC_RECORDSIMULATIONINFO, OnRecordsimulationinfo)
	ON_BN_CLICKED(IDC_HELPCONTENTS, OnHelp)
	ON_EN_CHANGE(IDC_SMCMACROKILLRATE, OnChangeSmcmacrokillrate)
	ON_EN_CHANGE(IDC_LIPIDARRIVALRATEDISPLAY, OnChangeLipidarrivalratedisplay)
	ON_EN_CHANGE(IDC_LIPIDDEATHRATEDISPLAY, OnChangeLipiddeathratedisplay)
	ON_BN_CLICKED(IDC_COLOURS, OnColours)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SimulationSettings message handlers

BOOL SimulationSettings::Create()
{
	return CDialog::Create (IDD);
}

BOOL SimulationSettings::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
//This message is sent when either a button is clicked, or the text boxes have the number in them
//changed
	switch (wParam)
	{
		case IDCANCEL:
		//If the 'close' button is clicked, or the cross on the top-right of the window, then the dialog
		//box is not destroyed but is simply hidden
			ShowWindow (SW_HIDE);
			break;
		case IDC_NOOFSMCHITTINGWALLDISPLAY:
			UpdateData ();	
			m_NumSMCScroll.SetScrollPos (m_NumSMCContactsPerCycle/10,TRUE);
			UpdateData (FALSE);	
			break;
		case IDC_MACROSETARRIVALCHANCES:
			DrawMacroArrivalGraph (20,20);
		break;
		case IDC_MACROSETKILLRATE:
			DrawMacroDeathGraph (390,20);
		break;
		case IDC_SMCSETARRIVALCHANCES:
			DrawSMCArrivalGraph (30,30);
		break;
		case IDC_SAVESETTINGS:
			SaveSettings (m_hWnd);
		break;
		case IDC_LOADSETTINGS:
			LoadSettings (m_hWnd); 
			m_NumMacrophageContactsPerCycle = NumMacrophageContactsPerCycle;
			m_NumSMCContactsPerCycle = NumSMCContactsPerCycle;
			m_SMCMacroKillRate = SMCMacroKillRate;
			m_LipidRemovalRate = LipidRemovalRate;
			m_LipidArrivalRate = LipidArrivalRate;
			UpdateData (false);
		break;

		//If the user clicks on Run Demo..., then the program opens a load dialog box and runs a demo
		case IDC_RUNDEMO:
			RunDemo (m_hWnd);
		break;

	}	
	return CDialog::OnCommand(wParam, lParam);
}

void DrawMacroArrivalGraph (int x,int y)
{
	/*
	if (MacroArrivalFuncInfo.GraphVisible)
		return;
	if (!MacroArrivalFuncInfo.Initialised)
		InitMacroArrivalFuncInfo ();
	MacroArrivalGraphWindow = CreateFunctionAdjusterWindow (&MacroArrivalFuncInfo,x,y);
	*/
	CFunctionAdjusterDlg Macrogrph (NULL,&MacroArrivalFuncInfo);
	Macrogrph.DoModal ();
}

void DrawSMCArrivalGraph (int x,int y)
{
	/*
	if (SMCArrivalFuncInfo.GraphVisible)
		return;
	if (!SMCArrivalFuncInfo.Initialised)
		InitSMCArrivalFuncInfo ();
	SMCArrivalGraphWindow = CreateFunctionAdjusterWindow (&SMCArrivalFuncInfo,x,y);
	*/
	CFunctionAdjusterDlg Macrogrph (NULL,&SMCArrivalFuncInfo);
	Macrogrph.DoModal ();
}


void DrawMacroDeathGraph (int x,int y)
{
	/*
	if (MacroDeathFuncInfo.GraphVisible)
		return;
	if (!MacroDeathFuncInfo.Initialised)
		InitMacroDeathFuncInfo ();
	MacroDeathGraphWindow = CreateFunctionAdjusterWindow (&MacroDeathFuncInfo,x,y);
	*/
	CFunctionAdjusterDlg Macrogrph (NULL,&MacroDeathFuncInfo);
	Macrogrph.DoModal ();
}

DWORD SimulationThread (LPDWORD lpStartupData)
{
    g_Restart = true;
    while (g_Restart)
    {
        g_Restart = false;
        MacroInit();
        if (GameMode)
            GameInit();

        g_LastUpdateTime = GetTickCount();
    	    
	    while (SimulationGo)
	    {
            int curTime = GetTickCount();
            int maxupdates = 5;
            while (g_LastUpdateTime < curTime && --maxupdates)
            {
                g_LastUpdateTime += (1000/UPDATES_PER_SEC);
                if (g_DoMacroUpdate)
                    MacroUpdate();
                if (GameMode)
                    GameUpdate();            
            }

            if (!maxupdates)
            {
                //if was delay, dont try to catch up
                g_LastUpdateTime  = GetTickCount();
            }
		    
            g_RenderTime = GetTickCount();
            MacroRender();
            if (GameMode)
                GameRender();
            ComposeSurfaces();
            if (GameMode)
                GameDrawHUD();
            DrawMainSurface();
	    }

        if (GameMode)
            GameTerm();
        MacroTerminate();

        if (g_Restart)
            SimulationGo=1;
    }
 
    return 0;
}

void StartSimulation (void)
{
	ModifyMenu (GetMenu (AtheroWindow),CM_START,MF_BYCOMMAND|MF_STRING,CM_PAUSE,"Pause");	//Remove the START! menu item
	InsertMenu (GetMenu (AtheroWindow),CM_PAUSE,MF_BYCOMMAND|MF_STRING,CM_RESTART,"Restart");
    g_DoMacroUpdate = true;

	//This function creates a new thread and makes the new thread do the heavy work - the simulation itself
	//This allows the program to continue normally, with all the windows functioning correctly
	SimulationThreadHandle = CreateThread (
	NULL,	//Security attributes - default
	NULL, //stack size - default
	(LPTHREAD_START_ROUTINE) SimulationThread,	//Start point of new thread
	NULL,	//No additional startup data
	NULL,//No additional creation flags
	&SimulationThreadID	//Recieves thread id
	);
}

void PauseSimulation (void)
{
	EnterCriticalSection (&LockCritSection);
	SuspendThread (SimulationThreadHandle);
	LeaveCriticalSection (&LockCritSection);
	SimulationGo = 0;
}

void ResumeSimulation (void)
{
	SimulationGo = 1;
	ResumeThread (SimulationThreadHandle);
}

void EndSimulation (void)
{
		
	ResumeSimulation ();	//If the simulation was paused, resume it (or setting SimulationGo = 0 will
	//have no effect)
	SimulationGo = 0;	//Causes the simulation to stop, freeing up memory
    g_Restart = false;
	DWORD ThreadExitCode;
	do
	{
		GetExitCodeThread (SimulationThreadHandle,&ThreadExitCode);
	}
	while (ThreadExitCode == STILL_ACTIVE);	//Wait for the simulation thread to terminate

	ModifyMenu (GetMenu (AtheroWindow),CM_RESTART,MF_BYCOMMAND|MF_STRING,CM_START,"START!");
	RemoveMenu (GetMenu (AtheroWindow),CM_PAUSE,MF_BYCOMMAND);
}

void StopSimualationUpdate()
{
    g_DoMacroUpdate = false;
}

void StartSimualationUpdate()
{
    g_DoMacroUpdate = true;
}

void RestartApp()
{
    g_Restart = true;
    SimulationGo = 0;
}



void SaveSettings (HWND hwnd)
{
	//Here a 'Save Settings' Dialog box is created, using one of windows' 'Common Dialog box' functions
	//which creates a save dialog box and returns the filename the user selected
	  BOOL Result;
	  OPENFILENAME SaveDialogBoxInfo;	//This structure contains all the information to put the dialog box
	  //on the screen
	  SaveDialogBoxInfo.lStructSize = sizeof (OPENFILENAME);
	  SaveDialogBoxInfo.hwndOwner = hwnd;
	  SaveDialogBoxInfo.hInstance = AtheroInstance;
	  SaveDialogBoxInfo.lpstrFilter = "Simulation Settings (*.csv)\0*.csv\0";
	  SaveDialogBoxInfo.lpstrCustomFilter = NULL;
	  SaveDialogBoxInfo.nFilterIndex = 0;
	  SaveDialogBoxInfo.lpstrFile = new char [256];
	  SaveDialogBoxInfo.nMaxFile = 256;
	  FillMemory (SaveDialogBoxInfo.lpstrFile,256,0);
	  SaveDialogBoxInfo.lpstrFileTitle = NULL;
	  SaveDialogBoxInfo.lpstrInitialDir = NULL;
	  SaveDialogBoxInfo.lpstrTitle = "Save Simulation Settings";
	  SaveDialogBoxInfo.Flags = OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
	  SaveDialogBoxInfo.lpstrDefExt = "csv";
	  Result = GetSaveFileName (&SaveDialogBoxInfo);
	  if (!Result)
			return;
	  SaveSettingsFromFile (hwnd,SaveDialogBoxInfo.lpstrFile);
}

void SaveSettingsFromFile (HWND hwnd,char *SaveFilename)
{
	  FILE *FileHandle = fopen (SaveFilename,"w");
	  if (!FileHandle)
	  {
			MessageBox (hwnd,"Unable to create file for saving","Save File Failed!",MB_OK);
			return;
	  }
	  fprintf (FileHandle,"Atherosclerosis Simulation Settings,\n");
	  fprintf (FileHandle,"VERSION,%f\n",VERSION);

	  fprintf (FileHandle,"ARTERY_RED, %d\n",ARTERY_RED);
	  fprintf (FileHandle,"ARTERY_GREEN, %d\n",ARTERY_GREEN);
	  fprintf (FileHandle,"ARTERY_BLUE, %d\n",ARTERY_BLUE);
	  
	  
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_MACROS_R, %d\n",COLOUR_FOR_MAX_NO_MACROS_R);
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_MACROS_G, %d\n",COLOUR_FOR_MAX_NO_MACROS_G);
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_MACROS_B, %d\n",COLOUR_FOR_MAX_NO_MACROS_B);
	  
	  fprintf (FileHandle,"COLOUR_FOR_1_MACRO_R, %d\n",COLOUR_FOR_1_MACRO_R);
	  fprintf (FileHandle,"COLOUR_FOR_1_MACRO_G, %d\n",COLOUR_FOR_1_MACRO_G);
	  fprintf (FileHandle,"COLOUR_FOR_1_MACRO_B, %d\n",COLOUR_FOR_1_MACRO_B);
	  
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_SMCS_R, %d\n",COLOUR_FOR_MAX_NO_SMCS_R);
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_SMCS_G, %d\n",COLOUR_FOR_MAX_NO_SMCS_G);
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_SMCS_B, %d\n",COLOUR_FOR_MAX_NO_SMCS_B);
	  
	  fprintf (FileHandle,"COLOUR_FOR_1_SMC_R, %d\n",COLOUR_FOR_1_SMC_R);
	  fprintf (FileHandle,"COLOUR_FOR_1_SMC_G, %d\n",COLOUR_FOR_1_SMC_G);
	  fprintf (FileHandle,"COLOUR_FOR_1_SMC_B, %d\n",COLOUR_FOR_1_SMC_B);
	  
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_LIPIDS_R, %d\n",COLOUR_FOR_MAX_NO_LIPIDS_R);
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_LIPIDS_G, %d\n",COLOUR_FOR_MAX_NO_LIPIDS_G);
	  fprintf (FileHandle,"COLOUR_FOR_MAX_NO_LIPIDS_B, %d\n",COLOUR_FOR_MAX_NO_LIPIDS_B);
	  
	  fprintf (FileHandle,"COLOUR_FOR_1_LIPID_R, %d\n",COLOUR_FOR_1_LIPID_R);
	  fprintf (FileHandle,"COLOUR_FOR_1_LIPID_G, %d\n",COLOUR_FOR_1_LIPID_G);
	  fprintf (FileHandle,"COLOUR_FOR_1_LIPID_B, %d\n",COLOUR_FOR_1_LIPID_B);

	  fprintf (FileHandle,"RateOfMacrophagesHittingArtery,");
	  fprintf (FileHandle,"%d\n",NumMacrophageContactsPerCycle);
	  fprintf (FileHandle,"SMCMacroInhibition,");
	  fprintf (FileHandle,"%d\n",SMCMacroKillRate);
	  fprintf (FileHandle,"LipidArrivalRate,");
	  fprintf (FileHandle,"%d\n",LipidArrivalRate);
	  fprintf (FileHandle,"LipidRemovalRate,");
	  fprintf (FileHandle,"%d\n",LipidRemovalRate);
	  fprintf (FileHandle,"RateOfSMCHittingArtery,");
	  fprintf (FileHandle,"%d\n",NumSMCContactsPerCycle);

	  fprintf (FileHandle,"Number of surrounding cells,Rate of Macrophage death\n");
	  int i;

	  for (i=0;i<=MacroDeathFuncInfo.NumPointsOnGraph;i++)
	  {
		  fprintf (FileHandle,"%d,",i*MacroDeathFuncInfo.Max_x_value/MacroDeathFuncInfo.NumPointsOnGraph);
		  fprintf (FileHandle,"%f\n",MacroDeathFuncInfo.yValuesDataForGraph[i]);
	  }

	  fprintf (FileHandle,"Number of surrounding cells,Rate of Macrophage arrival\n");
	  for (i=0;i<=NUM_DEATH_GRAPH_POINTS;i++)
	  {
		  fprintf (FileHandle,"%d,",i*MacroArrivalFuncInfo.Max_x_value/MacroArrivalFuncInfo.NumPointsOnGraph);
		  fprintf (FileHandle,"%f\n",MacroArrivalFuncInfo.yValuesDataForGraph[i]);
	  }

	  fprintf (FileHandle,"Number of surrounding cells,Rate of SMC arrival\n");
	  for (i=0;i<=NUM_ARRIVAL_GRAPH_POINTS;i++)
	  {
		  fprintf (FileHandle,"%d,",i*SMCArrivalFuncInfo.Max_x_value/SMCArrivalFuncInfo.NumPointsOnGraph);
		  fprintf (FileHandle,"%f\n",SMCArrivalFuncInfo.yValuesDataForGraph[i]);
	  }
	  fclose (FileHandle);
}


void SimulationSettings::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::CalcWindowRect(lpClientRect, nAdjustType);
}

BOOL SimulationSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	/*
	::SetDlgItemInt(m_hWnd,IDC_NOOFMACROHITTINGWALLDISPLAY,NumMacrophageContactsPerCycle,FALSE);	//These two lines set the edit boxes
	::SetDlgItemInt(m_hWnd,IDC_NOOFSMCHITTINGWALLDISPLAY,NumSMCContactsPerCycle,FALSE);	//to the values of NumMCPC and NumSMCCPC
	ScrollBar_SetRange (::GetDlgItem (m_hWnd,IDC_NOOFMACROHITTINGWALLSCROLL),0,MAX_MACRO_CONTACTS_PER_CYCLE/10,FALSE);	//And these set the range
	ScrollBar_SetPos (::GetDlgItem (m_hWnd,IDC_NOOFMACROHITTINGWALLSCROLL),NumMacrophageContactsPerCycle/10,TRUE);	//and position of the scrollbars
	ScrollBar_SetRange (::GetDlgItem (m_hWnd,IDC_NOOFSMCHITTINGWALLSCROLL),0,MAX_SMC_CONTACTS_PER_CYCLE/10,FALSE);
	ScrollBar_SetPos (::GetDlgItem (m_hWnd,IDC_NOOFSMCHITTINGWALLSCROLL),NumSMCContactsPerCycle/10,TRUE);
	//Also set a variable to say dialog has initialised
	*/
	m_NumMacrophageContactsPerCycle = NumMacrophageContactsPerCycle;
	m_NumSMCContactsPerCycle = NumSMCContactsPerCycle;
	m_SMCMacroKillRate = SMCMacroKillRate;
	m_LipidRemovalRate = LipidRemovalRate;
	m_LipidArrivalRate = LipidArrivalRate;
	UpdateData (FALSE);

	m_NumMacroScroll.SetScrollRange (0,MAX_MACRO_CONTACTS_PER_CYCLE/10);
	m_NumMacroScroll.SetScrollPos (NumMacrophageContactsPerCycle/10,TRUE);
	
	m_NumSMCScroll.SetScrollRange (0,MAX_SMC_CONTACTS_PER_CYCLE/10);
	m_NumSMCScroll.SetScrollPos (NumSMCContactsPerCycle/10,TRUE);
		
	m_LipidScroll.SetScrollRange (0,100);
	m_LipidScroll.SetScrollPos ((LipidArrivalRate*100)/65535,TRUE);
	
	m_LipidRemovalScroll.SetScrollRange (0,100);
	m_LipidRemovalScroll.SetScrollPos ((LipidRemovalRate*10000)/65535,TRUE);
	

	//SettingsDialogInitialised = TRUE;
	return TRUE;
}

void SimulationSettings::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void SimulationSettings::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if (nSBCode != SB_THUMBTRACK)
	nPos = pScrollBar->GetScrollPos (); //Only if the scrollbar is 'thumbtracking' (dragging the button across)
	//does pos equal the new position of the scrollbar, so if we are thumbtracking, we do not need to set
	//pos to the position of the scrollbar

	//Below says 'If you clicked the right arrow on the scrollbar, then pos goes up by one, if you
	//clicked the left, then down by one, etc

	int min,max;
	pScrollBar->GetScrollRange (&min, &max);

	switch (nSBCode)
	{
		case SB_LINERIGHT:
			nPos++;
			break;
		case SB_LINELEFT:
			nPos--;
			break;
		case SB_PAGELEFT:
			nPos-=(max-min)/25;
			break;
		case SB_PAGERIGHT:
			nPos+=(max-min)/25;
			break;
	}

	pScrollBar->SetScrollPos (nPos,TRUE);
	nPos = pScrollBar->GetScrollPos ();	//This is an easy way of making sure 'nPos' is in acceptable limits

	if (GetDlgItem (IDC_NOOFMACROHITTINGWALLSCROLL) == pScrollBar)
	{

	//The if statment says 'if the macrophage arrival rate scroll bar has moved...'
	//nPos is the new nPosition of the scrollbar
		SetDlgItemInt(IDC_NOOFMACROHITTINGWALLDISPLAY,nPos*10,FALSE);	//This sets the text box beneath the
									//scrollbar to read the new number of cell contacts per cycle
		NumMacrophageContactsPerCycle = nPos*10;	//And set the global variable as well
	}
	else if (GetDlgItem (IDC_NOOFSMCHITTINGWALLSCROLL) == pScrollBar)
	{
		//The if statment says 'if the SMC arrival rate scroll7 bar has moved...'
		SetDlgItemInt(IDC_NOOFSMCHITTINGWALLDISPLAY,nPos*10,FALSE); //Set the edit box to the new value of nPos(*10)
		NumSMCContactsPerCycle = nPos*10;	//And set the global variable as well
	}
	else if (GetDlgItem (IDC_LIPIDARRIVALRATE) == pScrollBar)
	{
		SetDlgItemInt(IDC_LIPIDARRIVALRATEDISPLAY,nPos,FALSE); //Set the edit box to the new value of pos(*10)
		LipidArrivalRate = (nPos*65535)/100;	//And set the global variable as well
	}
	else if (GetDlgItem (IDC_LIPIDREMOVALRATE) == pScrollBar)
	{
		SetDlgItemInt(IDC_LIPIDDEATHRATEDISPLAY,nPos,FALSE); //Set the edit box to the new value of pos(*10)
		LipidRemovalRate = (nPos*65535)/10000;	//And set the global variable as well
	}


	pScrollBar->SetScrollPos (nPos,TRUE);	//This is necessary again because when the edit boxes are modified,
	//they set the scrollbar to their new value!

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SimulationSettings::OnChangeNoofmacrohittingwalldisplay() 
{
	// TODO: Add your control notification handler code here
		UpdateData ();
		m_NumMacroScroll.SetScrollPos (m_NumMacrophageContactsPerCycle/10,TRUE);
			
}

void SimulationSettings::OnChangeNoofsmchittingwalldisplay() 
{

	// TODO: Add your control notification handler code here
	UpdateData ();
	m_NumSMCScroll.SetScrollPos (m_NumSMCContactsPerCycle/10,TRUE);
}



CString RecordDataFileName;

void SimulationSettings::OnRecordsimulationinfo() 
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked (IDC_RECORDSIMULATIONINFO))
	{
		CFileDialog FileDialog (FALSE,"csv",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Data File (*.csv)|*.csv" );
		FileDialog.m_ofn.lpstrTitle = "Select file to record data";
		FileDialog.m_ofn.lpstrInitialDir = ".\\Recordings";
		if (FileDialog.DoModal () == IDOK)
			RecordDataFileName = FileDialog.GetPathName ();
		else
			CheckDlgButton (IDC_RECORDSIMULATIONINFO,BST_UNCHECKED);
	}
	else
		RecordDataFileName.Empty ();;

}

void SimulationSettings::OnHelp() 
{
	// TODO: Add your control notification handler code here
	
	WinHelp (0,HELP_CONTENTS);
}


void SimulationSettings::OnChangeSmcmacrokillrate() 
{
	UpdateData (TRUE);
	SMCMacroKillRate = m_SMCMacroKillRate;
}

void SimulationSettings::OnChangeLipidarrivalratedisplay() 
{
	UpdateData ();
	m_LipidScroll.SetScrollPos (m_LipidArrivalRate,TRUE);	
	LipidArrivalRate = (m_LipidArrivalRate *65535)/100;
}

void SimulationSettings::OnChangeLipiddeathratedisplay() 
{
	UpdateData ();
	m_LipidRemovalScroll.SetScrollPos (m_LipidRemovalRate,TRUE);	
	LipidRemovalRate = (m_LipidRemovalRate * 65535)/2000;
		
}

void SimulationSettings::OnColours() 
{
	ColourDialog dlg;
	dlg.DoModal (); //FIXME: use ok cancel
}

