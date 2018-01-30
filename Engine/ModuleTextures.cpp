#include "Globals.h"
#include "ModuleTextures.h"
#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment(lib, "Devil/libx86/DevIl.lib")
#pragma comment(lib, "Devil/libx86/ILU.lib")
#pragma comment(lib, "Devil/libx86/ILUT.lib")

ModuleTextures::ModuleTextures(bool start_enabled)
{
	Awake_enabled = true;

	name = "Textures";
}

ModuleTextures::~ModuleTextures()
{
}

bool ModuleTextures::Init(JSON_Object * node)
{
	perf_timer.Start();

	bool ret = true;

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		LOG("DevIL version is different\n");
		ret = false;
	}

	ilInit();
	iluInit();
	ilutInit();

	ilutRenderer(ILUT_OPENGL);

	Awake_t = perf_timer.ReadMs();
	return ret;
}

//bool ModuleTextures::Start()
//{
//	perf_timer.Start();
//
//	Start_t = perf_timer.ReadMs();
//	return true;
//}
//
//update_status ModuleTextures::PreUpdate(float dt)
//{
//	perf_timer.Start();
//
//	preUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleTextures::Update(float dt)
//{
//	perf_timer.Start();
//
//	Update_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleTextures::PostUpdate(float dt)
//{
//	perf_timer.Start();
//
//	postUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

update_status ModuleTextures::UpdateConfig(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleTextures::CleanUp()
{
	return true;
}

GLuint ModuleTextures::LoadTexture(const char* filename)
{
	ILuint textureID;
	ILenum error;
	ILboolean success;

	// Texture Generation
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	success = ilLoadImage(filename);

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
	}

	//RELEASE MEMORY used by the image
	ilDeleteImages(1, &textureID); 

	return textureID;
}

GLuint ModuleTextures::LoadSkyboxTexture(const char * filename)
{
	ILuint textureID;
	ILenum error;
	ILboolean success;

	// Texture Generation
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	success = ilLoadImage(filename);

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
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//if the texture is smaller, than the image, we get the avarege of the pixels next to it
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		//same if the image bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//we repeat the pixels in the edge of the texture, it will hide that 1px wide line at the edge of the cube, which you have seen in the video
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    //we do it for vertically and horizontally (previous line)

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
	}

	//RELEASE MEMORY used by the image
	ilDeleteImages(1, &textureID);

	return textureID;
}
