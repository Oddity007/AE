#include "../Core.h"
#include "../SOIL/SOIL.h"
#include <math.h>
#include <string.h>
#include "../Camera.h"

///////////////////////////////////////////////////
//Texture Stuff
AETexture AETextureLoadWithFlags(const char* filename, unsigned int flags){
	//SOIL is EPIC, no denial.
	//Handles EVERYTHING, beautifully too
	AETexture texid = SOIL_load_OGL_texture
		(
			filename,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			flags | SOIL_FLAG_TEXTURE_REPEATS |  SOIL_FLAG_INVERT_Y
		);
	if(texid==0) printf("Texture loading of %s failed because %s",filename,SOIL_last_result());
	AETextureBind(texid);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	return texid;
}

AETexture AETextureLoadFromMemoryWithFlags(void* data,size_t dataSize, unsigned int flags){
	AETexture texture = SOIL_load_OGL_texture_from_memory(data, dataSize, SOIL_LOAD_AUTO, 0, flags | SOIL_FLAG_TEXTURE_REPEATS |  SOIL_FLAG_INVERT_Y);
	if(texture==0) printf("Texture loading from memory failed because %s",SOIL_last_result());
	AETextureBind(texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	return texture;
}

void AETextureDelete(AETexture texture){
	glDeleteTextures(1,&texture);
}
/////////////////////////////////////////////////////
//Utility



static uint64_t AEUInt64TwoToThe(uint64_t exponent){
	uint64_t x=1;
	for(uint64_t i=0;i<exponent;i++) x*=2;
	return x;
}

uint64_t AEUInt64From8Bytes(uint8_t* bytes,bool bigendian){
	uint64_t integer=0;
	for(int i=0;i<8;i++){
		int byteID=i;
		if(bigendian) byteID=8-i-1;
		integer+=(uint64_t)bytes[byteID]*AEUInt64TwoToThe(8*i);
	}
	return integer;
}

void AEUInt64To8Bytes(uint64_t integer,uint8_t* bytes,bool bigendian){
	for(int i=0;i<8;i++) bytes[i]=0;
	uint64_t quotient=integer;
	for(int i=0;i<8;i++){
		int byteID=i;
		if(bigendian) byteID=8-i-1;
		if(quotient==0) break;
		bytes[byteID]=quotient%256;
		quotient/=256;
	}
}

uint32_t AEUInt32From4Bytes(uint8_t* bytes,bool bigendian){
	uint32_t integer=0;
	for(int i=0;i<4;i++){
		int byteID=i;
		if(bigendian) byteID=4-i-1;
		integer+=(uint32_t)bytes[byteID]*AEUInt64TwoToThe(8*i);
	}
	return integer;
}

void AEUInt32To4Bytes(uint32_t integer,uint8_t* bytes,bool bigendian){
	for(int i=0;i<4;i++) bytes[i]=0;
	uint32_t quotient=integer;
	for(int i=0;i<4;i++){
		int byteID=i;
		if(bigendian) byteID=4-i-1;
		if(quotient==0) break;
		bytes[byteID]=quotient%256;
		quotient/=256;
	}
}

uint64_t AEHostU64FromNet(uint64_t netu64){
	return AEUInt64From8Bytes((uint8_t*)&netu64, true);
}

uint64_t AENetU64FromHost(uint64_t hostu64){
	AEUInt64To8Bytes(hostu64, (uint8_t*)&hostu64, true);
	return hostu64;
}

uint32_t AEHostU32FromNet(uint32_t netu32){
	return AEUInt32From4Bytes((uint8_t*)&netu32, true);
}

uint32_t AENetU32FromHost(uint32_t hostu32){
	AEUInt32To4Bytes(hostu32, (uint8_t*)&hostu32, true);
	return hostu32;
}

void AEError_internal(char* message,const char* function){
	printf("ERROR in %s():\n\t%s\n",function,message);
	fflush(stdout);
	abort();
}
////////////////////////////////////////////////////
//File Stuff


void AEMBufferInit(AEMBuffer* self){
	memset(self, 0, sizeof(AEMBuffer));
}

void AEMBufferDeinit(AEMBuffer* self){
	if (not self) return;
	free(self->bytes);
	memset(self, 0, sizeof(AEMBuffer));
}
void* AEMBufferBytesGet(AEMBuffer* self, size_t size){
	self->position+=size;
	if(self->position > self->length){
		self->position=self->length;
		return NULL;
	}
	return self->bytes+(self->position-size);
}

void AEMBufferRead(AEMBuffer* self, void* data, size_t size){
	void* from=AEMBufferBytesGet(self, size);
	if(data and from) memcpy(data, from, size);
}

void AEMBufferWrite(AEMBuffer* self, void* data, size_t size){
	size_t oldPosition=self->position;
	void* to=AEMBufferBytesGet(self, size);
	if(not to){
		self->position=oldPosition;
		self->length += size;
		self->allocated = self->length * 1.2;
		self->bytes=realloc(self->bytes, self->allocated);
		to=AEMBufferBytesGet(self, size);
	}
	if(data and to) memcpy(to, data, size);
}

void AEMBufferPositionSet(AEMBuffer* self, size_t position){
	self->position=position;
}

size_t AEMBufferPositionGet(AEMBuffer* self){
	return self->position;
}

size_t AEMBufferLengthGet(AEMBuffer* self){
	return self->length;
}

////////////////////////////////////////////////////
//View Stuff
AEContext AEContextActive={
	.w=800,
	.h=500,
	.r=8,
	.g=8,
	.b=8,
	.a=8,
	.stencil=8,
	.depth=8,
	.inFullscreen=0
};

void AEContextActiveSet(AEContext* context){
	AEContextActive=*context;
}

AEContext* AEContextActiveGet(void){
	return &AEContextActive;
}

void AEContextInit(AEContext* context,char* title,int w,int h){
	if(not context) context=AEContextActiveGet();
	
	if(context->init==NULL || context->refresh==NULL || context->pollinput==NULL || context->swapbuffers==NULL || context->deinit==NULL || context->seconds==NULL) AEError("AEContext function pointers need to all be filled before you can use the engine.");
	
	context->w=w;context->h=h;
	context->init(context,title);
	
	AECamera* cam=AECameraActiveGet();
	AECameraViewportSet(cam,context->w,context->h);
	
	//Because it also affects the modelview matrix unfortunately
	glPushMatrix();
	AECameraBind(cam);
	glPopMatrix();
	
	glClearColor(0,0,0,1);
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable(GL_DEPTH_TEST);
	glEnable( GL_TEXTURE_2D );
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

static void AEDefaultPerframeFunc(float step){}

void AEContextStart(AEContext* context,void (*perframe)(float)){
	//0 is a magical number, simply acts as the default
	if(perframe==NULL) perframe=AEDefaultPerframeFunc;
	
	if(not context) context=AEContextActiveGet();
	
	float now=context->seconds(context);
	float then=now;
	while(context->pollinput(context)){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //This is default
		//glClear(GL_DEPTH_BUFFER_BIT); // Use this if you always use a skybox
		AECameraBind(AECameraActiveGet());
		
		now=context->seconds(context);
		perframe((now-then));
		then=now;
		//Sounds....  Poetic
		
		context->swapbuffers(context);
	}
	context->deinit(context);
}

void AEContextQuit(AEContext* context){
	if(not context) context=AEContextActiveGet();
	context->deinit(context);
}