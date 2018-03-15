#include "ImportFont.h"
#include "SDL2_ttf/include/SDL_ttf.h"
#include "ResourceFont.h"
#include "ModuleFS.h"

ImportFont::ImportFont()
{
}


ImportFont::~ImportFont()
{
}

bool ImportFont::Import(const char * file, uint uuid, bool isAutoImport)
{

	std::string name = std::to_string(uuid);
	name = App->fs->FixName_directory(name);//?
	name = App->fs->FixExtension(name, ".ttf");

	App->fs->DuplicateFile(file, name.c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_FONT);
	/*
	   std::ifstream  src("from.ogv", std::ios::binary);
    std::ofstream  dst("to.ogv",   std::ios::binary);

	*/
	return false;
}

Font ImportFont::Load(const char * file, int size)
{
	Font ret;
	ret.font= TTF_OpenFont(file, size);
	ret.size = size;
	return ret;
}

bool ImportFont::LoadResource(const char * file, int size, ResourceFont * resource_font)
{
	Font font = Load(file, size);
	resource_font->Init(font);
	resource_font->LoadToMemory();
	return true;
}
