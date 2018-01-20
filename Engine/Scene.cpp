#include "Globals.h"
#include "Application.h"
#include "Scene.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "Component.h"
#include "CompTransform.h"
#include "CompMesh.h"
#include "ResourceMesh.h"
#include "ImportMesh.h"
#include "CompMaterial.h"
#include "WindowInspector.h"
#include "CompCamera.h"
#include "MathGeoLib.h"
#include "Quadtree.h"
#include "JSONSerialization.h"
#include "SkyBox.h"

#include "Gl3W/include/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"

#include <direct.h>

//#include "..\..\3D-Engine\ScriptingSystem\ScriptingSystem\ScriptManager.h"

#define SPHERE_DEFINITION 1536

Scene::Scene(bool start_enabled) : Module(start_enabled)
{
	Start_enabled = true;
	preUpdate_enabled = true;
	Update_enabled = true;

	name = "Scene";
	haveConfig = true;
}

Scene::~Scene()
{
	DeleteGameObjects(gameobjects, true);
	RELEASE(sceneBuff);
	RELEASE(skybox);
}

//bool Scene::Init(JSON_Object * node)
//{
//	perf_timer.Start();

//	Awake_t = perf_timer.ReadMs();
//	return true;
//}

bool Scene::Start()
{
	perf_timer.Start();

	/* Init Quadtree */
	size_quadtree = 50.0f;
	quadtree.Init(size_quadtree);

	/* Set size of the plane of the scene */
	size_plane = 50;

	/* Create Default Main Camera Game Object */
	CreateMainCamera(nullptr);

	////TEST 
	//ScriptManager Jordi;
	//Jordi.Compile("C:/Users/Administrador/Documents/GitHub/3D-Engine/Engine/Game/Assets/Example_test.cs");
	
	icon_options_transform = App->textures->LoadTexture("Images/UI/icon_options_transform.png");

	/* Init Skybox */ 
	skybox = new SkyBox();
	skybox->InitSkybox();
	skybox_index = 1;
	draw_skybox = true;

	Start_t = perf_timer.ReadMs();
	return true;
}

update_status Scene::PreUpdate(float dt)
{
	perf_timer.Start();

	// PreUpdate GameObjects ------------------------
	for (uint i = 0; i < gameobjects.size(); i++)
	{
		if (gameobjects[i]->WanttoDelete() == false)
		{
			gameobjects[i]->preUpdate(dt);
		}
		else
		{
			DeleteGameObject(gameobjects[i]);
		}
	}

	//// PreUpdate GameObjects ------------------------
	//for (uint i = 0; i < gameobjects_inscene->GetNumChilds(); i++)
	//{
	//	if (gameobjects_inscene->WanttoDelete() == false) //Scene
	//	{
	//		gameobjects_inscene->preUpdate(dt);
	//	}
	//}

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status Scene::Update(float dt)
{
	perf_timer.Start();

	// Update GameObjects -----------
	for (uint i = 0; i < gameobjects.size(); i++)
	{
		gameobjects[i]->Update(dt);
	}
	// -------------------------------------------------

	Update_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

//update_status Scene::PostUpdate(float dt)
//{
//	perf_timer.Start();
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
	
	return UPDATE_CONTINUE;
}

bool Scene::CleanUp()
{
	skybox->DeleteSkyboxTex();
	ClearAllVariablesScript();
	for (uint i = 0; i < gameobjects.size(); i++)
	{
		gameobjects[i]->CleanUp();
	}
	return true;
}

void Scene::EditorQuadtree()
{
	//QUADTREE EDITOR ---------------------------------------------------------
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.761f, 0.00f, 1.00f));
	if (ImGui::TreeNodeEx("QUADTREE", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PopStyleColor();

		/* Enable Debug Draw */
		ImGui::Checkbox("##quadtreedraw", &quadtree_draw); ImGui::SameLine();
		ImGui::Text("Draw Quadtree");
		ImGui::SliderFloat("Size", &size_quadtree, 50, 300);

		/* Remake Quadtree with actual static objects*/
		if (ImGui::Button("UPDATE QUADTREE"))
		{
			if (App->engineState == EngineState::STOP)
			{
				if (size_quadtree != quadtree.size)
				{
					quadtree.Init(size_quadtree);
				}
				else
				{
					quadtree.root_node->Clear();
				}
				quadtree.Bake(App->scene->gameobjects);
			}
			else
			{
				LOG("Update Quadtree not possible while GAME MODE is ON");
			}
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
	for (int i = 0; i < gameobjects.size(); i++)
	{
		gameobjects[i]->CheckScripts(fails);
	}
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
	for (int i = 0; i < gameobjects.size(); i++)
	{
		gameobjects[i]->StartScripts();
	}
}

void Scene::ClearAllVariablesScript()
{
	//Iterate all GameObjects and, if they have scripts, call their ClearAllVariablesScript
	for (int i = 0; i < gameobjects.size(); i++)
	{
		gameobjects[i]->ClearAllVariablesScript();
	}
}

void Scene::SetScriptVariablesToNull(GameObject * go)
{
	for (uint i = 0; i < gameobjects.size(); i++)
	{
		gameobjects[i]->RemoveScriptReference(go);
	}
}

GameObject* Scene::GetGameObjectfromScene(bool& active)
{
	if (!ImGui::Begin("GameObjects in Scene", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
	{
		ImGui::End();
	}
	else
	{
		for (int i = 0; i < gameobjects.size(); i++)
		{
			ImGui::PushID(i);
			GameObject* temp = gameobjects[i]->GetGameObjectfromScene(i);
			if (temp != nullptr)
			{
				ImGui::PopID();
				ImGui::End();
				active = false;
				return temp;
			}
			//if(ImGui::Selectable(gameobjects[i]->GetName()))
			//{

			//}
			ImGui::PopID();
		}
		ImGui::End();
	}
	return nullptr;
}

GameObject* Scene::GetGameObjectbyuid(uint uid)
{
	for (int i = 0; i < gameobjects.size(); i++)
	{
		GameObject* ret = gameobjects[i]->GetGameObjectbyuid(uid);
		if (ret != nullptr)
		{
			return ret;
		}
	}
	return nullptr;
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
void Scene::FillStaticObjectsVector(bool fill)
{
	static_objects.clear();

	if (fill)
	{
		quadtree.CollectObjects(static_objects);
	}
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
		gameobjects.push_back(obj);
	}
	return obj;
}

void Scene::DeleteGameObjects(std::vector<GameObject*>& gameobjects, bool isMain)
{
	for (int i = 0; i < gameobjects.size(); i++)
	{
		if (gameobjects[i]->GetNumChilds() > 0)
		{
			DeleteGameObjects(gameobjects[i]->GetChildsVec(), false);
		}
		// First of all, Set nullptr all pointer to this GameObject
		if (App->camera->GetFocus() == gameobjects[i])
		{
			App->camera->SetFocusNull();
		}
		if (((Inspector*)App->gui->winManager[INSPECTOR])->GetSelected() == gameobjects[i])
		{
			((Inspector*)App->gui->winManager[INSPECTOR])->SetLinkObjectNull();
		}
		// First delete all components
		if (gameobjects[i]->GetNumComponents() > 0)
		{
			gameobjects[i]->DeleteAllComponents();
		}
		// Now Delete GameObject
		GameObject* it = gameobjects[i];
		if(it != nullptr)
		{
			if (!it->isDeleteFixed())
			{
				RELEASE(it);
			}

			it = nullptr;
		}
	}
	gameobjects.clear();

	if (isMain)
	{
		App->camera->SetFocusNull();
		App->renderer3D->SetGameCamera(nullptr);
		((Inspector*)App->gui->winManager[INSPECTOR])->SetLinkObjectNull();
	}
}

void Scene::DeleteGameObject(GameObject* gameobject, bool isImport)
{
	if (gameobject != nullptr)
	{
		// First of all, Set nullptr all pointer to this GameObject
		if (App->camera->GetFocus() == gameobject)
		{
			App->camera->SetFocusNull();
		}

		SetScriptVariablesToNull(gameobject);

		if (((Inspector*)App->gui->winManager[INSPECTOR])->GetSelected() == gameobject)
		{
			((Inspector*)App->gui->winManager[INSPECTOR])->SetLinkObjectNull();
		}
		// First Delete All Childs and their components
		if (gameobject->GetNumChilds() > 0)
		{
			DeleteGameObjects(gameobject->GetChildsVec(), false);
		}
		// Then Delete Components
		if (gameobject->GetNumComponents() > 0)
		{
			gameobject->DeleteAllComponents();
		}

		// Finnaly Check have Parent and remove from childs
		if (gameobject->GetParent() != nullptr)
		{
			int index = gameobject->GetParent()->GetIndexChildbyName(gameobject->GetName());
			gameobject->GetParent()->RemoveChildbyIndex(index);
		}

		else if (isImport == false)
		{
			int index = 0;
			for (int i = 0; i < gameobjects.size(); i++)
			{
				if (strcmp(gameobject->GetName(), gameobjects[i]->GetName()) == 0)
				{
					index = i;
				}
			}
			std::vector<GameObject*>::iterator item = gameobjects.begin();
			for (int i = 0; i < gameobjects.size(); i++)
			{
				if (i == index)
				{
					GameObject* it = gameobjects[i];
					RELEASE(it);
					gameobjects.erase(item);
					it = nullptr;
					break;
				}
				item++;
			}
		}
		else
		{
			gameobject->GetChildsPtr()->clear();
			RELEASE(gameobject);
		}
	}
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
	mesh->resourceMesh = (ResourceMesh*)App->resource_manager->GetResource(1); // 1 == Cube
	if (mesh->resourceMesh != nullptr)
	{
		mesh->resourceMesh->NumGameObjectsUseMe++;
		// LOAD MESH
		if (mesh->resourceMesh->IsLoadedToMemory() == Resource::State::UNLOADED)
		{
			App->importer->iMesh->LoadResource(std::to_string(mesh->resourceMesh->GetUUID()).c_str(), mesh->resourceMesh);
		}
	}
	OBB* box = new OBB();
	box->pos = float3::zero;
	box->r = float3::one;
	box->axis[0] = float3(1, 0, 0);
	box->axis[1] = float3(0, 1, 0);
	box->axis[2] = float3(0, 0, 1);

	obj->bounding_box = new AABB(*box);

	//MATERIAL COMPONENT -------------------
	CompMaterial* mat = (CompMaterial*)obj->AddComponent(C_MATERIAL);
	mat->Enable();

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->gameobjects.push_back(obj);
	}

	LOG("CUBE Created.");
	RELEASE(box);
	return obj;
}

GameObject* Scene::CreateMainCamera(GameObject* parent)
{
	GameObject* obj = new GameObject(parent);

	// SET NAME -----------------------------------
	std::string name = "MainCamera";
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj->SetName(name_str);

	/* Predefined Main Camera has 2 Base components: Transform & Camera */

	// TRANSFORM COMPONENT --------------
	CompTransform* transform = (CompTransform*)obj->AddComponent(C_TRANSFORM);
	transform->Init(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	transform->Enable();

	// CAMERA COMPONENT -----------------
	CompCamera* camera = (CompCamera*)obj->AddComponent(C_CAMERA);
	camera->Enable();
	camera->SetMain(true);

	if (parent == nullptr)
	{
		// Only add to GameObjects list the Root Game Objects
		App->scene->gameobjects.push_back(obj);
	}

	LOG("MAIN CAMERA Created.");

	return obj;
}