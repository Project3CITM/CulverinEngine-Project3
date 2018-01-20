#ifndef _RESOURCEMATERIAL_
#define _RESOURCEMATERIAL_

#include "Resource_.h"
#include <string>

struct Texture
{
	uint id;
	//Const char* ?
	std::string nameExt;
	//std::string path;
	std::string name;

};

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(uint uid);
	virtual ~ResourceMaterial();

	void InitInfo(const char* name);
	void Init(Texture texture);
	void DeleteToMemory();

	bool LoadToMemory();
	uint GetTextureID();
	Resource::State IsLoadedToMemory();

private:
	Texture texture;
};

#endif