#ifndef RESOURCE_FONT_H
#define RESOURCE_FONT_H
#include "Resource_.h"

typedef struct _TTF_Font TTF_Font;
class ResourceFont:public Resource
{
public:
	ResourceFont(uint uid);
	~ResourceFont();

	TTF_Font* font = NULL;
};
#endif//RESOURCE_FONT_H
