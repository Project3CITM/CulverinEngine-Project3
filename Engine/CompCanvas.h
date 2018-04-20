#ifndef COMPONENT_CANVAS_H
#define COMPONENT_CANVAS_H

#include "Component.h"
#include "EventDefV2.h"
#include <vector>
class ShaderProgram;

class CompCanvasRender;
class CompRectTransform;
class CompGraphic;
class CompCanvas :public Component
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
	void Resize(int width, int height);

	void SyncComponent(GameObject* sync_parent);

	void UpdateCanvasScale();

	void AddGraphic(CompGraphic* to_add);
	void RemoveGraphic(CompGraphic* to_remove);
	void DrawDebugRectTransform();
	void DrawGraphic(bool debug = false);

	void ClearGraphicVector();

	void SetDefaultUIShader(ShaderProgram* shader);
	void SetDefaultTexture(int texture);
	int GetDefaultTexture()const;
	void SetNewCanvasAlphaValue(float value);
private:
	void CorrectCanvasAlpha();
	void SetCanvasAlpha(bool alpha_down);
public:
	enum DisplayMode
	{
		SCREEN_VIEW_MODE,
		WORLD_VIEW_MODE
	};
private:
	DisplayMode display = DisplayMode::SCREEN_VIEW_MODE;
	Event draw_mode;
	int default_texture_id;
	CompRectTransform* my_transform = nullptr;
	std::vector<CompGraphic*> graphic_vector;
	ShaderProgram* default_ui_shader = nullptr;
	float canvas_alpha = 1.0f;
	float current_canvas_alpha = 1.0f;
	bool first_update = true;

};

#endif // !COMPONENT_CANVAS_H
