#include "ResourceMaterial.h"
#include "Application.h"


ResourceMaterial::ResourceMaterial(uint uuid) : Resource(uuid, Resource::Type::MATERIAL, Resource::State::UNLOADED)
{
	num_game_objects_use_me = 0;
	LOG("Resource Material Created!");
}


ResourceMaterial::~ResourceMaterial()
{
	texture.name.clear();
	texture.name_ext.clear();
}

void ResourceMaterial::InitInfo(const char* nameResource, const char* path)
{
	name = nameResource;
	path_assets = path;
}

void ResourceMaterial::Init(Texture textureloaded)
{
	texture.id = textureloaded.id;
	texture.name = textureloaded.name;
}

void ResourceMaterial::DeleteToMemory()
{
	state = Resource::State::UNLOADED;
	glDeleteTextures(1, &texture.id);
	texture.id = 0;
	texture.name = "";
	LOG("UnLoaded Resource Material");
}

bool ResourceMaterial::LoadToMemory()
{
	state = Resource::State::LOADED;
	LOG("Loaded Resource Material");
	return true;
}

uint ResourceMaterial::GetTextureID()
{
	if (texture.id > 0)
	{
		return texture.id;
	}
}


Resource::State ResourceMaterial::IsLoadedToMemory()
{
	return state;
}

const char * ResourceMaterial::GetTextureName()
{
	return texture.name.c_str();
}

uint ResourceMaterial::GetTextureWidth() const
{
	return texture.w;
}

uint ResourceMaterial::GetTextureHeight() const
{
	return texture.h;
}






