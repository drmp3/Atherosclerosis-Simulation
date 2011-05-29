#include "athero.h"
#include "Sprite.h"
#include "ddrawx.h"
#include <vector>
#include "SimulationSettings.h"
#include "Macrophg.h"

#define MAX_FALLING_THINGS 256

#define SURFACE_WIDTH 512
#define SURFACE_HEIGHT 768
#define DEFAULT_MAN_SPEED 3.0f
#define MAX_MAN_SPEED 12.0f
#define MAN_ACCELERATION 0.4;
#define DEATH_CELL_COUNT 1200000
#define MAN_DEATH_TIME 500
#define SPLASH_DURATION 20000
#define FIRST_LAST_SPLASH_WAIT 35000
#define HEALTH_EXERCISE_GAIN 0.1f

#define GAMETIME 60000
#define SPLASH_KEY_ALLOW_TIME 200

#define RESTART_DELAY 1500 //wait this long before press a key works

Sprite* g_ManSprite;
Sprite* g_BGSprite;
Sprite* g_HUDBg;
Sprite* g_ProgressFull;
Sprite* g_ProgressEmpty;
Sprite* g_ProgressFullGreen;
Sprite* g_ProgressEmptyGreen;
float   g_Health;
float   g_ManAngle;
float   g_ManX;
float   g_ManY;
float   g_ManVelocityLeft = DEFAULT_MAN_SPEED;
float   g_ManVelocityRight = DEFAULT_MAN_SPEED;
float   g_ManHeartAttackX;
bool    g_ManLookLeft = false;
HFONT   g_HudFont = NULL;
float   g_Age = 30;
int     g_BloodFlow = 50;
int     g_PrevSimulHealth;
unsigned     g_RestartTimer;
bool    g_HeartAttack;
bool    g_HeartAttackFlash;
bool    g_GameEnded = false;
bool    g_DisplaySplashes = false;
HBITMAP g_SplashBitmap;
int     g_CurBitmapIdx;
int     g_BitmapTimeout;
unsigned     g_AllowKeyToSkipTime;
bool    g_LastSplash = false;



struct FallingThing
{
    Sprite* m_Sprite;
    int m_Badness;
    int m_Velocity;    
    float m_X;
    float m_Y;    
};

struct FallingThingInfo
{
    const char* m_SpriteName;
    int m_Badness;
    int m_RateOfAppearance;
    float m_Velocity;
    Sprite* m_Sprite;
};

FallingThingInfo g_FallingThingInfos[] =
{
    /* name             badness rate velocity */
    {"sprites/burger.dds", 15, 500, 5.0f},

    {"sprites/fries.dds", 15, 500, 6.5f},

    {"sprites/burger.dds", 15, 500, 8.0f},

    {"sprites/cigarette.dds", 30, 300, 6.5f},

    {"sprites/pizza.dds", 15, 500, 5.0f},

    {"sprites/carrot.dds", -10, 100, 7.0f},

    {"sprites/beans.dds", -20, 100, 7.5f},

    {"sprites/cabbage.dds", -25, 100, 8.5f},
};

typedef std::vector<FallingThing> FallingThingArr;

FallingThingArr g_FallingThings;

void SetSimulationHealth();
void RenderSplashes();

void GameInit()
{
    g_ManSprite = new Sprite("sprites/man.dds");
    g_BGSprite = new Sprite("sprites/background.dds");
    g_HUDBg = new Sprite("sprites/hud_bg.dds");
    g_ProgressEmpty = new Sprite("sprites/progress_empty.dds");
    g_ProgressFull = new Sprite("sprites/progress_full.dds");
    g_ProgressEmptyGreen = new Sprite("sprites/progress_empty_green.dds");
    g_ProgressFullGreen = new Sprite("sprites/progress_full_green.dds");

    for (int i=0;i<sizeof(g_FallingThingInfos)/sizeof(FallingThingInfo);i++)
    {
        g_FallingThingInfos[i].m_Sprite = new Sprite(g_FallingThingInfos[i].m_SpriteName);
    }
    g_ManX = SURFACE_WIDTH/2;
    g_ManY = SURFACE_HEIGHT - g_ManSprite->m_Height;
    g_ManAngle = 0.0f;
    g_HeartAttack = false;
    g_Health = 99;
    g_Age = 0;
    g_BloodFlow = 100;
    g_GameEnded = false;
    g_CurBitmapIdx = 0;
    SetSimulationHealth();
    StopSimualationUpdate();

    //start with the splashes
    g_DisplaySplashes = true;
 }

void GameTerm()
{
    delete g_HUDBg;
    delete g_BGSprite;
    delete g_ManSprite;    
    delete g_ProgressEmpty;
    delete g_ProgressFull;
    delete g_ProgressEmptyGreen;
    delete g_ProgressFullGreen;
    for (int i=0;i<sizeof(g_FallingThingInfos)/sizeof(FallingThingInfo);i++)
    {
        delete g_FallingThingInfos[i].m_Sprite;
        g_FallingThingInfos[i].m_Sprite = NULL;
    }
    g_FallingThings.clear();
    if (g_HudFont)
        DeleteObject(g_HudFont);
    g_HudFont = NULL;
}

void CreateFallingThing(FallingThingInfo& info)
{
    FallingThing f;
    memset(&f, 0, sizeof(f));
    f.m_Sprite = info.m_Sprite;
    f.m_X = (rand() * SURFACE_WIDTH)/RAND_MAX;
    f.m_Y = -100;
    f.m_Velocity = info.m_Velocity;
    f.m_Badness = info.m_Badness;
    g_FallingThings.push_back(f);
}

void AddNewFallingThings()
{
    for (int i=0;i<sizeof(g_FallingThingInfos)/sizeof(FallingThingInfo);i++)
    {
        if (rand() < g_FallingThingInfos[i].m_RateOfAppearance)
        {
            CreateFallingThing(g_FallingThingInfos[i]);
        }
    }
}

void MoveFallingThings()
{
    for (FallingThingArr::iterator it = g_FallingThings.begin(); it != g_FallingThings.end(); it++)
    {
        it->m_Y += it->m_Velocity;
    }
}

void CollideFallingThings()
{
    for (FallingThingArr::iterator it = g_FallingThings.begin(); it != g_FallingThings.end(); it++)
    {
        RECT dest;
        RECT man;
        RECT falling;
        man.left = g_ManX;
        man.right = g_ManX + g_ManSprite->m_Width;
        man.top = g_ManY;
        man.bottom = g_ManY + g_ManSprite->m_Height;

        falling.left = it->m_X;
        falling.right = it->m_X + it->m_Sprite->m_Width;
        falling.top = it->m_Y;
        falling.bottom = it->m_Y + it->m_Sprite->m_Height;

        if (IntersectRect(&dest, &man, &falling))
        {
            g_Health -= it->m_Badness;
            if (g_Health < 0)
                g_Health = 0;
            if (g_Health > 99)
                g_Health = 99;
            SetSimulationHealth();
            g_FallingThings.erase(it);
            break;
        }
    }
}

void RemoveDeadFallingThings()
{
    bool didOne = true;
    while (didOne)
    {
        didOne = false;
        for (FallingThingArr::iterator it = g_FallingThings.begin(); it != g_FallingThings.end(); it++)
        {
            if (it->m_Y > SURFACE_HEIGHT)
            {
                g_FallingThings.erase(it);
                didOne = true;
                break;
            }
        }
    }
}

void MoveMan()
{    
    bool leftPressed = GetAsyncKeyState(VK_LEFT);
    bool rightPressed = GetAsyncKeyState(VK_RIGHT);

    if (leftPressed)
    {        

        if (0 && g_ManVelocityLeft < MAX_MAN_SPEED)
        {
            g_ManVelocityLeft += MAN_ACCELERATION;
            g_ManX -= DEFAULT_MAN_SPEED;
        }
        else
            g_ManX -= MAX_MAN_SPEED;

        g_ManLookLeft = true;

        g_Health += HEALTH_EXERCISE_GAIN;
       
    } 
    else
        g_ManVelocityLeft = DEFAULT_MAN_SPEED;

    
    if (rightPressed)
    {
        g_ManLookLeft = false;
        if (0 && g_ManVelocityRight < MAX_MAN_SPEED)
        {
            g_ManVelocityRight += MAN_ACCELERATION;
            g_ManX += DEFAULT_MAN_SPEED;
        }
        else
            g_ManX += MAX_MAN_SPEED;
        g_Health += HEALTH_EXERCISE_GAIN;
    } 
    else
        g_ManVelocityRight = DEFAULT_MAN_SPEED;

    if (g_Health > 99.0f)
        g_Health = 99.0f;


    if (g_ManX < 0)
        g_ManX = 0;

    if (g_ManX > SURFACE_WIDTH - g_ManSprite->m_Width)
        g_ManX = SURFACE_WIDTH - g_ManSprite->m_Width;



}

void UpdateHeartAttack()
{
    if (g_ManAngle == 0.0f)
    {
        //First time
        g_ManHeartAttackX = g_ManX;        
    }
    if (fabs(g_ManAngle) < PI/2)
    {
        float add = (UPDATES_PER_SEC * PI/2.0f)/MAN_DEATH_TIME;
        if (g_ManLookLeft)
            g_ManAngle += add;
        else
            g_ManAngle -= add;

    }

    g_ManY = SURFACE_HEIGHT - g_ManSprite->m_Height + (g_ManSprite->m_Width/2)*sin(fabs(g_ManAngle)) + 6.0f;
    if (g_ManLookLeft)
        g_ManX = g_ManHeartAttackX - (g_ManSprite->m_Width/2)*cos(g_ManAngle);
    else
        g_ManX = g_ManHeartAttackX + (g_ManSprite->m_Width/2)*cos(g_ManAngle);
}

bool GetHeartAttackFlash()
{
    if (g_HeartAttack)
    {
        return g_HeartAttackFlash;
    }
    return true;
}

void EndGame()
{
    g_GameEnded = true;
    g_RestartTimer = GetTickCount() + RESTART_DELAY;
    StopSimualationUpdate();
}

bool GetAnyKey()
{
    for (int i=0;i<512;i++)
    {
        if (GetAsyncKeyState(i) & 0xffff0000)
            return true;
    }        
    return false;
}


void EndGameUpdate()
{
    if (g_GameEnded && GetTickCount() > g_RestartTimer)
    {        
        if (GetAnyKey())
            RestartApp();
    }
}

void UpdateSplashes()
{
    bool timeout = GetTickCount() > g_BitmapTimeout;
    bool next = timeout || !g_SplashBitmap;

    if (GetTickCount() > g_AllowKeyToSkipTime && GetAnyKey())
        next = true;

    if (next)
    {
        if (g_SplashBitmap)
            DeleteObject(g_SplashBitmap);
        g_SplashBitmap = NULL;
        char fname[256];
        sprintf(fname, "splashes/splash%d.bmp", g_CurBitmapIdx);        
        g_SplashBitmap = (HBITMAP)LoadImage (0, fname, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        if (!g_SplashBitmap)
        {
            //start game if not timeout
            if (!timeout)
            {
                g_DisplaySplashes = false;
                StartSimualationUpdate();
            }
            else
                g_CurBitmapIdx = 0;
            return;
        }        

        sprintf(fname, "splashes/splash%d.bmp", g_CurBitmapIdx+1);
        FILE* test = fopen(fname, "r");
        if (test)
        {
            g_LastSplash = false;
            fclose(test);
        }
        else
            g_LastSplash = true;

        int splash_duration = SPLASH_DURATION;

        if (g_LastSplash || !g_CurBitmapIdx)
        {
            splash_duration = FIRST_LAST_SPLASH_WAIT;
        }

        g_BitmapTimeout = GetTickCount() + splash_duration;
        g_AllowKeyToSkipTime = GetTickCount() + SPLASH_KEY_ALLOW_TIME;

        g_CurBitmapIdx++;
    }
}


void GameUpdate()
{
    if (g_DisplaySplashes)
    {
        UpdateSplashes();
        return;
    }

    if (!g_GameEnded)
        AddNewFallingThings();
    MoveFallingThings();
    if (g_HeartAttack)
        UpdateHeartAttack();
    else if (!g_GameEnded)
        MoveMan();
    CollideFallingThings();
    RemoveDeadFallingThings();       
    if (!g_GameEnded)
    {
        g_Age += (1000.0f/(GAMETIME * UPDATES_PER_SEC)) * 100.0f;
        g_BloodFlow = 100 - (GetTotalCellCount() * 100)/DEATH_CELL_COUNT;

        if (g_BloodFlow < 0)
        {
            g_BloodFlow = 0;
            g_HeartAttack = true;            
            EndGame();            
        }
    }

    if (g_Age >= 100 && !g_GameEnded)
    {
        EndGame();
    }

    if (g_GameEnded)
        EndGameUpdate();

}

void SetSimulationHealth()
{
    int simulHealth = g_Health/10;
    if (g_PrevSimulHealth != simulHealth)
    {
        g_PrevSimulHealth = simulHealth;
        char fname[256];
        sprintf(fname, "Health%d.csv", simulHealth);
        LoadSettingsFromFile (AtheroWindow, fname);
    }
}

void DrawFallingThings()
{
    for (FallingThingArr::iterator it = g_FallingThings.begin(); it != g_FallingThings.end(); it++)
    {
        //add a bit to smooth out the rendering
        float updatePeriod = 1000.0f/UPDATES_PER_SEC;
        float additional = it->m_Velocity * (((g_RenderTime+ updatePeriod) - g_LastUpdateTime)/updatePeriod);
        if (additional > 10.0f || additional < 0)
            additional = 0;
        it->m_Sprite->Draw(it->m_X, it->m_Y + additional);
    }
}

void DrawProgress(float x, float y, float pc, bool green)
{
    if (pc > 100)
        pc = 100;
    if (pc < 0)
        pc = 0;
    int fillpos = (pc * g_ProgressFull->m_Width)/100.0f;
    Sprite* full = green ? g_ProgressFullGreen : g_ProgressFull;
    Sprite* empty = green ? g_ProgressEmptyGreen : g_ProgressEmpty;
    full->DrawSub(x, y, 0, 0, fillpos, g_ProgressFull->m_Height);
    empty->DrawSub(x+fillpos, y, fillpos, 0, g_ProgressEmpty->m_Width-fillpos, g_ProgressEmpty->m_Height);

}

void GameDrawHUD()
{
    if (g_DisplaySplashes)
    {     
        RenderSplashes();
        return;
    }


    bool flashBF = (g_BloodFlow < 35) && !g_HeartAttack;
    bool drawBF = true;

    if (flashBF)
    {
        drawBF = (GetTickCount() % 1000) < 500;
    }


    g_HUDBg->Draw(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    int py = 450;
    int px = 610;
    DrawProgress(px, py, g_Age, true);

    px += 55;
    py += 50;

    DrawProgress(px, py, g_Health, false);

    px += 65;
    py += 50;

    if (drawBF)
        DrawProgress(px, py, g_BloodFlow, false);


    HDC dc = NULL;
    HANDLEDDERR( pd3dx->GetBackBuffer(0)->GetDC(&dc));

    if (!g_HudFont)
        g_HudFont = CreateFont(32,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,_T("Arial"));

    SelectObject(dc, g_HudFont);
    SetBkMode(dc, TRANSPARENT);    
    RECT r;
    r.right = 1000;
    r.bottom = 1000;

    SetTextColor(dc, RGB(190,0,0));

    r.left = 700;
    r.top = 20;
    DrawText(dc, "Fred's Artery", -1, &r, DT_LEFT|DT_TOP);



    SetTextColor(dc, RGB(0, 100, 0));
    r.left = 540;
    r.top = 450;
    
    DrawText(dc, "Age:", -1, &r, DT_LEFT|DT_TOP);

    SetTextColor(dc, RGB(200, 0,0));
    r.top += 50;
    DrawText(dc, "Health:", -1, &r, DT_LEFT|DT_TOP);
    r.top += 50;

    

    if (drawBF)
        DrawText(dc, "Bloodflow:", -1, &r, DT_LEFT|DT_TOP);
    if (flashBF && drawBF)
    {
        r.top += 50;
        DrawText(dc, "WARNING! HEART ATTACK\nIMMINENT!", -1, &r, DT_LEFT|DT_TOP);       
    }


    if (g_HeartAttack)
    {
        HFONT haFont = CreateFont(48,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,_T("Arial"));
        SelectObject(dc, haFont);

        SetTextColor(dc, RGB(200, 0,0));
        r.top = 20;
        r.left = 10;
        if ((GetTickCount() % 1000) < 500)
        {
            g_HeartAttackFlash = true;
            DrawText(dc, "HEART ATTACK!", -1, &r, DT_LEFT|DT_TOP);       
        }
        else
            g_HeartAttackFlash = false;
        r.top += 80;
        if (fabs(g_ManAngle) > PI/2)
        {
            DrawText(dc, "Eat healthier to survive\nto old age!", -1, &r, DT_LEFT|DT_TOP);
            r.top += 120;
            DrawText(dc, "Press a key to restart...", -1, &r, DT_LEFT|DT_TOP);

        }

        DeleteObject(haFont);
    }

    if (g_GameEnded && !g_HeartAttack)
    {        
        HFONT haFont = CreateFont(48,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,_T("Arial"));
        SelectObject(dc, haFont);
        SetTextColor(dc, RGB(0, 200,0));      
        r.top = 20;
        r.left = 10;
        DrawText(dc, "CONGRATULATIONS!", -1, &r, DT_LEFT|DT_TOP);
        r.top += 70;
        DrawText(dc, "You made it to old age!\nKeep up the healthy diet!", -1, &r, DT_LEFT|DT_TOP);

        r.top += 120;
        DrawText(dc, "Press a key to restart...", -1, &r, DT_LEFT|DT_TOP);
        DeleteObject(haFont);
    }

    pd3dx->GetBackBuffer(0)->ReleaseDC(dc);
  
}

void RenderSplashes()
{
    if (!g_SplashBitmap)
        return;

    HDC dc = NULL;
    HANDLEDDERR( pd3dx->GetBackBuffer(0)->GetDC(&dc));

    DrawState(
        dc,
        NULL,
        NULL,
        (LPARAM)g_SplashBitmap,
        NULL, 
        0,
        0,
        0,
        0,
        DST_BITMAP);

    
    SetTextColor(dc, RGB(128, 0,0));

    RECT r;
    r.right = 2000;
    r.bottom = 2000;    

    r.left = 685;
    r.top = 740;

    if (GetTickCount() % 1000 > 500 && GetTickCount() > g_AllowKeyToSkipTime)
    {
        if (g_LastSplash)
        {
            HFONT haFont = CreateFont(64,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,_T("Arial"));
            SelectObject(dc, haFont);
            r.left = 130;
            r.top = 700;
            DrawText(dc, "Press a key to start the game!", -1, &r, DT_LEFT|DT_TOP);
            DeleteObject(haFont);
        }
        else
        {
            HFONT haFont = CreateFont(32,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,_T("Arial"));
            SelectObject(dc, haFont);
            DrawText(dc, "Press a key to continue...", -1, &r, DT_LEFT|DT_TOP);
            DeleteObject(haFont);
        }
    }
     

    pd3dx->GetBackBuffer(0)->ReleaseDC(dc);
}

void GameRender()
{
    if (g_DisplaySplashes)
    {     
        return;
    }
    
    pd3dx->SetClearColor(RGB_MAKE(116,116,116));
    pd3dx->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
    

    D3DXPrepareDeviceForSprite(pd3dx->GetD3DDevice());

    g_BGSprite->Draw(0, SURFACE_HEIGHT - g_BGSprite->m_Height + 0.5f);
    g_ManSprite->Draw(g_ManX,g_ManY, !g_ManLookLeft, g_ManAngle);
    DrawFallingThings();    
}
