// ----------------------------------------------------------------------------
// Simple sample to prove that Assimp is easy to use with OpenGL.
// It takes a file name as command line parameter, loads it using standard
// settings and displays it.
//
// If you intend to _use_ this code sample in your app, do yourself a favour 
// and replace immediate mode calls with VBOs ...
//
// The vc8 solution links against assimp-release-dll_win32 - be sure to
// have this configuration built.
// ----------------------------------------------------------------------------

// AssetReader.cpp : implementation of the AssetReader class
//

#include "AssetReader.h"

// AssetReader

AssetReader::AssetReader()
{
	// the global Assimp scene object
	modelname = "";
	scene = NULL;
	isLogging = false;
}

AssetReader::~AssetReader()
{
	// We added a log stream to the library, it's our job to disable it
	// again. This will definitely release the last resources allocated
	// by Assimp.
	if (isLogging)
	{
		aiDetachAllLogStreams();
	}
}

std::string AssetReader::GetBasePath()
{
	std::string directory = "";
	size_t last_slash_idx = modelname.rfind('\\');
	if (std::string::npos == last_slash_idx)
	{
		last_slash_idx = modelname.rfind('/');
	}
	if (std::string::npos != last_slash_idx)
	{
		directory = modelname.substr(0, last_slash_idx + 1);
	}
	return directory;
}

bool AssetReader::Import3DFromFile( const std::string& pFile)
{

	//check if file exists
	std::ifstream fin(pFile.c_str());
	if(!fin.fail()) {
		modelname = pFile;
		fin.close();
	}
	else{
		printf("Couldn't open file: %s\n", pFile.c_str());
		printf("%s\n", importer.GetErrorString());
		return false;
	}

	if (isLogging)
	{
		SetupLog();
	}

	scene = importer.ReadFile( pFile, aiProcessPreset_TargetRealtime_MaxQuality );

	// If the import failed, report it
	if( !scene)
	{
		printf("%s\n", importer.GetErrorString());
		return false;
	}

	// Now we can access the file's contents.
	printf("Import of scene %s succeeded.",pFile.c_str());

	GetBoundingBox(&scene_min, &scene_max);	
	float tmp;
	tmp = scene_max.x - scene_min.x;
	tmp = scene_max.y - scene_min.y > tmp?scene_max.y - scene_min.y:tmp;
	tmp = scene_max.z - scene_min.z > tmp?scene_max.z - scene_min.z:tmp;

	scene_center.x = (scene_max.x + scene_min.x)/2;
	scene_center.y = (scene_max.y + scene_min.y)/2;
	scene_center.z = (scene_max.z + scene_min.z)/2;

	scaleFactor = 1.f / tmp;

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}

void AssetReader::SetupLog()
{
	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	//stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	//aiAttachLogStream(&stream);

	// ... same procedure, but this stream now writes the
	// log messages to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
	aiAttachLogStream(&stream);

}


// ----------------------------------------------------------------------------
// Scene loader stuff
//
void AssetReader::GetBoundingBoxForNode (const aiNode* nd, 
								aiVector3D* min, 
								aiVector3D* max, 
								aiMatrix4x4* trafo){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		GetBoundingBoxForNode(nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}

// ----------------------------------------------------------------------------
void AssetReader::GetBoundingBox (aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	GetBoundingBoxForNode(scene->mRootNode,min,max,&trafo);
}

