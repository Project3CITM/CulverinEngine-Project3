#ifndef _RESOURCEMATERIAL_
#define _RESOURCEMATERIAL_

#include "Resource_.h"
#include <string>

struct Texture
{
	uint id;
	//Const char* ?
	std::string name_ext;
	//std::string path;
	std::string name;
	uint w = 0;
	uint h = 0;
};

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(uint uid);
	virtual ~ResourceMaterial();

	void InitInfo(const char* name, const char* path);
	void Init(Texture texture);
	void DeleteToMemory();

	bool LoadToMemory();
	uint GetTextureID();
	Resource::State IsLoadedToMemory();
	const char* GetTextureName();
	uint GetTextureWidth() const ;
	uint GetTextureHeight() const;


private:
	Texture texture;
};

#endif