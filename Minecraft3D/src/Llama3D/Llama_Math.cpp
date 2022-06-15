
#include "Llama_3D.h"

#ifdef LLAMA_SINTABLE
// How precise the sine table is
const ll3d_int32 SinPrec = 100; // 100ths of the float

ll3d_float32 *LL3D_SinTable;
#endif

bool LL3D_SetupMath(void){
#ifdef LLAMA_SINTABLE
	LL3D_SinTable = (ll3d_float32*)malloc(360*SinPrec);
	if(!LL3D_SinTable){
		LL3D_ThrowError("Could not setup Sine Table!\n");
		return false;
	}
	// Set it up
	for(ll3d_uint32 i = 0; i < (ll3d_uint32)(360*SinPrec); i++){
		LL3D_SinTable[i] = (ll3d_float32)sin((ll3d_float32)i/(ll3d_float32)SinPrec);
	}
#endif

	return true;
};

void LL3D_FreeMath(void){
#ifdef LLAMA_SINTABLE
	if(LL3D_SinTable){
		free(LL3D_SinTable);
		LL3D_SinTable = NULL;
	}
#endif
};

#ifdef LLAMA_SINTABLE

ll3d_float32 LL3D_Sin(ll3d_float32 x){
	//if(!LL3D_SinTable) return sin(x);
	int o = (ll3d_float32)x * (ll3d_float32)SinPrec;
	o = LL3D_ABS(o);
	return LL3D_SinTable[o%(360*SinPrec)];
};

ll3d_float32 LL3D_Cos(ll3d_float32 x){
	//if(!LL3D_SinTable) return cos(x);
	int o = (ll3d_float32)x * (ll3d_float32)SinPrec;
	o = LL3D_ABS(o);
	o += 90*SinPrec;
	return LL3D_SinTable[o%(360*SinPrec)];
};

#endif

ll3d_int32 LL3D_RandomInt(){
	return rand();
};



/******************************************************************************
 * 
 * 
 *        Matrix and Vector Stuff
 * 
 * 
 * ****************************************************************************/


LL3D_Vector3D Matrix_MultiplyVector(LL3D_Matrix4x4 &m, LL3D_Vector3D &i)
{
	LL3D_Vector3D v;
	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
	return v;
}

LL3D_Matrix4x4 Matrix_MakeIdentity()
{
	LL3D_Matrix4x4 matrix;
	matrix.zero();
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

LL3D_Matrix4x4 Matrix_MakeRotationX(float fAngleRad)
{
	LL3D_Matrix4x4 matrix;
	matrix.zero();
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = LL3D_Cos(fAngleRad);
	matrix.m[1][2] = -LL3D_Sin(fAngleRad);
	matrix.m[2][1] = LL3D_Sin(fAngleRad);
	matrix.m[2][2] = LL3D_Cos(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

LL3D_Matrix4x4 Matrix_MakeRotationY(float fAngleRad)
{
	LL3D_Matrix4x4 matrix;
	matrix.zero();
	matrix.m[0][0] = LL3D_Cos(fAngleRad);
	matrix.m[0][2] = LL3D_Sin(fAngleRad);
	matrix.m[2][0] = -LL3D_Sin(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = LL3D_Cos(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

LL3D_Matrix4x4 Matrix_MakeRotationZ(float fAngleRad)
{
	LL3D_Matrix4x4 matrix;
	matrix.zero();
	matrix.m[0][0] = LL3D_Cos(fAngleRad);
	matrix.m[0][1] = -LL3D_Sin(fAngleRad);
	matrix.m[1][0] = LL3D_Sin(fAngleRad);
	matrix.m[1][1] = LL3D_Cos(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

LL3D_Matrix4x4 Matrix_MakeTranslation(float x, float y, float z)
{
	LL3D_Matrix4x4 matrix;
	matrix.zero();
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

LL3D_Matrix4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / LL3D_Tan(fFovDegrees * 0.5f / 180.0f * LL3D_PI);
	LL3D_Matrix4x4 matrix;
	matrix.zero();
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

LL3D_Matrix4x4 Matrix_MultiplyMatrix(LL3D_Matrix4x4 &m1, LL3D_Matrix4x4 &m2)
{
	LL3D_Matrix4x4 matrix;
//	for (int c = 0; c < 4; c++)
//		for (int r = 0; r < 4; r++)
//			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];

	matrix.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	matrix.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	matrix.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	matrix.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];

	matrix.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	matrix.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	matrix.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	matrix.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];

	matrix.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	matrix.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	matrix.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	matrix.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];

	matrix.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	matrix.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	matrix.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	matrix.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return matrix;
}

LL3D_Matrix4x4 Matrix_PointAt(LL3D_Vector3D &pos, LL3D_Vector3D &target, LL3D_Vector3D &up)
{
	// Calculate new forward direction
	LL3D_Vector3D newForward = Vector_Sub(target, pos);
	newForward = Vector_Normalise(newForward);

	// Calculate new Up direction
	LL3D_Vector3D a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
	LL3D_Vector3D newUp = Vector_Sub(up, a);
	newUp = Vector_Normalise(newUp);

	// New Right direction is easy, its just cross product
	LL3D_Vector3D newRight = Vector_CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix	
	LL3D_Matrix4x4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;

}

LL3D_Matrix4x4 Matrix_QuickInverse(LL3D_Matrix4x4 &m) // Only for Rotation/Translation Matrices
{
	LL3D_Matrix4x4 matrix;
	matrix.zero();
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

LL3D_Vector3D Vector_Add(LL3D_Vector3D &v1, LL3D_Vector3D &v2)
{
	LL3D_Vector3D o;
	o.x = v1.x + v2.x;
	o.y = v1.y + v2.y;
	o.z = v1.z + v2.z;
	o.w = 1.0f;
	return o;
}

LL3D_Vector3D Vector_Sub(LL3D_Vector3D &v1, LL3D_Vector3D &v2)
{
	LL3D_Vector3D o;
	o.x = v1.x - v2.x;
	o.y = v1.y - v2.y;
	o.z = v1.z - v2.z;
	o.w = 1.0f;
	return o;
}

LL3D_Vector3D Vector_Mul(LL3D_Vector3D &v1, float k)
{
	LL3D_Vector3D o;
	o.x = v1.x * k;
	o.y = v1.y * k;
	o.z = v1.z * k;
	o.w = 1.0f;
	return o;
}

LL3D_Vector3D Vector_Div(LL3D_Vector3D &v1, float k)
{
	LL3D_Vector3D o;
	o.x = v1.x / k;
	o.y = v1.y / k;
	o.z = v1.z / k;
	o.w = 1.0f;
	return o;
}

float Vector_DotProduct(LL3D_Vector3D &v1, LL3D_Vector3D &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}

float Vector_Length(LL3D_Vector3D &v)
{
	// Can probably remove the sqrt here
	return LL3D_Sqrt(Vector_DotProduct(v, v));
}

LL3D_Vector3D Vector_Normalise(LL3D_Vector3D &v)
{
	float l = Vector_Length(v);
	LL3D_Vector3D o;
	o.x = v.x / l;
	o.y = v.y / l;
	o.z = v.z / l;
	o.w = 1.0f;
	return o;
}

LL3D_Vector3D Vector_CrossProduct(LL3D_Vector3D &v1, LL3D_Vector3D &v2)
{
	LL3D_Vector3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	v.w = 1.0f;
	return v;
}

LL3D_Vector3D Vector_IntersectPlane(LL3D_Vector3D &plane_p, LL3D_Vector3D &plane_n, LL3D_Vector3D &lineStart, LL3D_Vector3D &lineEnd, float &t)
{
	plane_n = Vector_Normalise(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	t = (-plane_d - ad) / (bd - ad);
	LL3D_Vector3D lineStartToEnd = Vector_Sub(lineEnd, lineStart);
	LL3D_Vector3D lineToIntersect = Vector_Mul(lineStartToEnd, t);
	return Vector_Add(lineStart, lineToIntersect);
}


