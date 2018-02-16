#ifndef __MODULELIGHTNING_H__
#define __MODULELIGHTNING_H__

#include "Module.h"

#include "ModuleFramebuffers.h"

#include <vector>

#define DEFAULT_SHADOW_CAST_POINTS_COUNT 4

class CompLight;

class ModuleLightning : public Module
{
public:
	ModuleLightning(bool start_enabled = true);
	virtual ~ModuleLightning();

	bool Init(JSON_Object* node) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	bool SaveConfig(JSON_Object* node) override;
	bool CleanUp() override;

	void CalShadowMaps();
	void SetShadowCastPoints(uint points);
	void ResizeShadowMaps(uint w_res, uint h_res);

	std::vector<FrameBuffer>* GetShadowMaps();
	void GetShadowMapsResolution(uint& w_res, uint& h_res);

	void OnLightCreated(CompLight* l);
	void OnLightDestroyed(CompLight* l);
	
private:
	void AddShadowMapCastViews(uint ammount);

public:

private:
	uint shadow_cast_points_count = DEFAULT_SHADOW_CAST_POINTS_COUNT; // This value should be able to change from config and modiffied on load
	std::vector<FrameBuffer> shadow_maps;
	uint shadow_maps_res_w = 1024;
	uint shadow_maps_res_h = 1024;

	std::vector<CompLight*> scene_lights;
	std::vector<CompLight*> frame_used_lights;
};

#endif // __MODULELIGHTNING_H__