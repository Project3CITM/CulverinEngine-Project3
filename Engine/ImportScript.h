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
#include "MathGeoLib.h"

class CSharpScript;
class ResourceScript;
class GameObject;
class Component;
class Material;

class ImportScript
{
public:
	ImportScript();
	~ImportScript();
	void Clear();

	bool InitScriptingSystem();
	void ShutdownMono();

	bool Import(const char* file, uint uuid = 0, bool isAutoImport = false);
	bool LoadResource(const char* file, ResourceScript* resourceScript);
	bool ReImportScript(std::string fileAssets, std::string uid_script, ResourceScript * resourceScript, bool auto_reimport = false);
	MonoDomain* Load_domain();
	void Unload_domain();
	bool CreateNewScript(bool & active);
	int CompileScript(const char* file, std::string& libraryScript, const char* uid);
	CSharpScript* LoadScript_CSharp(std::string, std::string name = NULL);
	CSharpScript* CreateCSharp(MonoImage* image, std::string nameClass = NULL);
	MonoClass* GetMonoClassFromImage(MonoImage* image, std::string& name_space, std::string& classname);

	MonoDomain* GetMainDomain() const;
	MonoDomain* GetDomain() const;
	MonoImage* GetCulverinImage() const;
	std::string GetMonoPath() const;

	void SetCurrentScript(CSharpScript* current);

	// Map <MonoObject, GameObject>
	void SetMonoMap(GameObject* root, bool is_root = false); // Only used in "Play"
	void ClearLinkVariables();
	void ClearMonoMap();
	void UpdateMonoMap(GameObject* modificate, bool do_delete = false);
	void UpdateMonoMap(GameObject* modificate, MonoObject* object);
	MonoObject* GetMonoObject(GameObject* gameobject);
	GameObject* GetGameObject(MonoObject* monoobject);

	void UpdateMonoComp(Component* modificate, uint csreference);
	MonoObject* GetMonoObject(Component* component);
	Component* GetComponentMono(MonoObject* monoobject);

	void UpdateMonoPos(float3* pos, uint csreference);
	MonoObject* GetMonoObject(float3* pos);
	float3& GetPosMono(MonoObject* monoobject);

	void UpdateMonoScript(CSharpScript* script, uint csreference);
	MonoObject* GetMonoObject(CSharpScript* script);
	CSharpScript* GetScriptMono(MonoObject* monoobject);

	void UpdateMonoMaterial(Material* modificate, uint csreference);
	MonoObject* GetMonoObject(Material* script);
	Material* GetMaterialMono(MonoObject* monoobject);

	//Delete Functions -----------------------
	void RemoveGObjectVarFromScripting(GameObject* object);

	void RemoveGObjectFromMonoMap(GameObject*object);
	void RemoveComponentFromMonoList(Component* comp);
	void RemoveTransformPosPointerFromMap(float3* pospointer);
	void RemoveCSharpScriptFromMonoScript(CSharpScript* script);
	void RemoveGObjectReferencesFromMonoScript(GameObject* object);

	bool IsNameUnique(std::string name) const;

private:
	void LinkFunctions();

	// FUNCTIONS ---------
	/* Debug - Console */
	static void ConsoleLog(MonoObject* string);

	/* Scene Management */
	static void LoadScene(MonoString* scene_name);
	static void LoadSceneNoDestroy(MonoString* scene_name);
	static bool CheckSceneReady();
	static void RemoveNoDestroy();
	static void QuitScene();
	/* Scene Management */
	static void SendInteractiveSelected(MonoObject* interactive);
	/* Input */
	static mono_bool	GetPressAnyKey();
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
	static void			SetInputManagerActive(MonoString* str, mono_bool active);
	static void			SetInputManagerBlock(MonoString* str, mono_bool active);
	static mono_bool	GetInput_KeyDown(MonoString* name, MonoString* input);
	static mono_bool	GetInput_KeyUp(MonoString* name, MonoString* input);
	static mono_bool	GetInput_KeyRepeat(MonoString* name, MonoString* input);
	static mono_bool	GetInput_MouseButtonDown(MonoString* name, MonoString* input);
	static mono_bool	GetInput_MouseButtonUp(MonoString* name, MonoString* input);
	static float		GetInput_ControllerAxis(MonoString* name, MonoString* input);
	static void			RumblePlay(float intensity, int milliseconds);

	/* Time */
	static float GetDeltaTime();
	static float RealtimeSinceStartup();
	static float TimeScale();
	static void TimeScaleSet(float time);
	static void StartTimePlay(float time);
	static float TimePlay();

	/* CulverinBehaciour */
	static MonoObject*	GetLinkedObject(MonoObject* object, MonoString* name);
	static bool			GetEnabled(MonoObject* object, MonoObject* gameobject);
	static void			SetEnabled(MonoObject* object, mono_bool active, MonoObject* gameobject);

	/* GameObject */
	static mono_bool	IsStatic(MonoObject* object);
	static mono_bool	IsActive(MonoObject* object);
	static void			SetActive(MonoObject* object, mono_bool active);
	static MonoObject*	Find(MonoObject* object, MonoString* name);
	static MonoObject*	GetOwnGameObject();
	static int			ChildCount(MonoObject* object);
	static MonoObject*	GetChildByIndex(MonoObject* object, int index);
	static MonoObject*	GetChildByName(MonoObject* object, MonoString* name);
	static MonoObject*	GetChildByTagIndex(MonoObject* object, MonoString* tag, int index);
	static void			SetName(MonoObject* object, MonoString* name);
	static MonoString*	GetName(MonoObject* object);
	static MonoString*	GetTag(MonoObject* object);
	static void			SetTag(MonoObject* object, MonoString* name);
	static mono_bool	CompareTag(MonoObject* object, MonoString* tag);
	static MonoObject*	FindGameObjectWithTag(MonoObject* object, MonoString* tag);
	static void			CreateGameObject(MonoObject* object);
	static void			DeleteGameObject(MonoObject* object);
	static MonoObject*	GetComponent(MonoObject* object, MonoReflectionType* type);

	/* Object */
	static MonoObject*	Instantiate(MonoObject* object, MonoString* prefab);
	static MonoObject*	Instantiate_respawn(MonoObject* object, MonoString* prefab, float time);
	static void			Destroy(MonoObject* object, MonoObject* gameobject, float time);
	
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
	static MonoObject*	GetLocalTransform(MonoObject* object);
	static MonoObject*	GetGlobalTransform(MonoObject* object);

	/* Component */

	static MonoObject*	GetParentGameObject(MonoObject* object);

	/* Map */
	static MonoString* GetMapString(MonoObject* object);
	static int GetHeightMap();
	static int GetWidthMap();
	static float GetSeparation();

	/*Audio*/

	static void StopAllSounds();
	static void PauseAllSounds();
	static void ResumeAllSounds();
	static void ChangeRTPC(MonoString* var_name, float value);
	static void ChangeState(MonoString* group_name, MonoString* state_name);
	static void ChangeVolume(float volume);
	static void Mute(bool m);

	/*Component Particle*/
	static void ActivateEmission(MonoObject* obj, bool active);
	static bool IsEmitterActive(MonoObject* obj);

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
	static void SetRender(MonoObject * object, mono_bool flag);
	static void ActivateRender(MonoObject * object);
	static void DeactivateRender(MonoObject * object);
	/*Component Text*/
	static void SetAlpha(MonoObject* object, float alpha);
	static void SetText(MonoObject* object, MonoString* alpha);

	
	/*Component Graphic Image*/
	static void FillAmount(MonoObject* object, float value);
	static void SetColor(MonoObject* object, MonoObject* color, float alpha);

	/*Component Collier*/
	static MonoObject*	GetCollidedObject(MonoObject * object);
	static MonoObject*	GetContactPoint(MonoObject* object);
	static MonoObject*	GetContactNormal(MonoObject* object);
	static void			MoveStaticColliderTo(MonoObject* object, MonoObject* position);
	static void			CallOnContact(MonoObject* object);
	static void			CallOnTriggerEnter(MonoObject* object);
	static void			CollisionActive(MonoObject* object, bool active);

	/*Component RigidBody*/
	static MonoObject*	GetColliderPosition(MonoObject* object);
	static MonoObject*	GetColliderQuaternion(MonoObject* object);
	static void			RemoveJoint(MonoObject* object);
	static void			MoveKinematic(MonoObject* object, MonoObject* position, MonoObject* rotation);
	static void			ApplyForce(MonoObject* object, MonoObject* force);
	static void			ApplyImpulse(MonoObject* object, MonoObject* impulse);
	static void			ApplyTorqueForce(MonoObject* object, MonoObject* force);
	static void			ApplyTorqueImpulse(MonoObject* object, MonoObject* impulse);
	static void			LockMotion(MonoObject* object);
	static void			LockRotation(MonoObject* object);
	static void			LockTransform(MonoObject* object);
	static void			UnLockMotion(MonoObject* object);
	static void			UnLockRotation(MonoObject* object);
	static void			UnLockTransform(MonoObject* object);
	static void			ResetForce(MonoObject* object);
	static void			WakeUp(MonoObject* object);
	static void			SetAtMaxJointPose(MonoObject* object);

	/*Component Joint*/
	static void		   DeleteJoint(MonoObject* object);

	/*Component Animation*/
	static void PlayAnimation(MonoObject* object, MonoString* string, mono_bool blending);
	static void SetTransition(MonoObject* object, MonoString* string, mono_bool condition);
	static mono_bool IsAnimationStopped(MonoObject* object, MonoString* string);
	static mono_bool IsAnimationRunning(MonoObject* object, MonoString* string);
	static mono_bool IsAnimOverXTime(MonoObject* object, float num_between_0_1);
	static void SetClipsSpeed(MonoObject* object, float speed_value);
	static float GetClipDuration(MonoObject* object, MonoString* string);
	static void SetClipDuration(MonoObject* object, MonoString* string, float duration);
	static void SetActiveBlendingClip(MonoObject* object, MonoString* string);
	static void SetActiveBlendingClipWeight(MonoObject* object, float weight);
	static void SetBlendInTime(MonoObject* object, MonoString* string, float weight);
	static void PlayAnimationNode(MonoObject* object, MonoString* string);

	/*Component Material*/
	static void SetAlbedo(MonoObject* object, MonoString* string);
	static void SetNormals(MonoObject* object, MonoString* string);
	static void SetAmbientOcclusion(MonoObject* object, MonoString* string);

	/*Component Light*/
	static float GetIntensity(MonoObject* object);
	static void SetIntensity(MonoObject* object, float value);
	static float GetConstant(MonoObject* object);
	static void SetConstant(MonoObject* object, float value);
	static float GetLinear(MonoObject* object);
	static void SetLinear(MonoObject* object, float value);
	static float GetQuadratic(MonoObject* object);
	static void SetQuadratic(MonoObject* object, float value);

	/*Module Physics*/
	static MonoObject*	RayCast(MonoObject* origin, MonoObject* direction, float distance);

	/*Material*/
	static void	SetBool(MonoObject* object, MonoString* name, bool value);
	static void	SetFloat(MonoObject* object, MonoString* name, float value);
	static MonoObject* GetMaterialByName(MonoObject* object, MonoString* name);

	/*Random*/
	// min [inclusive] - max [inclusive]
	static int RangeInt(int min, int max); 
	// min [inclusive] - max [exclusive]
	static float RangeFloat(float min, float max);

public: 
	std::multimap<std::string, GameObject*> map_link_variables;
private:
	std::string nameNewScript;
	std::string mono_path;
	MonoDomain* domain = nullptr;
	MonoDomain* childDomain = nullptr;
	MonoImage* culverin_mono_image = nullptr;
	std::list<std::string> nameScripts;
	static CSharpScript* current;

	std::multimap<uint, GameObject*> mono_map;
	std::multimap<uint, Component*> mono_comp;
	std::multimap<uint, float3*> mono_pos;
	std::multimap<uint, CSharpScript*> mono_script;
	std::map<uint, Quat*> mono_quat;
	std::multimap<uint, Material*> mono_material;

};

#endif