#ifndef _IMPORT_ANIMATION_
#define _IMPORT_ANIMATION_

#include "Module.h"
#include "Application.h"
#include "ModuleImporter.h"

class ResourceAnimation;

class ImportAnimation	
{
public:
	ImportAnimation();
	~ImportAnimation();

	//bool Import(const char* file, const char* path, std::string& output_file);
	bool Import(const aiAnimation* mesh, const char* name, const char* file);
	bool LoadResource(const char * file, ResourceAnimation* resourceMaterial);
};

#endif
