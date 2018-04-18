#include "Application.h"
#include "Scene.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleGUI.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "Component.h"
#include "CompTransform.h"
#include "CompRectTransform.h"
#include "CompButton.h"
#include "CompCheckBox.h"
#include "CompCollider.h"
#include "CompImage.h"
#include "CompCanvas.h"
#include "CompEditText.h"
#include "CompText.h"
#include "CompMesh.h"
#include "CSharpScript.h"
#include "CompScript.h"
#include "ResourceMesh.h"
#include "CompCheckBox.h"
#include "CompSlider.h"
#include "ImportMesh.h"
#include "ImportScript.h"
#include "CompMaterial.h"
#include "WindowInspector.h"
#include "CompCamera.h"
#include "MathGeoLib.h"
#include "JSONSerialization.h"
#include "SkyBox.h"
#include "ModuleRenderGui.h"
#include "CompLight.h"
#include "Gl3W/include/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "DefaultShaders.h"
#include <direct.h>

//#include "..\..\3D-Engine\ScriptingSystem\ScriptingSystem\ScriptManager.h"

//Event system test
#include "ModuleEventSystemV2.h"

#define SPHERE_DEFINITION 1536

Scene::Scene(bool start_enabled) : Module(start_enabled)
{
	Start_enabled = true;
	preUpdate_enabled = true;
	Update_enabled = true;
	
	name = "Scene";
	have_config = true;
	load_scene = true;
}

Scene::~Scene()
{
	DeleteAllGameObjects(root);
	DeleteAllGameObjects(secondary_root);
	DeleteAllGameObjects(temporary_scene);
	search_name->GetChildsPtr()->clear();
	//DeleteAllGameObjects(dontdestroyonload);
	defined_tags.clear();
	for (uint i = 0; i < tagged_objects.size(); i++)
	{
		tagged_objects[i]->clear();
		RELEASE(tagged_objects[i]);
	}
	tagged_objects.clear();

	RELEASE(root);
	RELEASE(secondary_root);
	RELEASE(temporary_scene);
	RELEASE(search_name);
	RELEASE(dontdestroyonload);

	RELEASE(scene_buff);
	RELEASE(skybox);
}

bool Scene::Init(JSON_Object* node)
{
	perf_timer.Start();
	/* Init Octree */
	octree.Boundaries(AABB(float3(-1.0f, -1.0f, -1.0f), float3(1.0f, 1.0f, 1.0f)));
	skybox_index = json_object_get_number(node, "Skybox Index");
	Awake_t = perf_timer.ReadMs();
	return true;
}

bool Scene::Start()
{
	perf_timer.Start();

	// First of all create New Scene
	root = new GameObject("NewScene", 1);
	secondary_root = new GameObject("SecondScene", 1);
	temporary_scene = new GameObject("Temporary Scene", 1);
	search_name = new GameObject("search_name", 1);
	dontdestroyonload = new GameObject("Dont't Destroy On Load", 1);

	/* Init Quadtree */
	size_quadtree = 5000.0f;
//	quadtree.Init(size_quadtree);
	octree.limits.octreeMinSize = 50;
	octree.Boundaries(AABB(float3(-size_quadtree, -size_quadtree, -size_quadtree), float3(size_quadtree, size_quadtree, size_quadtree)));


	/* Set size of the plane of the scene */
	size_plane = 50;


	icon_options_transform = App->textures->LoadTexture("Images/UI/icon_options_transform.png");
	icon_resetAll = App->textures->LoadTexture("Images/UI/icon_resetAll.png");

	/* Init Skybox */ 
	skybox = new SkyBox();
	skybox->InitSkybox();
	//skybox_index = 1;
	draw_skybox = true;

	defined_tags.push_back("undefined");
	defined_tags.push_back("camera");
	defined_tags.push_back("player");

	RELEASE(scene_buff);
	scene_buff = new FrameBuffer();
	scene_buff->Create(App->window->GetWidth(), App->window->GetHeight());

	final_buff = new FrameBuffer();
	final_buff->resize = true;
	final_buff->Create(App->window->GetWidth(), App->window->GetHeight());

	glow_buff = new FrameBuffer();
	glow_buff->resize = false;
	glow_buff->Create(128, 128);

	horizontal_blur_buff = new FrameBuffer();
	horizontal_blur_buff->resize = false;
	horizontal_blur_buff->Create(128, 128);

	vertical_blur_buff = new FrameBuffer();
	vertical_blur_buff->resize = false;
	vertical_blur_buff->Create(128, 128);


	tagged_objects.reserve(defined_tags.size());
	for (uint i = 0; i < defined_tags.size(); i++)
	{
		tagged_objects.push_back(new std::vector<GameObject*>());
	}

	Start_t = perf_timer.ReadMs();
	return true;
}

update_status Scene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate: Scene", Profiler::Color::Blue);
	perf_timer.Start();

	if (root == nullptr)
	{
		return UPDATE_CONTINUE;
	}

	if (load_scene)
	{
		LoadScene();
	}

	// PreUpdate GameObjects ------------------------
	if (root->WanttoDelete() == false)
	{
		if (dontdestroyonload->GetNumChilds() > 0)
		{
			dontdestroyonload->PreUpdate(dt);
		}
		else
		{
			BROFILER_CATEGORY("Scene: PreUpdate", Profiler::Color::Red);
			root->PreUpdate(dt);
		}
	}
	else
	{
		DeleteGameObject(root);
	}

	//static bool ttt = true;
	//if (ttt)
	//{
	//	if (strcmp(App->GetActualScene().c_str(), "") != 0)
	//	{
	//		ttt = false;
	//		App->json_seria->LoadScene(App->GetActualScene().c_str());
	//	}
	//}

	//// PreUpdate GameObjects ------------------------
	//for (uint i = 0; i < gameobjects_inscene->GetNumChilds(); i++)
	//{
	//	if (gameobjects_inscene->WanttoDelete() == false) //Scene
	//	{
	//		gameobjects_inscene->preUpdate(dt);
	//	}
	//}

	deleted_objects.clear();

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status Scene::Update(float dt)
{
	BROFILER_CATEGORY("Update: Scene", Profiler::Color::Blue);
	perf_timer.Start();

	// Update GameObjects -----------
	if (dontdestroyonload->GetNumChilds() > 0)
	{
		dontdestroyonload->Update(dt);
	}
	else
	{
		BROFILER_CATEGORY("Scene: Update", Profiler::Color::Red);
		root->Update(dt);
	}

	// Draw Skybox (direct mode for now)
	//if (App->scene->draw_skybox) 

	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	App->scene->scene_buff->Bind("Scene");
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	App->scene->skybox->DrawSkybox(800, App->renderer3D->active_camera->frustum.pos, App->scene->skybox_index);


	// Draw Plane
	if (App->engine_state != EngineState::PLAY)
	{
		DrawPlane();
	}

	
	// Draw Quadtree
	if (octree_draw) octree.DebugDraw();

	App->scene->scene_buff->UnBind("Scene");
	// Draw GUI
	//App->render_gui->ScreenSpaceDraw();
	static int nico = 0;
	static bool noenter = false;
	nico++;
	if (App->mode_game && nico > 5 && noenter == false)
	{
		noenter = true;
		if (App->scene->CheckNoFails())
		{
			int exc = App->engine_state;
			App->SetState(EngineState::PLAY); // OR STOP

			//App->importer->iScript->ClearMonoMap();

			if (exc == EngineState::STOP)
			{
				App->importer->iScript->SetMonoMap(App->scene->root, true);
				//Start all scripts
				App->scene->StartScripts();
			}
		}
	}

	CompCamera* active_camera = App->renderer3D->GetActiveCamera();
	if (active_camera != nullptr)
	{
		BROFILER_CATEGORY("DoCulling: ModuleScene", Profiler::Color::Blue);
		active_camera->DoCulling();
	}

	Update_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

//update_status Scene::PostUpdate()
//{
//	perf_timer.Start();
//
//	root->postUpdate();
//
//	postUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

update_status Scene::UpdateConfig(float dt)
{
	/* Edit Plane Size */
	ImGui::PushItemWidth(ImGui::GetWindowWidth() / 4);
	ImGui::SliderInt("Plane Size", &size_plane, 5, 1000);

	/* Quadtree configuration */
	EditorQuadtree();

	/* Skybox configuration */
	EditorSkybox();

	/* ReImport all and delete Resources */
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.761f, 0.00f, 1.00f));
	ImGui::Text("WARNING!");
	ImGui::PopStyleColor();
	ImGui::BulletText("This button delete all resources and reimport all the files in assets with UUID from his .meta.json.");
	ImGui::BulletText("Only use this if you have a problem with Load and Save scene or some other kind of problem.");
	if (ImGui::ImageButton((ImTextureID*)icon_resetAll, ImVec2(60, 60), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		App->resource_manager->reimportAll = true;
	}

	return UPDATE_CONTINUE;
}

bool Scene::CleanUp()
{
	skybox->DeleteSkyboxTex();
	
	octree.Clear();
	root->CleanUp();
	secondary_root->CleanUp();
	temporary_scene->CleanUp();
	
	RELEASE(scene_buff);
	RELEASE(glow_buff);
	RELEASE(horizontal_blur_buff);
	RELEASE(vertical_blur_buff);
	RELEASE(final_buff);


	return true;
}

void Scene::LoadScene()
{
	bool new_scene = true;

	if (strcmp(App->GetActualScene().c_str(), "") != 0)
	{
		new_scene = false;
		App->json_seria->LoadScene(App->GetActualScene().c_str());

	}
	if (new_scene)
	{
		/* Create Default Main Camera Game Object */
		CreateMainCamera(nullptr);
		CreateMainLight(nullptr);

		// Also Light??
	}

	load_scene = false;
}

bool Scene::SetEventListenrs()
{
	AddListener(EventType::EVENT_DELETE_GO, this);
	AddListener(EventType::EVENT_DRAW, this);
	AddListener(EventType::EVENT_PARTICLE_DRAW, this);
	AddListener(EventType::EVENT_SCRIPT_DISABLED, this);
	AddListener(EventType::EVENT_SPAWN_GAMEOBJECT, this);
	return true;
}

void Scene::OnEvent(Event & event)
{
	BROFILER_CATEGORY("OnEvent: Scene", Profiler::Color::Blue);
	switch (event.Get_event_data_type())
	{
	case EventType::EVENT_DELETE_GO:
	{
		LOG("Delete GameObject");
		if (CheckDeletedObjcet(event.delete_go.uuid))
		{
			DeleteGameObject(event.delete_go.Todelte);
		}
		break;
	}
	case EventType::EVENT_SPAWN_GAMEOBJECT:
	{
		root->AddChildGameObject(event.spawnGO.Tospawn);
		break;
	}
	case EventType::EVENT_SCRIPT_DISABLED:
	{
		event.script_disabled.script->postUpdate();
		break;
	}
	}
}

void Scene::EditorQuadtree()
{
	//QUADTREE EDITOR ---------------------------------------------------------
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.761f, 0.00f, 1.00f));
	if (ImGui::TreeNodeEx("QUADTREE", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PopStyleColor();

		//TODO: Joan->Redo quadtree config options

		/* Enable Debug Draw */
		ImGui::Checkbox("##quadtreedraw", &octree_draw); ImGui::SameLine();
		ImGui::Text("Draw Octree");
		ImGui::SliderFloat("Size", &size_quadtree, 50, 300);

		/* Remake Quadtree with actual static objects*/
		if (ImGui::Button("UPDATE QUADTREE"))
		{
			octree.Clear(false);
			//Clac adaptative size of scene octree
			/**/
			//AABB AdaptativeAABB;
		
			//AdaptativeAABB.SetNegativeInfinity();
			
			//Make sure that we have all go
			GetAllSceneObjects();
			/*for (std::vector<GameObject*>::const_iterator item = game_objects_scene.cbegin(); item != game_objects_scene.cend(); ++item)
			{
				if ((*item)->IsStatic())
				{
					if ((*item)->box_fixed.IsFinite())
					{
						AdaptativeAABB.Enclose((*item)->box_fixed);
					}
				}
			}
			octree.Boundaries(AdaptativeAABB);*/
			/**/
			//Insert AABBs to octree
			for (std::vector<GameObject*>::const_iterator item = game_objects_scene.cbegin(); item != game_objects_scene.cend(); ++item)
				if ((*item)->IsStatic())
					octree.Insert(*item);


		/*	if (App->engine_state == EngineState::STOP)
			{
				if (size_quadtree != quadtree.size)
				{
					quadtree.Init(size_quadtree);
				}
				else
				{
					quadtree.root_node->Clear();
				}
				quadtree.Bake(App->scene->root);
			}
			else
			{
				LOG("Update Quadtree not possible while GAME MODE is ON");
			}*/
		}
		ImGui::TreePop();
	}
	else
	{
		ImGui::PopStyleColor();
	}
	// --------------------------------------------------------------------------------
}

void Scene::EditorSkybox()
{
	// SKYBOX EDITOR ------------------------------------------------------------------
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.761f, 0.00f, 1.00f));
	if (ImGui::TreeNodeEx("SKYBOX", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PopStyleColor();
		const char* skybox_selection[] = { "AFTERNOON", "SUNNY DAY", "NONE" };
		
		/* To Select your desired Skybox */
		if (ImGui::Combo("Select Skybox", &skybox_index, skybox_selection, IM_ARRAYSIZE(skybox_selection)))
		{
			if (skybox_index == 2) // Selected "NONE"
			{
				draw_skybox = false;
			}
			else
			{
				draw_skybox = true;
			}
		}
		if (draw_skybox)
		{
			ImGui::Image((ImTextureID*)skybox->GetTextureID(skybox_index), ImVec2(170, 170), ImVec2(1, 1), ImVec2(0, 0));
		}
		ImGui::TreePop();
	}
	else
	{
		ImGui::PopStyleColor();
	}
	ImGui::PopItemWidth();
	// ----------------------------------------------------------------------------------------
}

bool Scene::CheckNoFails()
{
	int fails = 0;
	
	root->CheckScripts(fails);

	if (fails == 0)
	{
		LOG("All Scripts are succesfully compiled.");
		return true;
	}
	else
	{
		LOG("[error] total scripts failed: %i.", fails);
	}
	return false;
}

void Scene::StartScripts()
{
	//Iterate all GameObjects and, if they have scripts, call their start
	root->StartScripts();
}

GameObject* Scene::GetGameObjectfromScene(bool& active)
{
	if (!ImGui::Begin("GameObjects in Scene", &active, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
	else
	{
		for (int i = 0; i < root->GetNumChilds(); i++)
		{
			ImGui::PushID(i);
			GameObject* temp = root->GetChildbyIndex(i)->GetGameObjectfromScene(i);
			
			if (temp != nullptr)
			{
				ImGui::PopID();
				ImGui::End();
				active = false;
				return temp;
			}

			ImGui::PopID();
		}
		ImGui::End();
	}
	return nullptr;
}

GameObject* Scene::GetGameObjectbyuid(uint uid)
{
	GameObject* ret = root->GetGameObjectbyuid(uid);
	
	if (ret != nullptr)
	{
		return ret;
	}

	return nullptr;
}

GameObject * Scene::FindGameObjectWithTag(const char * str)
{
	uint size = defined_tags.size();
	for (uint k = 0; k < size; k++)
	{
		if (strcmp(str, defined_tags[k].c_str()) == 0)
		{
			if(tagged_objects[k]->empty() == false)return tagged_objects[k]->front();
			break;
		}
	}
	return nullptr;
}

void Scene::FindGameObjectsWithTag(const char * tag, std::vector<GameObject*>* vec)
{
	uint size = defined_tags.size();
	for (uint k = 0; k < size; k++)
	{
		if (strcmp(tag, defined_tags[k].c_str()) == 0)
		{
			vec = tagged_objects[k];
		}
	}
}

void Scene::TagWindow()
{
	char buffer[100];
	GameObject* target = ((Inspector*)App->gui->win_manager[INSPECTOR])->GetSelected();
	sprintf(buffer, "Tag:  %s", target->GetTag());
	
	if (ImGui::BeginCombo("##Select", buffer))
	{
		for (int i = 0; i < defined_tags.size(); i++)
		{
			if (ImGui::Selectable(defined_tags[i].c_str()))
			{
				target->SetTag((char*)defined_tags[i].c_str());
			}
		}

		//ImGui::Button
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
		if (ImGui::Button("+ Add Tag"))
		{
			on_tag_creation = !on_tag_creation;
			on_tag_delete = false;
			memset(tag_buffer, '\0', 100);
		}
		ImGui::SameLine();
		if (ImGui::Button("- Delete Tag") && defined_tags.size() > 0)
		{
			on_tag_delete = !on_tag_delete;
			on_tag_creation = false;
			memset(tag_buffer, '\0', 100);
		}
		ImGui::PopStyleColor();

		if (on_tag_creation)
		{
			ImGui::SetItemDefaultFocus();
			if (ImGui::InputText("New Tag", tag_buffer, 100, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				AddTag(tag_buffer);
				memset(tag_buffer, '\0', 100);
				on_tag_creation = false;
			}
		}

		if (on_tag_delete && defined_tags.size() > 0)
		{
			uint size = defined_tags.size();
			if (size == 1)
			{
				DeleteObjectsTag(defined_tags.front().c_str());
				defined_tags.clear();
				on_tag_delete = false;
			}
			else if (ImGui::BeginCombo("##Delete", "Select Delete"))
			{
				for (int i = 0; i < size; i++)
				{
					if (ImGui::Selectable(defined_tags[i].c_str()))
					{
						DeleteTag(defined_tags[i].c_str(), i);
						on_tag_delete = false;
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::EndCombo();
	}	
}

void Scene::DeleteObjectsTag(const char * tag)
{
	if (strcmp(root->GetTag(), tag) == 0)
	{
		root->SetTag("undefined");
	}

	std::vector<GameObject*> childs;
	root->FindChildsWithTag(tag, &childs);
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		childs[k]->SetTag("undefined");
	}
	childs.clear();
}

bool Scene::FindTag(const char * tag) const
{
	uint size = defined_tags.size();
	for (uint k = 0; k < size; k++)
	{
		if (strcmp(tag, defined_tags[k].c_str()) == 0)return true;
	}
	return false;
}

uint Scene::TagsSize() const
{
	return defined_tags.size();
}

void Scene::AddTag(const char * str)
{
	if (defined_tags.size() >= 32)
	{
		LOG("Max number of tags defined! Lim: 32");
		return;
	}

	if(!FindTag(str))defined_tags.push_back(str);
	std::vector<GameObject*>* vec = new std::vector<GameObject*>();
	tagged_objects.push_back(vec);
}

void Scene::DeleteTag(const char * str, uint index)
{
	DeleteObjectsTag(str);
	uint size = defined_tags.size();
	if (index == 0)
	{
		for (uint k = 0; k < size; k++)
		{
			if (strcmp(str, defined_tags[k].c_str()) == 0)
			{
				index = k;
				break;
			}
		}
	}
	for (index; index < size - 1; index++)
	{
		defined_tags[index] = defined_tags[index + 1];
	}
	defined_tags.pop_back();

	RELEASE(tagged_objects[index]);

	for (index; index < size - 1; index++)
	{
		tagged_objects[index] = tagged_objects[index + 1];
	}
	
	tagged_objects.pop_back();
}

void Scene::RemoveTaggedObject(const GameObject * target)
{
	uint size = defined_tags.size();
	for (uint k = 0; k < size; k++)
	{
		if (strcmp(target->GetTag(), defined_tags[k].c_str()) == 0)
		{
			uint v_size = tagged_objects[k]->size();
			for (uint d = 0; d < v_size; d++)
			{
				if (tagged_objects[k]->at(d) == target)
				{
					for (uint h = d; h < v_size - 1; h++)
					{
						tagged_objects[k][d] = tagged_objects[k][d + 1];
					}
					tagged_objects[k]->pop_back();
					return;
				}
			}
		}
	}
}

void Scene::AddTaggedObject(const GameObject * target)
{
	if (strcmp(target->GetTag(), "undefined") == 0)return;

	uint size = defined_tags.size();
	for (uint k = 0; k < size; k++)
	{
		if (strcmp(target->GetTag(), defined_tags[k].c_str()) == 0)
		{
			tagged_objects[k]->push_back((GameObject*)target);
		}
	}
}

uint Scene::GetTagID(const char * tag)
{
	uint size = defined_tags.size();
	for (uint k = 0; k < size; k++)
	{
		if (strcmp(tag, defined_tags[k].c_str()) == 0)
		{
			return k;
		}
	}
	return 0;
}

void Scene::ClearAllTags()
{
	// Now remove tagged_objects
	for (int i = 0; i < tagged_objects.size(); i++)
	{
		tagged_objects[i]->clear();
	}
}

std::vector<std::string>* Scene::GetTagsVec()
{
	return &defined_tags;
}

void Scene::ModificateParent(GameObject* child, GameObject* new_parent)
{
	if (child != nullptr && new_parent != nullptr)
	{
		if (child->IsDeleteFixed() || new_parent->IsDeleteFixed())
		{
			return;
		}
		if (child->HaveParent())
		{
			for (int i = 0; i < child->GetParent()->GetChildsVec().size(); i++)
			{
				if (child->GetParent()->GetChildsVec()[i] == child)
				{
					child->GetParent()->GetChildsPtr()->erase(child->GetParent()->GetChildsPtr()->begin() + i);
					break;
				}
			}
		}
		else
		{
			bool ereased = false;
			for (int i = 0; i < root->GetChildsVec().size(); i++)
			{
				if (root->GetChildsVec()[i] == child)
				{
					root->GetChildsPtr()->erase(root->GetChildsPtr()->begin() + i);
					ereased = true;
					break;
				}
			}
			if (!ereased)
			{
				for (int i = 0; i < temporary_scene->GetChildsVec().size(); i++)
				{
					if (temporary_scene->GetChildsVec()[i] == child)
					{
						temporary_scene->GetChildsPtr()->erase(temporary_scene->GetChildsPtr()->begin() + i);
						ereased = true;
						break;
					}
				}
			}
		}
		if (new_parent->GetUUID() == 1)
		{
			child->SetParent(nullptr);
		}
		else
		{
			child->SetParent(new_parent);
		}
		new_parent->AddChildGameObject(child);
		if (strcmp(new_parent->GetName(), "Temporary Scene") == 0 && App->gui->develop_mode == false)
		{
			// --------------------
			RemoveAllPointers(child);
		}
	}
}

void Scene::RemoveAllPointers(GameObject* gameobject)
{
	for (int i = 0; i < gameobject->GetNumChilds(); i++)
	{
		if (gameobject->GetNumChilds() > 0)
		{
			RemoveAllPointers(*(gameobject->GetChildsPtr()->begin() + i));
		}
	}
	for (int i = 0; i < gameobject->GetNumComponents(); i++)
	{
		Component* comp = gameobject->GetComponentbyIndex(i);
		switch (comp->GetType())
		{
		case Comp_Type::C_SCRIPT:
		{
			CompScript* script = (CompScript*)comp;
			if (script->csharp != nullptr)
			{
				//Access chsharp script, it contains a vector of all variables with their respective info
				for (uint i = 0; i < script->csharp->variables.size(); i++)
				{
					if (script->csharp->variables[i]->access == VarAccess::Var_PUBLIC)
					{
						if (script->csharp->variables[i]->type == VarType::Var_GAMEOBJECT)
						{
							script->csharp->variables[i]->game_object = nullptr;
						}
					}
				}
			}
			break;
		}
		case Comp_Type::C_IMAGE:
		{
			CompImage* image = (CompImage*)comp;
			image->SetNullCanvas();
			break;
		}
		case Comp_Type::C_TEXT:
		{
			CompText* text = (CompText*)comp;
			text->SetNullCanvas();
			break;
		}
		case Comp_Type::C_CHECK_BOX:
		{
			CompCheckBox* check_box = (CompCheckBox*)comp;
			check_box->Tick = nullptr;
			check_box->ClearLinkedScripts();
			break;
		}
		case Comp_Type::C_BUTTON:
		{
			CompButton* button = (CompButton*)comp;
			button->ClearLinkedScripts();
			break;
		}
		case Comp_Type::C_SLIDER:
		{
			//CompSlider* slider = (CompSlider*)comp;
			break;
		}
		case Comp_Type::C_CANVAS:
		{
			CompCanvas* canvas = (CompCanvas*)comp;
			canvas->ClearGraphicVector();
			break;
		}
		case Comp_Type::C_COLLIDER:
		{
			CompCollider* collider = (CompCollider*)comp;
			collider->ClearCompCollider();
			break;
		}
		}
	}
}

void Scene::GetAllGameObjectsWithoutParents(GameObject* parent, GameObject* root)
{
	for (int i = 0; i < root->GetNumChilds(); i++)
	{
		GetAllGameObjectsWithoutParents(parent, root->GetChildbyIndex(i));
	}
	parent->GetChildsPtr()->push_back(root);
}

void Scene::ChangeRoot(GameObject* parent, GameObject* root)
{
	for (int i = 0; i < root->GetNumChilds(); i++)
	{
		parent->GetChildsPtr()->push_back(root->GetChildbyIndex(i));
	}
	root->GetChildsPtr()->clear();
}	

void Scene::DrawPlane()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);
	for (int i = -size_plane; i <= size_plane; i++)
	{
		glVertex3f(-size_plane, 0, i);
		glVertex3f(size_plane, 0, i);
	}
	for (int i = -size_plane; i <= size_plane; i++)
	{
		glVertex3f(i, 0, size_plane);
		glVertex3f(i, 0, -size_plane);
	}

	glEnd();
}

void Scene::DrawCube(float size)
{
	float difamb[] = { 1.0f, 0.5f, 0.3f, 1.0f };
	glBegin(GL_QUADS);
	//front face
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, difamb);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(size / 2, size / 2, size / 2);
	glVertex3f(-size / 2, size / 2, size / 2);
	glVertex3f(-size / 2, -size / 2, size / 2);
	glVertex3f(size / 2, -size / 2, size / 2);

	//left face
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-size / 2, size / 2, size / 2);
	glVertex3f(-size / 2, size / 2, -size / 2);
	glVertex3f(-size / 2, -size / 2, -size / 2);
	glVertex3f(-size / 2, -size / 2, size / 2);

	//back face
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(size / 2, size / 2, -size / 2);
	glVertex3f(-size / 2, size / 2, -size / 2);
	glVertex3f(-size / 2, -size / 2, -size / 2);
	glVertex3f(size / 2, -size / 2, -size / 2);

	//right face
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(size / 2, size / 2, -size / 2);
	glVertex3f(size / 2, size / 2, size / 2);
	glVertex3f(size / 2, -size / 2, size / 2);
	glVertex3f(size / 2, -size / 2, -size / 2);

	//top face
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(size / 2, size / 2, size / 2);
	glVertex3f(-size / 2, size / 2, size / 2);
	glVertex3f(-size / 2, size / 2, -size / 2);
	glVertex3f(size / 2, size / 2, -size / 2);

	//bottom face
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(size / 2, -size / 2, size / 2);
	glVertex3f(-size / 2, -size / 2, size / 2);
	glVertex3f(-size / 2, -size / 2, -size / 2);
	glVertex3f(size / 2, -size / 2, -size / 2);
	glEnd();
}

// Before Rendering with the game camera (in Game Mode), fill a vector with all the static objects 
// of the quadtree to iterate them to apply Culling (if activated)
// When exiting Game Mode, this function is called again only for clearing this vector
//void Scene::FillStaticObjectsVector(bool fill)
//{
//	static_objects.clear();
//
//	if (fill)
//	{
//		quadtree.CollectObjects(static_objects);
//	}
//}

void Scene::RecalculateStaticObjects()
{
	//static_objects.clear();
	//octree.CollectAllObjects(static_objects);
}

const std::vector<GameObject*>* Scene::GetAllSceneObjects()
{
	game_objects_scene.clear();
	root->GetAllSceneGameObjects(game_objects_scene);
	return &game_objects_scene;
}

GameObject * Scene::FindCanvas()
{
	std::queue<GameObject*> queue;
	queue.push(root);
	CompCanvas* tmp = nullptr;
	while (!queue.empty())
	{
		tmp = (CompCanvas*)queue.front()->FindComponentByType(Comp_Type::C_CANVAS);
		if (tmp != nullptr)
		{
			return queue.front();
		}
		for (int i = 0;  i < queue.front()->GetChildsVec().size(); i++)
		{
		
			queue.push(queue.front()->GetChildsVec()[i]);
		}
		queue.pop();
	}
	return nullptr;
}

GameObject* Scene::CreateGameObject(GameObject* parent)
{
	GameObject* obj = new GameObject(parent);

	// SET NAME -----------------------------------
	static uint cube_count = 0;
	std::string name = "Empty ";
	name += std::to_string(cube_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	/* Empty GameObject only has Transform Component */
	//TRANSFORM COMPONENT --------------
	CompTransform* transform = (CompTransform*)obj->AddComponent(C_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1)); // TRANSFORM WILL ACCUMULATE PARENTS TRANSFORMS
	transform->Enable();

	if (parent == nullptr)
	{
		root->AddChildGameObject(obj);
	}

	return obj;
}

// -----------------------------------------------------------------------------
void Scene::DeleteAllGameObjects(GameObject* gameobject, bool isMain, bool is_reimport)
{
	GameObject* child_index = nullptr;
	for (int i = 0; i < gameobject->GetNumChilds(); i++)
	{
		child_index = gameobject->GetChildbyIndex(i);

		if (child_index->GetNumChilds() > 0)
		{
			DeleteAllGameObjects(child_index, false, is_reimport);
		}

		// First of all, Set nullptr all pointer to this GameObject
		if (App->camera->GetFocus() == child_index)
		{
			App->camera->SetFocusNull();
		}

		if (!App->mode_game)
		{
			if (((Inspector*)App->gui->win_manager[INSPECTOR])->GetSelected() == child_index)
			{
				((Inspector*)App->gui->win_manager[INSPECTOR])->SetLinkObjectNull();
			}
		}

		if (is_reimport == false)
		{
			App->importer->iScript->RemoveGObjectVarFromScripting(child_index);
			App->importer->iScript->RemoveGObjectFromMonoMap(child_index);
			App->importer->iScript->RemoveGObjectReferencesFromMonoScript(child_index);
		}

		// First delete all components
		if (child_index->GetNumComponents() > 0)
		{
			child_index->DeleteAllComponents();
		}
		// Now Delete GameObject
		if(child_index != nullptr)
		{
			if (!child_index->IsDeleteFixed())
			{
				RELEASE(child_index);
			}

			child_index = nullptr;
		}
		//Prevent acces to invalid memory
		gameobject->SetChildToNull(i);
	}
	gameobject->GetChildsPtr()->clear();

	if (isMain)
	{
		App->camera->SetFocusNull();
		App->renderer3D->SetGameCamera(nullptr);
		if (!App->mode_game)
		{
			((Inspector*)App->gui->win_manager[INSPECTOR])->SetLinkObjectNull();
		}
	}
}

void Scene::DeleteGameObject(GameObject* gameobject, bool isImport, bool is_reimport)
{
	if (gameobject != nullptr)
	{
		// First of all, Set nullptr all pointer to this GameObject
		if (App->camera->GetFocus() == gameobject)
		{
			App->camera->SetFocusNull();
		}

		if (App->mode_game == false)
		{
			if (((Inspector*)App->gui->win_manager[INSPECTOR])->GetSelected() == gameobject)
			{
				((Inspector*)App->gui->win_manager[INSPECTOR])->SetLinkObjectNull();
			}
		}

		if (is_reimport == false)
		{
			App->importer->iScript->RemoveGObjectVarFromScripting(gameobject);
			App->importer->iScript->RemoveGObjectFromMonoMap(gameobject);
			App->importer->iScript->RemoveGObjectReferencesFromMonoScript(gameobject);
		}

		// First Delete All Childs and their components
		if (gameobject->GetNumChilds() > 0)
		{
			DeleteAllGameObjects(gameobject, false, is_reimport);
		}

		// Then Delete Components
		if (gameobject->GetNumComponents() > 0)
		{
			gameobject->DeleteAllComponents();
		}

		// Finally Check have Parent and remove from childs
		if (gameobject->GetParent() != nullptr)
		{
			LOG("[green] DELETE");
			int index = gameobject->GetParent()->GetIndexChildbyGO(gameobject);
			if (index != -1)
			{
				gameobject->GetParent()->RemoveChildbyIndex(index);
			}
			gameobject = nullptr;
		}

		else if (isImport == false && is_reimport == false)
		{
			for (int i = 0; i < root->GetNumChilds(); i++)
			{
				if (gameobject == root->GetChildbyIndex(i))
				{
					root->RemoveChildbyIndex(i);
					gameobject = nullptr;
					break;
				}
			}
		}
		else
		{
			gameobject->GetChildsPtr()->clear();
			RELEASE(gameobject);
		}
	}
}
bool Scene::CheckDeletedObjcet(uint uuid)
{
	// Return false if the object had been deleted and true if has to be deleted
	for (uint i = 0; i < deleted_objects.size(); i++)
	{
		if (deleted_objects[i] == uuid)
		{
			return false;
		}
	}
	deleted_objects.push_back(uuid);
	return true;
}
// -------------------------------------------------------------------------------------

Component* Scene::BlitSceneComponentsAsButtons(Comp_Type type, std::string& current_item)
{
	Component* ret = nullptr;
	if (type != actualType)
	{
		temp_vector.clear();
		root->GetComponentsByType(type, &temp_vector, true);
	}

	//ImGui::PushItemWidth(150);
	ImGui::Text("Select: "); ImGui::SameLine();

	

	if (ImGui::BeginCombo("##SelectComponent", current_item.c_str()))
	{
		static char comp_name[100];
		ImGui::InputText("Name: ", comp_name, 99); 

		for (int i = 0; i < temp_vector.size(); i++)
		{
			std::string tmp = temp_vector[i]->GetParent()->GetName();
			std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

			if (tmp.find(comp_name) == std::string::npos)
			{
			 continue;
			}

			char buffer[100];
			if (temp_vector[i]->GetType() == Comp_Type::C_SCRIPT)
			{
				sprintf(buffer, "%s.%s", temp_vector[i]->GetParent()->GetName(), ((CompScript*)temp_vector[i])->GetScriptName());
			}
			else
			{
				sprintf(buffer, "%s.%s", temp_vector[i]->GetParent()->GetName(), temp_vector[i]->GetName());
			}
			if (ImGui::Selectable(buffer))
			{
				current_item = buffer;
				ret = temp_vector[i];
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	return ret;
}

bool Scene::SaveConfig(JSON_Object* node)
{
	json_object_set_number(node, "Skybox Index", skybox_index);
	return true;
}

GameObject* Scene::CreateCube(GameObject* parent)
{
	GameObject* obj = new GameObject(parent);

	// SET NAME -----------------------------------
	static uint cube_count = 0;
	std::string name = "Cube ";
	name += std::to_string(cube_count++);
	char* name_str = new char[name.size()+1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	/* Predefined Cube has 3 Base components: Transform, Mesh & Material */

	//TRANSFORM COMPONENT --------------
	CompTransform* transform = (CompTransform*)obj->AddComponent(C_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1)); // TRANSFORM WILL ACCUMULATE PARENTS TRANSFORMS
	transform->Enable();

	CompMesh* mesh = (CompMesh*)obj->AddComponent(C_MESH);
	mesh->Enable();
	mesh->resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(2); // 2 == Cube
	if (mesh->resource_mesh != nullptr)
	{
		mesh->resource_mesh->num_game_objects_use_me++;
		// LOAD MESH
		if (mesh->resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
		{
			App->importer->iMesh->LoadResource(std::to_string(mesh->resource_mesh->GetUUID()).c_str(), mesh->resource_mesh);
		}
	}
	OBB* box = new OBB();
	box->pos = float3::zero;
	box->r = float3::one;
	box->axis[0] = float3(1, 0, 0);
	box->axis[1] = float3(0, 1, 0);
	box->axis[2] = float3(0, 0, 1);


	//MATERIAL COMPONENT -------------------
	CompMaterial* mat = (CompMaterial*)obj->AddComponent(C_MATERIAL);
	mat->Enable();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("CUBE Created.");
	RELEASE(box);
	return obj;
}

GameObject * Scene::CreatePlane(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);

	// SET NAME -----------------------------------
	static uint plane_count = 0;
	std::string name = "Plane ";
	name += std::to_string(plane_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	/* Predefined Cube has 3 Base components: Transform, Mesh & Material */

	//TRANSFORM COMPONENT --------------
	CompTransform* transform = (CompTransform*)obj->AddComponent(C_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1)); // TRANSFORM WILL ACCUMULATE PARENTS TRANSFORMS
	transform->Enable();

	CompMesh* mesh = (CompMesh*)obj->AddComponent(C_MESH);
	mesh->Enable();
	mesh->resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(4); //3 == Plane
	if (mesh->resource_mesh != nullptr)
	{
		mesh->resource_mesh->num_game_objects_use_me++;
		// LOAD MESH
		if (mesh->resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
		{
			App->importer->iMesh->LoadResource(std::to_string(mesh->resource_mesh->GetUUID()).c_str(), mesh->resource_mesh);
		}
	}
	OBB* box = new OBB();
	box->pos = float3::zero;
	box->r = float3::one;
	box->axis[0] = float3(1, 0, 0);
	box->axis[1] = float3(0, 1, 0);
	box->axis[2] = float3(0, 0, 1);


	//MATERIAL COMPONENT -------------------
	CompMaterial* mat = (CompMaterial*)obj->AddComponent(C_MATERIAL);
	mat->Enable();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("Plane Created.");
	RELEASE(box);
	return obj;
}

GameObject* Scene::CreateMainCamera(GameObject* parent)
{
	GameObject* obj = new GameObject(parent);

	// SET NAME -----------------------------------
	obj->SetName("MainCamera");

	/* Predefined Main Camera has 2 Base components: Transform & Camera */

	// TRANSFORM COMPONENT --------------
	CompTransform* transform = (CompTransform*)obj->AddComponent(C_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// CAMERA COMPONENT -----------------
	CompCamera* camera = (CompCamera*)obj->AddComponent(C_CAMERA);
	camera->Enable();
	camera->SetMain(true);
	camera->SetCullDynamics(false);

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("MAIN CAMERA Created.");

	return obj;
}

GameObject * Scene::CreateMainLight(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);

	// SET NAME -----------------------------------
	std::string name = "Main light";
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	/* Predefined Main Camera has 2 Base components: Transform & Camera */

	// TRANSFORM COMPONENT --------------
	CompTransform* transform = (CompTransform*)obj->AddComponent(C_TRANSFORM);
	transform->Init(float3(0, 60, -40), float3(-40, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// CAMERA COMPONENT -----------------
	CompLight* camera = (CompLight*)obj->AddComponent(C_LIGHT);
	camera->type = Light_type::DIRECTIONAL_LIGHT;
	camera->ui_light_type = 1;
	//camera->types_lights= 1;
	//camera->Enable();
	//camera->SetMain(true);

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("MAIN CAMERA Created.");

	return obj;
}

GameObject * Scene::CreateCanvas(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);

	// SET NAME -----------------------------------
	static uint canvas_count = 0;

	std::string name = "Canvas ";
	name += std::to_string(canvas_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	// TRANSFORM COMPONENT --------------
	CompRectTransform* transform = (CompRectTransform*)obj->AddComponent(Comp_Type::C_RECT_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// CANVAS COMPONENT -----------------
	CompCanvas* canvas = (CompCanvas*)obj->AddComponent(Comp_Type::C_CANVAS);
	canvas->Enable();
	canvas->SetDefaultTexture(App->renderer3D->id_checkImage);
	canvas->SetDefaultUIShader(App->module_shaders->CreateDefaultShader("default shader", UIShaderFrag, UIShaderVert));

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("CANVAS Created.");

	return obj;
}

GameObject * Scene::CreateImage(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);


	// SET NAME -----------------------------------
	static uint image_count = 0;

	std::string name = "Image ";
	name += std::to_string(image_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	// TRANSFORM COMPONENT --------------
	CompRectTransform* transform = (CompRectTransform*)obj->AddComponent(Comp_Type::C_RECT_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// IMAGE COMPONENT -----------------
	CompImage* image = (CompImage*)obj->AddComponent(Comp_Type::C_IMAGE);
	image->Enable();
	image->SyncComponent(nullptr);
	image->UpdateSpriteId();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("IMAGE Created.");

	return obj;
}

GameObject * Scene::CreateButton(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);


	// SET NAME -----------------------------------
	static uint button_count = 0;

	std::string name = "Button ";
	name += std::to_string(button_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	// TRANSFORM COMPONENT --------------
	CompRectTransform* transform = (CompRectTransform*)obj->AddComponent(Comp_Type::C_RECT_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// IMAGE COMPONENT -----------------
	CompImage* image = (CompImage*)obj->AddComponent(Comp_Type::C_IMAGE);
	image->Enable();
	image->SyncComponent(nullptr);
	image->UpdateSpriteId();

	// BUTTON COMPONENT -----------------
	CompButton* button = (CompButton*)obj->AddComponent(Comp_Type::C_BUTTON);
	button->Enable();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("BUTTON Created.");

	return obj;
}

GameObject * Scene::CreateCheckBox(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);


	// SET NAME -----------------------------------
	static uint check_box_count = 0;

	std::string name = "Check Box ";
	name += std::to_string(check_box_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	// TRANSFORM COMPONENT --------------
	CompRectTransform* transform = (CompRectTransform*)obj->AddComponent(Comp_Type::C_RECT_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// IMAGE COMPONENT -----------------
	CompImage* image = (CompImage*)obj->AddComponent(Comp_Type::C_IMAGE);
	image->Enable();
	image->SyncComponent(nullptr);
	image->UpdateSpriteId();
	// BUTTON COMPONENT -----------------
	CompCheckBox* check_box = (CompCheckBox*)obj->AddComponent(Comp_Type::C_CHECK_BOX);
	check_box->Enable();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("CHECK BOX Created.");

	return obj;
}

GameObject * Scene::CreateSlider(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);


	// SET NAME -----------------------------------
	static uint slider_count = 0;

	std::string name = "Slider ";
	name += std::to_string(slider_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	// TRANSFORM COMPONENT --------------
	CompRectTransform* transform = (CompRectTransform*)obj->AddComponent(Comp_Type::C_RECT_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// IMAGE COMPONENT -----------------
	CompImage* image = (CompImage*)obj->AddComponent(Comp_Type::C_IMAGE);
	image->Enable();
	image->SyncComponent(nullptr);
	image->UpdateSpriteId();
	// SLIDER -----------------
	CompSlider* slide = (CompSlider*)obj->AddComponent(Comp_Type::C_SLIDER);
	slide->Enable();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}

	LOG("SLIDER Created.");

	return obj;
}

GameObject * Scene::CreateText(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);


	// SET NAME -----------------------------------
	static uint text_count = 0;

	std::string name = "Text Box ";
	name += std::to_string(text_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);
	//TRANSFORM -------------
	CompRectTransform* transform = (CompRectTransform*)obj->AddComponent(Comp_Type::C_RECT_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();
	// TEXT COMPONENT -----------------
	CompText* text = (CompText*)obj->AddComponent(Comp_Type::C_TEXT);
	text->Enable();
	text->SyncComponent(nullptr);



	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}
	LOG("TEXT Created.");

	return obj;
}

GameObject * Scene::CreateEditText(GameObject * parent)
{
	GameObject* obj = new GameObject(parent);


	// SET NAME -----------------------------------
	static uint edit_text_count = 0;

	std::string name = "Edit Text ";
	name += std::to_string(edit_text_count++);
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	// TRANSFORM ----------------
	CompRectTransform* transform = (CompRectTransform*)obj->AddComponent(Comp_Type::C_RECT_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// TEXT COMPONENT -----------------
	CompText* text = (CompText*)obj->AddComponent(Comp_Type::C_TEXT);
	text->Enable();
	text->SyncComponent(nullptr);

	// TEXT COMPONENT -----------------
	CompEditText* edit_text = (CompEditText*)obj->AddComponent(Comp_Type::C_EDIT_TEXT);
	text->Enable();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->root->AddChildGameObject(obj);
	}
	LOG("EDIT TEXT Created.");
	return obj;
}
