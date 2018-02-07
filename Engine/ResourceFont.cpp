#include "ResourceFont.h"



ResourceFont::ResourceFont(uint uuid):Resource(uuid, Resource::Type::FONT, Resource::State::UNLOADED)
{
	num_game_objects_use_me = 0;
	LOG("Resource Font Created!");
}


ResourceFont::~ResourceFont()
{
}
