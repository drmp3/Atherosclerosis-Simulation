#include "athero.h"		//General definitions for the program

#include "ddrawx.h"
#pragma hdrstop
#include "resource.h"		//Definitions for the 'resources' - the icons and the dialog box
								//that the program uses
#include "SimulationInfo.h"


LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,WPARAM wParam, LPARAM lParam);
BOOL Athero_OnCreate (HWND hwnd,CREATESTRUCT FAR *lpCreateStruct);
void Athero_OnDestroy (HWND hwnd);
void Athero_OnPaint (HWND hwnd);
void Athero_OnCommand (HWND hwnd,int id, HWND hwndCtl, UINT codeNotify);

long Filesize(FILE *stream);
void FindStringAndEnterVariable (char* FileBuffer,char *String,int *Variable);
void FindStringAndEnterVariable (char* FileBuffer,char *String,float *Variable);

//Global variables here
char AtheroName []= "Atherosclerosis Simulation";
HINSTANCE AtheroInstance;
HWND AtheroWindow,SettingsDialogWindow,MacroArrivalGraphWindow,MacroDeathGraphWindow,SMCArrivalGraphWindow;
BOOL SettingsDialogInitialised = FALSE;
DWORD SimulationThreadID;
HANDLE SimulationThreadHandle;
char *ListOfDemoCommands[256];
int DemoCommandTimes [256];
int NoofFiles;
int CurrentDemoFile;
UINT TimerId;
HDC AtheroWindowDC;
SimulationInfo SimulationInfoDialog;
BOOL DemoRunning = 0;

/*int WINAPI WinMain (HINSTANCE hCurrent_instance,HINSTANCE hPrevious_Instance, LPSTR lpCmdLine, int nCmdShow)
{
//This is where the program starts
	MSG Incoming_message;
	AtheroInstance = hCurrent_instance;

	//Register the window classes:
	//First the main simulation window class - just acts like a normal window
	if (!RegisterAtheroWindow ())
			return FALSE;
	//And then the Kill Rate/Arrival Chances Function adjuster window type
	if (!RegisterFunctionAdjusterWindow ())
			return FALSE;

	AtheroWindow = CreateAtheroWindow ();
	AtheroWindowDC = GetDC (AtheroWindow);
	if (!AtheroWindow)
		return FALSE;
	//Use this window class to create a window

	SimulationInfoDialog.Create ();

	ShowWindow (AtheroWindow,SW_MAXIMIZE);		//Make the window maximized (as big as possible)
	UpdateWindow (AtheroWindow);		//and redraw it

	InitDirectDraw (CELL_BUFFER_X_SIZE,CELL_BUFFER_Y_SIZE,8,AtheroWindow,WINDOWED_MODE);		//Initialise the fast-drawing graphics system

	InitGraphsAndSettings ();

	CreateSettingsDialogBox ();	//put the settings dialog box on the screen

	//Wait for incoming messages to the window
	while (GetMessage (&Incoming_message,NULL,0,0))
	{
		if (!IsDialogMessage (SettingsDialogWindow,&Incoming_message))
			{
				TranslateMessage (&Incoming_message);
				//And when they come, despatch them to the 'WndProc' routine that handles messages
				DispatchMessage (&Incoming_message);
			}
	}
	ReleaseDC (AtheroWindow,AtheroWindowDC);
	
	return Incoming_message.wParam;
}
*/
BOOL RegisterAtheroWindow (void)
{
//This registers a new window class in the name of 'Atherosclerosis Simulation'
//When this class is used to create a window, the window will have some of its
//properties determined by the code below
	WNDCLASS New_window;
	New_window.style = CS_HREDRAW | CS_VREDRAW;
	New_window.lpfnWndProc = WndProc;
	New_window.cbClsExtra = 0;
	New_window.cbWndExtra = 0;
	New_window.hInstance= AtheroInstance;
	New_window.hIcon = LoadIcon (NULL,IDI_APPLICATION);
	New_window.hCursor = LoadCursor (NULL,IDC_ARROW);
	New_window.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	New_window.lpszMenuName = MAKEINTRESOURCE (ATHERO_MENU);		//Attach the ATHER_MENU (in resource file) to the window
	New_window.lpszClassName= AtheroName;
	return (RegisterClass (&New_window) != 0);
}

HWND CreateAtheroWindow (void)
{
//This routine creates the simulation window
	return ( CreateWindow (AtheroName,AtheroName,
	MAINWINDOW_FLAGS,MAINWINDOW_L,MAINWINDOW_T,MAINWINDOW_R,MAINWINDOW_B,
	NULL,NULL,AtheroInstance,NULL) );
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,WPARAM wParam, LPARAM lParam)
{
	DDHandleMessages (hwnd,Message,wParam,lParam);	//Pass the messages on to DirectDraw so direct draw can
						//handle drawing and moving the window
	switch (Message)
	{
		HANDLE_MSG (hwnd, WM_CREATE,Athero_OnCreate);
		HANDLE_MSG (hwnd, WM_DESTROY,Athero_OnDestroy);
		HANDLE_MSG (hwnd, WM_COMMAND,Athero_OnCommand);
		default:
			return Athero_DefProc (hwnd,Message,wParam,lParam);
	}
}


BOOL Athero_OnCreate (HWND hwnd,CREATESTRUCT FAR * lpCreateStruct)
{
	return TRUE;
}



void Athero_OnDestroy (HWND hwnd)
{
	PostQuitMessage (0);
}

void Athero_OnCommand (HWND hwnd,int id, HWND hwndCtl, UINT codeNotify)
{
//We arrive here if any of the buttons or menu items are clicked on the main simulation window
		switch (id)
	{
		case CM_START:
			//If 'START!' menu item clicked, then start the simulation!
			//But first make the 'START!' menu item read 'Pause'
			//And add 'Restart' to the menu
//			InitDirectDraw (CELL_BUFFER_X_SIZE,CELL_BUFFER_Y_SIZE,8,AtheroWindow,FULLSCREEN_MODE);		//Initialise the fast-drawing graphics system
			SimulationGo = 1;
			StartSimulation ();
			break;
		case CM_SETTINGS:
			CreateSettingsDialogBox ();
			break;
		case CM_PAUSE:
			ModifyMenu (GetMenu (hwnd),CM_PAUSE,MF_BYCOMMAND|MF_STRING,CM_RESUME,"Resume");
			PauseSimulation ();
			break;
		case CM_RESUME:
			ModifyMenu (GetMenu (hwnd),CM_RESUME,MF_BYCOMMAND|MF_STRING,CM_PAUSE,"Pause");
			ResumeSimulation ();
			break;
		case CM_SIMULATIONBREAK:
			BreakSimulation = 1;
			break;
		case CM_RESTART:
			if (DemoRunning)	//If the demo's running, stop it
				StopDemo ();
			ModifyMenu (GetMenu (hwnd),CM_RESUME,MF_BYCOMMAND|MF_STRING,CM_PAUSE,"Pause");
			EndSimulation ();
			SimulationGo =1;		//Then restart the simulation
			StartSimulation ();
			break;
	}

}

void InitGraphsAndSettings (void)
{
	 InitMacroArrivalFuncInfo ();
	 InitMacroDeathFuncInfo ();
	 InitSMCArrivalFuncInfo ();
	 LoadSettingsFromFile (AtheroWindow,"default.set");
}


void 	CreateSettingsDialogBox (void)
{

	if (!SettingsDialogInitialised)	//If the dialog box has not already been created, then create it
	{
		//Here the Settings dialog box is placed on the screen
		SettingsDialogWindow=	CreateDialog (		//The createdialog function creates a dialog box...
		AtheroInstance,
		MAKEINTRESOURCE (SETTINGS_DIALOG),	//The information for the dialog box is contained in an '.rc' file
														//which is linked into the application during compilation.
														//the MAKEINTRESOURCE (SETTINGS_DIALOG) identifies the requested
														//dialog box
		AtheroWindow,	 //The window handle for the simulations main window
		(DLGPROC) SettingsDialog_DialogProc);	//The function that will handle all the messages for the dialog box
	}
	else
	{
		//Otherwise just make it visible again
		ShowWindow (SettingsDialogWindow,SW_SHOW);
	}
}



BOOL CALLBACK SettingsDialog_DialogProc (HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
//This message handler has the number that represents the message in uMsg, wParam and lParam are two
//paramaters that mean different things for different messages (wParam is 16 bits, lParam is 32)
	switch (uMsg)
	{
		HANDLE_MSG (hwndDlg,WM_INITDIALOG,SettingsDialog_OnInitDialog);
		HANDLE_MSG (hwndDlg,WM_HSCROLL,SettingsDialog_OnHScroll);
		HANDLE_MSG (hwndDlg,WM_COMMAND,SettingsDialog_OnCommand);
		case WM_DESTROY:
			return TRUE;
	}
	return FALSE;
}

BOOL SettingsDialog_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//When the dialog box starts up, all scrollbars are initialised from the variables
	//and the edit boxes display the initial values
	SetDlgItemInt(hwnd,IDC_NOOFMACROHITTINGWALLDISPLAY,NumMacrophageContactsPerCycle,FALSE);	//These two lines set the edit boxes
	SetDlgItemInt(hwnd,IDC_NOOFSMCHITTINGWALLDISPLAY,NumSMCContactsPerCycle,FALSE);	//to the values of NumMCPC and NumSMCCPC
	ScrollBar_SetRange (GetDlgItem (hwnd,IDC_NOOFMACROHITTINGWALLSCROLL),0,MAX_MACRO_CONTACTS_PER_CYCLE/10,FALSE);	//And these set the range
	ScrollBar_SetPos (GetDlgItem (hwnd,IDC_NOOFMACROHITTINGWALLSCROLL),NumMacrophageContactsPerCycle/10,TRUE);	//and position of the scrollbars
	ScrollBar_SetRange (GetDlgItem (hwnd,IDC_NOOFSMCHITTINGWALLSCROLL),0,MAX_SMC_CONTACTS_PER_CYCLE/10,FALSE);
	ScrollBar_SetPos (GetDlgItem (hwnd,IDC_NOOFSMCHITTINGWALLSCROLL),NumSMCContactsPerCycle/10,TRUE);
	//Also set a variable to say dialog has initialised
	SettingsDialogInitialised = TRUE;
	return TRUE;
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

	ScrollBar_SetPos (hwndCtl,pos,TRUE);	//This is necessary again because when the edit boxes are modified,
	//they set the scrollbar to their new value!
}

void SettingsDialog_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
//This message is sent when either a button is clicked, or the text boxes have the number in them
//changed
	switch (id)
	{
		case IDCANCEL:
		//If the 'close' button is clicked, or the cross on the top-right of the window, then the dialog
		//box is not destroyed but is simply hidden
			ShowWindow (hwnd,SW_HIDE);
			break;
		case IDC_NOOFMACROHITTINGWALLDISPLAY:
		//The next two cases are when the edit boxes have been modified. If so, the scrollbars position is changed
			ScrollBar_SetPos (GetDlgItem (hwnd,IDC_NOOFMACROHITTINGWALLSCROLL),GetDlgItemInt (hwnd,IDC_NOOFMACROHITTINGWALLDISPLAY,NULL,FALSE),TRUE);
		break;
		case IDC_NOOFSMCHITTINGWALLDISPLAY:
			ScrollBar_SetPos (GetDlgItem (hwnd,IDC_NOOFSMCHITTINGWALLSCROLL),GetDlgItemInt (hwnd,IDC_NOOFSMCHITTINGWALLDISPLAY,NULL,FALSE),TRUE);
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
			SaveSettings (hwnd);
		break;
		case IDC_LOADSETTINGS:
			LoadSettings (hwnd);
		break;

		//If the user clicks on Run Demo..., then the program opens a load dialog box and runs a demo
		case IDC_RUNDEMO:
			RunDemo (hwnd);
		break;

	}
}


void DrawMacroArrivalGraph (int x,int y)
{
	if (MacroArrivalFuncInfo.GraphVisible)
		return;
	if (!MacroArrivalFuncInfo.Initialised)
		InitMacroArrivalFuncInfo ();
	MacroArrivalGraphWindow = CreateFunctionAdjusterWindow (&MacroArrivalFuncInfo,x,y);
}

void DrawSMCArrivalGraph (int x,int y)
{
	if (SMCArrivalFuncInfo.GraphVisible)
		return;
	if (!SMCArrivalFuncInfo.Initialised)
		InitSMCArrivalFuncInfo ();
	SMCArrivalGraphWindow = CreateFunctionAdjusterWindow (&SMCArrivalFuncInfo,x,y);
}


void DrawMacroDeathGraph (int x,int y)
{
	if (MacroDeathFuncInfo.GraphVisible)
		return;
	if (!MacroDeathFuncInfo.Initialised)
		InitMacroDeathFuncInfo ();
	MacroDeathGraphWindow = CreateFunctionAdjusterWindow (&MacroDeathFuncInfo,x,y);
}

void StartSimulation (void)
{
	ModifyMenu (GetMenu (AtheroWindow),CM_START,MF_BYCOMMAND|MF_STRING,CM_PAUSE,"Pause");	//Remove the START! menu item
	InsertMenu (GetMenu (AtheroWindow),CM_PAUSE,MF_BYCOMMAND|MF_STRING,CM_RESTART,"Restart");

	//This function creates a new thread and makes the new thread do the heavy work - the simulation itself
	//This allows the program to continue normally, with all the windows functioning correctly
	SimulationThreadHandle = CreateThread (
	NULL,	//Security attributes - default
	NULL, //stack size - default
	(LPTHREAD_START_ROUTINE) SimulationStartPoint,	//Start point of new thread
	NULL,	//No additional startup data
	NULL,//No additional creation flags
	&SimulationThreadID	//Recieves thread id
	);
}

void PauseSimulation (void)
{
	SuspendThread (SimulationThreadHandle);
}

void ResumeSimulation (void)
{
	ResumeThread (SimulationThreadHandle);
}

void EndSimulation (void)
{
			ResumeSimulation ();	//If the simulation was paused, resume it (or setting SimulationGo = 0 will
			//have no effect)
			SimulationGo = 0;	//Causes the simulation to stop, freeing up memory
			DWORD ThreadExitCode;
			do
			{
				GetExitCodeThread (SimulationThreadHandle,&ThreadExitCode);
			}
			while (ThreadExitCode == STILL_ACTIVE);	//Wait for the simulation thread to terminate

			ModifyMenu (GetMenu (AtheroWindow),CM_RESTART,MF_BYCOMMAND|MF_STRING,CM_START,"START!");
			RemoveMenu (GetMenu (AtheroWindow),CM_PAUSE,MF_BYCOMMAND);
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
	  SaveDialogBoxInfo.lpstrFilter = "Simulation Settings\0*.SET\0";
	  SaveDialogBoxInfo.lpstrCustomFilter = NULL;
	  SaveDialogBoxInfo.nFilterIndex = 0;
	  SaveDialogBoxInfo.lpstrFile = new char [256];
	  SaveDialogBoxInfo.nMaxFile = 256;
	  FillMemory (SaveDialogBoxInfo.lpstrFile,256,0);
	  SaveDialogBoxInfo.lpstrFileTitle = NULL;
	  SaveDialogBoxInfo.lpstrInitialDir = NULL;
	  SaveDialogBoxInfo.lpstrTitle = "Save Simulation Settings";
	  SaveDialogBoxInfo.Flags = OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
	  SaveDialogBoxInfo.lpstrDefExt = "SET";
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
	  fprintf (FileHandle,"Atherosclerosis Simulation Settings\n");
	  fprintf (FileHandle,"VERSION:\n%f\n",VERSION);
	  fprintf (FileHandle,"RateOfMacrophagesHittingArtery:\n");
	  fprintf (FileHandle,"%d\n",NumMacrophageContactsPerCycle);


	  fprintf (FileHandle,"RateOfSMCHittingArtery:\n");
	  fprintf (FileHandle,"%d\n",NumSMCContactsPerCycle);

	  fprintf (FileHandle,"RateOfMacrophageLoss:\n");
	  int i;
	  for (i=0;i<=MacroDeathFuncInfo.NumPointsOnGraph;i++)
	  {
		  fprintf (FileHandle,"xLoss%d:\n",i*MacroDeathFuncInfo.Max_x_value/MacroDeathFuncInfo.NumPointsOnGraph);
		  fprintf (FileHandle,"%f\n",MacroDeathFuncInfo.yValuesDataForGraph[i]);
	  }

	  fprintf (FileHandle,"RateOfMacrophageArrival:\n");
	  for (i=0;i<=NUM_DEATH_GRAPH_POINTS;i++)
	  {
		  fprintf (FileHandle,"xMacroArrival%d:\n",i*MacroArrivalFuncInfo.Max_x_value/MacroArrivalFuncInfo.NumPointsOnGraph);
		  fprintf (FileHandle,"%f\n",MacroArrivalFuncInfo.yValuesDataForGraph[i]);
	  }

	  fprintf (FileHandle,"RateOfSMCArrival:\n");
	  for (i=0;i<=NUM_ARRIVAL_GRAPH_POINTS;i++)
	  {
		  fprintf (FileHandle,"xSMCArrival%d:\n",i*SMCArrivalFuncInfo.Max_x_value/SMCArrivalFuncInfo.NumPointsOnGraph);
		  fprintf (FileHandle,"%f\n",SMCArrivalFuncInfo.yValuesDataForGraph[i]);
	  }
	  fclose (FileHandle);


}

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
	  LoadDialogBoxInfo.lpstrFilter = "Simulation Settings\0*.SET\0";
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
	  fread ((void*)FileBuffer,FileSize,1,FileHandle);
	  fclose (FileHandle);
	  float Version;
	  if (memcmp ((void*)FileBuffer,(void*)"Atherosclerosis Simulation Settings\n",strlen ("Atherosclerosis Simulation Settings\n")))
	  {
			MessageBox (hwnd,"Not an Atherosclerosis Simulation Settings file","Error!",MB_OK);
			return;
	  }
	  FindStringAndEnterVariable (FileBuffer,"VERSION:\n",&Version);
	  if (Version != VERSION)
	  {
			MessageBox (hwnd,"Incorrect file version","Error!",MB_OK);
			return;
	  }
	  FindStringAndEnterVariable (FileBuffer,"RateOfMacrophagesHittingArtery:\n",&NumMacrophageContactsPerCycle);
	  FindStringAndEnterVariable (FileBuffer,"RateOfSMCHittingArtery:\n",&NumSMCContactsPerCycle);

	  int i;
	  char xValueString [20];
	  for (i=0;i<=NUM_DEATH_GRAPH_POINTS;i++)
	  {
			sprintf (xValueString,"xMacroArrival%d:\n",i*MAX_MACROPHAGES_PER_PIXEL*9/NUM_ARRIVAL_GRAPH_POINTS);
			FindStringAndEnterVariable (FileBuffer,xValueString,&MacroArrivalFuncInfo.yValuesDataForGraph[i]);
	  }

	  for (i=0;i<=NUM_DEATH_GRAPH_POINTS;i++)
	  {
			sprintf (xValueString,"xLoss%d:\n",i*MAX_MACROPHAGES_PER_PIXEL*9/NUM_DEATH_GRAPH_POINTS);
			FindStringAndEnterVariable (FileBuffer,xValueString,&MacroDeathFuncInfo.yValuesDataForGraph[i]);
	  }


	  for (i=0;i<=NUM_ARRIVAL_GRAPH_POINTS;i++)
	  {
			sprintf (xValueString,"xSMCArrival%d:\n",i*MAX_MACROPHAGES_PER_PIXEL*9/NUM_ARRIVAL_GRAPH_POINTS);
			FindStringAndEnterVariable (FileBuffer,xValueString,&SMCArrivalFuncInfo.yValuesDataForGraph[i]);
	  }
	  GenerateyValuesForEveryxValue (&MacroArrivalFuncInfo);
	  GenerateyValuesForEveryxValue (&MacroDeathFuncInfo);
	  GenerateyValuesForEveryxValue (&SMCArrivalFuncInfo);
}

void FindStringAndEnterVariable (char* FileBuffer,char *String,int *Variable)
{
	//This searches 'FileBuffer' for 'String', and enters the number following 'String' into 'Variable'
	char*FoundStringPosition = strstr (FileBuffer,String);
	if (!FoundStringPosition)
	{
		MessageBox (SettingsDialogWindow,String,"Could not find variable in file",MB_OK);
		return;
	}
	FoundStringPosition+=strlen (String);
	sscanf (FoundStringPosition,"%d",Variable);
}

void FindStringAndEnterVariable (char* FileBuffer,char *String,float *Variable)
{
	//This searches 'FileBuffer' for 'String', and enters the number following 'String' into 'Variable'
	char*FoundStringPosition = strstr (FileBuffer,String);
	if (!FoundStringPosition)
	{
		MessageBox (SettingsDialogWindow,String,"Could not find variable in file",MB_OK);
		return;
	}
	FoundStringPosition+=strlen (String);
	sscanf (FoundStringPosition,"%f",Variable);
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















