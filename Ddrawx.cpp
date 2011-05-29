#include "athero.h"

#pragma hdrstop
#include "ddrawx.h"

//DirectDRAWeXtensions - Extensions to the DirectDraw Interface

BOOL AlreadyInitialised = 0;
LPDIRECTDRAW DDMainObject = NULL;
DDSURFACEDESC PrimarySurfaceDesc;
DDSURFACEDESC BackSurfaceDesc;
LPDIRECTDRAWSURFACE lpPrimarySurface = NULL;
LPDIRECTDRAWSURFACE lpBackSurface = NULL;
LPDIRECTDRAWSURFACE7 pCellSurface = NULL;
LPDIRECTDRAWCLIPPER WindowClipper = NULL;
LPD3DXCONTEXT pd3dx = NULL;
LPD3DXCONTEXT pd3dx_3D = NULL;
LPD3DXCONTEXT pd3dx_Game = NULL;

RECT WindowRect,MaxWindowRect;
int DrawAreaxSize;
int DrawAreaySize;
BOOL VmodeChanged = 0;
int CurrentDisplayMode;	// 0 = Fullscreen, 1 = windowed
int GDIMode = 0;
int BytesPerPixel;
HWND DDWindow;
CRITICAL_SECTION LockCritSection;

LPDIRECTDRAWPALETTE lpPalette = NULL;
PALETTEENTRY PaletteEntries [256];
int Colours_used;
int Colours_used_top;
int Colours_used_bottom;



BOOL InitDirectDraw (int Width,int Height,int bpp,HWND hwnd,int DisplayMode)
{
	HANDLEDDERR (D3DXInitialize ());

	HANDLEDDERR (
		D3DXCreateContext(D3DX_DEFAULT,
		0,
		hwnd,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		&pd3dx
	));

    HANDLEDDERR (
		D3DXCreateContext(D3DX_DEFAULT,
		D3DX_CONTEXT_OFFSCREEN,
		NULL,
		SCREEN_WIDTH/2,
		SCREEN_HEIGHT/2,
		&pd3dx_3D
	));

	CreateCellSurface ();

	InitializeCriticalSection (&LockCritSection);

    if (GameMode)
    {
        SetWindowPos (AtheroWindow, NULL, 0,0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
        SetWindowLong(AtheroWindow, GWL_STYLE, WS_POPUP);
    }





	/*
	DDSCAPS BackCaps;
	DDWindow = hwnd;
	DEVMODE dmode;
	dmode.dmSize = sizeof (dmode);
	dmode.dmBitsPerPel = 8;
	dmode.dmPelsWidth = 800;
	dmode.dmPelsHeight = 600;
	dmode.dmDisplayFlags = 0;
	dmode.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFLAGS|DM_BITSPERPEL;
	LONG ret = ChangeDisplaySettings (&dmode, 0);

	if (ret != DISP_CHANGE_SUCCESSFUL) {
		MessageBox (hwnd, "Could not change to 800x600x256 video mode", "Error", MB_ICONEXCLAMATION);
		return FALSE;
	}
	if (AlreadyInitialised)
		return FALSE;

	
	HANDLEDDERR (DirectDrawCreate (NULL,(LPDIRECTDRAW*)&DDMainObject,NULL))
	CurrentDisplayMode = DisplayMode;	//1 = windowed mode, 0 = fullscreen

	FillMemory ((void*)&PrimarySurfaceDesc,0,sizeof (PrimarySurfaceDesc));
	PrimarySurfaceDesc.dwSize = sizeof (PrimarySurfaceDesc);
	DrawAreaxSize = Width;
	DrawAreaySize = Height;
	WindowRect.left =0;
	WindowRect.right = Width-1;
	WindowRect.top = 0;
	WindowRect.bottom = Height-1;
	HANDLEDDERR (DDMainObject->CreateClipper (0,&WindowClipper,NULL))
	HANDLEDDERR (WindowClipper->SetHWnd (0,hwnd))

	if (DisplayMode)
	{
		//Init directdraw for a window
		HANDLEDDERR (DDMainObject->SetCooperativeLevel (hwnd,DDSCL_NORMAL|DDSCL_ALLOWREBOOT))
		PrimarySurfaceDesc.dwFlags = DDSD_CAPS;
		PrimarySurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		DrawAreaxSize = Width;
		DrawAreaySize = Height;
		RECT ClientRect;
		GetClientRect (hwnd,&ClientRect);
		ClientToScreen (hwnd,(LPPOINT)&ClientRect);
		ClientToScreen (hwnd,(LPPOINT)&ClientRect+1);
		WindowRect.left = ClientRect.left;
		WindowRect.top = ClientRect.top;
		WindowRect.right = ClientRect.right;
		WindowRect.bottom = ClientRect.bottom;
		//Now we have the rectangle for the window
		HANDLEDDERR (DDMainObject->CreateSurface (&PrimarySurfaceDesc,&lpPrimarySurface,NULL))
		HANDLEDDERR (lpPrimarySurface->GetSurfaceDesc (&PrimarySurfaceDesc));
		lpBackSurface = CreateMemSurface (Width,Height);
		if (!lpBackSurface)
			return FALSE;
		HANDLEDDERR (lpPrimarySurface->SetClipper (WindowClipper))
		GDIMode = 1;
	}
	else
	{
		SetWindowLong (hwnd,GWL_STYLE,WS_POPUP);
		SetWindowPos (hwnd,HWND_TOP,0,0,Width,Height,SWP_SHOWWINDOW);
		HANDLEDDERR (DDMainObject->SetCooperativeLevel (hwnd,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT))
		HANDLEDDERR (DDMainObject->SetDisplayMode (Width,Height,bpp))
		VmodeChanged = 1;
		PrimarySurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		PrimarySurfaceDesc.dwBackBufferCount = 1;
		PrimarySurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX|DDSCAPS_3DDEVICE;
		HANDLEDDERR (DDMainObject->CreateSurface (&PrimarySurfaceDesc,&lpPrimarySurface,NULL))
		BackCaps.dwCaps = DDSCAPS_BACKBUFFER;
		HANDLEDDERR (lpPrimarySurface->GetAttachedSurface (&BackCaps,&lpBackSurface))
		HANDLEDDERR (lpPrimarySurface->GetSurfaceDesc (&PrimarySurfaceDesc));
		BackSurfaceDesc.dwFlags = 0;
		BackSurfaceDesc.dwSize = sizeof (BackSurfaceDesc);
		HANDLEDDERR (lpBackSurface->GetSurfaceDesc (&BackSurfaceDesc));
		GDIMode = 0;
	}


	if (bpp==8)
	{
		int i;
		HDC TempDC = GetDC (hwnd);
		GetSystemPaletteEntries (TempDC,0,256,PaletteEntries);
		for (i=0;i<256;i++)
			PaletteEntries[i].peFlags = D3DPAL_RESERVED;
		ReleaseDC (hwnd,TempDC);
		memset (PaletteEntries+10,0,(256-19)*sizeof (PALETTEENTRY));
		for (i=10;i<256-10;i++)
				PaletteEntries[i].peFlags=D3DPAL_FREE|PC_NOCOLLAPSE;
		Colours_used += 20;
		Colours_used_top = 10;
		Colours_used_bottom = 10;
		HANDLEDDERR (DDMainObject->CreatePalette (DDPCAPS_8BIT|DDPCAPS_INITIALIZE,PaletteEntries,&lpPalette,NULL))
		HRESULT res = lpBackSurface->SetPalette (lpPalette);
		if (res == DDERR_INVALIDPIXELFORMAT)
		{
			DeInitDirectDraw ();
			exit (1);
		}
		else 
		{
			HANDLEDDERR (res)
		}

		HANDLEDDERR (lpPrimarySurface->SetPalette (lpPalette))
	}
	BytesPerPixel = bpp/8;

	LPDIRECTDRAWCLIPPER Clipper;
	LPRGNDATA ClipList=(LPRGNDATA)malloc (sizeof (RGNDATA)+sizeof(RECT));
	GetRegionData (CreateRectRgn (0,0,640,480),sizeof(RGNDATA)+sizeof(RECT),ClipList);
	DDShowErr (DDMainObject->CreateClipper (0,&Clipper,NULL));
	DDShowErr (Clipper->SetClipList (ClipList,0));
	DDShowErr (lpBackSurface->SetClipper (Clipper));
	Clipper->Release ();

	FillBuffers (0);
	AlreadyInitialised = TRUE;
	return TRUE;
	*/
    pd3dx->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
	AlreadyInitialised = TRUE;
	return TRUE;
}

HDC BackDC;

HDC GetBackSurfaceDC (void)
{
	DDShowErr (lpBackSurface->GetDC (&BackDC));
	return BackDC;
}

void ReleaseBackSurfaceDC (void)
{
	DDShowErr (lpBackSurface->ReleaseDC (BackDC));
}


void CreateCellSurface ()
{
	DDSURFACEDESC2 ddsd; 

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwHeight = CELL_BUFFER_Y_SIZE;
	ddsd.dwWidth  = CELL_BUFFER_X_SIZE;

	//RGB 8-8-8
#ifdef COLOUR_565
	HANDLEDDERR (D3DXMakeDDPixelFormat(
		D3DX_SF_R5G6B5,
		&ddsd.ddpfPixelFormat));
#else
	HANDLEDDERR (D3DXMakeDDPixelFormat(
		D3DX_SF_R5G5B5,
		&ddsd.ddpfPixelFormat));
#endif

	HANDLEDDERR (pd3dx->GetDD()->CreateSurface(
		&ddsd,        
		&pCellSurface,  
		NULL));

	pCellSurface->GetPixelFormat(&ddsd.ddpfPixelFormat);
}

DDSURFACEDESC2 LockedCellSurfaceDesc;

BYTE* LockCellSurface (void)
{
	EnterCriticalSection (&LockCritSection);
	LockedCellSurfaceDesc.dwSize =  sizeof (DDSURFACEDESC);
	LockedCellSurfaceDesc.dwFlags = 0;
	LockedCellSurfaceDesc.ddsCaps.dwCaps = 0;
	DDShowErr (pCellSurface->Lock (NULL,&LockedCellSurfaceDesc,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL));
	return (BYTE*) LockedCellSurfaceDesc.lpSurface;
}

/*
void CopyPaletteFromBackSurface (void)
{
	lpPalette->Release ();
	DDShowErr (	lpBackSurface->GetPalette (&lpPalette) );
	DDShowErr ( lpPalette->GetEntries (0,0,256,PaletteEntries) );
	DDShowErr ( lpPrimarySurface->SetPalette (lpPalette) );
}
*/



void UnlockCellSurface (void)
{
	DDShowErr (pCellSurface->Unlock (NULL));
	LeaveCriticalSection (&LockCritSection);
}


void FillCellSurface (D3DCOLOR colour)
{	
	DDBLTFX ddbltfx;
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(DDBLTFX);
	#ifdef COLOUR_565
	ddbltfx.dwFillColor = (colour&0xff)>>3
			| (((colour>>8)&0xff)>>2)<<5
			| (((colour>>16)&0xff)>>3)<<11;
#else
	ddbltfx.dwFillColor = (colour&0xff)>>3
			| (((colour>>8)&0xff)>>3)<<5
			| (((colour>>16)&0xff)>>3)<<10;
#endif
	DDShowErr (pCellSurface->Blt (NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx));
	/*
	WORD* ptr = (WORD*)LockCellSurface ();
	for (int x=0;x<CELL_BUFFER_X_SIZE;x++)
	{
		for (int y=0;y<CELL_BUFFER_Y_SIZE;y++)
		{
			*ptr++ = (colour&0xff)>>3
			| (((colour>>8)&0xff)>>3)<<5
			| (((colour>>16)&0xff)>>3)<<10;
		}
	}

	UnlockCellSurface ();
	*/
}


void ShowCellSurface ()
{
	
	LPDIRECTDRAWSURFACE7 pBackBuffer = pd3dx->GetBackBuffer(0);
	if (pBackBuffer)
	{      
        if (GameMode)
        {
            RECT r = {0};
            pd3dx->GetBufferSize((LPDWORD)&r.right, (LPDWORD)&r.bottom);
            r.left = r.right/2;
            r.top = r.bottom /2;
		    DDShowErr (pBackBuffer->Blt (&r, pCellSurface, NULL, 0, NULL));
        }
        else
        {
            DDShowErr (pBackBuffer->Blt (NULL, pCellSurface, NULL, 0, NULL));
        }
	}
	else {
		MessageBox (NULL, "", "No back buffer!", MB_ICONEXCLAMATION);
	}    
}

void ReleaseCellSurface ()
{
	pCellSurface->Release ();
}

void Show3D ()
{
    LPDIRECTDRAWSURFACE7 pBackBuffer = pd3dx->GetBackBuffer(0);
	if (pBackBuffer)
	{
        if (GameMode)
        {
            RECT r = {0};
            pd3dx->GetBufferSize((LPDWORD)&r.right, (LPDWORD)&r.bottom);
            r.left = r.right/2;
            r.bottom /=2;
		    DDShowErr (pBackBuffer->Blt (&r, pd3dx_3D->GetPrimary(), NULL, 0, NULL));
        }
        else
        {
            DDShowErr (pBackBuffer->Blt (NULL, pd3dx_3D->GetPrimary(), NULL, 0, NULL));
        }
	}
	else {
		MessageBox (NULL, "", "No back buffer!", MB_ICONEXCLAMATION);
	}
}

void ShowGame ()
{
    return;

    LPDIRECTDRAWSURFACE7 pBackBuffer = pd3dx->GetBackBuffer(0);
	if (pBackBuffer)
	{
        RECT r = {0};
        pd3dx->GetBufferSize((LPDWORD)&r.right, (LPDWORD)&r.bottom);
        r.left = 0;
        r.right/=2;
        r.top = 0;
        DDShowErr (pBackBuffer->Blt (&r, pd3dx_Game->GetPrimary(), NULL, 0, NULL));    
	}
	else {
		MessageBox (NULL, "", "No back buffer!", MB_ICONEXCLAMATION);
	}
}


/*
DDSURFACEDESC LockedPrimarySurfaceDesc;

BYTE* LockPrimarySurface (void)
{
	LockedPrimarySurfaceDesc.dwSize = sizeof (DDSURFACEDESC);
	DDShowErr (lpPrimarySurface->Lock (NULL,&LockedPrimarySurfaceDesc,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL));
	return (BYTE*) LockedPrimarySurfaceDesc.lpSurface;
}

void UnlockPrimarySurface (void)
{
	DDShowErr (lpPrimarySurface->Unlock (&LockedPrimarySurfaceDesc));
}


BYTE CreateNewPaletteEntry (int red,int green,int blue)
{
	if (Colours_used >= 256)
		return 0;
	Colours_used++;
	PaletteEntries [Colours_used_bottom].peRed = red;
	PaletteEntries [Colours_used_bottom].peGreen = green;
	PaletteEntries [Colours_used_bottom].peBlue = blue;
	PaletteEntries [Colours_used_bottom].peFlags = D3DPAL_READONLY;
	lpPalette->SetEntries (0,Colours_used_bottom,1,&PaletteEntries[Colours_used_bottom]);
	Colours_used_bottom++;
	return (Colours_used_bottom-1);
}

*/

/*
BYTE EnterColourIntoPalette (int red,int green,int blue,int ColourImportance)
{
	//This gets your values for rgb and makes a palette entry so you can use that colour
	//If the colour very closely matches or is intentical to another colour, then the index for the other
	//colour is returned.
	//If there is no room left in the palette, the nearest colour is returned
	int NearestColour = FindNearestPaletteIndex (red,green,blue);
	int DifferenceToColour =abs (red-PaletteEntries[NearestColour].peRed) +
						 abs (green-PaletteEntries[NearestColour].peGreen) +
						 abs (blue - PaletteEntries[NearestColour].peBlue);
	if (DifferenceToColour >ColourImportance && Colours_used < 256 && ColourImportance != CIMP_NONEWENTRY)
		return CreateNewPaletteEntry (red,green,blue);
	else
		return (BYTE) NearestColour;
}

BYTE FindNearestPaletteIndex (int red,int green,int blue)
{
	int index;
	int difference;
	int mindifindex;
	int mindifference = 256*3;
	for (index=0;index<256;index++)
	{
		difference = abs (red-PaletteEntries[index].peRed) +
						 abs (green-PaletteEntries[index].peGreen) +
						 abs (blue - PaletteEntries[index].peBlue);
		if (difference < mindifference)
		{
			mindifference = difference;
			mindifindex = index;
		}
	}
	return mindifindex;
}

LPDIRECTDRAWSURFACE CreateMemSurface (int Width,int Height)
{
	DDSURFACEDESC MemSurfaceDesc;
	LPDIRECTDRAWSURFACE MemSurface;
	memset ((void*)&MemSurfaceDesc,0,sizeof (MemSurfaceDesc));
	MemSurfaceDesc.dwSize = sizeof (MemSurfaceDesc);
	MemSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	MemSurfaceDesc.ddsCaps.dwCaps  = DDSCAPS_SYSTEMMEMORY | DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;
	MemSurfaceDesc.dwHeight = Height;
	MemSurfaceDesc.dwWidth = Width;
	HANDLEDDERR (DDMainObject->CreateSurface (&MemSurfaceDesc,&MemSurface,NULL))
	return MemSurface;
}
*/

void DeInitDirectDraw (void)
{
	ReleaseCellSurface ();
	if (pd3dx)
		pd3dx->Release ();
	D3DXUninitialize(); 
	/*
	ChangeDisplaySettings (NULL, 0);
	if	(lpPalette)
		lpPalette->Release ();
	if (VmodeChanged)
		DDMainObject->RestoreDisplayMode ();
	if (lpBackSurface)
		lpBackSurface->Release ();
	if (lpPrimarySurface)
		lpPrimarySurface->Release ();
	if (WindowClipper)
		WindowClipper->Release ();
	if (DDMainObject)
		DDMainObject->Release ();
		*/
	AlreadyInitialised = FALSE;
}

/*
void SetGDIMode (void)
{
	if (!CurrentDisplayMode)
	{
		GDIMode = 1;
		DDShowErr (lpPrimarySurface->SetClipper (WindowClipper));
		DDShowErr (DDMainObject->FlipToGDISurface ());
		InvalidateRect (DDWindow,NULL,FALSE);
		UpdateWindow (DDWindow);
	}
}

void SetNoGDIMode (void)
{
	if (!CurrentDisplayMode)
	{
		GDIMode = 0;
		DDShowErr (lpPrimarySurface->SetClipper (NULL));
	}
}
*/


int FillBuffers (DWORD colour)
{
	FillCellSurface (colour);
	/*
	FillBackSurface (colour);
	FlipSurfaces ();
	FillBackSurface (colour);
	*/
	ShowCellSurface ();    
	InvalidateRect (DDWindow,NULL,FALSE);
	UpdateWindow (DDWindow);

	return FALSE;
}

void FlipSurfaces (void)
{
	if (CurrentDisplayMode)	//If were in a window, do the copy
	{
		DDMainObject->WaitForVerticalBlank (DDWAITVB_BLOCKEND,NULL);
		if ((WindowRect.right-WindowRect.left <= 0) || (WindowRect.bottom-WindowRect.top <= 0))
			return;
		DDShowErr ( lpPrimarySurface->Blt (&WindowRect,lpBackSurface,NULL,DDBLT_WAIT|DDBLT_ASYNC,NULL) );
	}
	else
	{
		if (!GDIMode)
			lpPrimarySurface->Flip (NULL,DDFLIP_WAIT);
		else
		{
			DDShowErr ( lpPrimarySurface->Blt (&WindowRect,lpBackSurface,NULL,DDBLT_WAIT|DDBLT_ASYNC,NULL) );
			InvalidateRect (DDWindow,NULL,FALSE);
			UpdateWindow (DDWindow);
		}
	}
}

void DDHandleMessages (HWND hwnd, UINT Message,WPARAM wParam, LPARAM lParam)
{
	/*
	switch (Message)
	{
		case WM_MOVE:
			int Delta_x,Delta_y;
			Delta_x = LOWORD(lParam)-WindowRect.left;
			Delta_y = HIWORD(lParam)-WindowRect.top;
			WindowRect.left=LOWORD(lParam);
			WindowRect.top = HIWORD(lParam);
			WindowRect.right +=Delta_x;
			WindowRect.bottom +=Delta_y;
			break;

		case WM_SIZE:
			WindowRect.right =WindowRect.left+LOWORD(lParam);
			WindowRect.bottom = WindowRect.top+HIWORD(lParam);
		break;
		
	}
	*/
}

void ComposeSurfaces()
{
    EnterCriticalSection (&LockCritSection);	
    if (!GameMode)
        ShowCellSurface ();

	if (View3D)
	    Show3D();

   
    LeaveCriticalSection (&LockCritSection);
}

void DrawMainSurface()
{
    HANDLEDDERR (pd3dx->UpdateFrame(D3DX_DEFAULT));
}



void DDPaintWindow (HWND hwnd)
{
	PAINTSTRUCT paintstuff;
	if (!CurrentDisplayMode)
	{
		WindowRect.left = 0;
		WindowRect.right = 639;
		WindowRect.top = 0;
		WindowRect.bottom = 479;
	}
	BeginPaint (hwnd,&paintstuff);

	/*
	if (AlreadyInitialised && (paintstuff.rcPaint.right-paintstuff.rcPaint.left) > 0 &&
		(paintstuff.rcPaint.bottom - paintstuff.rcPaint.top) > 0 )
	{
		RECT rect,srcrect,destrect;
		GetClientRect (hwnd,&rect);
		float scalefactorx = (float)CELL_BUFFER_X_SIZE /(float)rect.right;
		float scalefactory = (float)CELL_BUFFER_Y_SIZE / (float)rect.bottom;
		srcrect.left=paintstuff.rcPaint.left*scalefactorx;
		srcrect.right=paintstuff.rcPaint.right*scalefactorx;
		srcrect.top=paintstuff.rcPaint.top*scalefactory;
		srcrect.bottom=paintstuff.rcPaint.bottom*scalefactory;
		destrect = paintstuff.rcPaint;
		ClientToScreen(hwnd,(POINT*)&destrect);
		ClientToScreen(hwnd,((POINT*)&destrect)+1);
		if ((srcrect.right-srcrect.left) > 0 && (srcrect.bottom-srcrect.top) > 0 )
		{
	//		DrawMacrophageBuffer ();
			LPDIRECTDRAWSURFACE7 lpPrimarySurface  = pd3dx->GetPrimary();
			lpPrimarySurface->Blt (&destrect,pd3dx->GetBackBuffer(0),&srcrect,DDBLT_WAIT|DDBLT_ASYNC,NULL);
		}
	}
	*/
	

	if (pd3dx && (paintstuff.rcPaint.right-paintstuff.rcPaint.left) > 0 &&
		(paintstuff.rcPaint.bottom - paintstuff.rcPaint.top) > 0 ) 
	{
		/*
		LPDIRECTDRAWCLIPPER pClipper,pOldClipper;

		pd3dx->GetDD()->CreateClipper (0, &pClipper, 0);

		pd3dx->GetPrimary()->GetClipper (&pOldClipper);		

		RECT destrect = paintstuff.rcPaint;
		ClientToScreen(hwnd,(POINT*)&destrect);
		ClientToScreen(hwnd,((POINT*)&destrect)+1);

		HRGN hrgn = CreateRectRgn(
		 destrect.left,   // x-coordinate of region's upper-left corner
		 destrect.top,    // y-coordinate of region's upper-left corner
		 destrect.right,  // x-coordinate of region's lower-right corner
		 destrect.bottom  // y-coordinate of region's lower-right corner
		 );


		char buf[1000];
		GetRegionData (hrgn, 1000, (LPRGNDATA)buf);
		DeleteObject (hrgn);

		pClipper->SetClipList ((LPRGNDATA)buf, 0);

		HANDLEDDERR (pd3dx->GetPrimary()->SetClipper (pClipper));
 
   */



		IntersectClipRect(paintstuff.hdc, paintstuff.rcPaint.left, paintstuff.rcPaint.top, paintstuff.rcPaint.right, paintstuff.rcPaint.bottom);

        DrawMainSurface();
		/*

		HANDLEDDERR (pd3dx->GetPrimary()->SetClipper (pOldClipper));

		pClipper->Release();
		*/
	}
	EndPaint (hwnd,&paintstuff);
}


short int Colours_left = 256;


#ifndef _DEBUG

#define HANDLE_ERR(err) case err: strcpy (ErrText,#err);\
strcat (ErrText,"\n"); \
strcat (ErrText,fnstring);\
DeInitDirectDraw ();\
MessageBox (NULL,ErrText,"DirectDraw Error!",MB_OK | MB_APPLMODAL);\
exit (1);

#else

#define HANDLE_ERR(err) case err: strcpy (ErrText,#err);\
strcat (ErrText,"\n"); \
strcat (ErrText,fnstring);\
MessageBox (NULL,ErrText,"DirectDraw Error!",MB_OK | MB_APPLMODAL);\
DebugBreak (); \
break;
#endif




BOOL DDPrintErr (int error,char *fnstring)
{
	char ErrText [200];
	if (!error)
		return false;

	D3DXGetErrorString(error, 
                       sizeof(ErrText), 
                       ErrText);

	MessageBox (NULL, ErrText, "Direct Draw Error", MB_ICONEXCLAMATION);

#ifdef _DEBUG
	__asm int 3;
#endif	




	/*
	switch (error)
		{
		case DD_OK:
			return FALSE;
		HANDLE_ERR (DDERR_ALREADYINITIALIZED)
		HANDLE_ERR (DDERR_BLTFASTCANTCLIP)
		HANDLE_ERR (DDERR_CANNOTATTACHSURFACE)
		HANDLE_ERR (DDERR_CANNOTDETACHSURFACE)
		HANDLE_ERR (DDERR_CANTCREATEDC )
		HANDLE_ERR (DDERR_CANTDUPLICATE)
		HANDLE_ERR (DDERR_CANTLOCKSURFACE)
		HANDLE_ERR (DDERR_CANTPAGELOCK)
		HANDLE_ERR (DDERR_CANTPAGEUNLOCK)
		HANDLE_ERR (DDERR_CLIPPERISUSINGHWND )
		HANDLE_ERR (DDERR_COLORKEYNOTSET)
		HANDLE_ERR (DDERR_DCALREADYCREATED)
		HANDLE_ERR (DDERR_DIRECTDRAWALREADYCREATED)
		HANDLE_ERR (DDERR_EXCEPTION )
		HANDLE_ERR (DDERR_EXCLUSIVEMODEALREADYSET)
		HANDLE_ERR (DDERR_GENERIC)
		HANDLE_ERR (DDERR_HEIGHTALIGN)
		HANDLE_ERR (DDERR_HWNDALREADYSET)
		HANDLE_ERR (DDERR_HWNDSUBCLASSED)
		HANDLE_ERR (DDERR_IMPLICITLYCREATED )
		HANDLE_ERR (DDERR_INCOMPATIBLEPRIMARY )
		HANDLE_ERR (DDERR_INVALIDCAPS )
		HANDLE_ERR (DDERR_INVALIDCLIPLIST )
		HANDLE_ERR (DDERR_INVALIDDIRECTDRAWGUID)
		HANDLE_ERR (DDERR_INVALIDMODE)
		HANDLE_ERR (DDERR_INVALIDOBJECT)
		HANDLE_ERR (DDERR_INVALIDPARAMS)
		HANDLE_ERR (DDERR_INVALIDPIXELFORMAT )
		HANDLE_ERR (DDERR_INVALIDPOSITION )
		HANDLE_ERR (DDERR_INVALIDRECT)
		HANDLE_ERR (DDERR_INVALIDSURFACETYPE)
		HANDLE_ERR (DDERR_LOCKEDSURFACES )
		HANDLE_ERR (DDERR_NO3D )
		HANDLE_ERR (DDERR_NOALPHAHW )
		HANDLE_ERR (DDERR_NOBLTHW )
		HANDLE_ERR (DDERR_NOCLIPLIST )
		HANDLE_ERR (DDERR_NOCLIPPERATTACHED )
		HANDLE_ERR (DDERR_NOCOLORCONVHW )
		HANDLE_ERR (DDERR_NOCOLORKEY)
		HANDLE_ERR (DDERR_NOCOLORKEYHW)
		HANDLE_ERR (DDERR_NOCOOPERATIVELEVELSET)
		HANDLE_ERR (DDERR_NODC )
		HANDLE_ERR (DDERR_NODDROPSHW)
		HANDLE_ERR (DDERR_NODIRECTDRAWHW)
		HANDLE_ERR (DDERR_NODIRECTDRAWSUPPORT)
		HANDLE_ERR (DDERR_NOEMULATION)
		HANDLE_ERR (DDERR_NOEXCLUSIVEMODE)
		HANDLE_ERR (DDERR_NOFLIPHW )
		HANDLE_ERR (DDERR_NOGDI )
		HANDLE_ERR (DDERR_NOHWND)
		HANDLE_ERR (DDERR_NOMIPMAPHW)
		HANDLE_ERR (DDERR_NOMIRRORHW)
		HANDLE_ERR (DDERR_NOOVERLAYDEST )
		HANDLE_ERR (DDERR_NOOVERLAYHW )
		HANDLE_ERR (DDERR_NOPALETTEATTACHED)
		HANDLE_ERR (DDERR_NOPALETTEHW )
		HANDLE_ERR (DDERR_NORASTEROPHW)
		HANDLE_ERR (DDERR_NOROTATIONHW)
		HANDLE_ERR (DDERR_NOSTRETCHHW)
		HANDLE_ERR (DDERR_NOT4BITCOLOR)
		HANDLE_ERR (DDERR_NOT4BITCOLORINDEX)
		HANDLE_ERR (DDERR_NOT8BITCOLOR)
		HANDLE_ERR (DDERR_NOTAOVERLAYSURFACE)
		HANDLE_ERR (DDERR_NOTEXTUREHW)
		HANDLE_ERR (DDERR_NOTFLIPPABLE)
		HANDLE_ERR (DDERR_NOTFOUND )
		HANDLE_ERR (DDERR_NOTINITIALIZED)
		HANDLE_ERR (DDERR_NOTLOCKED)
		HANDLE_ERR (DDERR_NOTPAGELOCKED)
		HANDLE_ERR (DDERR_NOTPALETTIZED)
		HANDLE_ERR (DDERR_NOVSYNCHW )
		HANDLE_ERR (DDERR_NOZBUFFERHW)
		HANDLE_ERR (DDERR_NOZOVERLAYHW)
		HANDLE_ERR (DDERR_OUTOFCAPS )
		HANDLE_ERR (DDERR_OUTOFMEMORY )
		HANDLE_ERR (DDERR_OUTOFVIDEOMEMORY)
		HANDLE_ERR (DDERR_OVERLAYCANTCLIP )
		HANDLE_ERR (DDERR_OVERLAYCOLORKEYONLYONEACTIVE)
		HANDLE_ERR (DDERR_OVERLAYNOTVISIBLE )
		HANDLE_ERR (DDERR_PALETTEBUSY)
		HANDLE_ERR (DDERR_PRIMARYSURFACEALREADYEXISTS)
		HANDLE_ERR (DDERR_REGIONTOOSMALL)
		HANDLE_ERR (DDERR_SURFACEALREADYATTACHED )

		HANDLE_ERR (DDERR_SURFACEALREADYDEPENDENT)
		HANDLE_ERR (DDERR_SURFACEBUSY)
		HANDLE_ERR (DDERR_SURFACEISOBSCURED)
		HANDLE_ERR (DDERR_SURFACELOST)
		HANDLE_ERR (DDERR_SURFACENOTATTACHED )
		HANDLE_ERR (DDERR_TOOBIGHEIGHT )
		HANDLE_ERR (DDERR_TOOBIGSIZE)
		HANDLE_ERR (DDERR_TOOBIGWIDTH)
		HANDLE_ERR (DDERR_UNSUPPORTED)
		HANDLE_ERR (DDERR_UNSUPPORTEDFORMAT )
		HANDLE_ERR (DDERR_UNSUPPORTEDMASK)
		HANDLE_ERR (DDERR_UNSUPPORTEDMODE)
		HANDLE_ERR (DDERR_VERTICALBLANKINPROGRESS)
		HANDLE_ERR (DDERR_WASSTILLDRAWING )
		HANDLE_ERR (DDERR_WRONGMODE)
		HANDLE_ERR (DDERR_XALIGN)
		default:
		MessageBox (NULL,"Unknown Error","Direct Draw Error!",MB_OK);
		return TRUE;
		}

		*/
		return FALSE;
}





