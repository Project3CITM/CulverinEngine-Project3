#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "wwished.h"
#include "ModuleFS.h"

#include <locale>
#include <codecvt>

ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	have_config = true;
	volume = 10;
	name = "Audio";
}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init(JSON_Object* node)
{
	perf_timer.Start();
	bool ret = true;
	
	LOG("Loading Wwished library");

	//Convert the char path to wstring
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	banks_full_path = App->fs->GetFullPath("AudioBanks");
	std::wstring base_path = converter.from_bytes(banks_full_path.c_str());
	init_bank_not_loaded = !Wwished::InitWwished(base_path.c_str(), "English(US)");

	if (init_bank_not_loaded)
		LOG("CAUTION: Init sound bank not loaded");

	volume = json_object_get_number(node, "Volume");
	mute = json_object_get_boolean(node, "Mute");

	Awake_t = perf_timer.ReadMs();
	return ret;
}

//bool ModuleWindow::Start()
//{
//	perf_timer.Start();
//
//	Start_t = perf_timer.ReadMs();
//	return true;
//}
//
//update_status ModuleWindow::PreUpdate(float dt)
//{
//	perf_timer.Start();
//
//	preUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleWindow::Update(float dt)
//{
//	perf_timer.Start();
//
//	Update_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleWindow::postUpdate(float dt)
//{
//	perf_timer.Start();
//
//	postUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

update_status ModuleAudio::PostUpdate(float dt)
{
	Wwished::ProcessAudio();

	return UPDATE_CONTINUE;
}

bool ModuleAudio::SaveConfig(JSON_Object * node)
{
	//Save audio config info --------------------------------
	json_object_set_number(node, "Volume", volume);
	json_object_set_boolean(node, "Mute", mute);
	// ------------------------------------------------------
	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	LOG("Unloading Wwished library");
	return Wwished::CloseWwished();
}

void ModuleAudio::DrawOnEditor()
{
	if (init_bank_not_loaded)
	{
		//ImGui::TextColored(ImVec4(255, 0, 0, 255), "Init audiobank is not loaded!");
		//return;
	}

	if (loaded_banks.empty())
	{
		ImGui::Text("No audiobanks loaded");
	}
	else
	{
		int index = 0;
		for (std::vector<std::string>::iterator it = loaded_banks.begin(); it != loaded_banks.end();)
		{

			ImGui::Text((*it).c_str());
			ImGui::SameLine();
			ImGui::PushID(index);
			
			if (ImGui::Button("Unload"))
			{
				it = loaded_banks.erase(it);
			}
			else 
			{
				it++;
				index++;
			}
			ImGui::PopID();
		}


	}
	ImGui::Separator();
	if (ImGui::Button("Load Audiobank"))
	{
		load_bank_window_opened = true;
	}

	if (load_bank_window_opened)
	{
		ImGui::OpenPopup("Load Audio Bank");
		ImGui::SetNextWindowSize(ImVec2(200, 500));
		ImGui::SetNextWindowPosCenter();

		if (ImGui::BeginPopupModal("Load Audio Bank", nullptr))
		{
			ImGui::BeginChildFrame(0, ImVec2(150, 420));

			
			for (std::experimental::filesystem::recursive_directory_iterator::value_type it : std::experimental::filesystem::recursive_directory_iterator(banks_full_path.c_str()))
			{
				std::string bank_name = std::experimental::filesystem::path(it.path().string().c_str()).stem().string().c_str();
				if (ImGui::Selectable(bank_name.c_str()))
				{
					loaded_banks.push_back(bank_name);
					load_bank_window_opened = false;
				}
			}

			ImGui::EndChildFrame();

			if (ImGui::Button("Cancel"))
			{
				load_bank_window_opened = false;
			}
		}


		ImGui::EndPopup();
	}

}



update_status ModuleAudio::UpdateConfig(float dt)
{
	if (ImGui::SliderInt("Volume", &volume, 0, 100))
	{
		if (mute == false)
		{
			//change volume
		}
	}

	if (ImGui::Checkbox("Mute", &mute))
	{
		//mute
	}
	return UPDATE_CONTINUE;
}
