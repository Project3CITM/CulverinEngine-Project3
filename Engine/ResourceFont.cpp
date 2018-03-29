#include "ResourceFont.h"
#include "ImportFont.h"
#include "SDL2_ttf/include/SDL_ttf.h"



ResourceFont::ResourceFont(uint uuid):Resource(uuid, Resource::Type::FONT, Resource::State::UNLOADED)
{
	num_game_objects_use_me = 0;
	LOG("Resource Font Created!");
}


ResourceFont::~ResourceFont()
{
	TTF_CloseFont(font.font);

}

void ResourceFont::InitInfo(const char * nameResource, const char * path)
{
	name = nameResource;
	path_assets = path;
}

void ResourceFont::Init(Font copy_font)
{
	font.size = copy_font.size;
	font.font = copy_font.font;

}

bool ResourceFont::ReLoadToMemory()
{
	if (font.font != NULL)
		TTF_CloseFont(font.font);
	App->importer->iFont->LoadResource(std::to_string(GetUUID()).c_str(), font.size, this);

	return true;
}

void ResourceFont::DeleteToMemory()
{
	state = Resource::State::UNLOADED;
	if (font.font == NULL)
		return;
	LOG("UnLoaded Resource Material");
}

bool ResourceFont::LoadToMemory()
{
	state = Resource::State::LOADED;
	LOG("Loaded Resource Material");
	return true;
}

void ResourceFont::SetSize(int size)
{
	font.size = size;
}

Resource::State ResourceFont::IsLoadedToMemory()
{
	return state;
}
