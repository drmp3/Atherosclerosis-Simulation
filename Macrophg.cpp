#include "athero.h"
#include "ddrawx.h"
#include <time.h>
#include "SimulationInfo.h"
#include "Macrophg.h"


#define random(rndnum) (rand () * (rndnum-1)) / RAND_MAX

//Function prototypes
void  GenerateRandomTable (void);
void InitGraphFunctions (void);
void InitMacrophageDataBuffer (void);
void InitSMCDataBuffer (void);
void InitLipidDataBuffer (void);
void CalculateMacrophageColours (void);
void CalculateSMCColours (void);
void CalculateLipidColours (void);
void BeginMainSimulationLoop (void);
void PlaceNewMacros (void);
void PlaceNewSMCs (void);
void CalculateNewMacroCounts (void);
void MarkDeadCells (void);
void RemoveDeadCells (void);
void DoLipidArrival (int i);
void RemoveLipid (int i);
void DrawMacrophageBuffer (void);
void CheckBreak ();
void GetRecordFile ();

//global variables for macrophages
int NumMacrophageContactsPerCycle;
int SMCMacroKillRate;
int NumSMCContactsPerCycle;
int LipidArrivalRate;
int LipidRemovalRate;
int TotalLipids;
int TotalSMCs;
BOOL DrawMacrophages;
BOOL DrawSMCs;
BOOL DrawLipids;
BOOL View3D;
BOOL GameMode;
int StopAtCycle=-1;
FUNCTIONADJUSTERINFO MacroArrivalFuncInfo;
FUNCTIONADJUSTERINFO MacroDeathFuncInfo;
FUNCTIONADJUSTERINFO SMCArrivalFuncInfo;
float MacrophageArrivalGraphyValues [NUM_ARRIVAL_GRAPH_POINTS+1];
float MacrophageDeathGraphyValues [NUM_DEATH_GRAPH_POINTS+1];
float SMCArrivalGraphyValues [NUM_ARRIVAL_GRAPH_POINTS+1];
WORD MacroColourTable [MAX_MACROPHAGES_PER_PIXEL+1];
WORD SMCColourTable [MAX_SMCS_PER_PIXEL+1];
WORD LipidColourTable [MAX_LIPIDS_PER_PIXEL+1];
BOOL SimulationGo = TRUE;
int BreakSimulation = FALSE;
int TotalNumMacros;
int NumCycles;
CFile *pRecordFile;
bool CellSurfaceChanged;

unsigned short *RandomNumberTable65535Pointer;
int RandomNumberTable65535CurOffset;
unsigned short *RandomNumberTableMAX_YPointer;
int RandomNumberTableMAX_YCurOffset;
unsigned short *RandomNumberTableMAX_XPointer;
int RandomNumberTableMAX_XCurOffset;



MACROPIXELDATA* MacrophageDataBuffer;	//Macrophage data buffer is a series of cell pixel data structures,
//which contain information as to the amount of time each of the cells have to live, as well as the number
//of surrounding cells and the number of cells contained at the point

SMCPIXELDATA* SMCDataBuffer;	//Smooth Muscle Cell data buffer is similar
LIPIDPIXELDATA *LipidDataBuffer; //Lipid data buffer is also similar

void MacroInit()
{
    //The Simulation starts here. All the global variables are set by the athero.cpp program
    if (!GameMode)
        GetRecordFile();
    //First, random number tables must be generated or generating random numbers will take too long
    GenerateRandomTable ();
    //Then memory is allocated for the main macrophage data buffer
    InitMacrophageDataBuffer ();
    //Then the memory for the smc data buffer is allocated
    InitSMCDataBuffer ();
    //Then memory for lipid data
    InitLipidDataBuffer ();
    //Then the colours that will be used to represent the cells will be calculated
    CalculateMacrophageColours ();
    CalculateSMCColours ();
    CalculateLipidColours ();
    
    NumCycles=0;
    TotalLipids=0;
    TotalSMCs=0;
}

void GetRecordFile ()
{
    if (!RecordDataFileName.IsEmpty ())
    {
        pRecordFile = new CFile (RecordDataFileName,CFile::modeWrite|CFile::modeCreate);
        char WriteStr []= "Simulation Start\nCycles,Macrophages\n0,0\n";
        pRecordFile->Write (WriteStr,strlen (WriteStr));
        CString DlgString = "Recording To File: ";
        DlgString+=RecordDataFileName;
        SimulationInfoDialog.SetDlgItemText (IDC_RECORDING,DlgString);
    }
    else
    {
        pRecordFile = NULL;
        SimulationInfoDialog.SetDlgItemText  (IDC_RECORDING,"Not Recording");
    }
    
}

void  GenerateRandomTable (void)
{
    //The random number tables consist of RAND_TABLE_SIZE short integers (16 bits), all of which are between
    //0 and 65535 for the 65535 table, 0 and CELL_BUFFER_X_SIZE for the MAX_X buffer, and 0 and CELL_BUFFER_Y_SIZE
    //For the MAX_Y table
    RandomNumberTable65535Pointer = new unsigned short [RAND_TABLE_SIZE];
    RandomNumberTableMAX_XPointer = new unsigned short [RAND_TABLE_SIZE];
    RandomNumberTableMAX_YPointer = new unsigned short [RAND_TABLE_SIZE];
    int i;
    srand (time (NULL));
    //srand (time); //Seed random number generator
    for (i=0;i<RAND_TABLE_SIZE;i++)
    {
        RandomNumberTable65535Pointer[i]=random (65536);
        RandomNumberTableMAX_XPointer[i]=random (CELL_BUFFER_X_SIZE+1);
        RandomNumberTableMAX_YPointer[i]=random (CELL_BUFFER_Y_SIZE+1);
    }
    
}

unsigned short FastRand65535 (void)
{
    //This function reads random numbers off the random table
    if (RandomNumberTable65535CurOffset < RAND_TABLE_SIZE)
        return RandomNumberTable65535Pointer [RandomNumberTable65535CurOffset++];
    else
    {
        RandomNumberTable65535CurOffset = random (RAND_TABLE_SIZE/2);
        return RandomNumberTable65535Pointer [RandomNumberTable65535CurOffset++];
    }
}

unsigned short FastRandMAX_X (void)
{
    //This function reads random numbers off the random table
    if (RandomNumberTableMAX_XCurOffset < RAND_TABLE_SIZE)
        return RandomNumberTableMAX_XPointer [RandomNumberTableMAX_XCurOffset++];
    else
    {
        RandomNumberTableMAX_XCurOffset = random (RAND_TABLE_SIZE/2);
        return RandomNumberTableMAX_XPointer [RandomNumberTableMAX_XCurOffset++];
    }
}

unsigned short FastRandMAX_Y (void)
{
    //This function reads random numbers off the random table
    if (RandomNumberTableMAX_YCurOffset < RAND_TABLE_SIZE)
        return RandomNumberTableMAX_YPointer [RandomNumberTableMAX_YCurOffset++];
    else
    {
        RandomNumberTableMAX_YCurOffset = random (RAND_TABLE_SIZE/2);;
        return RandomNumberTableMAX_YPointer [RandomNumberTableMAX_YCurOffset++];
    }
}



char MacroDeathGraphTitle [] = "Macrophage death rates";
char MacroDeathxAxisTitle [] = "Number of cells present";
char MacroDeathyAxisTitle [] = "Death Rate";

void InitMacroDeathFuncInfo (void)
{
    //Here we fill in the structure for the macrophage death graph
    //And initialise it for use in the graph and the simulation
    MacroDeathFuncInfo.Title = MacroDeathGraphTitle;	//Set all the titles...
    MacroDeathFuncInfo.xAxisName =MacroDeathxAxisTitle;
    MacroDeathFuncInfo.yAxisName =MacroDeathyAxisTitle;
    MacroDeathFuncInfo.Max_x_value = MAX_MACROPHAGES_PER_PIXEL *9;	//When the Death chances are to be
    //calculated, the number of cells in the surrounding four squares are counted, the probability
    //obtained and then a decision made. Therefore, when the cells are counted there are 5*MAXMACROSPERPIXEL
    //as the centre square is also counted.
    MacroDeathFuncInfo.Max_y_value = 65535;	//Real Max y value is 65535 - see structure comments in athero.h
    MacroDeathFuncInfo.NumPointsOnGraph = NUM_DEATH_GRAPH_POINTS; // 16 points on graph not including 0
    MacroDeathFuncInfo.xsize = 500;
    MacroDeathFuncInfo.ysize = 590;
    MacroDeathFuncInfo.yValuesDataForGraph =MacrophageDeathGraphyValues;	//Set the initial data
    InitialiseFunctionAdjuster (&MacroDeathFuncInfo);
}

char MacroArrivalGraphTitle [] = "Macrophage arrival probabilities";
char MacroArrivalxAxisTitle [] = "Number of cells present";
char MacroArrivalyAxisTitle [] = "% Prob.";

void InitMacroArrivalFuncInfo (void)
{
    //Here we fill in the structure for the macrophage arrival graph
    //And initialise it for use in the graph and the simulation
    MacroArrivalFuncInfo.Title = MacroArrivalGraphTitle;	//Set all the titles...
    MacroArrivalFuncInfo.xAxisName =MacroArrivalxAxisTitle;
    MacroArrivalFuncInfo.yAxisName =MacroArrivalyAxisTitle;
    MacroArrivalFuncInfo.Max_x_value = MAX_MACROPHAGES_PER_PIXEL*9;	//When the arrival chances are to be
    //calculated, the number of cells in the surrounding four squares are counted, the probability
    //obtained and then a decision made. Therefore, when the cells are counted there are 5*MAXMACROSPERPIXEL
    //as the centre square is also counted.
    MacroArrivalFuncInfo.Max_y_value = 65535;//Real Max y value is 65535 - see structure comments in athero.h
    MacroArrivalFuncInfo.NumPointsOnGraph = NUM_ARRIVAL_GRAPH_POINTS; // 16 points on graph not including 0
    MacroArrivalFuncInfo.xsize = 500;
    MacroArrivalFuncInfo.ysize = 590;
    MacroArrivalFuncInfo.yValuesDataForGraph =MacrophageArrivalGraphyValues;	//Set the initial data
    InitialiseFunctionAdjuster (&MacroArrivalFuncInfo);	//And then make the system fill in all the system
    //variables, including y values for every x value
}

char SMCArrivalGraphTitle [] = "Smooth Muscle Cell arrival probabilities";
char SMCArrivalxAxisTitle [] = "Number of macrophages present";
char SMCArrivalyAxisTitle [] = "% Prob.";

void InitSMCArrivalFuncInfo (void)
{
    //Here we fill in the structure for the SMCphage arrival graph
    //And initialise it for use in the graph and the simulation
    SMCArrivalFuncInfo.Title = SMCArrivalGraphTitle;	//Set all the titles...
    SMCArrivalFuncInfo.xAxisName =SMCArrivalxAxisTitle;
    SMCArrivalFuncInfo.yAxisName =SMCArrivalyAxisTitle;
    SMCArrivalFuncInfo.Max_x_value = MAX_MACROPHAGES_PER_PIXEL*9;	//When the arrival chances are to be
    //calculated, the number of cells in the surrounding four squares are counted, the probability
    //obtained and then a decision made. Therefore, when the cells are counted there are 5*MAXSMCSPERPIXEL
    //as the centre square is also counted.
    SMCArrivalFuncInfo.Max_y_value = 65535;//Real Max y value is 65535 - see structure comments in athero.h
    SMCArrivalFuncInfo.NumPointsOnGraph = NUM_ARRIVAL_GRAPH_POINTS; // 16 points on graph not including 0
    SMCArrivalFuncInfo.xsize = 500;
    SMCArrivalFuncInfo.ysize = 590;
    SMCArrivalFuncInfo.yValuesDataForGraph =SMCArrivalGraphyValues;	//Set the initial data
    InitialiseFunctionAdjuster (&SMCArrivalFuncInfo);	//And then make the system fill in all the system
    //variables, including y values for every x value
}



void InitMacrophageDataBuffer (void)
{
    TotalNumMacros=0;
    MacrophageDataBuffer = (MACROPIXELDATA*) malloc (sizeof (MACROPIXELDATA [CELL_BUFFER_X_SIZE*CELL_BUFFER_Y_SIZE]));
    //The line above allocates memory so that MacrophageDataBuffer points to a series of MACROPIXELDATA structures.
    //The pixel at the top left of the screen is represented by MacrophageDataBuffer[0], the top right
    //is MDB [CELL_BUFFER_X_SIZE-1] and beginning of the second line on the left side is MDB [CELL_BUFFER_X_SIZE],
    //and so on, until the last pixel which is the bottom right at MDB [(CELL_BUFFER_X_SIZE-1)*(CELL_BUFFER_Y_SIZE-1)]
    
    //The buffer is set to read all zeros
    memset (MacrophageDataBuffer,0,CELL_BUFFER_X_SIZE * CELL_BUFFER_Y_SIZE*sizeof (MACROPIXELDATA));
}

void InitSMCDataBuffer (void)
{
    //This is exactly the same as for the macrophages
    SMCDataBuffer = new SMCPIXELDATA [CELL_BUFFER_X_SIZE * CELL_BUFFER_Y_SIZE];
    memset (SMCDataBuffer,0,CELL_BUFFER_X_SIZE * CELL_BUFFER_Y_SIZE*sizeof (SMCPIXELDATA));
}

void InitLipidDataBuffer (void)
{
    TotalNumMacros=0;
    LipidDataBuffer = (LIPIDPIXELDATA*) malloc (sizeof (LIPIDPIXELDATA [CELL_BUFFER_X_SIZE*CELL_BUFFER_Y_SIZE]));
    //The line above allocates memory so that MacrophageDataBuffer points to a series of MACROPIXELDATA structures.
    //The pixel at the top left of the screen is represented by MacrophageDataBuffer[0], the top right
    //is MDB [CELL_BUFFER_X_SIZE-1] and beginning of the second line on the left side is MDB [CELL_BUFFER_X_SIZE],
    //and so on, until the last pixel which is the bottom right at MDB [(CELL_BUFFER_X_SIZE-1)*(CELL_BUFFER_Y_SIZE-1)]
    
    //The buffer is set to read all zeros
    memset (LipidDataBuffer,0,CELL_BUFFER_X_SIZE * CELL_BUFFER_Y_SIZE*sizeof (LIPIDPIXELDATA));
}

WORD MakeColour (unsigned r, unsigned g, unsigned b)
{
    r &= 0xff;
    g &= 0xff;
    b &= 0xff;
#ifdef COLOUR_565
    return (b>>3) | ((g>>2)<<5) | ((r>>3)<<11);	
#else
    return (b>>3) | ((g>>3)<<5) | ((r>>3)<<10);
#endif
}


void CalculateMacrophageColours (void)
{
    //MacroColourTable is a table so that when a pixel with x number of macrophages on it is to be plotted,
    //MacroColourTable [x] is the colour that should be used
    int i;
    for (i=1;i<=MAX_MACROPHAGES_PER_PIXEL;i++)
    {
        float FractionOfMaxCells = (i-1)/(float)(MAX_MACROPHAGES_PER_PIXEL-1);
        //MakeColour creates a new palette entry, given red,green,blue and the importance of the colour
        //red,green,blue have a maximum of 255
        MacroColourTable [i] = MakeColour (
            (COLOUR_FOR_MAX_NO_MACROS_R-COLOUR_FOR_1_MACRO_R)*FractionOfMaxCells+COLOUR_FOR_1_MACRO_R,	//red content of colour
            (COLOUR_FOR_MAX_NO_MACROS_G-COLOUR_FOR_1_MACRO_G)*FractionOfMaxCells+COLOUR_FOR_1_MACRO_G,	//green
            (COLOUR_FOR_MAX_NO_MACROS_B-COLOUR_FOR_1_MACRO_B)*FractionOfMaxCells+COLOUR_FOR_1_MACRO_B		//blue 
            );
    }
}

void CalculateSMCColours (void)
{
    int i;
    for (i=1;i<=MAX_SMCS_PER_PIXEL;i++)
    {
        float FractionOfMaxCells = (i-1)/(float)(MAX_SMCS_PER_PIXEL-1);
        //MakeColour creates a new palette entry, given red,green,blue and the importance of the colour
        //red,green,blue have a maximum of 255
        SMCColourTable [i] = MakeColour (
            (COLOUR_FOR_MAX_NO_SMCS_R-COLOUR_FOR_1_SMC_R)*FractionOfMaxCells+COLOUR_FOR_1_SMC_R,	//red content of colour
            (COLOUR_FOR_MAX_NO_SMCS_G-COLOUR_FOR_1_SMC_G)*FractionOfMaxCells+COLOUR_FOR_1_SMC_G,	//green
            (COLOUR_FOR_MAX_NO_SMCS_B-COLOUR_FOR_1_SMC_B)*FractionOfMaxCells+COLOUR_FOR_1_SMC_B		//blue
            );
    }
}

void CalculateLipidColours ()
{
    int i;
    for (i=1;i<=MAX_LIPIDS_PER_PIXEL;i++)
    {
        float FractionOfMaxBlobs = (i-1)/(float)(MAX_LIPIDS_PER_PIXEL-1);
        //MakeColour creates a new palette entry, given red,green,blue and the importance of the colour
        //red,green,blue have a maximum of 255
        LipidColourTable [i] = MakeColour (
            (COLOUR_FOR_MAX_NO_LIPIDS_R-COLOUR_FOR_1_LIPID_R)*FractionOfMaxBlobs+COLOUR_FOR_1_LIPID_R,	//red content of colour
            (COLOUR_FOR_MAX_NO_LIPIDS_G-COLOUR_FOR_1_LIPID_G)*FractionOfMaxBlobs+COLOUR_FOR_1_LIPID_G,	//green
            (COLOUR_FOR_MAX_NO_LIPIDS_B-COLOUR_FOR_1_LIPID_B)*FractionOfMaxBlobs+COLOUR_FOR_1_LIPID_B	//blue	 	
            );
    }
    
    LipidColourTable [0] = MakeColour (ARTERY_RED,ARTERY_GREEN,ARTERY_BLUE);	//Make sure the first entry is red - the
    //artery wall colour - if no cells are present
}

void MacroUpdate()
{
    //This is the main loop which generates and kills the cells
    
    //First, the new cells are placed (marked into the buffer, but the number of cell counts in the buffer
    //does not change). They are marked into the buffer by setting the cell lives to -1, which is an impossible life
    //for any old cell to have
    PlaceNewMacros ();
    PlaceNewSMCs ();
    //Then the new cell counts for each pixel (data in MACROPIXELDATA structure saying how many cells present
    //on each pixel) are calculated
    CalculateNewMacroCounts ();
    //Mark cells ready to die - this is done by setting their lives to a value of 0
    MarkDeadCells ();
    
    //Then remove all the cells with a life value of 0
    RemoveDeadCells ();
    
    NumCycles++;
    SimulationInfoDialog.m_NumMacrophages = TotalNumMacros;
    SimulationInfoDialog.m_NumCycles = NumCycles;
    
    if (NumCycles == StopAtCycle)
        PostMessage (AtheroWindow,WM_COMMAND,CM_PAUSE,0);
    
    if (pRecordFile)
    {
        char WriteOutput[30];
        sprintf (WriteOutput,"%d,%d\n",NumCycles,TotalNumMacros);
        pRecordFile->Write (WriteOutput,strlen (WriteOutput));
    }
    if (pCurrentDisplayGraph)
    {
        pCurrentDisplayGraph->InvalidateRect (NULL,false);
        //	pCurrentDisplayGraph->PostMessage (WM_PAINT);
        //	pCurrentDisplayGraph->UpdateWindow ();
    }
    
}

void MacroRender()
{
    SimulationInfoDialog.InvalidateRect (NULL,false);
    SimulationInfoDialog.PostMessage (WM_PAINT);
    //SimulationInfoDialog.UpdateWindow ();
    
    DrawMacrophageBuffer ();
    
    if (View3D) {
        BuildArtery (D3DXVECTOR3(-300.0f, 50.0f, -300.0f), D3DXVECTOR3 (0, 0.0f, 600.0f), D3DXVECTOR3 (600.0f,0.0f,0));
        DrawMacrophageBuffer3D();
    }    
}



void MacroTerminate()
{
    free (MacrophageDataBuffer);
    free (LipidDataBuffer);
    MacrophageDataBuffer = NULL;
    delete []SMCDataBuffer;
    delete []RandomNumberTable65535Pointer;
    delete []RandomNumberTableMAX_XPointer;
    delete []RandomNumberTableMAX_YPointer;
    delete pRecordFile;
}


void PlaceNewMacros (void)
{
    int i;
    int Newx,Newy;
    int ProbOfArrival;
    int NumberOfCellsPresentAtCoord;
    int NumberOfCellsSurroundingCoord;
    int OffsetToPixelData;
    CheckBreak ();
    for (i=0;i<NumMacrophageContactsPerCycle;i++)
    {
        //execute this loop NMCPC times
        
        //Get random coords
        Newx = FastRandMAX_X ();	//Newx is between 0 and CELL_BUFFER_X_SIZE-1
        Newy = FastRandMAX_Y ();	//Newy is between 0 and CELL_BUFFER_Y_SIZE-1
        if (Newx >= CELL_BUFFER_X_SIZE || Newy >= CELL_BUFFER_Y_SIZE)
            continue;
        //Get number of cells present on coord
        OffsetToPixelData =Newy*CELL_BUFFER_X_SIZE+Newx;
        NumberOfCellsSurroundingCoord =MacrophageDataBuffer [OffsetToPixelData].NoofSurroundingCells;
        NumberOfCellsPresentAtCoord = MacrophageDataBuffer [OffsetToPixelData].NoofCellsOnPixel;
        //Get probability of cell arriving given number of cells present
        //by looking at the yValues data from the arrival function
        //each of the y values is between 0 and 65535 (rather than the 100% actually printed)
        //This fits in the random number generator very well
        
        
        //if (NumberOfCellsSurroundingCoord || !MacroArrivalFuncInfo.UseValueForx0)
        //{
        //If the number of cells isnt zero, use the following method
        //to get probability of cell arriving
        ProbOfArrival = MacroArrivalFuncInfo.yValuesDataForEveryxValue[NumberOfCellsSurroundingCoord];
        
        ProbOfArrival /= (1+(((int)SMCDataBuffer [OffsetToPixelData].NoofCellsOnPixel)*SMCMacroKillRate)/100);
        
        
        //if (TotalNumMacros < 10)
        if (ProbOfArrival > FastRand65535 () && NumberOfCellsPresentAtCoord < MAX_MACROPHAGES_PER_PIXEL)
        {
            //If the if statement above happens, a new cell is added
            MacrophageDataBuffer [OffsetToPixelData].MacrophageLives [NumberOfCellsPresentAtCoord] = -1;
            
            
            //The line above sets the new cell's life to -1
            //The number of cells present at the pixel is NOT incremented - or it'll affect subsequent arrivals,
            //which is not a desired effect
        }
        //}
        /*else
        {
        //number of cells present is zero, and we are using 'valueforx0' to get
        //probability
        unsigned bigProbOfArrival = MacroArrivalFuncInfo.ValueForx0;
        if ( (bigProbOfArrival > ((unsigned)FastRand65535()<<16)+FastRand65535() ) && (NumberOfCellsPresentAtCoord < MAX_MACROPHAGES_PER_PIXEL))
        MacrophageDataBuffer [OffsetToPixelData].MacrophageLives [NumberOfCellsPresentAtCoord] = -1;
        //cell is added, as before
    }*/
    }
}

void PlaceNewSMCs (void)
{
    int i;
    int Newx,Newy;
    unsigned short ProbOfArrival;
    int NumberOfSMCsPresentAtCoord;
    int NumberOfMacrosSurroundingCoord;
    int OffsetToPixelData;
    for (i=0;i<NumSMCContactsPerCycle;i++)
    {
        //execute this loop NSMCCPC times
        
        //Get random coords
        Newx = FastRandMAX_X ();	//Newx is between 0 and CELL_BUFFER_X_SIZE-1
        Newy = FastRandMAX_Y ();	//Newy is between 0 and CELL_BUFFER_Y_SIZE-1
        if (Newx >= CELL_BUFFER_X_SIZE || Newy >= CELL_BUFFER_Y_SIZE)
            continue;

        OffsetToPixelData =Newy*CELL_BUFFER_X_SIZE+Newx;
        
        //We want the number of MACROPHAGES surrounding the point as this determines the arrival rate of
        //SMCs
        NumberOfMacrosSurroundingCoord =MacrophageDataBuffer [OffsetToPixelData].NoofSurroundingCells;
        
        //Get number of cells present on coord
        NumberOfSMCsPresentAtCoord = SMCDataBuffer [OffsetToPixelData].NoofCellsOnPixel;
        //Get probability of cell arriving given number of cells present
        //by looking at the yValues data from the arrival function
        //each of the y values is between 0 and 65535 (rather than the 100% actually printed)
        //This fits in the random number generator very well
        ProbOfArrival = SMCArrivalFuncInfo.yValuesDataForEveryxValue[NumberOfMacrosSurroundingCoord];
        
        if (ProbOfArrival > FastRand65535 () && NumberOfSMCsPresentAtCoord < MAX_SMCS_PER_PIXEL)
        {
            //If the if statement above happens, a new cell is added
            SMCDataBuffer [OffsetToPixelData].NoofCellsOnPixel++;
            TotalSMCs++;
        }
        //For the SMCs, this is done simply by incrementing the variable above. No marking is necessary,
        //because the smc arrival rate does not depend on the number of smcs present
    }
}

void AddSurroundingCells(int x, int y, int amount)
{
    for (int _x = x-1; _x <= x+1; _x++)
    {
        if (_x < 0 || _x >= CELL_BUFFER_X_SIZE)
            continue;
        for (int _y = y-1; _y <= y+1; _y++)
        {
            int yact = _y;
            if (_y < 0)
                yact = CELL_BUFFER_Y_SIZE-1;
            if (_y >= CELL_BUFFER_Y_SIZE)
                yact = 0;
            MacrophageDataBuffer [yact*CELL_BUFFER_X_SIZE + _x].NoofSurroundingCells += amount;
        }

    }
}


void CalculateNewMacroCounts (void)
{
    int h,v;
    int i=0;
    for (v=0;v<CELL_BUFFER_Y_SIZE;v++)
    {
        for (h=0;h<CELL_BUFFER_X_SIZE;h++)
        {
            while (MacrophageDataBuffer [i].MacrophageLives [MacrophageDataBuffer [i].NoofCellsOnPixel])
            {
                TotalNumMacros++;
                /* The way that while statement works is that MDB [i].NCOP contains the number of cells present on
                that pixel. MDB[i].MacrophageLives [NoofCellsPresent-1] is the life of the last cell present, and
                MDB[i].MacrophageLives [NoofCellsPresent] should be zero. If it isn't zero, then a new cell has been added
                and all the cell counts are incremented
                */
                //increment the cell count for that pixel
                MacrophageDataBuffer [i].NoofCellsOnPixel++;
                //and the cell counts for all the surrounding pixels                
                AddSurroundingCells(h,v,1);
            }
            i++;
        }
    }
}

void MarkDeadCells (void)
{
    int i;
    int v;
    int NoofCellsSurrounding;
    unsigned short DeathRate;
    unsigned short OldLife;
    for (i=0;i<CELL_BUFFER_X_SIZE*CELL_BUFFER_Y_SIZE;i++)
    {
        RemoveLipid (i);
        NoofCellsSurrounding = MacrophageDataBuffer [i].NoofSurroundingCells;
        DeathRate = MacroDeathFuncInfo.yValuesDataForEveryxValue[NoofCellsSurrounding];
        //Now the death rate must be subtracted from the lives of every cell present
        
        for (v=0;v<MacrophageDataBuffer [i].NoofCellsOnPixel;v++)
        {
            OldLife = MacrophageDataBuffer [i].MacrophageLives[v];
            MacrophageDataBuffer [i].MacrophageLives[v]-=DeathRate;
            if (MacrophageDataBuffer [i].MacrophageLives[v]>OldLife)
            {
                MacrophageDataBuffer [i].MacrophageLives[v]=0;
                DoLipidArrival (i);
            }
        }
    }
}

void RemoveDeadCells (void)
{
    int i=0,v,h,NoofCellsRemaining,c;
    for (v=0;v<CELL_BUFFER_Y_SIZE;v++)
    {
        for (h=0;h<CELL_BUFFER_X_SIZE;h++)
        {
            NoofCellsRemaining =MacrophageDataBuffer [i].NoofCellsOnPixel;
            for (c=0;c<MacrophageDataBuffer [i].NoofCellsOnPixel && NoofCellsRemaining;c++)
            {
                NoofCellsRemaining--;
                
                if (!MacrophageDataBuffer [i].MacrophageLives[c])
                {
                    MoveMemory (&MacrophageDataBuffer [i].MacrophageLives[c],&MacrophageDataBuffer [i].MacrophageLives[c+1],NoofCellsRemaining*sizeof (short int));
                    MacrophageDataBuffer [i].MacrophageLives[NoofCellsRemaining] = 0;
                    c--;
                    TotalNumMacros--;                    
                    MacrophageDataBuffer [i].NoofCellsOnPixel--;

                    AddSurroundingCells(h,v,-1);                    
                }
            }
            i++;
        }
    }
}

void DoLipidArrival (int i)
{
    if (LipidDataBuffer[i].NoofBlobsOnPixel < (MAX_LIPIDS_PER_PIXEL<<2)) 
    {
        if (FastRand65535 () < LipidArrivalRate)
        {
            LipidDataBuffer[i].NoofBlobsOnPixel++;
            TotalLipids++;
        }
    }
}

void RemoveLipid (int i)
{
    if (LipidDataBuffer[i].NoofBlobsOnPixel)
    {
        if (FastRand65535 () < LipidRemovalRate)
            LipidDataBuffer[i].NoofBlobsOnPixel--;
    }
}


void DrawMacrophageBuffer (void)
{
    if (!MacrophageDataBuffer)
        return;
    
    //First, a pointer is obtained to the video buffer to which we will draw the macrophages
    WORD *DrawMemory = (WORD*) LockCellSurface ();	//LockBackSurface does this
    int i;
    int surfIdx = 0;
    for (i=0;i<CELL_BUFFER_X_SIZE*CELL_BUFFER_Y_SIZE;i++)
    {
        if (SMCDataBuffer[i].NoofCellsOnPixel && DrawSMCs) {
            DrawMemory[surfIdx++] = SMCColourTable[SMCDataBuffer [i].NoofCellsOnPixel];
            /*
            DrawMemory[surfIdx++] = SMCColourTable[SMCDataBuffer [i].NoofCellsOnPixel] >> 8;
            DrawMemory[surfIdx++] = SMCColourTable[SMCDataBuffer [i].NoofCellsOnPixel] >> 16;
            */
        }
        else if (MacrophageDataBuffer[i].NoofCellsOnPixel && DrawMacrophages) {
            DrawMemory[surfIdx++] = MacroColourTable[MacrophageDataBuffer [i].NoofCellsOnPixel];
            ///DrawMemory[surfIdx++] = MacroColourTable[MacrophageDataBuffer [i].NoofCellsOnPixel] >> 8;
            //DrawMemory[surfIdx++] = MacroColourTable[MacrophageDataBuffer [i].NoofCellsOnPixel] >> 16;
        }
        else if (DrawLipids) {
            DrawMemory[surfIdx++] = LipidColourTable[LipidDataBuffer [i].NoofBlobsOnPixel>>2];
            //DrawMemory[surfIdx++] = SMCColourTable[LipidDataBuffer [i].NoofBlobsOnPixel>>2] >> 8;
            //DrawMemory[surfIdx++] = SMCColourTable[LipidDataBuffer [i].NoofBlobsOnPixel>>2] >> 16;
        }
        else  {
            //artery wall
            DrawMemory[surfIdx++] = LipidColourTable [0];
        }
    }
    UnlockCellSurface ();
    
    CellSurfaceChanged = true;
}

int GetTotalCellCount()
{
    return TotalNumMacros + TotalLipids + TotalSMCs;
}

void CheckBreak ()
{
    if (BreakSimulation)
        DebugBreak ();
}









