#include "ModuleResourceManager.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"
#include "ResourceScript.h"
#include "ImportMesh.h"
#include "ModuleFS.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"

ModuleResourceManager::ModuleResourceManager(bool start_enabled): Module(start_enabled)
{
	Start_enabled = true;
	preUpdate_enabled = true;

	name = "Resources Manager";
}

ModuleResourceManager::~ModuleResourceManager()
{
	std::map<uint, Resource*>::iterator it = resources.begin();
	for (int i = 0; i < resources.size(); i++)
	{
		it->second->DeleteToMemory();
		RELEASE(it->second);
		it++;
	}
	resources.clear();
	filesReimport.clear();
	for (int i = 0; i < resourcesToReimport.size(); i++)
	{
		RELEASE_ARRAY(resourcesToReimport[i].directoryObj);
		RELEASE_ARRAY(resourcesToReimport[i].nameMesh);
	}
	resourcesToReimport.clear();
	filestoDelete.clear();
}

bool ModuleResourceManager::Start()
{
	perf_timer.Start();

	// Create Resource Cube
	CreateResourceCube();
	Load();

	Start_t = perf_timer.ReadMs();
	return true;
}


update_status ModuleResourceManager::PreUpdate(float dt)
{
	perf_timer.Start();

	if (App->input->dropedfiles.size() > 0)
	{
		ImportFile(App->input->dropedfiles);
		App->input->dropedfiles.clear();
	}

	if (resourcesToReimport.size() > 0)
	{
		// Now ReImport the new Resources ------
		// First put all fbx and textures to vector, with this we dont repeat import.
		filesReimport.push_back(resourcesToReimport[0].directoryObj);
		for (int i = 1; i < resourcesToReimport.size(); i++)
		{
			if (strcmp(filesReimport[filesReimport.size() - 1], resourcesToReimport[i].directoryObj) != 0)
			{
				filesReimport.push_back(resourcesToReimport[i].directoryObj);
			}
		}
		// Then delete all Resources that want ReImport
		std::map<uint, Resource*>::iterator it;
		for (int i = 0; i < resourcesToReimport.size(); i++)
		{
			it = resources.find(resourcesToReimport[i].uuid);
			it->second->SetState(Resource::State::REIMPORTED);
			//delete it->second;
			//resources.erase(it);
		}
		reimportNow = true;
	}

	// Delete Resource from Memory if anyone use it.
	std::map<uint, Resource*>::iterator it = resources.begin();
	for (int i = 0; i < resources.size(); i++)
	{
		if (it->second->NumGameObjectsUseMe == 0 && it->second->GetType() != Resource::Type::SCRIPT)
		{
			if (it->second->IsLoadedToMemory() == Resource::State::LOADED)
			{
				it->second->DeleteToMemory();
				LOG("Resources: %s, unloaded from memory", it->second->name);
			}
		}
		it++;
	}

	// Prepare to Delete Resources and File in Library, so first set the resource state to WANTDELETE
	if (filestoDelete.size() > 0)
	{
		std::map<uint, Resource*>::iterator it;
		for (int i = 0; i < filestoDelete.size(); i++)
		{
			it = resources.find(filestoDelete[i]);
			it->second->SetState(Resource::State::WANTDELETE);
		}
		deleteNow = true;
	}
	if (reimportedScripts)
	{
		scriptsSetNormal = true;
		reimportedScripts = false;
	}
	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleResourceManager::PostUpdate(float dt)
{
	if (resourcesToReimport.size() > 0 && reimportNow)
	{
		// if a Resource state == Resource::State::REIMPORT delete it.
		std::map<uint, Resource*>::iterator it;
		for (int i = 0; i < resourcesToReimport.size(); i++)
		{
			it = resources.find(resourcesToReimport[i].uuid);
			if (it->second->GetState() == Resource::State::REIMPORTED)
			{
				// First Delete file save in Library
				if (it->second->GetType() == Resource::Type::MATERIAL)
				{
					App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MATERIALS);
				}
				else if (it->second->GetType() == Resource::Type::MESH)
				{
					App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MESHES);
				}
				else if (it->second->GetType() == Resource::Type::SCRIPT)
				{
					//need delete
					//App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MESHES);
				}
				delete it->second;
				resources.erase(it);
			}
		}

		// Now ReImport
		LOG("ReImporting...");
		ImportFile(filesReimport, resourcesToReimport);
		LOG("Finished ReImport.");
		// After reimport, update time of vector of files in filesystem.
		App->fs->UpdateFilesAsstes();
		filesReimport.clear();
		for (int i = 0; i < resourcesToReimport.size(); i++)
		{
			RELEASE_ARRAY(resourcesToReimport[i].directoryObj);
			RELEASE_ARRAY(resourcesToReimport[i].nameMesh);
		}
		resourcesToReimport.clear();
		reimportNow = false;
	}

	// Now Delete Resources and file in Library.
	if (filestoDelete.size() > 0 && deleteNow)
	{
		LOG("Deleting Resources...");
		std::map<uint, Resource*>::iterator it = resources.begin();
		for (int i = 0; i < resources.size(); i++)
		{
			if (it->second->GetState() == Resource::State::WANTDELETE)
			{
				// Delete to memory
				it->second->DeleteToMemory();
				// First Delete file save in Library
				LOG("Delete %s", it->second->name);
				if (it->second->GetType() == Resource::Type::MATERIAL)
				{
					App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MATERIALS);
				}
				else if (it->second->GetType() == Resource::Type::MESH)
				{
					App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MESHES);
				}
				else if (it->second->GetType() == Resource::Type::SCRIPT)
				{
					//Need delete
					//App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MESHES);
				}
				delete it->second;
				resources.erase(it);
				it = resources.begin();
				i = -1;
			}
			else
			{
				it++;
			}
		}
		LOG("Finished Delete Resources");
		filestoDelete.clear();
		deleteNow = false;
	}

	// If a script modificated return to normal stat
	if (scriptsSetNormal)
	{
		scriptsSetNormal = false;
		std::map<uint, Resource*>::iterator it = resources.begin();
		for (int i = 0; i < resources.size(); i++)
		{
			if (it->second->GetType() == Resource::Type::SCRIPT && it->second->GetState() == Resource::State::REIMPORTEDSCRIPT)
			{
				it->second->SetState(Resource::State::LOADED);
			}
			it++;
		}
	}

	return UPDATE_CONTINUE;
}

bool ModuleResourceManager::CleanUp()
{
	Save();
	std::map<uint, Resource*>::iterator it = resources.begin();
	for (int i = 0; i < resources.size(); i++)
	{
		it->second->SetState(Resource::State::WANTDELETE);
		it++;
	}
	return true;
}

void ModuleResourceManager::ImportFile(std::list<const char*>& file)
{
	std::list<const char*>::iterator& it = file.begin();
	for (int i = 0; i < file.size(); i++)
	{
		// Get Type file
		Resource::Type dropped_File_type = Resource::Type::UNKNOWN;
		dropped_File_type = CheckFileType(it._Ptr->_Myval);

		if (dropped_File_type != Resource::Type::UNKNOWN)
		{
			// Check if a folder!
			if (dropped_File_type == Resource::Type::FOLDER)
			{
				App->fs->GetAllFilesFromFolder(it._Ptr->_Myval, file);
				std::string newDirectory = it._Ptr->_Myval;
				newDirectory = App->fs->FixName_directory(newDirectory);
				std::string temp = "\\" + newDirectory;
				newDirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory() + temp;
				((Project*)App->gui->winManager[WindowName::PROJECT])->SetDirectory(App->fs->CreateFolder(newDirectory.c_str(), true).c_str());
				file.erase(it);
				i = -1;
				it = file.begin();
			}
			else if (App->importer->Import(it._Ptr->_Myval, dropped_File_type))
			{
				// Copy file to Specify folder in Assets (This folder is the folder active)
				App->fs->CopyFileToAssets(it._Ptr->_Myval, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory());
				it++;
			}
		}
		else
		{
			LOG("[error] This file: %s with this format %s is incorrect!", App->fs->FixName_directory(it._Ptr->_Myval).c_str(), App->fs->GetExtension(it._Ptr->_Myval));
		}
	}
	((Project*)App->gui->winManager[WindowName::PROJECT])->UpdateNow();
	App->fs->UpdateFilesAsstes();
}

void ModuleResourceManager::ImportFile(std::vector<const char*>& file, std::vector<ReImport>& resourcesToReimport)
{
	for (int i = 0; i < file.size(); i++)
	{
		// Get Type file
		Resource::Type dropped_File_type = Resource::Type::UNKNOWN;
		dropped_File_type = CheckFileType(file[i]);

		if (dropped_File_type != Resource::Type::UNKNOWN)
		{
			if (App->importer->Import(file[i], dropped_File_type, resourcesToReimport))
			{
				// Copy file to Specify folder in Assets (This folder is the folder active)
				//App->fs->CopyFileToAssets(it._Ptr->_Myval, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory());
			}
		}
		else
		{
			LOG("[error] This file: %s with this format %s is incorrect!", App->fs->FixName_directory(file[i]).c_str(), App->fs->GetExtension(file[i]));
		}
	}
	((Project*)App->gui->winManager[WindowName::PROJECT])->UpdateNow();
	App->fs->UpdateFilesAsstes();
}

Resource* ModuleResourceManager::CreateNewResource(Resource::Type type, uint uuid)
{
	Resource* ret = nullptr;
	uint uid;
	if(uuid == 0)
		uid = App->random->Int();
	else
		uid = uuid;

	switch (type) 
	{
	case Resource::MATERIAL: ret = (Resource*) new ResourceMaterial(uid); break;
	case Resource::MESH: ret = (Resource*) new ResourceMesh(uid); break;
	case Resource::SCRIPT: ret = (Resource*) new ResourceScript(uid); break;
	}
	if (ret != nullptr)
		resources[uid] = ret;
	return ret;
}

Resource* ModuleResourceManager::GetResource(uint id)
{
	std::map<uint, Resource*>::iterator it = resources.find(id);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

Resource* ModuleResourceManager::GetResource(const char* material)
{
	std::map<uint, Resource*>::iterator it = resources.begin();
	for (int i = 0; i < resources.size(); i++)
	{
		if (strcmp(it->second->name, material) == 0)
		{
			return it->second;
		}
		it++;
	}
	return nullptr;
}

Resource::Type ModuleResourceManager::CheckFileType(const char* filedir)
{
	if (filedir != nullptr)
	{
		std::string file_type;
		std::string path(filedir);

		size_t extension_pos = path.find_last_of(".");

		file_type = path.substr(extension_pos + 1);

		//Set lowercase the extension to normalize it
		for (std::string::iterator it = file_type.begin(); it != file_type.end(); it++)
		{
			*it = tolower(*it);
		}

		if (file_type == "png" || file_type == "jpg" || file_type == "dds" || file_type == "tga")
		{
			return Resource::Type::MATERIAL;
		}
		else if (file_type == "fbx" || file_type == "obj" || file_type == "FBX")
		{
			return Resource::Type::MESH;
		}
		else if (file_type == "cs")
		{
			return Resource::Type::SCRIPT;
		}
		else
		{
			if (std::experimental::filesystem::is_directory(filedir))
			{
				return Resource::Type::FOLDER;
			}
			else
			{
				return Resource::Type::UNKNOWN;
			}
		}
	}
}

void ModuleResourceManager::Init_IndexVertex(float3* vertex_triangulate, uint num_index, std::vector<uint>& indices, std::vector<float3>& vertices)
{
	bool temp = false;
	std::vector<float3> all_index;
	uint size = 0;

	for (int i = 0; i < num_index; i++)
	{
		temp = false;
		size = all_index.size();

		for (int j = 0; j < size; j++)
		{
			if (all_index[j] == vertex_triangulate[i])
			{
			indices.push_back(j);
			temp = true;
			}
		}

		if (temp == false)
		{
			all_index.push_back(vertex_triangulate[i]);
			indices.push_back(all_index.size() - 1);
			vertices.push_back(vertex_triangulate[i]);
		}
	}
}

void ModuleResourceManager::CreateResourceCube()
{
	OBB* box = new OBB();
	box->pos = float3::zero;
	box->r = float3::one;
	box->axis[0] = float3(1, 0, 0);
	box->axis[1] = float3(0, 1, 0);
	box->axis[2] = float3(0, 0, 1);

	AABB* bounding_box;
	bounding_box = new AABB(*box);
	RELEASE(box);
	float3* vertices_array = new float3[36];

	bounding_box->Triangulate(1, 1, 1, vertices_array, NULL, NULL, false);
	std::vector<uint> indices;
	std::vector<float3> vertices;
	Init_IndexVertex(vertices_array, 36, indices, vertices);
	App->importer->iMesh->Import(8, 36, 0, indices, vertices, 1); // 1 == Cube
	RELEASE_ARRAY(vertices_array);
	RELEASE(bounding_box);
}

Resource*  ModuleResourceManager::ShowResources(bool& active, Resource::Type type)
{
	const char* nameWindow = nullptr;
	const char* subname = nullptr;
	if (type == Resource::Type::MESH)
	{
		nameWindow = "Select Mesh";
		subname = "All Meshes";
	}
	else if (type == Resource::Type::MATERIAL)
	{
		nameWindow = "Select Material";
		subname = "All Materials:";
	}
	else if (type == Resource::Type::SCRIPT)
	{
		nameWindow = "Select Script";
		subname = "All Scripts:";
	}
	if (!ImGui::Begin(nameWindow, &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders)) //TODO ELLIOT CLOSE Windows example
	{
		ImGui::End();
	}
	else
	{
		ImGui::Text(subname);
		ImGui::Spacing();
		std::map<uint, Resource*>::iterator it = resources.begin();
		for (int i = 0; i < resources.size(); i++)
		{
			ImGui::PushID(i);
			if (type == it->second->GetType())
			{
				if (type == Resource::Type::SCRIPT && it->second->NumGameObjectsUseMe > 0)
				{
				}
				else
				{
					ImGui::ButtonEx(it->second->name);
					if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
					{
						if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
						{
							ImGui::PopID();
							ImGui::End();
							active = false;
							return GetResource(it->second->GetUUID());
						}
					}
				}
			}
			it++;
			ImGui::PopID();
		}
		ImGui::End();
	}
	return nullptr;
}

void ModuleResourceManager::ShowAllResources(bool& active) 
{
	if (!ImGui::Begin("Resources", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
	{
		ImGui::End();
	}
	else
	{
		static int selected_type = -1;
		static std::string type_Name[] = { "Material", "Mesh", "Script"};
		static Resource::Type type_R[] = { Resource::Type::MATERIAL,
			Resource::Type::MESH, Resource::Type::SCRIPT};
		//Frist Select Type
		ImGui::Text("Select Type Resource:");
		if (ImGui::Button("Select"))
			ImGui::OpenPopup("select");
		ImGui::SameLine();
		ImGui::Text(selected_type == -1 ? "<None>" : type_Name[selected_type].c_str());
		if (ImGui::BeginPopup("select"))
		{
			for (int i = 0; i < IM_ARRAYSIZE(type_Name); i++)
				if (ImGui::Selectable(type_Name[i].c_str()))
					selected_type = i;
			ImGui::EndPopup();
		}
		ImGui::Separator();
		Resource::Type typeResource = Resource::Type::UNKNOWN;
		if (selected_type != -1)
		{
			typeResource = type_R[selected_type];
		}
		std::map<uint, Resource*>::iterator it = resources.begin();
		for (int i = 0; i < resources.size(); i++)
		{
			ImGui::PushID(i);
			if (typeResource == it->second->GetType())
			{
				//Only Draw the Type Selected.
				ImGui::Bullet();
				ImGui::Text("Name: %s", it->second->name);
				char namedit[50];
				strcpy_s(namedit, 50, it->second->name);
				ImGui::AlignFirstTextHeightToWidgets();
				ImGui::Text("Edit Name: "); ImGui::SameLine();
				if (ImGui::InputText("##nameModel", namedit, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					it->second->name = App->fs->ConverttoChar(std::string(namedit).c_str());
					//TODO ELLIOT SaveMeta
				}
				ImGui::Text("Number of GameObjects Use this Resource: "); ImGui::SameLine();
				if (it->second->NumGameObjectsUseMe > 0)
				{
					ImGui::TextColored(ImVec4(0, 0.933, 0, 1), "%i", it->second->NumGameObjectsUseMe);
				}
				else
				{
					ImGui::TextColored(ImVec4(0.933, 0, 0, 1), "%i", it->second->NumGameObjectsUseMe);
				}
				ImGui::Text("UID of Resource:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 0.666, 1, 1), "%i", it->second->GetUUID());
				if (it->second->GetType() == Resource::Type::SCRIPT)
				{
					ImGui::Text("Directory in Assets: "); ImGui::SameLine();
					ImGui::TextColored(ImVec4(0, 0.933, 0, 1), "%s", it->second->path_assets.c_str());
				}
			}
			it++;
			ImGui::PopID();
		}
		//ImGui::Separator();
		ImGui::End();
	}
}

bool ModuleResourceManager::ReImportAllScripts()
{
	bool ret = true;
	reimportedScripts = true;
	std::map<uint, Resource*>::iterator it = resources.begin();
	for (int i = 0; i < resources.size(); i++)
	{
		if (Resource::Type::SCRIPT == it->second->GetType())
		{
			if (((ResourceScript*)it->second)->ReImportScript(((ResourceScript*)it->second)->GetPathdll()) == false)
			{
				it->second->SetState(Resource::State::FAILED);
				ret = false;
			}
			else
			{
				it->second->SetState(Resource::State::REIMPORTEDSCRIPT);
			}
		}
		it++;
	}
	return ret;
}

void ModuleResourceManager::Save()
{
	LOG("----- SAVING RESOURCES -----");

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("Resources.json");

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Resources");
		json_object_clear(config_node);
		int numResources = resources.size() - ResourcePrimitive;
		json_object_dotset_number_with_std(config_node, "Info.Number of Resources", numResources);
		json_object_dotset_boolean_with_std(config_node, "Info.Load Resources", loadResources);
		// Update Resoruces
		std::map<uint, Resource*>::iterator it = resources.begin();
		it++; // ++ = Resource "Cube" dont save - ResourcePrimitive
		for (uint i = 0; i < numResources; i++)
		{
			std::string name = "Resource " + std::to_string(i);
			name += ".";
			json_object_dotset_number_with_std(config_node, name + "UUID & UUID Directory", it->second->GetUUID());
			json_object_dotset_number_with_std(config_node, name + "Type", (int)it->second->GetType());
			json_object_dotset_string_with_std(config_node, name + "Name", it->second->name);
			json_object_dotset_string_with_std(config_node, name + "PathAssets", it->second->path_assets.c_str());
			it++;
		}
	}
	json_serialize_to_file(config_file, "Resources.json");
	json_value_free(config_file);
}

void ModuleResourceManager::Load()
{
	LOG("----- LOADING RESOURCES -----");

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("Resources.json");
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Resources");
		int NumberResources = json_object_dotget_number(config_node, "Info.Number of Resources");
		loadResources = json_object_dotget_boolean(config_node, "Info.Load Resources");
		if (loadResources)
		{
			if (NumberResources > 0)
			{
				for (int i = 0; i < NumberResources; i++)
				{
					std::string name = "Resource " + std::to_string(i);
					name += ".";
					Resource::Type type = (Resource::Type)(int)json_object_dotget_number_with_std(config_node, name + "Type");
					switch (type)
					{
					case Resource::Type::MESH:
					{
						uint uid = json_object_dotget_number_with_std(config_node, name + "UUID & UUID Directory");
						ResourceMesh* mesh = (ResourceMesh*)CreateNewResource(type, uid);
						mesh->name = App->GetCharfromConstChar(json_object_dotget_string_with_std(config_node, name + "Name"));
						break;
					}
					case Resource::Type::MATERIAL:
					{
						uint uid = json_object_dotget_number_with_std(config_node, name + "UUID & UUID Directory");
						ResourceMaterial* material = (ResourceMaterial*)CreateNewResource(type, uid);
						material->name = App->GetCharfromConstChar(json_object_dotget_string_with_std(config_node, name + "Name"));
						break;
					}
					case Resource::Type::SCRIPT:
					{
						uint uid = json_object_dotget_number_with_std(config_node, name + "UUID & UUID Directory");
						ResourceScript* script = (ResourceScript*)CreateNewResource(type, uid);
						script->path_assets = json_object_dotget_string_with_std(config_node, name + "PathAssets");
						script->name = App->GetCharfromConstChar(json_object_dotget_string_with_std(config_node, name + "Name"));
						break;
					}
					case Resource::Type::UNKNOWN:
					{
						LOG("Error load resoruce");
						break;
					}
					}
				}
			}
		}
	}
	json_value_free(config_file);
}
