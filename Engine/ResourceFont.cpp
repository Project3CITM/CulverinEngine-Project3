#include "ResourceFont.h"



ResourceFont::ResourceFont(uint uuid):Resource(uuid, Resource::Type::FONT, Resource::State::UNLOADED)
{
	NumGameObjectsUseMe = 0;
	LOG("Resource Font Created!");
}


ResourceFont::~ResourceFont()
{
}
