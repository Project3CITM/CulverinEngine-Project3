#include "Application.h"
#include "CubeMap_Texture.h"
#include "GL3W/include/glew.h"
#include "ModuleRenderer3D.h"


CubeMap_Texture::CubeMap_Texture()
{
	cubemap_size = 1024;
	
	//App->renderer3D->cube_maps.push_back(this);
}

CubeMap_Texture::CubeMap_Texture(unsigned int size) : cubemap_size(size)
{
	Create();
}

void CubeMap_Texture::Create()
{
	glGenFramebuffers(1, &cubemap_fbo);

	//Depth 
	glGenRenderbuffers(1, &cubemap_depth_id);

	//texture
	glGenTextures(1, &cubemap_texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_id);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8,
			cubemap_size, cubemap_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, cubemap_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubemap_texture_id, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeMap_Texture::Destroy()
{
	glDeleteFramebuffers(1, &cubemap_fbo);
	glDeleteTextures(1, &cubemap_texture_id);
}

void CubeMap_Texture::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, cubemap_fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, cubemap_depth_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemap_size, cubemap_size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cubemap_depth_id);
}

void CubeMap_Texture::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

