#ifndef _IMPORT_FONT_
#define _IMPORT_FONT_

#include "Module.h"
#include "Application.h"
#include "ModuleImporter.h"
struct Font;
class ResourceFont;

class ImportFont
{
public:
	ImportFont();
	~ImportFont();


	//bool Import(const char* file, const char* path, std::string& output_file);
	bool Import(const char* file, uint uuid = 0, bool isAutoImport = false);
	Font Load(const char * file, int size);
	bool LoadResource(const char * file,int size, ResourceFont* resource_font);

};

#endif