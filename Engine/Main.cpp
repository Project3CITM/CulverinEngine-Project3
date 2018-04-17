#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "Brofiler/Brofiler.h"
#pragma comment( lib, "Brofiler/ProfilerCore32.lib" )

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment ( lib, "SDL2_ttf/lib/x86/SDL2_ttf.lib")
#include "GL3W/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "SDL2_ttf/include/SDL_ttf.h"

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char ** argv)
{
	LOG("Starting game '%s'...", TITLE);

	int main_return = EXIT_FAILURE;

	if (argc > 1)
	{
		std::string make_build;
		std::string destination;
		std::string build_name;
		std::string initial_scene;
		bool game_mode;
		for (int i = 0; i < argc; i++)
		{
			// Create a build in /Build folder
			std::string argv_ = argv[i];
			size_t EndName = argv_.find("--make_build:");
			if (EndName != std::string::npos)
			{
				make_build = argv_.substr(EndName + 13);
				// Make Build
				if (strcmp(make_build.c_str(), "/Assets") == 0)
				{
					App = new Application(true);
					App->InitBuild();
					LOG("Application Created");
				}
				continue;
			}
			EndName = argv_.find("--destination:");
			if (EndName != std::string::npos)
			{
				destination = argv_.substr(EndName + 14);
				LOG("Destination: %s", destination.c_str());
				continue;
			}
			EndName = argv_.find("--build_name:");
			if (EndName != std::string::npos)
			{
				build_name = argv_.substr(EndName + 13);
				LOG("build_name: %s", build_name.c_str());
				continue;
			}
			EndName = argv_.find("--initial_scene:");
			if (EndName != std::string::npos)
			{
				initial_scene = argv_.substr(EndName + 16);
				LOG("initial_scene: %s", initial_scene.c_str());
				continue;
			}
			EndName = argv_.find("--game_mode:");
			if (EndName != std::string::npos)
			{
				std::string game_mode_string = argv_.substr(EndName + 12);
				if (strcmp(game_mode_string.c_str(), "true") == 0)
				{
					game_mode = true;
					LOG("game_mode = true ----");
				}
				else if (strcmp(game_mode_string.c_str(), "false") == 0)
				{
					game_mode = false;
					LOG("game_mode = false ----");
				}
				else
				{
					LOG("[error] In --game_mode: not select if true or false!");
					if (App != nullptr)
					{
						App->SaveLogs();
					}
					return EXIT_FAILURE;
				}
				continue;
			}
		}
		// Now Make Build :D
		LOG("MakeBuild");
		App->SaveLogs();
		if (App != nullptr)
		{
			App->MakeBuild(build_name, initial_scene, destination, game_mode);
		}
		LOG("Build Created -------------------");
		std::string savelogs_path = destination + "/Logs.txt";
		App->SaveLogs(savelogs_path.c_str());
		App->SaveLogs();
		delete App;
		return EXIT_SUCCESS;
	}
	else
	{
		main_states state = MAIN_CREATION;

		while (state != MAIN_EXIT)
		{
			BROFILER_FRAME("MAIN");
			switch (state)
			{
			case MAIN_CREATION:

				LOG("-------------- Application Creation --------------");
				App = new Application();
				state = MAIN_START;
				break;

			case MAIN_START:

				LOG("-------------- Application Init --------------");
				TTF_Init();
				if (App->Init() == false)
				{
					LOG("Application Init exits with ERROR");
					state = MAIN_EXIT;

				}
				else
				{
					state = MAIN_UPDATE;
					LOG("-------------- Application Update --------------");
				}

				break;

			case MAIN_UPDATE:
			{
				int update_return = App->Update();

				if (update_return == UPDATE_ERROR)
				{
					LOG("Application Update exits with ERROR");
					state = MAIN_EXIT;
				}

				if (update_return == UPDATE_STOP)
					state = MAIN_FINISH;
			}
			break;

			case MAIN_FINISH:

				LOG("-------------- Application CleanUp --------------");
				if (App->CleanUp() == false)
				{
					LOG("Application CleanUp exits with ERROR");
				}
				else
					main_return = EXIT_SUCCESS;

				state = MAIN_EXIT;

				break;

			}
		}

		LOG("Exiting game '%s'...\n", TITLE);

		delete App;
		TTF_Quit();
	}

	return main_return;
}