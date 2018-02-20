#ifndef _COMPONENT_PARTICLE_SYSTEM_
#define _COMPONENT_PARTICLE_SYSTEM_

#include "Component.h"

class ParticleSystem;
class ResourceMaterial;

class CompParticleSystem :public Component
{
public:
	CompParticleSystem(Comp_Type t, GameObject* parent);
	CompParticleSystem(const CompParticleSystem& copy, GameObject* parent);
	~CompParticleSystem();

	void Init();
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