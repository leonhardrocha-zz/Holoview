#ifndef _OPENGL21_RENDERER_H_
#define _OPENGL21_RENDERER_H_

#include "BaseAssetRenderer.h"
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

// include DevIL for image loading
#include <IL/il.h>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

class OpenGL21Renderer : BaseAssetRenderer
{
public:
	OpenGL21Renderer() : scene_list(0) {};
	~OpenGL21Renderer() { };
	virtual void Initialize();
	virtual void Render();

	std::string Modelname;

protected:
	bool flipZ;
	int scene_list;
	const aiScene* scene;
	AssetReader reader;
	virtual void LoadGLTextures(std::string directory);
	virtual void ApplyMaterial(const aiMaterial *mtl);
	virtual void RecursiveRender (const aiScene *sc, const aiNode* nd);

private:
	float modelviewMatrix[16];
	float viewUp[3];
	float viewFront[3];
	std::map<std::string, GLuint> textureIdMap;	
};

#endif