#include "wwished.h"
#include <assert.h>

#include "include_wwise.h"
#include "AK/DefaultIO/Win32/AkFilePackageLowLevelIOBlocking.h"

//Plug-ins
#include <AK/Plugin/AkRoomVerbFXFactory.h>

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

//Initialize all Wwise modules. Receives the base path for soundbanks and the current language
bool Wwished::InitWwished(const wchar_t* base_path, const char* language)
{

	//Init default Wwise memory manager
	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;
	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(!"Could not create the memory manager.");
		return false;
	}


	//Initialize stream manager
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);
	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}

	//Initialize default IO device
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}


	// Create the Sound Engine using default initialization parameters
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}


	// Initialize the music engine using default initialization parameters
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);
	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}


	
#ifndef AK_OPTIMIZED
	// Initialize communications for debug purposes
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		return false;
	}
#endif 
	
	//Set base path for sound banks
	AKRESULT base_path_res = g_lowLevelIO.SetBasePath(base_path);
	if (base_path_res != AK_Success)
	{
		assert(!"Invalid base path!");
		return false;
	}

	//Set language
	Utility::SetLanguage(language);

	//Loads the Init Sound Bank
	Utility::LoadBank("Init.bnk");


	return true;
}

void Wwished::ProcessAudio()
{
	AK::SoundEngine::RenderAudio();
}

bool Wwished::CloseWwished()
{
	//Terminate comunication module (IMPORTANT: this must be the first module in being terminated)
	#ifndef AK_OPTIMIZED
		AK::Comm::Term();
	#endif 
		
	//Terminate the music engine
	AK::MusicEngine::Term();

	//Terminate the sound engine
	AK::SoundEngine::Term();

	//Terminate IO device
	g_lowLevelIO.Term();
	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	//Terminate the Memory Manager
	AK::MemoryMgr::Term();


	return true;
}

void Wwished::SetDefaultListener(unsigned long id)
{
	AkGameObjectID listener_id = id;
	AKRESULT res = AK::SoundEngine::SetDefaultListeners(&listener_id, 1);
}

void Wwished::Utility::SetLanguage(const char * language)
{
	AKRESULT res = AK::StreamMgr::SetCurrentLanguage((AkOSChar*)language);
	if (res == AK_Fail)
	{
		assert(!"Invalid language!");
	}
}

//Gets the bank path and returns true if exists. IMPORTANT: never call this before InitWwished()
bool Wwished::Utility::LoadBank(const char * path)
{
	bool ret = true;
	unsigned long bank_id;
	AKRESULT res = AK::SoundEngine::LoadBank(path, AK_DEFAULT_POOL_ID, bank_id);
	
	if (res != AK_Success)
	{
		ret = false;
	}

	return ret;
}

bool Wwished::Utility::UnLoadBank(const char * path)
{
	bool ret = true;
	AKRESULT res = AK::SoundEngine::UnloadBank(path, NULL);
	if (res != AK_Success)
		ret = false;

	return ret;
}

Wwished::SoundEmitter * Wwished::Utility::CreateEmitter(unsigned long id, const char * name, float x, float y, float z, bool is_default_listener)
{
	SoundEmitter* emitter = nullptr;

	emitter = new SoundEmitter(id, name);

	if (is_default_listener)
	{
		AkGameObjectID listener_id = emitter->GetID();
		AK::SoundEngine::SetDefaultListeners(&listener_id, 1);
	}
	
	emitter->SetPosition(x, y, z);

	return emitter;
}

void Wwished::Utility::ChangeState(const char* group, const char* new_state)
{
	AK::SoundEngine::SetState(group, new_state);
}

void Wwished::Utility::StopAllSounds()
{
	AK::SoundEngine::StopAll();
}

void Wwished::Utility::PauseAllSounds()
{
	AK::SoundEngine::PostEvent("Pause_All", AK_INVALID_GAME_OBJECT);
}

void Wwished::Utility::RestartAllSounds()
{
	AK::SoundEngine::PostEvent("Resume_All", AK_INVALID_GAME_OBJECT);
}

void Wwished::Utility::SetRTPCValue(const char * name, float value, AkGameObjectID id)
{
	AK::SoundEngine::SetRTPCValue(name, (AkRtpcValue)value, id);
}



// SoundEmitter class methods -------------------------------------------------
Wwished::SoundEmitter::SoundEmitter(unsigned long id, const char * n)
{
	EmitterID = id;
	name = n;
	AKRESULT res = AK::SoundEngine::RegisterGameObj((AkGameObjectID)EmitterID, name);
	if (res != AK_Success)
	{
		assert(!"Could not register GameObj! See res variable to more info");
	}
}

Wwished::SoundEmitter::~SoundEmitter()
{	
	AKRESULT res = AK::SoundEngine::UnregisterGameObj(EmitterID);
	if (res != AK_Success)
	{
		assert(!"Could not unregister GameObj! See res variable to more info");
	}
}

unsigned long Wwished::SoundEmitter::GetID()const
{
	return EmitterID;
}

const char * Wwished::SoundEmitter::GetName()const
{
	return name;
}



//The two vectors must be normalized and orthogonal!
void Wwished::SoundEmitter::SetPosition(float x, float y, float z, float x_front, float y_front, float z_front, float x_top, float y_top, float z_top)
{
	position.X = x;
	position.Y = y;
	position.Z = z;

	orient_front.X = x_front;
	orient_front.Y = y_front;
	orient_front.Z = z_front;
	orient_top.X = x_top;
	orient_top.Y = y_top;
	orient_top.Z = z_top;

	float length_front = sqrt(pow(orient_front.X, 2)+pow(orient_front.Y, 2) + pow(orient_front.Z, 2));
	float length_top = sqrt(pow(orient_top.X, 2) + pow(orient_top.Y, 2) + pow(orient_top.Z, 2));

	//Normalize vectors
	orient_front.X = orient_front.X / length_front;
	orient_front.Y = orient_front.Y / length_front;
	orient_front.Z = orient_front.Z / length_front;
	orient_top.X = orient_top.X / length_top;
	orient_top.Y = orient_top.Y / length_top;
	orient_top.Z = orient_top.Z / length_top;

	//Check if the are orthogonals
	float dot_prod = orient_top.X*orient_front.X + orient_top.Y*orient_front.Y + orient_top.Z*orient_front.Z;

	if (dot_prod >= 0.0001)
		assert(!"Vectors are not orthogonal!");

	AkSoundPosition sound_pos;
	sound_pos.Set(position, orient_front, orient_top);
	AKRESULT res = AK::SoundEngine::SetPosition((AkGameObjectID)EmitterID, sound_pos);
	if (res != AK_Success)
		assert(!"Something went wrong. Check the res variable for more info");
}

void Wwished::SoundEmitter::SetListener(unsigned long * id)const
{
	AKRESULT res = AK::SoundEngine::SetListeners(EmitterID, (AkGameObjectID*)id, 1);
}


void Wwished::SoundEmitter::PlayEvent(unsigned long id)const
{
	AK::SoundEngine::PostEvent(id, EmitterID);
}

void Wwished::SoundEmitter::PlayEvent(const char* name)const
{
	AK::SoundEngine::PostEvent(name, EmitterID);
}

void Wwished::SoundEmitter::StopEvent(const char * name)const
{
	AK::SoundEngine::ExecuteActionOnEvent(name, AK::SoundEngine::AkActionOnEventType_Stop);
}

void Wwished::SoundEmitter::SetAuxiliarySends(AkReal32 value, const char * target_bus, AkGameObjectID listener_id)const
{
	AkAuxSendValue environment;
	environment.listenerID = listener_id;
	environment.auxBusID = AK::SoundEngine::GetIDFromString(target_bus);
	environment.fControlValue = value;

	AKRESULT res = AK::SoundEngine::SetGameObjectAuxSendValues(EmitterID, &environment, 1);
}
