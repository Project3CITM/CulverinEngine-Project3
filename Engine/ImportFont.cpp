#include "ImportFont.h"
#include "SDL2_ttf/include/SDL_ttf.h"
#include "ResourceFont.h"


ImportFont::ImportFont()
{
}


ImportFont::~ImportFont()
{
}

bool ImportFont::Import(const char * file, uint uuid, bool isAutoImport)
{
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
