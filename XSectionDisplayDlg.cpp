// XSectionDisplayDlg.cpp : implementation file
//

#include "athero.h"
#include <math.h>
#include "XSectionDisplayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXSectionDisplayDlg dialog
CXSectionDisplayDlg *pCurrentDisplayGraph;

CXSectionDisplayDlg::CXSectionDisplayDlg(CWnd* pParent /*=NULL*/, POINT* pstartpoint, POINT *pendpoint)
	: CDialog(CXSectionDisplayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXSectionDisplayDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Startpoint = *pstartpoint;
	m_Endpoint = *pendpoint;
	//translate these points to cell buffer positions
	RECT rect;
	::GetClientRect (AtheroWindow,&rect);
	float scalefactorx = (float)CELL_BUFFER_X_SIZE /(float)rect.right;
	float scalefactory = (float)CELL_BUFFER_Y_SIZE / (float)rect.bottom;
	m_Endpoint.x*=scalefactorx;
	m_Endpoint.y*=scalefactory;
	m_Startpoint.x*=scalefactorx;
	m_Startpoint.y*=scalefactory;
}


void CXSectionDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXSectionDisplayDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXSectionDisplayDlg, CDialog)
	//{{AFX_MSG_MAP(CXSectionDisplayDlg)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_COMMAND(CM_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXSectionDisplayDlg message handlers

int AveragePoints (POINT *input, POINT *output, int NumInput)
{
	int iindex =0;
	int NumOut = 0;
	while (iindex < NumInput) {
		int cnt = 0,sy = 0, sx =0;;
		output[NumOut].x = input[iindex].x;
		int begidx = iindex;
		while ((iindex < NumInput) && (cnt < 4)) {
			sy += input[iindex++].y;
			cnt++;
		}
		//iindex goes from 0 to NumInput, the expression in square brackets below evaluates to begidx + 0 when iindex == 0
		//and begidx + cnt when iindex == NumInput. This enables the x-range of the averaged coords
		//to span the entire x-range of the input coords (bit of a hack i suppose)
		output[NumOut].x = input[begidx + (((cnt-1) * (iindex))/NumInput)].x; 
		if (NumOut == 0)
			output[NumOut].x = 0; //save fiddling with the func
		output[NumOut++].y = sy/cnt;
	}
	return NumOut;
}

int DoSubdivision (POINT *input, POINT *output, int NumInput)
{
/*	output[0].x = input[0].x;
	output[0].y = input[0].y;

	output[1].x = (input[0].x + input[1].x)/2;
	output[1].y = (input[0].y + input[1].y)/2;
*/
	int curout = 0;
	for (int i = 2; i< NumInput-2;i++) {
		output[curout].x = (input[i-2].x + input[i-1].x*28 + input[i].x*70 + input[i+1].x*28 + input[i+2].x)/128;
		output[curout++].y = (input[i-2].y + input[i-1].y*28 + input[i].y*70 + input[i+1].y*28 + input[i+2].y)/128;
		output[curout].x = (input[i-1].x*8  + input[i].x*56 + input[i+1].x*56 + input[i+2].x*8)/128;
		output[curout++].y = (input[i-1].y*8  + input[i].y*56 + input[i+1].y*56 + input[i+2].y*8)/128;
		
	}

	return curout;
}

void CXSectionDisplayDlg::OnPaint() 
{

	EnterCriticalSection (&LockCritSection);

	{
	CPaintDC dc (this); // device context for painting
	
	// TODO: Add your message handler code here
	if (!MacrophageDataBuffer)
		return;

	const int subdivisions = 2;
	const int points_add_to_beg = 6;
	const int points_add_to_end = 6;

	const int graph_y_height_in_cells = /*MAX_MACROPHAGES_PER_PIXEL+*/MAX_SMCS_PER_PIXEL+MAX_LIPIDS_PER_PIXEL; 
	//don't include macros cos they die out eventually!


	RECT rect;
	GetClientRect (&rect);
	int GraphxSize = rect.right;
	int GraphySize = rect.bottom;
	

	int left =  m_Endpoint.x < m_Startpoint.x ? m_Endpoint.x : m_Startpoint.x;
	int top  =  m_Endpoint.y < m_Startpoint.y ? m_Endpoint.y : m_Startpoint.y;
	int right = (left == m_Endpoint.x) ? m_Startpoint.x : m_Endpoint.x;
	int bottom = (top == m_Endpoint.y) ? m_Startpoint.y : m_Endpoint.y;
	int dx = right-left+1;
	int dy = bottom-top+1;
	int StepX;
	int NumPoints;

	if (dx > dy) {
		StepX = 1;
		NumPoints = dx;
	}
	else { 
		StepX = 0;
		NumPoints = dy;
	}

	POINT * MacroPoints = new POINT [(NumPoints+points_add_to_beg+points_add_to_end) << subdivisions]; 
	POINT * SMCPoints = new POINT [(NumPoints+points_add_to_beg+points_add_to_end) << subdivisions]; 
	POINT * LipidPoints = new POINT [(NumPoints+points_add_to_beg+points_add_to_end) << subdivisions]; 
	POINT * TempPoints = new POINT [(NumPoints+points_add_to_beg+points_add_to_end) << subdivisions];

	float Curx = m_Startpoint.x;
	float Cury = m_Startpoint.y;
	float Incy = ((float)(m_Endpoint.y - m_Startpoint.y)/(float)(m_Endpoint.x - m_Startpoint.x));
	float Incx = ((float)(m_Endpoint.x - m_Startpoint.x)/(float)(m_Endpoint.y - m_Startpoint.y));
	float CurGraphx = 0;
	float CurGraphInc = (float)GraphxSize / ((float)NumPoints-1);
	int c =0;
	while (c < NumPoints) {
		MacroPoints[c].x = SMCPoints[c].x = LipidPoints[c].x = CurGraphx;
		int index = CELL_BUFFER_X_SIZE*int(Cury)+int(Curx);
		int NoofMacros = MacrophageDataBuffer[index].NoofCellsOnPixel;
		int NoofSMCs = SMCDataBuffer[index].NoofCellsOnPixel;
		int NoofLipids = LipidDataBuffer[index].NoofBlobsOnPixel;
		LipidPoints[c].y = GraphySize - (NoofLipids * GraphySize) / graph_y_height_in_cells;
		MacroPoints[c].y = LipidPoints[c].y - (NoofMacros * GraphySize) / graph_y_height_in_cells;
		SMCPoints[c].y = MacroPoints[c].y - (NoofSMCs * GraphySize) / graph_y_height_in_cells;
		CurGraphx += CurGraphInc;
		if (StepX) {
			if (m_Endpoint.x > m_Startpoint.x) {
				Curx+=1.0f;
				Cury+=Incy;
			}
			else {
				Curx-=1.0f;
				Cury-=Incy;
			}
		}
		else {
			if (m_Endpoint.y > m_Startpoint.y) {
				Cury+=1.0f;
				Curx+=Incx;
			}
			else {
				Cury-=1.0f;
				Curx-=Incx;
			}
		}
		c++;
	}


	int NewNumPoints;
	NewNumPoints = AveragePoints (MacroPoints, TempPoints, NumPoints);
	for (int i=0; i< NewNumPoints;i++) 
		MacroPoints[i+points_add_to_beg] = TempPoints[i];
	AveragePoints (SMCPoints, TempPoints, NumPoints);
	for (i=0; i< NewNumPoints;i++) 
		SMCPoints[i+points_add_to_beg] = TempPoints[i];
	AveragePoints (LipidPoints, TempPoints, NumPoints);
	for (i=0; i< NewNumPoints;i++) 
		LipidPoints[i+points_add_to_beg] = TempPoints[i];
	NumPoints = NewNumPoints;
			
	//c == NumPoints +4 here
	for (int addtoend = NumPoints+points_add_to_beg;addtoend<NumPoints+points_add_to_beg+points_add_to_end;addtoend++) {
		MacroPoints[addtoend] = MacroPoints[NumPoints+points_add_to_beg-1];
		SMCPoints[addtoend] = SMCPoints[NumPoints+points_add_to_beg-1];
		LipidPoints[addtoend] = LipidPoints[NumPoints+points_add_to_beg-1];
	}

	for (int addtobeg = 0;addtobeg<points_add_to_beg;addtobeg++) {
		MacroPoints[addtobeg] = MacroPoints[points_add_to_beg];
		SMCPoints[addtobeg] = SMCPoints[points_add_to_beg];
		LipidPoints[addtobeg] = LipidPoints[points_add_to_beg];
	}

	NumPoints+=points_add_to_beg+points_add_to_end;
	

	

	int count = 0;
	while (count++ < subdivisions) {
		NewNumPoints = DoSubdivision (MacroPoints, TempPoints, NumPoints);
		for (int i=0; i< NewNumPoints;i++) 
			MacroPoints[i] = TempPoints[i];
		DoSubdivision (SMCPoints, TempPoints, NumPoints);
		for (i=0; i< NewNumPoints;i++) 
			SMCPoints[i] = TempPoints[i];
		DoSubdivision (LipidPoints, TempPoints, NumPoints);
		for (i=0; i< NewNumPoints;i++) 
			LipidPoints[i] = TempPoints[i];
		NumPoints = NewNumPoints;
	}

	HBRUSH MacroBrush = CreateSolidBrush (RGB (128,128,0));
	HBRUSH SMCBrush = CreateSolidBrush(RGB (0,0,128));
	HBRUSH BackBrush = CreateSolidBrush(RGB (200,200,200));
	HBRUSH LipidBrush = CreateSolidBrush(RGB (200,200,0));

	

	SMCPoints[NumPoints].x = SMCPoints[NumPoints-1].x;
	SMCPoints[NumPoints].y = 0;
	SMCPoints[NumPoints+1].x = 0;
	SMCPoints[NumPoints+1].y = 0;
	dc.SelectObject (BackBrush);
	dc.Polygon (SMCPoints, NumPoints+2);

	LipidPoints[NumPoints].x = SMCPoints[NumPoints].x = MacroPoints[NumPoints].x = MacroPoints[NumPoints-1].x;
	LipidPoints[NumPoints].y = SMCPoints[NumPoints].y =MacroPoints[NumPoints].y = GraphySize;
	LipidPoints[NumPoints+1].x = SMCPoints[NumPoints+1].x = MacroPoints[NumPoints+1].x = 0;
	LipidPoints[NumPoints+1].y = SMCPoints[NumPoints+1].y =MacroPoints[NumPoints+1].y = GraphySize;

	dc.SelectObject (SMCBrush);
	dc.Polygon (SMCPoints, NumPoints+2);

	
	dc.SelectObject (MacroBrush);
	dc.Polygon (MacroPoints, NumPoints+2);

	dc.SelectObject (LipidBrush);
	dc.Polygon (LipidPoints, NumPoints+2);



	/*
	
	
	//now we draw the graph

	float LineLen = sqrt ((m_Endpoint.x-m_Startpoint.x)*(m_Endpoint.x-m_Startpoint.x)
					    + (m_Endpoint.y-m_Startpoint.y)*(m_Endpoint.y-m_Startpoint.y));

	float LinePos = 0;

	RECT rect;
	GetClientRect (&rect);
	int GraphxSize = rect.right;
	int GraphySize = rect.bottom;
	float CurxPos=m_Startpoint.x,CuryPos = m_Startpoint.y;
	float xIncPerGraphPixel = (m_Endpoint.x - m_Startpoint.x) /(float)GraphxSize;
	float yIncPerGraphPixel = (m_Endpoint.y - m_Startpoint.y) /(float)GraphxSize;
	HPEN MacroPen = CreatePen (PS_SOLID,1,RGB (128,128,0));
	HPEN SMCPen = CreatePen (PS_SOLID,1,RGB (0,0,128));
	HPEN BackPen = CreatePen (PS_SOLID,1,RGB (200,200,200));
	
	int i;
	for (i=0;i<GraphxSize;i++)
	{
		int NoofMacros = (MacrophageDataBuffer+CELL_BUFFER_X_SIZE*int(CuryPos)+int(CurxPos))->NoofCellsOnPixel;
		int MacroGraphHeight = (NoofMacros *GraphySize) / (MAX_MACROPHAGES_PER_PIXEL+MAX_SMCS_PER_PIXEL);
		int NoofSMCS = (SMCDataBuffer+CELL_BUFFER_X_SIZE*int(CuryPos)+int(CurxPos))->NoofCellsOnPixel;
		int SMCGraphHeight = (NoofSMCS*GraphySize)/(MAX_MACROPHAGES_PER_PIXEL+MAX_SMCS_PER_PIXEL);
		dc.SelectObject (MacroPen);
		dc.MoveTo(i,GraphySize);
		dc.LineTo(i,GraphySize-MacroGraphHeight);
		dc.SelectObject (SMCPen);
		dc.LineTo(i,GraphySize-MacroGraphHeight-SMCGraphHeight);
		dc.SelectObject (BackPen);
		dc.LineTo(i,0);
		CurxPos+=xIncPerGraphPixel;
		CuryPos+=yIncPerGraphPixel;
	}

  */

	
	delete [] MacroPoints;
	delete [] SMCPoints;
	delete [] LipidPoints;
	delete [] TempPoints;
	DeleteObject (MacroBrush);
	DeleteObject (SMCBrush);
	DeleteObject (BackBrush);
	DeleteObject (LipidBrush);
	}
	LeaveCriticalSection (&LockCritSection);		
	
}



void CXSectionDisplayDlg::OnDestroy() 
{
	pCurrentDisplayGraph = NULL;	
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

BOOL CXSectionDisplayDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText ("Cross Section Graph");
	pCurrentDisplayGraph = this;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXSectionDisplayDlg::OnAbout() 
{
	// TODO: Add your command handler code here
	
}
