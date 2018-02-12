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
#include "CompRectTransform.h"
#include "CompMaterial.h"
#include "CompCamera.h"
#include "CompScript.h"
#include "CompAnimation.h"
#include "CompButton.h"
#include "CompCheckBox.h"
#include "CompImage.h"
#include "CompText.h"
#include "CompEditText.h"
#include "CompCanvas.h"
#include "CompCanvasRender.h"
#include "CompAudio.h"

//Event system test
#include "ModuleEventSystem.h"

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
	active = copy.IsActive();
	visible = copy.IsVisible();
	static_obj = copy.IsStatic();
	bb_active = copy.IsAABBActive();
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

void GameObject::ClearAllVariablesScript()
{
	if (active)
	{
		//Start Active scripts --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->IsActive())
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
			if (childs[i]->IsActive())
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
				DeleteComponent(components[i]);
			}
			else
			{
				if (components[i]->IsActive())
				{
					components[i]->PreUpdate(dt);
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
		for (uint i = 0; i < components.size(); i++)
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
	to_delete = check_delete;
}

bool GameObject::IsDeleteFixed() const
{
	return to_delete;
}

void GameObject::Draw()
{
	if (visible)
	{
		//Draw Components --------------------------
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->IsActive() && components[i]->GetType() == Comp_Type::C_MESH)
			{
				components[i]->Draw();
				/*
				Event draw_event;
				draw_event.draw.type = EventType::EVENT_DRAW;
				draw_event.draw.Dtype = draw_event.draw.DRAW_3D;
				draw_event.draw.ToDraw = components[i];
				PushEvent(draw_event);
				*/
			}
		}

		//Draw child Game Objects -------------------
		for (uint i = 0; i < childs.size(); i++)
		{
			if (childs[i]->IsActive())
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

	if (haveParent == false)
	{
		parent_ = App->scene->root;
	}

	while (stop == false)
	{

		if (i < parent_->GetNumChilds())
		{
			bool stop_research = false;
			int j = 0;
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

void GameObject::ShowHierarchy()
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
	if (ImGui::TreeNodeEx(name, node_flags))
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
		((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->SetGameObjectCopy(this);
	}
	if (ImGui::MenuItem("Paste"))
	{
		((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->CopyGameObject(this);
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
			e.deletego.type = EventType::EVENT_DELETE_GO;
			e.deletego.Todelte = this;
			PushEvent(e);
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
	ImGui::SameLine(ImGui::GetWindowWidth() / 2 - WIDTH_ADDCOMPONENT / 2);
	//ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2);
	static bool add_component = false;
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.208f, 0.208f, 0.208f, 1.00f));
	if (ImGui::ButtonEx("ADD COMPONENT", ImVec2(WIDTH_ADDCOMPONENT, 20)))
	{
		add_component = !add_component;
	}
	ImGui::PopStyleColor();
	ImVec2 pos_min = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
	ImVec2 pos_max = ImVec2(pos_min.x + (ImGui::GetWindowWidth() / 2) + 42, pos_min.y + (WIDTH_ADDCOMPONENT / 2) + 20); // Magic number
	if (add_component)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.258f, 0.258f, 0.258f, 1.00f));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y));
		ImGui::SetNextWindowSize(ImVec2(WIDTH_ADDCOMPONENT, WIDTH_ADDCOMPONENT / 2));
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
		if (ImGui::MenuItem("Animation"))
		{
			AddComponent(Comp_Type::C_ANIMATION);
			add_component = false;
		}
		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Canvas"))
			{
				AddComponent(Comp_Type::C_CANVAS);
				add_component = false;
			}
			if (ImGui::MenuItem("Image"))
			{
				AddComponent(Comp_Type::C_IMAGE);
				add_component = false;
			}
			if (ImGui::MenuItem("Text"))
			{
				AddComponent(Comp_Type::C_TEXT);
				add_component = false;
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Audio"))
		{
			AddComponent(Comp_Type::C_AUDIO);
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

bool GameObject::IsActive() const
{
	return active;
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
		if (components[i]->GetType() == type) // We need to check if the component is ACTIVE first?¿
		{
			return components[i];
		
		}
	}
	return nullptr;
}

Component * GameObject::FindParentComponentByType(Comp_Type type)const
{
	
	Component * ret = nullptr;
	ret = FindComponentByType(Comp_Type::C_CANVAS);
	if (ret == nullptr)
	{
		GameObject* item = parent;
		while (item != nullptr)
		{
			ret = ((CompCanvas*)item->FindComponentByType(Comp_Type::C_CANVAS));
			if (ret != nullptr)
				break;
			item = item->GetParent();
		}
	}
	return ret;
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
		else if (type == Comp_Type::C_RECT_TRANSFORM)
		{
			if (FindComponentByType(Comp_Type::C_RECT_TRANSFORM) != nullptr)
			{
				return nullptr;
			}
			LOG("Adding RECT TRANSFORM COMPONENT.");
			CompRectTransform* transform = new CompRectTransform(type, this);
			if (!components.empty())
			{
				RELEASE(components[0]);
				components.at(0) = transform;
			}
			else
			{
				components.push_back(transform);
			}
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
		else if (type == Comp_Type::C_CANVAS)
		{
			AddComponent(Comp_Type::C_RECT_TRANSFORM);
			LOG("Adding CANVAS COMPONENT.");
			CompCanvas* canvas = new CompCanvas(type, this);
			components.push_back(canvas);

			//If is not RectTransform
			//TODO change transform
			return canvas;
		}
		else if (type == Comp_Type::C_CANVAS_RENDER)
		{
			LOG("Adding CANVAS RENDER COMPONENT.");
			CompCanvasRender* canvas_renderer = new CompCanvasRender(type, this);
			components.push_back(canvas_renderer);

			return canvas_renderer;
		}
		else if (type == Comp_Type::C_IMAGE)
		{

			//If is not RectTransform
			//TODO change transform
			LOG("Adding CANVAS RENDER COMPONENT.");
			CompCanvasRender* canvas_renderer = new CompCanvasRender(Comp_Type::C_CANVAS_RENDER, this);
			components.push_back(canvas_renderer);
			LOG("Adding IMAGE COMPONENT.");
			CompImage* image = new CompImage(type, this);
			components.push_back(image);
			canvas_renderer->ProcessImage(image);
			return image;
		}
		else if (type == Comp_Type::C_TEXT)
		{

			//If is not RectTransform
			//TODO change transform
			LOG("Adding CANVAS RENDER COMPONENT.");
			CompCanvasRender* canvas_renderer = new CompCanvasRender(Comp_Type::C_CANVAS_RENDER, this);
			components.push_back(canvas_renderer);
			LOG("Adding TEXT COMPONENT.");
			CompText* text = new CompText(type, this);
			components.push_back(text);
			return text;
		}
		else if (type == Comp_Type::C_EDIT_TEXT)
		{

			//If is not RectTransform
			//TODO change transform
			LOG("Adding EDIT TEXT COMPONENT.");
			CompEditText* edit_text = new CompEditText(type, this);
			components.push_back(edit_text);

			/* Link image to the button if exists */
			CompText* text_to_link = (CompText*)FindComponentByType(Comp_Type::C_TEXT);
			if (text_to_link != nullptr)
			{
			}
			else
			{
				LOG("TEXT not linked to any Edit Text");
			}
			return edit_text;
		}
		else if (type == Comp_Type::C_BUTTON)
		{

			//If is not RectTransform
			//TODO change transform
			LOG("Adding BUTTON COMPONENT.");
			CompButton* button = new CompButton(type, this);
			components.push_back(button);

			/* Link image to the button if exists */
			CompImage* image_to_link = (CompImage*)FindComponentByType(Comp_Type::C_IMAGE);
			if (image_to_link != nullptr)
			{
			}
			else
			{
				LOG("IMAGE not linked to any Button");
			}
			return button;
		}
		else if (type == Comp_Type::C_CHECK_BOX)
		{

			//If is not RectTransform
			//TODO change transform
			LOG("Adding CHECK BOX COMPONENT.");
			CompCheckBox* check_box = new CompCheckBox(type, this);
			components.push_back(check_box);

			/* Link image to the button if exists */
			CompImage* image_to_link = (CompImage*)FindComponentByType(Comp_Type::C_IMAGE);
			if (image_to_link != nullptr)
			{

			}
			else
			{
				LOG("IMAGE not linked to any CheckBox");
			}
			return check_box;
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
		else if (type == Comp_Type::C_ANIMATION)
		{
			LOG("Adding ANIMATION COMPONENT.");
			CompAnimation* anim = new CompAnimation(type, this);
			components.push_back(anim);
			return anim;
		}
		else if (type == Comp_Type::C_AUDIO)
		{
			LOG("Adding AUDIO COMPONENT.");
			CompAudio* audio = new CompAudio(type, this);
			components.push_back(audio);
			return audio;
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
	case (Comp_Type::C_ANIMATION):
	{
		CompAnimation* anim = new CompAnimation((CompAnimation&)copy, this); //Anim copy constructor
		components.push_back(anim);
		break;
	}
	case (Comp_Type::C_AUDIO):
	{
		CompAudio* audio = new CompAudio((CompAudio&)copy, this); //Audio copy constructor
		components.push_back(audio);
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
			this->AddComponent(Comp_Type::C_ANIMATION, true);
			break;
		case Comp_Type::C_AUDIO:
			this->AddComponent(Comp_Type::C_AUDIO, true);
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
		if (((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied() == components[i])
		{
			((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetLinkComponentNull();
		}
		components[i]->Clear();
		RELEASE(components[i]);
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
				if (((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied() == components[i])
				{
					((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetLinkComponentNull();
				}
				components[i]->Clear();
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
		childs[i]->GetComponentTransform()->UpdateGlobalMatrixRecursive();
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

void GameObject::RemoveScriptReference(GameObject* go)
{
	CompScript* script = (CompScript*)FindComponentByType(Comp_Type::C_SCRIPT);
	if (script != nullptr)
	{
		script->RemoveReferences(go);
	}
}
