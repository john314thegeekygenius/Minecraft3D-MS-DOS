/*
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;
*/
#include "Llama_3D.h"

// Error stuff
bool LL3D_IsThereError = false;
std::string ErrorString = "";

bool LL3D_Init(){
	std::cout << "Llama 3D Software Renderer for MS-DOS " << LLAMA_VER << std::endl;
	std::cout << "Copyright (c) JB Games LLC 2021 - 2022 " << std::endl;

	// Make sure there are no false errors
	LL3D_IsThereError = false;
	ErrorString = "";

	// Setup the math stuff
	if(!LL3D_SetupMath())
		return false;

	LL3D_RenderingType = LL3D_REND_TEX; // Fancy Textures!
	LL3D_RasteringType = LL3D_RAST_SOLID; // Fancy Textures!

	return true;
};

void LL3D_Shutdown(){
	// Free the math stuff
	LL3D_FreeMath();

	LL3D_PrintExitCode();
};

void LL3D_SetRenderType(LL3D_RenderType r){
	LL3D_RenderingType = r;
};

void LL3D_SetRasterType(LL3D_RasterType r){
	LL3D_RasteringType = r;
};


/****************************************************

		3D Stuff

*****************************************************/

// Vector that stores the world mesh data
LL3D_Mesh LL3D_WorldMesh;

// Store triagles for rastering later
std::vector<LL3D_Triangle> LL3D_Triangles2Raster;

extern MK2D_PixelBuffer *LL3D_PixelBuffer;
extern PixelBuffer_f *LL3D_DepthBuffer;

#define LL3D_MAX_LIGHTS 8

LL3D_Light LL3D_SceneLights[LL3D_MAX_LIGHTS];
int LL3D_LightAvailable = 0;

void LL3D_ClearScene(){
	LL3D_WorldMesh.tris.clear();
	for(int i = 0 ; i < LL3D_MAX_LIGHTS ; i++)
		LL3D_SceneLights[i].Reset();
	LL3D_LightAvailable = 0;
};

bool LL3D_AddLight(LL3D_Vector3D &p,LL3D_Vector3D &d, MK2D_RGB &rgbc, LL3D_LightType ltype){
	if(LL3D_LightAvailable < LL3D_MAX_LIGHTS){
		LL3D_SceneLights[LL3D_LightAvailable].pos = p;
		LL3D_SceneLights[LL3D_LightAvailable].dir = d;
		LL3D_SceneLights[LL3D_LightAvailable].col = rgbc;
		LL3D_SceneLights[LL3D_LightAvailable].type = ltype;
		LL3D_LightAvailable += 1;
		return true;
	}
	return false;
};

void LL3D_AddMesh(LL3D_Mesh &mesh){
	for(int i = 0; i < mesh.tris.size(); i++){
		LL3D_WorldMesh.tris.push_back(mesh.tris[i]);
	}
};

LL3D_Triangle meshCube[12] = {
	// SOUTH
	{ 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,}, 
	{ 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
					  																			   
	// EAST           																			   
	{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
	{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
					   																			   
	// NORTH           																			   
	{ 1.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
	{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
					   																			   
	// WEST            																			   
	{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
	{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
					   																			   
	// TOP             																			   
	{ 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
	{ 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},
					   																			  
	// BOTTOM          																			  
	{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
	{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

	};

void LL3D_AddCube(ll3d_float32 x, ll3d_float32 y, ll3d_float32 z, MK2D_Texture2D *tex, bool cull_faces){
	LL3D_Triangle cubetri;
	for(int i = 0; i < 12; i++){
		cubetri = meshCube[i];

		cubetri.p[0].x += x;
		cubetri.p[1].x += x;
		cubetri.p[2].x += x;
		cubetri.p[0].y += y;
		cubetri.p[1].y += y;
		cubetri.p[2].y += y;
		cubetri.p[0].z += z;
		cubetri.p[1].z += z;
		cubetri.p[2].z += z;

		cubetri.attr.culls = cull_faces;
		cubetri.attr.affectedByLight = true;
		cubetri.attr.usesZBuffer = true;
		cubetri.tex = tex;
//		printf("x %f,y %f,z %f\n",cubetri.p[0].x,cubetri.p[0].y,cubetri.p[0].z);
		LL3D_WorldMesh.tris.push_back(cubetri);
	}
	if(!LL3D_WorldMesh.tris.size()){
		LL3D_ThrowError("Could not push mesh into world!\n");
	}
};

bool LL3D_LoadObjFile(std::string name, LL3D_Mesh &mesh){
	FILE *fp = NULL;

	#define READ2END(c) while(ch!=c){ ch = fgetc(fp); if(ch==EOF){return false;} };

	fp = fopen(name.c_str(),"rb");
	if(!fp){
		return false;
	}else{
		// Local cache of verts
		std::vector<LL3D_Vector3D> verts;

		int ch = 0;
		while(1){
			ch = fgetc(fp);
			if(ch==EOF){
				return true; // ???
			}
			if(ch=='#'){
				// Read to end of line
				READ2END('\n')
			}else
			if(ch=='v'){
				LL3D_Vector3D v;
				float off;
				bool neg = false;
				// Read to float
				ch = fgetc(fp);
				//READ2END(' ')

				// Decode Float
			#define DECODEFLOAT(a)\
				ch = fgetc(fp);\
				neg = false;\
				if(ch=='-'){\
					neg = true;\
					ch = fgetc(fp); \
				}\
				while(ch!='.'){ \
					a += (float)(ch-'0');\
					ch = fgetc(fp); \
					if(ch==EOF){return false;} \
				};\
				if(ch=='.'){\
					off = 10.0f;\
					ch = fgetc(fp); \
					while(ch!=' '&&ch!='\n'){ \
						a += (float)(ch-'0') / off;\
						off *= 10.0f;\
						ch = fgetc(fp); \
						if(ch==EOF){return false;} \
					};\
				}\
				if(neg) a *= -1.0f;

				v.Set(0,0,0);
				DECODEFLOAT(v.x)
				DECODEFLOAT(v.y)
				DECODEFLOAT(v.z)

//				printf("x %f,y %f,z %f\n",v.x,v.y,v.z);
				verts.push_back(v);
			}else
			if(ch=='f'){
				int f[3];
				// Read to int
				READ2END(' ')

				// Decode Int
			#define DECODEINT(a)\
				ch = fgetc(fp);\
				a = 0;\
				while(ch!=' '&&ch!='\n'){ \
					a *= 10;\
					a += (ch-'0');\
					ch = fgetc(fp); \
					if(ch==EOF){return false;} \
				};

				DECODEINT(f[0])
				DECODEINT(f[1])
				DECODEINT(f[2])

				f[0] -= 1; if(f[0]<0) return false;
				f[1] -= 1; if(f[1]<0) return false;
				f[2] -= 1; if(f[2]<0) return false;

				LL3D_Triangle mvt;
				mvt.p[0] = verts[f[0]];
				mvt.p[1] = verts[f[1]];
				mvt.p[2] = verts[f[2]];
				mvt.t[0].Set(0,0);
				mvt.t[1].Set(0,0);
				mvt.t[2].Set(0,0);
				mvt.tex = NULL;
				mvt.attr.culls = true;
				mvt.attr.affectedByLight = true;
				mvt.attr.usesZBuffer = true;
//				printf("F: %i x %f,y %f,z %f\n",f[0],mvt.p[0].x,mvt.p[0].y,mvt.p[0].z);
//				getch();
				mesh.tris.push_back(mvt);
			}else{
				READ2END('\n')
			}
		}
	}
	return true;
};


/***************************************************************************
 * 
 * 
 * 
 *           Projection Stuff
 * 
 * 
 * 
 * **************************************************************************/


LL3D_Camera * ProjectionCamera;

LL3D_Matrix4x4 matWorld;
LL3D_Matrix4x4 matView;
LL3D_Matrix4x4 matWorldView;

LL3D_Vector3D WorldTransform;
LL3D_Vector3D WorldTranslate;

void LL3D_SetWorldCamera(LL3D_Camera &cam){
	ProjectionCamera = &cam;
	if(!ProjectionCamera)
		LL3D_ThrowError("Could not set projection camera!\n");
	WorldTransform.Set(0,0,0);
	WorldTranslate.Set(0,0,0);

};

void LL3D_Translate(ll3d_float32 x, ll3d_float32 y, ll3d_float32 z){
	WorldTransform.Set(x,y,z);
};

void LL3D_TranslateScene(ll3d_float32 x, ll3d_float32 y, ll3d_float32 z){
	WorldTranslate.Set(x,y,z);
};

void LL3D_ProjectCamera(){
	// Set up "World Tranmsform" 
	LL3D_Matrix4x4 matRotZ, matRotX, matRotZX;
	LL3D_Matrix4x4 matTrans;

	matRotZ = Matrix_MakeRotationZ(ProjectionCamera->rotation.z);
	matRotX = Matrix_MakeRotationX(ProjectionCamera->rotation.x);

	matWorld = Matrix_MakeIdentity();	// Form World Matrix
	matRotZX = Matrix_MultiplyMatrix(matRotX, matRotZ); // Transform by rotation

	//matTrans = Matrix_MakeTranslation(WorldTransform.x,WorldTransform.y,WorldTransform.z);

	// Rotate the world
//	matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX); // Transform by rotation
	//matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

	LL3D_Vector3D vUp = { 0,1,0 };
	LL3D_Vector3D vTarget = { 0,0,1 };

	// Create "Point At" Matrix for camera
	LL3D_Matrix4x4 matCameraRot = Matrix_MakeRotationY(ProjectionCamera->rotation.y);
	matCameraRot = Matrix_MultiplyMatrix(matCameraRot, matRotZX); // Transform by rotation

	ProjectionCamera->vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
	vTarget = Vector_Add(ProjectionCamera->position, ProjectionCamera->vLookDir);
	LL3D_Matrix4x4 matCamera = Matrix_PointAt(ProjectionCamera->position, vTarget, vUp);

	// Make view matrix from camera
	matView = Matrix_QuickInverse(matCamera);

	// Calculate Transformation Matrix
	matWorldView = Matrix_MultiplyMatrix(matWorld, matView);

};


int Triangle_ClipAgainstPlane(LL3D_Vector3D plane_p, LL3D_Vector3D plane_n, LL3D_Triangle &in_tri, LL3D_Triangle &out_tri1, LL3D_Triangle &out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n = Vector_Normalise(plane_n);

	out_tri1.t[0] = in_tri.t[0];
	out_tri2.t[0] = in_tri.t[0];
	out_tri1.t[1] = in_tri.t[1];
	out_tri2.t[1] = in_tri.t[1];
	out_tri1.t[2] = in_tri.t[2];
	out_tri2.t[2] = in_tri.t[2];

	float d3 = Vector_DotProduct(plane_n, plane_p);

	// Return signed shortest distance from point to plane, plane normal must be normalised
	#define tdist(p) ((plane_n.x * p.x) + (plane_n.y * p.y) + (plane_n.z * p.z) - d3)

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	LL3D_Vector3D* inside_points[3];  int nInsidePointCount = 0;
	LL3D_Vector3D* outside_points[3]; int nOutsidePointCount = 0;
	LL3D_Vector2D* inside_tex[3]; int nInsideTexCount = 0;
	LL3D_Vector2D* outside_tex[3]; int nOutsideTexCount = 0;


	// Get signed distance of each point in triangle to plane
	float d0 = tdist(in_tri.p[0]);
	float d1 = tdist(in_tri.p[1]);
	float d2 = tdist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; }
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
	}
	if (d1 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1];
	}
	if (d2 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
	}

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
		out_tri1.attr =  in_tri.attr;
		out_tri1.tex =  in_tri.tex;

//		out_tri1.attr.light_col.SetRGB(255,255,0);

		// The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];
		out_tri1.t[0] = *inside_tex[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		float t;
		out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[1].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
		out_tri1.t[1].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
		out_tri1.t[1].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;

		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
		out_tri1.t[2].x = t * (outside_tex[1]->x - inside_tex[0]->x) + inside_tex[0]->x;
		out_tri1.t[2].y = t * (outside_tex[1]->y - inside_tex[0]->y) + inside_tex[0]->y;
		out_tri1.t[2].z = t * (outside_tex[1]->z - inside_tex[0]->z) + inside_tex[0]->z;

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles
		out_tri1.attr =  in_tri.attr;
		out_tri1.tex =  in_tri.tex;

		out_tri2.attr =  in_tri.attr;
		out_tri2.tex =  in_tri.tex;

	//	out_tri1.attr.light_col.SetRGB(255,0,0);
	//	out_tri2.attr.light_col.SetRGB(0,255,0);

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.t[0] = *inside_tex[0];
		out_tri1.t[1] = *inside_tex[1];

		float t;
		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[2].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
		out_tri1.t[2].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
		out_tri1.t[2].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.t[0] = *inside_tex[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.t[1] = out_tri1.t[2];
		
		out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
		out_tri2.t[2].x = t * (outside_tex[0]->x - inside_tex[1]->x) + inside_tex[1]->x;
		out_tri2.t[2].y = t * (outside_tex[0]->y - inside_tex[1]->y) + inside_tex[1]->y;
		out_tri2.t[2].z = t * (outside_tex[0]->z - inside_tex[1]->z) + inside_tex[1]->z;

		return 2; // Return two newly formed triangles which form a quad
	}
	return 0;
};

LL3D_Vector3D vOffsetView = { 1,1,0 };

LL3D_Vector3D TriClipA = { 0.0f, 0.0f, 0.1f };
LL3D_Vector3D TriClipB = { 0.0f, 0.0f, 1.0f };

int LL3D_GetNumOfTris(){
	return LL3D_WorldMesh.tris.size();
};

int LL3D_GetNumOfRasterTris(){
	return LL3D_Triangles2Raster.size();
};

void LL3D_ProjectObjects(){
	if(LL3D_IsThereError) return;

	LL3D_Triangles2Raster.clear();

	LL3D_Triangle triProjected, triTransformed, triViewed;

	// Calculate triangle Normal
	LL3D_Vector3D normal, line1, line2;

	for(int i = 0; i < LL3D_WorldMesh.tris.size(); i++){
		// World Matrix Transform
		triTransformed.p[0] = Matrix_MultiplyVector(matWorld, LL3D_WorldMesh.tris[i].p[0]);
		triTransformed.p[1] = Matrix_MultiplyVector(matWorld, LL3D_WorldMesh.tris[i].p[1]);
		triTransformed.p[2] = Matrix_MultiplyVector(matWorld, LL3D_WorldMesh.tris[i].p[2]);
		triTransformed.t[0] = LL3D_WorldMesh.tris[i].t[0];
		triTransformed.t[1] = LL3D_WorldMesh.tris[i].t[1];
		triTransformed.t[2] = LL3D_WorldMesh.tris[i].t[2];
		triTransformed.attr = LL3D_WorldMesh.tris[i].attr;
		triTransformed.tex = LL3D_WorldMesh.tris[i].tex;

		// Get lines either side of triangle
		line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
		line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

		// Take cross product of lines to get normal to triangle surface
		normal = Vector_CrossProduct(line1, line2);

		// You normally need to normalise a normal!
		normal = Vector_Normalise(normal);
		
		// Get Ray from triangle to camera
		LL3D_Vector3D vCameraRay = Vector_Sub(triTransformed.p[0], ProjectionCamera->position);

		// If ray is aligned with normal, then triangle is visible
		if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
		{
			// Illumination
			// TODO: Make this a list
			LL3D_Vector3D light_direction = { 0.0f, 1.0f, -1.0f };
			light_direction = Vector_Normalise(light_direction);

			// How "aligned" are light direction and triangle surface normal?
			float dp = LL3D_Max(0.1f, Vector_DotProduct(light_direction, normal));

			MK2D_RGB c;
			c.red = dp*255.0f;
			c.green = dp*255.0f;
			c.blue = dp*255.0f;

			// Choose colours as required
			triTransformed.attr.light_col = c;

			// Convert World Space --> View Space
			triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
			triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
			triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
			triViewed.t[0] = triTransformed.t[0];
			triViewed.t[1] = triTransformed.t[1];
			triViewed.t[2] = triTransformed.t[2];
			triViewed.attr = triTransformed.attr;
			triViewed.tex = triTransformed.tex;

			// Clip Viewed Triangle against near plane, this could form two additional
			// additional triangles. 
			int nClippedTriangles = 0;
			LL3D_Triangle clipped[2];

			nClippedTriangles = Triangle_ClipAgainstPlane(TriClipA,TriClipB, triViewed, clipped[0], clipped[1]);

			// We may end up with multiple triangles form the clip, so project as
			// required
			for (int n = 0; n < nClippedTriangles; n++)
			{
				// Project triangles from 3D --> 2D
				triProjected.p[0] = Matrix_MultiplyVector(ProjectionCamera->matProj, clipped[n].p[0]);
				triProjected.p[1] = Matrix_MultiplyVector(ProjectionCamera->matProj, clipped[n].p[1]);
				triProjected.p[2] = Matrix_MultiplyVector(ProjectionCamera->matProj, clipped[n].p[2]);
				triProjected.t[0] = clipped[n].t[0];
				triProjected.t[1] = clipped[n].t[1];
				triProjected.t[2] = clipped[n].t[2];
				triProjected.attr = clipped[n].attr;
				triProjected.tex  = clipped[n].tex;

				triProjected.t[0].x = triProjected.t[0].x / triProjected.p[0].w;
				triProjected.t[1].x = triProjected.t[1].x / triProjected.p[1].w;
				triProjected.t[2].x = triProjected.t[2].x / triProjected.p[2].w;

				triProjected.t[0].y = triProjected.t[0].y / triProjected.p[0].w;
				triProjected.t[1].y = triProjected.t[1].y / triProjected.p[1].w;
				triProjected.t[2].y = triProjected.t[2].y / triProjected.p[2].w;

				triProjected.t[0].z = 1.0f / triProjected.p[0].w;
				triProjected.t[1].z = 1.0f / triProjected.p[1].w;
				triProjected.t[2].z = 1.0f / triProjected.p[2].w;


				// Scale into view, we moved the normalising into cartesian space
				// out of the matrix.vector function from the previous videos, so
				// do this manually
				triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

				// X/Y are inverted so put them back
				triProjected.p[0].x *= -1.0f;
				triProjected.p[1].x *= -1.0f;
				triProjected.p[2].x *= -1.0f;
				triProjected.p[0].y *= -1.0f;
				triProjected.p[1].y *= -1.0f;
				triProjected.p[2].y *= -1.0f;

				// Offset verts into visible normalised space
				triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
				triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
				triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
				triProjected.p[0].x *= (float)LL3D_PixelBuffer->half_width;
				triProjected.p[0].y *= (float)LL3D_PixelBuffer->half_height;
				triProjected.p[1].x *= (float)LL3D_PixelBuffer->half_width;
				triProjected.p[1].y *= (float)LL3D_PixelBuffer->half_height;
				triProjected.p[2].x *= (float)LL3D_PixelBuffer->half_width;
				triProjected.p[2].y *= (float)LL3D_PixelBuffer->half_height;

				// Store triangle for clipping
				LL3D_Triangles2Raster.push_back(triProjected);
			}	
		}
	}
 /*   auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    std::cout << "Project:"<<ms_double.count() << "ms" << std::endl;
*/
};

LL3D_Vector3D TriClipRend[8] = {
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 199.0f, 0.0f }, { 0.0f, -1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
	{ 319, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }
};

LL3D_Vector3D TriRendClip[8] = {
	{0.0f, -1.0f, 0.0f} , {0.0f, 1.0f, 0.0f},
	{0.0f, +1.0f, 0.0f} , {0.0f, -1.0f, 0.0f},
	{-1.0f, 0.0f, 0.0f} , {1.0f, 0.0f, 0.0f},
	{ 1.0f, 0.0f, 0.0f} , {-1.0f, 0.0f, 0.0f},
};

void LL3D_RenderObjects(){
	if(LL3D_IsThereError) return;

	if(LL3D_RasteringType==LL3D_RAST_NONE) return;

   // auto t1 = high_resolution_clock::now();

	TriClipRend[2].y = (float)(LL3D_PixelBuffer->height);
	TriClipRend[6].x = (float)(LL3D_PixelBuffer->width);

	// Don't clear the screen
	//memset(LL3D_PixelBuffer->pixelbuffer,6,LL3D_PixelBuffer->fastsize);
	memset(LL3D_DepthBuffer->pixelbuffer,0,LL3D_DepthBuffer->fastsize);


	// Clip triangles against all four screen edges, this could yield
	// a bunch of triangles, so create a queue that we traverse to 
	// ensure we only test new triangles generated against planes
	LL3D_Triangle clipped[2];
	std::list<LL3D_Triangle> listTriangles;

	int nTrisToAdd = 0;

	for(int i = 0; i < LL3D_Triangles2Raster.size(); i++){

		// Add initial triangle
		listTriangles.push_back(LL3D_Triangles2Raster[i]);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			while (nNewTriangles > 0)
			{
				// Take triangle from front of queue
				LL3D_Triangle test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;
		
//				printf("x %f,y %f,z %f\n",test.p[0].x,test.p[0].y,test.p[0].z);

				// Clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p)
				{
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane(TriClipRend[0],TriClipRend[1], test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane(TriClipRend[2],TriClipRend[3], test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane(TriClipRend[4],TriClipRend[5], test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane(TriClipRend[6],TriClipRend[7], test, clipped[0], clipped[1]); break;
				}

				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				if(nTrisToAdd){
					listTriangles.push_back(clipped[0]);
					if(nTrisToAdd==2){
						listTriangles.push_back(clipped[1]);
					}
				}
			}
			nNewTriangles = listTriangles.size();
		}


		// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		while(listTriangles.size())
		{
			LL3D_Triangle &t = listTriangles.front();
			ll3d_uint8 col;
//			if(t.tex){
//				col = t.tex->SampleTex(1.0,1.0);
//			}else{
			if(!t.tex){
				MK2D_RGB lightedCol;
				lightedCol = MK2D_MultiplyRGB(t.attr.tri_color,t.attr.light_col);//MK2D_ModulateRGB(t.attr.tri_color,t.attr.light_col);
				col = MK2D_GetPalRGB(lightedCol);
			}
			/*
			if(LL3D_RenderingType==LL3D_REND_TEX&&t.tex){
				if(LL3D_RasteringType==LL3D_RAST_SOLID){
					LL3D_TexturedTriangle(t.p[0].x, t.p[0].y, t.t[0].x, t.t[0].y, t.t[0].z,
						t.p[1].x, t.p[1].y, t.t[1].x, t.t[1].y, t.t[1].z,
						t.p[2].x, t.p[2].y, t.t[2].x, t.t[2].y, t.t[2].z, t.tex);
				}else{
					
					LL3D_TexturedTriangleChunky(t.p[0].x, t.p[0].y, t.t[0].x, t.t[0].y, t.t[0].z,
						t.p[1].x, t.p[1].y, t.t[1].x, t.t[1].y, t.t[1].z,
						t.p[2].x, t.p[2].y, t.t[2].x, t.t[2].y, t.t[2].z, t.tex);
				}
			}
			if(LL3D_RenderingType==LL3D_REND_SOLID||
				(LL3D_RenderingType==LL3D_REND_TEX&&!t.tex)){
				LL3D_FillDepthTriangle( t.p[0].x, t.p[0].y, t.t[0].z,
									    t.p[1].x, t.p[1].y, t.t[1].z,
										t.p[2].x, t.p[2].y, t.t[2].z, col);
			}
			else if(LL3D_RenderingType==LL3D_REND_SOLID_NDEPTH){
				LL3D_FillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, col);
			}else if(LL3D_RenderingType==LL3D_REND_WIRE){
				LL3D_DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, col);
			}*/
			listTriangles.pop_front();
		}
	}
/*    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    std::cout << "Render:"<<ms_double.count() << "ms" << std::endl;
*/
};

// Clears the renderer
void LL3D_ClearRender(unsigned char clearcolor){
	// Don't clear the screen
	memset(LL3D_PixelBuffer->pixelbuffer,clearcolor,LL3D_PixelBuffer->fastsize);
	memset(LL3D_DepthBuffer->pixelbuffer,0,LL3D_DepthBuffer->fastsize);

};


void LL3D_RotateMesh(LL3D_Mesh &mesh, LL3D_Vector3D rotation){
	LL3D_Matrix4x4 matRotZ, matRotX, matRotY, matRotZX,matRotZXY;
	LL3D_Matrix4x4 matTrans;

	matRotX = Matrix_MakeRotationX(rotation.x);
	matRotY = Matrix_MakeRotationX(rotation.y);
	matRotZ = Matrix_MakeRotationZ(rotation.z);
	matRotZX = Matrix_MultiplyMatrix(matRotX, matRotZ);
	matRotZXY = Matrix_MultiplyMatrix(matRotZX, matRotY);

	for(int tx = 0; tx < mesh.tris.size(); tx++){
		LL3D_Triangle &tri = mesh.tris[tx];
		tri.p[0] = Matrix_MultiplyVector(matRotZXY, tri.p[0]);
		tri.p[1] = Matrix_MultiplyVector(matRotZXY, tri.p[1]);
		tri.p[2] = Matrix_MultiplyVector(matRotZXY, tri.p[2]);
	}
};

// All in one Pipline Function
int LL3D_RenderScene(int flags, LL3D_Mesh &mesh2rend){
	// How many triangles were draw to the screen
	int LL_TriangleDrawCount = 0;

	// Remove any old triangles
	//LL3D_Triangles2Raster.clear();

	// Process Triangles
	for(int tx = 0; tx < mesh2rend.tris.size(); tx++){
		LL3D_Triangle &tri = mesh2rend.tris[tx];
		LL3D_Triangle triTransformed;

		// Just copy through texture coordinates
		triTransformed.t[0].Set( tri.t[0].x, tri.t[0].y, tri.t[0].z );
		triTransformed.t[1].Set( tri.t[1].x, tri.t[1].y, tri.t[1].z );
		triTransformed.t[2].Set( tri.t[2].x, tri.t[2].y, tri.t[2].z ); // Think!

		// Dont forget attributes
		triTransformed.attr = tri.attr;
		triTransformed.tex = tri.tex;

		// Transform Triangle from object into projected space
		triTransformed.p[0] = Matrix_MultiplyVector(matWorldView, tri.p[0]);
		triTransformed.p[1] = Matrix_MultiplyVector(matWorldView, tri.p[1]);
		triTransformed.p[2] = Matrix_MultiplyVector(matWorldView, tri.p[2]);

		triTransformed.translate(WorldTranslate.x,WorldTranslate.y,WorldTranslate.z);

		// Calculate Triangle Normal in WorldView Space
		LL3D_Vector3D normal, line1, line2;
		line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
		line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);
		normal = Vector_CrossProduct(line1, line2);
		normal = Vector_Normalise(normal);

		float triDP = Vector_DotProduct(normal, triTransformed.p[0]);

		// Cull triangles that face away from viewer
		if (flags & LL3D_FLAG_RENDER_CW && triDP > 0.0f) continue; // clock wise
		if (flags & LL3D_FLAG_RENDER_CCW && triDP < 0.0f) continue; // counter clock wise
				
		// If Lighting, calculate shading
		if (flags & LL3D_FLAG_RENDER_LIGHTS){
			MK2D_RGB ambient_clamp = { 0,0,0 };
			MK2D_RGB light_combined = { 0,0,0 };
			
			float nLightR = 0, nLightG = 0, nLightB = 0;

			for (int i = 0; i < LL3D_MAX_LIGHTS; i++)
			{
				switch (LL3D_SceneLights[i].type)
				{
				case LL3D_LIGHT_DISABLED:
					break;
				case LL3D_LIGHT_AMBIENT:
					ambient_clamp = LL3D_SceneLights[i].col;						
					break;
				case LL3D_LIGHT_DIRECTIONAL:
					{
						
						LL3D_Vector3D light_dir = Vector_Normalise(LL3D_SceneLights[i].dir);
						float light = Vector_DotProduct(light_dir, normal);
						if (light > 0)
						{
							int j = 0;
						}

						light = std::max(light, 0.0f);
						nLightR += light * ((float)LL3D_SceneLights[i].col.red/255.0f);
						nLightG += light * ((float)LL3D_SceneLights[i].col.green/255.0f);
						nLightB += light * ((float)LL3D_SceneLights[i].col.blue/255.0f);
					}
					break;
				case LL3D_LIGHT_POINT:
					// Not yet supported
					break;
				}

				nLightR = std::max(nLightR, (float)ambient_clamp.red / 255.0f);
				nLightG = std::max(nLightG, (float)ambient_clamp.green / 255.0f);
				nLightB = std::max(nLightB, (float)ambient_clamp.blue / 255.0f);

			}

			nLightR *= 255.0f;
			nLightG *= 255.0f;
			nLightB *= 255.0f;

			if(nLightR>255) nLightR = 255;
			if(nLightG>255) nLightG = 255;
			if(nLightB>255) nLightB = 255;

			triTransformed.attr.light_col.SetRGB((unsigned char)nLightR,(unsigned char)nLightG,(unsigned char)nLightB);

			//triTransformed.col[0] = olc::Pixel(uint8_t(nLightR * triTransformed.col[0].r), uint8_t(nLightG * triTransformed.col[0].g), uint8_t(nLightB * triTransformed.col[0].b));
			//triTransformed.col[1] = olc::Pixel(uint8_t(nLightR * triTransformed.col[1].r), uint8_t(nLightG * triTransformed.col[1].g), uint8_t(nLightB * triTransformed.col[1].b));
			//triTransformed.col[2] = olc::Pixel(uint8_t(nLightR * triTransformed.col[2].r), uint8_t(nLightG * triTransformed.col[2].g), uint8_t(nLightB * triTransformed.col[2].b));

			//GFX3D::vec3d light_dir = { 1,1,1 };
			//light_dir = GFX3D::Math::Vec_Normalise(light_dir);
			//float light = GFX3D::Math::Vec_DotProduct(light_dir, normal);
			//if (light < 0) light = 0;
			//triTransformed.col[0] = olc::Pixel(light * 255.0f, light * 255.0f, light * 255.0f);
			//triTransformed.col[1] = olc::Pixel(light * 255.0f, light * 255.0f, light * 255.0f);
			//triTransformed.col[2] = olc::Pixel(light * 255.0f, light * 255.0f, light * 255.0f);				
		}

		// Clip triangle against near plane
		int nClippedTriangles = 0;
		LL3D_Triangle clipped[2];
		nClippedTriangles = Triangle_ClipAgainstPlane(TriClipA, TriClipB, triTransformed, clipped[0], clipped[1]);
		
		// This may yield two new triangles
		for (int n = 0; n < nClippedTriangles; n++)
		{
			LL3D_Triangle triProjected = clipped[n];

			// Project new triangle
			triProjected.p[0] = Matrix_MultiplyVector(ProjectionCamera->matProj, clipped[n].p[0]);
			triProjected.p[1] = Matrix_MultiplyVector(ProjectionCamera->matProj, clipped[n].p[1]);
			triProjected.p[2] = Matrix_MultiplyVector(ProjectionCamera->matProj, clipped[n].p[2]);

			// Apply Projection to Verts
			triProjected.p[0].x = triProjected.p[0].x / triProjected.p[0].w;
			triProjected.p[1].x = triProjected.p[1].x / triProjected.p[1].w;
			triProjected.p[2].x = triProjected.p[2].x / triProjected.p[2].w;

			triProjected.p[0].y = triProjected.p[0].y / triProjected.p[0].w;
			triProjected.p[1].y = triProjected.p[1].y / triProjected.p[1].w;
			triProjected.p[2].y = triProjected.p[2].y / triProjected.p[2].w;

			triProjected.p[0].z = triProjected.p[0].z / triProjected.p[0].w;
			triProjected.p[1].z = triProjected.p[1].z / triProjected.p[1].w;
			triProjected.p[2].z = triProjected.p[2].z / triProjected.p[2].w;

			// Apply Projection to Tex coords
			triProjected.t[0].x = triProjected.t[0].x / triProjected.p[0].w;
			triProjected.t[1].x = triProjected.t[1].x / triProjected.p[1].w;
			triProjected.t[2].x = triProjected.t[2].x / triProjected.p[2].w;

			triProjected.t[0].y = triProjected.t[0].y / triProjected.p[0].w;
			triProjected.t[1].y = triProjected.t[1].y / triProjected.p[1].w;
			triProjected.t[2].y = triProjected.t[2].y / triProjected.p[2].w;

			triProjected.t[0].z = 1.0f / triProjected.p[0].w;
			triProjected.t[1].z = 1.0f / triProjected.p[1].w;
			triProjected.t[2].z = 1.0f / triProjected.p[2].w;

			// Clip against viewport in screen space
			// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to 
			//  ensure we only test new triangles generated against planes
			LL3D_Triangle sclipped[2];
			std::list<LL3D_Triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triProjected);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					LL3D_Triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clip it against a plane. We only need to test each 
					// subsequent plane, against subsequent new triangles
					// as all triangles after a plane clip are guaranteed
					// to lie on the inside of the plane. I like how this
					// comment is almost completely and utterly justified
					
					switch (p)
					{
						case 0:	nTrisToAdd = Triangle_ClipAgainstPlane(TriRendClip[0],TriRendClip[1], test, sclipped[0], sclipped[1]); break;
						case 1:	nTrisToAdd = Triangle_ClipAgainstPlane(TriRendClip[2],TriRendClip[3], test, sclipped[0], sclipped[1]); break;
						case 2:	nTrisToAdd = Triangle_ClipAgainstPlane(TriRendClip[4],TriRendClip[5], test, sclipped[0], sclipped[1]); break;
						case 3:	nTrisToAdd = Triangle_ClipAgainstPlane(TriRendClip[6],TriRendClip[7], test, sclipped[0], sclipped[1]); break;
					}

					// Clipping may yield a variable number of triangles, so
					// add these new ones to the back of the queue for subsequent
					// clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(sclipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}
			while(listTriangles.size())
			{
				LL3D_Triangle &triRaster = listTriangles.front();
				/*
				if(LL3D_RenderingType==LL3D_REND_SOLID||!triRaster.tex){
					if(triRaster.attr.tri_color.red == triRaster.attr.tri_color.green &&
					triRaster.attr.tri_color.green == triRaster.attr.tri_color.blue &&
					triRaster.attr.tri_color.blue == 0){
						listTriangles.pop_front();
						continue;
					}
				}*/
				//for (auto &triRaster : listTriangles)
				// Scale to viewport
				triRaster.p[0].x *= -1.0f;
				triRaster.p[1].x *= -1.0f;
				triRaster.p[2].x *= -1.0f;
				triRaster.p[0].y *= -1.0f;
				triRaster.p[1].y *= -1.0f;
				triRaster.p[2].y *= -1.0f;
				LL3D_Vector3D vOffsetView = { 1,1,0 };
				triRaster.p[0] = Vector_Add(triRaster.p[0], vOffsetView);
				triRaster.p[1] = Vector_Add(triRaster.p[1], vOffsetView);
				triRaster.p[2] = Vector_Add(triRaster.p[2], vOffsetView);
				triRaster.p[0].x *= LL3D_PixelBuffer->half_width;
				triRaster.p[0].y *= LL3D_PixelBuffer->half_height;
				triRaster.p[1].x *= LL3D_PixelBuffer->half_width;
				triRaster.p[1].y *= LL3D_PixelBuffer->half_height;
				triRaster.p[2].x *= LL3D_PixelBuffer->half_width;
				triRaster.p[2].y *= LL3D_PixelBuffer->half_height;
				//vOffsetView = { fViewX,fViewY,0 };
				//triRaster.p[0] = Math::Vec_Add(triRaster.p[0], vOffsetView);
				//triRaster.p[1] = Math::Vec_Add(triRaster.p[1], vOffsetView);
				//triRaster.p[2] = Math::Vec_Add(triRaster.p[2], vOffsetView);

				int Rendcol = 0;
				MK2D_RGB lightedCol;

				if(LL3D_RenderingType==LL3D_REND_SOLID||!triRaster.tex){
					lightedCol = MK2D_MaxRGB(triRaster.attr.tri_color,triRaster.attr.light_col);//MK2D_ModulateRGB(t.attr.tri_color,t.attr.light_col);
					lightedCol.red *= triRaster.attr.light_lum;
					lightedCol.green *= triRaster.attr.light_lum;
					lightedCol.blue *= triRaster.attr.light_lum;
					Rendcol = MK2D_GetPalRGB(lightedCol);
				}else{
					lightedCol.red = triRaster.attr.light_lum;
					lightedCol.green = triRaster.attr.light_lum;
					lightedCol.blue = triRaster.attr.light_lum;
				}
				if(LL3D_RenderingType==LL3D_REND_TEX&&triRaster.tex){
					if(LL3D_RasteringType==LL3D_RAST_SOLID){
						LL3D_TexturedTriangle(
							(int)triRaster.p[0].x,(int)triRaster.p[0].y, triRaster.t[0].x, triRaster.t[0].y, triRaster.t[0].z, 
							(int)triRaster.p[1].x,(int)triRaster.p[1].y, triRaster.t[1].x, triRaster.t[1].y, triRaster.t[1].z,
							(int)triRaster.p[2].x,(int)triRaster.p[2].y, triRaster.t[2].x, triRaster.t[2].y, triRaster.t[2].z,
							triRaster.tex,lightedCol);
					}else{
						
						LL3D_TexturedTriangleChunky(
							(int)triRaster.p[0].x,(int)triRaster.p[0].y, triRaster.t[0].x, triRaster.t[0].y, triRaster.t[0].z, 
							(int)triRaster.p[1].x,(int)triRaster.p[1].y, triRaster.t[1].x, triRaster.t[1].y, triRaster.t[1].z,
							(int)triRaster.p[2].x,(int)triRaster.p[2].y, triRaster.t[2].x, triRaster.t[2].y, triRaster.t[2].z,
							triRaster.tex,lightedCol);
					}
				}
				if(LL3D_RenderingType==LL3D_REND_SOLID||
					(LL3D_RenderingType==LL3D_REND_TEX&&!triRaster.tex)){
					LL3D_FillDepthTriangle( (int)triRaster.p[0].x, (int)triRaster.p[0].y, triRaster.t[0].z, 
										    (int)triRaster.p[1].x, (int)triRaster.p[1].y, triRaster.t[1].z, 
											(int)triRaster.p[2].x, (int)triRaster.p[2].y, triRaster.t[2].z, 
											Rendcol);
				}
				else if(LL3D_RenderingType==LL3D_REND_SOLID_NDEPTH){
					LL3D_FillTriangle(
									(int)triRaster.p[0].x, (int)triRaster.p[0].y, 
									(int)triRaster.p[1].x, (int)triRaster.p[1].y, 
									(int)triRaster.p[2].x, (int)triRaster.p[2].y, 
									Rendcol);
				}else if(LL3D_RenderingType==LL3D_REND_WIRE){
					LL3D_DrawTriangle(
									(int)triRaster.p[0].x, (int)triRaster.p[0].y, 
									(int)triRaster.p[1].x, (int)triRaster.p[1].y, 
									(int)triRaster.p[2].x, (int)triRaster.p[2].y, 
									Rendcol);
				}
				listTriangles.pop_front();

				LL_TriangleDrawCount++;
			}
		}
	}

	return LL_TriangleDrawCount;
};


/*
TODO:
Optimize the rendering with the following


	uint32_t GFX3D::PipeLine::Render(std::vector<olc::GFX3D::triangle> &triangles, uint32_t flags, int nOffset, int nCount)
	{
		// Calculate Transformation Matrix
		mat4x4 matWorldView = Math::Mat_MultiplyMatrix(matWorld, matView);
		//matWorldViewProj = Math::Mat_MultiplyMatrix(matWorldView, matProj);

		// Store triangles for rastering later
		std::vector<GFX3D::triangle> vecTrianglesToRaster;

		int nTriangleDrawnCount = 0;

		// Process Triangles
		//for (auto &tri : triangles)
//		omp_set_dynamic(0);
//		omp_set_num_threads(4);
//#pragma omp parallel for schedule(static)
		for(int tx = nOffset; tx < nOffset+nCount; tx++)
		{
			GFX3D::triangle &tri = triangles[tx];
			GFX3D::triangle triTransformed;

			// Just copy through texture coordinates
			triTransformed.t[0] = { tri.t[0].x, tri.t[0].y, tri.t[0].z };
			triTransformed.t[1] = { tri.t[1].x, tri.t[1].y, tri.t[1].z };
			triTransformed.t[2] = { tri.t[2].x, tri.t[2].y, tri.t[2].z }; // Think!

			// Dont forget vertex colours
			triTransformed.col[0] = tri.col[0];
			triTransformed.col[1] = tri.col[1];
			triTransformed.col[2] = tri.col[2];

			// Transform Triangle from object into projected space
			triTransformed.p[0] = GFX3D::Math::Mat_MultiplyVector(matWorldView, tri.p[0]);
			triTransformed.p[1] = GFX3D::Math::Mat_MultiplyVector(matWorldView, tri.p[1]);
			triTransformed.p[2] = GFX3D::Math::Mat_MultiplyVector(matWorldView, tri.p[2]);

			// Calculate Triangle Normal in WorldView Space
			GFX3D::vec3d normal, line1, line2;
			line1 = GFX3D::Math::Vec_Sub(triTransformed.p[1], triTransformed.p[0]);
			line2 = GFX3D::Math::Vec_Sub(triTransformed.p[2], triTransformed.p[0]);
			normal = GFX3D::Math::Vec_CrossProduct(line1, line2);
			normal = GFX3D::Math::Vec_Normalise(normal);

			// Cull triangles that face away from viewer
			if (flags & RENDER_CULL_CW && GFX3D::Math::Vec_DotProduct(normal, triTransformed.p[0]) > 0.0f) continue;
			if (flags & RENDER_CULL_CCW && GFX3D::Math::Vec_DotProduct(normal, triTransformed.p[0]) < 0.0f) continue;
					
			// If Lighting, calculate shading
			if (flags & RENDER_LIGHTS)
			{
				olc::Pixel ambient_clamp = { 0,0,0 };
				olc::Pixel light_combined = { 0,0,0 };
				uint32_t nLightSources = 0;
				float nLightR = 0, nLightG = 0, nLightB = 0;

				for (int i = 0; i < 4; i++)
				{
					switch (lights[i].type)
					{
					case LIGHT_DISABLED:
						break;
					case LIGHT_AMBIENT:
						ambient_clamp = lights[i].col;						
						break;
					case LIGHT_DIRECTIONAL:
						{
							nLightSources++;
							GFX3D::vec3d light_dir = GFX3D::Math::Vec_Normalise(lights[i].dir);
							float light = GFX3D::Math::Vec_DotProduct(light_dir, normal);
							if (light > 0)
							{
								int j = 0;
							}

							light = std::max(light, 0.0f);
							nLightR += light * (lights[i].col.r/255.0f);
							nLightG += light * (lights[i].col.g/255.0f);
							nLightB += light * (lights[i].col.b/255.0f);
						}
						break;
					case LIGHT_POINT:
						break;
					}
				}

				//nLightR /= nLightSources;
				//nLightG /= nLightSources;
				//nLightB /= nLightSources;

				nLightR = std::max(nLightR, ambient_clamp.r / 255.0f);
				nLightG = std::max(nLightG, ambient_clamp.g / 255.0f);
				nLightB = std::max(nLightB, ambient_clamp.b / 255.0f);

				triTransformed.col[0] = olc::Pixel(uint8_t(nLightR * triTransformed.col[0].r), uint8_t(nLightG * triTransformed.col[0].g), uint8_t(nLightB * triTransformed.col[0].b));
				triTransformed.col[1] = olc::Pixel(uint8_t(nLightR * triTransformed.col[1].r), uint8_t(nLightG * triTransformed.col[1].g), uint8_t(nLightB * triTransformed.col[1].b));
				triTransformed.col[2] = olc::Pixel(uint8_t(nLightR * triTransformed.col[2].r), uint8_t(nLightG * triTransformed.col[2].g), uint8_t(nLightB * triTransformed.col[2].b));



				//GFX3D::vec3d light_dir = { 1,1,1 };
				//light_dir = GFX3D::Math::Vec_Normalise(light_dir);
				//float light = GFX3D::Math::Vec_DotProduct(light_dir, normal);
				//if (light < 0) light = 0;
				//triTransformed.col[0] = olc::Pixel(light * 255.0f, light * 255.0f, light * 255.0f);
				//triTransformed.col[1] = olc::Pixel(light * 255.0f, light * 255.0f, light * 255.0f);
				//triTransformed.col[2] = olc::Pixel(light * 255.0f, light * 255.0f, light * 255.0f);
			}
			//else
			//	triTransformed.col = olc::WHITE;

			// Clip triangle against near plane
			int nClippedTriangles = 0;
			GFX3D::triangle clipped[2];
			nClippedTriangles = GFX3D::Math::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triTransformed, clipped[0], clipped[1]);
			
			// This may yield two new triangles
			for (int n = 0; n < nClippedTriangles; n++)
			{
				triangle triProjected = clipped[n];

				// Project new triangle
				triProjected.p[0] = GFX3D::Math::Mat_MultiplyVector(matProj, clipped[n].p[0]);
				triProjected.p[1] = GFX3D::Math::Mat_MultiplyVector(matProj, clipped[n].p[1]);
				triProjected.p[2] = GFX3D::Math::Mat_MultiplyVector(matProj, clipped[n].p[2]);

				// Apply Projection to Verts
				triProjected.p[0].x = triProjected.p[0].x / triProjected.p[0].w;
				triProjected.p[1].x = triProjected.p[1].x / triProjected.p[1].w;
				triProjected.p[2].x = triProjected.p[2].x / triProjected.p[2].w;

				triProjected.p[0].y = triProjected.p[0].y / triProjected.p[0].w;
				triProjected.p[1].y = triProjected.p[1].y / triProjected.p[1].w;
				triProjected.p[2].y = triProjected.p[2].y / triProjected.p[2].w;

				triProjected.p[0].z = triProjected.p[0].z / triProjected.p[0].w;
				triProjected.p[1].z = triProjected.p[1].z / triProjected.p[1].w;
				triProjected.p[2].z = triProjected.p[2].z / triProjected.p[2].w;

				// Apply Projection to Tex coords
				triProjected.t[0].x = triProjected.t[0].x / triProjected.p[0].w;
				triProjected.t[1].x = triProjected.t[1].x / triProjected.p[1].w;
				triProjected.t[2].x = triProjected.t[2].x / triProjected.p[2].w;

				triProjected.t[0].y = triProjected.t[0].y / triProjected.p[0].w;
				triProjected.t[1].y = triProjected.t[1].y / triProjected.p[1].w;
				triProjected.t[2].y = triProjected.t[2].y / triProjected.p[2].w;

				triProjected.t[0].z = 1.0f / triProjected.p[0].w;
				triProjected.t[1].z = 1.0f / triProjected.p[1].w;
				triProjected.t[2].z = 1.0f / triProjected.p[2].w;

				// Clip against viewport in screen space
				// Clip triangles against all four screen edges, this could yield
				// a bunch of triangles, so create a queue that we traverse to 
				//  ensure we only test new triangles generated against planes
				GFX3D::triangle sclipped[2];
				std::list<GFX3D::triangle> listTriangles;
				

				// Add initial triangle
				listTriangles.push_back(triProjected);
				int nNewTriangles = 1;

				for (int p = 0; p < 4; p++)
				{
					int nTrisToAdd = 0;
					while (nNewTriangles > 0)
					{
						// Take triangle from front of queue
						triangle test = listTriangles.front();
						listTriangles.pop_front();
						nNewTriangles--;

						// Clip it against a plane. We only need to test each 
						// subsequent plane, against subsequent new triangles
						// as all triangles after a plane clip are guaranteed
						// to lie on the inside of the plane. I like how this
						// comment is almost completely and utterly justified
						switch (p)
						{
						case 0:	nTrisToAdd = GFX3D::Math::Triangle_ClipAgainstPlane({ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, sclipped[0], sclipped[1]); break;
						case 1:	nTrisToAdd = GFX3D::Math::Triangle_ClipAgainstPlane({ 0.0f, +1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, sclipped[0], sclipped[1]); break;
						case 2:	nTrisToAdd = GFX3D::Math::Triangle_ClipAgainstPlane({ -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, sclipped[0], sclipped[1]); break;
						case 3:	nTrisToAdd = GFX3D::Math::Triangle_ClipAgainstPlane({ +1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, sclipped[0], sclipped[1]); break;
						}


						// Clipping may yield a variable number of triangles, so
						// add these new ones to the back of the queue for subsequent
						// clipping against next planes
						for (int w = 0; w < nTrisToAdd; w++)
							listTriangles.push_back(sclipped[w]);
					}
					nNewTriangles = listTriangles.size();
				}

				for (auto &triRaster : listTriangles)
				{
					// Scale to viewport
					//triRaster.p[0].x *= -1.0f;
					//triRaster.p[1].x *= -1.0f;
					//triRaster.p[2].x *= -1.0f;
					//triRaster.p[0].y *= -1.0f;
					//triRaster.p[1].y *= -1.0f;
					//triRaster.p[2].y *= -1.0f;
					vec3d vOffsetView = { 1,1,0 };
					triRaster.p[0] = Math::Vec_Add(triRaster.p[0], vOffsetView);
					triRaster.p[1] = Math::Vec_Add(triRaster.p[1], vOffsetView);
					triRaster.p[2] = Math::Vec_Add(triRaster.p[2], vOffsetView);
					triRaster.p[0].x *= 0.5f * fViewW;
					triRaster.p[0].y *= 0.5f * fViewH;
					triRaster.p[1].x *= 0.5f * fViewW;
					triRaster.p[1].y *= 0.5f * fViewH;
					triRaster.p[2].x *= 0.5f * fViewW;
					triRaster.p[2].y *= 0.5f * fViewH;
					vOffsetView = { fViewX,fViewY,0 };
					triRaster.p[0] = Math::Vec_Add(triRaster.p[0], vOffsetView);
					triRaster.p[1] = Math::Vec_Add(triRaster.p[1], vOffsetView);
					triRaster.p[2] = Math::Vec_Add(triRaster.p[2], vOffsetView);

					// For now, just draw triangle

					//if (flags & RENDER_TEXTURED)
					//{
					//	TexturedTriangle(
					//		triRaster.p[0].x, triRaster.p[0].y, triRaster.t[0].x, triRaster.t[0].y, triRaster.t[0].z,
					//		triRaster.p[1].x, triRaster.p[1].y, triRaster.t[1].x, triRaster.t[1].y, triRaster.t[1].z,
					//		triRaster.p[2].x, triRaster.p[2].y, triRaster.t[2].x, triRaster.t[2].y, triRaster.t[2].z,
					//		sprTexture);

					//	RasterTriangle(
					//		triRaster.p[0].x, triRaster.p[0].y, triRaster.t[0].x, triRaster.t[0].y, triRaster.t[0].z, triRaster.col,
					//		triRaster.p[1].x, triRaster.p[1].y, triRaster.t[1].x, triRaster.t[1].y, triRaster.t[1].z, triRaster.col,
					//		triRaster.p[2].x, triRaster.p[2].y, triRaster.t[2].x, triRaster.t[2].y, triRaster.t[2].z, triRaster.col,
					//		sprTexture, nFlags);

					//}

					if (flags & RENDER_WIRE)
					{
						DrawTriangleWire(triRaster, olc::RED);
					}
					else
					{
						RasterTriangle(
							(int)triRaster.p[0].x,(int)triRaster.p[0].y, triRaster.t[0].x, triRaster.t[0].y, triRaster.t[0].z, triRaster.col[0],
							(int)triRaster.p[1].x,(int)triRaster.p[1].y, triRaster.t[1].x, triRaster.t[1].y, triRaster.t[1].z, triRaster.col[1],
							(int)triRaster.p[2].x,(int)triRaster.p[2].y, triRaster.t[2].x, triRaster.t[2].y, triRaster.t[2].z, triRaster.col[2],
							sprTexture, flags);
						
					}




					nTriangleDrawnCount++;
				}
			}
		}

		return nTriangleDrawnCount;
	}

*/

bool LL3D_IsError(){
	return LL3D_IsThereError;
};

void LL3D_ThrowError(std::string errorstr){
	LL3D_IsThereError = true;
	ErrorString = errorstr;
};

void LL3D_PrintExitCode(){
	if(LL3D_IsThereError){
		std::cout << "Runtime Error: " << ErrorString.c_str() << std::endl;
	}
	else{
		std::cout << "Thanks for using Llama3D!" << std::endl;
	}
};
