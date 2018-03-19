#ifndef _SCENE_
#define _SCENE_

#include "Module.h"
#include "Globals.h"
#include "ModuleFramebuffers.h"
#include "CompMesh.h"
#include "Quadtree.h"
#include "Octree.h"
#include <vector>

class GameObject;
class SkyBox;
//Event system test
union Event;

class Scene : public Module
{
public:
	Scene(bool start_enabled = false);
	virtual ~Scene();

	//bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	//update_status PostUpdate();
	update_status UpdateConfig(float dt);
	bool CleanUp();

	//Scene Loading
	void LoadScene();

	//Event system test
	bool SetEventListeners();
	void OnEvent(Event& event);

	// Editor outputs 
	void EditorQuadtree();
	void EditorSkybox();

	// Scripting utility functions
	bool CheckNoFails();
	void StartScripts();

	GameObject* GetGameObjectfromScene(bool& active);
	GameObject* GetGameObjectbyuid(uint uid);
	GameObject* FindGameObjectWithTag(const char* str);
	void		FindGameObjectsWithTag(const char* tag, std::vector<GameObject*>* vec); //You get a pointer to the vec not a new one!

	void						TagWindow();
	void						DeleteObjectsTag(const char* tag);
	bool						FindTag(const char* tag)const;
	uint						TagsSize()const;
	void						AddTag(const char* str);
	void						DeleteTag(const char* str, uint index = 0);
	void						RemoveTaggedObject(const GameObject* target);
	void						AddTaggedObject(const GameObject* target);
	uint						GetTagID(const char* str);
	std::vector<std::string>*	GetTagsVec();

	void ModificateParent(GameObject* child, GameObject* new_parent);

	
	// DRAWING METHODS ---------
	void DrawPlane();
	void DrawCube(float size);

	// CULLING HELPER FUNCTION -----------
	//void FillStaticObjectsVector(bool fill);
	void RecalculateStaticObjects();

	//OBJECTS CREATION / DELETION ---------------------
	GameObject* FindCanvas();
	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateCube(GameObject* parent = nullptr);
	GameObject* CreatePlane(GameObject* parent = nullptr);
	GameObject* CreateMainCamera(GameObject* parent = nullptr);
	GameObject* CreateMainLight(GameObject* parent = nullptr);
	GameObject* CreateCanvas(GameObject* parent = nullptr);
	GameObject* CreateImage(GameObject* parent = nullptr);
	GameObject* CreateButton(GameObject* parent = nullptr);
	GameObject* CreateCheckBox(GameObject* parent = nullptr);
	GameObject * CreateSlider(GameObject * parent = nullptr);
	GameObject* CreateText(GameObject* parent = nullptr);
	GameObject* CreateEditText(GameObject* parent = nullptr);

	void DeleteAllGameObjects(GameObject* gameobject, bool isMain = true, bool is_reimport = false);
	void DeleteGameObject(GameObject* gameobject, bool isImport = false, bool is_reimport = false);
	// -------------------------------------

	// UI ----------------------------------
	Component* BlitSceneComponentsAsButtons(Comp_Type type, std::string& current_item);
	// -------------------------------------

public:
	FrameBuffer* scene_buff = nullptr;

	//Container of all Game Objects
	GameObject* root = nullptr;

	//Container Vector of Static Objects (to speeding searches with quadtree)
	std::list<GameObject*> static_objects;

	// Scene Saved
	bool scene_saved = true; // TODO XAVI - need implementation with Event System

	// Quadtree ----------------
	//Quadtree quadtree;
	Octree octree;
	bool quadtree_draw = false;
	// -------------------------

	// Skybox --------------------
	SkyBox* skybox = nullptr;
	int skybox_index = 0;
	bool draw_skybox = true;
	// ---------------------------

	GameObject* drag = nullptr;
	uint icon_options_transform = 0;

private:

	int		size_plane = 0;
	float	size_quadtree = 0.0f;
	uint	icon_resetAll = 0;
	bool	load_scene = true;
	std::vector<Component*> temp_vector;
	Comp_Type				actualType = Comp_Type::C_UNKNOWN;
	bool					on_tag_creation = false;
	bool					on_tag_delete = false;
	char					tag_buffer[100];
	std::vector<std::string>				defined_tags;
	std::vector<std::vector<GameObject*>*>	tagged_objects;
};

#endif