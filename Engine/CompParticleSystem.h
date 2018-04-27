#ifndef _COMPONENT_PARTICLE_SYSTEM_
#define _COMPONENT_PARTICLE_SYSTEM_

#include "Component.h"
#include "parson.h"
#include "Color.h"
#include "MathGeoLib.h"

class ParticleSystem;
class ResourceMaterial;
struct ParticleState;
struct ParticleTextureData;
class ParticleEmitter;

class CompParticleSystem :public Component
{
public:
	CompParticleSystem(Comp_Type t, GameObject* parent);
	CompParticleSystem(const CompParticleSystem& copy, GameObject* parent);
	~CompParticleSystem();


	void Update(float dt);
	void Clear();
	void Draw();

	// LOAD - SAVE METHODS ------------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);

	// EDITOR METHODS -----------------
	void ShowOptions();
	void ShowInspectorInfo();


	void CopyComponent();

	void ImGuiLoadPopUp();
	void ImGuiLoadTexturePopUp();
	void ImGuiLoadParticlePopUp();
	void ImGuiLoadEmitterPopUp();
	void ImGuiLoadMeshPopUp();
	void ImGuiSavePopUp();
	void ImGuiSaveParticlePopUp()const;
	void ImGuiSaveEmitterPopUp()const;


	void ActivateEmitter(bool);
	float GetDiscardDistance() const;
	bool IsActive() const;
	bool IsPreview() const;

	void SetSystemAsNull();

private:


	void SetTextureResource(uint uuid, int columns = 1, int rows = 1, int numberOfFrames = 1, uint AnimationOrder = 0);
	void SetTextureResource(const char* Path, int columns = 1, int rows = 1, int numberOfFrames = 1, uint AnimationOrder = 0);



	void DrawDirectory(const char* directory);

	/*DEBUG FUNCS*/
	void SetDebugOptions(bool ShowEmitterBoundBox, bool ShowEmitter);
	void GetDebugOptions(bool& ShowEmitterBoundBox, bool& ShowEmitter) const;


	bool SaveParticleStates(ResourceMaterial* TextureResource, const ParticleTextureData* TexData, const ParticleState* stateI, const ParticleState* stateF) const;
	bool LoadParticleStates(const char* file_name, CompParticleSystem* system, ParticleState& stateI, ParticleState& stateF);

	bool SaveParticleEmitter(const CompParticleSystem* system, const ParticleEmitter* emitter) const;
	bool LoadParticleEmitter(const char* file_name, CompParticleSystem* system, ParticleEmitter& emitter);

	/*JSON FUNCS*/
	bool SetInt(JSON_Object* conf, const char* field, int value) const;
	bool SetUInt(JSON_Object* conf, const char* field, uint value) const;
	bool SetFloat(JSON_Object* conf, const char* field, float value) const;
	bool SetDouble(JSON_Object* conf, const char* field, double value) const;
	bool SetBool(JSON_Object* conf, const char* field, bool value) const;
	bool SetString(JSON_Object* conf, const char* field, const char* value) const;
	bool SetFloat2(JSON_Object* conf, const char* field, float2 value) const;
	bool SetFloat3(JSON_Object* conf, const char* field, float3 value) const;
	bool SetFloat4(JSON_Object* conf, const char* field, float4 value) const;
	bool SetFloat4x4(JSON_Object* conf, const char* field, float4x4 value) const;
	bool SetQuat(JSON_Object* conf, const char* field, Quat value) const;
	bool SetColor(JSON_Object* conf, const char* field, Color color) const;

	int GetInt(JSON_Object* conf, const char* field, int default = 0) const;
	uint GetUInt(JSON_Object* conf, const char* field, uint default = 0) const;
	float GetFloat(JSON_Object* conf, const char* field, float default = 0.0f) const;
	double GetDouble(JSON_Object* conf, const char* field, double default = 0.0) const;
	bool GetBool(JSON_Object* conf, const char* field, bool default = false) const;
	const char* GetString(JSON_Object* conf, const char* field, const char* default = "") const;
	float2 GetFloat2(JSON_Object* conf, const char* field, float2 default = float2::zero) const;
	float3 GetFloat3(JSON_Object* conf, const char* field, float3 default = float3::zero) const;
	float4 GetFloat4(JSON_Object* conf, const char* field, float4 default = float4::zero) const;
	float4x4 GetFloat4x4(JSON_Object* conf, const char* field, float4x4 default = float4x4::identity) const;
	Quat GetQuat(JSON_Object* conf, const char* field, Quat default = Quat::identity) const;
	Color GetColor(JSON_Object* conf, const char* field, Color default = White) const;

private:



	ParticleSystem* part_system = nullptr;
	
	bool pop_up_load_open = false;
	bool pop_up_save_open = false;
	std::string file_to_load;
	std::string file_to_load_name;
	std::string file_to_save;
	std::string file_to_save_name;
	std::string directory_temporal_str;


	std::string particle_resource_name;
	std::string emitter_resource_name;

	


	enum
	{
		Texture_Resource,
		Particle_Resource,
		Emitter_Resource,
		MeshResource
	} file_type = Texture_Resource;


	
};
#endif //_COMPONENT_PARTICLE_SYSTEM_