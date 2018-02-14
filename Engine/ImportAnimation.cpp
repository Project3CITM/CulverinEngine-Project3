#include "ImportAnimation.h"
#include "ResourceAnimation.h"
#include "ModuleFS.h"
#include "ModuleGUI.h"
#include "JSONSerialization.h"


ImportAnimation::ImportAnimation()
{
}

ImportAnimation::~ImportAnimation()
{
}

bool ImportAnimation::Import(const char * file, uint uuid, bool isAutoImport)
{
	/*
	Configuration save;
	ResourceAnimation* tmpanim = new ResourceAnimation(0);

	tmpanim->name = animation->mName.C_Str();
	tmpanim->duration = animation->mDuration;
	tmpanim->ticksPerSec = animation->mTicksPerSecond;

	save.SetString("Name", animation->mName.C_Str());
	save.SetFloat("Duration", animation->mDuration);
	save.SetFloat("TicksPerSec", animation->mTicksPerSecond);

	save.AddArray("Bones");
	for (int y = 0; y < animation->mNumChannels; y++)
	{
		Configuration AnimBone;
		Bone* tempbone = new Bone();
		tempbone->name = animation->mChannels[y]->mNodeName.C_Str();
		AnimBone.SetString("Name", animation->mChannels[y]->mNodeName.C_Str());
		AnimBone.AddArray("PositionKeys");

		for (int r = 0; r < animation->mChannels[y]->mNumPositionKeys; r++)
		{
			PositionKey* tempposkey = new PositionKey();
			tempposkey->time = animation->mChannels[y]->mPositionKeys[r].mTime;
			tempposkey->position.x = animation->mChannels[y]->mPositionKeys[r].mValue.x;
			tempposkey->position.y = animation->mChannels[y]->mPositionKeys[r].mValue.y;
			tempposkey->position.z = animation->mChannels[y]->mPositionKeys[r].mValue.z;
			tempbone->positionkeys.push_back(tempposkey);

			Configuration positionKey;
			positionKey.SetFloat("Time", animation->mChannels[y]->mPositionKeys[r].mTime);
			positionKey.AddArrayFloat("Position", &animation->mChannels[y]->mPositionKeys[r].mValue.x, 3);
			AnimBone.AddArrayEntry(positionKey);
		}
		AnimBone.AddArray("RotationKeys");
		for (int x = 0; x < animation->mChannels[y]->mNumRotationKeys; x++)
		{
			RotationKey* temprotkey = new RotationKey();
			temprotkey->time = animation->mChannels[y]->mRotationKeys[x].mTime;
			temprotkey->rotation.w = animation->mChannels[y]->mRotationKeys[x].mValue.w;
			temprotkey->rotation.x = animation->mChannels[y]->mRotationKeys[x].mValue.x;
			temprotkey->rotation.y = animation->mChannels[y]->mRotationKeys[x].mValue.y;
			temprotkey->rotation.z = animation->mChannels[y]->mRotationKeys[x].mValue.z;
			tempbone->rotationkeys.push_back(temprotkey);

			Configuration rotationKey;
			rotationKey.SetFloat("Time", animation->mChannels[y]->mRotationKeys[x].mTime);
			rotationKey.AddArrayFloat("Rotation", &animation->mChannels[y]->mRotationKeys[x].mValue.w, 4);
			AnimBone.AddArrayEntry(rotationKey);
		}
		AnimBone.AddArray("ScaleKeys");
		for (int x = 0; x < animation->mChannels[y]->mNumScalingKeys; x++)
		{
			ScaleKey* tempscakey = new ScaleKey();
			tempscakey->time = animation->mChannels[y]->mScalingKeys[x].mTime;
			tempscakey->scale.x = animation->mChannels[y]->mScalingKeys[x].mValue.x;
			tempscakey->scale.y = animation->mChannels[y]->mScalingKeys[x].mValue.y;
			tempscakey->scale.z = animation->mChannels[y]->mScalingKeys[x].mValue.z;
			tempbone->scalekeys.push_back(tempscakey);

			Configuration scalekey;
			scalekey.SetFloat("Time", animation->mChannels[y]->mScalingKeys[x].mTime);
			scalekey.AddArrayFloat("Scale", &animation->mChannels[y]->mScalingKeys[x].mValue.x, 3);
			AnimBone.AddArrayEntry(scalekey);
		}
		save.AddArrayEntry(AnimBone);
		tmpanim->bones.push_back(tempbone);
	}
	char* buffer = nullptr;
	uint fileSize = save.SaveFile(&buffer, "Animation");
	App->filesystem->SaveFile(animation->mName.C_Str(), buffer, fileSize, FileType::fileAnimation);
	RELEASE(tmpanim);
	RELEASE_ARRAY(buffer);
	*/
	return true;
}

bool ImportAnimation::LoadResource(const char * file, ResourceAnimation * resourceMaterial)
{
	return false;
}
