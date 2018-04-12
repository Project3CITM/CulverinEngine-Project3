#ifndef __DEPTHCUBEMAP_H__
#define __DEPTHCUBEMAP_H__

class DepthCubeMap
{
public:
	DepthCubeMap(unsigned int shadow_width = 1024, unsigned int shadow_height = 1024);
	virtual ~DepthCubeMap();

	void Create();
	void Destroy();

	void Resize(unsigned int width, unsigned int height);

	unsigned int GetTexture()const;
	unsigned int GetFrameBufferId()const;
	void GetShadowResolution(unsigned int& width, unsigned int& height);

	void Bind();
	void UnBind();

private:
	unsigned int depth_map_fbo = 0;
	unsigned int depth_cubemap_texture = 0;
	unsigned int shadow_width, shadow_height;
};

#endif // !__DEPTHCUBEMAP_H__