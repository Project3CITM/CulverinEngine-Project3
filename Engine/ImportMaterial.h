#ifndef _IMPORT_MATERIAL_
#define _IMPORT_MATERIAL_

#include "Module.h"
#include "Application.h"
#include "ModuleImporter.h"

struct Texture;
class ResourceMaterial;

class ImportMaterial
{
public:
	ImportMaterial();
	~ImportMaterial();

	//bool Import(const char* file, const char* path, std::string& output_file);
	bool Import(const char* file, uint uuid = 0);
	Texture Load(const char * file);
	bool LoadResource(const char * file, ResourceMaterial* resourceMaterial);


};

#endif