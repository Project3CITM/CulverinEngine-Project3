#include "CompCamera.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "MathGeoLib.h"
#include "Application.h"
#include "JSONSerialization.h"
#include "Scene.h"
#include "ModuleFS.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleWindow.h"
#include "SDL\include\SDL_opengl.h"
#include <math.h>

CompCamera::CompCamera(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	uid = App->random->Int();

	/* Set camera vars*/
	width = 16;
	height = 9;
	aspect_ratio = width / height; // We set aspect ratio 16:9 by now

	near_plane = 0.2;
	far_plane = 1000;
	vertical_fov = 60; /* In degrees */

	/* Set frustum vars */
	frustum.type = PerspectiveFrustum;
	frustum.pos.Set(0, 0, 0);
	frustum.front.Set(0, 0, 1);
	frustum.up.Set(0, 1, 0);
	frustum.nearPlaneDistance = near_plane;
	frustum.farPlaneDistance = far_plane;
	frustum.verticalFov = vertical_fov * DEGTORAD;
	frustum.horizontalFov = Atan(aspect_ratio*Tan(frustum.verticalFov / 2)) * 2;

	name_component = "Camera";
}

CompCamera::CompCamera(const CompCamera& copy, GameObject* parent) : Component(Comp_Type::C_CAMERA, parent)
{
	uid = App->random->Int();

	/* Set camera vars from copy object */
	width = copy.width;
	height = copy.height;
	aspect_ratio = width / height; // We set aspect ratio 16:9 by now

	near_plane = copy.near_plane;
	far_plane = copy.far_plane;
	vertical_fov = copy.vertical_fov; /* In degrees */

	/* Set frustum vars  from copy object */
	frustum.type = copy.frustum.type;
	frustum.pos = copy.frustum.pos;
	frustum.front = copy.frustum.front;
	frustum.up = copy.frustum.up;
	frustum.nearPlaneDistance = near_plane;
	frustum.farPlaneDistance = far_plane;
	frustum.verticalFov = vertical_fov * DEGTORAD;
	frustum.horizontalFov = Atan(aspect_ratio*Tan(frustum.verticalFov / 2)) * 2;

	name_component = "Camera";
}

CompCamera::~CompCamera()
{
}

void CompCamera::PreUpdate(float dt)
{
	int w = 0;
	int h = 0;
	if (App->mode_game)
	{
		SDL_GetWindowSize(App->window->window, &w, &h);
		if (abs(GetRatio() - (float)w / (float)h) > 0.1)
		{
			SetRatio((float)w / (float)h);
		}
	}
	else
	{
		w = GetSizeDock("Scene").x;
		h = GetSizeDock("Scene").y;
		
		if (abs(GetRatio() - (float)w / (float)h) > 0.1)
		{
			SetRatio((float)w / (float)h);
		}
	}
}

void CompCamera::Update(float dt)
{
	if (parent->GetComponentTransform()->GetUpdated())
	{
		UpdateFrustum();
	}
}

// Update frustum from transform component of the gameobject
void CompCamera::UpdateFrustum()
{
	const CompTransform* transform = parent->GetComponentTransform();
	
	float4x4 trans = transform->GetGlobalTransform();
	frustum.pos = trans.Col3(3);
	frustum.front = trans.Col3(2).Normalized();
	frustum.up = trans.Col3(1).Normalized();
}

void CompCamera::Draw()
{
	// Only draw frustums on editor mode, not in game mode
	if (App->engine_state == EngineState::STOP)
	{
		DebugDraw();
	}
}

void CompCamera::DebugDraw()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void CompCamera::Clear()
{
	if (is_main)
	{
		// Unlink this camera from renderer3D
		App->renderer3D->SetGameCamera(nullptr);
	}
}

void CompCamera::ShowOptions()
{
	//ImGui::MenuItem("CREATE", NULL, false, false);
	if (ImGui::MenuItem("Reset", NULL, false, false))
	{
		// Not implmented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_CAMERA)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompCamera*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompCamera::ShowInspectorInfo()
{
	// Reset Values Button -------------------------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsCamera");
	}
	ImGui::PopStyleVar();
	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsCamera"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	if (ImGui::Checkbox("Main Camera", &is_main))
	{
		// Change this only out of Game Mode
		if (App->engine_state == EngineState::STOP) 
		{
			SetMain(is_main);
		}
		else
		{
			is_main = !is_main;
			LOG("You can't change 'Main Camera' variable when Game Mode is ON.")
		}
	}

	// Show Warning Popup of the camera
	if (showPopup)
	{
		ShowCameraPopup();
	}

	/* Enable-Disable Culling */
	if (ImGui::Checkbox("Culling", &culling))
	{
		if (!culling && is_main) //Disabling culling only available for the active camera
		{
			UnCull();
		}
		else if (!is_main)
		{
			LOG("Culling won't have effect because this camera is not the active camera.");
		}
	}

	/* Enable-Disable Culling */
	ImGui::Checkbox("DynamicUnCull", &cull_dynamics);


	// EDITABLE FRUSTUM VARIABLES ------------------
	ImGui::PushItemWidth(80);
	if (ImGui::DragFloat("Near Plane", &near_plane, 0.5f, 0.01f, 699.9f))
	{
		SetNear(near_plane);
	}
	if (ImGui::DragFloat("Far Plane", &far_plane, 0.5f, 700.0f, 1000.0f)) 
	{
		SetFar(far_plane);
	}
	if (ImGui::SliderFloat("FOV", &vertical_fov, 1, 179))
	{
		SetFov(vertical_fov);
	}
	// ------------------------------------------------

	ImGui::PopItemWidth();
	ImGui::TreePop();
}

void CompCamera::ShowCameraPopup()
{
	ImGui::OpenPopup("Main Camera Problem");
	if (ImGui::BeginPopupModal("Main Camera Problem", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("There is an active Game Camera already.");
		float width = ImGui::GetItemRectSize().x * 0.5;
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text(""); ImGui::SameLine(width - 20);
		if (ImGui::Button("OK", ImVec2(40, 20)))
		{
			showPopup = false;
		}
	}
	ImGui::EndPopup();
}

void CompCamera::DoCulling()
{
	if (parent == nullptr || !culling)
	{
		if (!cull_dynamics)
		{
			UnCullDynamics();
		}
		return;
	}
	
	// First check culling with static objects (optimized with quadtree)
	CullStaticObjects();
	

	// Then check dynamic objects
	BROFILER_CATEGORY("CullDynamic: CompCamera", Profiler::Color::Blue);
	if (cull_dynamics)
	{
		CullDynamicObjects();
	}
	else UnCullDynamics();
}

void CompCamera::CullStaticObjects()
{
	// First, set all static objects invisible

	// Get all static objects that are inside the frustum (accelerated with quadtree)
	//candidates_to_cull.clear();
	BROFILER_CATEGORY("CullStatic: CompCamera", Profiler::Color::Blue);
	App->scene->octree.CollectIntersections(candidates_to_cull, frustum);

	//BROFILER_CATEGORY("Draw CullStatic: CompCamera", Profiler::Color::Blue);
	// Set visible only these static objects
	/*while (!candidates_to_cull.empty())
	{
		candidates_to_cull.front()->Draw(); // INSIDE CAMERA VISION
		candidates_to_cull.pop_front();
	}*/
}

void CompCamera::CullDynamicObjects()
{
	const AABB* box = nullptr;
	float3 frustum_center = frustum.CenterPoint();
	float far_plane_Sq = frustum.MinimalEnclosingAABB().HalfSize().LengthSq();

	const int num = App->scene->dynamic_objects.size();
	GameObject** ptr = (num > 0) ? App->scene->dynamic_objects.data() : nullptr;
	
	GameObject* curr_obj = nullptr;
	for (int i = 0; i < num; i++)
	{
		curr_obj = ptr[i];
		if (curr_obj->box_fixed.IsFinite()) // Check if it has AABB and it's not the camera itself
		{
			box = &curr_obj->box_fixed;

			if ((box->CenterPoint() - frustum_center).LengthSq() - box->HalfSize().LengthSq() >  far_plane_Sq)
			{
				curr_obj->SetVisible(false); // OUTSIDE CAMERA VISION
			}
			else
			{
				curr_obj->SetVisible(true); // INSIDE CAMERA VISION
				curr_obj->Draw();
			}
		}
	}
}

void CompCamera::UnCull()
{
	//// Push all active elements that are root & active
	//for (uint i = 0; i < App->scene->root->GetNumChilds(); i++)
	//{
	//	if (App->scene->root->GetChildbyIndex(i)->IsActive())
	//	{
	//		candidates_to_cull.push_back(App->scene->root->GetChildbyIndex(i));
	//	}
	//}

	//// Check candidates_to_cull vector until it's empty
	//while (candidates_to_cull.empty() == false)
	//{
	//	candidates_to_cull.front()->SetVisible(true);

	//	//Push all childs that are active to the candidates vector
	//	for (std::vector<GameObject*>::iterator it = candidates_to_cull.front()->GetChildsPtr()->begin(); it != candidates_to_cull.front()->GetChildsPtr()->end(); it++)
	//	{
	//		if ((*it)->IsActive())
	//		{
	//			candidates_to_cull.push_back((*it));
	//		}
	//	}

	//	// Delete from vector the object already checked
	//	//candidates_to_cull.pop_front();
	//}
}

void CompCamera::UnCullDynamics()
{
	const int num = App->scene->dynamic_objects.size();
	GameObject** ptr = (num > 0) ? App->scene->dynamic_objects.data() : nullptr;

	GameObject* curr_obj = nullptr;
	for (int i = 0; i < num; i++)
	{
		curr_obj = ptr[i];
		if (curr_obj->box_fixed.IsFinite()) // Check if it has AABB and it's not the camera itself
		{
			curr_obj->SetVisible(true); // INSIDE CAMERA VISION
			curr_obj->Draw();
		}
	}
}
 
void CompCamera::LookAt(const float3& position)
{
	float3 direction = position - frustum.pos;
	float3x3 matrix = float3x3::LookAt(frustum.front, direction.Normalized(), frustum.up, float3(0, 1, 0));

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
}

Culling CompCamera::ContainsAABox(const AABB& refBox) const
{
	float3 corner[8];
	int totalIn = 0;
	refBox.GetCornerPoints(corner); // get the corners of the box into corner array
	
	// Test all 8 corners against the 6 sides of the frustum.
	// If all of them are behinf one specific plane = OUT.
	// If one of them is inside all the planes = IN or INTERSECT
	for (uint p = 0; p < 6; p++)
	{
		int In_count = 8;
		int point_In = 1;

		for(uint i = 0; i < 8; i++)
		{
			//Test all points against plane[p]
			if (frustum.GetPlane(p).IsOnPositiveSide(corner[i]))
			{
				// The point is behind the plane
				point_In = 0;
				In_count--;
			}
		}

		// If all points are out of the plane
		if (In_count == 0)
		{
			return CULL_OUT;
		}

		// Sum all points that are inside of the planes
		totalIn += point_In;
	}

	if (totalIn == 6)
	{
		return CULL_IN;
	}

	return CULL_INTERSECT;
}

void CompCamera::SetMain(bool isMain)
{
	if (isMain)
	{
		if (App->renderer3D->game_camera == nullptr)
		{
			// If there isn't any Game camera active, activate this camera
			App->renderer3D->SetGameCamera(this);
			is_main = true;
		}
		else
		{
			App->renderer3D->SetGameCamera(this);
			/* Otherwise, enable Pop Up of the camera and turn main  
			variable to false (not possible to be more than one active cameras at a time) */
			//showPopup = true;
			is_main = true;
		}
	}
	else 
	{
		App->renderer3D->SetGameCamera(nullptr);
		is_main = false;
	}
}

void CompCamera::SetPos(float3 pos)
{
	frustum.pos = pos;
}

void CompCamera::SetNear(float near_p)
{
	// Cap this to not overpas fast plane
	if (near_p < frustum.farPlaneDistance)
	{
		frustum.nearPlaneDistance = near_p;
	}
	else
	{
		// Recover previous value for the output option
		near_plane = frustum.nearPlaneDistance;
	}
}

void CompCamera::SetFar(float far_p)
{
	// Cap this to not overpass near plane
	if (far_p > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = far_p;
	}
	else
	{
		// Recover previous value for the output option
		far_plane = frustum.farPlaneDistance;
	}
}

void CompCamera::SetFov(float vertical)
{
	if (!App->mode_game)
	{
		float w = GetSizeDock("Scene").x;
		float h = GetSizeDock("Scene").y;
		float div = (w/h);
		frustum.verticalFov = vertical * DEGTORAD;
		frustum.horizontalFov = (2.0f * math::Atan(math::Tan(frustum.verticalFov / 2.0f) *div));
	}
	else if(App->renderer3D->active_camera)
	{
		int w=App->window->GetWidth();
		int h = App->window->GetHeight();
		float div = ((float)w / (float)h);
		frustum.verticalFov = vertical * DEGTORAD;
		App->renderer3D->active_camera->frustum.horizontalFov = (2.0f * math::Atan(math::Tan(App->renderer3D->active_camera->frustum.verticalFov / 2.0f) *div));
	}
}

void CompCamera::SetRatio(float ratio)
{
	aspect_ratio = ratio;
	frustum.horizontalFov = Atan(aspect_ratio*Tan(frustum.verticalFov / 2.0f)) * 2.0f;
}

void CompCamera::SetCullDynamics(bool active)
{
	cull_dynamics = active;
}

float CompCamera::GetNear() const
{
	return frustum.nearPlaneDistance;
}

float CompCamera::GetFar() const
{
	return frustum.farPlaneDistance;
}

float CompCamera::GetFOV() const
{
	return frustum.verticalFov * RADTODEG;
}

float CompCamera::GetRatio() const
{
	return frustum.AspectRatio();
}

float* CompCamera::GetViewMatrix() const
{
	static float4x4 matrix;
	matrix = frustum.ViewMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}

float* CompCamera::GetProjectionMatrix() const
{
	static float4x4 matrix;
	matrix = frustum.ProjectionMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}

void CompCamera::CopyValues(const CompCamera* component)
{
	//more...
}

void CompCamera::Save(JSON_Object * object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_CAMERA);

	// Transform variables ------
	App->fs->json_array_dotset_float3(object, name + "Position", frustum.pos);
	App->fs->json_array_dotset_float3(object, name + "Front", frustum.front);
	App->fs->json_array_dotset_float3(object, name + "Up", frustum.up);

	// Frustum variables --------
	json_object_dotset_number_with_std(object, name + "Near Plane", frustum.nearPlaneDistance);
	json_object_dotset_number_with_std(object, name + "Far Plane", frustum.farPlaneDistance);
	json_object_dotset_number_with_std(object, name + "Vertical Pov", frustum.verticalFov);

	// Config options variables ---------
	json_object_dotset_boolean_with_std(object, name + "Main Camera", is_main);
	json_object_dotset_boolean_with_std(object, name + "Culling", culling);
	json_object_dotset_boolean_with_std(object, name + "Dynamic Culling", cull_dynamics);
}

void CompCamera::Load(const JSON_Object * object, std::string name)
{
	// Set frustum variables -------
	frustum.pos = App->fs->json_array_dotget_float3_string(object, name + "Position");
	frustum.front = App->fs->json_array_dotget_float3_string(object, name + "Front");
	frustum.up = App->fs->json_array_dotget_float3_string(object, name + "Up");
	frustum.nearPlaneDistance = json_object_dotget_number_with_std(object, name + "Near Plane");
	frustum.farPlaneDistance = json_object_dotget_number_with_std(object, name + "Far Plane");
	frustum.verticalFov = json_object_dotget_number_with_std(object, name + "Vertical Pov");

	is_main = json_object_dotget_boolean_with_std(object, name + "Main Camera");

	culling = json_object_dotget_boolean_with_std(object, name + "Culling");
	cull_dynamics = json_object_dotget_boolean_with_std(object, name + "Dynamic Culling");

	Enable();
}

void CompCamera::SyncComponent(GameObject * sync_parent)
{
	// Set output variables ----
	near_plane = frustum.nearPlaneDistance;
	far_plane = frustum.farPlaneDistance;
	vertical_fov = frustum.verticalFov * RADTODEG; /* output variable in Degrees */
	SetFov(vertical_fov);

	SetMain(is_main);
	UpdateFrustum();
}

void CompCamera::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int);				//UID
	
	buffer_size += sizeof(bool);			//Culling
	buffer_size += sizeof(bool);			//Cull_dynamics
	buffer_size += sizeof(bool);			//Is_Main

	buffer_size += sizeof(float);			//near_plane
	buffer_size += sizeof(float);			//far_plane
	buffer_size += sizeof(float);			//vertical_fov

	buffer_size += sizeof(float);			//Frustum Position
	buffer_size += sizeof(float);			//Frustum Position
	buffer_size += sizeof(float);			//Frustum Position

	buffer_size += sizeof(float);			//Frustum Front
	buffer_size += sizeof(float);			//Frustum Front
	buffer_size += sizeof(float);			//Frustum Front

	buffer_size += sizeof(float);			//Frustum Up
	buffer_size += sizeof(float);			//Frustum Up
	buffer_size += sizeof(float);			//Frustum Up
}

void CompCamera::SaveBinary(char ** cursor, int position) const
{
	Component::SaveBinary(cursor, position);
	App->json_seria->SaveIntBinary(cursor, uid);

	// Config options variables ---------
	App->json_seria->SaveBooleanBinary(cursor, culling);
	App->json_seria->SaveBooleanBinary(cursor, cull_dynamics);
	App->json_seria->SaveBooleanBinary(cursor, is_main);

	// Frustum variables --------
	App->json_seria->SaveFloatBinary(cursor, frustum.nearPlaneDistance);
	App->json_seria->SaveFloatBinary(cursor, frustum.farPlaneDistance);
	App->json_seria->SaveFloatBinary(cursor, frustum.verticalFov);

	// Transform variables ------
	App->json_seria->SaveFloat3Binary(cursor, frustum.pos);
	App->json_seria->SaveFloat3Binary(cursor, frustum.front);
	App->json_seria->SaveFloat3Binary(cursor, frustum.up);
}

void CompCamera::LoadBinary(char ** cursor)
{
	uid = App->json_seria->LoadIntBinary(cursor);
	culling = App->json_seria->LoadBooleanBinary(cursor);
	cull_dynamics = App->json_seria->LoadBooleanBinary(cursor);
	is_main = App->json_seria->LoadBooleanBinary(cursor);

	frustum.nearPlaneDistance = App->json_seria->LoadFloatBinary(cursor);
	frustum.farPlaneDistance = App->json_seria->LoadFloatBinary(cursor);
	frustum.verticalFov = App->json_seria->LoadFloatBinary(cursor);

	frustum.pos = App->json_seria->LoadFloat3Binary(cursor);
	frustum.front = App->json_seria->LoadFloat3Binary(cursor);
	frustum.up = App->json_seria->LoadFloat3Binary(cursor);

	Enable();
}