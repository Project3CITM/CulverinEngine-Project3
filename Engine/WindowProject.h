#ifndef _WINDOWPROJECT_
#define _WINDOWPROJECT_

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"
#include "WindowManager.h"
#include "Timer.h"

#define MARGEBUTTON 20
#define DISTANCEBUTTONS 10
#define SPERATIONCOLUMN 200

enum TYPE_FILE
{
	NON,
	FOLDER,
	FBX,
	OBJ,
	PNG,
	JPG,
	SCRIPT,
	DDS
};


struct FilesNew
{
	const char* directory_name = nullptr;
	const char* directory_name_next = nullptr;
	char* file_name = nullptr;
	TYPE_FILE file_type;
};

struct FoldersNew
{
	const char* directory_name = nullptr;
	bool active = false;
	bool haveSomething = false;
	char* file_name = nullptr;
	std::vector<FoldersNew> folder_child;
};

class Project : public WindowManager
{
public:

	Project();
	virtual ~Project();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	TYPE_FILE SetType(std::string name);

	//NEW 
	void ShowProject();
	void Folders_update(std::vector<FoldersNew>& folders);
	void Files_Update(const std::vector<FilesNew>& files);

	const char* GetDirectory() const;
	void SetDirectory(const char* newDirectory);
	void UpdateNow();
	void SetAllFolderBool(std::vector<FoldersNew>& folders, bool setBoolean);
	//void AddFile(std::vector<Files>* folderViwe, std::string newFile);

private:

	//std::vector<Folders> folders;
	char* directory_see = nullptr;
	int sizeFiles;
	std::vector<FoldersNew> folders;
	std::vector<FilesNew> files;
	bool updateFoldersNow = false;
	bool updateFilesNow = false;

	uint folder_icon;
	uint icon_png;
	uint icon_jpg;
	uint icon_fbx;
	uint icon_obj;
	uint icon_script;
	uint icon_unknown;
	Timer timeFolders;
	Timer timeFiles;
};

#endif