#ifndef _RESOURCEMANAGER_
#define _RESOURCEMANAGER_

#include "Module.h"
#include "Resource_.h"
#include <map>
#include <vector>
#include <list>
#include "Math\float3.h"

#define ResourcePrimitive 1

struct Vertex;

struct ReImport
{
	uint uuid = 0;
	const char* nameMesh = nullptr;
	const char* directoryObj = nullptr;
};

class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(bool start_enabled = true);
	virtual ~ModuleResourceManager();

	bool Start();
	update_status PreUpdate(float dt);
	//update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void ImportFile(std::list<const char*>& file);
	void ImportFile(std::vector<const char*>& file, std::vector<ReImport>& resourcesToReimport);
	
	Resource* CreateNewResource(Resource::Type type, uint uuid = 0);
	Resource* GetResource(uint id);
	Resource* GetResource(const char* material); //Only Use in ImportMesh -> Add ResourceMaterial
	Resource::Type CheckFileType(const char* filedir);

	void Init_IndexVertex(float3* vertex_triangulate, uint num_index, std::vector<uint>& indices, std::vector<float3>& vertices);
	void CreateResourceCube();

	Resource* ShowResources(bool& active, Resource::Type type);
	void ShowAllResources(bool& active);
	bool ReImportAllScripts();

	void Save();
	void Load();

public:
	std::vector<ReImport> resourcesToReimport;
	std::vector<uint> filestoDelete;
private:
	std::map<uint, Resource*> resources;
	std::vector<const char*> filesReimport;
	bool reimportNow = false;
	bool deleteNow = false;
	bool loadResources = true;
	bool reimportedScripts = false;
	bool scriptsSetNormal = false;
};

#endif