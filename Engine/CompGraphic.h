#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H
#include "Component.h"

class CompGraphic : public Component
{
public:
	CompGraphic(Comp_Type t, GameObject* parent);
	CompGraphic(const CompGraphic& copy, GameObject* parent);
	~CompGraphic();

	void SetTextureID(uint set_texture_id);
	uint GetTextureID()const;
private:
public:
private:
	uint texture_id = 0;
};

#endif//COMPONENT_GRAPHIC_H
