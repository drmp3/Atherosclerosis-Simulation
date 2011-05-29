// MainWindow.cpp : implementation file
//

#include "stdafx.h"
#include "athero.h"
#include "ddrawx.h"
#include "resource.h"
#include "MainWindow.h"
#include "SimulationInfo.h"
#include "SplashScreenDlg.h"
#include "XSectionDisplayDlg.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


long Filesize(FILE *stream);
char* FindStringAndEnterVariable (char* FileBuffer,char *String,int *Variable);
char* FindStringAndEnterVariable (char* FileBuffer,char *String,float *Variable);

//Global variables here
char AtheroName []= "Atherosclerosis Simulation";
HINSTANCE AtheroInstance;
HWND AtheroWindow,SettingsDialogWindow;
BOOL SettingsDialogInitialised = FALSE;
char *ListOfDemoCommands[256];
int DemoCommandTimes [256];
int NoofFiles;
int CurrentDemoFile;
UINT TimerId;
HDC AtheroWindowDC;
BOOL DemoRunning = 0;
SimulationSettings SimulationSettingsDialog;
SimulationInfo SimulationInfoDialog;
bool g_LeftPressed;
bool g_RightPressed;

/////////////////////////////////////////////////////////////////////////////
// CMainWindow dialog


CMainWindow::CMainWindow(CWnd* pParent /*=NULL*/)
	: CDialog(CMainWindow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	XSectionMode = false;
	
	Adjust3DView = false;
	Adjust3DZoom = false;
	
	XSectionStart.x = -1;
	pCurrentDisplayGraph = NULL;
	DrawMacrophages = 1;
	DrawSMCs = 1;
	DrawLipids = 1;
	View3D = 0;
    GameMode = 0;

    char mname[512];
    GetModuleFileName(NULL, mname, 512);
    if (strstr(mname, "ame")) //search for Game, ignore potential caps issue
    {
        GameMode = 1;
        View3D=1;
    }
    
}


void CMainWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainWindow, CDialog)
	//{{AFX_MSG_MAP(CMainWindow)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(CM_MENUHELP, OnMenuhelp)
	ON_COMMAND(CM_XSECTION, OnXsection)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_COMMAND(CM_ABOUT, OnAbout)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CANCELMODE()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainWindow message handlers

LRESULT CMainWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	DDHandleMessages (m_hWnd,message,wParam,lParam);
	return CDialog::WindowProc(message, wParam, lParam);
}


BOOL CMainWindow::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch (wParam)
	{
		case CM_START:
			//If 'START!' menu item clicked, then start the simulation!
			//But first make the 'START!' menu item read 'Pause'
			//And add 'Restart' to the menu

//			InitDirectDraw (CELL_BUFFER_X_SIZE,CELL_BUFFER_Y_SIZE,8,AtheroWindow,FULLSCREEN_MODE);		//Initialise the fast-drawing graphics system
			ModifyMenu (::GetMenu (m_hWnd),CM_START,MF_BYCOMMAND|MF_STRING,CM_PAUSE,"Pause");
			InsertMenu (::GetMenu (m_hWnd),CM_INFO,MF_BYCOMMAND|MF_STRING,CM_RESTART,"Restart");
			SimulationGo = 1;
            if (!GameMode)
			    SimulationInfoDialog.ShowWindow (SW_SHOWNOACTIVATE);
			AtheroWindow = m_hWnd;
			StartSimulation ();
			break;
		case CM_SETTINGS:
			SimulationSettingsDialog.ShowWindow (SW_RESTORE);
			break;
		case CM_PAUSE:
			ModifyMenu (::GetMenu (m_hWnd),CM_PAUSE,MF_BYCOMMAND|MF_STRING,CM_RESUME,"Resume");
			PauseSimulation ();
			break;
		case CM_RESUME:
			ModifyMenu (::GetMenu (m_hWnd),CM_RESUME,MF_BYCOMMAND|MF_STRING,CM_PAUSE,"Pause");
			ResumeSimulation ();
			break;
		case CM_RESTART:
			if (DemoRunning)	//If the demo's running, stop it
				StopDemo ();
			ModifyMenu (::GetMenu (m_hWnd),CM_RESUME,MF_BYCOMMAND|MF_STRING,CM_PAUSE,"Pause");
			EndSimulation ();
			SimulationGo =1;		//Then restart the simulation
			StartSimulation ();
			break;
		case ATHERO_DRAWSMC:
			if (DrawSMCs) {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWSMC, MF_UNCHECKED);
				DrawSMCs = 0;
			}
			else {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWSMC, MF_CHECKED);
				DrawSMCs = 1;
			}
			break;
		case ATHERO_DRAWMACRO:
			if (DrawMacrophages) {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWMACRO, MF_UNCHECKED);
				DrawMacrophages = 0;
			}
			else {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWMACRO, MF_CHECKED);
				DrawMacrophages = 1;
			}
			break;
		case ATHERO_DRAWLIPIDS:
			if (DrawLipids) {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWLIPIDS, MF_UNCHECKED);
				DrawLipids = 0;
			}
			else {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWLIPIDS, MF_CHECKED);
				DrawLipids = 1;
			}
			break;
		case IDC_3DVIEW:
			if (View3D) {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWLIPIDS, MF_UNCHECKED);
				View3D = 0;
				ShutDown3D ();
			}
			else {
				GetMenu ()->CheckMenuItem (ATHERO_DRAWLIPIDS, MF_CHECKED);
				View3D = 1;
				Initialise3D ();
			}
			break;
        case IDOK:
            if (GameMode)
                return 0;
            break;



	}

	return CDialog::OnCommand(wParam, lParam);
}

void InitGraphsAndSettings (void)
{
	 InitMacroArrivalFuncInfo ();
	 InitMacroDeathFuncInfo ();
	 InitSMCArrivalFuncInfo ();
	 LoadSettingsFromFile (AtheroWindow,"default.csv");
}
/*

BOOL CALLBACK SettingsDialog_DialogProc (HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
//This message handler has the number that represents the message in uMsg, wParam and lParam are two
//paramaters that mean different things for different messages (wParam is 16 bits, lParam is 32)
	switch (uMsg)
	{
		HANDLE_MSG (hwndDlg,WM_HSCROLL,SettingsDialog_OnHScroll);
		HANDLE_MSG (hwndDlg,WM_COMMAND,SettingsDialog_OnCommand);
		case WM_DESTROY:
			return TRUE;
	}
	return FALSE;
}

void SettingsDialog_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
if (code != SB_THUMBTRACK)
	pos = ScrollBar_GetPos (hwndCtl); //Only if the scrollbar is 'thumbtracking' (dragging the button across)
	//does pos equal the new position of the scrollbar, so if we are thumbtracking, we do not need to set
	//pos to the position of the scrollbar

	//Below says 'If you clicked the right arrow on the scrollbar, then pos goes up by one, if you
	//clicked the left, then down by one, etc

	switch (code)
	{
		case SB_LINERIGHT:
			pos++;
			break;
		case SB_LINELEFT:
			pos--;
			break;
		case SB_PAGELEFT:
			pos-=50;
			break;
		case SB_PAGERIGHT:
			pos+=50;
			break;
	}

	ScrollBar_SetPos (hwndCtl,pos,TRUE);
	pos = ScrollBar_GetPos (hwndCtl);	//This is an easy way of making sure 'pos' is in acceptable limits

	if (GetDlgItem (hwnd,IDC_NOOFMACROHITTINGWALLSCROLL) == hwndCtl)
	{

	//The if statment says 'if the macrophage arrival rate scroll bar has moved...'
	//pos is the new position of the scrollbar
		SetDlgItemInt(hwnd,IDC_NOOFMACROHITTINGWALLDISPLAY,pos*10,FALSE);	//This sets the text box beneath the
									//scrollbar to read the new number of cell contacts per cycle
		NumMacrophageContactsPerCycle = pos*10;	//And set the global variable as well
	}
	else if (GetDlgItem (hwnd,IDC_NOOFSMCHITTINGWALLSCROLL) == hwndCtl)
	{
		//The if statment says 'if the SMC arrival rate scroll7 bar has moved...'
		SetDlgItemInt(hwnd,IDC_NOOFSMCHITTINGWALLDISPLAY,pos*10,FALSE); //Set the edit box to the new value of pos(*10)
		NumSMCContactsPerCycle = pos*10;	//And set the global variable as well
	}
	else if (GetDlgItem (hwnd,IDC_LIPIDARRIVALRATE) == hwndCtl)
	{
		SetDlgItemInt(hwnd,IDC_LIPIDARRIVALDISPLAY,pos/300,FALSE); //Set the edit box to the new value of pos(*10)
		LipidArrivalRate = pos/300;	//And set the global variable as well
	}
	else if (GetDlgItem (hwnd,IDC_LIPIDREMOVALRATE) == hwndCtl)
	{
		SetDlgItemInt(hwnd,IDC_LIPIDDEATHRATEDISPLAY,pos/300,FALSE); //Set the edit box to the new value of pos(*10)
		LipidRemovalRate = pos/300;	//And set the global variable as well
	}

	ScrollBar_SetPos (hwndCtl,pos,TRUE);	//This is necessary again because when the edit boxes are modified,
	//they set the scrollbar to their new value!
}

void SettingsDialog_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

}

*/

void LoadSettings (HWND hwnd)
{
	//Here a 'Save Settings' Dialog box is created, using one of windows' 'Common Dialog box' functions
	//which creates a save dialog box and returns the filename the user selected
	  BOOL Result;
	  OPENFILENAME LoadDialogBoxInfo;	//This structure contains all the information to put the dialog box
	  //on the screen
	  LoadDialogBoxInfo.lStructSize = sizeof (OPENFILENAME);
	  LoadDialogBoxInfo.hwndOwner = hwnd;
	  LoadDialogBoxInfo.hInstance = AtheroInstance;
	  LoadDialogBoxInfo.lpstrFilter = "Simulation Settings (*.csv)\0*.CSV\0";
	  LoadDialogBoxInfo.lpstrCustomFilter = NULL;
	  LoadDialogBoxInfo.nFilterIndex = 0;
	  LoadDialogBoxInfo.lpstrFile = new char [256];
	  LoadDialogBoxInfo.nMaxFile = 256;
	  FillMemory (LoadDialogBoxInfo.lpstrFile,256,0);
	  LoadDialogBoxInfo.lpstrFileTitle = NULL;
	  LoadDialogBoxInfo.lpstrInitialDir = NULL;
	  LoadDialogBoxInfo.lpstrTitle = "Load Simulation Settings";
	  LoadDialogBoxInfo.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	  LoadDialogBoxInfo.lpstrDefExt = "SET";
	  Result = GetOpenFileName (&LoadDialogBoxInfo);
	  if (!Result)
			return;
	  LoadSettingsFromFile (hwnd,LoadDialogBoxInfo.lpstrFile);
}

void LoadSettingsFromFile (HWND hwnd,char *LoadFilename)
{
	  FILE *FileHandle = fopen (LoadFilename,"r");
	  if (!FileHandle)
	  {
			MessageBox (hwnd,"Unable to open file","Load file failed!",MB_OK);
			return;
	  }
	  int FileSize = Filesize (FileHandle);
	  char *FileBuffer = new char [FileSize+1];
	  char *OrgFileBuffer = FileBuffer;
	  FileBuffer[FileSize]=0;
	  fread ((void*)FileBuffer,FileSize,1,FileHandle);
	  fclose (FileHandle);
	  float Version;
	  if (memcmp ((void*)FileBuffer,(void*)"Atherosclerosis Simulation Settings,\n",strlen ("Atherosclerosis Simulation Settings,\n")))
	  {
			MessageBox (hwnd,"Not an Atherosclerosis Simulation Settings file","Error!",MB_ICONEXCLAMATION);
			return;
	  }
	  FindStringAndEnterVariable (FileBuffer,"VERSION,",&Version);
	  if (Version < (float)VERSION)
	  {
		  MessageBox (hwnd,"Data file is from a previous version\nIgnore following warnings and save new data file\n","Version Incorrect",MB_ICONINFORMATION);
	  }
	  else if (Version != (float) VERSION)
	  {
		  MessageBox (hwnd,"This data file is from a newer version of this program.\n Cannot open data file.","Version Incorrect!",MB_ICONEXCLAMATION);
		  return;
	  }  

	  
	  FindStringAndEnterVariable (FileBuffer,"ARTERY_RED", &ARTERY_RED);
	  FindStringAndEnterVariable (FileBuffer,"ARTERY_GREEN", &ARTERY_GREEN);
	  FindStringAndEnterVariable (FileBuffer,"ARTERY_BLUE", &ARTERY_BLUE);

	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_MACROS_R",&COLOUR_FOR_MAX_NO_MACROS_R);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_MACROS_G",&COLOUR_FOR_MAX_NO_MACROS_G);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_MACROS_B",&COLOUR_FOR_MAX_NO_MACROS_B);
	  
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_MACRO_R",&COLOUR_FOR_1_MACRO_R);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_MACRO_G",&COLOUR_FOR_1_MACRO_G);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_MACRO_B",&COLOUR_FOR_1_MACRO_B);
	  
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_SMCS_R",&COLOUR_FOR_MAX_NO_SMCS_R);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_SMCS_G",&COLOUR_FOR_MAX_NO_SMCS_G);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_SMCS_B",&COLOUR_FOR_MAX_NO_SMCS_B);
	  
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_SMC_R",&COLOUR_FOR_1_SMC_R);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_SMC_G",&COLOUR_FOR_1_SMC_G);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_SMC_B",&COLOUR_FOR_1_SMC_B);
	  
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_LIPIDS_R",&COLOUR_FOR_MAX_NO_LIPIDS_R);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_LIPIDS_G",&COLOUR_FOR_MAX_NO_LIPIDS_G);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_MAX_NO_LIPIDS_B",&COLOUR_FOR_MAX_NO_LIPIDS_B);
	  
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_LIPID_R",&COLOUR_FOR_1_LIPID_R);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_LIPID_G",&COLOUR_FOR_1_LIPID_G);
	  FindStringAndEnterVariable (FileBuffer,"COLOUR_FOR_1_LIPID_B",&COLOUR_FOR_1_LIPID_B);

	  FindStringAndEnterVariable (FileBuffer,"RateOfMacrophagesHittingArtery,",&NumMacrophageContactsPerCycle);
	  FindStringAndEnterVariable (FileBuffer,"SMCMacroInhibition,",&SMCMacroKillRate);
	  FindStringAndEnterVariable (FileBuffer,"LipidArrivalRate,",&LipidArrivalRate);
	  FindStringAndEnterVariable (FileBuffer,"LipidRemovalRate,",&LipidRemovalRate);
	  FileBuffer = FindStringAndEnterVariable (FileBuffer,"RateOfSMCHittingArtery,",&NumSMCContactsPerCycle);

	  int i;
	  char xValueString [20];

	  for (i=0;i<=NUM_DEATH_GRAPH_POINTS;i++)
	  {
			sprintf (xValueString,"%d,",i*MAX_MACROPHAGES_PER_PIXEL*9/NUM_DEATH_GRAPH_POINTS);
			FileBuffer = FindStringAndEnterVariable (FileBuffer,xValueString,&MacroDeathFuncInfo.yValuesDataForGraph[i]);
	  }

	  for (i=0;i<=NUM_ARRIVAL_GRAPH_POINTS;i++)
	  {
			sprintf (xValueString,"%d,",i*MAX_MACROPHAGES_PER_PIXEL*9/NUM_ARRIVAL_GRAPH_POINTS);
			FileBuffer = FindStringAndEnterVariable (FileBuffer,xValueString,&MacroArrivalFuncInfo.yValuesDataForGraph[i]);
	  }

	  

	  for (i=0;i<=NUM_ARRIVAL_GRAPH_POINTS;i++)
	  {
			sprintf (xValueString,"%d,",i*MAX_MACROPHAGES_PER_PIXEL*9/NUM_ARRIVAL_GRAPH_POINTS);
			FileBuffer = FindStringAndEnterVariable (FileBuffer,xValueString,&SMCArrivalFuncInfo.yValuesDataForGraph[i]);
	  }
	  GenerateyValuesForEveryxValue (&MacroArrivalFuncInfo);
	  GenerateyValuesForEveryxValue (&MacroDeathFuncInfo);
	  GenerateyValuesForEveryxValue (&SMCArrivalFuncInfo);
	  delete [] OrgFileBuffer;
}

char* FindStringAndEnterVariable (char* FileBuffer,char *String,int *Variable)
{
	//This searches 'FileBuffer' for 'String', and enters the number following 'String' into 'Variable'
	char*FoundStringPosition = strstr (FileBuffer,String);
	if (!FoundStringPosition)
	{
		MessageBox (SettingsDialogWindow,String,"Could not find variable in file (File from wrong version or corrupted)",MB_OK);
		return NULL;
	}
	FoundStringPosition+=strlen (String);
	sscanf (FoundStringPosition,"%d",Variable);
	return FoundStringPosition;
}

char* FindStringAndEnterVariable (char* FileBuffer,char *String,float *Variable)
{
	//This searches 'FileBuffer' for 'String', and enters the number following 'String' into 'Variable'
	char*FoundStringPosition = strstr (FileBuffer,String);
	if (!FoundStringPosition)
	{
		MessageBox (SettingsDialogWindow,String,"Could not find variable in file",MB_OK);
		return NULL;
	}
	FoundStringPosition+=strlen (String);
	sscanf (FoundStringPosition,"%f",Variable);
	return FoundStringPosition;
}





long Filesize(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}




void RunDemo (HWND hwnd)
{
	  OPENFILENAME LoadDialogBoxInfo;	//This structure contains all the information to put the dialog box
	  //on the screen

	  //All the items below are for a standard 'open file' common dialog box - consult on line help for more information
	  LoadDialogBoxInfo.lStructSize = sizeof (OPENFILENAME);
	  LoadDialogBoxInfo.hwndOwner = hwnd;
	  LoadDialogBoxInfo.hInstance = AtheroInstance;
	  LoadDialogBoxInfo.lpstrFilter = "Simulation Demo Lists\0*.DMO\0";
	  LoadDialogBoxInfo.lpstrCustomFilter = NULL;
	  LoadDialogBoxInfo.nFilterIndex = 0;
	  LoadDialogBoxInfo.lpstrFile = new char [256];
	  LoadDialogBoxInfo.nMaxFile = 256;
	  FillMemory (LoadDialogBoxInfo.lpstrFile,256,0);
	  LoadDialogBoxInfo.lpstrFileTitle = NULL;
	  LoadDialogBoxInfo.lpstrInitialDir = NULL;
	  LoadDialogBoxInfo.lpstrTitle = "Load Simulation Demo List";
	  LoadDialogBoxInfo.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	  LoadDialogBoxInfo.lpstrDefExt = "DMO";
	  int Result = GetOpenFileName (&LoadDialogBoxInfo); //This puts the open dialog box on the screen and waits for user input
												//when the user has chosen a file, the LoadDialogBoxInfo.lpstrFile
												//is a pointer to the complete filename
	  if (!Result)
			return;
			//The line below opens the file, using the filename from the dialog box and "r" for read access
	  FILE *FileHandle = fopen (LoadDialogBoxInfo.lpstrFile,"r");
	  //FileHandle is the handle for the file to be used with the fread command
	  if (!FileHandle)
	  {
			MessageBox (hwnd,"Could not open file","Load Demo list failed",MB_OK);
			return;
	  }
	  // DemoList is defined as a string the size of the demo list file and has memory allocated accordingly
	  char *DemoList = new char [Filesize (FileHandle)];

	  //The DemoList file is read into the DemoList string
	  fread ((void*)DemoList,1,Filesize (FileHandle),FileHandle);
	  //And the file is closed
	  fclose (FileHandle);

	  float Version;
	  //The version number is read from the DemoList string and compared with the current version number
	  FindStringAndEnterVariable (DemoList,"VERSION:\n",&Version);
	  if (Version != VERSION)
	  {
	  //if the version is incorrect, the routine aborts
			MessageBox (hwnd,"Incorrect file or file version","Error!",MB_OK);
			return;
	  }

	  //NoofFiles will store the number of files in the demo list..
	  FindStringAndEnterVariable (DemoList,"NUMBER OF COMMANDS:\n",&NoofFiles);//...and is read in by this line
	  int i;
	  for (i=0;i<NoofFiles;i++)
		  ListOfDemoCommands[i] = ::new char[256];	//ListOfDemoCommands is a pointer to an array of string pointers
	//how this will work is that ListOfDemoCommands [0] will be a pointer to the first file,
							//ListODF [1] will be the second, etc.
	  char *PointerToReadInFiles = strstr (DemoList,"LIST OF COMMANDS:\n")+strlen ("LIST OF COMMANDS:\n");
	  //PointerToRadInFiles points to the beginning of the list of files in the Demo List file

	  for (i=0;i<NoofFiles;i++)
	  {
		  //First, read in the filename from the string by using scanf, while at the same time reading
		  //in the time that file will run for
		  sscanf (PointerToReadInFiles,"%s %d",ListOfDemoCommands [i],&DemoCommandTimes[i]);
		  PointerToReadInFiles = strchr (PointerToReadInFiles,'\n')+1;  //Set the pointer to the next file
	  }
	  delete (DemoList); //Free up the memory from the loaded demo list file

	  EndDialog (SettingsDialogWindow,0);	//Close down the settings dialog box
	  CurrentDemoFile = 0;			//Current Demo file is the index to the file currently in use
	  LoadSettingsFromFile (hwnd,ListOfDemoCommands [CurrentDemoFile]);	//Load up the settings from that file in the demo

	  if (SimulationGo==1)	//If the simulation is going, then end it...
			EndSimulation ();
	  SimulationGo=1;
	  StartSimulation ();	//...And then restart it.

	  TimerId = SetTimer (NULL,	//This creates a timer which will call the DemoTimerProc function in DemoFileTimes [CDF] seconds time
					0,		//this function will reset the simulation and start the next file in the demo list
					1000*DemoCommandTimes [CurrentDemoFile],
					(TIMERPROC) DemoTimerProc
					);
	  DemoRunning = 1;
}

VOID CALLBACK DemoTimerProc(HWND  hwnd,UINT  uMsg,UINT  idEvent,DWORD  dwTime)
{
	//When this routine is called, it is time to switch to the next file in the demo list
	EndSimulation ();	//First end the simulation
	SimulationGo =1;	//reset the simulation's go flag
	CurrentDemoFile++;	//increment the current demo file were on
	if (CurrentDemoFile==NoofFiles)
		CurrentDemoFile = 0;
	LoadSettingsFromFile (hwnd,ListOfDemoCommands[CurrentDemoFile]);	//load the settings from this file
	StartSimulation ();	//restart the simulation
	KillTimer (NULL,TimerId);	//remove the old timer
	TimerId = SetTimer (NULL,	//and set the new timer to call this routine in however many seconds time is required
				0,
				1000*DemoCommandTimes [CurrentDemoFile],
				(TIMERPROC) DemoTimerProc
				);
}

void StopDemo (void)
{
	DemoRunning = 0;
	KillTimer (NULL,TimerId);	//stop the demo's timer from working any more
	 int i;
	 for (i=0;i<NoofFiles;i++)
		delete ListOfDemoCommands[i];	//free up the memory for all of the commands

}

int CMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_hIcon = theApp.LoadIcon (IDI_APP);

	SetIcon (m_hIcon,true);
	

	
	AtheroWindow = m_hWnd;

 	SetClassLong (m_hWnd,GCL_HBRBACKGROUND,NULL);

	ShowWindow (SW_SHOWNORMAL);    	

    if (GameMode)
    {                
        SetWindowPos (NULL, 0,0, 800, 600, 0);                
    }
    else
    {
        SetWindowPos (NULL, 0,0, 640, 480, 0);
    }

	
	


	/*
	RECT rect;
	
	SetWindowPos (&wndTop,10,10,CELL_BUFFER_X_SIZE,CELL_BUFFER_Y_SIZE,SWP_NOREDRAW );

	GetClientRect (&rect);

	SetWindowPos (&wndTop,10,10,2*CELL_BUFFER_X_SIZE-rect.right,2*CELL_BUFFER_Y_SIZE-rect.bottom,SWP_SHOWWINDOW );

  */

	InitDirectDraw (CELL_BUFFER_X_SIZE,CELL_BUFFER_Y_SIZE,8,m_hWnd,WINDOWED_MODE);		//Initialise the fast-drawing graphics system

	

	// TODO: Add your specialized creation code here
	InitGraphsAndSettings ();
	RegisterFunctionAdjusterWindow ();
    if (!GameMode)
	    SimulationSettingsDialog.Create ();

	SimulationInfoDialog.Create ();
	SimulationInfoDialog.ShowWindow (SW_HIDE);

    
    if (!GameMode)
	    FillBuffers((ARTERY_BLUE)|((ARTERY_GREEN)<<8)|((ARTERY_RED)<<16));

	//ShowWindow (SW_SHOWMAXIMIZED);

    if (!GameMode)
    {
	    CSplashScreenDlg StartupDlg;
	    StartupDlg.DoModal ();
    }

	if (View3D)
		Initialise3D ();

    if (GameMode)
    {
        SetMenu(NULL);
        //GetMenu()->DestroyMenu();
        SimulationGo = 1;
     	AtheroWindow = m_hWnd;
		StartSimulation ();
    }


	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;




	return 0;
}

void CMainWindow::OnDestroy() 
{
	DeInitDirectDraw ();
	CDialog::OnDestroy();
	
}

void CMainWindow::OnClose() 
{
	EndSimulation ();
	CDialog::OnClose();
}

void CMainWindow::OnMenuhelp() 
{
	WinHelp (0,HELP_CONTENTS);
}

void CMainWindow::OnXsection() 
{
	if (MacrophageDataBuffer)	//simulation must have started to do this
	{
		SetClassLong (m_hWnd,GCL_HCURSOR,(DWORD)LoadCursor (NULL,IDC_CROSS));
		XSectionMode = true;
	}
}

void CMainWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (XSectionMode)	//simulation must have started
		XSectionStart = point;

		
	CDialog::OnLButtonDown(nFlags, point);
}

void CMainWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
	

	if (View3D && !Adjust3DView && !GameMode) {
		Adjust3DView = TRUE;
		Adjust3DStart = point;
		ShowCursor(FALSE);
		//PauseSimulation ();
	}
	else if (View3D && !GameMode) {
		Adjust3DView = FALSE;
		ShowCursor(TRUE);
		//ResumeSimulation ();
	}

	if (XSectionMode)
	{
		RECT rect;
		rect.left = min (XSectionStart.x,XSectionEnd.x)-1;
		rect.top = min (XSectionStart.y,XSectionEnd.y)-1;

		rect.right = max (XSectionStart.x,XSectionEnd.x)+1;
		rect.bottom = max (XSectionStart.y,XSectionEnd.y)+1;
		
		InvalidateRect (&rect,false);

		XSectionMode = false;		
	
		SetClassLong (m_hWnd,GCL_HCURSOR,(DWORD)LoadCursor (NULL,IDC_ARROW));
		if  ( ((XSectionStart.x-point.x)*(XSectionStart.x-point.x) + (XSectionStart.y-point.y)*(XSectionStart.y-point.y)) > 200)
		{
			CXSectionDisplayDlg Dlg (this,&XSectionStart,&point);			
			XSectionStart.x = -1;
			Dlg.DoModal ();
		}		
		XSectionStart.x = -1;
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CMainWindow::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (XSectionMode && XSectionStart.x != -1 && !SimulationGo)
	{
		RECT rect;
		rect.left = min (XSectionStart.x,XSectionEnd.x)-1;
		rect.top = min (XSectionStart.y,XSectionEnd.y)-1;

		rect.right = max (XSectionStart.x,XSectionEnd.x)+1;
		rect.bottom = max (XSectionStart.y,XSectionEnd.y)+1;
		
		InvalidateRect (&rect,false);
		UpdateWindow();

		CDC *pWindowDC = GetDC();
		HPEN hPen = CreatePen (PS_DOT,0,RGB (255,255,255));
		pWindowDC->SelectObject (hPen);
		pWindowDC->MoveTo(XSectionStart);
		pWindowDC->LineTo(point);
		pWindowDC->DeleteDC ();
		DeleteObject (hPen);
	}
	XSectionEnd = point;

	if (View3D && (Adjust3DView || Adjust3DZoom))
	{
		CPoint MouseDelta = CPoint(point) - Adjust3DStart;
		if (Adjust3DView)
			Do3DViewPosAdjustment (MouseDelta.x, MouseDelta.y);
		else
			Do3DZoomAdjustment (MouseDelta.x, MouseDelta.y);

		Adjust3DStart = point;
        
		
		//POINT screenP = Adjust3DStart;
		//ClientToScreen (&screenP);
		//SetCursorPos (screenP.x, screenP.y);
		//InvalidateRect (NULL,false);
		//UpdateWindow();

	}

	CDialog::OnMouseMove(nFlags, point);
}

void CMainWindow::OnPaint() 
{
	DDPaintWindow (m_hWnd);

	if (XSectionMode && XSectionStart.x != -1 && SimulationGo)
	{
		InvalidateRect (NULL, NULL);
		CPaintDC WindowDC (this);
		HPEN hPen = CreatePen (PS_DOT,0,RGB (255,255,255));
		WindowDC.SelectObject (hPen);
		WindowDC.MoveTo(XSectionStart);
		WindowDC.LineTo(XSectionEnd);
		//leteObject (hPen);
	}
		
}

void CMainWindow::OnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal ();
}

void CMainWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (View3D && !GameMode) {
		Adjust3DZoom = TRUE;
		Adjust3DStart = point;
		ShowCursor(FALSE);
	}
	CDialog::OnRButtonDown(nFlags, point);
}

void CMainWindow::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (View3D && !GameMode) {
		Adjust3DZoom = FALSE;
		Adjust3DStart = point;
		ShowCursor(TRUE);
	}
	CDialog::OnRButtonUp(nFlags, point);
}


BOOL CMainWindow::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE))
            pMsg->wParam = VK_TAB;
    }
    return CDialog::PreTranslateMessage(pMsg);
}
