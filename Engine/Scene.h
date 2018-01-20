#ifndef _SCENE_
#define _SCENE_

#include "Module.h"
#include "Globals.h"
#include "ModuleFramebuffers.h"
#include "CompMesh.h"
#include "Quadtree.h"
#include <vector>

class GameObject;
class SkyBox;

class Scene : public Module
{
public:
	Scene(bool start_enabled = false);
	virtual ~Scene();

	//bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	//update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool CleanUp();

	// Editor outputs ----
	void EditorQuadtree();
	void EditorSkybox();

	// Scripting utility functions
	bool CheckNoFails();
	void StartScripts();
	void ClearAllVariablesScript();

	//Remove references of that gameobject in all scripts
	void SetScriptVariablesToNull(GameObject* go);

	GameObject* GetGameObjectfromScene(bool& active);
	GameObject* GetGameObjectbyuid(uint uid);

	// DRAWING METHODS ---------
	void DrawPlane();
	void DrawCube(float size);

	// CULLING HELPER FUNCTION -----------
	void FillStaticObjectsVector(bool fill);

	//OBJECTS CREATION / DELETION ---------------------
	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateCube(GameObject* parent = nullptr);
	GameObject* CreateMainCamera(GameObject* parent = nullptr);
	void DeleteGameObjects(std::vector<GameObject*>& gameobjects, bool isMain = true);
	void DeleteGameObject(GameObject* gameobject, bool isImport = false);
	// -------------------------------------

public:
	FrameBuffer* sceneBuff = nullptr;

	//Container Vector of all Game Objects
	std::vector<GameObject*> gameobjects;
	// std::vector<GameObject*> gameobjects to GameObject* gameobjects
	GameObject* gameobjects_inscene = nullptr; //Rename to gameobjects

	//Container Vector of Static Objects (to speeding searches with quadtree)
	std::vector<GameObject*> static_objects;

	// Quadtree ----------------
	Quadtree quadtree;
	bool quadtree_draw = false;
	// -------------------------

	// Skybox --------------------
	SkyBox* skybox = nullptr;
	int skybox_index = 0;
	bool draw_skybox = true;
	// ---------------------------

	GameObject* drag = nullptr;
	GLuint icon_options_transform = 0;

private:
	int size_plane = 0;
	float size_quadtree = 0.0f;
};

#endif