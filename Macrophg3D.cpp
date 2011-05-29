#define D3D_OVERLOADS
#include "athero.h"
#include "ddrawx.h"
#include <time.h>
#include "SimulationInfo.h"
#include "Macrophg.h"
#include "MacroGame.h"



float CameraAngleY = -2.18f; //Y controls the rotational position of the camera
float CameraAngleX = 0;
float CameraAngleZ = 0.252f; //Z controls how 'high' the camera is
const float MousePixelsToRadiansFactor_z = 0.01f;
const float MousePixelsToRadiansFactor_y = 0.01f;
const float MousePixelsToDistance = 4.0f;
const float MaxCameraDistance = 10000.0f;
const float MinCameraDistance = 100.0f;
float CameraDistance = 544.0f;
const float cellsToHeightRatio = 0.5f;
LPDIRECTDRAWSURFACE7 pTextureSurface = NULL;
const int GRIDWIDTH = 100;
const int GRIDHEIGHT = 100;



BOOL bHaveGeometry = FALSE;

D3DXVECTOR3 CameraPos;
D3DXVECTOR3 CameraTarget;

D3DLVERTEX cellGrid[GRIDWIDTH * GRIDHEIGHT * 2 + GRIDHEIGHT*2];
unsigned short cellIndicies [CELL_BUFFER_X_SIZE * CELL_BUFFER_Y_SIZE*6];
int NumVerts;
int NumIndicies;

D3DXVECTOR3 TransformPoint (D3DXVECTOR3& point)
{
	LPDIRECT3DDEVICE7 pd3dd = pd3dx_3D->GetD3DDevice();

	D3DXVECTOR4 res2;
	D3DMATRIX world,view,projection;

	pd3dd->GetTransform( D3DTRANSFORMSTATE_WORLD, &world);
	pd3dd->GetTransform( D3DTRANSFORMSTATE_VIEW, &view);
    pd3dd->GetTransform( D3DTRANSFORMSTATE_PROJECTION, &projection);

	D3DMATRIX trans =  view*world;
	D3DXVec3Transform(
		&res2,
		&point,
		(D3DXMATRIX*)&trans
	); 
	trans = projection*trans;
	
	D3DXVec3Transform(
		&res2,
		&point,
		(D3DXMATRIX*)&trans
	); 

	return D3DXVECTOR3 (res2.x, res2.y, res2.z);
}

void PositionCamera ()
{
	//rotate round y axis first then z

	D3DXMATRIX trans1,trans2,trans3;
	D3DXMatrixMultiply (
		&trans3,
		D3DXMatrixRotationZ(
			&trans2,
			CameraAngleZ),
		D3DXMatrixRotationY(
			&trans1,
			CameraAngleY)
			);
	D3DXVECTOR3 camPos (CameraDistance, 0, 0);

	D3DXVec3TransformCoord(
		&CameraPos,
		&camPos,
		&trans3);
}



void Initialise3D ()
{
	PositionCamera ();

	DWORD width = CELL_BUFFER_X_SIZE;
	DWORD height = CELL_BUFFER_X_SIZE;

	D3DX_SURFACEFORMAT surfFMT = D3DX_SF_R5G5B5;

	DWORD numMipMaps = 0;
	DWORD pFlags = D3DX_TEXTURE_NOMIPMAP;

	LPDIRECT3DDEVICE7 pd3dd = pd3dx_3D->GetD3DDevice();
	
    /*
	HANDLEDDERR( D3DXCreateTexture(
		pd3dd,
		&pFlags,
		&width,
		&height,
		&surfFMT,
		NULL,
		&pTextureSurface,
		&numMipMaps
	))
    */

	
}

void UpdateTextureSurface ()
{

	LPDIRECT3DDEVICE7 pd3dd = pd3dx_3D->GetD3DDevice();

	if (0 && CellSurfaceChanged)
	{
		EnterCriticalSection (&LockCritSection);

		HANDLEDDERR(D3DXLoadTextureFromSurface(
			pd3dd,
			pTextureSurface,
			0,
			pCellSurface,
			NULL,
			NULL,
			D3DX_FT_POINT));
		CellSurfaceChanged = false;
		LeaveCriticalSection (&LockCritSection);
	}


}

void ShutDown3D ()
{
	if (pTextureSurface)
		pTextureSurface->Release ();
	pTextureSurface = NULL;
}

void Do3DViewPosAdjustment (int x, int y)
{
	CameraAngleY += ((float)x) * MousePixelsToRadiansFactor_y;
	CameraAngleZ += ((float)y) * MousePixelsToRadiansFactor_z;

	if (CameraAngleZ < D3DXToRadian(3))
		CameraAngleZ = D3DXToRadian(3);

	if (CameraAngleZ > D3DXToRadian(89))
		CameraAngleZ = D3DXToRadian(89);


	PositionCamera ();
}

void Do3DZoomAdjustment (int x, int y)
{
	CameraDistance += ((float)y) * MousePixelsToDistance;
	if (CameraDistance < MinCameraDistance)
		CameraDistance = MinCameraDistance;
	if (CameraDistance > MaxCameraDistance)
		CameraDistance = MaxCameraDistance;
	PositionCamera ();
}


#define DOGRIDSQUARE(x,y,weight)\
{\
	int _x = x; int _y = y;\
	if ( (_x > 0) && (_x < CELL_BUFFER_X_SIZE) && (_y >0) && (_y < CELL_BUFFER_Y_SIZE) )\
	{\
			int gridpos = _x + _y * CELL_BUFFER_X_SIZE;\
			int height = MacrophageDataBuffer[gridpos].NoofCellsOnPixel + \
				 SMCDataBuffer[gridpos].NoofCellsOnPixel +\
				 LipidDataBuffer[gridpos].NoofBlobsOnPixel;			\
			totalheight += height * weight;\
			totalweight += weight;\
	}\
}	



D3DXVECTOR3 GetCylinderPosition (int x, int y, float height, D3DXVECTOR3& corner, D3DXVECTOR3& lengthvec, D3DXVECTOR3& widthvec, D3DXVECTOR3* pNormal)
{
	//build a cylinder of radius widthvec/2 whose end lies in the direction of widthvec

    //curve the long axis around curve_radius
    float curve_radius = 1000.0f;
    float length = D3DXVec3Length(&lengthvec);
    float angle_per_len = atan2(length, curve_radius);
    float rot_angle = angle_per_len * ((float)x / (float)(GRIDWIDTH-1)); 

    //curve is around the Y axis on the XZ plane
    D3DXMATRIX rotmat;
    D3DXMatrixRotationY(&rotmat, -rot_angle);

	float radius = D3DXVec3Length(&widthvec) * 0.5f / PI;
	D3DXVECTOR3 endvec1 = widthvec,endvec2;
	D3DXVec3Cross(
				&endvec2,
				&lengthvec,
				&widthvec);

	D3DXVECTOR3 nendvec1,nendvec2;
	D3DXVec3Normalize(
				&nendvec1,
				&endvec1);

	D3DXVec3Normalize(
				&nendvec2,
				&endvec2);

    D3DXVec3TransformNormal(&nendvec1, &nendvec1, &rotmat);
    D3DXVec3TransformNormal(&nendvec2, &nendvec2, &rotmat);



	
	float angle = 2.0f * PI * (float)y / (float)(GRIDHEIGHT);

	D3DXVECTOR3 pos = nendvec2 * sin(angle) + nendvec1 * cos(angle); 

	*pNormal = pos; //we have them this way up, oops

	pos *= (radius-height);	 //make it to radius-height length

    D3DXVECTOR3 curve_point (-curve_radius + cos(rot_angle) * curve_radius, 0, sin(rot_angle) * curve_radius);

    pos += curve_point;

	//pos += lengthvec * ((float)x / (float)(GRIDWIDTH-1));

	pos += corner + widthvec*0.5f; //put it in the middle of where the old grid was

	return pos;
}

float CalcLight(D3DXVECTOR3& pos, D3DXVECTOR3& normal)
{
    float ambient = 0.3f;

    D3DXVECTOR3 lightDir (0.0f, -1.0f, 0.0f);

    float dot = D3DXVec3Dot(&lightDir, &normal);

    if (dot < 0) dot = 0;

    return ambient +  dot * (1.0f - ambient);
}


void BuildArtery (D3DXVECTOR3& corner, D3DXVECTOR3& lengthvec, D3DXVECTOR3& widthvec) 
{
	bHaveGeometry = TRUE;
	int index = 0;
	int index_index = 0;

	const float xFrac = 1.0f/GRIDWIDTH;
	const float yFrac = 1.0f/GRIDHEIGHT;
	
	D3DXVECTOR3 yPos = corner, temp, gridNormal;

	D3DXVec3Cross(
		&temp,
		&widthvec,
		&lengthvec);
	
	D3DXVec3Normalize(&gridNormal, &temp);

    WORD* cellSurf = (WORD*)LockCellSurface ();


	
	int x,y;
	for (y=0; y<GRIDHEIGHT; y++)
	{		
		D3DXVECTOR3 xPos = yPos;
		for (x=0; x<GRIDWIDTH; x++)
		{	
			int cellxpos = (x * CELL_BUFFER_X_SIZE)/GRIDWIDTH;
			int cellypos = (y * CELL_BUFFER_Y_SIZE)/GRIDHEIGHT;

			int totalheight = 0;
			int totalweight = 0;
			

			//interpolate using 1-4-6 grid

			DOGRIDSQUARE (cellxpos-1,cellypos-1,10);
			DOGRIDSQUARE (cellxpos,cellypos-1,10);
			DOGRIDSQUARE (cellxpos+1,cellypos-1,10);

			DOGRIDSQUARE (cellxpos-1,cellypos,10);
			DOGRIDSQUARE (cellxpos,cellypos,10);
			DOGRIDSQUARE (cellxpos+1,cellypos,10);

			DOGRIDSQUARE (cellxpos-1,cellypos+1,10);
			DOGRIDSQUARE (cellxpos,cellypos+1,10);
			DOGRIDSQUARE (cellxpos+1,cellypos+1,10);


			D3DXVECTOR3 normal;
			D3DXVECTOR3 thisPos;

			float height = ((float)totalheight/(float)totalweight)*(cellsToHeightRatio);

            //if (outside_only)
              //  height = 0;
			
			if (true)
				thisPos = GetCylinderPosition (x,y,height,corner,lengthvec,widthvec, &gridNormal);
			else
				thisPos = xPos + D3DXVECTOR3 (0,height,0);

			if (x > 0 && y > 0)
			{
				D3DXVECTOR3 w,l,norm;
				w = thisPos - *((D3DXVECTOR3*)&cellGrid[index-1]);
				l = thisPos - *((D3DXVECTOR3*)&cellGrid[index-GRIDWIDTH]);
				D3DXVec3Cross(
				&norm,
				&l,
				&w);

				D3DXVec3Normalize(
				&normal,
				&norm);
			}
			else {
				normal = gridNormal;
			}

            float lightFactor = CalcLight(thisPos, normal);

            int gridpos = cellxpos + cellypos * CELL_BUFFER_X_SIZE;
            WORD col = cellSurf[gridpos];
            BYTE b = (col & 0x1f)<<3;
            BYTE g = (col & (0x3f<<5))>>3;
            BYTE r = (col & (0x1f<<11))>>8;
            //if (outside_only)
            //{
                //r = ARTERY_RED;
                //g = ARTERY_GREEN;
                //b = ARTERY_BLUE;
            //}
            D3DCOLOR c = RGB_MAKE((int)(r*lightFactor),(int)(g*lightFactor),(int)(b*lightFactor));            
			
			cellGrid[index] = D3DLVERTEX( thisPos, c, c, (float)x/(float)GRIDWIDTH, (float)y/(float)GRIDHEIGHT);
			index++;
			xPos += widthvec*xFrac;
		}		
		yPos += lengthvec*yFrac;
	}

    UnlockCellSurface ();

    //Make the outer wall
    for (y=0; y<GRIDHEIGHT; y++)
	{
		for (x=0; x<GRIDWIDTH; x++)
        {
            float height = -10.0f;
            D3DXVECTOR3 thisPos = GetCylinderPosition (x,y,height,corner,lengthvec,widthvec, &gridNormal);
            float lightFactor = CalcLight(thisPos, -gridNormal);
            int r = ARTERY_RED;
            int g = ARTERY_GREEN;
            int b = ARTERY_BLUE;
            D3DCOLOR c = RGB_MAKE((int)(r*lightFactor),(int)(g*lightFactor),(int)(b*lightFactor));            
            cellGrid[index] = D3DLVERTEX( thisPos, c, c, 0, 0);
            index++;
        }
    }

    //Make the inner ring at each end
    for (x = 0; x < GRIDWIDTH; x+=(GRIDWIDTH-1))
    {
        for (y=0; y<GRIDHEIGHT; y++)
	    {
            float height = 0.0f;
            D3DXVECTOR3 thisPos = GetCylinderPosition (x,y,height,corner,lengthvec,widthvec, &gridNormal);
            //D3DCOLOR c = RGB_MAKE(ARTERY_RED,0xff,0xff);
            D3DCOLOR c = RGB_MAKE((int)(ARTERY_RED*0.8f),ARTERY_GREEN,ARTERY_BLUE);
            cellGrid[index] = D3DLVERTEX( thisPos, c, c, 0, 0);
            index++;
        }
    }


	NumVerts = index;
    

	int vIndex = 0;
    for (int rep = 0; rep < 2;rep++)
    {
	    for (y=0; y<(GRIDHEIGHT-1); y++)
	    {		
		    vIndex = y * GRIDWIDTH + rep * (GRIDWIDTH*GRIDHEIGHT);
		    for (x=0; x<(GRIDWIDTH-1); x++)
		    {
			    cellIndicies[index_index++] = vIndex;
			    cellIndicies[index_index++] = vIndex+GRIDWIDTH;
			    cellIndicies[index_index++] = vIndex+1;

			    cellIndicies[index_index++] = vIndex+GRIDWIDTH;
			    cellIndicies[index_index++] = vIndex+GRIDWIDTH+1;
			    cellIndicies[index_index++] = vIndex+1;
			    vIndex++;
		    }
	    }

        //close up the cylinder along its length
		vIndex = (GRIDHEIGHT-1)* GRIDWIDTH + rep * (GRIDWIDTH*GRIDHEIGHT);
		for (x=0; x<(GRIDWIDTH-1); x++)
		{
			cellIndicies[index_index++] = vIndex;
			cellIndicies[index_index++] = x + rep * (GRIDWIDTH*GRIDHEIGHT);
			cellIndicies[index_index++] = vIndex+1;

			cellIndicies[index_index++] = x + rep * (GRIDWIDTH*GRIDHEIGHT);
			cellIndicies[index_index++] = x+1 + rep * (GRIDWIDTH*GRIDHEIGHT);
			cellIndicies[index_index++] = vIndex+1;
			vIndex++;
		}

        //do the rings at the ends between cell surface and inner wall, and
        //between inner wall and outer wall

        //x is 'for each end'
        for (int inner =0;inner<2;inner++)
        {
            for (int end =0; end < 2; end++)
            {
                //vIndex starts at 0 (first ring) or GRIDWIDTH*(GRIDHEIGHT-1) (the last ring)                
                int ringidx = GRIDWIDTH*GRIDHEIGHT*2 + end * GRIDHEIGHT;
                int end_ofs = end * (GRIDWIDTH-1);
                int outer_ofs = inner * (GRIDWIDTH*GRIDHEIGHT);
                for (y=0; y<GRIDHEIGHT-1; y++)
	            {
                    vIndex = end_ofs + outer_ofs + y * GRIDWIDTH;
                    int vIndex_1 = end_ofs + outer_ofs + (y+1) * GRIDWIDTH;

                    cellIndicies[index_index++] = vIndex;
			        cellIndicies[index_index++] = ringidx;
			        cellIndicies[index_index++] = vIndex_1;

			        cellIndicies[index_index++] = ringidx;
			        cellIndicies[index_index++] = ringidx+1;
			        cellIndicies[index_index++] = vIndex_1;
                    vIndex++;
                    ringidx++;
                }

                //y = GRIDHEIGHT-1
                vIndex = end_ofs + outer_ofs + y * GRIDWIDTH;
                int vIndex_1 = end_ofs + outer_ofs;

                cellIndicies[index_index++] = vIndex;
			    cellIndicies[index_index++] = ringidx;
			    cellIndicies[index_index++] = vIndex_1;

			    cellIndicies[index_index++] = ringidx;
			    cellIndicies[index_index++] = GRIDWIDTH*GRIDHEIGHT*2 + end * GRIDHEIGHT;
			    cellIndicies[index_index++] = vIndex_1;
                vIndex++;
                ringidx++;

            }
        }
    }

	

	NumIndicies = index_index;




}

D3DVERTEX g_pvTriangleVertices[6];
int cnt = 0;

void DrawMacrophageBuffer3D()
{
	if (!bHaveGeometry)
		return;

	pd3dx_3D->SetClearColor( 
		RGB_MAKE(255 ,210, 210));
	pd3dx_3D->Clear (D3DCLEAR_TARGET);
	pd3dx_3D->Clear (D3DCLEAR_ZBUFFER);
	LPDIRECT3DDEVICE7 pd3dd = pd3dx_3D->GetD3DDevice();

    if (GameMode && !GetHeartAttackFlash())
        return;

	HANDLEDDERR(pd3dd->BeginScene ());	

	cnt--;

	
	CameraTarget = D3DVECTOR (0.0f, -0.0f, 0.0f);

	
    // Start by setting up an identity matrix.
    D3DMATRIX mat;
    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
    
    // The world matrix controls the position and orientation 
    // of the polygons in world space. We'll use it later to 
    // spin the triangle.
    D3DXMATRIX matWorld = mat;
    pd3dd->SetTransform( D3DTRANSFORMSTATE_WORLD, matWorld );

	//CameraAngleY += 0.1;
	//CameraAngleZ += 0.1;
	//PositionCamera ();
 
    // The view matrix defines the position and orientation of 
    // the camera. Here, we are just moving it back along the z-
    // axis by 10 units.
    D3DXMATRIX matViewx;
	D3DXVECTOR3 up (0,1.0f,0.0f);
	D3DXMatrixLookAt(
		&matViewx,
		&CameraPos,
		&CameraTarget,
		&up
		);

    pd3dd->SetTransform( D3DTRANSFORMSTATE_VIEW, matViewx );

	// Set up a very simple projection that scales x and y 
    // by 2, and translates z by -1.0.

	D3DXMATRIX pmat;

	D3DXMatrixPerspective(&pmat, 10, 10, 10.0f, 50000);

	/*
    D3DMATRIX matProj = mat;
    matProj._11 =  2.0f;
    matProj._22 =  2.0f;
    matProj._34 =  1.0f;
    matProj._43 = -1.0f;
    matProj._44 =  0.0f;
	*/
    pd3dd->SetTransform( D3DTRANSFORMSTATE_PROJECTION, pmat );

	D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(mtrl) );
    mtrl.ambient.r = 0.2f;
    mtrl.ambient.g = 0.2f;
    mtrl.ambient.b = 0.2f;
	mtrl.diffuse.r = 0.6f;
    mtrl.diffuse.g = 0.6f;
    mtrl.diffuse.b = 0.6f;
	mtrl.specular.r = 0.8f;
    mtrl.specular.g = 0.8f;
    mtrl.specular.b = 0.8f;
    pd3dd->SetMaterial( &mtrl );


	D3DLIGHT7 d3dLight;
 
    // Initialize the structure.
    ZeroMemory(&d3dLight, sizeof(D3DLIGHT7));
 
    // Set up for a white point light.
    d3dLight.dltType = D3DLIGHT_DIRECTIONAL;
    d3dLight.dcvDiffuse.r = 1.0f;
    d3dLight.dcvDiffuse.g = 1.0f;
    d3dLight.dcvDiffuse.b = 1.0f;
    d3dLight.dcvAmbient.r = 1.0f;
    d3dLight.dcvAmbient.g = 1.0f;
    d3dLight.dcvAmbient.b = 1.0f;
    d3dLight.dcvSpecular.r = 1.0f;
    d3dLight.dcvSpecular.g = 1.0f;
    d3dLight.dcvSpecular.b = 1.0f;
 
    // Position it high in the scene, and behind the viewer.
    // (Remember, these coordinates are in world space, so
    //  the "viewer" could be anywhere in world space, too. 
    //  For the purposes of this example, assume the viewer
    //  is at the origin of world space.)
    d3dLight.dvPosition.x = 0.0f;
    d3dLight.dvPosition.y = 5000.0f;
    d3dLight.dvPosition.z = 0.0f;
	d3dLight.dvDirection.x = 0.3;
	d3dLight.dvDirection.y = 0.3;
	d3dLight.dvDirection.z = 0.3;
 
    // Don't attenuate.
    d3dLight.dvAttenuation0 = 1.0f; 
    d3dLight.dvRange = D3DLIGHT_RANGE_MAX;
    // Set the property info for the first light.
    HANDLEDDERR(pd3dd->SetLight(0, &d3dLight));
	HANDLEDDERR(pd3dd->LightEnable (0, FALSE));

	d3dLight.dvPosition.x = 0.0f;
    d3dLight.dvPosition.y = -5000.0f;
    d3dLight.dvPosition.z = 0.0f;

	d3dLight.dvDirection.x = 0.3;
	d3dLight.dvDirection.y = 0.3;
	d3dLight.dvDirection.z = 0.3;
 

	HANDLEDDERR(pd3dd->SetLight(1, &d3dLight));
	HANDLEDDERR(pd3dd->LightEnable (1, FALSE));

 

   
	pd3dd->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	//pd3dd->SetRenderState( D3DRENDERSTATE_DIFFUSEMATERIALSOURCE , D3DMCS_COLOR1);
	//pd3dd->SetRenderState( D3DRENDERSTATE_SPECULARMATERIALSOURCE  , D3DMCS_COLOR1);
	pd3dd->SetRenderState( D3DRENDERSTATE_AMBIENTMATERIALSOURCE  , D3DMCS_COLOR1);
	pd3dd->SetRenderState( D3DRENDERSTATE_AMBIENT, RGBA_MAKE(0xff, 0xff, 0xff, 0) );

	//pd3dd->SetRenderState( D3DRENDERSTATE_AMBIENT, RGBA_MAKE(0xff, 0xff, 0xff, 0) );
	pd3dd->SetRenderState( D3DRENDERSTATE_WRAP0, D3DWRAPCOORD_0|D3DWRAPCOORD_1);

	//pd3dd->SetRenderState( D3DRENDERSTATE_FILLMODE , D3DFILL_WIREFRAME );

	D3DVECTOR p1( 0.0f, 300.0f, -30);
    D3DVECTOR p2( 300.0f,-300.0f, -30);
    D3DVECTOR p3(-300.0f,-300.0f, -30);
    D3DVECTOR vNormal( 0.0f, 0.0f, 1.0f );

	D3DXMATRIX trans =  matViewx*matWorld;

	trans = pmat*trans;


	D3DXVECTOR4 transres1,transres2,transres3;
	D3DXVECTOR3 p12= p1;
	D3DXVECTOR3 p22= p2;
	D3DXVECTOR3 p32= p3;

	D3DXVec3Transform(
		&transres1,
		&p12,
		&trans
	); 

	D3DXVec3Transform(
		&transres2,
		&p22,
		&trans
	); 

	D3DXVec3Transform(
		&transres3,
		&p32,
		&trans
	); 


    
    // Initialize the 3 vertices for the front of the triangle
    g_pvTriangleVertices[0] = D3DVERTEX( p1, vNormal, 0, 0 );
    g_pvTriangleVertices[1] = D3DVERTEX( p2, vNormal, 0, 0 );
    g_pvTriangleVertices[2] = D3DVERTEX( p3, vNormal, 0, 0 );
    
    // Initialize the 3 vertices for the back of the triangle
    g_pvTriangleVertices[3] = D3DVERTEX( p1, -vNormal, 0, 0 );
    g_pvTriangleVertices[4] = D3DVERTEX( p3, -vNormal, 0, 0 );
    g_pvTriangleVertices[5] = D3DVERTEX( p2, -vNormal, 0, 0 );


	
//	BuildArtery (D3DXVECTOR3(-300.0f,0.0f, -300.0f), D3DXVECTOR3 (0, 0.0f, 600.0f), D3DXVECTOR3 (600.0f,0.0f,0), &numVerts, &numIndicies);

	//pd3dd->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
      //                         cellGrid, 6, NULL );

	//UpdateTextureSurface ();

	//pd3dd->SetTexture(0, pTextureSurface);


	
	HANDLEDDERR(pd3dd->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,   
		D3DFVF_LVERTEX,
		cellGrid,                   
		NumVerts,                 
		cellIndicies,                    
		NumIndicies,                  
		0
	));
	

 

	HANDLEDDERR(pd3dd->EndScene ());	
}