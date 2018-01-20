#include "WindowHardware.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "SDL\include\SDL.h"

Hardware::Hardware() : WindowManager()
{
	active.push_back(Active());
	name = "Hardware";
}

Hardware::~Hardware()
{
	active.clear();
}

bool Hardware::Start()
{
	return true;
}

update_status Hardware::Update(float dt)
{
	if (active[0].active)
	{
		if (!ImGui::Begin("Hardware", &App->gui->winManager[HARDWARE]->active[0].active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ShowBorders)) //TODO ELLIOT CLOSE Windows example
		{
			ImGui::End();
			return UPDATE_CONTINUE;
		}
		//ImGuiStyle& style = ImGui::GetStyle();
		//style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		//ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 50.0f, 1.0f, "%.2f");
		ImGui::Text("SDL Version: ");
		SDL_version compiled;
		SDL_VERSION(&compiled);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);
		//ImGui::SameLine(); ShowHelpMarker("The TextDisabled color is stored in ImGuiStyle.");
		ImGui::Separator();
		ImGui::Text("CPUs: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", SDL_GetCPUCount());
		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d Mb (Cache: %d Kb)", SDL_GetSystemRAM(), SDL_GetCPUCacheLineSize());
		ImGui::Separator();
		ImGui::Text("Vendor: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VENDOR));
		ImGui::Text("GPU: "); ImGui::SameLine(); 
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_RENDERER));
		ImGui::Text("Brand: "); ImGui::SameLine(); 
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VENDOR));
		ImGui::Text("Version: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));

		//OpenGL
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("OpenGL version supported:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));
		ImGui::Text("GLSL:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::End();
	}


	return UPDATE_CONTINUE;
}

bool Hardware::CleanUp()
{

	return true;
}
