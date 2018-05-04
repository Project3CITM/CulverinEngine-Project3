#ifndef COMPONENT_CHECK_BOX_H
#define COMPONENT_CHECK_BOX_H
#include "CompInteractive.h"
#include "ClickAction.h"

#include <vector>

class CompScript;

class CompCheckBox:public CompInteractive, public ClickAction
{
public:
	CompCheckBox(Comp_Type t, GameObject* parent);
	CompCheckBox(const CompCheckBox& copy, GameObject* parent);
	~CompCheckBox();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCheckBox * component);

	// Save & Load ----------------------------------------------------------------------
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	void GetOwnBufferSize(uint& buffer_size);
	void SaveBinary(char** cursor, int position) const;
	void LoadBinary(char** cursor);
	//----------------------------------------------------------------------------------

	void SyncComponent(GameObject * sync_parent);
	void SyncScript();
	void OnPointDown(Event event_input);
	void OnClick();
	void ClearLinkedScripts();
	void SetTick(CompImage* tick);
	void OnSubmit(Event event_input);

	void HideTick();

private:
	CompImage * tick = nullptr;
	std::vector<CompScript*> linked_scripts;
	bool active = false;
	uint tick_uid;
};

#endif//COMPONENT_CHECK_BOX_H
