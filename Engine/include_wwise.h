#ifndef INCLUDE_WWISE_H
#define INCLUDE_WWISE_H


#include <AK/SoundEngine/Common/AkSoundEngine.h>	  
#include <AK/MusicEngine/Common/AkMusicEngine.h>	  
#include <AK/SoundEngine/Common/AkMemoryMgr.h>       
#include <AK/SoundEngine/Common/AkModule.h>           
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>   

#include <AK/Tools/Common/AkPlatformFuncs.h>           



// Input libraries
#ifndef _DEBUG  // Profile build configuration must be loaded instead of Debug
#define AK_OPTIMIZED
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkStreamMgr.lib")
#pragma comment( lib, "AK/Release(StaticCRT)/lib/AkRoomVerbFX.lib")
#else
#include <AK/Comm/AkCommunication.h>
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/CommunicationCentral.lib")
#pragma comment( lib, "AK/ws2_32.lib") 
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkStreamMgr.lib")
#pragma comment( lib, "AK/Debug(StaticCRT)/lib/AkRoomVerbFX.lib")

#endif

// Microsoft DirectX external libs
#pragma comment( lib, "AK/dinput8.lib")
#pragma comment( lib, "AK/dsound.lib") 
#pragma comment( lib, "AK/dxguid.lib")  


// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
	void * AllocHook(size_t in_size);
	void FreeHook(void * in_ptr);
	void * VirtualAllocHook(void * in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect);
	void VirtualFreeHook(void * in_pMemAddress, size_t in_size, DWORD in_dwFreeType);
#endif
}

#endif