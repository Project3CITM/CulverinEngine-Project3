#include "ModuleResourceManager.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"
#include "ResourceScript.h"
#include "ResourceAnimation.h"
#include "ResourceFont.h"
#include "ImportMesh.h"
#include "JSONSerialization.h"
#include "ModuleFS.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"

ModuleResourceManager::ModuleResourceManager(bool start_enabled): Module(start_enabled)
{	
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
	postUpdate_enabled = true;

	name = "Resources Manager";
}

ModuleResourceManager::~ModuleResourceManager()
{
	std::map<uint, Resource*>::iterator it = resources.begin();
	for(; it != resources.end(); it++)
	{
		it->second->DeleteToMemory();
		RELEASE(it->second);
	}
	resources.clear();
	files_reimport.clear();
	for (int i = 0; i < resources_to_reimport.size(); i++)
	{
		RELEASE_ARRAY(resources_to_reimport[i].directory_obj);
		RELEASE_ARRAY(resources_to_reimport[i].name_mesh);
		RELEASE_ARRAY(resources_to_reimport[i].path_dll);
	}
	resources_to_reimport.clear();
	files_to_delete.clear();
}

bool ModuleResourceManager::Init(JSON_Object* node)
{
	CreateResourceCube();
	CreateResourcePlane();

	// Load resources with meta
	NewLoad();

	return true;
}

bool ModuleResourceManager::Start()
{
	perf_timer.Start();

	if (App->mode_game == false)
	{
		for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
		{
			std::string path_resources_library;
			uint uid_resources = it->first;
			App->json_seria->ResourcesInLibrary(path_resources_library, (int)it->second->GetType());
			path_resources_library += std::to_string(uid_resources);
			if (uid_resources != 0)
			{
				Resource* to_reimport = GetResource(uid_resources);
				if (to_reimport->GetType() == Resource::Type::SCRIPT)
				{
					path_resources_library += ".dll";
				}
				if (to_reimport->GetType() == Resource::Type::MATERIAL)
				{
					path_resources_library += ".dds";
				}
				if (to_reimport->GetType() == Resource::Type::FONT)
				{
					path_resources_library += ".ttf";
				}
				if (App->fs->CheckIsFileExist(path_resources_library) || to_reimport->GetType() == Resource::Type::ANIMATION)
				{
					continue;
				}

				if (to_reimport != nullptr)
				{
					ReImport temp;
					if (to_reimport->GetType() == Resource::Type::MATERIAL)
					{
						std::string temp_mat = "Assets/";
						temp_mat += to_reimport->name;
						temp.directory_obj = App->fs->ConverttoConstChar(temp_mat);
					}
					else
						temp.directory_obj = App->fs->ConverttoConstChar(to_reimport->path_assets);
					temp.name_mesh = App->fs->ConverttoConstChar(to_reimport->name);
					if (to_reimport->GetType() == Resource::Type::SCRIPT)
					{
						temp.path_dll = App->fs->ConverttoConstChar(((ResourceScript*)to_reimport)->GetPathdll());
					}
					temp.uuid = uid_resources;
					resources_to_reimport.push_back(temp);
				}
			}
		}
		if (resources_to_reimport.size() > 0)
		{
			files_reimport.push_back(resources_to_reimport[0].directory_obj);
			for (int i = 1; i < resources_to_reimport.size(); i++)
			{
				if (strcmp(files_reimport[files_reimport.size() - 1], resources_to_reimport[i].directory_obj) != 0)
				{
					files_reimport.push_back(resources_to_reimport[i].directory_obj);
				}
			}
			// if a Resource state == Resource::State::REIMPORT delete it.
			std::map<uint, Resource*>::iterator it;
			for (int i = 0; i < resources_to_reimport.size(); i++) // i = 1 -> ResourcePrimitive
			{
				it = resources.find(resources_to_reimport[i].uuid);
				delete it->second;
				resources.erase(it);
			}

			// Now ReImport
			LOG("ReImporting...");
			ImportFile(files_reimport, resources_to_reimport, true);
			LOG("Finished ReImport.");
			// After reimport, update time of vector of files in filesystem.
			App->fs->UpdateFilesAssets();
			files_reimport.clear();
			for (int i = 0; i < resources_to_reimport.size(); i++)
			{
				RELEASE_ARRAY(resources_to_reimport[i].directory_obj);
				RELEASE_ARRAY(resources_to_reimport[i].name_mesh);
				RELEASE_ARRAY(resources_to_reimport[i].path_dll);
			}
			resources_to_reimport.clear();
			reimport_now = false;
		}
	}

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

	//Reimport All files ----------------
	if (reimportAll)
	{
		App->fs->ReImportAllFiles();
	}

	if (resources_to_reimport.size() > 0)
	{
		// Now ReImport the new Resources ------
		// First put all fbx and textures to vector, with this we dont repeat import.
		files_reimport.push_back(resources_to_reimport[0].directory_obj);
		for (int i = 1; i < resources_to_reimport.size(); i++)
		{
			if (strcmp(files_reimport[files_reimport.size() - 1], resources_to_reimport[i].directory_obj) != 0)
			{
				files_reimport.push_back(resources_to_reimport[i].directory_obj);
			}
		}
		// Then delete all Resources that want ReImport
		std::map<uint, Resource*>::iterator it;
		for (int i = 0; i < resources_to_reimport.size(); i++)
		{
			it = resources.find(resources_to_reimport[i].uuid);
			it->second->SetState(Resource::State::REIMPORTED);
			//delete it->second;
			//resources.erase(it);
		}
		reimport_now = true;
	}

	// Delete Resource from Memory if anyone use it.
	std::map<uint, Resource*>::iterator it = resources.begin();
	for (int i = 0; i < resources.size(); i++)
	{
		if (it->second->num_game_objects_use_me == 0 && it->second->GetType() != Resource::Type::SCRIPT)
		{
			if (it->second->IsLoadedToMemory() == Resource::State::LOADED)
			{
				it->second->DeleteToMemory();
				//LOG("Resources: %s, unloaded from memory", it->second->name);
			}
		}
		it++;
	}

	// Prepare to Delete Resources and File in Library, so first set the resource state to WANTDELETE
	if (files_to_delete.size() > 0)
	{
		std::map<uint, Resource*>::iterator it;
		for (int i = 0; i < files_to_delete.size(); i++)
		{
			it = resources.find(files_to_delete[i]);
			it->second->SetState(Resource::State::WANTDELETE);
		}
		delete_now = true;
	}
	if (reimported_scripts)
	{
		scripts_set_normal = true;
		reimported_scripts = false;
	}
	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleResourceManager::PostUpdate(float dt)
{
	if (resources_to_reimport.size() > 0 && reimport_now)
	{
		// if a Resource state == Resource::State::REIMPORT delete it.
		std::map<uint, Resource*>::iterator it;
		for (int i = 0; i < resources_to_reimport.size(); i++) // i = 1 -> ResourcePrimitive
		{
			it = resources.find(resources_to_reimport[i].uuid);
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
					App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_SCRIPTS);
				}
				else if (it->second->GetType() == Resource::Type::ANIMATION)
				{
					App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_ANIMATIONS);
				}
				else if (it->second->GetType() == Resource::Type::FONT)
				{
					App->fs->DeleteFileLibrary(std::to_string(it->second->GetUUID()).c_str(), DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_ANIMATIONS);
				}
				delete it->second;
				resources.erase(it);
			}
		}

		if (reimportAll)
		{
			std::map<uint, Resource*>::iterator it = resources.begin();
			for (int i = 1; i < resources.size(); i++) // i = 1 -> ResourcePrimitive
			{
				RELEASE(it->second);
				resources.erase(it);
				it = resources.begin();
				i = 0;
			}
			reimportAll = false;
		}

		// Now ReImport
		LOG("ReImporting...");
		ImportFile(files_reimport, resources_to_reimport);
		LOG("Finished ReImport.");
		// After reimport, update time of vector of files in filesystem.
		App->fs->UpdateFilesAssets();
		files_reimport.clear();
		for (int i = 0; i < resources_to_reimport.size(); i++)
		{
			RELEASE_ARRAY(resources_to_reimport[i].directory_obj);
			RELEASE_ARRAY(resources_to_reimport[i].name_mesh);
			RELEASE_ARRAY(resources_to_reimport[i].path_dll);
		}
		resources_to_reimport.clear();
		reimport_now = false;
	}

	// Now Delete Resources and file in Library.
	if (files_to_delete.size() > 0 && delete_now)
	{
		LOG("Deleting Resources...");
		std::map<uint, Resource*>::iterator it = resources.begin();
		it++;
		for (int i = 0; i < resources.size(); i++)
		{
			if (it->second->GetState() == Resource::State::WANTDELETE)
			{
				// Delete to memory
				it->second->DeleteToMemory();
				// First Delete file save in Library
				LOG("Delete %s", it->second->name.c_str());
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
		files_to_delete.clear();
		delete_now = false;
	}

	// If a script modificated return to normal stat
	if (scripts_set_normal)
	{
		scripts_set_normal = false;
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
				std::string new_directory = it._Ptr->_Myval;
				new_directory = App->fs->FixName_directory(new_directory);
				std::string temp = "\\" + new_directory;
				new_directory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory() + temp;
				((Project*)App->gui->win_manager[WindowName::PROJECT])->SetDirectory(App->fs->CreateFolder(new_directory.c_str(), true).c_str());
				file.erase(it);
				i = -1;
				it = file.begin();
			}
			else
			{
				std::string new_file = App->fs->CopyFileToAssets(it._Ptr->_Myval, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory());
				if (new_file != "")
				{
					if (App->importer->Import(new_file.c_str(), dropped_File_type))
					{
						// Copy file to Specify folder in Assets (This folder is the folder active)

						it++;
					}
				}
				else if (App->importer->Import(it._Ptr->_Myval, dropped_File_type))
				{
					// Copy file to Specify folder in Assets (This folder is the folder active)

					it++;
				}
			}
		}
		else
		{
			LOG("[error] This file: %s with this format %s is incorrect!", App->fs->FixName_directory(it._Ptr->_Myval).c_str(), App->fs->GetExtension(it._Ptr->_Myval));
		}
	}
	((Project*)App->gui->win_manager[WindowName::PROJECT])->UpdateNow();
	App->fs->UpdateFilesAssets();
}

void ModuleResourceManager::ImportFile(std::vector<const char*>& file, std::vector<ReImport>& resourcesToReimport, bool auto_reimport)
{
	for (int i = 0; i < file.size(); i++)
	{
		// Get Type file
		Resource::Type dropped_File_type = Resource::Type::UNKNOWN;
		dropped_File_type = CheckFileType(file[i]);

		if (dropped_File_type != Resource::Type::UNKNOWN)
		{
			if (App->importer->Import(file[i], dropped_File_type, resourcesToReimport, auto_reimport))
			{
				// Copy file to Specify folder in Assets (This folder is the folder active)
				//App->fs->CopyFileToAssets(it._Ptr->_Myval, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory());
			}
		}
		else
		{
			LOG("[error] This file: %s with this format %s is incorrect!", App->fs->FixName_directory(file[i]).c_str(), App->fs->GetExtension(file[i]).c_str());
		}
	}
	if (App->mode_game == false)
	{
		if (auto_reimport == false)
		{
			((Project*)App->gui->win_manager[WindowName::PROJECT])->UpdateNow();
		}
	}
	App->fs->UpdateFilesAssets();
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
	case Resource::ANIMATION: ret = (Resource*) new ResourceAnimation(uid); break;
	case Resource::FONT: ret = (Resource*) new ResourceFont(uid); break;

	}
	if (ret != nullptr)
	{
		if (resources[uid] != nullptr)
		{
			delete resources[uid];
			resources[uid] = nullptr;
		}
		resources[uid] = ret;
	}
	return ret;
}

Resource* ModuleResourceManager::GetResource(uint id)
{
	std::map<uint, Resource*>::iterator it = resources.find(id);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

Resource* ModuleResourceManager::GetResource(const char* name)
{
	std::map<uint, Resource*>::iterator it = resources.begin();
	for (int i = 0; i < resources.size(); i++)
	{
		if (strcmp(it->second->name.c_str(), name) == 0)
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
		else if (file_type == "fbx" || file_type == "obj" || file_type == "FBX" || file_type == "dae" || file_type == "DAE")
		{
			return Resource::Type::MESH;
		}
		else if (file_type == "cs")
		{
			return Resource::Type::SCRIPT;
		}
		else if (file_type == "ttf")
		{
			return Resource::Type::FONT;
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
	return Resource::Type::UNKNOWN;
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
	App->importer->iMesh->Import(8, 36, 0, 0, indices, vertices, "cube_default", 2); // 2 == Cube
	RELEASE_ARRAY(vertices_array);
	RELEASE(bounding_box);
}

void ModuleResourceManager::CreateResourcePlane()
{

	std::vector<uint> indices;
	std::vector<float3> vertices;

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);

	vertices.push_back(float3(-0.5, 0, 0.5));
	vertices.push_back(float3(0.5, 0, 0.5));
	vertices.push_back(float3(-0.5, 0, -0.5));
	vertices.push_back(float3(0.5, 0, -0.5));

	App->importer->iMesh->Import(4, 6, 0, 0, indices, vertices, "plane_default",4); // 3 == Plane

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
	else if (type == Resource::Type::ANIMATION)
	{
		nameWindow = "Select Animations";
		subname = "All Animations:";
	}
	else if (type == Resource::Type::FONT)
	{
		nameWindow = "Select font";
		subname = "All Fonts:";
	}
	if (!ImGui::Begin(nameWindow, &active, ImGuiWindowFlags_NoCollapse)) //TODO ELLIOT CLOSE Windows example
	{
		ImGui::End();
	}
	else
	{
		ImGui::Text(subname);
		ImGui::Spacing();
		static ImGuiTextFilter filter;
		filter.Draw();
		std::map<uint, Resource*>::iterator it = resources.begin();
		for (int i = 0; i < resources.size(); i++)
		{
			ImGui::PushID(i);
			if (type == it->second->GetType())
			{
				if (filter.PassFilter(it->second->name.c_str()))
				{
					ImGui::ButtonEx(it->second->name.c_str());
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
		//for (int i = 0; i < resources.size(); i++)
		//{
		//	ImGui::PushID(i);
		//	if (type == it->second->GetType())
		//	{
		//		ImGui::ButtonEx(it->second->name.c_str());
		//		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
		//		{
		//			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
		//			{
		//				ImGui::PopID();
		//				ImGui::End();
		//				active = false;
		//				return GetResource(it->second->GetUUID());
		//			}
		//		}
		//	}
		//	it++;
		//	ImGui::PopID();
		//}
		ImGui::End();
	}
	return nullptr;
}

void ModuleResourceManager::ShowAllResources(bool& active) 
{
	if (!ImGui::Begin("Resources", &active, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
	else
	{
		static int selected_type = -1;
		static std::string type_Name[] = { "Material", "Mesh", "Script", "Animation","Font"};
		static Resource::Type type_R[] = { Resource::Type::MATERIAL,
			Resource::Type::MESH, Resource::Type::SCRIPT, Resource::Type::ANIMATION, Resource::Type::FONT };
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
				ImGui::Text("Name: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.933, 0.933, 0, 1), "%s", it->second->name.c_str());
				char namedit[200];
				strcpy_s(namedit, 200, it->second->name.c_str());
				ImGui::AlignFirstTextHeightToWidgets();
				ImGui::Text("Edit Name: "); ImGui::SameLine();
				if (ImGui::InputText("##nameModel", namedit, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					it->second->name = App->fs->ConverttoChar(std::string(namedit).c_str());
					//TODO ELLIOT SaveMeta
				}
				ImGui::Text("Number of GameObjects Use this Resource: "); ImGui::SameLine();
				if (it->second->num_game_objects_use_me > 0)
				{
					ImGui::TextColored(ImVec4(0, 0.933, 0, 1), "%i", it->second->num_game_objects_use_me);
				}
				else
				{
					ImGui::TextColored(ImVec4(0.933, 0, 0, 1), "%i", it->second->num_game_objects_use_me);
				}
				ImGui::Text("UID of Resource:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 0.666, 1, 1), "%i", it->second->GetUUID());
				ImGui::Text("Directory in Assets: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 0.933, 0, 1), "%s", it->second->path_assets.c_str());
				if (it->second->GetType() == Resource::Type::SCRIPT)
				{
					ImGui::Text("Directory in Library: "); ImGui::SameLine();
					ImGui::TextColored(ImVec4(0, 0.933, 0.933, 1), "%s", ((ResourceScript*)it->second)->GetPathdll().c_str());
				}
				if (it->second->GetType() == Resource::Type::ANIMATION)
				{
					ImGui::Text("Directory in Library: "); ImGui::SameLine();
					ImGui::TextColored(ImVec4(0, 0.933, 0.933, 1), "%s", ((ResourceAnimation*)it->second)->path_library.c_str());
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
	reimported_scripts = true;
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

void ModuleResourceManager::CheckLibrary()
{

}

void ModuleResourceManager::NewLoad()
{
	std::vector<std::string> files_meta;
	App->fs->GetAllMetas(App->fs->GetMainDirectory(), files_meta);
	App->json_seria->CreateResourcesLoad(files_meta);

	//Duplicate Particle folder to library
	App->fs->CopyFolderToLibrary("ParticleSystem");

}