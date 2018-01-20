#ifndef _IMPORT_
#define _IMPORT_

#include "Module.h"
#include "GameObject.h"
#include "ModuleResourceManager.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


#pragma comment (lib,"Assimp/libx86/assimp.lib")

class ImportMesh;
class ImportMaterial;
class ImportScript;
class CompTransform;
struct ReImport;

//enum FileTypeImport
//{
//	F_UNKNOWN_i = -1,
//	F_MODEL_i,
//	F_TEXTURE_i
//};

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	//update_status Update(float dt);
	//update_status PostUpdate(float dt);
	GameObject* ProcessNode(aiNode* node, const aiScene* scene, GameObject* obj);
	GameObject* ProcessNode(aiNode* node, const aiScene* scene, GameObject* obj, std::vector<ReImport>& resourcesToReimport);
	void ProcessTransform(aiNode* node, CompTransform* trans);
	void ProcessTransform(CompTransform* trans);
	bool CleanUp();


	bool Import(const char* file, Resource::Type type);
	bool Import(const char* file, Resource::Type type, std::vector<ReImport>& resourcesToReimport);
	//FileTypeImport CheckFileType(char* filedir);
	//bool Import();

public:
	char directoryExe[MAX_PATH];
	//FileTypeImport dropped_File_type = F_UNKNOWN_i;

	ImportMesh* iMesh = nullptr;
	ImportMaterial* iMaterial = nullptr;
	ImportScript* iScript = nullptr;

private:

};

#endif