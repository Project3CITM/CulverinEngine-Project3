#ifndef _CUBEMAP_TEXTURE_H_
#define _CUBEMAP_TEXTURE_H_

#include <string>

class CubeMap_Texture
{
public:
	CubeMap_Texture();
	CubeMap_Texture(unsigned int size);
	~CubeMap_Texture() {
		Destroy(); 
	}

	void Resize(int new_size) 
	{
		Destroy();
		cubemap_size = new_size;
		Create();
	}

	unsigned int GetTextureId() const {	
		return cubemap_texture_id;
	}
	
	unsigned int GetDepthId() const {
		return cubemap_depth_id;
	}

	unsigned int GetFbo() const { return cubemap_fbo; }
	unsigned int GetSize() const { return cubemap_size; }
	std::string GetName() const { return name; }
	void SetName(std::string _name) { name = _name; }
	void Bind();
	void UnBind();
	void Create();
	void Destroy();
public:
	Component * parent;
private:

	std::string name;

	unsigned int cubemap_texture_id = 0;
	unsigned int cubemap_fbo = 0;
	unsigned int cubemap_depth_id = 0;
	unsigned int cubemap_size = 128;
	

};

#endif // !_CUBEMAP_TEXTURE_H_

