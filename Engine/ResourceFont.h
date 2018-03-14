#ifndef RESOURCE_FONT_H
#define RESOURCE_FONT_H
#include "Resource_.h"

typedef struct _TTF_Font TTF_Font;

struct Font
{
	TTF_Font* font = NULL;
	int size = 12;
};
class ResourceFont:public Resource
{
public:
	ResourceFont(uint uid);
	~ResourceFont();
	void InitInfo(const char* name, const char* path);
	void Init(Font copy_font);
	void DeleteToMemory();
	bool LoadToMemory();
	Resource::State IsLoadedToMemory();

public:
	Font font;
};
#endif//RESOURCE_FONT_H
