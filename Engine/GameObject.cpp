#include "GameObject.h"
#include "Application.h"
#include "WindowInspector.h"
#include "WindowHierarchy.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleImporter.h"
#include "ImportScript.h"
#include "ModuleFS.h"
#include "Scene.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "Component.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "CompRectTransform.h"
#include "CompMaterial.h"
#include "CompCamera.h"
#include "CompScript.h"
#include "CompAnimation.h"
#include "CompUIAnimation.h"
#include "CompButton.h"
#include "CompCheckBox.h"
#include "CompImage.h"
#include "CompSlider.h"
#include "CompText.h"
#include "CompEditText.h"
#include "CompCanvas.h"
#include "CompCanvasRender.h"
#include "CompAudio.h"
#include "JSONSerialization.h"
#include "CompBone.h"
#include "CompFSM.h"
#include "CompLight.h"
#include "CompCollider.h"
#include "CompRigidBody.h"
#include "CompJoint.h"
#include "CompParticleSystem.h"
#include "ResourceMesh.h"
#include "CompCubeMapRenderer.h"
#include <queue>

//Event system test
#include "ModuleEventSystemV2.h"

GameObject::GameObject(GameObject* parent) : parent(parent)
{
	Enable();
	SetVisible(true);
	uid = App->random->Int();

	if (parent != nullptr)
	{
		// Push this game object into the childs list of its parent
		parent->childs.push_back(this);
		parent_active = parent->IsParentActive();
	}


	box_fixed.SetNegativeInfinity();
}

GameObject::GameObject(std::string nameGameObject) : parent(nullptr)
{
	Enable();
	SetVisible(true);
	uid = App->random->Int();
	name = nameGameObject;

	box_fixed.SetNegativeInfinity();
}

GameObject::GameObject(std::string nameGameObject, uint uuid) : parent(nullptr)
{
	Enable();
	SetVisible(true);
	uid = uuid;
	name = nameGameObject;

	box_fixed.SetNegativeInfinity();
}

GameObject::GameObject(const GameObject& copy, bool haveparent, GameObject* parent_)
{
	// New UUID and name for this copy
	uid = App->random->Int();
	std::string nametemp = copy.GetName();
	size_t EndName = nametemp.find_last_of("(");
	nametemp = nametemp.substr(0, EndName - 1);
	NameNotRepeat(nametemp, haveparent, parent_);
	//nametemp += " (copy)";
	name = nametemp.c_str();

	// Same data as the 'copy' gameobject
	active = copy.IsActive();
	parent_active = parent->IsParentActive();
	visible = copy.IsVisible();
	static_obj = copy.IsStatic();
	bb_active = copy.IsAABBActive();

	if (copy.box_fixed.IsFinite())
	{
		box_fixed = copy.box_fixed;
	}
	else
	{
		box_fixed.SetNegativeInfinity();
	}

	//Create all components from copy object with same data
	for (uint i = 0; i < copy.GetNumComponents(); i++)
	{
		AddComponentCopy(*copy.components[i]);
	}

	//Create childrens from copy object with same data to this new game object
	for (uint i = 0; i < copy.GetNumChilds(); i++)
	{
		//Create from copy constructor all childs of the game object to copy
		childs.push_back(new GameObject(*copy.GetChildbyIndex(i), haveparent, parent_));
		childs[i]->parent = this;
	}
}

GameObject::~GameObject()
{
	//RELEASE_ARRAY(name); FIX THIS
	parent = nullptr;

	if (!static_obj)
	{
		App->scene->RemoveDynamicObject(this);
	}

	if (components.size() > 0)
	{
		for (uint i = 0; i < components.size(); i++)
		{
			components[i]->Clear();
			if (components[i]->GetType() == C_LIGHT)
			{
				Event delete_light_event;
				delete_light_event.Set_event_data(EventType::EVENT_DELETE_LIGHT);
				delete_light_event.delete_light.light = (CompLight*)components[i];
				delete_light_event.delete_light.light->to_delete = true;
				PushEvent(delete_light_event);
			}
			else RELEASE(components[i]);
		}
		components.clear();
	}

	if (childs.size() > 0)
	{
		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i]->CleanUp();
			RELEASE(childs[i]);
		}
		childs.clear();
	}
}

bool GameObject::CheckScripts(int& numfails)
{
	int allScriptsCompiled = 0;
	if (active)
	{
		//Check compilation errors --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->IsActive())
			{
				if (components[i]->GetType() == Comp_Type::C_SCRIPT)
				{
					if (((CompScript*)components[i])->CheckScript() == false)
					{
						allScriptsCompiled++;
					}
				}
			}
		}

		//Check child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->IsActive())
			{
				childs[i]->CheckScripts(numfails);
			}
		}
	}

	numfails = allScriptsCompiled;
	if (allScriptsCompiled == 0)
	{
		//LOG("All Scripts are succesfully compiled.");
		return true;
	}
	//LOG("[error] total scripts failed: %i", allScriptsCompiled);
	return false;
}

void GameObject::StartScripts()
{
	if (active)
	{
		if (static_obj)
		{
			static_obj_gm = true;
		}
		//Start Active scripts --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->IsActive())
			{
				if (components[i]->GetType() == Comp_Type::C_SCRIPT)
				{
					((CompScript*)components[i])->Start();
				}
			}
		}

		//Check child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->IsActive())
			{
				childs[i]->StartScripts();
			}
		}
	}
}

GameObject* GameObject::GetGameObjectbyuid(uint uid)
{
	std::queue<GameObject*> obj_queue;

	if (active)
	{
		obj_queue.push(this);

		while (!obj_queue.empty())
		{
			// Return GameObject with specified UID
			if (obj_queue.front()->GetUUID() == uid)
			{
				return obj_queue.front();
			}

			// Check child Game Objects -------------------
			for (uint i = 0; i < obj_queue.front()->childs.size(); i++)
			{
				obj_queue.push(obj_queue.front()->childs[i]);
			}

			obj_queue.pop();
		}
	}
	return nullptr;
}

GameObject* GameObject::GetGameObjectfromScene(int id)
{
	if (active)
	{
		bool show_childs = false;
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (childs.size() == 0)
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (ImGui::TreeNodeEx(GetName(), node_flags))
		{
			show_childs = true;
		}
		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
		{
			//LOG("%.2f - %.2f  / /  %.2f - %.2f", ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				if (show_childs)
					ImGui::TreePop();
				return this;
			}
		}
		if (show_childs)
		{
			//Check child Game Objects -------------------
			for (uint i = 0; i < childs.size(); i++)
			{
				GameObject* temp = childs[i]->GetGameObjectfromScene(id);
				if (temp != nullptr)
				{
					ImGui::TreePop();
					return temp;
				}
			}
			ImGui::TreePop();
		}
	}
	return nullptr;
}

void GameObject::GetAllSceneGameObjects(std::vector<GameObject*>& SceneGameObjects) const
{
	SceneGameObjects.push_back((GameObject*)this);

	for (std::vector<GameObject*>::const_iterator item = childs.cbegin(); item != childs.cend(); ++item)
		(*item)->GetAllSceneGameObjects(SceneGameObjects);
}

void GameObject::FindChildsWithTag(const char * tag, std::vector<GameObject*>* vec)
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (strcmp(childs[k]->GetTag(), tag) == 0)
		{
			vec->push_back(childs[k]);
		}
		childs[k]->FindChildsWithTag(tag, vec);
	}
}

GameObject * GameObject::FindGameObjectWithTag(const char * tag)
{
	return App->scene->FindGameObjectWithTag(tag);
}

void GameObject::Init()
{
}

void GameObject::PreUpdate(float dt)
{
	to_delete = false;

	if (active)
	{
		//preUpdate Components --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->WantDelete())
			{
				DeleteComponent(i);
			}
			else
			{
				if (components[i]->IsActive())
				{
					components[i]->PreUpdate(dt);
				}
				else
				{
					if (components[i]->GetType() == Comp_Type::C_SCRIPT)
					{
						if (((CompScript*)components[i])->p_active || ((CompScript*)components[i])->do_start)
						{
							components[i]->PreUpdate(dt);
						}
					}
				}
			}
		}

		//preUpdate child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->to_delete)
			{
				App->scene->DeleteGameObject(childs[i]);
			}
			else
			{
				if (childs[i]->IsActive())
				{
					childs[i]->PreUpdate(dt);
				}
			}
		}
	}
}

void GameObject::Update(float dt)
{
	if (active)
	{
		//Update Components --------------------------
		for (uint i = static_obj_gm; i < components.size(); i++)
		{
			if (components[i]->IsActive())
			{
				components[i]->Update(dt);
			}
		}

		//Update child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->IsActive())
			{
				childs[i]->Update(dt);
			}
		}
	}
}

//void GameObject::postUpdate()
//{
//}

bool GameObject::CleanUp()
{

	box_fixed.SetNegativeInfinity();


	//preUpdate Components --------------------------
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->Clear();
	}

	//preUpdate child Game Objects -------------------
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->CleanUp();
	}
	return true;
}

void GameObject::FixedDelete(bool check_delete)
{
	to_delete = check_delete;
}

bool GameObject::IsDeleteFixed() const
{
	return to_delete;
}

void GameObject::Draw()
{
	if (visible && active && parent_active)
	{
		bool comp_active = false;
		Component* comp = nullptr;
		Comp_Type type = Comp_Type::C_TRANSFORM;
		//Draw Components --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			comp = components[i];
			type = comp->GetType();
			if (comp->IsActive())
			{
				if (type == Comp_Type::C_MESH)
				{
					//components[i]->Draw();
					/**/
					CompMesh* comp_mesh = (CompMesh*)comp;
					Event draw_event;
					draw_event.Set_event_data(EventType::EVENT_DRAW);
					if (comp_mesh->GetMaterial()->material->alpha < 1.0f) draw_event.draw.Dtype = draw_event.draw.DRAW_3D_ALPHA;
					else draw_event.draw.Dtype = draw_event.draw.DRAW_3D;
					draw_event.draw.ToDraw = comp;
					PushEvent(draw_event);
					/**/
				}
				else if (type == Comp_Type::C_CAMERA)
				{
					comp->Draw();
				}
				else if (type == Comp_Type::C_PARTICLE_SYSTEM)
				{
					comp->Draw();
				}
				else if (type == Comp_Type::C_ANIMATION)
				{
					comp->Draw();
				}
				else if (type == Comp_Type::C_LIGHT)
				{
					CompLight* l = (CompLight*)comp;
					l->use_light_to_render = true;
					Event draw_event;
					draw_event.Set_event_data(EventType::EVENT_REQUEST_3D_3DA_MM);
					draw_event.request_3d3damm.light = (CompLight*)comp;
					PushEvent(draw_event);
					components[i]->Draw();
								
				}
			}


		}
		/*
		//Draw child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			//Static objects will be drawn using camera culling
			if (childs[i]->IsActive() && !childs[i]->IsStatic())
			{
				childs[i]->Draw();
			}
		}*/
		
		if (bb_active)
		{
			// Draw Bounding Box
			DrawBoundingBox();
		}
	}
}

bool GameObject::Enable()
{
	if (!active)
	{
		SetActive(true);
	}

	return active;
}

bool GameObject::Disable()
{
	if (active)
	{
		SetActive(false);
	}
	return active;
}

void GameObject::SetName(const char * name)
{
	this->name = name;
}

void GameObject::SetTag(const char * tag)
{
	if (strcmp(this->tag.c_str(), "undefined") != 0)
	{
		App->scene->RemoveTaggedObject(this);
	}
	this->tag = tag;
	App->scene->AddTaggedObject(this);
}

const char* GameObject::GetName() const
{
	return name.c_str();
}

const char * GameObject::GetTag() const
{
	return tag.c_str();
}

bool GameObject::CompareTag(const char * str) const
{
	return strcmp(tag.c_str(), str) == 0;
}

void GameObject::NameNotRepeat(std::string& name, bool haveParent, GameObject* parent_)
{
	bool stop = false;
	uint i = 0;
	std::string nameRepeat = name;

	if (haveParent == false)
	{
		parent_ = App->scene->root;
	}

	while (stop == false)
	{

		if (i < parent_->GetNumChilds())
		{
			bool stop_research = false;
			uint j = 0;
			bool haveRepeat = false;
			while (stop_research == false)
			{
				if (j < parent_->GetNumChilds())
				{
					std::string nameChild = parent_->GetChildbyIndex(j)->GetName();
					if (nameRepeat == nameChild)
					{
						haveRepeat = true;
					}
				}
				else
				{
					stop_research = true;
				}
				j++;
			}
			if (haveRepeat == false)
			{
				stop_research = true;
				stop = true;
				name = nameRepeat;
			}
			nameRepeat = name;
			nameRepeat += " (" + std::to_string(i + 1) + ")";
		}
		else
		{
			stop = true;
		}
		i++;
	}
}

void GameObject::ShowHierarchy(bool use_search)
{
	if (!IsVisible())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.385f, 0.385f, 0.385f, 1.00f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
	}
	bool treeNod = false;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetSelected() == this)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (childs.size() == 0)
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf;
	}
	if (GetUUID() == 1) // this is Scene
	{
		node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	//ImGui::SetNextTreeNodeOpen(true);
	if (set_next_tree_node_open && App->gui->develop_mode)
	{
		ImGui::SetNextTreeNodeOpen(true);
		set_next_tree_node_open = false;
	}
	if (ImGui::TreeNodeEx(name.c_str(), node_flags))
	{
		treeNod = true;
	}
	if (ImGui::IsItemClicked())
	{
		//Set inspector window of this Game Object
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->LinkObject(this);
		((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->SetGameObjectSelected(this);
		App->camera->SetFocus(this);
		App->scene->drag = this;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 6));
	if (ImGui::BeginPopupContextItem("Create"))
	{
		ShowGameObjectOptions();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	ImGui::SameLine(); App->ShowHelpMarker("Right Click to open Options");
	if (treeNod)
	{
		ImGui::PopStyleColor();

		if (App->scene->drag != nullptr)
		{
			if (ImGui::IsItemHoveredRect())
			{
				if (App->scene->drag != this)
				{
					ImGui::BeginTooltip();
					ImGui::Text("Move %s to %s", App->scene->drag->GetName(), this->GetName());
					ImGui::EndTooltip();
				}
			}
		}
		if (use_search)
		{
			ImGui::TreePop();
			return;
		}
		for (uint i = 0; i < childs.size(); i++)
		{
			ImGui::PushID(i);
			childs[i]->ShowHierarchy();
			ImGui::PopID();
		}

		ImGui::TreePop();
	}
	else
	{
		ImGui::PopStyleColor();
	}
}

void GameObject::ShowGameObjectOptions()
{
	//Create child Game Objects / Components
	if (ImGui::MenuItem("Copy", NULL, false, false))
	{
		((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->SetGameObjectCopy(this);
	}
	if (ImGui::MenuItem("Paste", NULL, false, false))
	{
		((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->CopyGameObject(this);
	}
	ImGui::Separator();
	//selected
	bool modificate_parent = false;
	if (((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->GetSelected() != this)
	{
		modificate_parent = true;
	}
	if (ImGui::MenuItem("Selected go Child this", NULL, false, modificate_parent))
	{
		App->scene->ModificateParent(((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->GetSelected(), this);
	}
	if (ImGui::MenuItem("Create as Prefab", NULL, false))
	{
		App->json_seria->SavePrefab(*this, App->fs->GetMainDirectory().c_str(), "", false);
		((Project*)App->gui->win_manager[WindowName::PROJECT])->UpdateNow();
		App->fs->UpdateFilesAssets();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Rename", NULL, false, false))
	{
		// Not functional
	}
	if (ImGui::MenuItem("Duplicate", NULL, false, false))
	{
		// GetParent()->AddChildGameObject_Copy(this);
	}
	if (ImGui::MenuItem("Delete"))
	{
		if (App->engine_state == EngineState::STOP)
		{
			//((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->SetGameObjecttoDelete(this);
			Event e;
			e.Set_event_data(EventType::EVENT_DELETE_GO);
			e.delete_go.Todelte = this;
			e.delete_go.uuid = uid;
			PushEvent(e);
		}
		else
		{
			LOG("Deleting a GameObject while PlayMode may cause crashes... you can't delete now.");
		}
	}
	//if (ImGui::MenuItem("Set All Childs with tag 'No C#'"))
	//{
	//	SetAllChildsTag(this, "NoC#");
	//}
	//if (ImGui::MenuItem("Set All Childs with tag 'No C#' NO PARENT"))
	//{
	//	SetAllChildsTag(this, "NoC#", true);
	//}
	if (ImGui::MenuItem("Show Number of Childs (recursive)", NULL, false, App->gui->develop_mode))
	{
		uint count = 0;
		GetChildsRecursive(this, count);
		LOG("[error] Total: %i", count);
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Create Empty"))
	{
		GameObject* empty = App->scene->CreateGameObject(this);
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->LinkObject(empty);
	}
	if (ImGui::MenuItem("Create Cube"))
	{
		GameObject* cube = App->scene->CreateCube(this);
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->LinkObject(cube);
	}


	//if (ImGui::MenuItem("Sphere"))
	//{
	//	GameObject* sphere = App->scene->CreateSphere(this);
	//	((Inspector*)App->gui->winManager[WindowName::INSPECTOR])->LinkObject(sphere);
	//}

	ImGui::Separator();
	// Add Components -----------------------------------------------
	//ImGui::MenuItem("ADD COMPONENT", NULL, false, false);
	if (this->GetUUID() != 1)
	{
		if (ImGui::BeginMenu("Add Component"))
		{
			if (ImGui::MenuItem("Transform"))
			{
				AddComponent(Comp_Type::C_TRANSFORM);
			}
			if (ImGui::MenuItem("Mesh"))
			{
				AddComponent(Comp_Type::C_MESH);
			}
			if (ImGui::MenuItem("Material"))
			{
				AddComponent(Comp_Type::C_MATERIAL);
			}
			if (ImGui::MenuItem("Script"))
			{
				AddComponent(Comp_Type::C_SCRIPT);
			}
			if (ImGui::MenuItem("Animation"))
			{
				AddComponent(Comp_Type::C_ANIMATION);
			}
			if (ImGui::MenuItem("Audio"))
			{
				AddComponent(Comp_Type::C_AUDIO);
			}
			if (ImGui::MenuItem("Bone"))
			{
				AddComponent(Comp_Type::C_BONE);
			}
			if (ImGui::MenuItem("Finite State Machine"))
			{
				AddComponent(Comp_Type::C_FSM);
			}
			if (ImGui::MenuItem("Light"))
			{
				AddComponent(Comp_Type::C_LIGHT);
			}
			if (ImGui::MenuItem("CubeMap Renderer"))
			{
				AddComponent(Comp_Type::C_CUBEMAP_RENDERER);
			}
			if (ImGui::MenuItem("Collider"))
			{
				AddComponent(Comp_Type::C_COLLIDER);
			}
			if (ImGui::MenuItem("Rigid Body"))
			{
				AddComponent(Comp_Type::C_RIGIDBODY);
			}
			if (ImGui::MenuItem("Joint"))
			{
				AddComponent(Comp_Type::C_JOINT);
			}
			if (ImGui::MenuItem("Particle System"))
			{
				AddComponent(Comp_Type::C_PARTICLE_SYSTEM);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Canvas"))
			{
				GameObject* canvas = App->scene->CreateCanvas(this);
				App->gui->SetLinkInspector(canvas);

			}
			if (ImGui::MenuItem("Image"))
			{
				CompCanvas* comp_canvas = (CompCanvas*)FindParentComponentByType(Comp_Type::C_CANVAS);
				GameObject* parent = this;
				if (comp_canvas == nullptr)
				{
					parent = App->scene->CreateCanvas(this);
				}
				GameObject* image = App->scene->CreateImage(parent);

				App->gui->SetLinkInspector(image);

			}
			if (ImGui::MenuItem("Button"))
			{
				CompCanvas* comp_canvas = (CompCanvas*)FindParentComponentByType(Comp_Type::C_CANVAS);
				GameObject* parent = this;
				if (comp_canvas == nullptr)
				{
					parent = App->scene->CreateCanvas(this);
				}
				GameObject* button = App->scene->CreateButton(parent);
				App->gui->SetLinkInspector(button);

			}
			if (ImGui::MenuItem("Check Box"))
			{
				CompCanvas* comp_canvas = (CompCanvas*)FindParentComponentByType(Comp_Type::C_CANVAS);
				GameObject* parent = this;
				if (comp_canvas == nullptr)
				{
					parent = App->scene->CreateCanvas(this);
				}
				GameObject* check_box = App->scene->CreateCheckBox(parent);
				App->gui->SetLinkInspector(check_box);

			}
			if (ImGui::MenuItem("Slider"))
			{
				CompCanvas* comp_canvas = (CompCanvas*)FindParentComponentByType(Comp_Type::C_CANVAS);
				GameObject* parent = this;
				if (comp_canvas == nullptr)
				{
					parent = App->scene->CreateCanvas(this);
				}
				GameObject* slider = App->scene->CreateSlider(parent);
				App->gui->SetLinkInspector(slider);

			}
			if (ImGui::MenuItem("Text"))
			{
				CompCanvas* comp_canvas = (CompCanvas*)FindParentComponentByType(Comp_Type::C_CANVAS);
				GameObject* parent = this;
				if (comp_canvas == nullptr)
				{
					parent = App->scene->CreateCanvas(this);
				}
				GameObject* text = App->scene->CreateText(parent);
				App->gui->SetLinkInspector(text);

			}
			if (ImGui::MenuItem("Edit Text"))
			{
				CompCanvas* comp_canvas = (CompCanvas*)FindParentComponentByType(Comp_Type::C_CANVAS);
				GameObject* parent = this;
				if (comp_canvas == nullptr)
				{
					parent = App->scene->CreateCanvas(this);
				}
				GameObject* edit_text = App->scene->CreateEditText(parent);
				App->gui->SetLinkInspector(edit_text);

			}
			ImGui::EndMenu();
		}
	}
	// -------------------------------------------------------------
}

//void GameObject::SetAllChildsTag(GameObject* child, const char* tag, bool parent)
//{
//	if (child->GetNumChilds() > 0)
//	{
//		for (int i = 0; i < child->GetNumChilds(); i++)
//		{
//			child->SetAllChildsTag(child->GetChildbyIndex(i), tag);
//		}
//	}
//	if (child->GetNumComponents() > 1 && child->FindComponentByType(Comp_Type::C_COLLIDER) == nullptr &&
//		child->FindComponentByType(Comp_Type::C_SCRIPT) == nullptr && parent == false)
//	{
//		child->SetTag(tag);
//	}
//}

void GameObject::GetChildsRecursive(GameObject* child, uint& count)
{
	if (child->GetNumChilds() > 0)
	{
		for (int i = 0; i < child->GetNumChilds(); i++)
		{
			child->GetChildsRecursive(child->GetChildbyIndex(i), count);
		}
	}
	count++;
}

void GameObject::ShowInspectorInfo()
{
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.211f, 0.211f, 0.211f, 1.00f));
	if (ImGui::BeginChild(ImGui::GetID("Inspector"), ImVec2(ImGui::GetWindowWidth(), 150)))
	{
		static GLuint icon_GameObject = App->textures->LoadTexture("Images/UI/icon_GameObject.png");
		ImGui::Spacing();

		ImGui::Text(""); ImGui::SameLine(5);
		ImGui::Image((ImTextureID*)icon_GameObject, ImVec2(23, 23), ImVec2(-1, 1), ImVec2(0, 0)); ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));

		/* ENABLE-DISABLE CHECKBOX*/
		if(ImGui::Checkbox("##1", &active))
		{
			SetActive(active);
		}

		/* NAME OF THE GAMEOBJECT */
		ImGui::SameLine();
		ImGui::PopStyleVar();
		char namedit[150];
		strcpy_s(namedit, 150, name.c_str());
		if (ImGui::InputText("##nameModel", namedit, 150, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			name = App->fs->ConverttoChar(std::string(namedit).c_str());
		}
		//ImGui::InputText("##nameModel", name, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine(); App->ShowHelpMarker("Hold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n");
		ImGui::PopStyleVar();

		/*TAG*/
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 3));
		App->scene->TagWindow();
		ImGui::PopStyleVar();

		/* STATIC CHECKBOX */
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));

		static bool window_active = false;
		static bool static_object = false;
		ImGui::Text(""); ImGui::SameLine(8);
		if (ImGui::Checkbox("##2", &static_obj))
		{
			if (App->engine_state == EngineState::STOP)
			{
				window_active = true;
				static_object = static_obj;
				static_obj = !static_obj;
			}
			else
			{
				window_active = false;
				static_obj = !static_obj;
				LOG("You can't change 'Static' variable when Game Mode is ON.");
			}
		}

		if (window_active)
		{
			if (childs.size() > 0)
			{
				// Show window to change childs to static
				ShowFreezeChildsWindow(static_object, window_active);
			}
			else
			{
				FreezeTransforms(static_object, false);
				window_active = false;
			}
		}
		ImGui::SameLine();
		ImGui::PopStyleVar();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "Static");
		ImGui::PopStyleVar();
	}

	/* BOUNDING BOX CHECKBOX */
	if (box_fixed.IsFinite())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));

		ImGui::Text(""); ImGui::SameLine(8);
		ImGui::Checkbox("##3", &bb_active);

		ImGui::SameLine();
		ImGui::PopStyleVar();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "Bounding Box");
		ImGui::PopStyleVar();
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();

	// UPDATE EDITOR WINDOWS OF EACH COMPONENT
	for (uint i = 0; i < components.size(); i++)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
		bool open = false;
		char tmp[255];
		sprintf(tmp, "%i", components[i]->GetUUID());
		ImGui::PushID(tmp);
		if (ImGui::TreeNodeEx(components[i]->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			open = true;
		}
		ImGui::PopStyleColor();
		if (ImGui::BeginPopupContextItem("Open"))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 3));
			components[i]->ShowOptions();
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}
		if (open)
		{
			components[i]->ShowInspectorInfo();
			ImGui::Separator();
		}
		ImGui::PopID();
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("");
	ImGui::SameLine(ImGui::GetWindowWidth() / 2 - WIDTH_ADDCOMPONENT / 2);
	//ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2);
	static bool add_component = false;
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.208f, 0.208f, 0.208f, 1.00f));
	if (ImGui::ButtonEx("ADD COMPONENT", ImVec2(WIDTH_ADDCOMPONENT, 20)))
	{
		add_component = !add_component;
	}
	ImGui::PopStyleColor();
	ImVec2 pos_min = ImVec2(0, 0);
	ImVec2 pos_max = ImVec2(0, 0);
	if (add_component)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.258f, 0.258f, 0.258f, 1.00f));
		ImVec2 pos_win_act = ImGui::GetItemRectMin();
		if (pos_win_act.y + WIDTH_ADDCOMPONENT + 24 > ImGui::GetWindowHeight() + 100)
		{
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y - 204));
			pos_min = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y - 204);
			pos_max = ImVec2(pos_min.x + (ImGui::GetWindowWidth() / 2) + 42, pos_min.y + (WIDTH_ADDCOMPONENT)+20); // Magic number
		}
		else
		{
			pos_min = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
			pos_max = ImVec2(pos_min.x + (ImGui::GetWindowWidth() / 2) + 42, pos_min.y + (WIDTH_ADDCOMPONENT)+20); // Magic number
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y));
		}

		ImGui::SetNextWindowSize(ImVec2(WIDTH_ADDCOMPONENT, WIDTH_ADDCOMPONENT));
		ImGui::Begin("AddComponent", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.311f, 0.311f, 0.311f, 1.00f));
		if (ImGui::BeginChild(ImGui::GetID("AddComponent"), ImVec2(ImGui::GetWindowWidth(), 20), false, ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::Text("");
			ImGui::SameLine(62);
			ImGui::Text("Component");
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		if (ImGui::MenuItem("Transform"))
		{
			AddComponent(Comp_Type::C_TRANSFORM);
			add_component = false;
		}
		if (ImGui::MenuItem("Mesh"))
		{
			AddComponent(Comp_Type::C_MESH);
			add_component = false;
		}
		if (ImGui::MenuItem("Material"))
		{
			AddComponent(Comp_Type::C_MATERIAL);
			add_component = false;
		}
		if (ImGui::MenuItem("Camera"))
		{
			AddComponent(Comp_Type::C_CAMERA);
			add_component = false;
		}
		if (ImGui::MenuItem("Script"))
		{
			AddComponent(Comp_Type::C_SCRIPT);
			add_component = false;
		}
		if (ImGui::MenuItem("Animation"))
		{
			AddComponent(Comp_Type::C_ANIMATION);
			add_component = false;
		}
		if (ImGui::MenuItem("UI Animation"))
		{
			AddComponent(Comp_Type::C_ANIMATION_UI);
			add_component = false;
		}
		if (ImGui::MenuItem("Finite State Machine"))
		{
			AddComponent(Comp_Type::C_FSM);
			add_component = false;
		}
		if (ImGui::MenuItem("Light"))
		{
			AddComponent(Comp_Type::C_LIGHT);
			add_component = false;
		}
		if (ImGui::MenuItem("Cube Map Renderer"))
		{
			AddComponent(Comp_Type::C_CUBEMAP_RENDERER);
			add_component = false;
		}
		//if (ImGui::BeginMenu("UI"))
		//{

		//	ImGui::EndMenu();
		//}
		if (ImGui::MenuItem("Canvas"))
		{
			AddComponent(Comp_Type::C_CANVAS);
			add_component = false;
		}
		if (ImGui::MenuItem("Image"))
		{
			CompImage* image = (CompImage*)AddComponent(Comp_Type::C_IMAGE);
			image->SetTextureID(App->renderer3D->id_checkImage);
			add_component = false;
		}
		if (ImGui::MenuItem("Text"))
		{
			CompText* text = (CompText*)AddComponent(Comp_Type::C_TEXT);
			text->SetTextureID(App->renderer3D->id_checkImage);
			add_component = false;
		}
		if (ImGui::MenuItem("Audio"))
		{
			AddComponent(Comp_Type::C_AUDIO);
			add_component = false;
		}
		if (ImGui::MenuItem("Bone"))
		{
			AddComponent(Comp_Type::C_BONE);
			add_component = false;
		}
		if (ImGui::MenuItem("Collider"))
		{
			AddComponent(Comp_Type::C_COLLIDER);
			add_component = false;
		}
		if (ImGui::MenuItem("Rigid Body"))
		{
			AddComponent(Comp_Type::C_RIGIDBODY);
			add_component = false;
		}
		if (ImGui::MenuItem("Joint"))
		{
			AddComponent(Comp_Type::C_JOINT);
			add_component = false;
		}
		if (ImGui::MenuItem("Particle System"))
		{
			AddComponent(Comp_Type::C_PARTICLE_SYSTEM);
			add_component = false;
		}

		ImGui::End();
		ImGui::PopStyleColor();

	}
	if (ImGui::IsMouseHoveringRect(pos_min, pos_max) == false)
	{
		if (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(2))
			add_component = false;
	}
}

void GameObject::FreezeTransforms(bool freeze, bool change_childs)
{
	if (GetComponentTransform() != nullptr)
	{
		static_obj = freeze;
		GetComponentTransform()->Freeze(freeze);

		if (change_childs)
		{
			for (uint i = 0; i < childs.size(); i++)
			{
				childs[i]->static_obj = freeze;
				childs[i]->FreezeTransforms(freeze, change_childs);
			}
		}
	}
	if (static_obj)
	{
		App->scene->octree.Insert(this);
		App->scene->RemoveDynamicObject(this);
	}
	else
	{
		App->scene->octree.Remove(this);
		App->scene->dynamic_objects.push_back(this);
	}
}

void GameObject::ShowFreezeChildsWindow(bool freeze, bool& active)
{
	ImGui::OpenPopup("Change Static Flags");
	if (ImGui::BeginPopupModal("Change Static Flags", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (freeze)
		{
			ImGui::TextWrapped("Do you want to enable the static flags for all the child objects as well?");
		}
		else
		{
			ImGui::TextWrapped("Do you want to disable the static flags for all the child objects as well?");
		}
		ImGui::Spacing();
		if (ImGui::Button("Yes change children", ImVec2(140, 0)))
		{
			FreezeTransforms(freeze, true);
			active = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("No, this object only", ImVec2(130, 0)))
		{
			FreezeTransforms(freeze, false);
			active = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 0)))
		{
			static_obj = !freeze;
			active = false;
		}
	}
	ImGui::EndPopup();
}

void GameObject::SetActive(bool active)
{
	this->active = active;

	bool set_pactive = (parent_active && active) ? true : false;
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->SetParentActive(set_pactive);
	}
}

void GameObject::SetParentActive(bool active)
{
	parent_active = active;

	if (this->active)
	{
		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i]->SetParentActive(active);
		}
	}
}

void GameObject::SetVisible(bool visible)
{
	this->visible = visible;
}

void GameObject::SetStatic(bool set_static)
{
	static_obj = set_static;
}

bool GameObject::IsActive() const
{
	return active;
}

bool GameObject::IsParentActive() const
{
	return parent_active;
}

bool GameObject::IsVisible() const
{
	return visible;
}

bool GameObject::IsStatic() const
{
	return static_obj;
}

Component* GameObject::FindComponentByType(Comp_Type type) const
{
	Component* comp = nullptr;

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i] != nullptr && components[i]->GetType() == type) // We need to check if the component is ACTIVE first?¿
		{

			return components[i];
		}
	}
	return nullptr;
}

void GameObject::GetComponentsByType(Comp_Type type, std::vector<Component*>* fill_comp, bool iterate_hierarchy) const
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
		{
			fill_comp->push_back(components[i]);
		}
	}

	if (iterate_hierarchy)
	{
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			childs[k]->GetComponentsByType(type, fill_comp, iterate_hierarchy);
		}
	}
}

Component* GameObject::GetComponentsByUID(int uid, bool iterate_hierarchy)
{
	if (iterate_hierarchy)
	{
		std::queue<GameObject*> queue;
		queue.push(this);
		while (!queue.empty())
		{
			for (uint i = 0; i < queue.front()->components.size(); i++)
			{
				if (queue.front()->components[i]->GetUUID() == uid)
				{
					Component* ret = queue.front()->components[i];

					while (!queue.empty())
						queue.pop();

					return ret;
				}
			}
			for (uint k = 0; k < queue.front()->childs.size(); k++)
			{
				queue.push(queue.front()->childs[k]);
			}
			queue.pop();
		}

	}
	else
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetUUID() == uid)
			{
				return components[i];
			}
		}
	}
	return nullptr;
}
void GameObject::GetComponentsByRangeOfType(Comp_Type start, Comp_Type end, std::vector<Component*>* fill_comp, bool iterate_hierarchy)
{
	if (iterate_hierarchy)
	{
		std::queue<GameObject*> queue;
		queue.push(this);
		while (!queue.empty())
		{
			for (uint i = 0; i < queue.front()->components.size(); i++)
			{
				if (queue.front()->components[i]->GetType() >= start && queue.front()->components[i]->GetType() <= end)
				{
					fill_comp->push_back(queue.front()->components[i]);
				}
			}
			for (uint k = 0; k < queue.front()->childs.size(); k++)
			{
				queue.push(queue.front()->childs[k]);
			}
			queue.pop();
		}

	}
	else
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() >= start && components[i]->GetType() <= end)
			{
				fill_comp->push_back(components[i]);
			}
		}
	}
}

Component* GameObject::GetComponentByName(const char* name_component) const
{
	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() != Comp_Type::C_SCRIPT)
		{
			if (strcmp(components[i]->GetName(), name_component) == 0)
			{
				return components[i];
			}
		}
		else
		{
			if (strcmp(((CompScript*)components[i])->GetScriptName(), name_component) == 0)
			{
				return components[i];
			}
		}
	}
	return nullptr;
}

Component* GameObject::FindParentComponentByType(Comp_Type type)const
{

	Component * ret = nullptr;
	ret = FindComponentByType(type);
	if (ret == nullptr)
	{
		GameObject* item = parent;
		while (item != nullptr)
		{
			ret = item->FindComponentByType(type);
			if (ret != nullptr)
				break;
			item = item->GetParent();
		}
	}
	return ret;
}

Component* GameObject::FindComponentByUUID(uint uid) const
{
	Component* comp = nullptr;

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetUUID() == uid) // We need to check if the component is ACTIVE first?¿
		{
			return components[i];
		}
	}

	return nullptr;
}

Component* GameObject::AddComponent(Comp_Type type, bool isFromLoader)
{
	bool dupe = false;
	for (uint i = 0; i < components.size(); i++)
	{
		//We need to check if there is already a component of that type (no duplication)
		if (components[i]->GetType() == type && type != Comp_Type::C_SCRIPT)
		{
			dupe = true;
			LOG("There's already one component of this type in '%s'.", name.c_str());
			break;
		}
	}

	if (!dupe)
	{
		switch (type)
		{
		case Comp_Type::C_MESH:
		{
			CompMesh* mesh = new CompMesh(type, this);
			components.push_back(mesh);
			/* Link Material to the Mesh if exists */
			const CompMaterial* material_link = (CompMaterial*)FindComponentByType(Comp_Type::C_MATERIAL);
			if (material_link != nullptr) mesh->LinkMaterial(material_link);
			return mesh;
		}
		case Comp_Type::C_TRANSFORM:
		{
			CompTransform* transform = new CompTransform(type, this);
			components.push_back(transform);
			return transform;
		}
		case Comp_Type::C_RECT_TRANSFORM:
		{
			if (FindComponentByType(Comp_Type::C_RECT_TRANSFORM) != nullptr) return nullptr;
			CompRectTransform* transform = new CompRectTransform(type, this);
			if (components.size() > 0 && components[0]->GetType() == Comp_Type::C_TRANSFORM)
			{
				RELEASE(components[0]);
				components[0] = transform;
			}
			else components.push_back(transform);
			return transform;
		}
		case Comp_Type::C_MATERIAL:
		{
			CompMaterial* material = new CompMaterial(type, this);
			components.push_back(material);
			/* Link Material to the Mesh if exists */
			CompMesh* mesh_to_link = (CompMesh*)FindComponentByType(Comp_Type::C_MESH);
			if (mesh_to_link != nullptr) mesh_to_link->LinkMaterial(material);
			else LOG("MATERIAL not linked to any mesh");
			return material;
		}
		case Comp_Type::C_CANVAS:
		{
			AddComponent(Comp_Type::C_RECT_TRANSFORM);
			CompCanvas* canvas = new CompCanvas(type, this);
			components.push_back(canvas);
			//If is not RectTransform
			//TODO change transform
			return canvas;
		}
		case Comp_Type::C_CANVAS_RENDER:
		{
			CompCanvasRender* canvas_renderer = new CompCanvasRender(type, this);
			components.push_back(canvas_renderer);
			return canvas_renderer;
		}
		case Comp_Type::C_IMAGE:
		{
			//If is not RectTransform
			//TODO change transform
			CompCanvasRender* canvas_renderer = (CompCanvasRender*)FindComponentByType(Comp_Type::C_CANVAS_RENDER);
			if (canvas_renderer == nullptr)
			{
				canvas_renderer = new CompCanvasRender(Comp_Type::C_CANVAS_RENDER, this);
				components.push_back(canvas_renderer);
			}
			CompImage* image = new CompImage(type, this);

			components.push_back(image);
			return image;
		}
		case Comp_Type::C_TEXT:
		{
			//If is not RectTransform
			//TODO change transform
			CompCanvasRender* canvas_renderer = (CompCanvasRender*)FindComponentByType(Comp_Type::C_CANVAS_RENDER);
			if (canvas_renderer == nullptr)
			{
				canvas_renderer = new CompCanvasRender(Comp_Type::C_CANVAS_RENDER, this);
				components.push_back(canvas_renderer);
			}
			CompText* text = new CompText(type, this);

			components.push_back(text);
			return text;
		}
		case Comp_Type::C_EDIT_TEXT:
		{
			//If is not RectTransform
			//TODO change transform
			CompEditText* edit_text = new CompEditText(type, this);
			components.push_back(edit_text);
			/* Link image to the button if exists */
			CompText* text_to_link = (CompText*)FindComponentByType(Comp_Type::C_TEXT);
			if (text_to_link != nullptr)
			{

			}
			else LOG("TEXT not linked to any Edit Text");
			return edit_text;
		}
		case Comp_Type::C_BUTTON:
		{
			//If is not RectTransform
			//TODO change transform
			CompButton* button = new CompButton(type, this);
			components.push_back(button);
			/* Link image to the button if exists */
			CompImage* image_to_link = (CompImage*)FindComponentByType(Comp_Type::C_IMAGE);
			if (image_to_link != nullptr)
			{

			}
			else LOG("IMAGE not linked to any Button");
			return button;
		}
		case Comp_Type::C_CHECK_BOX:
		{
			//If is not RectTransform
			//TODO change transform
			CompCheckBox* check_box = new CompCheckBox(type, this);
			components.push_back(check_box);
			/* Link image to the button if exists */
			CompImage* image_to_link = (CompImage*)FindComponentByType(Comp_Type::C_IMAGE);
			if (image_to_link != nullptr)
			{

			}
			else LOG("IMAGE not linked to any CheckBox");
			return check_box;
		}
		case Comp_Type::C_SLIDER:
		{
			//If is not RectTransform
			//TODO change transform
			CompSlider* slider = new CompSlider(type, this);
			components.push_back(slider);
			CompImage* image_to_link = (CompImage*)FindComponentByType(Comp_Type::C_IMAGE);
			if (image_to_link != nullptr)
			{

			}
			else LOG("IMAGE not linked to any slider");
			return slider;
		}
		case Comp_Type::C_CAMERA:
		{
			CompCamera* camera = new CompCamera(type, this);
			components.push_back(camera);
			return camera;
		}
		case Comp_Type::C_SCRIPT:
		{
			CompScript* script = new CompScript(type, this);
			if (!isFromLoader) script->Init();
			components.push_back(script);
			return script;
		}
		case Comp_Type::C_ANIMATION:
		{
			CompAnimation* anim = new CompAnimation(type, this);
			components.push_back(anim);
			return anim;
		}
		case Comp_Type::C_ANIMATION_UI:
		{
			LOG("Adding ANIMATION COMPONENT.");
			CompUIAnimation* anim = new CompUIAnimation(type, this);
			components.push_back(anim);
			return anim;
		}
		case Comp_Type::C_AUDIO:
		{
			CompAudio* audio = new CompAudio(type, this);
			components.push_back(audio);
			return audio;
		}
		case Comp_Type::C_BONE:
		{
			CompBone* bone = new CompBone(type, this);
			components.push_back(bone);
			return bone;
		}
		case Comp_Type::C_FSM:
		{
			CompFiniteStateMachine* fsm = new CompFiniteStateMachine(type, this);
			components.push_back(fsm);
			return fsm;
		}
		case Comp_Type::C_LIGHT:
		{
			CompLight* light = new CompLight(type, this);
			components.push_back(light);
			return light;
		}

		case Comp_Type::C_CUBEMAP_RENDERER:
		{
			LOG("Adding CUBEMAP RENDERER.");
			CompCubeMapRenderer* cubemap = new CompCubeMapRenderer(type, this);
			components.push_back(cubemap);
			return cubemap;
		}
		case Comp_Type::C_COLLIDER:
		{
			CompCollider* collider = new CompCollider(type, this);
			components.push_back(collider);
			return collider;
		}
		case Comp_Type::C_RIGIDBODY:
		{
			CompRigidBody* rigid_body = new CompRigidBody(type, this);
			components.push_back(rigid_body);
			return rigid_body;
		}
		case Comp_Type::C_PARTICLE_SYSTEM:
		{
			CompParticleSystem* particlesystem = new CompParticleSystem(type, this);
			components.push_back(particlesystem);
			return particlesystem;
		}
		case Comp_Type::C_JOINT:
		{
			CompJoint* joint = new CompJoint(type, this);
			components.push_back(joint);
			return joint;
		}
		}
	}

	return nullptr;
}

void GameObject::AddComponentCopy(const Component& copy)
{
	switch (copy.GetType())
	{
	case Comp_Type::C_TRANSFORM:
	{
		CompTransform* transform = new CompTransform((CompTransform&)copy, this); //Transform copy constructor
		components.push_back(transform);
		break;
	}
	case Comp_Type::C_MESH:
	{
		CompMesh* mesh = new CompMesh((CompMesh&)copy, this); //Mesh copy constructor
		components.push_back(mesh);
		/* Link Material to the Mesh (if exists) */
		CompMaterial* material_link = (CompMaterial*)FindComponentByType(Comp_Type::C_MATERIAL);
		if (material_link != nullptr)
		{
			mesh->LinkMaterial(material_link);
		}
		else
		{
			LOG("Havent Material");
		}
		break;
	}
	case Comp_Type::C_MATERIAL:
	{
		CompMaterial* material = new CompMaterial((CompMaterial&)copy, this); //Material copy constructor
		components.push_back(material);
		/* Link Mesh to the Material (if exists) */
		CompMesh* mesh_to_link = (CompMesh*)FindComponentByType(Comp_Type::C_MESH);
		if (mesh_to_link != nullptr)
		{
			mesh_to_link->LinkMaterial(material);
		}
		else
		{
			//LOG("MATERIAL not linked to any mesh");
		}
		break;
	}
	case Comp_Type::C_CAMERA:
	{
		CompCamera* camera = new CompCamera((CompCamera&)copy, this); //Camera copy constructor
		components.push_back(camera);
		break;
	}
	case Comp_Type::C_SCRIPT:
	{
		CompScript* script = new CompScript((CompScript&)copy, this); //Script copy constructor
		components.push_back(script);
		break;
	}
	case Comp_Type::C_ANIMATION:
	{
		CompAnimation* anim = new CompAnimation((CompAnimation&)copy, this); //Anim copy constructor
		components.push_back(anim);
		break;
	}
	case Comp_Type::C_ANIMATION_UI:
	{
		CompUIAnimation* anim = new CompUIAnimation((CompUIAnimation&)copy, this); //Anim copy constructor
		components.push_back(anim);
		break;
	}
	case Comp_Type::C_AUDIO:
	{
		CompAudio* audio = new CompAudio((CompAudio&)copy, this); //Audio copy constructor
		components.push_back(audio);
		break;
	}
	case Comp_Type::C_BONE:
	{
		CompBone* bone = new CompBone((CompBone&)copy, this); //Bone copy constructor
		components.push_back(bone);
		break;
	}
	case Comp_Type::C_FSM:
	{
		CompFiniteStateMachine* fsm = new CompFiniteStateMachine((CompFiniteStateMachine&)copy, this); //FSM copy constructor
		components.push_back(fsm);
		break;
	}
	case Comp_Type::C_LIGHT:
	{
		CompLight* light = new CompLight((CompLight&)copy, this); //Light copy constructor
		components.push_back(light);
		break;
	}
	case Comp_Type::C_COLLIDER:
	{
		CompCollider* collider = new CompCollider((CompCollider&)copy, this); //Collider copy constructor
		components.push_back(collider);
		break;
	}
	case Comp_Type::C_RIGIDBODY:
	{
		CompRigidBody* rigid_body = new CompRigidBody((CompRigidBody&)copy, this); //Rigid Body contructor
		components.push_back(rigid_body);
		break;
	}
	case Comp_Type::C_PARTICLE_SYSTEM:
	{
		CompParticleSystem* particle_system = new CompParticleSystem((CompParticleSystem&)copy, this); //Particle System constructor
		components.push_back(particle_system);
		break;
	}
	case Comp_Type::C_JOINT:
	{
		CompJoint* joint = new CompJoint((CompJoint&)copy, this); //Rigid Body contructor
		components.push_back(joint);
		break;
	}
	default:
		break;
	}
}

void GameObject::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int);			//identifier
	buffer_size += sizeof(GetUUID());
	int uuidParent = -1;
	if (this->GetParent() != nullptr)
	{
		uuidParent = this->GetParent()->GetUUID();
	}
	buffer_size += sizeof(uuidParent);
	//Save name size
	buffer_size += sizeof(int);			
	buffer_size += name.size();
	//Save Tag size
	buffer_size += sizeof(int);
	buffer_size += tag.size();
	buffer_size += sizeof(bb_active);
	buffer_size += sizeof(static_obj);
	buffer_size += sizeof(animation_translations);
	buffer_size += sizeof(animation_rotations);
	buffer_size += sizeof(animation_scales);

	// Save Components size
	//buffer_size += sizeof(components.size());
	//for (uint i = 0; i < components.size(); i++)
	//{
	//	components[i]->GetOwnBufferSize(buffer_size);
	//}

	//for (uint i = 0; i < childs.size(); i++)
	//{
	//	childs[i]->GetOwnBufferSize(buffer_size);
	//}
}

void GameObject::SaveComponents(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	for (uint i = 0; i < components.size(); i++)
	{
		std::string temp = name + "Component " + std::to_string(i) + ".";
		components[i]->Save(object, temp, saveScene, countResources);
	}
}

void GameObject::LoadComponents(const JSON_Object* object, std::string name, uint numComponents)
{
	// First Add All components by type
	for (uint i = 0; i < numComponents; i++)
	{
		std::string temp = name + "Component " + std::to_string(i) + ".";
		Comp_Type type = (Comp_Type)(int)json_object_dotget_number_with_std(object, temp + "Type");
		switch (type)
		{
		case Comp_Type::C_UNKNOWN:
			break;
		case Comp_Type::C_TRANSFORM:
			this->AddComponent(Comp_Type::C_TRANSFORM);
			break;
		case Comp_Type::C_RECT_TRANSFORM:
			this->AddComponent(Comp_Type::C_RECT_TRANSFORM);
			break;
		case Comp_Type::C_MESH:
			this->AddComponent(Comp_Type::C_MESH);
			break;
		case Comp_Type::C_MATERIAL:
			this->AddComponent(Comp_Type::C_MATERIAL);
			break;
		case Comp_Type::C_CAMERA:
			this->AddComponent(Comp_Type::C_CAMERA);
			break;
		case Comp_Type::C_BUTTON:
			this->AddComponent(Comp_Type::C_BUTTON);
			break;
		case Comp_Type::C_CHECK_BOX:
			this->AddComponent(Comp_Type::C_CHECK_BOX);
			break;
		case Comp_Type::C_CANVAS:
			this->AddComponent(Comp_Type::C_CANVAS);
			break;
		case Comp_Type::C_CANVAS_RENDER:
			this->AddComponent(Comp_Type::C_CANVAS_RENDER);
			break;
		case Comp_Type::C_TEXT:
			this->AddComponent(Comp_Type::C_TEXT);
			break;
		case Comp_Type::C_EDIT_TEXT:
			this->AddComponent(Comp_Type::C_EDIT_TEXT);
			break;
		case Comp_Type::C_IMAGE:
			this->AddComponent(Comp_Type::C_IMAGE);
			break;
		case Comp_Type::C_SCRIPT:
			this->AddComponent(Comp_Type::C_SCRIPT, true);
			break;
		case Comp_Type::C_ANIMATION:
			this->AddComponent(Comp_Type::C_ANIMATION);
			break;
		case Comp_Type::C_ANIMATION_UI:
			this->AddComponent(Comp_Type::C_ANIMATION_UI);
			break;
		case Comp_Type::C_AUDIO:
			this->AddComponent(Comp_Type::C_AUDIO);
			break;
		case Comp_Type::C_BONE:
			this->AddComponent(Comp_Type::C_BONE);
			break;
		case Comp_Type::C_FSM:
			this->AddComponent(Comp_Type::C_FSM);
			break;
		case Comp_Type::C_LIGHT:
			this->AddComponent(Comp_Type::C_LIGHT);
			break;
		case Comp_Type::C_CUBEMAP_RENDERER:
			this->AddComponent(Comp_Type::C_CUBEMAP_RENDERER);
			break;
		case Comp_Type::C_COLLIDER:
			this->AddComponent(Comp_Type::C_COLLIDER);
			break;
		case Comp_Type::C_RIGIDBODY:
			this->AddComponent(Comp_Type::C_RIGIDBODY);
			break;
		case Comp_Type::C_PARTICLE_SYSTEM:
			this->AddComponent(Comp_Type::C_PARTICLE_SYSTEM);
			break;
		case Comp_Type::C_JOINT:
			this->AddComponent(Comp_Type::C_JOINT);
			break;
		default:
			break;
		}
	}

	// Now Iterate All components and Load variables
	for (int i = 0; i < components.size(); i++)
	{
		std::string temp = name + "Component " + std::to_string(i) + ".";
		components[i]->Load(object, temp);

		/*if (components[i]->GetType() == Comp_Type::C_MESH && ((CompMesh*)components[i])->HasSkeleton())
		((CompMesh*)components[i])->GenSkeleton();*/
	}
}
void GameObject::SyncComponents(GameObject* parent)
{

	// Now Iterate All components and Load variables
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->SyncComponent(parent);
	}
}

void GameObject::SyncComponentsRecursive(GameObject * sync_parent)
{
	// Now Iterate All components and Load variables
	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->SyncComponent(sync_parent);
	}

	// Now Iterate All Childs to Sync Components
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->SyncComponentsRecursive(sync_parent);
	}
}

int GameObject::GetNumComponents() const
{
	return components.size();
}

CompTransform* GameObject::GetComponentTransform() const
{
	CompTransform* ret = (CompTransform*)FindComponentByType(Comp_Type::C_TRANSFORM);
	if (ret != nullptr)
	{
		return ret;
	}
	else
	{
		return (CompTransform*)FindComponentByType(Comp_Type::C_RECT_TRANSFORM);

	}

}
CompRectTransform* GameObject::GetComponentRectTransform() const
{
	return (CompRectTransform*)FindComponentByType(Comp_Type::C_RECT_TRANSFORM);
}
CompMesh* GameObject::GetComponentMesh() const
{
	return (CompMesh*)FindComponentByType(Comp_Type::C_MESH);
}

CompMaterial* GameObject::GetComponentMaterial() const
{
	return (CompMaterial*)FindComponentByType(Comp_Type::C_MATERIAL);
}

CompScript* GameObject::GetComponentScript() const
{
	return (CompScript*)FindComponentByType(Comp_Type::C_SCRIPT);
}

CompAnimation * GameObject::GetComponentAnimation() const
{
	return (CompAnimation*)FindComponentByType(Comp_Type::C_ANIMATION);
}

CompLight * GameObject::GetComponentLight() const
{
	return (CompLight*)FindComponentByType(Comp_Type::C_LIGHT);
}

CompBone * GameObject::GetComponentBone() const
{
	return (CompBone*)FindComponentByType(Comp_Type::C_BONE);
}

CompImage * GameObject::GetComponentImage() const
{
	return (CompImage*)FindComponentByType(Comp_Type::C_IMAGE);
}

Component* GameObject::GetComponentbyIndex(uint i) const
{
	if (i < components.size() && i >= 0)
	{
		return components[i];
	}
	return nullptr;
}

void GameObject::DeleteAllComponents()
{
	Component* comp = nullptr;
	for (uint i = 0; i < components.size(); i++)
	{
		comp = components[i];
		if (!App->mode_game)
		{
			if (((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied() == comp)
			{
				((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetLinkComponentNull();
			}
		}

		if (comp->InScripting())
		{
			App->importer->iScript->RemoveComponentFromMonoList(comp);
			if (comp->GetType() == C_TRANSFORM)
			{
				App->importer->iScript->RemoveTransformPosPointerFromMap(((CompTransform*)comp)->GetPosPointer());
			}
		}

		comp->Clear();

		if (comp->GetType() == C_LIGHT)
		{
			Event delete_light_event;
			delete_light_event.Set_event_data(EventType::EVENT_DELETE_LIGHT);
			delete_light_event.delete_light.light = (CompLight*)comp;
			delete_light_event.delete_light.light->to_delete = true;
			PushEvent(delete_light_event);
		}
		else RELEASE(comp);
		components[i] = nullptr;
	}
	components.clear();
}

void GameObject::DeleteComponent(uint index)
{
	if (index < components.size() > 0)
	{
		Component* comp = components[index];
		if (((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied() == comp)
		{
			((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetLinkComponentNull();
		}

		comp->Clear();
		if (comp->GetType() == C_LIGHT)
		{
			Event delete_light_event;
			delete_light_event.Set_event_data(EventType::EVENT_DELETE_LIGHT);
			delete_light_event.delete_light.light = (CompLight*)comp;
			delete_light_event.delete_light.light->to_delete = true;
			PushEvent(delete_light_event);
		}
		else RELEASE(comp);
		comp = nullptr;

		components.erase(components.begin() + index);
	}
}

uint GameObject::GetNumChilds() const
{
	return childs.size();
}

GameObject* GameObject::GetChildbyIndex(uint pos_inVec) const
{
	return childs[pos_inVec];
}

GameObject* GameObject::GetChildbyName(const char* name) const
{
	if (childs.size() > 0)
	{
		for (uint i = 0; i < childs.size(); i++)
		{
			if (strcmp(childs[i]->GetName(), name) == 0)
			{
				return childs[i];
			}
		}
	}
	return nullptr;
}

GameObject* GameObject::GetChildByTagIndex(const char* tag, int index) const
{
	if (childs.size() > 0)
	{
		int index_count = 0;
		for (uint i = 0; i < childs.size(); i++)
		{
			if (strcmp(childs[i]->GetTag(), tag) == 0)
			{
				if (index_count == index)
				{
					return childs[i];
				}
				else
					index_count++;
			}
		}
	}
	return nullptr;
}

GameObject* GameObject::GetChildByRealName(std::string name) const
{
	if (childs.size() > 0)
	{
		for (uint i = 0; i < childs.size(); i++)
		{
			std::string child_name = childs[i]->GetName();
			std::size_t found = child_name.find(name);
			if (found != std::string::npos)
			{
				return childs[i];
			}
		}
	}
	return nullptr;
}

void GameObject::GetChildDeepSearch(const char * name, std::vector<GameObject*>& GOVector) const
{
	GameObject* found = GetChildbyName(name);

	if (found != nullptr)
	{
		GOVector.push_back(found);
	}
	if (childs.size() > 0)
	{
		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i]->GetChildDeepSearch(name, GOVector);
		}
	}
}


int GameObject::GetIndexChildbyName(const char * name) const
{
	if (childs.size() > 0)
	{
		for (uint i = 0; i < childs.size(); i++)
		{
			if (strcmp(childs[i]->GetName(), name) == 0)
			{
				return i;
			}
		}
	}
	return -1;
}

int GameObject::GetIndexChildbyGO(const GameObject * child) const
{
	if (childs.size() > 0)
	{
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i] == child)
			{
				return i;
			}
		}
	}
	return -1;
}

void GameObject::RemoveChildbyIndex(uint index)
{
	if (childs.size() > 0)
	{
		std::vector<GameObject*>::iterator item = childs.begin();
		for (uint i = 0; i < childs.size(); i++)
		{
			if (i == index)
			{
				GameObject* it = childs[i];
				childs[i]->SetParent(nullptr);
				childs.erase(item);
				RELEASE(it);
				it = nullptr;
				break;
			}
			item++;
		}
	}
}

std::vector<GameObject*> GameObject::GetChildsVec() const
{
	return childs;
}

std::vector<GameObject*>* GameObject::GetChildsPtr()
{
	return &childs;
}

void GameObject::AddChildGameObject_Copy(const GameObject* child)
{
	GameObject* temp = new GameObject(*child);
	temp->uid = App->random->Int();
	std::string temp_name = temp->name;
	temp_name += " (1)";
	temp->name = temp_name.c_str();
	temp_name.clear();
	temp->parent = this;
	childs.push_back(temp);
}

void GameObject::AddChildGameObject(GameObject* child)
{
	if (this->GetUUID() == 1)
	{
		child->parent = nullptr;
	}
	else
	{
		child->parent = this;
	}
	childs.push_back(child);
}

void GameObject::UpdateChildsMatrices()
{
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->GetComponentTransform()->SetToUpdate();
	}
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

bool GameObject::HaveParent() const
{
	return (parent != nullptr);
}

void GameObject::SetParent(GameObject* new_parent)
{
	this->parent = new_parent;
}

void GameObject::AddBoundingBox(const ResourceMesh* mesh)
{

	box_fixed.Enclose(mesh->aabb_box);

}

void GameObject::DrawBoundingBox()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(box_fixed.Edge(i).a.x, box_fixed.Edge(i).a.y, box_fixed.Edge(i).a.z);
		glVertex3f(box_fixed.Edge(i).b.x, box_fixed.Edge(i).b.y, box_fixed.Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void GameObject::SetAABBActive(bool active)
{
	bb_active = active;
}

bool GameObject::IsAABBActive() const
{
	return bb_active;
}

uint GameObject::GetUUID() const
{
	return uid;
}

void GameObject::SetUUID(uint uuid)
{
	uid = uuid;
}

void GameObject::SetUUIDRandom()
{
	uid = App->random->Int();
}

bool GameObject::WanttoDelete() const
{
	return to_delete;
}

void GameObject::SettoDelete()
{
	to_delete = true;
}

void GameObject::SetChildToNull(uint index)
{
	if (index < childs.size())
	{
		childs[index] = nullptr;
	}
}

//ANIMATION PURPOSES---------------------------

bool GameObject::AreTranslationsActivateds() const
{
	return animation_translations;
}

bool GameObject::AreRotationsActivateds() const
{
	return animation_rotations;
}

bool GameObject::AreScalesActivateds() const
{
	return animation_scales;
}
void GameObject::ToggleAnimationTranslations(bool activate)
{
	animation_translations = activate;
}
void GameObject::ToggleAnimationRotation(bool activate)
{
	animation_rotations = activate;
}
void GameObject::ToggleAnimationScale(bool activate)
{
	animation_scales = activate;
}
//--------------------------------------------