#include "DepthCubeMap.h"

#include "GL3W/include/glew.h"


DepthCubeMap::DepthCubeMap(unsigned int shadow_width, unsigned int shadow_height) : shadow_width(shadow_width), shadow_height(shadow_height)
{
}


DepthCubeMap::~DepthCubeMap()
{
	Destroy();
}

void DepthCubeMap::Create()
{
	glGenFramebuffers(1, &depth_map_fbo);
	glGenTextures(GL_TEXTURE_CUBE_MAP, &depth_cubemap_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap_texture);
	
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap_texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthCubeMap::Destroy()
{
	glDeleteFramebuffers(1, &depth_map_fbo);
	glDeleteTextures(1, &depth_cubemap_texture);
}

void DepthCubeMap::Resize(unsigned width, unsigned height)
{
	if(width != shadow_width || height != shadow_height)
	{
		shadow_width = width;
		shadow_height = height;
		Destroy();
		Create();
	}
}

unsigned DepthCubeMap::GetTexture() const
{
	return depth_cubemap_texture;
}

unsigned DepthCubeMap::GetFrameBufferId() const
{
	return depth_map_fbo;
}

void DepthCubeMap::GetShadowResolution(unsigned& width, unsigned& height)
{
	width = shadow_width;
	height = shadow_height;
}

void DepthCubeMap::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
}

void DepthCubeMap::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
