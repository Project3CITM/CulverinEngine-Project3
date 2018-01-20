#include "GameObject.h"
#include "Application.h"
#include "WindowInspector.h"
#include "WindowHierarchy.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleFS.h"
#include "Scene.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "Component.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "CompMaterial.h"
#include "CompCamera.h"
#include "CompScript.h"

GameObject::GameObject(GameObject* parent) :parent(parent)
{
	Enable();
	SetVisible(true);
	uid = App->random->Int();

	if (parent != nullptr)
	{
		// Push this game object into the childs list of its parent
		parent->childs.push_back(this);
	}
}

GameObject::GameObject(char* nameGameObject, uint uuid)
{
	Enable();
	SetVisible(true);
	uid = uuid;
	name = nameGameObject;
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
	name = App->GetCharfromConstChar(nametemp.c_str());

	// Same data as the 'copy' gameobject
	active = copy.isActive();
	visible = copy.isVisible();
	static_obj = copy.isStatic();
	bb_active = copy.isAABBActive();
	if (copy.bounding_box != nullptr)
	{
		bounding_box = new AABB(*copy.bounding_box);
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
	RELEASE_ARRAY(name);
	delete bounding_box;
	bounding_box = nullptr;
	parent = nullptr;

	if (components.size() > 0)
	{
		components.clear();
	}

	if(childs.size() > 0)
	{
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
			if (components[i]->isActive())
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
			if (childs[i]->isActive())
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
		//Start Active scripts --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->isActive())
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
			if (childs[i]->isActive())
			{
				childs[i]->StartScripts();
			}
		}
	}
}

void GameObject::ClearAllVariablesScript()
{
	if (active)
	{
		//Start Active scripts --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->isActive())
			{
				if (components[i]->GetType() == Comp_Type::C_SCRIPT)
				{
					((CompScript*)components[i])->ClearVariables();
				}
			}
		}

		//Check child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->isActive())
			{
				childs[i]->ClearAllVariablesScript();
			}
		}
	}
}

GameObject* GameObject::GetGameObjectbyuid(uint uid)
{
	if (active)
	{
		if (GetUUID() == uid)
		{
			return this;
		}
		//Check child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			childs[i]->GetGameObjectbyuid(uid);
		}
	}
	return nullptr;
}

GameObject* GameObject::GetGameObjectfromScene(int id)
{
	if (active)
	{
		ImGui::Selectable(GetName());
		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
		{
			//LOG("%.2f - %.2f  / /  %.2f - %.2f", ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				return this;
			}
		}
		//Check child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			GameObject* temp = childs[i]->GetGameObjectfromScene(id);
			if (temp != nullptr)
			{
				return temp;
			}
		}
	}
	return nullptr;
}

void GameObject::Init()
{
}

void GameObject::preUpdate(float dt)
{
	fixedDelete = false;

	if (active)
	{
		//preUpdate Components --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->WantDelete())
			{
				DeleteComponent(components[i]);
			}
			else
			{
				if (components[i]->isActive())
				{
					components[i]->preUpdate(dt);
				}
			}
		}

		//preUpdate child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->toDelete)
			{
				App->scene->DeleteGameObject(childs[i]);
			}
			else
			{
				if (childs[i]->isActive())
				{
					childs[i]->preUpdate(dt);
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
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->isActive())
			{
				components[i]->Update(dt);
			}
		}

		//Update child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->isActive())
			{
				childs[i]->Update(dt);
			}
		}

		// BOUNDING BOX -----------------
		if (bounding_box != nullptr)
		{
			//CompMesh* mesh = (CompMesh*)(FindComponentByType(C_MESH));
			CompTransform* transform = (CompTransform*)(FindComponentByType(C_TRANSFORM));
			if (transform != nullptr)
			{
				//Resize the Bounding Box
				//bounding_box->SetFromCenterAndSize(transform->GetPos(), transform->GetScale()*2);
				box_fixed = *bounding_box;
				box_fixed.TransformAsAABB(transform->GetGlobalTransform());
			}
		}
	}
}

void GameObject::postUpdate()
{
}

bool GameObject::CleanUp()
{
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
	fixedDelete = check_delete;
}

bool GameObject::isDeleteFixed() const
{
	return fixedDelete;
}

void GameObject::Draw()
{
	if (visible)
	{
		//Draw Components --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->isActive())
			{
				components[i]->Draw();
			}
		}

		//Draw child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->isActive())
			{
				childs[i]->Draw();
			}
		}

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
		active = true;
	}

	return active;
}

bool GameObject::Disable()
{
	if (active)
	{
		active = false;
	}
	return active;
}

void GameObject::SetName(char * name)
{
	this->name = name;
}

const char* GameObject::GetName() const
{
	return name;
}

void GameObject::NameNotRepeat(std::string& name, bool haveParent, GameObject* parent_)
{
	bool stop = false;
	int i = 0;
	std::string nameRepeat = name;
	while (stop == false)
	{
		if (haveParent)
		{
			if (i < parent_->GetNumChilds())
			{
				bool stop_reserch = false;
				int ds = 0;
				bool haveRepeat = false;
				while (stop_reserch == false)
				{
					if (ds < parent_->GetNumChilds())
					{
						std::string nameChild = parent_->GetChildbyIndex(ds)->GetName();
						if (nameRepeat == nameChild)
						{
							haveRepeat = true;
						}
					}
					else
					{
						stop_reserch = true;
					}
					ds++;
				}
				if (haveRepeat == false)
				{
					stop_reserch = true;
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
		}
		else
		{
			if (i < App->scene->gameobjects.size())
			{
				bool stop_reserch = false;
				int ds = 0;
				bool haveRepeat = false;
				while (stop_reserch == false)
				{
					if (ds < App->scene->gameobjects.size())
					{
						std::string nameChild = App->scene->gameobjects[ds]->GetName();
						if (nameRepeat == nameChild)
						{
							haveRepeat = true;
						}
					}
					else
					{
						stop_reserch = true;
					}
					ds++;
				}
				if (haveRepeat == false)
				{
					stop_reserch = true;
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
		}

		i++;
	}
}

void GameObject::ShowHierarchy()
{
	if (!isVisible())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.385f, 0.385f, 0.385f, 1.00f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
	}
	bool treeNod = false;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (((Inspector*)App->gui->winManager[WindowName::INSPECTOR])->GetSelected() == this)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (childs.size() == 0)
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf;
	}
	//ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNodeEx(name, node_flags))
	{
		treeNod = true;
	}
	if (ImGui::IsItemClicked())
	{
		//Set inspector window of this Game Object
		((Inspector*)App->gui->winManager[WindowName::INSPECTOR])->LinkObject(this);
		((Hierarchy*)App->gui->winManager[WindowName::HIERARCHY])->SetGameObjectSelected(this);
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
	if(treeNod)
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
	if (ImGui::MenuItem("Copy"))
	{
		((Hierarchy*)App->gui->winManager[WindowName::HIERARCHY])->SetGameObjectCopy(this);
	}
	if (ImGui::MenuItem("Paste"))
	{
		((Hierarchy*)App->gui->winManager[WindowName::HIERARCHY])->CopyGameObject(this);
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
		if (App->engineState == EngineState::STOP)
		{
			((Hierarchy*)App->gui->winManager[WindowName::HIERARCHY])->SetGameObjecttoDelete(this);
		}
		else
		{
			LOG("Deleting a GameObject while PlayMode may cause crashes... you can't delete now.");
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Create Empty"))
	{
		GameObject* empty = App->scene->CreateGameObject(this);
		((Inspector*)App->gui->winManager[WindowName::INSPECTOR])->LinkObject(empty);
	}
	if (ImGui::MenuItem("Create Cube"))
	{
		GameObject* cube = App->scene->CreateCube(this);
		((Inspector*)App->gui->winManager[WindowName::INSPECTOR])->LinkObject(cube);
	}
	//if (ImGui::MenuItem("Sphere"))
	//{
	//	GameObject* sphere = App->scene->CreateSphere(this);
	//	((Inspector*)App->gui->winManager[WindowName::INSPECTOR])->LinkObject(sphere);
	//}
	ImGui::Separator();
	ImGui::MenuItem("ADD COMPONENT", NULL, false, false);
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
}

void GameObject::ShowInspectorInfo()
{
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.211f, 0.211f, 0.211f, 1.00f));
	if (ImGui::BeginChild(ImGui::GetID("Inspector"), ImVec2(ImGui::GetWindowWidth(), 90)))
	{
		static GLuint icon_GameObject = App->textures->LoadTexture("Images/UI/icon_GameObject.png");
		ImGui::Spacing();

		ImGui::Text(""); ImGui::SameLine(5);
		ImGui::Image((ImTextureID*)icon_GameObject, ImVec2(23, 23), ImVec2(-1, 1), ImVec2(0, 0)); ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));

		/* ENABLE-DISABLE CHECKBOX*/
		ImGui::Checkbox("##1", &active);

		/* NAME OF THE GAMEOBJECT */
		ImGui::SameLine();
		ImGui::PopStyleVar();
		char namedit[50];
		strcpy_s(namedit, 50, name);
		if (ImGui::InputText("##nameModel", namedit, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			name = App->fs->ConverttoChar(std::string(namedit).c_str());
		}
		//ImGui::InputText("##nameModel", name, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine(); App->ShowHelpMarker("Hold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n");
		ImGui::PopStyleVar();

		/* STATIC CHECKBOX */
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
		
		static bool window_active = false;
		static bool static_object = false;
		ImGui::Text(""); ImGui::SameLine(8);
		if (ImGui::Checkbox("##2", &static_obj))
		{
			if (App->engineState == EngineState::STOP)
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
	if (bounding_box != nullptr)
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
		if(open)
		{
			components[i]->ShowInspectorInfo();
			ImGui::Separator();
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("");
	ImGui::SameLine(ImGui::GetWindowWidth() / 2 - Width_AddComponent / 2);
	//ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2);
	static bool add_component = false;
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.208f, 0.208f, 0.208f, 1.00f));
	if (ImGui::ButtonEx("ADD COMPONENT", ImVec2(Width_AddComponent, 20)))
	{
		add_component = !add_component;
	}
	ImGui::PopStyleColor();
	ImVec2 pos_min = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
	ImVec2 pos_max = ImVec2(pos_min.x + (ImGui::GetWindowWidth() / 2), pos_min.y + (Width_AddComponent / 2));
	if (add_component)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.258f, 0.258f, 0.258f, 1.00f));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y));
		ImGui::SetNextWindowSize(ImVec2(Width_AddComponent, Width_AddComponent / 2));
		ImGui::Begin("AddComponent", NULL, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.311f, 0.311f, 0.311f, 1.00f));
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
		ImGui::End();
		ImGui::PopStyleColor();
	}
	if (ImGui::IsMouseHoveringRect(pos_min, pos_max) == false)
	{
		if(ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(2))
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
}

void GameObject::SetVisible(bool visible)
{
	this->visible = visible;
}

void GameObject::SetStatic(bool set_static)
{
	static_obj = set_static;
}

bool GameObject::isActive() const
{
	return active;
}

bool GameObject::isVisible() const
{
	return visible;
}

bool GameObject::isStatic() const
{
	return static_obj;
}

Component* GameObject::FindComponentByType(Comp_Type type) const
{
	Component* comp = nullptr;

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type) // We need to check if the component is ACTIVE first?¿
		{
			comp = components[i];
			break;
		}
	}

	return comp;
}

Component* GameObject::AddComponent(Comp_Type type, bool isFromLoader)
{
	bool dupe = false;
	for (uint i = 0; i < components.size(); i++)
	{
		//We need to check if there is already a component of that type (no duplication)
		if (components[i]->GetType() == type) 
		{
			dupe = true;
			LOG("There's already one component of this type in '%s'.", name);
			break;
		}
	}

	if (!dupe)
	{
		if (type == Comp_Type::C_MESH)
		{
			LOG("Adding MESH COMPONENT.");
			CompMesh* mesh = new CompMesh(type, this);
			components.push_back(mesh);
			/* Link Material to the Mesh if exists */
			const CompMaterial* material_link = (CompMaterial*)FindComponentByType(Comp_Type::C_MATERIAL);
			if (material_link != nullptr)
			{
				mesh->LinkMaterial(material_link);
			}
			else
			{
				LOG("Havent Material");
			}
			return mesh;
		}

		else if (type == Comp_Type::C_TRANSFORM)
		{
			LOG("Adding TRANSFORM COMPONENT.");
			CompTransform* transform = new CompTransform(type, this);
			components.push_back(transform);
			return transform;
		}

		else if (type == Comp_Type::C_MATERIAL)
		{
			LOG("Adding MATERIAL COMPONENT.");
			CompMaterial* material = new CompMaterial(type, this);
			components.push_back(material);

			/* Link Material to the Mesh if exists */
			CompMesh* mesh_to_link = (CompMesh*)FindComponentByType(Comp_Type::C_MESH);
			if (mesh_to_link != nullptr)
			{
				mesh_to_link->LinkMaterial(material);
			}
			else
			{
				LOG("MATERIAL not linked to any mesh");
			}
			return material;
		}

		else if (type == Comp_Type::C_CAMERA)
		{
			LOG("Adding CAMERA COMPONENT.");
			CompCamera* camera = new CompCamera(type, this);
			components.push_back(camera);
			return camera;
		}

		else if (type == Comp_Type::C_SCRIPT)
		{
			LOG("Adding SCRIPT COMPONENT.");
			CompScript* script = new CompScript(type, this);
			if (isFromLoader == false)
			{
				script->Init();
			}
			components.push_back(script);
			return script;
		}
	}

	return nullptr;
}

void GameObject::AddComponentCopy(const Component& copy)
{
	switch (copy.GetType())
	{
	case (Comp_Type::C_TRANSFORM):
	{
		CompTransform* transform = new CompTransform((CompTransform&)copy, this); //Transform copy constructor
		components.push_back(transform);
		break;
	}
	case (Comp_Type::C_MESH):
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
	case (Comp_Type::C_MATERIAL):
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
			LOG("MATERIAL not linked to any mesh");
		}
		break;
	}
	case (Comp_Type::C_CAMERA):
	{
		CompCamera* camera = new CompCamera((CompCamera&)copy, this); //Camera copy constructor
		components.push_back(camera);
		break;
	}
	case (Comp_Type::C_SCRIPT):
	{
		CompScript* script = new CompScript((CompScript&)copy, this); //Script copy constructor
		components.push_back(script);
		break;
	}
	default:
		break;
	}
}

void GameObject::SaveComponents(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	for (int i = 0; i < components.size(); i++)
	{
		std::string temp = name + "Component " + std::to_string(i) + ".";
		components[i]->Save(object, temp, saveScene, countResources);
	}
}

void GameObject::LoadComponents(const JSON_Object* object, std::string name, uint numComponents)
{
	// First Add All components by type
	for (int i = 0; i < numComponents; i++)
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
		case Comp_Type::C_MESH:
			this->AddComponent(Comp_Type::C_MESH);
			break;
		case Comp_Type::C_MATERIAL:
			this->AddComponent(Comp_Type::C_MATERIAL);
			break;
		case Comp_Type::C_CAMERA:
			this->AddComponent(Comp_Type::C_CAMERA);
			break;
		case Comp_Type::C_SCRIPT:
			this->AddComponent(Comp_Type::C_SCRIPT, true);
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
	}
}

int GameObject::GetNumComponents() const
{
	return components.size();
}

CompTransform* GameObject::GetComponentTransform() const
{
	return (CompTransform*)FindComponentByType(Comp_Type::C_TRANSFORM);
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
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->Clear();
		delete components[i];
	}
	components.clear();
}

void GameObject::DeleteComponent(Component* component)
{
	if (component != nullptr && components.size() > 0)
	{
		std::vector<Component*>::iterator item = components.begin();
		for (int i = 0; i < components.size(); i++)
		{
			if (component == components[i])
			{
				components.erase(item);
				break;
			}
			item++;
		}
		RELEASE(component);
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
		for (int i = 0; i < childs.size(); i++)
		{
			if (strcmp(childs[i]->GetName(), name) == 0)
			{
				return childs[i];
			}
		}
	}
	return nullptr;
}

uint GameObject::GetIndexChildbyName(const char * name) const
{
	if (childs.size() > 0)
	{
		for (int i = 0; i < childs.size(); i++)
		{
			if (strcmp(childs[i]->GetName(), name) == 0)
			{
				return i;
			}
		}
	}
	return 0;
}

void GameObject::RemoveChildbyIndex(uint index)
{
	if (childs.size() > 0)
	{
		std::vector<GameObject*>::iterator item = childs.begin();
		for (int i = 0; i < childs.size(); i++)
		{
			if (i == index)
			{
				GameObject* it = childs[i];
				RELEASE(it);
				childs.erase(item);
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
	temp->name = App->GetCharfromConstChar(temp_name.c_str());
	temp_name.clear();
	temp->parent = this;
	childs.push_back(temp);
}

void GameObject::AddChildGameObject_Load(GameObject* child)
{
	child->parent = this;
	childs.push_back(child);
}

void GameObject::AddChildGameObject_Replace(GameObject* child)
{
	child->parent = this;
	childs.push_back(child);
	App->scene->gameobjects.pop_back();
}

void GameObject::UpdateChildsMatrices()
{
	for (uint i = 0; i < childs.size(); i++)
	{
		childs[i]->GetComponentTransform()->UpdateGlobalMatrixRecursive();
	}
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

bool GameObject::HaveParent()
{
	return (parent != nullptr);
}

void GameObject::AddBoundingBox(const ResourceMesh* mesh)
{
	if (bounding_box == nullptr)
	{
		bounding_box = new AABB();
	}
	bounding_box->SetNegativeInfinity();
	bounding_box->Enclose(mesh->vertices, mesh->num_vertices);
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

bool GameObject::isAABBActive() const
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
	return toDelete;
}

void GameObject::SettoDelete()
{
	toDelete = true;
}

void GameObject::RemoveScriptReference(GameObject* go)
{
	CompScript* script = (CompScript*)FindComponentByType(Comp_Type::C_SCRIPT);
	if (script != nullptr)
	{
		script->RemoveReferences(go);
	}
}
