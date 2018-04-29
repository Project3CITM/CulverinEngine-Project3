#ifndef COMPONENT_SLIDER_H
#define COMPONENT_SLIDER_H
#include "CompInteractive.h"

class CompSlider : public CompInteractive
{

public:
	CompSlider(Comp_Type t, GameObject* parent);
	CompSlider(const CompImage& copy, GameObject* parent);
	~CompSlider();

	void ShowOptions();
	void ShowInspectorInfo();
	void SetNewPositions();
	void SetSlideBarPos();
	void SetSlideBarBall();
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	uint SaveSliderCompUID(CompImage *img) const;

	void Load(const JSON_Object * object, std::string name);

	void SyncComponent(GameObject * sync_parent);

	void SyncSliderComponents(GameObject* sync_parent);
	void SyncBar();
	void OnDrag(Event event_input);
	void SetSliderBg(CompImage*bg);
	void SetSliderBar(CompImage*bar);


private:
	
public:

private:
	std::vector<CompScript*> linked_scripts;
	float min_pos = 0;
	float max_pos = 0;
	CompImage * slide_bg = nullptr;
	uint uuid_reimported_slide_bg = 0;

	CompImage * slide_bar = nullptr;
	uint uuid_reimported_slide_bar = 0;
	float speed = 1.0f;
	float fill = 0.5f;
};

#endif