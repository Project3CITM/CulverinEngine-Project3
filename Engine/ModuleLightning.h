#ifndef __MODULELIGHTNING_H__
#define __MODULELIGHTNING_H__

#include "Module.h"

#include "ModuleFramebuffers.h"
#include "DepthCubeMap.h"
#include "glm\glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

#define DEFAULT_SHADOW_CAST_POINTS_COUNT 9

class CompLight;
class ShaderProgram;
class CompMesh;
class ResourceMesh;

bool OrderLights(CompLight* l1, CompLight* l2);

class DepthFrameBuffer
{
public:
	DepthFrameBuffer();
	DepthFrameBuffer(int width, int height);
	~DepthFrameBuffer();

	void Create(int width, int height);
	void Destroy();
	void Resize(int width, int height);
	void Bind(const char* window);
	void UnBind(const char* window);

	uint GetTexture()const;

public:
	int width = 0;
	int height = 0;
	uint frame_id = 0;
	uint texture = 0;
	uint depthTex=0;
};

class ModuleLightning : public Module
{
public:
	ModuleLightning(bool start_enabled = true);
	virtual ~ModuleLightning();

	bool Init(JSON_Object* node) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status UpdateConfig(float dt) override;
	bool SaveConfig(JSON_Object* node) override;
	bool CleanUp() override;
	void OnEvent(Event& event);

	void SetShadowCastPoints(uint points);
	void ResizeShadowMaps(uint w_res, uint h_res);

	std::vector<DepthCubeMap*>* GetShadowMaps();
	void GetShadowMapsResolution(uint& w_res, uint& h_res);

	void OnLightCreated(CompLight* l);
	void OnLightDestroyed(CompLight* l);

	std::vector<CompLight*> GetSceneLights()const;
	std::vector<CompLight*>* GetActiveLights(); // Return the point active lights for that frame.
	void GetActiveLightsCount(uint ammount, std::vector<CompLight*>& to_fill); // Fills the parameter vector with the asked amount of closest lights

	bool SetEventListenrs();

	
private:
	void AddShadowMapCastViews(uint ammount);
	void CalcPointShadowMaps(Event& events, CompLight* light);
	void CalcDirectionalShadowMap(CompLight* light, CompMesh* mesh);

public:
	//test
	ShaderProgram * shadow_Shader = nullptr;
	ShaderProgram* point_light_shadow_depth_shader = nullptr;

	FrameBuffer text;
	DepthFrameBuffer test_fix;
	ResourceMesh*  light_UI_plane = nullptr;


	glm::mat4 depthBiasMVP;

	bool light_extense_debug_info = false;

private:
	uint shadow_cast_points_count = DEFAULT_SHADOW_CAST_POINTS_COUNT; // This value should be able to change from config and modiffied on load
	std::vector<DepthCubeMap*> shadow_point_lights_maps;
	uint shadow_maps_res_w = 256;
	uint shadow_maps_res_h = 256;


	std::vector<CompLight*> scene_lights;
	std::vector<CompLight*> frame_used_lights;
	const std::vector<GameObject*>* scene_objects;
	// Some directional light variables on shadow map render
	// On directional light shadow map calculations, distance from the scene camera to look at from shadow map camera
	float dstSceneCameraToLookAt = 20.0f;
	float projSize = 20.0f, nearPlane = 1.0f, farPlane = 60.0f;
	float dstShadowmapCameraToLookAt = 20.0f;
};

#endif // __MODULELIGHTNING_H__