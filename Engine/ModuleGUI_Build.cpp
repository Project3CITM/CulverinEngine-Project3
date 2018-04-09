#include "ModuleGUI.h"
#include "Application.h"
#include "ModuleFS.h"
#include "JSONSerialization.h"
#include "WindowProject.h"

#include <shlobj.h>

void ModuleGUI::ShowWindowBuild(bool* active)
{
	if (!ImGui::Begin("Build Settings", &window_about_us, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
	{
		active = false;
		ImGui::End();
		return;
	}

	ImGui::Text("Set Initial Scene:");
	static std::string initial_scene;
	//-----------------------------------------
	static uint icon_scene = ((Project*)App->gui->win_manager[PROJECT])->GetIconScene();
	if (load_scenes)
	{
		App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), scenes_build, "scene.json", false);

		for (int i = 0; i < scenes_build.size(); i++)
		{
			scenes_only_name.push_back(App->fs->GetOnlyName(scenes_build[i]));
		}
		load_scenes = false;
	}
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.211f, 0.211f, 0.211f, 1.00f));
	int height_child = 180; // min height permision
	int lines = (scenes_build.size() / 5) - 2;
	if (lines > 0 && scenes_build.size() > 10)
	{
		height_child += (lines * 70);
	}
	if (ImGui::BeginChild(ImGui::GetID("Build Settings"), ImVec2(525, height_child), true, ImGuiWindowFlags_NoScrollbar))
	{
		static int size_files = 90;
		int num_icons_pLine = 5;
		int icon_line = 0;
		for (int i = 0; i < scenes_build.size(); i++)
		{
			//std::string namebySize = files[i].file_name;
			static char nameTemp[100];
			int lenght = strlen(scenes_only_name[i].c_str());
			memset(nameTemp, 0, 99);
			for (int j = 0; j < lenght; j++)
			{
				std::string temp = scenes_only_name[i];
				nameTemp[j] = temp[j];
			}
			//ImVec2 size_name = ImGui::CalcTextSize(namebySize.c_str(), NULL, true);
			ImVec2 size_name = ImGui::CalcTextSize(nameTemp, NULL, true);
			if (size_name.x > size_files)
			{
				int siz = strlen(nameTemp);
				for (uint ds = siz; ds > 0; ds--)
				{
					uint st = strlen(nameTemp);
					size_name = ImGui::CalcTextSize(nameTemp, NULL, true);
					if (size_name.x < size_files)
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

			ImGui::ImageButtonWithTextDOWN_NoReajust((ImTextureID*)icon_scene, nameTemp, ImVec2(size_files - 20, size_files - 20), ImVec2(-1, 1), ImVec2(0, 0));
			if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
			{
				if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
				{
					initial_scene = scenes_only_name[i];
					initial_scene += ".scene.json";
				}
			}
			if (icon_line < num_icons_pLine && i + 1 < scenes_build.size())
			{
				ImGui::SameLine();
				icon_line++;
			}
			else
			{
				icon_line = 0;
			}
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleColor();
	//--------------------------------------------

	// Settings
	ImGui::Text("Initial Scene:"); ImGui::SameLine();
	if (initial_scene.size() > 0)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.00f, 0.0f, 1.00f), "%s", initial_scene.c_str());
	}
	else
	{
		ImGui::TextColored(ImVec4(0.0f, 1.00f, 0.0f, 1.00f), "Not Selected");
	}
	ImGui::Separator();
	ImGui::TextWrapped("Application Settings");
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text("Company Name:"); ImGui::SameLine();
	static std::string company_name;
	char namedit[150];
	strcpy_s(namedit, 150, company_name.c_str());
	if (ImGui::InputText("##nameModel", namedit, 150, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		company_name = App->fs->ConverttoChar(std::string(namedit).c_str());
		//delete[] namedit;
	}
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text("Game Name:"); ImGui::SameLine();
	static std::string game_name;
	char namedit_game[150];
	strcpy_s(namedit_game, 150, game_name.c_str());
	if (ImGui::InputText("##game_name", namedit_game, 150, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		game_name = App->fs->ConverttoChar(std::string(namedit_game).c_str());
		//delete[] namedit_game;
	}
	ImGui::Separator();
	static bool full_screen = false;
	static bool native_resolution = false;
	static bool run_in_background = false;
	static bool resizable_window = true;
	static bool borderless = false;
	static bool full_desktop = false;
	static bool game_mode = true;
	static bool use_release = false;

	ImGui::Bullet(); ImGui::SameLine();
	ImGui::TextWrapped("Resolution");
	ImGui::Indent();
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Default Is Full Screen"); ImGui::SameLine();
	ImGui::Checkbox("##fullscreen", &full_screen);
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Default Is Native Resolution"); ImGui::SameLine();
	ImGui::Checkbox("##native_resolution", &native_resolution);
	ImGui::SameLine(); App->ShowHelpMarker("Doesn't work at the moment");
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Run In Background"); ImGui::SameLine();
	ImGui::Checkbox("##run_in_background", &run_in_background);
	ImGui::SameLine(); App->ShowHelpMarker("Doesn't work at the moment");
	ImGui::Unindent();

	ImGui::Separator();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::TextWrapped("Presentation");
	ImGui::Indent();
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Resizable Window"); ImGui::SameLine();
	ImGui::Checkbox("##resizable_window", &resizable_window);
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Borderless"); ImGui::SameLine();
	ImGui::Checkbox("##borderless", &borderless);
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Full Desktop"); ImGui::SameLine();
	ImGui::Checkbox("##full_desktop", &full_desktop);
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Supported Aspect Ratios");
	ImGui::SameLine(); App->ShowHelpMarker("Doesn't work at the moment");
	ImGui::Unindent();

	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Game Mode"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.0f, 1.00f, 0.0f, 1.00f), "True");
	ImGui::SameLine(); App->ShowHelpMarker("Only Use with True");
	//ImGui::Checkbox("##game_mode", &game_mode);
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Bullet(); ImGui::SameLine();
	ImGui::Text("Executable Release"); ImGui::SameLine();
	ImGui::Checkbox("##use_release", &use_release);
	ImGui::SameLine(); App->ShowHelpMarker("True == use App-release.exe \nFalse == use App-Debug.exe");

	ImGui::Separator();
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text(""); ImGui::SameLine(490);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.89f, 0.0f, 0.0f, 1.0f));
	if (ImGui::Button("Build", ImVec2(50,25)))
	{
		// First Create a new folder in desktop
		std::string desktop;
		TCHAR desktopData[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL,
			CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE,
			NULL,
			SHGFP_TYPE_CURRENT,
			desktopData)))
		{
			desktop = desktopData;
		}
		//desktop = "C:/Users/Administrador/Desktop";
		App->fs->NormalitzatePath(desktop);
		desktop += "/" + game_name;

		//if (App->fs->CheckIsFileExist(desktop))
		//{
		//	LOG("[error] There is a file exists with this name!");
		//	ImGui::End();
		//	return;
		//}
		desktop = App->fs->CreateFolder(desktop.c_str(), true);

		// Now Create a folder Assets and copy-paste all metas/cs
		std::string desktop_assets = desktop + "/Assets";
		App->fs->CreateFolder(desktop_assets.c_str());
		std::vector<std::string> files_meta;
		App->fs->GetAllMetas(App->fs->GetMainDirectory(), files_meta);
		for (int i = 0; i < files_meta.size(); i++)
		{
			std::string desktop_assets_temp = desktop_assets + "/";
			desktop_assets_temp += App->fs->FixName_directory(files_meta[i]);
			App->fs->CopyPasteFile(files_meta[i].c_str(), desktop_assets_temp.c_str());
		}
		files_meta.clear();
		// Now copy-paste all files .cs
		std::vector<std::string> files_cs;
		App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), files_cs, "cs");
		for (int i = 0; i < files_cs.size(); i++)
		{
			std::string desktop_assets_temp = desktop_assets + "/";
			desktop_assets_temp += App->fs->FixName_directory(files_cs[i]);
			App->fs->CopyPasteFile(files_cs[i].c_str(), desktop_assets_temp.c_str());
		}
		files_cs.clear();
		// Now copy all scenes
		for (int i = 0; i < scenes_build.size(); i++)
		{
			std::string desktop_assets_temp = desktop_assets + "/";
			desktop_assets_temp += scenes_only_name[i];
			desktop_assets_temp += ".scene.json";
			App->fs->CopyPasteFile(scenes_build[i].c_str(), desktop_assets_temp.c_str());
		}
		// Copy Folder Maps -------------------------------------------
		std::string path_copy = App->fs->GetMainDirectory() + "/Maps";
		std::string path_paste = desktop_assets + "/Maps";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());
		// Copy Folder ParticleSystem -------------------------------------------
		path_copy = App->fs->GetMainDirectory() + "/ParticleSystem";
		path_paste = desktop_assets + "/ParticleSystem";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());
		// Copy Folder Shaders -------------------------------------------
		path_copy = App->fs->GetMainDirectory() + "/Shaders";
		path_paste = desktop_assets + "/Shaders";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

		// After, copy folders (Library, AudioBanks, Images, Mono, ScriptManager, Fonts)
		// Library -----------------------------------------------------
		path_copy = App->fs->GetGameDirectory() + "/Library";
		path_paste = desktop + "/Library";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());
		// AudioBanks -----------------------------------------------------
		path_copy = App->fs->GetGameDirectory() + "/AudioBanks";
		path_paste = desktop + "/AudioBanks";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());
		// Images -----------------------------------------------------
		path_copy = App->fs->GetGameDirectory() + "/Images";
		path_paste = desktop + "/Images";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());
		// Mono -----------------------------------------------------
		path_copy = App->fs->GetGameDirectory() + "/Mono";
		path_paste = desktop + "/Mono";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());
		// ScriptManager -----------------------------------------------------
		path_copy = App->fs->GetGameDirectory() + "/ScriptManager";
		path_paste = desktop + "/ScriptManager";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());
		// Fonts -----------------------------------------------------
		path_copy = App->fs->GetGameDirectory() + "/Fonts";
		path_paste = desktop + "/Fonts";
		App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

		// Then all files (no Folders) in Game.
		std::vector<std::string> files_game;
		App->fs->GetOnlyFilesFromFolder(App->fs->GetGameDirectory(), files_game);
		for (int i = 0; i < files_game.size(); i++)
		{
			std::string files_game_new = desktop + "/";
			files_game_new += App->fs->FixName_directory(files_game[i]);
			App->fs->CopyPasteFile(files_game[i].c_str(), files_game_new.c_str());
		}

		// Then Edit config file with new settings
		std::string new_config = desktop;
		new_config += "/config.json";
		App->json_seria->SaveConfig(new_config, game_name, initial_scene, game_mode, full_screen, resizable_window, borderless, full_desktop);


		// Finnaly .exe from Release
		std::string folder = App->fs->GetGameDirectory();
		folder += "../../Release/CulverinEngine.exe";
		std::string executable = desktop;
		executable += "/" + game_name + ".exe";
		App->fs->CopyPasteFile(folder.c_str(), executable.c_str());

		// ^^
	}
	//ImGui::SameLine();
	//if (ImGui::ButtonEx("Build And Run", ImVec2(0,0), ImGuiButtonFlags_Disabled))
	//{
	//	//
	//	//
	//	//
	//}
	//ImGui::SameLine(); App->ShowHelpMarker("Doesn't work at the moment");
	ImGui::PopStyleColor();
	ImGui::End();
}



/*

How get Desktop path


char term[260];
TCHAR appData[MAX_PATH];
if (SUCCEEDED(SHGetFolderPath(NULL,
CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE,
NULL,
SHGFP_TYPE_CURRENT,
appData)))
{
std::string temp = appData;
}
//std::string tesst = SHGetDesktopFolder(NULL);
//SHGetKnownFolderPath(NULL, )

*/