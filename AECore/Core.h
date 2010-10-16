/*
Writen by Oliver Daids, 2009.

Ambition Engine Core.
*/

#pragma once
#include "AE.h"
#include "SOIL.h"

typedef struct AEContext{
	unsigned int w,h,r,g,b,a,stencil,depth,inFullscreen;
	void* aux;
	
	void (*init)(struct AEContext* self,char* title);
	void (*refresh)(struct AEContext* self);
	int (*pollinput)(struct AEContext* self);
	void (*swapbuffers)(struct AEContext* self);
	void (*deinit)(struct AEContext* self);
	double (*seconds)(struct AEContext* self);
}AEContext;

void AEContextActiveSet(AEContext* context);
AEContext* AEContextActiveGet(void);

void AEContextInit(AEContext* context,char* title,int w,int h);
void AEContextStart(AEContext* context,void (*func)(float));
void AEContextQuit(AEContext* context);

///////////////////////////////////////////
//////////Utility stuff
//////////////////////////////////////////
typedef GLuint AETexture;
#define AETextureLoadFlagDXTCompression SOIL_FLAG_COMPRESS_TO_DXT
#define AETextureLoadFlagMipmaps SOIL_FLAG_MIPMAPS
#define AETextureLoadFlagInvertY SOIL_FLAG_INVERT_Y

#define AETextureLoadFlagDefault SOIL_FLAG_COMPRESS_TO_DXT|SOIL_FLAG_MIPMAPS

AETexture AETextureLoadWithFlags(const char* filename,unsigned int flags);
#define AETextureLoad(filename) AETextureLoadWithFlags(filename,AETextureLoadFlagDefault)
AETexture AETextureLoadFromMemoryWithFlags(void* data,size_t dataSize, unsigned int flags);
static inline void AETextureBind(AETexture texture){glBindTexture(GL_TEXTURE_2D, texture);}
void AETextureDelete(AETexture texture);

void AEError_internal(char* message,const char* function);
#define AEError(msg) AEError_internal(msg,__func__)

uint64_t AEHostU64FromNet(uint64_t netu64);
uint64_t AENetU64FromHost(uint64_t hostu64);

uint32_t AEHostU32FromNet(uint32_t netu32);
uint32_t AENetU32FromHost(uint32_t hostu32);


typedef struct {
	size_t length;
	size_t allocated;
	size_t position;
	void* bytes;
}AEMBuffer;

void AEMBufferInit(AEMBuffer* self);
void AEMBufferDeinit(AEMBuffer* self);
void* AEMBufferBytesGet(AEMBuffer* self, size_t size);
void AEMBufferRead(AEMBuffer* self, void* data, size_t size);
void AEMBufferWrite(AEMBuffer* self, void* data, size_t size);
void AEMBufferPositionSet(AEMBuffer* self, size_t position);
size_t AEMBufferPositionGet(AEMBuffer* self);
size_t AEMBufferLengthGet(AEMBuffer* self);