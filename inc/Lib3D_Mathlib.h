#ifndef LIB3D_MATH_H
#define LIB3D_MATH_H

#include "../Container/Vector.h"
#include "Vector2.h"

typedef struct vec2d{
	float u;
	float v;
	float w;
} vec2d;

typedef struct vec3d{
	float x;
	float y;
	float z;
	float w;
} vec3d;

vec3d vec3d_New(float x,float y,float z){
	return (vec3d){ x,y,z,1.0f };
}
vec3d vec3d_Null(){
	return (vec3d){ 0.0f,0.0f,0.0f,1.0f };
}
vec3d vec3d_Add(vec3d v1, vec3d v2){
	return vec3d_New( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}
vec3d vec3d_Sub(vec3d v1, vec3d v2){
	return vec3d_New( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}
vec3d vec3d_Mul(vec3d v1, float k){
	return vec3d_New( v1.x * k, v1.y * k, v1.z * k );
}
vec3d vec3d_Div(vec3d v1, float k){
	if(k==0.0f) return vec3d_New( 0.0f,0.0f,0.0f );
	return vec3d_New( v1.x / k, v1.y / k, v1.z / k );
}
float vec3d_DotProduct(vec3d v1, vec3d v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
float vec3d_Length(vec3d v){
	return sqrtf(vec3d_DotProduct(v, v));
}
vec3d vec3d_Normalise(vec3d v){
	float l = vec3d_Length(v);
	return vec3d_New( v.x / l, v.y / l, v.z / l );
}
vec3d vec3d_Perp(vec3d v){
	return vec3d_New( -v.z, v.y, v.x );
}
vec3d vec3d_Round(vec3d v){
	return vec3d_New( roundf(v.x),roundf(v.y),roundf(v.z) );
}
vec3d vec3d_CrossProduct(vec3d v1, vec3d v2){
	vec3d v = vec3d_New(0.0f,0.0f,0.0f);
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}
vec3d vec3d_IntersectPlane(vec3d plane_p, vec3d* plane_n, vec3d lineStart, vec3d lineEnd){
	*plane_n = vec3d_Normalise(*plane_n);
	float plane_d = -vec3d_DotProduct(*plane_n, plane_p);
	float ad = vec3d_DotProduct(lineStart, *plane_n);
	float bd = vec3d_DotProduct(lineEnd, *plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	vec3d lineStartToEnd = vec3d_Sub(lineEnd, lineStart);
	vec3d lineToIntersect = vec3d_Mul(lineStartToEnd, t);
	return vec3d_Add(lineStart, lineToIntersect);
}
float vec3d_Dist(vec3d plane_p, vec3d plane_n,vec3d p){
	//vec3d n = vec3d_Normalise(p);
	return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - vec3d_DotProduct(plane_n, plane_p));
}


typedef struct triangle{
	vec3d p[3];
	vec3d n;
	unsigned int c;
} triangle;

void triangle_CalcNorm(triangle* t){
	vec3d line1 = vec3d_Sub(t->p[1],t->p[0]);
	vec3d line2 = vec3d_Sub(t->p[2],t->p[0]);
	vec3d normal = vec3d_CrossProduct(line1,line2);

	t->n = vec3d_Normalise(normal);
}
void triangle_ShadeNorm(triangle* t,vec3d dirLight){
	float l = F32_Min(F32_Max(0.2f,vec3d_DotProduct(t->n,dirLight)),1.0f);
	t->c = Pixel_Mulf(t->c,l);
}
void triangle_Scale(triangle* t,float s){
	t->p[0] = vec3d_Mul(t->p[0],s);
	t->p[1] = vec3d_Mul(t->p[1],s);
	t->p[2] = vec3d_Mul(t->p[2],s);
}
void triangle_Offset(triangle* t,vec3d offset){
	t->p[0] = vec3d_Add(t->p[0],offset);
	t->p[1] = vec3d_Add(t->p[1],offset);
	t->p[2] = vec3d_Add(t->p[2],offset);
}
int triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle in_tri, triangle* out_tri1, triangle* out_tri2){
	// Make sure plane normal is indeed normal
	plane_n = vec3d_Normalise(plane_n);
	
	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vec3d* inCUBE_SIDE_points[3];  int nInsidePointCount = 0;
	vec3d* outCUBE_SIDE_points[3]; int nOutsidePointCount = 0;
	// Get signed distance of each point in triangle to plane
	float d0 = vec3d_Dist(plane_p,plane_n,in_tri.p[0]);
	float d1 = vec3d_Dist(plane_p,plane_n,in_tri.p[1]);
	float d2 = vec3d_Dist(plane_p,plane_n,in_tri.p[2]);
	if (d0 >= 0) { inCUBE_SIDE_points[nInsidePointCount++] = &in_tri.p[0]; }
	else { outCUBE_SIDE_points[nOutsidePointCount++] = &in_tri.p[0]; }
	if (d1 >= 0) { inCUBE_SIDE_points[nInsidePointCount++] = &in_tri.p[1]; }
	else { outCUBE_SIDE_points[nOutsidePointCount++] = &in_tri.p[1]; }
	if (d2 >= 0) { inCUBE_SIDE_points[nInsidePointCount++] = &in_tri.p[2]; }
	else { outCUBE_SIDE_points[nOutsidePointCount++] = &in_tri.p[2]; }
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
		*out_tri1 = in_tri;
		return 1; // Just the one returned original triangle is valid
	}
	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle
		// Copy appearance info to new triangle
		out_tri1->c =  in_tri.c;
		// The inside point is valid, so keep that...
		out_tri1->p[0] = *inCUBE_SIDE_points[0];
		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		out_tri1->p[1] = vec3d_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[0], *outCUBE_SIDE_points[0]);
		out_tri1->p[2] = vec3d_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[0], *outCUBE_SIDE_points[1]);
		return 1; // Return the newly formed single triangle
	}
	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles
		// Copy appearance info to new triangles
		out_tri1->c =  in_tri.c;
		out_tri2->c =  in_tri.c;
		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1->p[0] = *inCUBE_SIDE_points[0];
		out_tri1->p[1] = *inCUBE_SIDE_points[1];
		out_tri1->p[2] = vec3d_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[0], *outCUBE_SIDE_points[0]);
		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2->p[0] = *inCUBE_SIDE_points[1];
		out_tri2->p[1] = out_tri1->p[2];
		out_tri2->p[2] = vec3d_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[1], *outCUBE_SIDE_points[0]);
		return 2; // Return two newly formed triangles which form a quad
	}
	return -1;
}


typedef struct mat4x4{
	float m[4][4];
} mat4x4;

mat4x4 Matrix_Null(){
	return (mat4x4){{
		{ 0.0f,0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f,0.0f }
	}};
}
vec3d Matrix_MultiplyVector(mat4x4 m, vec3d i){
	vec3d v;
	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
	return v;
}
mat4x4 Matrix_MultiplyMatrix(mat4x4 m1, mat4x4 m2){
	mat4x4 matrix = Matrix_Null();
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}
mat4x4 Matrix_PointAt(vec3d pos, vec3d target, vec3d up){
	vec3d newForward = vec3d_Sub(target, pos);
	newForward = vec3d_Normalise(newForward);
	
	vec3d a = vec3d_Mul(newForward,vec3d_DotProduct(up,newForward));
	vec3d newUp = vec3d_Sub(up, a);
	newUp = vec3d_Normalise(newUp);

	vec3d newRight = vec3d_CrossProduct(newUp,newForward);

	return (mat4x4){{
		{ newRight.x,	newRight.y,		newRight.z,		0.0f },
		{ newUp.x,		newUp.y,		newUp.z,		0.0f },
		{ newForward.x,	newForward.y,	newForward.z,	0.0f },
		{ pos.x,		pos.y,			pos.z,			1.0f }
	}};
}
mat4x4 Matrix_QuickInverse(mat4x4 m){
	mat4x4 matrix = Matrix_Null();
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}
mat4x4 Matrix_MakeView(vec3d pos,vec3d target,vec3d up){
	vec3d newForward = vec3d_Normalise(vec3d_Sub(target, pos));
	vec3d a = vec3d_Mul(newForward,vec3d_DotProduct(up,newForward));
	vec3d newUp = vec3d_Normalise(vec3d_Sub(up, a));
	vec3d newRight = vec3d_CrossProduct(newUp,newForward);

	return (mat4x4){{
		{ newRight.x,						newUp.x,						newForward.x,						0.0f },
		{ newRight.y,						newUp.y,						newForward.y,						0.0f },
		{ newRight.z,						newUp.z,						newForward.z,						0.0f },
		{ -vec3d_DotProduct(newRight,pos),	-vec3d_DotProduct(newUp,pos),	-vec3d_DotProduct(newForward,pos),	1.0f }
	}};
}

mat4x4 Matrix_MakeIdentity(){
	return (mat4x4){{
		{ 1.0f,0.0f,0.0f,0.0f },
		{ 0.0f,1.0f,0.0f,0.0f },
		{ 0.0f,0.0f,1.0f,0.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	}};
}
mat4x4 Matrix_MakeRotationX(float fAngleRad){
	return (mat4x4){{
		{ 1.0f,0.0f,			0.0f,			0.0f },
		{ 0.0f,cosf(fAngleRad),	sinf(fAngleRad),0.0f },
		{ 0.0f,-sinf(fAngleRad),cosf(fAngleRad),0.0f },
		{ 0.0f,0.0f,			0.0f,			1.0f }
	}};
}
mat4x4 Matrix_MakeRotationY(float fAngleRad){
	return (mat4x4){{
		{ cosf(fAngleRad),	0.0f,sinf(fAngleRad),	0.0f },
		{ 0.0f,				1.0f,0.0f,				0.0f },
		{ -sinf(fAngleRad),	0.0f,cosf(fAngleRad),	0.0f },
		{ 0.0f,				0.0f,0.0f,				1.0f }
	}};
}
mat4x4 Matrix_MakeRotationZ(float fAngleRad){
	return (mat4x4){{
		{ cosf(fAngleRad),	sinf(fAngleRad),0.0f,0.0f },
		{ -sinf(fAngleRad),	cosf(fAngleRad),0.0f,0.0f },
		{ 0.0f,				0.0f,			1.0f,0.0f },
		{ 0.0f,				0.0f,			0.0f,1.0f }
	}};
}
mat4x4 Matrix_MakeTranslation(float x, float y, float z){
	return (mat4x4){{
		{ 1.0f,	0.0f,	0.0f,	0.0f },
		{ 0.0f,	1.0f,	0.0f,	0.0f },
		{ 0.0f,	0.0f,	1.0f,	0.0f },
		{ x,	y,		z,		1.0f }
	}};
}
mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar){
	const float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	return (mat4x4){{
		{ fAspectRatio * fFovRad,	0.0f,	0.0f,								0.0f },
		{ 0.0f,						fFovRad,0.0f,								0.0f },
		{ 0.0f,						0.0f,	fFar / (fFar - fNear),				1.0f },
		{ 0.0f,						0.0f,	(-fFar * fNear) / (fFar - fNear),	0.0f }
	}};
}
mat4x4 Matrix_MakeWorld(vec3d origin,vec3d angle){
	const mat4x4 matRotX = Matrix_MakeRotationX(angle.x);
	const mat4x4 matRotY = Matrix_MakeRotationY(angle.y);
	const mat4x4 matRotZ = Matrix_MakeRotationZ(angle.z);
	const mat4x4 matTrans = Matrix_MakeTranslation(origin.x,origin.y,origin.z);
	const mat4x4 matWorld1 = Matrix_MultiplyMatrix(matTrans,matRotY);
	const mat4x4 matWorld2 = Matrix_MultiplyMatrix(matRotX,matRotZ);
	const mat4x4 matWorld = Matrix_MultiplyMatrix(matWorld1,matWorld2);
	return matWorld;
}
mat4x4 Matrix_MakeWorldR(vec3d origin,vec3d angle){
	const mat4x4 matRotX = Matrix_MakeRotationX(angle.x);
	const mat4x4 matRotY = Matrix_MakeRotationY(angle.y);
	const mat4x4 matRotZ = Matrix_MakeRotationZ(angle.z);
	const mat4x4 matTrans = Matrix_MakeTranslation(origin.x,origin.y,origin.z);
	const mat4x4 matWorld1 = Matrix_MultiplyMatrix(matRotY,matRotZ);
	const mat4x4 matWorld2 = Matrix_MultiplyMatrix(matRotX,matTrans);
	const mat4x4 matWorld = Matrix_MultiplyMatrix(matWorld1,matWorld2);
	return matWorld;
}
mat4x4 Matrix_MakePerspektive(vec3d pos,vec3d up,vec3d a){
	const mat4x4 matCameraRotX = Matrix_MakeRotationX(a.x);
	const mat4x4 matCameraRotY = Matrix_MakeRotationY(a.y);
	const mat4x4 matCameraRotZ = Matrix_MakeRotationZ(a.z);
	
	vec3d lookdir = vec3d_New(0.0f,0.0f,1.0f);
	lookdir = Matrix_MultiplyVector(matCameraRotX,lookdir);
	lookdir = Matrix_MultiplyVector(matCameraRotY,lookdir);
	lookdir = Matrix_MultiplyVector(matCameraRotZ,lookdir);
	
	const vec3d target = vec3d_Add(pos,lookdir);
	//const mat4x4 matCamera = Matrix_PointAt(pos,target,up);
	//const mat4x4 matView = Matrix_QuickInverse(matCamera);
	const mat4x4 matView = Matrix_MakeView(pos,target,up);
	return matView;
}

typedef struct Camera {
	vec3d p;
	vec3d d;
	vec3d v;
	vec3d up;
	vec3d ld;
	vec3d sd;
	vec3d a;
	float fov;
} Camera;

Camera Camera_New(){
	Camera c;
	c.p = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	c.d = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	c.v = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	c.up = (vec3d){ 0.0f,1.0f,0.0f,1.0f };
	c.ld = (vec3d){ 0.0f,0.0f,1.0f,1.0f };
	c.ld = (vec3d){ 1.0f,0.0f,0.0f,1.0f };
	c.a = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	c.fov = 90;
	return c;
}
Camera Camera_Make(vec3d p,vec3d d,vec3d a,float fov){
	Camera c;
	c.p = p;
	c.d = d;
	c.v = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	c.up = (vec3d){ 0.0f,1.0f,0.0f,1.0f };
	c.ld = (vec3d){ 0.0f,0.0f,1.0f,1.0f };
	c.sd = (vec3d){ 1.0f,0.0f,0.0f,1.0f };
	c.a = a;
	c.fov = fov;
	return c;
}
void Camera_Update(Camera* c){
	if(c->a.x < -3.14159f * 0.5f + 0.01f) c->a.x = -3.14159f * 0.5f + 0.01f;
	if(c->a.x >  3.14159f * 0.5f - 0.01f) c->a.x =  3.14159f * 0.5f - 0.01f;
	
	mat4x4 matCameraRotY = Matrix_MakeRotationY(c->a.y);
	//mat4x4 matCameraRotX = Matrix_MakeRotationX(c->a.x);
	
	vec3d lookdir = vec3d_New(0.0f,0.0f,1.0f);
	vec3d sidedir = vec3d_New(1.0f,0.0f,0.0f);
	//lookdir = Matrix_MultiplyVector(matCameraRotX,lookdir);
	lookdir = Matrix_MultiplyVector(matCameraRotY,lookdir);
	//sidedir = Matrix_MultiplyVector(matCameraRotX,sidedir);
	sidedir = Matrix_MultiplyVector(matCameraRotY,sidedir);

	c->ld = lookdir;
	c->sd = sidedir;
}
void Camera_Focus(Camera* c,Vec2 before,Vec2 now,Vec2 screen){
	if(now.x!=before.x || now.y!=before.y){
		Vec2 d = Vec2_Sub(now,before);
		Vec2 a = Vec2_Mulf(Vec2_Div(d,(Vec2){ screen.x,screen.y }),8.0f * 3.141592654f);

		c->a.y += a.x;
		c->a.x += a.y;
	}
}

#endif