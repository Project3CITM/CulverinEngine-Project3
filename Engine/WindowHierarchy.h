#ifndef __MODULEHIERARCHY_H__
#define __MODULEHIERARCHY_H__

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"
#include "WindowManager.h"

class GameObject;

class Hierarchy : public WindowManager
{
public:

	Hierarchy();
	virtual ~Hierarchy();

	bool Start();
	update_status Update(float dt);
	void ShowHierarchy();
	void ShowOptions();
	void ShowDeleteWindow();

	// GameObjects* -----------
	void SetGameObjecttoDelete(GameObject* todelete);
	void SetGameObjectCopy(GameObject* copy);
	void CopyGameObject(GameObject * copy_);
	const GameObject* GetCopied() const;
	GameObject* GetSelected();
	void SetGameObjectSelected(GameObject* select);
	void ChangeShowConfirmDelete();
	bool CleanUp();

public:

	bool show_confirm_delete = true;

private:

	std::string model_name = "";
	bool have_model = false;
	GameObject* to_delete = nullptr;
	GameObject* selected = nullptr;
	GameObject* copy = nullptr;
	bool wait_to_select = false;
	bool dont_ask_me_next_time = false;

};
#endif //__MODULEHIERARCHY_H__