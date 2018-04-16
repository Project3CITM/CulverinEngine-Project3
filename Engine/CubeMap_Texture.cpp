#include "CubeMap_Texture.h"
#include "GL3W/include/glew.h"

CubeMap_Texture::CubeMap_Texture(unsigned int size) : cubemap_size(size)
{
	Create();
}

void CubeMap_Texture::Create()
{
	//Create the cube map texture
	glGenTextures(1, &cubemap_texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_id);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			cubemap_size, cubemap_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//Create the buffer
	glGenBuffers(1, &cubemap_fbo);
}

void CubeMap_Texture::Destroy()
{
	glDeleteFramebuffers(1, &cubemap_fbo);
	glDeleteTextures(1, &cubemap_texture_id);
}

void CubeMap_Texture::Bind()
{
	glBindBuffer(GL_FRAMEBUFFER, cubemap_fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void CubeMap_Texture::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

