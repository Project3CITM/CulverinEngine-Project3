#ifndef _MODULEFRAMEBUFFER_
#define _MODULEFRAMEBUFFER_

#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "WindowManager.h"

class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();

	bool Create(int, int);
	void Bind(const char* window);
	void UnBind(const char* window);
	void Destroy();
	void Resize(int Width, int Height);
	void WantRefreshRatio();
	unsigned int GetTexture() const;

public:
	bool	refresh = false;
	uint	frame_id = 0;
	uint	rbo;
	uint	texture;
	uint	depth_id = 0;

	int		width;
	int		height;
	float2	size;
};

#endif