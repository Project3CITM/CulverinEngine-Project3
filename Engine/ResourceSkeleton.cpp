#include "ResourceSkeleton.h"
#include "Application.h"


SkeletonBone::~SkeletonBone()
{
	for (std::vector<VertexWeight*>::iterator temp = weights.begin(); temp != weights.end(); temp++)
	{
		RELEASE((*temp));
	}
	weights.clear();
}

ResourceSkeleton::ResourceSkeleton(uint uuid) : Resource(uuid, Resource::Type::SKELETON, Resource::State::UNLOADED)
{
	num_game_objects_use_me = 0;
	LOG("Resource Skeleton Created!");
}

ResourceSkeleton::~ResourceSkeleton()
{
	for (std::vector<SkeletonBone*>::iterator temp = bones.begin(); temp != bones.end(); temp++)
	{
		RELEASE((*temp));
	}
	bones.clear();
}

void ResourceSkeleton::InitInfo(char * name, const char * path)
{
	name = name;
	path_assets = path;
}

bool ResourceSkeleton::LoadToMemory()
{
	state = Resource::State::LOADED;
	LOG("Loaded Resource Skeleton");
	return false;
}

void ResourceSkeleton::DeleteToMemory()
{
	state = Resource::State::UNLOADED;

	LOG("Unloaded Resource Skeleton!");
}
