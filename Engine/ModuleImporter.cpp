#include "ModuleImporter.h"
#include "Application.h"
#include "ImportMesh.h"
#include "ImportMaterial.h"
#include "ImportScript.h"
#include "CompMaterial.h"
#include "CompTransform.h"
#include "ModuleFS.h"
#include "ModuleInput.h"
#include "Scene.h"
#include "ModuleGUI.h"
#include "ModuleWindow.h"
#include "JSONSerialization.h"

#include <direct.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;

	name = "Importer";
}


ModuleImporter::~ModuleImporter()
{
	RELEASE(iMesh);
	RELEASE(iMaterial);
	RELEASE(iScript);
}

bool ModuleImporter::Init(JSON_Object* node)
{
	perf_timer.Start();

	// Will contain exe path
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		// When passing NULL to GetModuleHandle, it returns handle of exe itself
		GetModuleFileName(hModule, directoryExe, (sizeof(directoryExe)));
	}
	iMesh = new ImportMesh();
	iMaterial = new ImportMaterial();
	iScript = new ImportScript();

	Awake_t = perf_timer.ReadMs();
	return true;
}

bool ModuleImporter::Start()
{
	perf_timer.Start();

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	// Now InitSystem Domain Mono
	if (iScript->InitScriptingSystem())
	{
		LOG("Culverin Assembly Init SUCCESS.");
	}
	else
	{
		LOG("Culverin Assembly Init FAIL.");
	}

	Start_t = perf_timer.ReadMs();
	return true;
}

update_status ModuleImporter::PreUpdate(float dt)
{
	perf_timer.Start();

	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	{
		//ImportMesh* imp = new ImportMesh();
		//imp->Load("Baker_house.rin");
	}

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

GameObject* ModuleImporter::ProcessNode(aiNode* node, const aiScene* scene, GameObject* obj)
{	
	static int count = 0;
	GameObject* objChild = new GameObject(obj);
	objChild->SetName(App->GetCharfromConstChar(node->mName.C_Str()));

	CompTransform* trans = (CompTransform*)objChild->AddComponent(C_TRANSFORM);
	ProcessTransform(node, trans);

	// Process all the Node's MESHES
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		GameObject* newObj = nullptr;

		if (node->mNumMeshes > 1)
		{
			newObj = new GameObject(obj);
			std::string newName = "Submesh" + std::to_string(i);
			newObj->SetName(App->GetCharfromConstChar(newName.c_str()));
			CompTransform* newTrans = (CompTransform*)newObj->AddComponent(C_TRANSFORM);
			ProcessTransform(node, newTrans);
		}

		else
		{
			newObj = objChild;
		}

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		iMesh->Import(scene, mesh, newObj, node->mName.C_Str());
	}

	// Process children
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, objChild);
	}

	return objChild;
}

GameObject* ModuleImporter::ProcessNode(aiNode* node, const aiScene* scene, GameObject* obj, std::vector<ReImport>& resourcesToReimport)
{
	static int count = 0;
	GameObject* objChild = new GameObject(obj);
	objChild->SetName(App->GetCharfromConstChar(node->mName.C_Str()));

	CompTransform* trans = (CompTransform*)objChild->AddComponent(C_TRANSFORM);
	ProcessTransform(node, trans);

	// Process all the Node's MESHES
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		GameObject* newObj = nullptr;

		if (node->mNumMeshes > 1)
		{
			newObj = new GameObject(obj);
			std::string newName = "Submesh" + std::to_string(i);
			newObj->SetName(App->GetCharfromConstChar(newName.c_str()));
			CompTransform* newTrans = (CompTransform*)newObj->AddComponent(C_TRANSFORM);
			ProcessTransform(node, newTrans);
		}

		else
		{
			newObj = objChild;
		}

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		bool isReimported = false;
		for (int i = 0; i < resourcesToReimport.size(); i++)
		{
			if (strcmp(node->mName.C_Str(), resourcesToReimport[i].nameMesh) == 0)
			{
				iMesh->Import(scene, mesh, newObj, node->mName.C_Str(), resourcesToReimport[i].uuid);
				isReimported = true;
			}
		}
		if (isReimported == false)
		{
			iMesh->Import(scene, mesh, newObj, node->mName.C_Str(), 0);
		}
	}

	// Process children
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, objChild, resourcesToReimport);
	}

	return objChild;
}

void ModuleImporter::ProcessTransform(aiNode* node, CompTransform* trans)
{
	aiVector3D aiPos;
	aiQuaternion aiRot;
	aiVector3D aiScale;
	aiMatrix4x4 aiMatrix;
	float4x4 matrix;

	aiMatrix = node->mTransformation;

	node->mTransformation.Decompose(aiScale, aiRot, aiPos);

	trans->SetPos(float3(aiPos.x, aiPos.y, aiPos.z));
	trans->SetRot(Quat(aiRot.x, aiRot.y, aiRot.z, aiRot.w));
	trans->SetScale(float3(aiScale.x, aiScale.y, aiScale.z));

	trans->Enable();
}

void ModuleImporter::ProcessTransform(CompTransform* trans)
{
	//Set all variables to zero/identity
	trans->ResetMatrix();
	trans->Enable();
}

bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

bool ModuleImporter::Import(const char* file, Resource::Type type)
{
	bool ret = true;

	switch (type)
	{
	case Resource::Type::MESH:
	{
		LOG("IMPORTING MODEL, File Path: %s", file);

		//Clear vector of textures, but dont import same textures!
		//iMesh->PrepareToImport();
		const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
		if (scene != nullptr)
		{
			GameObject* obj = ProcessNode(scene->mRootNode, scene, nullptr);
			obj->SetName(App->GetCharfromConstChar(App->fs->FixName_directory(file).c_str()));

			//Now Save Serialitzate OBJ -> Prefab
			std::string Newdirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "\\" + App->fs->FixName_directory(file);
			App->Json_seria->SavePrefab(*obj, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());

			//App->scene->gameobjects.push_back(obj);
			App->scene->DeleteGameObject(obj, true);
		}
		else
		{
			ret = false;
			LOG("Cannot import this fbx.");
		}
		aiReleaseImport(scene);
		break;
	}
	case Resource::Type::MATERIAL:
	{
		LOG("IMPORTING TEXTURE, File Path: %s", file);
		iMaterial->Import(file);

		break;
	}
	case Resource::Type::SCRIPT:
	{
		LOG("IMPORTING SCRIPT, File Path: %s", file);
		iScript->Import(file);

		break;
	}
	case Resource::Type::UNKNOWN:
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "UNKNOWN file type dropped on window",
			file, App->window->window);
		LOG("UNKNOWN FILE TYPE, File Path: %s", file);
		break;
	}

	default:
		break;
	}

	return true;
}

bool ModuleImporter::Import(const char* file, Resource::Type type, std::vector<ReImport>& resourcesToReimport)
{
	bool ret = true;

	switch (type)
	{
	case Resource::Type::MESH:
	{
		LOG("IMPORTING MODEL, File Path: %s", file);

		//Clear vector of textures, but dont import same textures!
		//iMesh->PrepareToImport();
		const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
		if (scene != nullptr)
		{
			GameObject* obj = ProcessNode(scene->mRootNode, scene, nullptr, resourcesToReimport);
			obj->SetName(App->GetCharfromConstChar(App->fs->FixName_directory(file).c_str()));

			//Now Save Serialitzate OBJ -> Prefab
			std::string Newdirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "\\" + App->fs->FixName_directory(file);
			App->Json_seria->SavePrefab(*obj, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());

			App->scene->gameobjects.push_back(obj); // Need cleen
			App->scene->DeleteGameObject(obj);
		}
		else
		{
			ret = false;
			LOG("Cannot import this fbx.");
		}
		aiReleaseImport(scene);
		break;
	}
	case Resource::Type::MATERIAL:
	{
		LOG("IMPORTING TEXTURE, File Path: %s", file);
		//
		bool isReImport = false;
		for (int i = 0; i < resourcesToReimport.size(); i++)
		{
			if (strcmp(file, resourcesToReimport[i].directoryObj) == 0)
			{
				iMaterial->Import(file, resourcesToReimport[i].uuid);
				isReImport = true;
				break;
			}
		}
		if (isReImport == false)
		{
			iMaterial->Import(file);
		}
		break;
	}
	case Resource::Type::SCRIPT:
	{
		LOG("IMPORTING SCRIPT, File Path: %s", file);
		//
		bool isReImport = false;
		for (int i = 0; i < resourcesToReimport.size(); i++)
		{
			if (strcmp(file, resourcesToReimport[i].directoryObj) == 0)
			{
				iScript->Import(file, resourcesToReimport[i].uuid);
				isReImport = true;
				break;
			}
		}
		if (isReImport == false)
		{
			iScript->Import(file);
		}
		break;
	}
	case Resource::Type::UNKNOWN:
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "UNKNOWN file type dropped on window",
			file, App->window->window);
		LOG("UNKNOWN FILE TYPE, File Path: %s", file);
		break;
	}

	default:
		break;
	}

	return true;
}
