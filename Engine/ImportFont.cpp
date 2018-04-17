#include "ImportFont.h"
#include "SDL2_ttf/include/SDL_ttf.h"
#include "ResourceFont.h"
#include "ModuleGUI.h"

#include "ModuleFS.h"
#include "JSONSerialization.h"

ImportFont::ImportFont()
{
}


ImportFont::~ImportFont()
{
}

bool ImportFont::Import(const char * file, uint uuid, bool isAutoImport)
{


	uint uuid_mesh = 0;
	if (uuid == 0) // if direfent create a new resource with the resource deleted
	{
		uuid_mesh = App->random->Int();
	}
	else
	{
		uuid_mesh = uuid;
	}

	std::string name = std::to_string(uuid_mesh);
	name = App->fs->FixName_directory(name);//?
	name = App->fs->FixExtension(name, ".ttf");

	ResourceFont* res_font = (ResourceFont*)App->resource_manager->CreateNewResource(Resource::Type::FONT, uuid_mesh);
	res_font->InitInfo(App->fs->FixName_directory(file).c_str(), file);
	std::string Newdirectory;
	if (App->build_mode == false)
	{
		Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
		Newdirectory += "\\" + App->fs->FixName_directory(file);


		App->json_seria->SaveFont(res_font, App->fs->GetOnlyPath(file).c_str(), Newdirectory.c_str());
	}
	App->fs->DuplicateFile(file, name, DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_FONT);


	/*
	   std::ifstream  src("from.ogv", std::ios::binary);
    std::ofstream  dst("to.ogv",   std::ios::binary);

	*/
	return false;
}

Font ImportFont::Load(const char * file, int size)
{
	Font ret;
	std::string temp = file;
	temp = DIRECTORY_LIBRARY_FONTS + temp + ".ttf";

	ret.font= TTF_OpenFont(temp.c_str(), size);
	if (!ret.font) {

	}
	ret.size = size;
	return ret;
}

bool ImportFont::LoadResource(const char * file, int size, ResourceFont * resource_font)
{
	Font font = Load(file, size);
	resource_font->Init(font);
	resource_font->LoadToMemory();
	return true;
}
