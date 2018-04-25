#include "JSONSerialization.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceScript.h"
#include "ResourceFont.h"
#include "ResourceAnimation.h"
#include "CompScript.h"
#include "ModuleFS.h"
#include "ModuleResourceManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "ModuleAudio.h"
#include "InputManager.h"
#include "InputAction.h"
#include "PlayerActions.h"
#include "ModuleAnimation.h"
#include "ResourceMesh.h"

std::string JSONSerialization::SaveSceneBinary()
{
	std::string ret;
	return ret;
}

void JSONSerialization::SaveIntBinary(char** cursor, int value)
{
	uint bytes_to_copy = sizeof(value);
	memcpy((*cursor), &value, bytes_to_copy);
	(*cursor) += bytes_to_copy;
}

void JSONSerialization::SaveFloatBinary(char** cursor, float value)
{
	uint bytes_to_copy = sizeof(value);
	memcpy((*cursor), &value, bytes_to_copy);
	(*cursor) += bytes_to_copy;
}

void JSONSerialization::SaveBoolBinary(char** cursor, bool value)
{
	uint bytes_to_copy = sizeof(value);
	memcpy((*cursor), &value, bytes_to_copy);
	(*cursor) += bytes_to_copy;
}

void JSONSerialization::SaveStringBinary(char** cursor, std::string value)
{
	uint bytes_to_copy = sizeof(int);
	int name_size = value.size();
	memcpy((*cursor), &name_size, bytes_to_copy);
	(*cursor) += bytes_to_copy;
	memcpy((*cursor), value.c_str(), name_size);
	(*cursor) += name_size;

}

void JSONSerialization::SaveConstCharBinary(char** cursor, const char* value)
{
	uint bytes_to_copy = sizeof(int);
	int name_size = strlen(value);
	memcpy((*cursor), &name_size, bytes_to_copy);
	(*cursor) += bytes_to_copy;
}

void JSONSerialization::SaveFloat2Binary(char** cursor, math::float2 value)
{
	uint bytes_to_copy = sizeof(float);
	memcpy((*cursor), &value.x, bytes_to_copy);
	(*cursor) += bytes_to_copy;
	memcpy((*cursor), &value.y, bytes_to_copy);
	(*cursor) += bytes_to_copy;
}

void JSONSerialization::SaveFloat3Binary(char** cursor, math::float3 value)
{
	uint bytes_to_copy = sizeof(float);
	memcpy((*cursor), &value.x, bytes_to_copy);
	(*cursor) += bytes_to_copy;
	memcpy((*cursor), &value.y, bytes_to_copy);
	(*cursor) += bytes_to_copy;
	memcpy((*cursor), &value.z, bytes_to_copy);
	(*cursor) += bytes_to_copy;
}

void JSONSerialization::SaveFloat4Binary(char** cursor, math::float4 value)
{
	uint bytes_to_copy = sizeof(float);
	memcpy((*cursor), &value.x, bytes_to_copy);
	(*cursor) += bytes_to_copy;
	memcpy((*cursor), &value.y, bytes_to_copy);
	(*cursor) += bytes_to_copy;
	memcpy((*cursor), &value.z, bytes_to_copy);
	(*cursor) += bytes_to_copy;
	memcpy((*cursor), &value.w, bytes_to_copy);
	(*cursor) += bytes_to_copy;
}
