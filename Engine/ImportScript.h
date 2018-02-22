#ifndef _IMPORTSCRIPT_
#define _IMPORTSCRIPT_

// Info-> http://docs.go-mono.com/?link=root:/embed

#include "Globals.h"
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>
#include <list>
#include <map>

class CSharpScript;
class ResourceScript;
class GameObject;

class ImportScript
{
public:
	ImportScript();
	~ImportScript();

	bool InitScriptingSystem();
	void ShutdownMono();

	bool Import(const char* file, uint uuid = 0, bool isAutoImport = false);
	bool LoadResource(const char* file, ResourceScript* resourceScript);
	bool ReImportScript(std::string fileAssets, std::string uid_script, ResourceScript * resourceScript);
	MonoDomain* Load_domain();
	void Unload_domain();
	bool CreateNewScript(bool & active);
	int CompileScript(const char* file, std::string& libraryScript, const char* uid);
	CSharpScript* LoadScript_CSharp(std::string);
	CSharpScript* CreateCSharp(MonoImage* image);
	MonoClass* GetMonoClassFromImage(MonoImage* image, std::string& name_space, std::string& classname);

	MonoDomain* GetMainDomain() const;
	MonoDomain* GetDomain() const;
	MonoImage* GetCulverinImage() const;
	std::string GetMonoPath() const;

	void SetCurrentScript(CSharpScript* current);

	// Map <MonoObject, GameObject>
	void SetMonoMap(GameObject* root, bool is_root = false); // Only used in "Play"
	void ClearMonoMap();
	void UpdateMonoMap(GameObject* modificate);
	void UpdateMonoMap(GameObject* modificate, MonoObject* object);
	MonoObject* GetMonoObject(GameObject* gameobject);
	GameObject* GetGameObject(MonoObject* monoobject);

	GameObject* GetGameObjectLink(std::string name);


	bool IsNameUnique(std::string name) const;

private:
	void LinkFunctions();

	// FUNCTIONS ---------
	/* Debug - Console */
	static void ConsoleLog(MonoString* string);

	/* Input */
	static mono_bool	GetKeyDown(int key);
	static mono_bool	GetKeyUp(int key);
	static mono_bool	GetKeyRepeat(int key);
	static mono_bool	GetMouseButtonDown(int buttonmouse);
	static mono_bool	GetMouseButtonUp(int buttonmouse);
	static mono_bool	GetMouseButtonRepeat(int buttonmouse);
	static MonoObject*	GetMousePosition();
	static int			GetMouseXAxis();
	static int			GetMouseYAxis();
	static int			GetMouseMoutionX();
	static int			GetMouseMoutionY();

	/* Time */
	static float GetDeltaTime();

	/* Link */
	static MonoObject* GetLinkedObject(MonoObject* object, MonoString* name);

	/* GameObject */
	static mono_bool	IsStatic(MonoObject* object);
	static mono_bool	IsActive(MonoObject* object);
	static void			SetActive(MonoObject* object, mono_bool active);
	static MonoObject*	Find(MonoObject* object, MonoString* name);
	static MonoObject*	GetOwnGameObject();
	static int			ChildCount(MonoObject* object);
	static MonoObject*	GetChildByIndex(MonoObject* object, int index);
	static MonoObject*	GetChildByName(MonoObject* object, MonoString* name);
	static void			SetName(MonoObject* object, MonoString* name);
	static MonoString*	GetName(MonoObject* object);
	static MonoString*	GetTag(MonoObject* object);
	static void			SetTag(MonoObject* object, MonoString* name);
	static mono_bool	CompareTag(MonoObject* object, MonoString* tag);
	static MonoObject*	FindGameObjectWithTag(MonoObject* object, MonoString* tag);
	static void			CreateGameObject(MonoObject* object);
	static void			DeleteGameObject(MonoObject* object);
	static MonoObject*	GetComponent(MonoObject* object, MonoReflectionType* type);
	
	/*Transform*/
	static MonoObject*	GetForwardVector(MonoObject* object);
	static MonoObject*	GetBackwardVector(MonoObject* object);
	static MonoObject*	GetUpVector(MonoObject* object);
	static MonoObject*	GetDownVector(MonoObject* object);
	static MonoObject*	GetRightVector(MonoObject* object);
	static MonoObject*	GetLeftVector(MonoObject* object);
	static void			SetForwardVector(MonoObject* object, MonoObject* vector3);
	static void			SetBackwardVector(MonoObject* object, MonoObject* vector3);
	static void			SetUpVector(MonoObject* object, MonoObject* vector3);
	static void			SetDownVector(MonoObject* object, MonoObject* vector3);
	static void			SetRightVector(MonoObject* object, MonoObject* vector3);
	static void			SetLeftVector(MonoObject* object, MonoObject* vector3);
	static MonoObject*	GetPosition(MonoObject* object);
	static MonoObject*	GetGlobalPosition(MonoObject* object);
	static void			SetPosition(MonoObject* object, MonoObject* vector3);
	static void			SetGlobalPosition(MonoObject* object, MonoObject* vector3);
	static void			Translate(MonoObject* object, MonoObject* vector3);
	static MonoObject*	GetRotation(MonoObject* object);
	static MonoObject*	GetGlobalRotation(MonoObject* object);
	static void			SetRotation(MonoObject* object, MonoObject* vector3);
	static void			SetGlobalRotation(MonoObject* object, MonoObject* vector3);
	static void			IncrementRotation(MonoObject* object, MonoObject* vector3);
	static void			RotateAroundAxis(MonoObject* object, MonoObject* vector3, float angle);
	static void			SetScale(MonoObject* object, MonoObject* vector3);
	static MonoObject*	GetScale(MonoObject* object);
	static void			LookAt(MonoObject* object, MonoObject* vector3);
	static void			LookAtTrans(MonoObject* object, MonoObject* trans);

	/* Component */

	static MonoObject* GetParentGameObject();

	/* Map */
	static MonoString* GetMapString(MonoObject* object);
	static int GetHeightMap();
	static int GetWidthMap();

	/*Audio*/

	static void StopAllSounds();
	static void PauseAllSounds();
	static void ResumeAllSounds();
	static void ChangeRTPC(MonoString* var_name, float value);
	static void ChangeState(MonoString* group_name, MonoString* state_name);
	static void ChangeVolume(float volume);
	static void Mute(bool m);

	/*Component Audio*/
	static void PlayAudioEvent(MonoObject* object, MonoString* name);
	static void StopAudioEvent(MonoObject* object, MonoString* name);
	static void SetAuxiliarySends(MonoObject* object, MonoString* bus, float value);

	/*Component Interactive*/
	static void Activate(MonoObject* object, int uid);
	static void Deactivate(MonoObject* object, int uid);

	/*Component Interactive Button*/
	static void Clicked(MonoObject * object);

	/*Component Graphic*/
	static void SetRaycastTarget(MonoObject * object, mono_bool flag);
	/*Component Graphic Image*/
	static void FillAmount(MonoObject * object, float value);

	/*Component Collier*/
	static MonoObject* GetCollidedObject(MonoObject * object);

public: 
	std::map<std::string, GameObject*> map_link_variables;

private:
	std::string nameNewScript;
	std::string mono_path;
	MonoDomain* domain = nullptr;
	MonoDomain* childDomain = nullptr;
	MonoImage* culverin_mono_image = nullptr;
	std::list<std::string> nameScripts;
	static CSharpScript* current;

	std::map<MonoObject*, GameObject*> mono_map;

};

#endif