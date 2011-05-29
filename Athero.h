//Athero.h - header file for atherosclerosis simulation program
#pragma once
#define WIN32_EXTRA_LEAN
#include "stdafx.h"
#include <afxwin.h>
#include <afxext.h>
#include <windowsx.h>
#include <commdlg.h>
#include <ddraw.h>
#include <d3dx.h>
#include <io.h>
#include <sys\stat.h>
#include <stdio.h>
#include <string.h>
#include <d3drm.h>
#include "resource.h"
#include "AtheroApp.h"
#include "XSectionDisplayDlg.h"


#define COLOUR_565
//Definitions from athero.cpp
//Do this many simulation ticks per second
#define UPDATES_PER_SEC 25


//Definitions for the whole simulation
#define MAX_MACROPHAGES_PER_PIXEL 40
//#define MAX_SMCS_PER_PIXEL	20
#define MAX_SMCS_PER_PIXEL	60
#define MAX_LIPIDS_PER_PIXEL 30
#define MAX_MACRO_CONTACTS_PER_CYCLE 300000
#define MAX_SMC_CONTACTS_PER_CYCLE 100000
#define ARRIVAL_GRAPH_SENSITIVITY 2000 //65535 = Max sensitivity, 0 = does nothing
#define VERSION 1.3

#define CELL_BUFFER_X_SIZE 320
#define CELL_BUFFER_Y_SIZE 200

#define NUM_DEATH_GRAPH_POINTS 20
#define NUM_ARRIVAL_GRAPH_POINTS 20

extern int ARTERY_RED;
extern int ARTERY_GREEN;
extern int ARTERY_BLUE;



extern int  COLOUR_FOR_MAX_NO_MACROS_R;
extern int COLOUR_FOR_MAX_NO_MACROS_G;
extern int COLOUR_FOR_MAX_NO_MACROS_B;

extern int COLOUR_FOR_1_MACRO_R;
extern int COLOUR_FOR_1_MACRO_G;
extern int COLOUR_FOR_1_MACRO_B;

extern int COLOUR_FOR_MAX_NO_SMCS_R;
extern int COLOUR_FOR_MAX_NO_SMCS_G;
extern int COLOUR_FOR_MAX_NO_SMCS_B;

extern int COLOUR_FOR_1_SMC_R;
extern int COLOUR_FOR_1_SMC_G;
extern int COLOUR_FOR_1_SMC_B;

extern int COLOUR_FOR_MAX_NO_LIPIDS_R;
extern int COLOUR_FOR_MAX_NO_LIPIDS_G;
extern int COLOUR_FOR_MAX_NO_LIPIDS_B;

extern int COLOUR_FOR_1_LIPID_R;
extern int COLOUR_FOR_1_LIPID_G;
extern int COLOUR_FOR_1_LIPID_B;

const float PI = 3.1415926f;



//#define COLOUR_FOR_MAX_NO_MACROS_R 255
//#define COLOUR_FOR_MAX_NO_MACROS_G 210
//#define COLOUR_FOR_MAX_NO_MACROS_B 0

//#define COLOUR_FOR_1_MACRO_R 90
//#define COLOUR_FOR_1_MACRO_G 50
//#define COLOUR_FOR_1_MACRO_B 0

//#define COLOUR_FOR_MAX_NO_SMCS_R 100
//#define COLOUR_FOR_MAX_NO_SMCS_G 100
//#define COLOUR_FOR_MAX_NO_SMCS_B 200

//#define COLOUR_FOR_1_SMC_R 25
//#define COLOUR_FOR_1_SMC_G 25
//#define COLOUR_FOR_1_SMC_B 50

//#define COLOUR_FOR_MAX_NO_SMCS_R 1
//#define COLOUR_FOR_MAX_NO_SMCS_G 239
//#define COLOUR_FOR_MAX_NO_SMCS_B 57

//#define COLOUR_FOR_1_SMC_R 1
//#define COLOUR_FOR_1_SMC_G 51
//#define COLOUR_FOR_1_SMC_B 46

//#define COLOUR_FOR_MAX_NO_LIPIDS_R 255
//#define COLOUR_FOR_MAX_NO_LIPIDS_G 255
//#define COLOUR_FOR_MAX_NO_LIPIDS_B 255

//#define COLOUR_FOR_1_LIPID_R 100
//#define COLOUR_FOR_1_LIPID_G 100
//#define COLOUR_FOR_1_LIPID_B 100



//Definitions for the simulation's main window
void InitGraphsAndSettings (void);
void CreateFunctionAdjusterWindow (void);
HWND CreateAtheroWindow (void);
BOOL RegisterAtheroWindow (void);
void 	CreateSettingsDialogBox (void);
BOOL CALLBACK SettingsDialog_DialogProc (HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL SettingsDialog_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void SettingsDialog_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
void SettingsDialog_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void DrawMacroArrivalGraph (int x,int y);
void DrawMacroDeathGraph (int x,int y);
void DrawSMCArrivalGraph (int x,int y);
void StartSimulation (void);
void SaveSettings (HWND hwnd);
void LoadSettings (HWND hwnd);
void LoadSettingsFromFile (HWND hwnd,char *LoadFilename);
void SaveSettingsFromFile (HWND hwnd,char *SaveFilename);
void PauseSimulation (void);
void ResumeSimulation (void);
void EndSimulation (void);
void RunDemo (HWND hwnd);
VOID CALLBACK DemoTimerProc(HWND  hwnd,UINT  uMsg,UINT  idEvent,DWORD  dwTime);
void StopDemo (void);

extern bool g_LeftPressed;
extern bool g_RightPressed;
extern HWND AtheroWindow;
extern HDC AtheroWindowDC;
class SimulationInfo;
extern SimulationInfo SimulationInfoDialog;
extern DWORD g_LastUpdateTime;
extern DWORD g_RenderTime;


#define  Athero_DefProc DefWindowProc
//All these definitions go into the CreateWindow call below
#define MAINWINDOW_L 100	//The left coordinate of the simulation's main window
#define MAINWINDOW_T 100	//The top coordinate of the simulation's main window
#define MAINWINDOW_R 300	//Right
#define MAINWINDOW_B 300	//Bottom
#define MAINWINDOW_FLAGS WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN//Flags saying what type of window we want

#define FUNCTIONADJUSTERNAME "Function Adjuster" //Name for function adjuster class
//The following structure contains all the information needed to put a function adjuster on the screen
typedef struct _FUNCTIONADJUSTERINFO
{
	char *Title;		//Text string for graph window title
	int xsize;			//width of graph window (pixels)
	int ysize;			//height of graph window
	int NumPointsOnGraph;	//Number of points on the graph, NOT including the point at (0,y)
	char *xAxisName;	//Text string for x axis label
	char *yAxisName;	//Text string for y axis label
	int Max_x_value;	//Maximum value for x axis
	int Max_y_value;	//Maximum value for y axis that will be entered into yValuesDataForEveryxValue
	//The y axis will always be a log scale that goes up to 100%. When 100% is set at a point, Max_y_value
	//will be entered into the yValuesDataForExV
	float *yValuesDataForGraph;	//These are the initial percentages to be used to set up the graph

//These next variables need not be set by the user - they are internal
	BOOL GraphVisible;	// 0 = Graph not on screen, 1 = Graph on screen
	BOOL Initialised;		// 0 = Variables below not correct, 1 = Variables below initialised
	unsigned short *yValuesDataForEveryxValue;	// Pointer to Max_x_value+1 short integers which
	//represent the y value of the graph (between 0 and Max_y_value) for every integer on the x axis
	unsigned ValueForx0;
	bool UseValueForx0;
	int xAxisRow;
	int yAxisColumn;
	int xAxisLength;
	int yAxisLength;
} FUNCTIONADJUSTERINFO;


extern HINSTANCE AtheroInstance;	  //This is the handle to the application itself - used in some function calls
extern HWND AtheroWindow;			//This is handle to the simulation's main window, used for drawing, messages etc
extern CString RecordDataFileName;

//Definitions from func_adj.cpp
BOOL RegisterFunctionAdjusterWindow (void);
HWND CreateFunctionAdjusterWindow (FUNCTIONADJUSTERINFO *fInfo,int xpos,int ypos);
void InitialiseFunctionAdjuster (FUNCTIONADJUSTERINFO * lpFunctionAdjusterInfo);
void GenerateyValuesForEveryxValue (FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo);

//Definitions from macrophg.cpp
typedef struct _MACROPIXELDATA
{
	WORD MacrophageLives [MAX_MACROPHAGES_PER_PIXEL+1];
	BYTE NoofCellsOnPixel;
	unsigned short int NoofSurroundingCells;
} MACROPIXELDATA;

typedef struct _SMCPIXELDATA
{
	BYTE NoofCellsOnPixel;
} SMCPIXELDATA;

typedef struct _LIPIDPIXELDATA
{
	BYTE NoofBlobsOnPixel;
} LIPIDPIXELDATA;


#define RAND_TABLE_SIZE 100000	//20k of random numbers are prestored
extern int NumMacrophageContactsPerCycle;
extern int NumSMCContactsPerCycle;
extern int SMCMacroKillRate;
extern int LipidArrivalRate;
extern int LipidRemovalRate;
extern BOOL DrawMacrophages;
extern BOOL DrawSMCs;
extern BOOL DrawLipids;
extern BOOL View3D;
extern BOOL GameMode;
extern FUNCTIONADJUSTERINFO MacroArrivalFuncInfo;
extern FUNCTIONADJUSTERINFO MacroDeathFuncInfo;
extern FUNCTIONADJUSTERINFO SMCArrivalFuncInfo;
extern float MacrophageArrivalGraphyValues [NUM_ARRIVAL_GRAPH_POINTS+1];
extern float MacrophageDeathGraphyValues [NUM_DEATH_GRAPH_POINTS+1];
extern BOOL SimulationGo;	//Settings this to FALSE will cause the simulation to stop
extern int BreakSimulation;
extern int StopAtCycle;
extern MACROPIXELDATA* MacrophageDataBuffer;	
extern SMCPIXELDATA* SMCDataBuffer;
extern LIPIDPIXELDATA *LipidDataBuffer;	
extern CXSectionDisplayDlg * pCurrentDisplayGraph;
extern CRITICAL_SECTION LockCritSection;

void CalculateMacrophageColours ();
void CalculateSMCColours ();
void CalculateLipidColours ();


DWORD SimulationStartPoint (LPDWORD lpStartupData);
void InitMacroArrivalFuncInfo (void);
void InitMacroDeathFuncInfo (void);
void InitSMCArrivalFuncInfo (void);
void DrawMacrophageBuffer ();
void DrawMacrophageBuffer3D();
void Initialise3D ();
void ShutDown3D ();
void Do3DViewPosAdjustment (int x, int y);
void Do3DZoomAdjustment (int x, int y);
void BuildArtery (D3DXVECTOR3& corner, D3DXVECTOR3& lengthvec, D3DXVECTOR3& widthvec);
		





