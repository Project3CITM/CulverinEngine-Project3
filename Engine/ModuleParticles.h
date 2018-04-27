#ifndef __ModuleParticles_H__
#define __ModuleParticles_H__

#include "Module.h"
#include "parson.h"
#include <vector>
#include <string>
#include "ParticleSystem.h"


class ModuleParticles : public Module
{
public:

	ModuleParticles(bool start_enabled = true);
	~ModuleParticles();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

	ParticleSystem* CreateParticleSystem();	

	GLuint GetGeometryBuffer() const;

private:

	GLuint geometry_buffer = 0;
	std::vector<ParticleSystem*> particle_systems;
};

#endif // __ModuleAudio_H__