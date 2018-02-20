#ifndef _COMPONENT_PARTICLE_SYSTEM_
#define _COMPONENT_PARTICLE_SYSTEM_

#include "Component.h"
#include "parson.h"
#include "Color.h"

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

	void PreUpdate(float dt);
	void Update(float dt);
	void Draw();
	void Clear();

	// LOAD - SAVE METHODS ------------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);

	// EDITOR METHODS -----------------
	void ShowOptions();
	void ShowInspectorInfo();

	void ImGuiLoadPopUp();
	void ImGuiLoadTexturePopUp();
	void ImGuiLoadParticlePopUp();
	void ImGuiLoadEmitterPopUp();
	void ImGuiLoadMeshPopUp();
	void ImGuiSavePopUp();
	void ImGuiSaveParticlePopUp();
	void ImGuiSaveEmitterPopUp();

private:
	void DrawDirectory(const char* directory);



	bool SaveParticleStates(const char* file_name, const ResourceMaterial* TextureResource, const ParticleTextureData* TexData, const ParticleState* stateI, const ParticleState* stateF) const;
	bool LoadParticleStates(CompParticleSystem* system, ParticleState& stateI, ParticleState& stateF) const;

	bool SaveParticleEmitter(CompParticleSystem* system, const ParticleEmitter* emitter) const;
	bool LoadParticleEmitter(CompParticleSystem* system, ParticleEmitter& emitter) const;

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

private:

	ParticleSystem* part_system = nullptr;
	ResourceMaterial* texture_resource = nullptr;
	bool pop_up_load_open = false;
	bool pop_up_save_open = false;
	std::string file_to_load;
	std::string file_to_load_name;
	std::string file_to_save;
	std::string file_to_save_name;
	std::string directory_temporal_str;

	std::string child_particle;
	std::string child_particle_name;
	std::string child_emitter;
	std::string child_emitter_name;
	bool child_loaded = false;

	enum
	{
		Texture_Resource,
		Particle_Resource,
		Emitter_Resource,
		Child_Particle_Resource,
		Child_Emitter_Resource,
		MeshResource
	} file_type = Texture_Resource;


	
};
#endif //_COMPONENT_PARTICLE_SYSTEM_