#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib.h"
#include <string>
#include <vector>

#define Width_AddComponent 204

class CompTransform;
class CompMesh;
class CompMaterial;
class CompScript;

class GameObject
{
public:
	GameObject(GameObject* parent);
	GameObject(char* nameGameObject, uint uuid);
	GameObject(const GameObject& copy, bool haveparent = false, GameObject* parent = nullptr);
	~GameObject();

	// Play Engine
	// Return if all scripts in scene are compilated correctly.
	bool CheckScripts(int & numfails);
	void StartScripts();
	void ClearAllVariablesScript();
	GameObject* GetGameObjectbyuid(uint uid);
	GameObject* GetGameObjectfromScene(int);

	void Init();
	void preUpdate(float dt);
	void Update(float dt);
	void postUpdate();
	bool CleanUp();

	void FixedDelete(bool check_delete);
	bool isDeleteFixed() const;
	void Draw();
	bool Enable();
	bool Disable();

	// Name
	void SetName(char* name);
	const char* GetName() const;
	void NameNotRepeat(std::string& name, bool haveParent, GameObject* parent);

	// EDITOR METHODS ------------------
	void ShowHierarchy();
	void ShowGameObjectOptions();
	void ShowInspectorInfo();
	void FreezeTransforms(bool freeze, bool change_childs);
	void ShowFreezeChildsWindow(bool freeze, bool& active);
	// ----------------------------------

	void SetActive(bool active);
	void SetVisible(bool visible);
	void SetStatic(bool set_static);

	bool isActive() const;
	bool isVisible() const;
	bool isStatic() const;

	// Components -----------------------------
	Component* AddComponent(Comp_Type type, bool isFromLoader = false);
	void AddComponentCopy(const Component& copy);
	int GetNumComponents() const;
	Component* FindComponentByType(Comp_Type type) const;
	CompTransform* GetComponentTransform() const;
	CompMesh* GetComponentMesh() const;
	CompMaterial* GetComponentMaterial() const;
	CompScript * GetComponentScript() const;
	Component* GetComponentbyIndex(uint i) const;
	void DeleteAllComponents();
	void DeleteComponent(Component* component);

	void SaveComponents(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void LoadComponents(const JSON_Object * object, std::string name, uint numComponents);

	// Childs ---------------------------------
	uint GetNumChilds() const;
	GameObject* GetChildbyIndex(uint pos_inVec) const;
	GameObject* GetChildbyName(const char* name) const;
	uint GetIndexChildbyName(const char* name) const;
	void RemoveChildbyIndex(uint index);
	std::vector<GameObject*> GetChildsVec() const;
	std::vector<GameObject*>* GetChildsPtr();
	void AddChildGameObject_Copy(const GameObject* child);
	void AddChildGameObject_Load(GameObject* child);
	void AddChildGameObject_Replace(GameObject* child);

	// Transform Modifications -----------------
	void UpdateChildsMatrices();

	// Parent ----------------
	GameObject* GetParent() const; //Not const pointer to enable parent variables modification
	bool HaveParent();

	// Bounding Box -----------------------
	void AddBoundingBox(const ResourceMesh* mesh);
	void DrawBoundingBox();
	AABB* bounding_box = nullptr;
	AABB  box_fixed;
	void SetAABBActive(bool active);
	bool isAABBActive() const;

	uint GetUUID() const;
	void SetUUID(uint uuid);
	void SetUUIDRandom();

	// Delete -----------------
	bool WanttoDelete() const;
	void SettoDelete();
	void RemoveScriptReference(GameObject* go);

	bool beingUsedByScript = false;
private:
	uint uid = 0;
	char* name = "CHANGE THIS";
	bool active = false;
	bool visible = false;
	bool static_obj = false;
	bool toDelete = false; 
	bool fixedDelete = false;
	bool bb_active = false;

	GameObject* parent = nullptr;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	

};


#endif