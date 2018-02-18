#ifndef _COMPONENT_PARTICLE_SYSTEM_
#define _COMPONENT_PARTICLE_SYSTEM_

#include "Component.h"

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
};
#endif //_COMPONENT_PARTICLE_SYSTEM_