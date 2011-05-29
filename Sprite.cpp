//Sprite class
#include "stdafx.h"
#include "athero.h"
#include "Sprite.h"
#include "ddrawx.h"
#include "d3dxsprite.h"

union DDS_header {
  struct {
    unsigned int    dwMagic;
    unsigned int    dwSize;
    unsigned int    dwFlags;
    unsigned int    dwHeight;
    unsigned int    dwWidth;
    unsigned int    dwPitchOrLinearSize;
    unsigned int    dwDepth;
    unsigned int    dwMipMapCount;
    unsigned int    dwReserved1[ 11 ];

    //  DDPIXELFORMAT
    struct {
      unsigned int    dwSize;
      unsigned int    dwFlags;
      unsigned int    dwFourCC;
      unsigned int    dwRGBBitCount;
      unsigned int    dwRBitMask;
      unsigned int    dwGBitMask;
      unsigned int    dwBBitMask;
      unsigned int    dwAlphaBitMask;
    }               sPixelFormat;

    //  DDCAPS2
    struct {
      unsigned int    dwCaps1;
      unsigned int    dwCaps2;
      unsigned int    dwDDSX;
      unsigned int    dwReserved;
    }               sCaps;
    unsigned int    dwReserved2;
  };
  char data[ 128 ];
};


Sprite::Sprite(const char* filename)
{
    char res[256];
    strcpy(res, filename);
    m_Width = 0;
    m_Height = 0;

    FILE* f = fopen(filename, "rb");
    if (!f)
    {
        MessageBox(AtheroWindow, filename, "File not found", MB_ICONEXCLAMATION);
        return;
    }

    DDS_header hdr;
    fread(&hdr, sizeof(DDS_header), 1, f);
    fclose(f);

    m_Width = hdr.dwWidth;
    m_Height = hdr.dwHeight;


    HANDLEDDERR(  D3DXCreateTextureFromFile( 
                               pd3dx->GetD3DDevice(),
                               NULL, 
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               &m_Surf,
                               NULL,
                               res,
                               D3DX_FT_DEFAULT));
}

void Sprite::Draw(float x, float y, bool flipx, float angle)
{    
    D3DXVECTOR3 pos;
    pos.x = x + m_Width/2;
    pos.y = y + m_Height/2;
    pos.z = 1.0f;
    

    RECT srcRect;
    RECT *psrcRect = NULL;
        
    if (flipx)
    {
        srcRect.top = 0;
        srcRect.bottom = m_Height;        
        srcRect.right = 0;
        srcRect.left = m_Width;
        psrcRect = &srcRect;
    }

    D3DXDrawSpriteSimple(m_Surf, pd3dx->GetD3DDevice(), &pos, 1.0f, 1.0f, angle, NULL, psrcRect);
}

void Sprite::DrawSub(float x, float y, float subx, float suby, float subw, float subh)
{
    D3DXVECTOR3 pos;
    pos.x = x + subw/2;
    pos.y = y + subh/2;
    pos.z = 1.0f;
    

    RECT srcRect;
    srcRect.top = suby;
    srcRect.bottom = suby + subh;        
    srcRect.right = subx + subw;
    srcRect.left = subx;

    D3DXDrawSpriteSimple(m_Surf, pd3dx->GetD3DDevice(), &pos, 1.0f, 1.0f, 0.0f, NULL, &srcRect);
}
