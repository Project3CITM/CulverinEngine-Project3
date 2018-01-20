#include "ModuleFS.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "JSONSerialization.h"
#include "TextEditor.h"

ModuleFS::ModuleFS(bool start_enabled) : Module(start_enabled)
{
	Start_enabled = true;
	preUpdate_enabled = true;

	name = "File System";
}


ModuleFS::~ModuleFS()
{
	DeleteAllFilesAssets(allfilesAsstes);
	allfilesAsstes.clear(); 
}

bool ModuleFS::Init(JSON_Object * node)
{
	// Will contain exe path
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		// When passing NULL to GetModuleHandle, it returns handle of exe itself
		GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
	}
	directory_Game = ownPth;

	//In release this code: -----------------------------------------
	//size_t EndName = directory_Game.find_last_of("\\");
	//directory_Game = directory_Game.substr(0, EndName);
	//directory_Assets = directory_Game;
	//directory_Game += "\\Assets"; // "\\Game\\Assets"
	//LOG("%s", directory_Assets);
	//LOG("%s", directory_Game);

	//Not release this: ---------------------------------------------
	size_t EndName = directory_Game.find_last_of("\\");
	directory_Game = directory_Game.substr(0, EndName);
	EndName = directory_Game.find_last_of("\\");
	directory_Game = directory_Game.substr(0, EndName);
	directory_Assets = directory_Game + "\\Game";
	directory_Game += "\\Game\\Assets"; // "\\Game\\Assets"

										// Check if Main Folders exist --------------------
	CreateFolder("Library");
	CreateFolder("Library\\Meshes");
	CreateFolder("Library\\Materials");
	CreateFolder("Library\\Scripts");
	CreateFolder("Assets");
	return true;
}

bool ModuleFS::Start()
{
	perf_timer.Start();


	// Get All Files From Assets
	GetAllFilesAssets(directory_Game, allfilesAsstes);
	checkAssets.Start();

	Start_t = perf_timer.ReadMs();
	return true;
}

update_status ModuleFS::PreUpdate(float dt)
{
	perf_timer.Start();

	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		//App->resource_manager->resourcesToReimport.push_back(App->Json_seria->GetUUIDPrefab(allfilesAsstes[0].directory_name, 0));
		//char* buffer = nullptr;

		//// Loading File
		//uint size = LoadFile(files[1].c_str(), &buffer);
		//int x = 0;

		//CopyFileToAssets("C:\\Users\\Administrador\\Documents\\GitHub\\3D-Engine\\Engine\\Game\\Assets\\TEST_Time.png", "C:\\Users\\Administrador\\Documents\\GitHub\\3D-Engine\\Engine\\Game\\Library\\TEST_Time.png");
		//
		//uint temp = App->random->Int();
		//LOG("%i", temp);
	}

	if (checkAssets.ReadSec() > 20)
	{
		checkAssets.Start();
		//DeleteAllFilesAssets(allfilesAsstes);
		//GetAllFilesAssets(directory_Game, allfilesAsstes);

		// Need something...
		//AnyfileModificated(allfilesAsstes); 
	}

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

void ModuleFS::CopyFileToAssets(const char* fileNameFrom, const char* fileNameTo)
{
	//assert(fileExists(fileNameFrom));
	namespace fs = std::experimental::filesystem;
	std::string exits;
	if (fileNameTo == "")
	{
		exits = GetFullPath("Assets/");
		exits += FixName_directory(fileNameFrom);
	}
	else
	{
		exits = fileNameTo;
		exits += "/" + FixName_directory(fileNameFrom);
	}

	if (fs::exists(exits) == false)
	{
		fs::copy(fileNameFrom, exits);
	}
	exits.clear();
	// Copy Folders
	//std::filesystem::copy("/dir1", "/dir3", std::filesystem::copy_options::recursive);
}

std::string ModuleFS::CopyFileToAssetsS(const char* fileNameFrom, const char* fileNameTo)
{
	//assert(fileExists(fileNameFrom));
	namespace fs = std::experimental::filesystem;
	std::string exits;
	if (fileNameTo == "")
	{
		exits = GetFullPath("Assets/");
		exits += FixName_directory(fileNameFrom);
	}
	else
	{
		exits = fileNameTo;
		exits += "/" + FixName_directory(fileNameFrom);
	}

	if (fs::exists(exits) == false)
	{
		fs::copy(fileNameFrom, exits);
	}
	return exits;
}

bool ModuleFS::CheckAssetsIsModify()
{
	return true;
}

void ModuleFS::GetAllFolders(std::experimental::filesystem::path path, std::string folderActive, std::vector<FoldersNew>& folders)
{
	namespace stdfs = std::experimental::filesystem;

	DeleteFolders(folders);

	const stdfs::directory_iterator end{};

	if (path == "")
	{
		path = directory_Assets;
	}

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		if (stdfs::is_directory(*iter))
		{
			const char* isdirectA = ConverttoConstChar(iter->path().string());
			const char* isFolderAssets = ConverttoConstChar(FixName_directory(isdirectA));
			if (strcmp(isFolderAssets, "Assets") == 0)
			{
				FoldersNew folder_temp;

				folder_temp.directory_name = ConverttoConstChar(iter->path().string());
				folder_temp.file_name = ConverttoChar(FixName_directory(iter->path().string()));
				if (folderActive == folder_temp.directory_name)
				{
					folder_temp.active = true;
				}
				folder_temp.haveSomething = GetAllFoldersChild(iter->path().string(), folderActive, folder_temp.folder_child);
				folders.push_back(folder_temp);
			}
			RELEASE_ARRAY(isFolderAssets);
			RELEASE_ARRAY(isdirectA);
		}
	}
}

bool ModuleFS::GetAllFoldersChild(std::experimental::filesystem::path path, std::string folderActive, std::vector<FoldersNew>& folders)
{
	namespace stdfs = std::experimental::filesystem;

	const stdfs::directory_iterator end{};

	if (path == "")
	{
		path = directory_Game;
	}

	bool have_something = false;
	if (stdfs::is_empty(path) == false)
	{
		have_something = true;
	}
	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		if (stdfs::is_directory(*iter))
		{
			FoldersNew folder_temp;
			folder_temp.directory_name = ConverttoConstChar(iter->path().string());
			folder_temp.file_name = ConverttoChar(FixName_directory(iter->path().string()));
			if (folderActive == folder_temp.directory_name)
			{
				folder_temp.active = true;
			}
			folder_temp.haveSomething = GetAllFoldersChild(iter->path().string(), folderActive, folder_temp.folder_child);
			folders.push_back(folder_temp);
		}
	}
	return have_something;
}

void ModuleFS::GetAllFiles(std::experimental::filesystem::path path, std::vector<FilesNew>& files)
{
	namespace stdfs = std::experimental::filesystem;

	DeleteFiles(files);

	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		std::string extension = GetExtension(iter->path().string());
		if (strcmp(extension.c_str(), "json") != 0)
		{
			FilesNew files_temp;
			files_temp.directory_name = ConverttoConstChar(iter->path().string());
			if (stdfs::is_directory(*iter))
			{
				files_temp.directory_name_next = ConverttoConstChar(iter->path().string() + "\\");
			}
			else
				files_temp.directory_name_next = nullptr;

			files_temp.file_name = ConverttoChar(FixName_directory(iter->path().string()));
			files_temp.file_type = ((Project*)App->gui->winManager[PROJECT])->SetType(files_temp.file_name);
			files.push_back(files_temp);
		}
		extension.clear();
	}
}

void ModuleFS::GetAllFilesAssets(std::experimental::filesystem::path path, std::vector<AllFiles>& files)
{
	namespace stdfs = std::experimental::filesystem;

	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		std::string extension = GetExtension(iter->path().string());
		for (std::string::iterator it = extension.begin(); it != extension.end(); it++)
		{
			*it = tolower(*it);
		}
		if (IsPermitiveExtension(extension.c_str()))
		{
			AllFiles files_temp;
			stdfs::file_time_type temp = stdfs::last_write_time(iter->path());
			std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
			files_temp.ftime = cftime;
			files_temp.directory_name = ConverttoConstChar(iter->path().string());
			files_temp.file_name = ConverttoChar(FixName_directory(iter->path().string()));
			files.push_back(files_temp);
		}
		if (stdfs::is_directory(*iter))
		{
			GetAllFilesAssets(iter->path().string(), files);
		}
	}
}

void ModuleFS::GetAllFilesFromFolder(std::experimental::filesystem::path path, std::list<const char*>& files)
{
	namespace stdfs = std::experimental::filesystem;

	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		std::string extension = GetExtension(iter->path().string());
		for (std::string::iterator it = extension.begin(); it != extension.end(); it++)
		{
			*it = tolower(*it);
		}
		if (IsPermitiveExtension(extension.c_str()))
		{
			if (App->resource_manager->CheckFileType(extension.c_str()) == Resource::Type::MESH)
			{
				files.push_back(ConverttoConstChar(iter->path().string()));
			}
			else if (App->resource_manager->CheckFileType(extension.c_str()) == Resource::Type::MATERIAL)
			{
				files.push_front(ConverttoConstChar(iter->path().string()));
			}
		}
		else
		{
			std::string isFolder = ConverttoConstChar(iter->path().string());
			if (App->resource_manager->CheckFileType(isFolder.c_str()) == Resource::Type::FOLDER)
			{
				files.push_back(ConverttoConstChar(iter->path().string()));
			}
			isFolder.clear();
		}
		extension.clear();
	}
}

// This return a vector with uuid the Resource to need ReImport
void ModuleFS::GetAllFilesFromFolder(std::experimental::filesystem::path path, std::vector<uint>& files)
{
	namespace stdfs = std::experimental::filesystem;

	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		std::string extension = GetExtension(iter->path().string());
		for (std::string::iterator it = extension.begin(); it != extension.end(); it++)
		{
			*it = tolower(*it);
		}
		if (IsPermitiveExtension(extension.c_str()))
		{
			std::string directory = ConverttoConstChar(iter->path().string());
			// LOG("MODIFICATED");
			switch (App->resource_manager->CheckFileType(extension.c_str()))
			{
			case Resource::Type::MESH:
			{
				bool finish = false; int id = 0;
				while (finish == false)
				{
					ReImport temp = App->Json_seria->GetUUIDPrefab(directory.c_str(), id++);
					if (temp.uuid != 0)
					{
						App->resource_manager->filestoDelete.push_back(temp.uuid);
					}
					else
					{
						finish = true;
					}
				}
				break;
			}
			case Resource::Type::MATERIAL:
			{
				ReImport temp = App->Json_seria->GetUUIDMaterial(directory.c_str());
				if (temp.uuid != 0)
				{
					App->resource_manager->filestoDelete.push_back(temp.uuid);
				}
				break;
			}
			}
			directory.clear();
		}
		//else
		//{
		//	std::string isFolder = ConverttoConstChar(iter->path().string());
		//	if (App->resource_manager->CheckFileType(isFolder.c_str()) == Resource::Type::FOLDER)
		//	{
		//		files.push_back(ConverttoConstChar(iter->path().string()));
		//	}
		//}
		extension.clear();
	}
}

void ModuleFS::GetUUIDFromFile(std::string path, std::vector<uint>& files)
{
	namespace stdfs = std::experimental::filesystem;
	std::string extension = GetExtension(path);
	for (std::string::iterator it = extension.begin(); it != extension.end(); it++)
	{
		*it = tolower(*it);
	}
	if (IsPermitiveExtension(extension.c_str()))
	{
		// LOG("MODIFICATED");
		switch (App->resource_manager->CheckFileType(extension.c_str()))
		{
		case Resource::Type::MESH:
		{
			bool finish = false; int id = 0;
			while (finish == false)
			{
				ReImport temp = App->Json_seria->GetUUIDPrefab(path.c_str(), id++);
				if (temp.uuid != 0)
				{
					App->resource_manager->filestoDelete.push_back(temp.uuid);
					//RELEASE_ARRAY(temp.directoryObj);
					//RELEASE_ARRAY(temp.nameMesh);
				}
				else
				{
					finish = true;
				}
			}
			break;
		}
		case Resource::Type::MATERIAL:
		{
			ReImport temp = App->Json_seria->GetUUIDMaterial(path.c_str());
			if (temp.uuid != 0)
			{
				App->resource_manager->filestoDelete.push_back(temp.uuid);
				//RELEASE_ARRAY(temp.directoryObj);
				//RELEASE_ARRAY(temp.nameMesh);
			}
			break;
		}
		}
	}
}


bool ModuleFS::AnyfileModificated(std::vector<AllFiles>& files)
{
	namespace stdfs = std::experimental::filesystem;

	const stdfs::directory_iterator end{};
	std::experimental::filesystem::path path = directory_Game;
	uint idpath = 0;
	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		if (stdfs::is_directory(*iter) == false)
		{
			std::string extension = GetExtension(iter->path().string());
			//Set lowercase the extension to normalize it
			for (std::string::iterator it = extension.begin(); it != extension.end(); it++)
			{
				*it = tolower(*it);
			}
			if (IsPermitiveExtension(extension.c_str()))
			{
				stdfs::file_time_type temp = stdfs::last_write_time(iter->path());
				std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
				if (files[idpath++].ftime == cftime)
				{
					// No Modificated
				}
				else
				{
					// LOG("MODIFICATED");
					switch (App->resource_manager->CheckFileType(extension.c_str()))
					{
					case Resource::Type::MESH:
					{
						bool finish = false; int id = 0;
						while (finish == false)
						{
							ReImport temp = App->Json_seria->GetUUIDPrefab(files[idpath - 1].directory_name, id++);
							if (temp.uuid != 0)
							{
								App->resource_manager->resourcesToReimport.push_back(temp);
							}
							else
							{
								finish = true;
							}
						}
						break;
					}
					case Resource::Type::MATERIAL:
					{
						App->resource_manager->resourcesToReimport.push_back(App->Json_seria->GetUUIDMaterial(files[idpath - 1].directory_name));
						break;
					}
					}
				}
			}
		}
		else
		{
			AnyfileModificatedFolder(iter->path().string(), files, idpath);
		}
	}
	return true;
}

bool ModuleFS::AnyfileModificatedFolder(std::experimental::filesystem::path path, std::vector<AllFiles>& files, uint& idpath)
{
	namespace stdfs = std::experimental::filesystem;

	const stdfs::directory_iterator end{};
	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		if (stdfs::is_directory(*iter) == false)
		{
			std::string extension = GetExtension(iter->path().string());
			//Set lowercase the extension to normalize it
			for (std::string::iterator it = extension.begin(); it != extension.end(); it++)
			{
				*it = tolower(*it);
			}
			if (IsPermitiveExtension(extension.c_str()))
			{
				stdfs::file_time_type temp = stdfs::last_write_time(iter->path());
				std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
				if (files[idpath++].ftime == cftime)
				{
					// No Modificated
				}
				else
				{
					// LOG("MODIFICATED");
					switch (App->resource_manager->CheckFileType(extension.c_str()))
					{
					case Resource::Type::MESH:
					{
						bool finish = false; int id = 0;
						while (finish == false)
						{
							ReImport temp = App->Json_seria->GetUUIDPrefab(files[idpath - 1].directory_name, id++);
							if (temp.uuid != 0)
							{
								App->resource_manager->resourcesToReimport.push_back(temp);
							}
							else
							{
								finish = true;
							}
						}
						break;
					}
					case Resource::Type::MATERIAL:
					{
						App->resource_manager->resourcesToReimport.push_back(App->Json_seria->GetUUIDMaterial(files[idpath - 1].directory_name));
						break;
					}
					}
				}
			}
		}
		else
		{
			AnyfileModificatedFolder(iter->path(), files, idpath);
		}
	}
	return true;
}

bool ModuleFS::IsPermitiveExtension(const char* extension)
{
	if (strcmp(extension, "png") == 0 || strcmp(extension, "jpg") == 0 ||
		strcmp(extension, "fbx") == 0 || strcmp(extension, "obj") == 0 || 
		strcmp(extension, "tga") == 0)
	{
		return true;
	}
	return false;
}

// If we Import a new File!
void ModuleFS::UpdateFilesAsstes()
{
	checkAssets.Start();
	DeleteAllFilesAssets(allfilesAsstes);
	GetAllFilesAssets(directory_Game, allfilesAsstes);
}

// If focused window check if have a file was modificated and reimport.-
void ModuleFS::CheckNowfilesAssets()
{
	checkAssets.Start();
	AnyfileModificated(allfilesAsstes);
}

void ModuleFS::DeleteFiles(std::vector<FilesNew>& files)
{
	for (int i = 0; i < files.size(); i++)
	{
		RELEASE_ARRAY(files[i].directory_name);
		RELEASE_ARRAY(files[i].directory_name_next)
		RELEASE_ARRAY(files[i].file_name)
	}
	files.clear();
}

void ModuleFS::DeleteFolders(std::vector<FoldersNew>& folders)
{
	for (int i = 0; i < folders.size(); i++)
	{
		RELEASE_ARRAY(folders[i].directory_name);
		RELEASE_ARRAY(folders[i].file_name);
		DeleteFolders(folders[i].folder_child);
	}
	folders.clear();
}

void ModuleFS::DeleteAllFilesAssets(std::vector<AllFiles>& filesAssets)
{
	for (int i = 0; i < filesAssets.size(); i++)
	{
		RELEASE_ARRAY(filesAssets[i].directory_name);
		RELEASE_ARRAY(filesAssets[i].file_name);
	}
	filesAssets.clear();
}

bool ModuleFS::DeleteFileLibrary(const char* file, DIRECTORY_IMPORT directory)
{
	std::string temp = file;
	switch (directory)
	{
	case DIRECTORY_IMPORT::IMPORT_DEFAULT:
	{
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_ASSETS:
	{
		temp = DIRECTORY_ASSETS + temp;
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY:
	{
		temp = DIRECTORY_LIBRARY + temp;
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MESHES:
	{
		temp = DIRECTORY_LIBRARY_MESHES + temp;
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MATERIALS:
	{
		temp = DIRECTORY_LIBRARY_MATERIALS + temp + ".dds";
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_SCRIPTS:
	{
		//temp = DIRECTORY_LIBRARY_SCRIPTS + temp + "";
		break;
	}
	}
	if (std::remove(temp.c_str()) == 0)
	{
		LOG("File Removed");
		return true;
	}
	return false;
}

uint ModuleFS::LoadFile(const char* file, char** buffer, DIRECTORY_IMPORT directory)
{
	// First apply direcotry
	std::string temp = file;
	switch (directory)
	{
	case IMPORT_DEFAULT:
	{
		break;
	}
	case IMPORT_DIRECTORY_ASSETS:
	{
		temp = DIRECTORY_ASSETS + temp;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY:
	{
		temp = DIRECTORY_LIBRARY + temp;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY_MESHES:
	{
		temp = DIRECTORY_LIBRARY_MESHES + temp;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY_MATERIALS:
	{
		temp = DIRECTORY_LIBRARY_MATERIALS + temp + ".dds";
		break;
	}
	}
	std::ifstream is(temp, std::ifstream::binary);
	int length = 0;
	if (is)
	{
		// get length of file:
		is.seekg(0, is.end);
		length = is.tellg();
		is.seekg(0, is.beg);

		*buffer = new char[length];

		is.read(*buffer, length);

		if (is)
		{
			LOG("File Loaded.")
		}
		else
		{
			LOG("Error %s", is.gcount());
		}

		is.close();
	}
	else
	{
		LOG("Error to Load File -> %s", file);
	}
	return length;
}

bool ModuleFS::SaveFile(const char* data, std::string name, uint size, DIRECTORY_IMPORT directory)
{
	// First apply direcotry
	switch (directory)
	{
	case IMPORT_DEFAULT:
	{
		break;
	}
	case IMPORT_DIRECTORY_ASSETS:
	{
		name = DIRECTORY_ASSETS + name;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY:
	{
		name = DIRECTORY_LIBRARY + name;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY_MESHES:
	{
		name = DIRECTORY_LIBRARY_MESHES + name;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY_MATERIALS:
	{
		name = DIRECTORY_LIBRARY_MATERIALS + name;
		break;
	}
	}
	// Open or Created ----------------------------------------
	std::ofstream outfile(name, std::ofstream::binary);

	if (outfile.good())
	{
		// write to outfile
		outfile.write(data, size);
		LOG("Save File %s", name.c_str());
	}
	else
	{
		LOG("Failed to write the file %s", name.c_str());
	}

	outfile.close();
	// ------------------------------------------------------------
	return false;
}

// Only Scripting!!! ------------------------------------------
bool ModuleFS::SaveScript(std::string name, TextEditor& editor, DIRECTORY_IMPORT directory)
{
	// First apply direcotry
	switch (directory)
	{
	case DIRECTORY_IMPORT::IMPORT_DEFAULT:
	{
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_ASSETS:
	{
		name = DIRECTORY_ASSETS + name;
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY:
	{
		name = DIRECTORY_LIBRARY + name;
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MESHES:
	{
		name = DIRECTORY_LIBRARY_MESHES + name;
		break;
	}
	case DIRECTORY_IMPORT::IMPORT_DIRECTORY_LIBRARY_MATERIALS:
	{
		name = DIRECTORY_LIBRARY_MATERIALS + name;
		break;
	}
	}
	// Open or Created ----------------------------------------
	name = name + ".cs";
	std::ofstream outfile(name);

	if (outfile.good())
	{
		// write to outfile
		outfile.write(editor.GetText().c_str(), editor.GetText().size());
		LOG("Save File %s", name.c_str());
	}
	else
	{
		LOG("Failed to write the file %s", name.c_str());
	}

	outfile.close();
	// ------------------------------------------------------------
	return true;
}

std::string ModuleFS::LoadScript(std::string file)
{
	std::ifstream fil(file.c_str());
	if (fil.good())
	{
		std::string str((std::istreambuf_iterator<char>(fil)), std::istreambuf_iterator<char>());
		return str;
	}
	return "";
}

bool ModuleFS::CheckIsFileExist(const std::string& name) {
	std::ifstream fil(name.c_str());
	return fil.good();
}

std::string ModuleFS::GetMainDirectory()
{
	return directory_Game;
}

void ModuleFS::FixNames_directories(std::vector<std::string>& files)
{
	for (int i = 0; i < files.size(); i++)
	{
		NormalitzatePath(files[i]);
		size_t EndName = files[i].find_last_of("/");
		files[i] = files[i].substr(EndName + 1);
	}
}

std::string ModuleFS::FixName_directory(std::string file)
{
	NormalitzatePath(file);
	size_t EndName = file.find_last_of("/");
	file = file.substr(EndName + 1);
	return file;
}

// If newExtension == nullptr, only return name not extension
std::string ModuleFS::FixExtension(std::string file, const char* newExtension)
{
	size_t EndName = file.find_last_of(".");
	file = file.substr(0, EndName);
	if(newExtension != nullptr)
		file += newExtension;
	return file;
}

std::string ModuleFS::GetFullPath(std::string path)
{
	if (path != "")
	{
		std::experimental::filesystem::path getpath(path);
		std::experimental::filesystem::path full_path = std::experimental::filesystem::system_complete(getpath);
		return full_path.string();
	}
	return "";
}

std::string ModuleFS::GetExtension(std::string file)
{
	size_t EndName = file.find_last_of(".");
	file = file.substr(EndName + 1);
	return file;
}

char* ModuleFS::ConverttoChar(std::string name)
{
	char* temp = new char[name.length() + 1];
	strcpy(temp, name.c_str());
	temp[name.length()] = '\0';
	return temp;
}

void ModuleFS::NormalitzatePath(std::string& path) const
{
	for (std::string::iterator temp = path.begin(); temp != path.end(); temp++)
	{
		if (*temp == '\\')
		{
			*temp = '/';
		}
	}
}

std::string ModuleFS::GetOnlyName(std::string file)
{
	std::string nameFile = file;
	NormalitzatePath(nameFile);
	size_t EndName = nameFile.find_last_of("/");
	nameFile = nameFile.substr(EndName + 1);
	EndName = nameFile.find_last_of(".");
	nameFile = nameFile.substr(0, EndName);

	return nameFile;
}

const char* ModuleFS::ConverttoConstChar(std::string name)
{
	char* temp = new char[name.length() + 1];
	strcpy(temp, name.c_str());
	temp[name.length()] = '\0';
	return temp;
}

std::string ModuleFS::GetAssetsDirectory()
{
	return directory_Assets;
}

std::string ModuleFS::AddDirectorybyType(std::string name, DIRECTORY_IMPORT directory)
{
	std::string temp = name;
	switch (directory)
	{
	case IMPORT_DEFAULT:
	{
		break;
	}
	case IMPORT_DIRECTORY_ASSETS:
	{
		temp = DIRECTORY_ASSETS + temp;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY:
	{
		temp = DIRECTORY_LIBRARY + temp;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY_MESHES:
	{
		temp = DIRECTORY_LIBRARY_MESHES + temp;
		break;
	}
	case IMPORT_DIRECTORY_LIBRARY_MATERIALS:
	{
		temp = DIRECTORY_LIBRARY_MATERIALS + temp;
		break;
	}
	}
	return temp;
}

std::string ModuleFS::CreateFolder(const char* file_name, bool forceCreate)
{
	namespace fs = std::experimental::filesystem;

	if (!fs::exists(file_name)) // Check if src folder exists
	{ 
		fs::create_directory(file_name); // create src folder
		return file_name;
	}
	else
	{
		if (forceCreate)
		{
			bool stop = false;
			int i = 1;
			std::string force;
			while (stop == false)
			{
				force = file_name;
				force += " " + std::to_string(i);
				if (!fs::exists(force.c_str()))
				{
					fs::create_directory(force.c_str());
					return force;
				}
				i++;
			}
		}
	}
}

void ModuleFS::CreateFolder(const char* file_name)
{
	namespace fs = std::experimental::filesystem;

	if (!fs::exists(file_name)) { // Check if src folder exists
		fs::create_directory(file_name); // create src folder
	}
}

// Special JSON Array Float3
JSON_Status ModuleFS::json_array_dotset_float3(JSON_Object *object, std::string name, float3 transform)
{
	JSON_Value* value = json_value_init_array();
	if (value == NULL) {
		return JSONFailure;
	}
	JSON_Array* arry = json_value_get_array(value);
	if (json_object_dotset_value(object, name.c_str(), value) == JSONFailure)
	{
		json_value_free(value);
		return JSONFailure;
	}
	json_array_append_number(arry, transform.x);
	json_array_append_number(arry, transform.y);
	json_array_append_number(arry, transform.z);
	return JSONSuccess;
}

JSON_Status ModuleFS::json_array_dotset_float2(JSON_Object *object, std::string name, float2 transform)
{
	JSON_Value* value = json_value_init_array();
	if (value == NULL) {
		return JSONFailure;
	}
	JSON_Array* arry = json_value_get_array(value);
	if (json_object_dotset_value(object, name.c_str(), value) == JSONFailure)
	{
		json_value_free(value);
		return JSONFailure;
	}
	json_array_append_number(arry, transform.x);
	json_array_append_number(arry, transform.y);
	return JSONSuccess;
}

JSON_Status ModuleFS::json_array_dotset_float4(JSON_Object *object, std::string name, float4 transform)
{
	JSON_Value* value = json_value_init_array();
	if (value == NULL) {
		return JSONFailure;
	}
	JSON_Array* arry = json_value_get_array(value);
	if (json_object_dotset_value(object, name.c_str(), value) == JSONFailure)
	{
		json_value_free(value);
		return JSONFailure;
	}
	json_array_append_number(arry, transform.x);
	json_array_append_number(arry, transform.y);
	json_array_append_number(arry, transform.z);
	json_array_append_number(arry, transform.w);
	return JSONSuccess;
}

float3 ModuleFS::json_array_dotget_float3_string(const JSON_Object* object, std::string name)
{
	JSON_Array* array = json_object_dotget_array(object, name.c_str());
	float3 transform;
	transform.x = (float)json_value_get_number(json_array_get_value(array, 0));
	transform.y = (float)json_value_get_number(json_array_get_value(array, 1));
	transform.z = (float)json_value_get_number(json_array_get_value(array, 2));

	return transform;
}
float2 ModuleFS::json_array_dotget_float2_string(const JSON_Object* object, std::string name)
{
	JSON_Array* array = json_object_dotget_array(object, name.c_str());
	float2 transform;
	transform.x = (float)json_value_get_number(json_array_get_value(array, 0));
	transform.y = (float)json_value_get_number(json_array_get_value(array, 1));

	return transform;
}
float4 ModuleFS::json_array_dotget_float4_string(const JSON_Object* object, std::string name)
{
	JSON_Array* array = json_object_dotget_array(object, name.c_str());
	float4 transform;
	transform.x = (float)json_value_get_number(json_array_get_value(array, 0));
	transform.y = (float)json_value_get_number(json_array_get_value(array, 1));
	transform.z = (float)json_value_get_number(json_array_get_value(array, 2));
	transform.w = (float)json_value_get_number(json_array_get_value(array, 3));

	return transform;
}