// FunctionAdjusterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "athero.h"
#include "FunctionAdjusterDlg.h"
#include "EnterValueForx0Dlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFunctionAdjusterDlg dialog


CFunctionAdjusterDlg::CFunctionAdjusterDlg(CWnd* pParent /*=NULL*/ ,FUNCTIONADJUSTERINFO *pFuncAdjInfo)
	: CDialog(CFunctionAdjusterDlg::IDD, pParent)
{
	lpFunctionAdjusterInfo = pFuncAdjInfo;
	if (!lpFunctionAdjusterInfo->Initialised)
	{
		lpFunctionAdjusterInfo->yValuesDataForEveryxValue = (unsigned short*)GlobalAlloc (GPTR,(lpFunctionAdjusterInfo->Max_x_value+1)*sizeof (unsigned short));
	//The last two lines allocate memory for the yValuesDataForEveryxValue integer table which will store the
	//y values for every x value on the graph
		GenerateyValuesForEveryxValue (lpFunctionAdjusterInfo);
		lpFunctionAdjusterInfo->Initialised = true;
	}
	//{{AFX_DATA_INIT(CFunctionAdjusterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFunctionAdjusterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFunctionAdjusterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFunctionAdjusterDlg, CDialog)
	//{{AFX_MSG_MAP(CFunctionAdjusterDlg)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_SETFIRSTXVALUE, OnSetfirstxvalue)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFunctionAdjusterDlg message handlers

void CFunctionAdjusterDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	//Now we can use this structure to draw the function adjuster
	//Now dc is a CPaintDC class  we can use to draw to the FunctionAdjuster's window
	dc.SetTextCharacterExtra (-1);
	CFont Font;
	Font.CreateFont (14,0,0,0,FW_DONTCARE,0,0,0,0,0,0,0,DEFAULT_PITCH|FF_MODERN,"Arial");
	dc.SelectObject (&Font);

	RECT DrawArea;
	GetClientRect (&DrawArea);	//This fills in the rectangle structure DrawArea (left,top,right,bottom)
	//With the client area (drawable area) of the function adjuster's window.
	//This will be used to determine the size of the axis to be drawn
	SIZE SizeOfyAxisText,SizeOfxAxisText;
	GetTextExtentPoint32 (dc.m_hDC,lpFunctionAdjusterInfo->yAxisName,strlen (lpFunctionAdjusterInfo->yAxisName),&SizeOfyAxisText);
	//Now SizeOfyAxisText contains the width and height of the text to put by the y axis, so we will plot the
	//y axis line just to the right of this.
	GetTextExtentPoint32 (dc.m_hDC,lpFunctionAdjusterInfo->xAxisName,strlen (lpFunctionAdjusterInfo->yAxisName),&SizeOfxAxisText);
	//And this does the same for the x axis text
	dc.SetBkMode (TRANSPARENT);	//Makes the text have no background colour

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
	dc.TextOut (TextxPos,TextyPos,lpFunctionAdjusterInfo->yAxisName,strlen (lpFunctionAdjusterInfo->yAxisName));
	//That draws the y axis text 5 pixels from the left hand side, half way down the window

	//Draw the y axis line...
	dc.MoveTo (yAxisColumn,yAxisTopRow);
	dc.LineTo (yAxisColumn,xAxisRow);
	//So a line is drawn from (yAxisColumn,5) (coordinates are measured from top left of window) to (yAxisColumn,xAxisRow)
	//And this will be our y-axis



	//Now we draw the x-axis line
	dc.MoveTo (yAxisColumn,xAxisRow);
	dc.LineTo (DrawArea.right-10,xAxisRow);
	//The x axis is from (yAxisColumn,xAxisRow) to (yAxisColumn,Width of window - 10)
	//Now we put the x-axis label text on the screen
	dc.TextOut ((DrawArea.right-SizeOfxAxisText.cx)/2,xAxisRow+SizeOfxAxisText.cy+6,lpFunctionAdjusterInfo->xAxisName,strlen (lpFunctionAdjusterInfo->xAxisName));
	//Now we write the text at the top of the y axis indicating the 'printed maximum value'
	dc.SetTextAlign (TA_RIGHT);	//This lets us specifiy the right edge of the text
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
		dc.MoveTo (yAxisColumn,yCoord);		//Draw a small graduation line at this height, 5 pixels across
		dc.LineTo (yAxisColumn-5,yCoord);	//And draw the t
		if (yCurPos < 10)
		{
			yCurPos -= 9;
			continue;
		}
		sprintf (NumberString,"%d",yCurPos);
		dc.TextOut (yAxisColumn-10,yCoord-SizeOfxAxisText.cy/2,NumberString,strlen (NumberString));
	}

	dc.SetTextAlign (TA_CENTER);

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
			dc.LineTo (xCoordOfMark,HeightOfPoint);
		}
		//This loop will go across the x axis, numbering the axis
		dc.MoveTo (xCoordOfMark,xAxisRow);
		dc.LineTo (xCoordOfMark,xAxisRow+5);	//draw a little line vertically downwards
		NumChars = sprintf (NumberString,"%d",(count*lpFunctionAdjusterInfo->Max_x_value)/NumberOfPoints);
		//The line above makes the string with the number in it
		dc.TextOut (xCoordOfMark,xAxisRow+6,NumberString,NumChars);	//Put the number on the screen
		//And we also draw the polygon line that makes up the graph
		dc.MoveTo (xCoordOfMark,HeightOfPoint);
	}
	Font.DeleteObject();
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CFunctionAdjusterDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (nFlags == MK_LBUTTON)
	{
		int x= point.x, y=point.y;

		// x,y is the coordinate of the mouse click, which is all that is needed for this routine
		//x,y are relative to the top-left of the client area (drawable area) of the graph window
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
				InvalidateRect (NULL,TRUE);	//Tell windows the graph needs redrawing
				UpdateWindow ();		//And send the window a WM_PAINT message (which will go straight to the graph
				//drawing routine
			}
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CFunctionAdjusterDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GenerateyValuesForEveryxValue (lpFunctionAdjusterInfo);
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CFunctionAdjusterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(lpFunctionAdjusterInfo->Title);
	SetWindowPos (NULL,100,100,lpFunctionAdjusterInfo->xsize,
		lpFunctionAdjusterInfo->xsize,SWP_SHOWWINDOW); 	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CFunctionAdjusterDlg::OnSetfirstxvalue() 
{
	// TODO: Add your command handler code here
	CEnterValueForx0Dlg Valueforx0 (this,lpFunctionAdjusterInfo);
	Valueforx0.DoModal ();
	UpdateWindow ();
}

void CFunctionAdjusterDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	RedrawWindow ();	
}
