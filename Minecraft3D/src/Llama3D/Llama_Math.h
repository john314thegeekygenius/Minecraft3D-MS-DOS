// Llama 3D Maths Header

#ifndef __LLAMA_MATH__
#define __LLAMA_MATH__

#include <math.h>

#define LL3D_PI  (ll3d_float32)3.141592654
#define LL3D_2PI (ll3d_float32)6.283185308
#define LL3D_Rad2DEG (ll3d_float32)114.591559011

bool LL3D_SetupMath(void);
void LL3D_FreeMath(void);

ll3d_int32 LL3D_RandomInt();

#ifdef LLAMA_SINTABLE
ll3d_float32 LL3D_Sin(ll3d_float32 x);
ll3d_float32 LL3D_Cos(ll3d_float32 x);
#else
#define LL3D_Sin(x) (ll3d_float32)sin(x)
#define LL3D_Cos(x) (ll3d_float32)cos(x)
#endif
#define LL3D_Tan(x) (ll3d_float32)tan(x)
#define LL3D_Sqrt(x) (ll3d_float32)sqrt(x)
#define LL3D_Min(a,b) (((a)<(b))?(a):(b))
#define LL3D_Max(a,b) (((a)>(b))?(a):(b))
#define LL3D_ABS(a) (((a)>=0.0f)?(a):-(a))





/******************************************************************************
 * 
 * 
 *        Matrix and Vector Stuff
 * 
 * 
 * ****************************************************************************/

LL3D_Vector3D Matrix_MultiplyVector(LL3D_Matrix4x4 &m, LL3D_Vector3D &i);
LL3D_Matrix4x4 Matrix_MakeIdentity();
LL3D_Matrix4x4 Matrix_MakeRotationX(float fAngleRad);
LL3D_Matrix4x4 Matrix_MakeRotationY(float fAngleRad);
LL3D_Matrix4x4 Matrix_MakeRotationZ(float fAngleRad);
LL3D_Matrix4x4 Matrix_MakeTranslation(float x, float y, float z);
LL3D_Matrix4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
LL3D_Matrix4x4 Matrix_MultiplyMatrix(LL3D_Matrix4x4 &m1, LL3D_Matrix4x4 &m2);
LL3D_Matrix4x4 Matrix_PointAt(LL3D_Vector3D &pos, LL3D_Vector3D &target, LL3D_Vector3D &up);
LL3D_Matrix4x4 Matrix_QuickInverse(LL3D_Matrix4x4 &m); // Only for Rotation/Translation Matrices
LL3D_Vector3D Vector_Add(LL3D_Vector3D &v1, LL3D_Vector3D &v2);
LL3D_Vector3D Vector_Sub(LL3D_Vector3D &v1, LL3D_Vector3D &v2);
LL3D_Vector3D Vector_Mul(LL3D_Vector3D &v1, float k);
LL3D_Vector3D Vector_Div(LL3D_Vector3D &v1, float k);
float Vector_DotProduct(LL3D_Vector3D &v1, LL3D_Vector3D &v2);
float Vector_Length(LL3D_Vector3D &v);
LL3D_Vector3D Vector_Normalise(LL3D_Vector3D &v);
LL3D_Vector3D Vector_CrossProduct(LL3D_Vector3D &v1, LL3D_Vector3D &v2);
LL3D_Vector3D Vector_IntersectPlane(LL3D_Vector3D &plane_p, LL3D_Vector3D &plane_n, LL3D_Vector3D &lineStart, LL3D_Vector3D &lineEnd, float &t);








#endif

