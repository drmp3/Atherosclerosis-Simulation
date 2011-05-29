#include "athero.h"
#include <math.h>

LRESULT CALLBACK FunctionAdjuster_WndProc (HWND hwnd, UINT Message,WPARAM wParam, LPARAM lParam);
BOOL FunctionAdjuster_OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct);
void FunctionAdjuster_OnPaint (HWND hwnd);
void FunctionAdjuster_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
void FunctionAdjuster_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
void GenerateyValuesForEveryxValue (FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo);
int ReadValueOffFunctionAdjuster (int xValue,FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo);


HWND CreateFunctionAdjusterWindow (FUNCTIONADJUSTERINFO *fInfo,int xpos,int ypos)
{
// fInfo is a pointer to a FUNCTIONADJUSTERINFO structure - this structure contains all the information
// needed to display a function adjuster and also contains the function's y values

//First we create the window for the function adjuster to exist in. This window is a child to the
//Main simulation window
	return CreateWindowEx (	//The window creation paramaters follow:
	WS_EX_TOOLWINDOW|WS_EX_OVERLAPPEDWINDOW|WS_EX_DLGMODALFRAME	,		//'Extended window style' - makes the window have a small title bar
	FUNCTIONADJUSTERNAME,	//The name of the function adjuster class
	fInfo->Title,	//The window title from the given info structure
	WS_VISIBLE | WS_POPUP | WS_OVERLAPPED|WS_CAPTION| WS_SYSMENU,	//Makes the window visible, have a title bar, etc
	xpos,ypos,			//x,y position of window on screen
	fInfo->xsize,fInfo->ysize,	//Width,Height of window
	AtheroWindow,			//Parent window handle - this makes this window a child of the main simulation window
	NULL,		//No menu
	AtheroInstance,	//Handle to application saying this application is creating the window
	fInfo);		//Finally, we give the window the address of the function adjuster structure, so it can use
					//this to draw the graph
}



BOOL RegisterFunctionAdjusterWindow (void)
{
//This registers a new window class in the name of 'Function adjuster'
//When this class is used to create a window, the window will have a graph on it
//which can be used to control the arrival chances and death rates of the cells.
	WNDCLASS New_window;
	New_window.style = CS_HREDRAW | CS_VREDRAW;
	New_window.lpfnWndProc = FunctionAdjuster_WndProc;	//This bit determines the windows message handler
													//It will be set to the routine that draws and adjusts the graph
	New_window.cbClsExtra = 0;
	New_window.cbWndExtra = 8;		//This makes windows allocate 4 extra bytes in the windows memory
															//for the FUNCTIONADJUSTERINFO structure pointer
	New_window.hInstance= AtheroInstance;
	New_window.hIcon = NULL;
	New_window.hCursor = LoadCursor (NULL,IDC_ARROW);
	New_window.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	New_window.lpszMenuName = NULL;
	New_window.lpszClassName= FUNCTIONADJUSTERNAME;
	return (RegisterClass (&New_window) != 0);
}


LRESULT CALLBACK FunctionAdjuster_WndProc (HWND hwnd, UINT Message,WPARAM wParam, LPARAM lParam)
//This function handles messages for a function adjuster window,
//for example, when it recieves 'draw' messages, it draws the function graph.
{
FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo;
	switch (Message)	//Message is an integer, which represents the type of message that has been sent
	{
		HANDLE_MSG (hwnd,WM_CREATE,FunctionAdjuster_OnCreate);	//When the window is being created, the
							//fucntionadjusterinfo structure must be associated with the window, so when the
							//WM_CREATE message is sent, this is done
		HANDLE_MSG (hwnd,WM_PAINT,FunctionAdjuster_OnPaint);	//If a paint message is sent, the graph drawing
										//routine is called
		HANDLE_MSG (hwnd,WM_MOUSEMOVE,FunctionAdjuster_OnMouseMove);
										//if the mouse moves over the window, then the MouseMove routine is called
	case WM_LBUTTONDOWN:
		FunctionAdjuster_OnMouseMove (hwnd,LOWORD (lParam),HIWORD(lParam),wParam);
		return 0;
		//If the user clicks the mouse, the move mouse routine is called, as the same effect is needed
		HANDLE_MSG (hwnd,WM_LBUTTONUP,FunctionAdjuster_OnLButtonUp);
	case WM_DESTROY:
		 //When closing, get the graph info structure from the window and set it so GraphVisible is zero
		lpFunctionAdjusterInfo = (FUNCTIONADJUSTERINFO *)GetWindowLong (hwnd,0);
		lpFunctionAdjusterInfo->GraphVisible = FALSE;
		GenerateyValuesForEveryxValue (lpFunctionAdjusterInfo);
      //And also calculate y values for every x value for the graph
		return 0;
	default:
		return DefWindowProc (hwnd,Message,wParam,lParam);	//Pass any unhandled messages onto the default
																		//message handler (internal in Windows)
	}

}

BOOL FunctionAdjuster_OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{
	SetWindowLong (hwnd,0,(LONG)lpCreateStruct->lpCreateParams);
	//What this does is to set a value in the memory allocated to the window to point to the FUNCTIONADJUSTERINFO
	//structure given by the caller
	FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo =(FUNCTIONADJUSTERINFO *)lpCreateStruct->lpCreateParams;
	InitialiseFunctionAdjuster (lpFunctionAdjusterInfo);
	lpFunctionAdjusterInfo->GraphVisible = 1;
	return TRUE;
}

void InitialiseFunctionAdjuster (FUNCTIONADJUSTERINFO * lpFunctionAdjusterInfo)
{
	if (lpFunctionAdjusterInfo->Initialised)
		return;
	lpFunctionAdjusterInfo->yValuesDataForEveryxValue = (unsigned short*)GlobalAlloc (GPTR,(lpFunctionAdjusterInfo->Max_x_value+1)*sizeof (unsigned short));
	//The last two lines allocate memory for the yValuesDataForEveryxValue integer table which will store the
	//y values for every x value on the graph
	GenerateyValuesForEveryxValue (lpFunctionAdjusterInfo);
	lpFunctionAdjusterInfo->Initialised = TRUE;
}



void FunctionAdjuster_OnPaint (HWND hwnd)
{
	FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo;	//lpFuncadjinfo is a pointer to a FUNCTIONADJ. structure
	lpFunctionAdjusterInfo = (FUNCTIONADJUSTERINFO *)GetWindowLong (hwnd,0);
	//And we set this pointer to the value we saved in the OnCreate routine
	//Now we can use this structure to draw the function adjuster
	PAINTSTRUCT PaintStruct;
	HDC PaintDC = BeginPaint  (hwnd,&PaintStruct);

	//Now PaintDC is a handle we can use to draw to the FunctionAdjuster's window
	SetTextCharacterExtra (PaintDC,-1);
	RECT DrawArea;
	GetClientRect (hwnd,&DrawArea);	//This fills in the rectangle structure DrawArea (left,top,right,bottom)
	//With the client area (drawable area) of the function adjuster's window.
	//This will be used to determine the size of the axis to be drawn
	SIZE SizeOfyAxisText,SizeOfxAxisText;
	GetTextExtentPoint32 (PaintDC,lpFunctionAdjusterInfo->yAxisName,strlen (lpFunctionAdjusterInfo->yAxisName),&SizeOfyAxisText);
	//Now SizeOfyAxisText contains the width and height of the text to put by the y axis, so we will plot the
	//y axis line just to the right of this.
	GetTextExtentPoint32 (PaintDC,lpFunctionAdjusterInfo->xAxisName,strlen (lpFunctionAdjusterInfo->yAxisName),&SizeOfxAxisText);
	//And this does the same for the x axis text
	SetBkMode (PaintDC,TRANSPARENT);	//Makes the text have no background colour

	//Now we work out on what row we want to put the x axis, given the height of its text
	int xAxisRow = DrawArea.bottom - SizeOfxAxisText.cy *2 - 7; //xAxisRow is y coord of x Axis
	lpFunctionAdjusterInfo->xAxisRow = xAxisRow;		//the x axis row is saved into the structure

	int yAxisColumn = SizeOfyAxisText.cx+10;//yAxisColumn is x coord of y Axis
	lpFunctionAdjusterInfo->yAxisColumn = yAxisColumn;	//The y axis column is saved in the structure

	int xAxisLength = DrawArea.right - yAxisColumn - 10;	//the x axis length is computed
	lpFunctionAdjusterInfo->xAxisLength = xAxisLength;		//and saved in this structure

	int yAxisTopRow = 10;			//The starting height of the y axis, from the top of the window

	int yAxisLength = xAxisRow - yAxisTopRow;								//The same goes for the y axis
	lpFunctionAdjusterInfo->yAxisLength = yAxisLength;


		//we will plot the y axis text 5 pixels from the left of the window
											//so the column we will draw the y axis line in is 5 pixels from the right of the text
											// (5 pixels + width of text + 5 pixels)

	int TextxPos =5;	// x coord of text set
	int TextyPos =DrawArea.bottom/2-SizeOfyAxisText.cy/2;	//y coord of text set
	TextOut (PaintDC,TextxPos,TextyPos,lpFunctionAdjusterInfo->yAxisName,strlen (lpFunctionAdjusterInfo->yAxisName));
	//That draws the y axis text 5 pixels from the left hand side, half way down the window

	//Draw the y axis line...
	MoveToEx (PaintDC,yAxisColumn,yAxisTopRow,NULL);
	LineTo (PaintDC,yAxisColumn,xAxisRow);
	//So a line is drawn from (yAxisColumn,5) (coordinates are measured from top left of window) to (yAxisColumn,xAxisRow)
	//And this will be our y-axis



	//Now we draw the x-axis line
	MoveToEx (PaintDC,yAxisColumn,xAxisRow,NULL);
	LineTo (PaintDC,DrawArea.right-10,xAxisRow);
	//The x axis is from (yAxisColumn,xAxisRow) to (yAxisColumn,Width of window - 10)
	//Now we put the x-axis label text on the screen
	TextOut (PaintDC,(DrawArea.right-SizeOfxAxisText.cx)/2,xAxisRow+SizeOfxAxisText.cy+6,lpFunctionAdjusterInfo->xAxisName,strlen (lpFunctionAdjusterInfo->xAxisName));
	//Now we write the text at the top of the y axis indicating the 'printed maximum value'
	SetTextAlign (PaintDC,TA_RIGHT);	//This lets us specifiy the right edge of the text
												//when we draw it - so I dont need to calculate the
												//x coordinate of the text to get the right position



	//Now we must graduate the y axis with a logarithmic scale
	char NumberString [10];
	double logScaleFactor = (double) yAxisLength/log10 (100);
		//Now to find the vertical height of a point at (x,y) we go log10(y+1)*logScaleFactor
	int GraduationInterval = 10;
	//We will graduate the y axis every GraduationInterval points
	int yCurPos;
	for (yCurPos = 0; yCurPos <= 100;yCurPos+=GraduationInterval)
	{
		int HeightFromxAxis = log10 (yCurPos+1) * logScaleFactor;	//work out height from x axis, as above
		int yCoord = yAxisTopRow + yAxisLength-HeightFromxAxis;	//work out y coord from height
		MoveToEx (PaintDC,yAxisColumn,yCoord,NULL);		//Draw a small graduation line at this height, 5 pixels across
		LineTo (PaintDC,yAxisColumn-5,yCoord);	//And draw the t
		if (yCurPos < 10)
		{
			yCurPos -= 9;
			continue;
		}
		sprintf (NumberString,"%d",yCurPos);
		TextOut (PaintDC,yAxisColumn-10,yCoord-SizeOfxAxisText.cy/2,NumberString,strlen (NumberString));
	}

	SetTextAlign (PaintDC,TA_CENTER);

	//This next section draws the graph and numbers the x axis.

	int count;
	int NumberOfPoints = lpFunctionAdjusterInfo->NumPointsOnGraph;
	int xCoordOfMark;
	int NumChars;
	for (count =0;count <= NumberOfPoints;count++)
	{
		int HeightOfPoint =xAxisRow-log10(lpFunctionAdjusterInfo->yValuesDataForGraph[count]+1)*logScaleFactor;
		xCoordOfMark = yAxisColumn+xAxisLength*(float)count/NumberOfPoints;
		if (count)
		{
			LineTo (PaintDC,xCoordOfMark,HeightOfPoint);
		}
		//This loop will go across the x axis, numbering the axis
		MoveToEx (PaintDC,xCoordOfMark,xAxisRow,NULL);
		LineTo (PaintDC,xCoordOfMark,xAxisRow+5);	//draw a little line vertically downwards
		NumChars = sprintf (NumberString,"%d",(count*lpFunctionAdjusterInfo->Max_x_value)/NumberOfPoints);
		//The line above makes the string with the number in it
		TextOut (PaintDC,xCoordOfMark,xAxisRow+6,NumberString,NumChars);	//Put the number on the screen
		//And we also draw the polygon line that makes up the graph
		MoveToEx (PaintDC,xCoordOfMark,HeightOfPoint,NULL);
	}
}

void FunctionAdjuster_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
//if the left mouse button is down, then we will adjust the graph
	if (keyFlags == MK_LBUTTON)
	{
		// x,y is the coordinate of the mouse click, which is all that is needed for this routine
		//x,y are relative to the top-left of the client area (drawable area) of the graph window
		FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo = (FUNCTIONADJUSTERINFO *)GetWindowLong (hwnd,0);
		//lpFunctionAdjusterInfo is the structure with information about the graph in it.

		//Now the x coordinate is converted into a value which indicates which point we are adjusting
		// 0 means the leftmost point, NumPointsOnGraph is the rightmost point
		// This is done by making the coordinate relative to the graph origin
		x-=lpFunctionAdjusterInfo->yAxisColumn;
		y=lpFunctionAdjusterInfo->xAxisRow-y;	//The y coordinate is now 0 at x axis and xAxisRow at top of y axis

		double logScaleFactor = (double) lpFunctionAdjusterInfo->yAxisLength/log10 (100);
		double yValueOfPointToAdjust = pow (10,(double)y/logScaleFactor) -1.0;	//Do the conversion from log to linear

		if (yValueOfPointToAdjust>=0 && yValueOfPointToAdjust<=100)	//If the y value is negative, dont move the point
		{
			int IndexOfPointToAdjust=(((x*lpFunctionAdjusterInfo->NumPointsOnGraph*16)/lpFunctionAdjusterInfo->xAxisLength)+8)/16;
					//We divide the x coordinate by the x axis length so that 0 represents the left of the graph and
					//1 represents the right, and then multiply by NumPointsOnGraph, so 0 represents the left and
					//NumPointsOnGraph represents the right
					//This number is an integer, and is the index to the point we want to adjust
			if (IndexOfPointToAdjust >= 0 && IndexOfPointToAdjust <=lpFunctionAdjusterInfo->NumPointsOnGraph)
			//If the IndexOfPointtoadjust is not in an acceptable range, dont move any points
			{
				lpFunctionAdjusterInfo->yValuesDataForGraph [IndexOfPointToAdjust] =yValueOfPointToAdjust;	//Now the point data has changed,
																					//The graph is redrawn:
				InvalidateRect (hwnd,NULL,TRUE);	//Tell windows the graph needs redrawing
				UpdateWindow (hwnd);		//And send the window a WM_PAINT message (which will go straight to the graph
				//drawing routine
			}
		}
	}
}

void FunctionAdjuster_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
//When the user releases the mouse button, the program calculates the y value for every x value on the
//graph, placing the numbers in the yValuesForEveryxValue table
		FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo = (FUNCTIONADJUSTERINFO *)GetWindowLong (hwnd,0);
		GenerateyValuesForEveryxValue (lpFunctionAdjusterInfo);
}


int ReadValueOffFunctionAdjuster (int xValue,FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo)
{
	//lpFunctionAdjusterInfo is our pointer to the structure
	//xValue is the x coordinate on the graph for which the y coordinate is to be returned
	//First, the xValue is converted into a floating point which represents the index number of the point
	//to be adjusted (0 = leftmost point,NumPointsOnGraph = rightmost point)
	double xIndex = (double)(xValue*lpFunctionAdjusterInfo->NumPointsOnGraph)/(double)lpFunctionAdjusterInfo->Max_x_value;
	//xIndex will not necessarily be an integer number, and the fractional part of this number represents
	//where exactly the point lies
	//for example, 1.2 repesents 2/10ths of the way between point 1 and point 2, so the value to return is
	//0.2 * y value of point 2 + 0.8 * y value of point 1
	double IntOfxIndex;
	double FracOfxIndex = modf (xIndex,&IntOfxIndex);	//Frac of xIndex is the fractional part of x index
	double yValuePercentage =lpFunctionAdjusterInfo->yValuesDataForGraph [int(IntOfxIndex)]*(1-FracOfxIndex)+
				lpFunctionAdjusterInfo->yValuesDataForGraph [int(IntOfxIndex)+1]*FracOfxIndex;
	return (yValuePercentage * lpFunctionAdjusterInfo->Max_y_value/100.0);	//and the calculation is done in the return statement
}

void GenerateyValuesForEveryxValue (FUNCTIONADJUSTERINFO *lpFunctionAdjusterInfo)
{
//This function generates a y value for every integer x value in the function adjuster,
//saving it the yValuesTable
	int i;
	for (i=0;i<=lpFunctionAdjusterInfo->Max_x_value;i++)
		lpFunctionAdjusterInfo->yValuesDataForEveryxValue[i]=ReadValueOffFunctionAdjuster (i,lpFunctionAdjusterInfo);
}











