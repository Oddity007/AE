/*
Writen by Oliver Daids, 2009.

An Engine Core.
*/

#ifndef AEv1_h
#define AEv1_h
#ifdef __cplusplus
extern "C" {
//#error C++ Warning: STOP!!!!  Foul language is not permitted beyond this point.
#endif
//#include <SDL/SDL.h>
//#include <SDL/SDL_opengl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#pragma pack(push, 1)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///							Core
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////	types	///////////////////////////////////////
typedef struct{float x,y;}AEVec2;
typedef struct{float x,y,z;}AEVec3;
typedef struct{float x,y,z,w;}AEVec4;
typedef struct{int x,y;}AEVec2i;
typedef struct{int x,y,z;}AEVec3i;
typedef struct{int x,y,z,w;}AEVec4i;

typedef struct{unsigned int v[3],n[3],t[3];}AEFace;
typedef struct{
	struct{unsigned int v,f,t,n;}count;
	AEVec3* v;
	AEVec2* t;
	AEVec3* n;
	AEFace* f;
	void* aux;
}AEMesh;

typedef struct{//YadaYadaYada Packing
	AEVec2 t;
	AEVec3 v;
}AEVBOVert;

typedef struct{//YadaYadaYada Packing
	AEVec2 t;
	AEVec3 n,v;
}AEVBOVertWithNormal;

typedef struct{
	unsigned int vbo,nbo,ibo,vcount,icount,vallocated,iallocated,*indices;
	AEVec3* n;
	AEVBOVert* verts;
	char hasNormals;
}AEVBO;

typedef struct AEObject{
	//Position Info
	float x,y,z;
	AEVec3 rotation;
	//Essential data
	void (*event)(struct AEObject* object,int event,void* eventData);
	void* model;
	unsigned int texture;
	//Physical Body Data
	AEVec3 size,velocity;
	float mass;
	//Auxilary, for when it is easier not to extend this "class"
	void* aux;
	char isStatic;
}AEObject;

#define AEObjectEventInit 1
#define AEObjectEventRender 2
#define AEObjectEventCollide 3
#define AEObjectEventRelease 4
#define AEObjectEventRenderFromCompile 5

#define AEObjectDynamic 0
#define AEObjectStatic 1
#define AEObjectCompiled 2

//Types that are only used for a singleton

typedef struct{
	float x,y,z;
	AEVec3 rotation;
	//View data
	float fov,near,far;
}AECameraType;

typedef struct{
	int w,h;
	int r,g,b,a,stencil,depth,inFullscreen;
}AEScreenType;
///////////////////// Variables /////////////////

extern AECameraType AECamera;

extern AEScreenType AEScreen;

extern AEVec2i AEMouse;

extern AEObject** AEObjects;

extern unsigned int AEObjectCount;

extern int AEBlockKeyInput;

extern unsigned char AEVBOAddAllocateFreq;

extern unsigned int AETextureLoadFlags;

//////////////////////////////////	prototypes	////////////////////////////////////


/////////   
void AEInit(char* title,int w,int h);
void AEStart(void (*func)(float));
void AEQuit(void);
void AERefreshViewport(char in2d);
/////////
int AEKey(int key);
int AEMouseButton(char button);
void AEPollInput(void);
/////////
unsigned int AETextureLoad(const char* filename);
inline void AETextureBind(unsigned int texture){glBindTexture(GL_TEXTURE_2D,(GLuint)texture);}
void AETextureDelete(unsigned int texture);

unsigned int AELinearSearch_internal(void* value,void* array,int length,int size);
#define AELinearSearch(val,array,len) AELinearSearch_internal(val,array,len,sizeof(*val))

#include "HeaderEnd.h"