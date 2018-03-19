#include "ModuleFramebuffers.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "CompCamera.h"
FrameBuffer::FrameBuffer()
{
	frame_id = 0;
	rbo = 0;
	texture = 0;
	width = 0;
	height = 0;
	resize = true;
}

FrameBuffer::~FrameBuffer()
{
	Destroy();
}

bool FrameBuffer::Create(int width, int height)
{
	//Creates Framebuffer Object
	glGenFramebuffers(1, &frame_id);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);

	//Create Color texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Create Depth Texture
	glGenTextures(1, &rbo);
	glBindTexture(GL_TEXTURE_2D, rbo);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	//Attach both textures to fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rbo, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);


	this->width = width;
	this->height = height;

	//If the fbo is not created return false
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void FrameBuffer::Bind(const char* window)
{
	if(resize)
		Resize(App->window->GetWidth(), App->window->GetHeight());

	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);
}

void FrameBuffer::UnBind(const char* window)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Init(const char * window)
{
	if (App->mode_game == false)
	{
		Bind(window);

	}
	// Refresh Projection of the camera
	App->renderer3D->UpdateProjection(App->renderer3D->active_camera);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf(App->renderer3D->active_camera->GetViewMatrix());

}

void FrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &frame_id);
	glDeleteTextures(1, &texture);
	glDeleteRenderbuffers(1, &rbo);
}

void FrameBuffer::Resize(int width, int height)
{
	if (this->width != width || this->height != height || refresh)
	{
		Destroy();
		Create(width, height);
		App->renderer3D->OnResize(width, height);
		refresh = false;
	}
}

void FrameBuffer::WantRefreshRatio()
{
	refresh = true;
}


unsigned int FrameBuffer::GetTexture() const
{
	return texture;
}
