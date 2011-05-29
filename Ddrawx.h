#include <d3dx.h>
#include <ddraw.h>
#include <d3drm.h>


typedef short int RGB16;
#define RED16(colour) (colour & 0x1f)<<3
#define GREEN16(colour) ((colour & 0x3e0) >> 5)<<3
#define BLUE16(colour) ((colour &0x7c00) >> 10)<<3
#define MAKECOLOUR16(red,green,blue) ((red>>3) & 0x1f) | (((green>>3) & 0x1f) << 5) | (((blue>>3) & 0x1f) << 10)

#define CIMP_VHIGH 0
#define CIMP_HIGH 10
#define CIMP_MEDIUM 20
#define CIMP_LOW  50
#define CIMP_NONEWENTRY -1

#define WINDOWED_MODE 1
#define FULLSCREEN_MODE 0

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

extern LPDIRECTDRAWSURFACE lpBackSurface;
extern LPDIRECTDRAWSURFACE7 pCellSurface;
extern LPD3DXCONTEXT pd3dx;
extern LPD3DXCONTEXT pd3dx_3D;
extern LPD3DXCONTEXT pd3dx_Game;

#define HANDLEDDERR(fn) DDPrintErr (fn,#fn);
#define DDShowErr(fn) DDPrintErr (fn,#fn)

BOOL InitDirectDraw (int Width,int Height,int bpp,HWND hwnd,int DisplayMode);
void DeInitDirectDraw (void);
void CreateCellSurface ();
BYTE* LockCellSurface (void);
void UnlockCellSurface (void);
void FillCellSurface (DWORD colour);
void ShowCellSurface ();
void Show3D();
void ReleaseCellSurface ();
BYTE* LockPrimarySurface (void);
void UnlockPrimarySurface (void);
void FillBackSurface (BYTE colour);
int FillBuffers (DWORD colour);
LPDIRECTDRAWSURFACE CreateMemSurface (int Width,int Height);
void FlipSurfaces (void);
void SetGDIMode (void);
void SetNoGDIMode (void);
void 	CopyPaletteFromBackSurface (void);
BYTE FindNearestPaletteIndex (int red,int green,int blue);
BYTE EnterColourIntoPalette (int red,int green,int blue,int ColourImportance);
void DeInitDirectDraw (void);
void DDHandleMessages (HWND hwnd, UINT Message,WPARAM wParam, LPARAM lParam);
void DDPaintWindow (HWND hwnd);
void ComposeSurfaces();
void DrawMainSurface();
BOOL DDPrintErr (int error,char *fnsring);

