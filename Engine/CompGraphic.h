#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H
#include "Component.h"
class CompRectTransform;
class CompCanvasRender;
class CompGraphic : public Component
{
public:
	CompGraphic(Comp_Type t, GameObject* parent);
	CompGraphic(const CompGraphic& copy, GameObject* parent);
	~CompGraphic();
	void AddCanvasRender();

	void SetTextureID(uint set_texture_id);

	uint GetTextureID()const;
	CompRectTransform* GetRectTrasnform()const;
	CompCanvasRender* GetCanvasRender()const;
private:
public:
protected:
	CompCanvasRender* my_canvas_render = nullptr;
	CompRectTransform* transform = nullptr;
	uint texture_id = 0;
};

#endif//COMPONENT_GRAPHIC_H
