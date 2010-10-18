#pragma once
#include "Image.h"

//EXT means that it's not guarranteed to stay in here, be warned when using it

//struct AEMaterial;
typedef struct AEMaterial AEMaterial;

bool AEMaterialsShadersEnabledGet(void);
void AEMaterialsShadersEnabledSet(bool on);

float AEMaterialsTimeGet(void);
void AEMaterialsTimeSet(float seconds);

AEMaterial* AEMaterialNew(void);

AEMaterial* AEMaterialRetain(AEMaterial* material);

void AEMaterialDelete(AEMaterial* material);

void AEMaterialBind(AEMaterial* material);
void AEMaterialBindLightmapEXT(AEMaterial* material,AETexture lightmap);
void AEMaterialBindShaderOnlyEXT(AEMaterial* material);

//Does not store a copy of the shader text!
//Calling it with both texts as NULL links the shaders and generates a program
void AEMaterialShaderSet(AEMaterial* material,char* vshadertext,char* fshadertext);
//Null terminated array of Null terminated strings
void AEMaterialShaderSetMulti(AEMaterial* material,char** vshadertextarray,char** fshadertextarray);

//"Consumes" the image passed into it so that AEMaterialTextureSet(material,"name",AEImageLoad("filename.png"));  will work with out leak.  If you don't want it to delete it, just *Retain() it before passing it in.
void AEMaterialTextureSetWithFlags(AEMaterial* material,char* name,AEImage* image,unsigned int flags);
#define AEMaterialTextureSet(material,name,image) AEMaterialTextureSetWithFlags(material,name,image,AETextureLoadFlagDefault)