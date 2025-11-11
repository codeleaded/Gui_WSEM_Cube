#ifndef LIB3D_WORLD3D_H
#define LIB3D_WORLD3D_H

#include "Files.h"
#include "ConstParser.h"
#include "Lib3D_Mathlib.h"


typedef Vector Mesh;// Vector<triangle>

Mesh Mesh_New(){
	return Vector_New(sizeof(triangle));
}
void Mesh_Shade(Mesh* m,vec3d dir){
	for(int i = 0;i<m->size;i++){
		triangle* t = (triangle*)Vector_Get(m,i);
		triangle_ShadeNorm(t,dir);
	}
}
void Mesh_Read(Mesh* m,char* Path){
	char c = Files_isFile(Path);
	if(!c){
		printf("[Mesh]: Read -> Error!\n");
		return;
	}

	Vector verts = Vector_New(sizeof(vec3d));

	FilesSize fs;
	char* Content = Files_ReadTB(Path,&fs);
	int start = 0;
	int end = 0;
	
	while(1){
		start = end;
		end = start + CStr_Find(Content + start,'\n') + 1;
		if(start==end || end<0) break;

		char* lineIn = CStr_ChopTo(Content + start,'\n');

		//char junk;
		if(lineIn[0] == 'v'){
			vec3d v = vec3d_Null();
			
			Vector vec = CStr_ChopDown(lineIn,' ');
			//junk = *(char*)Vector_Get(&vec,0);
			v.x = (float)Double_Parse((i8*)*(char**)Vector_Get(&vec,1),DOUBLE_SIGNED);
			v.y = (float)Double_Parse((i8*)*(char**)Vector_Get(&vec,2),DOUBLE_SIGNED);
			v.z = (float)Double_Parse((i8*)*(char**)Vector_Get(&vec,3),DOUBLE_SIGNED);
			
			for(int i = 0;i<vec.size;i++) free(*(char**)Vector_Get(&vec,i));
			Vector_Free(&vec);

			Vector_Push(&verts,&v);
		}else if(lineIn[0] == 'f'){
			int f[3];

			Vector vec = CStr_ChopDown(lineIn,' ');
			//junk = *(char*)Vector_Get(&vec,0);
			f[0] = (float)Number_Parse((i8*)*(char**)Vector_Get(&vec,1));
			f[1] = (float)Number_Parse((i8*)*(char**)Vector_Get(&vec,2));
			f[2] = (float)Number_Parse((i8*)*(char**)Vector_Get(&vec,3));
			for(int i = 0;i<vec.size;i++) free(*(char**)Vector_Get(&vec,i));
			Vector_Free(&vec);
			
			triangle t = {
				.p = {
					*(vec3d*)Vector_Get(&verts,f[0] - 1),
					*(vec3d*)Vector_Get(&verts,f[1] - 1),
					*(vec3d*)Vector_Get(&verts,f[2] - 1)
				},
				.c = 0xFFFFFFFF
			};
			triangle_CalcNorm(&t);
			Vector_Push(m,&t);
		}
	
		CStr_Free(&lineIn);
	}
	Vector_Free(&verts);
}
// void Mesh_ReadTex(Mesh* m,char* Path){
// 	if(!Files_isFile(Path)){
// 		printf("[Mesh]: Read -> Error!\n");
// 		return;
// 	}

// 	Vector verts = Vector_New(sizeof(vec3d));
// 	Vector texs = Vector_New(sizeof(vec2d));

// 	char* Content = Files_Path(Path);
// 	int end = -1;
	
// 	while(1){
// 		char* line = Content + end + 1;
// 		end = CStr_Find(line,'\n');
// 		if(end<0) break;

// 		char junk;

// 		if (line[0] == 'v'){
// 			if (line[1] == 't'){
// 				vec2d v;
// 				s >> junk >> junk >> v.u >> v.v;
// 				// A little hack for the spyro texture
// 				//v.u = 1.0f - v.u;
// 				//v.v = 1.0f - v.v;
// 				texs.push_back(v);
// 			}else{
// 				vec3d v;
// 				s >> junk >> v.x >> v.y >> v.z;
// 				verts.push_back(v);
// 			}
// 		}
// 		if (!bHasTexture){
// 			if (line[0] == 'f'){
// 				int f[3];
// 				s >> junk >> f[0] >> f[1] >> f[2];
// 				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
// 			}
// 		}else{
// 			if (line[0] == 'f'){
// 				s >> junk;
// 				string tokens[6];
// 				int nTokenCount = -1;
// 				while (!s.eof())
// 				{
// 					char c = s.get();
// 					if (c == ' ' || c == '/')
// 						nTokenCount++;
// 					else
// 						tokens[nTokenCount].append(1, c);
// 				}
// 				tokens[nTokenCount].pop_back();
				
// 				tris.push_back({ 
// 					verts[stoi(tokens[0]) - 1],
// 					verts[stoi(tokens[2]) - 1],
// 					verts[stoi(tokens[4]) - 1],
// 					texs[stoi(tokens[1]) - 1],
// 					texs[stoi(tokens[3]) - 1],
// 					texs[stoi(tokens[5]) - 1]
// 				});
// 			}
// 		}
// 	}
// }
void Mesh_Write(Mesh* m,char* Path){
	
}
void Mesh_Free(Mesh* m){
	Vector_Free(m);
}


#define WORLD3D_NORMAL_NONE	0
#define WORLD3D_NORMAL_CAP	1
#define WORLD3D_NORMAL_RCAP	2

typedef struct World3D{
	Mesh trisIn;
	Mesh trisBuff;
	Mesh trisOut;
	mat4x4 model;
	mat4x4 view;
	mat4x4 proj;
	unsigned int normal : 2;
	unsigned int flags : 31;
} World3D;

World3D World3D_New(){
	World3D m;
	m.trisIn = Vector_New(sizeof(triangle));
	m.trisBuff = Vector_New(sizeof(triangle));
	m.trisOut = Vector_New(sizeof(triangle));
	m.model = Matrix_MakeIdentity();
	m.view = Matrix_MakeIdentity();
	m.proj = Matrix_MakeIdentity();
	m.normal = WORLD3D_NORMAL_NONE;
	m.flags = 0;
	return m;
}
World3D World3D_Make(mat4x4 model,mat4x4 view,mat4x4 proj){
	World3D m;
	m.trisIn = Vector_New(sizeof(triangle));
	m.trisBuff = Vector_New(sizeof(triangle));
	m.trisOut = Vector_New(sizeof(triangle));
	m.model = model;
	m.view = view;
	m.proj = proj;
	m.normal = WORLD3D_NORMAL_NONE;
	m.flags = 0;
	return m;
}

void World3D_Set_Model(World3D* m,mat4x4 model){
	m->model = model;
}
void World3D_Set_View(World3D* m,mat4x4 view){
	m->view = view;
}
void World3D_Set_Proj(World3D* m,mat4x4 proj){
	m->proj = proj;
}

int World3D_Compare(const void* e1,const void* e2) {
	triangle t1 = *(triangle*)e1;
	triangle t2 = *(triangle*)e2;
	float z1 = (t1.p[0].z+t1.p[1].z+t1.p[2].z)/3;
    float z2 = (t2.p[0].z+t2.p[1].z+t2.p[2].z)/3;
    return z1 == z2 ? 0 : (z1 < z2 ? 1 : -1);
}
void World3D_triangle_process(World3D* m,triangle* t,vec3d camera,Vec2 output){
	triangle tri = *t;
	
	tri.p[0] = Matrix_MultiplyVector(m->model,tri.p[0]);
	tri.p[1] = Matrix_MultiplyVector(m->model,tri.p[1]);
	tri.p[2] = Matrix_MultiplyVector(m->model,tri.p[2]);
	triangle_CalcNorm(&tri);
	triangle_ShadeNorm(&tri,(vec3d){ 0.2f,0.1f,-0.6f });
	
	vec3d vCameraRay = vec3d_Sub(tri.p[0],camera);
	float dp = vec3d_DotProduct(tri.n,vCameraRay);

	char c = 0;
	if(m->normal == WORLD3D_NORMAL_NONE) 					c = 1;
	else if(m->normal == WORLD3D_NORMAL_CAP && dp < 0.0f) 	c = 1;
	else if(m->normal == WORLD3D_NORMAL_RCAP && dp > 0.0f) 	c = 1;

	if(c){
		tri.p[0] = Matrix_MultiplyVector(m->view,tri.p[0]);
		tri.p[1] = Matrix_MultiplyVector(m->view,tri.p[1]);
		tri.p[2] = Matrix_MultiplyVector(m->view,tri.p[2]);
		
		int nClippedTriangles = 0;
		triangle clipped[2];
		nClippedTriangles = triangle_ClipAgainstPlane(vec3d_New(0.0f,0.0f,0.1f),vec3d_New(0.0f,0.0f,1.0f),tri,&clipped[0],&clipped[1]);
		for (int n = 0; n < nClippedTriangles; n++){
			clipped[n].p[0] = Matrix_MultiplyVector(m->proj, clipped[n].p[0]);
			clipped[n].p[1] = Matrix_MultiplyVector(m->proj, clipped[n].p[1]);
			clipped[n].p[2] = Matrix_MultiplyVector(m->proj, clipped[n].p[2]);
			
			clipped[n].p[0] = vec3d_Div(clipped[n].p[0], clipped[n].p[0].w);
			clipped[n].p[1] = vec3d_Div(clipped[n].p[1], clipped[n].p[1].w);
			clipped[n].p[2] = vec3d_Div(clipped[n].p[2], clipped[n].p[2].w);
			
			clipped[n].p[0].x *= -1.0f;
			clipped[n].p[1].x *= -1.0f;
			clipped[n].p[2].x *= -1.0f;
			clipped[n].p[0].y *= -1.0f;
			clipped[n].p[1].y *= -1.0f;
			clipped[n].p[2].y *= -1.0f;

			vec3d vOffsetView = vec3d_New( 1,1,0 );
			clipped[n].p[0] = vec3d_Add(clipped[n].p[0], vOffsetView);
			clipped[n].p[1] = vec3d_Add(clipped[n].p[1], vOffsetView);
			clipped[n].p[2] = vec3d_Add(clipped[n].p[2], vOffsetView);
			clipped[n].p[0].x *= 0.5f * output.x;
			clipped[n].p[0].y *= 0.5f * output.y;
			clipped[n].p[1].x *= 0.5f * output.x;
			clipped[n].p[1].y *= 0.5f * output.y;
			clipped[n].p[2].x *= 0.5f * output.x;
			clipped[n].p[2].y *= 0.5f * output.y;
			Vector_Push(&m->trisBuff,&clipped[n]);
		}			
	}
}
void World3D_update(World3D* m,vec3d p,Vec2 output){
	Vector_Clear(&m->trisBuff);
	Vector_Clear(&m->trisOut);

	for(int i = 0;i<m->trisIn.size;i++){
		triangle* t = (triangle*)Vector_Get(&m->trisIn,i);
		World3D_triangle_process(m,t,p,output);
	}
	
	qsort(m->trisBuff.Memory,m->trisBuff.size,m->trisBuff.ELEMENT_SIZE,World3D_Compare);

	for(int i = 0;i<m->trisBuff.size;i++){
		triangle* triToRaster = (triangle*)Vector_Get(&m->trisBuff,i);
		triangle clipped[2];

		Vector list = Vector_New(sizeof(triangle));
		Vector_Push(&list,triToRaster);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				triangle test = *(triangle*)Vector_Get(&list,0);
				Vector_Remove(&list,0);
				nNewTriangles--;

				switch (p)
				{
				case 0:	nTrisToAdd = triangle_ClipAgainstPlane(vec3d_New( 0.0f, 0.0f, 0.0f ), 		 vec3d_New( 0.0f, 1.0f, 0.0f ), test, &clipped[0], &clipped[1]); break;
				case 1:	nTrisToAdd = triangle_ClipAgainstPlane(vec3d_New( 0.0f, output.y - 1, 0.0f ),vec3d_New( 0.0f, -1.0f, 0.0f ),test, &clipped[0], &clipped[1]); break;
				case 2:	nTrisToAdd = triangle_ClipAgainstPlane(vec3d_New( 0.0f, 0.0f, 0.0f ), 		 vec3d_New( 1.0f, 0.0f, 0.0f ), test, &clipped[0], &clipped[1]); break;
				case 3:	nTrisToAdd = triangle_ClipAgainstPlane(vec3d_New( output.x - 1, 0.0f, 0.0f ),vec3d_New( -1.0f, 0.0f, 0.0f ),test, &clipped[0], &clipped[1]); break;
				}

				for (int w = 0; w < nTrisToAdd; w++)
					Vector_Push(&list,&clipped[w]);
			}
			nNewTriangles = list.size;
		}

		for(int i = 0;i<list.size;i++) Vector_Push(&m->trisOut,Vector_Get(&list,i));
		Vector_Free(&list);
	}
}

void World3D_Free(World3D* m){
	Vector_Free(&m->trisIn);
	Vector_Free(&m->trisBuff);
	Vector_Free(&m->trisOut);
}

#endif