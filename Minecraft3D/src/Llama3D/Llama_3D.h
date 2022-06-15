// Llama 3D Header

#ifndef __LLAMA_3D__
#define __LLAMA_3D__

#define LLAMA_VER "v.0.1 alpha"

typedef unsigned char  ll3d_uint8;
typedef signed char    ll3d_int8;

typedef unsigned short ll3d_uint16;
typedef signed short   ll3d_int16;

typedef unsigned int   ll3d_uint32;
typedef signed int     ll3d_int32;

typedef float          ll3d_float32;

#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <list>
#include <time.h>

#ifndef USE_SDL
#include <dos.h>
#include <conio.h>
#endif

#include "../Monkey2D/Monkey_2D.h"

void LL3D_ThrowError(std::string errorstr);

typedef struct LL3D_Vector2D {
	ll3d_float32 x,y,z;
	inline void Set(ll3d_float32 a,ll3d_float32 b,ll3d_float32 c = 1.0f){
		x = a; y = b; z = c;
	};
}LL3D_Vector2D;

typedef struct LL3D_Vector3D {
	ll3d_float32 x,y,z,w;
	inline void Set(ll3d_float32 a,ll3d_float32 b,ll3d_float32 c,ll3d_float32 d = 1.0f){
		x = a; y = b; z = c; w = d;
	};
}LL3D_Vector3D;

typedef struct LL3D_Matrix4x4 {
	ll3d_float32 m[4][4];
	inline void zero(){
		memset(m,0,sizeof(m));
	};

}LL3D_Matrix4x4;


#include "Llama_Math.h"
#include "Llama_Graphics.h"


typedef struct LL3D_TriAttributes {
	MK2D_RGB light_col;
	MK2D_RGB tri_color;
	ll3d_uint8 light_lum;
	ll3d_uint8 culls;
	bool affectedByLight;
	bool usesZBuffer;
}LL3D_TriAttributes;

typedef struct LL3D_Triangle {
	LL3D_Vector3D p[3];
	LL3D_Vector2D t[3]; // Texture Coords
	MK2D_Texture2D *tex;
	LL3D_TriAttributes attr; 
	void inline set(float x1,float y1,float z1,
					float x2,float y2,float z2,
					float x3,float y3,float z3,unsigned char col){
		p[0].x = x1; p[0].y = y1; p[0].z = z1;
		p[1].x = x2; p[1].y = y2; p[1].z = z2;
		p[2].x = x3; p[2].y = y3; p[2].z = z3;
		attr.tri_color = MK2D_FindPalRGB(col);
	};
	void inline translate(float x, float y, float z){
		p[0].x += x; p[1].x += x; p[2].x += x;
		p[0].y += y; p[1].y += y; p[2].y += y;
		p[0].z += z; p[1].z += z; p[2].z += z;
	};
}LL3D_Triangle;

typedef struct LL3D_Mesh {
	std::vector<LL3D_Triangle> tris;
	void inline translate(float x, float y, float z){
		for(int i = 0; i < tris.size(); i++){
			tris[i].translate(x,y,z);
		}
	};
	void inline setcolor(unsigned char color){
		for(int i = 0; i < tris.size(); i++){
			tris[i].attr.tri_color = MK2D_FindPalRGB(color);
		}
	};
}LL3D_Mesh;

typedef struct LL3D_Camera {
	LL3D_Vector3D position;
	LL3D_Vector3D rotation;
	LL3D_Matrix4x4 matProj;
	LL3D_Vector3D vLookDir;

	inline void Reset(){
		position.Set(0,0,0);
		rotation.Set(0,0,0);
		vLookDir.Set(0,0,0);
	};

	inline void SetMat(float fov, float aspect){
		matProj = Matrix_MakeProjection(fov, aspect, 0.1f, 1000.0f);
	};

	inline void clamp(){
		if(rotation.x <= -1.0f) rotation.x = -1.0f;	
		if(rotation.x >= 1.0f) rotation.x = 1.0f;	
		if(rotation.y < 0) rotation.y += LL3D_2PI;
		if(rotation.y >= LL3D_2PI) rotation.y -= LL3D_2PI;	
		if(rotation.z < 0) rotation.z += LL3D_2PI;
		if(rotation.z >= LL3D_2PI) rotation.z -= LL3D_2PI;	
	};
}LL3D_Camera;

typedef enum {
	LL3D_LIGHT_DISABLED,
	LL3D_LIGHT_AMBIENT,
	LL3D_LIGHT_DIRECTIONAL,
	LL3D_LIGHT_POINT
}LL3D_LightType;

typedef struct LL3D_Light {
	LL3D_Vector3D pos;
	LL3D_Vector3D dir;
	MK2D_RGB col;
	LL3D_LightType type;
	inline void Reset(void){
		pos.Set(0,0,0);
		dir.Set(0,0,0);
		col.SetRGB(0,0,0);
		type = LL3D_LIGHT_DISABLED;
	};
}LL3D_Light;


extern LL3D_Mesh LL3D_WorldMesh;


bool LL3D_Init();
void LL3D_Shutdown();
void LL3D_SetRenderType(LL3D_RenderType r);
void LL3D_SetRasterType(LL3D_RasterType r);

void LL3D_SetWorldCamera(LL3D_Camera &cam);
void LL3D_ProjectCamera();
void LL3D_Translate(ll3d_float32 x, ll3d_float32 y, ll3d_float32 z);
void LL3D_TranslateScene(ll3d_float32 x, ll3d_float32 y, ll3d_float32 z);
void LL3D_RotateMesh(LL3D_Mesh &mesh, LL3D_Vector3D rotation);

void LL3D_ClearScene();
bool LL3D_AddLight(LL3D_Vector3D &p,LL3D_Vector3D &d, MK2D_RGB &rgbc, LL3D_LightType ltype);
void LL3D_AddMesh(LL3D_Mesh &mesh);
void LL3D_AddCube(ll3d_float32 x, ll3d_float32 y, ll3d_float32 z, MK2D_Texture2D *tex, bool cull_faces);
bool LL3D_LoadObjFile(std::string name, LL3D_Mesh &mesh);

int Triangle_ClipAgainstPlane(LL3D_Vector3D plane_p, LL3D_Vector3D plane_n, LL3D_Triangle &in_tri, LL3D_Triangle &out_tri1, LL3D_Triangle &out_tri2);
void LL3D_ProjectObjects();
void LL3D_RenderObjects();

// Make sure to call this befroe render scene
void LL3D_ClearRender(unsigned char clearcolor = 0);
// Use this instead of Project and Render
int LL3D_RenderScene(int flags = LL3D_FLAG_RENDER_CW | LL3D_FLAG_RENDER_LIGHTS, LL3D_Mesh &mesh2rend = LL3D_WorldMesh);

int LL3D_GetNumOfTris();
int LL3D_GetNumOfRasterTris();

bool LL3D_IsError();
void LL3D_PrintExitCode();

// What kind of rendering do we want to use
extern LL3D_RenderType LL3D_RenderingType;
extern LL3D_RasterType LL3D_RasteringType;

#endif

