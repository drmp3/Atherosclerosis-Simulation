#define D3D_OVERLOADS
#include "stdafx.h"
#include "athero.h"
#include "ddrawx.h"
#include <time.h>
#include "SimulationInfo.h"



float CameraAngleY = 0.0; //Y controls the rotational position of the camera
float CameraAngleX = 0;
float CameraAngleZ = 1.0; //Z controls how 'high' the camera is
float CameraDistance = 100.0f;
const float MousePixelsToRadiansFactor_z = 0.01f;
const float MousePixelsToRadiansFactor_y = 0.01f;

BOOL bHaveGeometry = FALSE;

D3DXVECTOR3 CameraPos;
D3DXVECTOR3 CameraTarget;

D3DVERTEX cellGrid[CELL_BUFFER_X_SIZE * CELL_BUFFER_Y_SIZE];
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
}

void ShutDown3D ()
{
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
}
		

void BuildArtery (D3DXVECTOR3& corner, D3DXVECTOR3& lengthvec, D3DXVECTOR3& widthvec)
{
	bHaveGeometry = TRUE;
	int index = 0;
	int index_index = 0;
	const int GRIDWIDTH = 100;
	const int GRIDHEIGHT = 100;

	const float xFrac = 1.0f/GRIDWIDTH;
	const float yFrac = 1.0f/GRIDHEIGHT;
	
	D3DXVECTOR3 yPos = corner, temp, gridNormal;

	D3DXVec3Cross(
		&temp,
		&widthvec,
		&lengthvec);
	
	D3DXVec3Normalize(&gridNormal, &temp);

	
	
	int x,y;
	for (y=0; y<GRIDHEIGHT; y++)
	{		
		D3DXVECTOR3 xPos = yPos;
		for (x=0; x<GRIDWIDTH; x++)
		{	
			int cellxpos = (x * CELL_BUFFER_X_SIZE)/GRIDWIDTH;
			int cellypos = (y * CELL_BUFFER_Y_SIZE)/GRIDHEIGHT;
			int upvec = MacrophageDataBuffer[cellxpos + cellypos * CELL_BUFFER_X_SIZE].NoofCellsOnPixel/3;
			D3DXVECTOR3 normal;

			if (x > 0 && y > 0)
			{
				D3DXVECTOR3 w,l,norm;
				w = *((D3DXVECTOR3*)&cellGrid[index]) - *((D3DXVECTOR3*)&cellGrid[index-1]);
				l = *((D3DXVECTOR3*)&cellGrid[index]) - *((D3DXVECTOR3*)&cellGrid[index-GRIDWIDTH]);
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
			
			cellGrid[index] = D3DVERTEX( xPos + D3DXVECTOR3 (0,upvec,0), normal, 0, 0 );
			index++;
			xPos += widthvec*xFrac;

			D3DXVECTOR3 vec = TransformPoint(xPos);
			if (upvec != 0)
			{
				int a = 2;
			}

		}		
		yPos += lengthvec*yFrac;
	}

	NumVerts = index;


	int vIndex = 0;
	for (y=0; y<(GRIDHEIGHT-1); y++)
	{		
		vIndex = y * GRIDWIDTH;
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
	
	NumIndicies = index_index;



}

D3DVERTEX g_pvTriangleVertices[6];
int cnt = 0;

void DrawMacrophageBuffer3D()
{
	if (!bHaveGeometry)
		return;

	pd3dx_3D->Clear (D3DCLEAR_TARGET);
	pd3dx_3D->Clear (D3DCLEAR_ZBUFFER);
	LPDIRECT3DDEVICE7 pd3dd = pd3dx_3D->GetD3DDevice();
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

	D3DXMatrixPerspective(&pmat, 100, 100, 10, 50000);

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
    mtrl.ambient.r = 0.0f;
    mtrl.ambient.g = 0.0f;
    mtrl.ambient.b = 0.0f;
	mtrl.diffuse.r = 0.8f;
    mtrl.diffuse.g = 0.8f;
    mtrl.diffuse.b = 0.8f;
    pd3dd->SetMaterial( &mtrl );


	D3DLIGHT7 d3dLight;
 
    // Initialize the structure.
    ZeroMemory(&d3dLight, sizeof(D3DLIGHT7));
 
    // Set up for a white point light.
    d3dLight.dltType = D3DLIGHT_POINT;
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
 
    // Don't attenuate.
    d3dLight.dvAttenuation0 = 1.0f; 
    d3dLight.dvRange = D3DLIGHT_RANGE_MAX;
    // Set the property info for the first light.
    HANDLEDDERR(pd3dd->SetLight(0, &d3dLight));
	HANDLEDDERR(pd3dd->LightEnable (0, TRUE));


	pd3dd->SetRenderState( D3DRENDERSTATE_AMBIENT, 0x000000 );
	pd3dd->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

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


	
	HANDLEDDERR(pd3dd->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,   
		D3DFVF_VERTEX,              
		cellGrid,                   
		NumVerts,                 
		cellIndicies,                    
		NumIndicies,                  
		0
	));
	

 

	HANDLEDDERR(pd3dd->EndScene ());	
}