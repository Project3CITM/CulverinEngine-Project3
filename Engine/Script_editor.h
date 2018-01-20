#ifndef _SCRIPT_EDITOR_H_
#define _SCRIPT_EDITOR_H_

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"
#include "WindowManager.h"
#include "TextEditor.h"

class ResourceScript;

enum ActionEditor
{
	COPY,
	PASTE,
	UNDO,
	REUNDO,
	RETURN
};

class Script_editor
{
public:
	Script_editor(ResourceScript* parent);
	~Script_editor();

	void Start(std::string nameScript, bool isnew = true);
	void SetInitInfo(std::string nameScript);
	void Show(bool& active);

	void SetAction(ActionEditor action);
	void SaveScript(bool ReImport = false);

public:
	TextEditor editor;
	std::string textSelected;
	std::string name;
	std::string directory;

private:
	ResourceScript* parent;
};


#endif
