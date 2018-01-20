#include "WindowProject.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "JSONSerialization.h"

Project::Project()
{
	active.push_back(Active());
	name = "Project";
}


Project::~Project()
{
	active.clear();
	App->fs->DeleteFolders(folders);
	App->fs->DeleteFiles(files);
	RELEASE_ARRAY(directory_see);
}

bool Project::Start()
{
	folder_icon = App->textures->LoadTexture("Images/UI/folder_icon.png");
	icon_png = App->textures->LoadTexture("Images/UI/icon_png.png");
	icon_jpg = App->textures->LoadTexture("Images/UI/icon_jpg.png");
	icon_fbx = App->textures->LoadTexture("Images/UI/icon_fbx.png");
	icon_obj = App->textures->LoadTexture("Images/UI/icon_obj.png");
	icon_script = App->textures->LoadTexture("Images/UI/icon_script.png");
	icon_unknown = App->textures->LoadTexture("Images/UI/icon_unknown.png");

	//directory_see = App->fs->GetMainDirectory();
	//directory_see = "Assets\\";
	directory_see = App->GetCharfromConstChar(App->fs->GetMainDirectory().c_str());
	//directory_see = "";
	//folders = App->fs->Get_AllFolders(directory_see);
	//Iterate_files(folders);
	//ReorderFiles(folders);
	//folders[0].active = true;
	sizeFiles = 50;
	timeFolders.Start();
	timeFiles.Start();
	App->fs->GetAllFolders("", directory_see, folders);
	App->fs->GetAllFiles(directory_see, files);
	return true;
}

update_status Project::Update(float dt)
{
	if (App->fs->CheckAssetsIsModify())
	{

	}
	//if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	//{
	//	//GetFolderSee();
	//}
	if (active[0].active)
	{
		//ShowProject();
		ShowProject();
	}


	return UPDATE_CONTINUE;
}

bool Project::CleanUp()
{

	return true;
}

TYPE_FILE Project::SetType(std::string name)
{
	size_t EndName = name.find_last_of(".");
	std::string temp = name.substr(EndName + 1);
	if (name.compare(temp) != 0)
	{
		// Isn't a Folder
		if (temp == "png")
		{
			return TYPE_FILE::PNG;
		}
		else if (temp == "jpg")
		{
			return TYPE_FILE::JPG;
		}
		else if (temp == "dds")
		{
			return TYPE_FILE::DDS;
		}
		else if (temp == "fbx")
		{
			return TYPE_FILE::FBX;
		}
		else if (temp == "FBX")
		{
			return TYPE_FILE::FBX;
		}
		else if (temp == "obj")
		{
			return TYPE_FILE::OBJ;
		}
		else if (temp == "cs" || temp == "cpp" || temp == "c")
		{
			return TYPE_FILE::SCRIPT;
		}
		else
		{
			return TYPE_FILE::NON;
		}
	}
	else
	{
		return TYPE_FILE::FOLDER;
	}
}

void Project::ShowProject()
{
	//DeleteFolders(folders);
	//DeleteFiles(files);
	//folders.clear();
	//files.clear();
	if (!BeginDock("Project", NULL, 0)) //TODO ELLIOT CLOSE Windows example
	{
		EndDock();
		return;
	}
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.211f, 0.211f, 0.211f, 1.00f));
	if (ImGui::BeginChild(ImGui::GetID("Project"), ImVec2(ImGui::GetWindowWidth(), 20)))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 2));
		if (ImGui::Button("Select.."))
			ImGui::OpenPopup("select");
		ImGui::PopStyleVar(2);
		ImGui::SameLine();
		if (ImGui::BeginPopup("select"))
		{
			if (ImGui::Selectable("Create Folder..."))
			{

			}
			if (ImGui::Selectable("Import file"))
			{

			}
			ImGui::EndPopup();
		}
		// Size of Items ----------
		ImGui::SameLine(ImGui::GetWindowWidth() - 160);
		ImGui::PushItemWidth(100);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 2));
		ImGui::Text("Size:"); ImGui::SameLine();
		ImGui::SliderInt("##Size1", &sizeFiles, 25, 100);
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
	}
	ImGui::EndChild();
	ImGui::PopStyleColor();

	// ---------------------------------------------------------------------------------------------------
	ImGui::Columns(2, "Project_Assets");
	ImGui::SetColumnWidth(0, SPERATIONCOLUMN);
	//Column 1 LEFT ------------------------
	ImGui::Spacing();
	// Folders ---------------------
	if (timeFolders.ReadSec() > 30 || updateFoldersNow)
	{
		timeFolders.Start();
		updateFoldersNow = false;
		App->fs->GetAllFolders("", directory_see, folders);
	}

	Folders_update(folders);

	ImGui::NextColumn();
	ImGui::Separator();
	//GetAllFiles
	//files = App->fs->GetAllFilesNew(directory_see);
	if (timeFiles.ReadSec() > 30 || updateFilesNow)
	{
		timeFiles.Start();
		updateFilesNow = false;
		App->fs->GetAllFiles(directory_see, files);
	}

	Files_Update(files);

	EndDock();
}

void Project::Folders_update(std::vector<FoldersNew>& folders)
{
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.335f, 0.337f, 0.357f, 1.00f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 3));
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 12);
	for (int i = 0; i < folders.size(); i++)
	{
		ImGui::PushID(i);
		ImGuiTreeNodeFlags node_flags;
		if (folders[i].active)
		{
			node_flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnArrow;
		}
		else
		{
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
		}
		if (folders[i].folder_child.size() == 0)
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (ImGui::TreeNodeEx(folders[i].file_name, node_flags))
		{
			if (ImGui::BeginPopupContextItem("rename context menu"))
			{
				ImGui::Text("Edit name:");
				ImGuiInputTextFlags text_flag;
				if (folders[i].haveSomething)
				{
					text_flag = ImGuiInputTextFlags_ReadOnly;
				}
				else
				{
					text_flag = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
				}
				char namedit[50];
				strcpy_s(namedit, 50, folders[i].file_name);
				if (ImGui::InputText("##edit2", namedit, 50, text_flag))
				{
					folders[i].file_name = App->fs->ConverttoChar(std::string(namedit).c_str());
					ImGui::CloseCurrentPopup();
				}
				//ImGui::InputText("##edit2", folders[i].file_name, 256);
				ImGui::Spacing();
				if (ImGui::Button("Create New Folder"))
				{
					std::string newFolder = GetDirectory();
					newFolder += "/";
					newFolder += "New Folder";
					App->fs->CreateFolder(newFolder.c_str(), true);
					ImGui::CloseCurrentPopup();
					UpdateNow();
				}
				ImGui::Spacing();
				if (strcmp(folders[i].file_name, "Assets") != 0)
				{
					if (ImGui::Button("Remove"))
					{
						if (folders[i].haveSomething)
						{
							App->fs->GetAllFilesFromFolder(folders[i].directory_name, App->resource_manager->filestoDelete);
						}
						std::experimental::filesystem::remove_all(folders[i].directory_name);
						UpdateNow();
						SetAllFolderBool(folders, false);
						directory_see = App->GetCharfromConstChar(App->fs->GetMainDirectory().c_str());
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
			if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
			{
				//LOG("%.2f - %.2f  / /  %.2f - %.2f", ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
				if (ImGui::IsMouseDoubleClicked(0) && folders[i].active == false && ImGui::IsMouseHoveringWindow())
				{
					SetAllFolderBool(folders, false);
					folders[i].active = true;
					updateFoldersNow = true;
					updateFilesNow = true;
					directory_see = App->GetCharfromConstChar(folders[i].directory_name);
				}
			}
			if (folders[i].folder_child.size() > 0)
			{
				Folders_update(folders[i].folder_child);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor();
}

void Project::Files_Update(const std::vector<FilesNew>& files)
{
	uint width = DISTANCEBUTTONS;
	for (int i = 0; i < files.size(); i++)
	{
		ImGui::PushID(i);

		//std::string namebySize = files[i].file_name;
		static char nameTemp[100];
		int lenght = strlen(files[i].file_name);
		memset(nameTemp, 0, 99);
		for (int j = 0; j < lenght; j++)
		{
			nameTemp[j] = files[i].file_name[j];
		}
		//ImVec2 size_name = ImGui::CalcTextSize(namebySize.c_str(), NULL, true);
		ImVec2 size_name = ImGui::CalcTextSize(nameTemp, NULL, true);
		if (size_name.x > sizeFiles)
		{
			int siz = strlen(nameTemp);
			for (uint ds = siz; ds > 0; ds--)
			{
				uint st = strlen(nameTemp);
				size_name = ImGui::CalcTextSize(nameTemp, NULL, true);
				if (size_name.x < sizeFiles)
				{
					nameTemp[st - 3] = '.';
					nameTemp[st - 2] = '.';
					nameTemp[st - 1] = '.';
					nameTemp[st] = '\0';
					break;
				}
				else
					nameTemp[st - 1] = 0;
			}
		}

		switch (files[i].file_type)
		{
		case TYPE_FILE::NON:
		{
			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)icon_unknown, nameTemp, ImVec2(sizeFiles, sizeFiles), ImVec2(-1, 1), ImVec2(0, 0));

			break;
		}
		case TYPE_FILE::FOLDER:
		{
			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)folder_icon, nameTemp, ImVec2(sizeFiles, sizeFiles), ImVec2(-1, 1), ImVec2(0, 0));
			if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
			{
				//LOG("%.2f - %.2f  / /  %.2f - %.2f", ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
				if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
				{
					//ChangefileViwer(files[i].parentFolder->folder_child, files[i].file_name);
					directory_see = App->GetCharfromConstChar(files[i].directory_name);
					updateFilesNow = true;
					updateFoldersNow = true;
				}
			}
			break;
		}
		case TYPE_FILE::FBX:
		{
			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)icon_fbx, nameTemp, ImVec2(sizeFiles, sizeFiles), ImVec2(-1, 1), ImVec2(0, 0));
			if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
			{
				if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
				{
					std::string directory_prebaf = GetDirectory();
					directory_prebaf += "/";
					directory_prebaf += files[i].file_name;
					directory_prebaf += ".meta.json";
					App->Json_seria->LoadPrefab(directory_prebaf.c_str());
					directory_prebaf.clear();
				}
			}
			break;
		}
		case TYPE_FILE::OBJ:
		{
			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)icon_obj, nameTemp, ImVec2(sizeFiles, sizeFiles), ImVec2(-1, 1), ImVec2(0, 0));
			if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
			{
				if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
				{
					std::string directory_prebaf = GetDirectory();
					directory_prebaf += "/";
					directory_prebaf += files[i].file_name;
					directory_prebaf += ".meta.json";
					App->Json_seria->LoadPrefab(directory_prebaf.c_str());
					directory_prebaf.clear();
				}
			}
			break;
		}
		case TYPE_FILE::SCRIPT:
		{
			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)icon_script, nameTemp, ImVec2(sizeFiles, sizeFiles), ImVec2(-1, 1), ImVec2(0, 0));
			break;
		}
		case TYPE_FILE::PNG:
		{
			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)icon_png, nameTemp, ImVec2(sizeFiles, sizeFiles), ImVec2(-1, 1), ImVec2(0, 0));
			break;
		}
		case TYPE_FILE::JPG:
		{
			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)icon_jpg, nameTemp, ImVec2(sizeFiles, sizeFiles), ImVec2(-1, 1), ImVec2(0, 0));
			break;
		}
		}
		width += sizeFiles + MARGEBUTTON + DISTANCEBUTTONS; // Size of imatge + marge button + between buttons
		int temp = ImGui::GetWindowWidth();
		if ((width + DISTANCEBUTTONS + SPERATIONCOLUMN) > temp)
		{
			width = 0;
		}
		else
		{
			if (i + 1 < files.size())
				ImGui::SameLine();
		}
		if (ImGui::BeginPopupContextItem("rename context menu"))
		{
			ImGui::Text("Edit name:");
			ImGui::InputText("##edit2", files[i].file_name, 50, ImGuiInputTextFlags_ReadOnly);
			//ImGui::InputText("##edit2", folders[i].file_name, 256);
			ImGui::Spacing();
			if (ImGui::Button("Create New Folder"))
			{
				std::string newFolder = GetDirectory();
				newFolder += "/";
				newFolder += "New Folder";
				App->fs->CreateFolder(newFolder.c_str(), true);
				ImGui::CloseCurrentPopup();
				UpdateNow();
			}
			ImGui::Spacing();

			if (ImGui::Button("Remove"))
			{
				if (folders[i].haveSomething)
				{
					App->fs->GetUUIDFromFile(files[i].directory_name, App->resource_manager->filestoDelete);
				}
				std::experimental::filesystem::remove_all(files[i].directory_name);
				UpdateNow();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
}

const char* Project::GetDirectory() const
{
	return directory_see;
}

void Project::SetDirectory(const char* newDirectory)
{
	directory_see = App->GetCharfromConstChar(newDirectory);
	updateFoldersNow = true;
	updateFilesNow = true;
}

void Project::UpdateNow()
{
	updateFoldersNow = true;
	updateFilesNow = true;
}

void Project::SetAllFolderBool(std::vector<FoldersNew>& folders, bool setBoolean)
{
	for (int i = 0; i < folders.size(); i++)
	{
		if (folders[i].folder_child.size() > 0)
		{
			SetAllFolderBool(folders[i].folder_child, setBoolean);
		}
		folders[i].active = setBoolean;
	}
}