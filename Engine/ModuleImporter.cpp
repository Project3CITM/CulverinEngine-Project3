#include "ModuleImporter.h"
#include "Application.h"
#include "ImportMesh.h"
#include "ImportMaterial.h"
#include "ImportScript.h"
#include "ImportAnimation.h"
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
	RELEASE(iAnimation);
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
	iAnimation = new ImportAnimation();

	// Now InitSystem Domain Mono
	if (iScript->InitScriptingSystem())
	{
		LOG("Culverin Assembly Init SUCCESS.");
	}
	else
	{
		LOG("Culverin Assembly Init FAIL.");
	}

	Awake_t = perf_timer.ReadMs();
	return true;
}

bool ModuleImporter::Start()
{
	perf_timer.Start();

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

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

GameObject* ModuleImporter::ProcessNode(aiNode* node, const aiScene* scene, GameObject* obj, const char* file)
{	
	static int count = 0;
	GameObject* objChild = new GameObject(obj);
	objChild->SetName(node->mName.C_Str());

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
			newObj->SetName(newName.c_str());
			CompTransform* newTrans = (CompTransform*)newObj->AddComponent(C_TRANSFORM);
			ProcessTransform(node, newTrans);
		}
		else
		{
			newObj = objChild;
		}

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		iMesh->Import(scene, mesh, newObj, node->mName.C_Str(), file);
	}

	// Process children
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, objChild, file);
	}

	return objChild;
}

GameObject* ModuleImporter::ProcessNode(aiNode* node, const aiScene* scene, GameObject* obj, std::vector<ReImport>& resourcesToReimport)
{
	static int count = 0;
	GameObject* objChild = new GameObject(obj);
	objChild->SetName(node->mName.C_Str());

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
			newObj->SetName(newName.c_str());
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
			if (strcmp(node->mName.C_Str(), resourcesToReimport[i].name_mesh) == 0)
			{
				iMesh->Import(scene, mesh, newObj, node->mName.C_Str(), resourcesToReimport[i].directory_obj, resourcesToReimport[i].uuid);
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

bool ModuleImporter::Import(const char* file, Resource::Type type, bool isAutoImport)
{
	bool ret = true;

	switch (type)
	{
	case Resource::Type::MESH:
	{
		LOG("IMPORTING MODEL, File Path: %s", file);

		//Clear vector of textures, but dont import same textures!
		//iMesh->PrepareToImport();
		unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_LimitBoneWeights;
		const aiScene* scene = aiImportFile(file, flags);
		if (scene != nullptr)
		{
			if (scene->HasAnimations())
			{
				std::string fbx_name = App->fs->GetOnlyName(file);
				for (int i = 0; i < scene->mNumAnimations; i++)
				{
					scene->mAnimations[i]->mName = fbx_name;
					scene->mAnimations[i]->mName.Append("Animation");
					scene->mAnimations[i]->mName.Append(std::to_string(i).c_str());
					LOG("IMPORTING ANIMATION, File Path: %s", scene->mAnimations[i]->mName.C_Str());
					iAnimation->Import(scene->mAnimations[i], scene->mAnimations[i]->mName.C_Str(), fbx_name.c_str());
				}
			}
			GameObject* obj = ProcessNode(scene->mRootNode, scene, nullptr, file);
			obj->SetName(App->fs->FixName_directory(file).c_str());

			//Now Save Serialitzate OBJ -> Prefab
			std::string Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "\\" + App->fs->FixName_directory(file);
			if (isAutoImport)
			{
				App->json_seria->SavePrefab(*obj, App->fs->GetOnlyPath(file).c_str(), Newdirectory.c_str());
			}
			else
			{
				App->json_seria->SavePrefab(*obj, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
			}

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
		iMaterial->Import(file, 0, isAutoImport);

		break;
	}
	case Resource::Type::SCRIPT:
	{
		LOG("IMPORTING SCRIPT, File Path: %s", file);
		iScript->Import(file, 0, isAutoImport);

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

bool ModuleImporter::Import(const char* file, Resource::Type type, std::vector<ReImport>& resourcesToReimport, bool auto_reimport)
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
			if (scene->HasAnimations())
			{
				std::string fbx_name = App->fs->GetOnlyName(file);
				for (int i = 0; i < scene->mNumAnimations; i++)
				{
					scene->mAnimations[i]->mName = fbx_name;
					scene->mAnimations[i]->mName.Append("Animation");
					scene->mAnimations[i]->mName.Append(std::to_string(i).c_str());
					LOG("IMPORTING ANIMATION, File Path: %s", scene->mAnimations[i]->mName.C_Str());
					iAnimation->Import(scene->mAnimations[i], scene->mAnimations[i]->mName.C_Str(), fbx_name.c_str());
				}
			}
			GameObject* obj = ProcessNode(scene->mRootNode, scene, nullptr, resourcesToReimport);
			obj->SetName(App->fs->FixName_directory(file).c_str());

			//Now Save Serialitzate OBJ -> Prefab
			std::string Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "\\" + App->fs->FixName_directory(file);
			App->json_seria->SavePrefab(*obj, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());

			//TODO-> Elliot -------------------------------------------
			App->scene->root->AddChildGameObject(obj); // Temp obj needs to be erased
			App->scene->DeleteGameObject(obj);
			//---------------------------------------------------------
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
			if (strcmp(file, resourcesToReimport[i].directory_obj) == 0)
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
			if (strcmp(file, resourcesToReimport[i].directory_obj) == 0)
			{
				iScript->ReImportScript(file, std::to_string(resourcesToReimport[i].uuid), nullptr, auto_reimport);
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
