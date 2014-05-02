#ifndef _BASE_ASSET_RENDERER_H_
#define _BASE_ASSET_RENDERER_H_

#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "AssetReader.h"

class BaseAssetRenderer
{
public:
	virtual void Initialize()=0;
	virtual void Render()=0;

protected:
	BaseAssetRenderer *renderer;
	virtual void LoadGLTextures(std::string directory)=0;
	virtual void ApplyMaterial(const aiMaterial *mtl)=0;
	virtual void RecursiveRender (const aiScene *sc, const aiNode* nd)=0;
};

#endif