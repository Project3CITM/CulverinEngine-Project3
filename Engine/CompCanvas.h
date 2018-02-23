#ifndef COMPONENT_CANVAS_H
#define COMPONENT_CANVAS_H

#include "Component.h"
#include "EventDef.h"
#include <vector>
class CompCanvasRender;
class CompRectTransform;
class CompGraphic;
class CompCanvas:public Component
{
public:
	CompCanvas(Comp_Type t, GameObject* parent);
	CompCanvas(const CompCanvas& copy, GameObject* parent);
	~CompCanvas();
	void PreUpdate(float dt);
	void Update(float dt);
	void Clear();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCanvas * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);


	void SyncComponent();

	void UpdateCanvasScale();

	void AddGraphic(CompGraphic* to_add);
	void RemoveGraphic(CompGraphic* to_remove);
	void DrawDebugRectTransform();
	void DrawGraphic();
	void SetDefaultTexture(int texture);
	int GetDefaultTexture()const;
private:
public:
private:
	Event draw_mode;
	int default_texture_id;
	CompRectTransform* my_transform = nullptr;
	std::vector<CompGraphic*> graphic_vector;

};

#endif // !COMPONENT_CANVAS_H
