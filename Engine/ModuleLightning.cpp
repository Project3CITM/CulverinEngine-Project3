#include "ModuleLightning.h"



ModuleLightning::ModuleLightning(bool start_enabled) : Module(start_enabled)
{
	name = "Lightning";
	have_config = true;
}


ModuleLightning::~ModuleLightning()
{
}

bool ModuleLightning::Init(JSON_Object* node)
{
	// TODO: Read ammount of shadow cast point from config. Will use default for now for testing purposes
	shadow_cast_points_count = 3;

	return true;
}

bool ModuleLightning::Start()
{
	AddShadowMapCastViews(shadow_cast_points_count);

	return true;
}

update_status ModuleLightning::PreUpdate(float dt)
{
	// TODO: Calc here the closest lights used

	return UPDATE_CONTINUE;
}

update_status ModuleLightning::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleLightning::SaveConfig(JSON_Object* node)
{
	return true;
}

bool ModuleLightning::CleanUp()
{
	for (std::vector<FrameBuffer>::iterator it = shadow_maps.begin(); it != shadow_maps.end(); ++it)
	{
		(*it).Destroy();
	}

	return true;
}

void ModuleLightning::CalShadowMaps()
{
	//TODO: Actually calc the shadow maps
	//TODO: Need to get the main/render camera somehow

	// Start rendering z buffer into the frame buffer


}

void ModuleLightning::SetShadowCastPoints(uint points)
{
	shadow_cast_points_count = points;
	if(shadow_cast_points_count > shadow_maps.size())
	{
		// Then create new frame buffers
		// TODO: Probably would be better to add a max ammount of shadow maps

		AddShadowMapCastViews(shadow_cast_points_count - shadow_maps.size());
	}
}

void ModuleLightning::ResizeShadowMaps(uint w_res, uint h_res)
{
	shadow_maps_res_w = w_res;
	shadow_maps_res_h = h_res;

	for(std::vector<FrameBuffer>::iterator it = shadow_maps.begin(); it != shadow_maps.end(); ++it)
	{
		(*it).Resize(shadow_maps_res_w, shadow_maps_res_h);
	}
}

std::vector<FrameBuffer>* ModuleLightning::GetShadowMaps()
{
	return &shadow_maps;
}

void ModuleLightning::GetShadowMapsResolution(uint& w_res, uint& h_res)
{
	w_res = shadow_maps_res_w;
	h_res = shadow_maps_res_h;
}

void ModuleLightning::OnLightCreated(CompLight* l)
{
	//TODO: Link this with component creation

	scene_lights.push_back(l);
}

void ModuleLightning::OnLightDestroyed(CompLight* l)
{
	//TODO: Link this with component destruction

	for(std::vector<CompLight*>::iterator it = scene_lights.begin(); it != scene_lights.end(); ++it)
	{
		if((*it) == l)
		{
			scene_lights.erase(it);

			for(std::vector<CompLight*>::iterator it2 = frame_used_lights.begin(); it2 != frame_used_lights.end(); ++it2)
			{
				frame_used_lights.erase(it2);
				return;
			}

			return;
		}
	}
}

void ModuleLightning::AddShadowMapCastViews(uint ammount)
{
	for(int i = 0; i < ammount; ++i)
	{
		FrameBuffer buff;
		buff.Create(shadow_maps_res_w, shadow_maps_res_h);
		shadow_maps.push_back(buff);
	}
}

// ------------- UI -----------------------------------------

update_status ModuleLightning::UpdateConfig(float dt)
{
	//TODO: Add possibility to change this. For now just info display
	ImGui::Text("Shadow cast points used:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", shadow_cast_points_count);

	ImGui::Text("Shadow maps resolution:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i x %i", shadow_maps_res_w, shadow_maps_res_h);

	ImGui::Text("Lights in the scene:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", scene_lights.size());

	ImGui::Text("Lights used on frame:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", frame_used_lights.size());

	if(ImGui::TreeNodeEx("Shadow maps"))
	{
		for(uint i = 0; i < shadow_maps.size(); ++i)
		{
			if(i == shadow_cast_points_count)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "This shadow maps are not used currently!");
				ImGui::Separator();
			}

			std::string shadowmap_name = "Shadow map: " + std::to_string(i);
			if(ImGui::TreeNodeEx(shadowmap_name.c_str()))
			{
				FrameBuffer* frame = &shadow_maps[i];
				ImGui::Image((ImTextureID*)frame->GetTexture(), ImVec2(256, 256));

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	return UPDATE_CONTINUE;
}