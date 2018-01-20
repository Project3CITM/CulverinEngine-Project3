#include "ImportMaterial.h"
#include "CompMaterial.h"
#include "ModuleFS.h"
#include "ModuleGUI.h"
#include "JSONSerialization.h"
#include "ResourceMaterial.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment(lib, "Devil/libx86/DevIl.lib")
#pragma comment(lib, "Devil/libx86/ILU.lib")
#pragma comment(lib, "Devil/libx86/ILUT.lib")

ImportMaterial::ImportMaterial()
{
}


ImportMaterial::~ImportMaterial()
{
}

bool ImportMaterial::Import(const char* file, uint uuid)
{
	bool ret = false;
	char* buffer;
	uint size_file = App->fs->LoadFile(file, &buffer);
	if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size_file))
	{
		ILuint size;
		ILubyte *data;
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
		if (size > 0)
		{
			data = new ILubyte[size]; // allocate data buffer
			uint uuid_mesh = 0;
			if (uuid == 0) // if direfent create a new resource with the resource deleted
			{
				uuid_mesh = App->random->Int();
			}
			else
			{
				uuid_mesh = uuid;
			}
			ResourceMaterial* res_material = (ResourceMaterial*)App->resource_manager->CreateNewResource(Resource::Type::MATERIAL, uuid_mesh);
			res_material->InitInfo(App->fs->FixName_directory(file).c_str());
			std::string Newdirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "\\" +App->fs->FixName_directory(file);
			App->Json_seria->SaveMaterial(res_material, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
			std::string name = std::to_string(uuid_mesh);
			name = App->fs->FixName_directory(name);//?
			name = App->fs->FixExtension(name, ".dds");
			if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
				ret = App->fs->SaveFile((char*)data, name, size, IMPORT_DIRECTORY_LIBRARY_MATERIALS);
			RELEASE_ARRAY(data);
		}
	}
	RELEASE_ARRAY(buffer);
	//return ret;

	return false;
}

Texture ImportMaterial::Load(const char* file)
{
	Texture texture;
	ILuint textureID;
	ILenum error;
	ILboolean success;

	// Texture Generation
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	std::string temp = file;
	temp = DIRECTORY_LIBRARY_MATERIALS + temp + ".dds";
	success = ilLoadImage(temp.c_str());

	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		//Flip the image into the right way
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		if (!ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE))
		{
			error = ilGetError();
			LOG("Image conversion failed - IL reportes error: %i, %s", error, iluErrorString(error));
			exit(-1);
			texture.id = 0;
			return texture;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			ilGetInteger(IL_IMAGE_FORMAT),
			ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT),
			0,
			ilGetInteger(IL_IMAGE_FORMAT),
			GL_UNSIGNED_BYTE,
			ilGetData());

		LOG("Texture Application Successful.");
	}

	else
	{
		error = ilGetError();
		LOG("Image Load failed - IL reportes error: %i, %s", error, iluErrorString(error));
		texture.id = 0;
		return texture;
	}

	//RELEASE MEMORY used by the image
	ilDeleteImages(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, 0);


	texture.id = textureID;
	texture.name = file;

	//Add the texture to the component
	if (textureID != 0)
	{
		return texture;
	}
	texture.id = 0;
	return texture;
}

bool ImportMaterial::LoadResource(const char* file, ResourceMaterial* resourceMaterial)
{
	Texture texture = Load(file);
	LOG("Resources: %s, Loaded in Memory!", resourceMaterial->name);
	if (texture.id > 0)
	{
		resourceMaterial->Init(texture);
		resourceMaterial->LoadToMemory();
		return true;
	}
	return true;
}
