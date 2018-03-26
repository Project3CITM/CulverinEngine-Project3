#ifndef _MODULEFS_
#define _MODULEFS_

#include "Globals.h"
#include "Module.h"
#include "WindowProject.h"
#include "Timer.h"

#include <filesystem>
#include <iostream>
#include <experimental/filesystem>
#include <fstream>

#define DIRECTORY_ASSETS "Assets/"
#define DIRECTORY_LIBRARY "Library/"
#define DIRECTORY_LIBRARY_MESHES "Library/Meshes/"
#define DIRECTORY_LIBRARY_MATERIALS "Library/Materials/"
#define DIRECTORY_LIBRARY_SCRIPTS "Library/Scripts/"
#define DIRECTORY_LIBRARY_ANIMATIONS "Library/Animations/"
#define DIRECTORY_LIBRARY_JSON "Library/JSON/"
#define DIRECTORY_LIBRARY_FONTS "Library/Fonts/"

class TextEditor;

enum DIRECTORY_IMPORT
{
	IMPORT_DEFAULT,
	IMPORT_DIRECTORY_ASSETS,
	IMPORT_DIRECTORY_LIBRARY,
	IMPORT_DIRECTORY_LIBRARY_MESHES,
	IMPORT_DIRECTORY_LIBRARY_MATERIALS,
	IMPORT_DIRECTORY_LIBRARY_SCRIPTS,
	IMPORT_DIRECTORY_LIBRARY_ANIMATIONS,
	IMPORT_DIRECTORY_LIBRARY_JSON,
	IMPORT_DIRECTORY_LIBRARY_FONT
};

struct AllFiles
{
	const char* directory_name = nullptr;
	char* file_name = nullptr;
	std::time_t ftime;
};

class ModuleFS : public Module
{
public:
	ModuleFS(bool start_enabled = true);
	~ModuleFS();
	bool Init(JSON_Object* node);

	bool Start();
	update_status PreUpdate(float dt);

	std::string CopyFileToAssets(const char* fileNameFrom, const char* fileNameTo = "");
	// Same to CopyFileToAssets but this return path of file to save
	std::string CopyFileToAssetsS(const char* fileNameFrom, const char* fileNameTo = "");

	void CopyFolderToLibrary(const char* folder);

	//update_status UpdateConfig(float dt);

	// Imort all files with havent ".meta".
	bool ImportAllFilesNoMeta(std::vector<AllFiles>& files);

	uint LoadFile(const char* file, char** buffer, DIRECTORY_IMPORT directory = IMPORT_DEFAULT);
	
	//Name of the file (NOT DIRECTORY) ------------------
	bool SaveFile(const char* data, std::string name, uint size, DIRECTORY_IMPORT directory = IMPORT_DEFAULT);
	// Only Scripting!!! ---------------
	bool SaveScript(std::string name, TextEditor& editor, DIRECTORY_IMPORT directory = IMPORT_DEFAULT);
	// Load Script From Assets ------------
	std::string LoadScript(std::string file);
	// Duplicate File
	bool DuplicateFile(const char* origin, std::string destination, DIRECTORY_IMPORT directory = IMPORT_DEFAULT);
	// Get Files to Window Project ---------------------------------------------------
	void GetAllFolders(std::experimental::filesystem::path path, std::string folderActive, std::vector<FoldersNew>& folders);
	bool GetAllFoldersChild(std::experimental::filesystem::path path, std::string folderActive, std::vector<FoldersNew>& folders);
	void GetAllFiles(std::experimental::filesystem::path path, std::vector<FilesNew>& files);
	void GetAllFilesByExtension(std::experimental::filesystem::path path, std::vector<std::string>& files, const char* extension);

	// Get Files to check if a file was modificated ---------------------------------
	void GetAllFilesAssets(std::experimental::filesystem::path path, std::vector<AllFiles>& files);
	void GetAllFilesFromFolder(std::experimental::filesystem::path path, std::list<const char*>& files);
	void GetAllFilesFromFolder(std::experimental::filesystem::path path, std::vector<uint>& files);
	void GetUUIDFromFile(std::string path, std::vector<uint>& files);
	void GetAllMetas(std::experimental::filesystem::path path, std::vector<std::string>& files_meta);
	void ReImportAllFiles();
	bool AnyfileModificated(std::vector<AllFiles>& files);
	bool AnyfileModificatedFolder(std::experimental::filesystem::path path, std::vector<AllFiles>& files, uint& id);
	bool IsPermitiveExtension(const char * extension);
	void UpdateFilesAssets();
	void CheckNowfilesAssets();

	// Delete Methods -----------------------------------
	void DeleteFiles(std::vector<FilesNew>& files);
	void DeleteFolders(std::vector<FoldersNew>& folders);
	void DeleteAllFilesAssets(std::vector<AllFiles>& filesAssets);

	bool DeleteFileLibrary(const char* file, DIRECTORY_IMPORT directory);

	bool CheckIsFileExist(const std::string & name);

	std::string GetMainDirectory();

	// Utilities ----------------------------------
	void FixNames_directories(std::vector<std::string>& files);
	std::string FixName_directory(std::string files);
	std::string FixExtension(std::string file, const char* newExtension = nullptr);
	std::string GetFullPath(std::string path);
	std::string GetOnlyPath(std::string file);
	std::string GetPathWithoutExtension(std::string file);
	std::string GetExtension(std::string file, bool is_meta = false);
	char* ConverttoChar(std::string name);
	void NormalitzatePath(std::string& path) const;
	std::string GetOnlyName(std::string file);
	std::string GetToAsstes(std::string file);
	const char* ConverttoConstChar(std::string name);
	std::string CreateFolder(const char* file_name, bool forceCreate);
	void CreateFolder(const char * file_name);

	bool CheckAssetsIsModify();

	// SERIALIZATION METHODS
	// Special JSON Array -> float3, float2, Color
	/* float3 */
	JSON_Status json_array_dotset_float3(JSON_Object *object, std::string name, float3 transform);
	/* float2 */
	JSON_Status json_array_dotset_float2(JSON_Object *object, std::string name, float2 transform);
	/* Color - (r,g,b,w) or (x,y,z,w).*/
	JSON_Status json_array_dotset_float4(JSON_Object *object, std::string name, float4 transform);

	float3 json_array_dotget_float3_string(const JSON_Object* object, std::string name);
	float2 json_array_dotget_float2_string(const JSON_Object* object, std::string name);
	float4 json_array_dotget_float4_string(const JSON_Object* object, std::string name);

	//Don't used for now -------------------------
	std::string GetAssetsDirectory();

private:
	Timer checkAssets;
	std::vector<AllFiles> allfilesAsstes;

	char ownPth[MAX_PATH];
	std::string directory_assets;
	std::string directory_game;

};

#endif