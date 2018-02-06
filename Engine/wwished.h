#ifndef _WWISHED_H_
#define _WWISHED_H_


#include <AK/SoundEngine/Common/AkTypes.h>


namespace Wwished
{
	//Life cycle functions
	bool InitWwished(const wchar_t* base_path, const char* language);
	void ProcessAudio();
	bool CloseWwished();

	void SetDefaultListener(unsigned long id);
		
	class SoundEmitter
	{
	public:

		SoundEmitter(unsigned long id, const char* n);
		~SoundEmitter();

		unsigned long GetID() const;
		const char* GetName()const;
		void SetPosition(float x=0, float y = 0, float z = 0, float x_front = 1, float y_front=0, float z_front=0, float x_top=0, float y_top=1, float z_top=0);
		void SetListener(unsigned long* id)const;

		void PlayEvent(unsigned long id)const;
		void PlayEvent(const char* name)const;

		void StopEvent(const char* name)const;

		void SetAuxiliarySends(AkReal32 value, const char* target_bus, AkGameObjectID listener_id)const;

	private:
		AkGameObjectID EmitterID;
		const char* name = nullptr;
		AkVector position;
		AkVector orient_top;
		AkVector orient_front;
	};


	namespace Utility
	{
		void SetLanguage(const char* language);
		bool LoadBank(const char* path);
		bool UnLoadBank(const char* path);
		SoundEmitter* CreateEmitter(unsigned long id, const char* name,  float x, float y, float z, bool is_default_listener = false);
		void ChangeState(const char* group, const char* new_state);	
		void StopAllSounds();
		void PauseAllSounds();
		void RestartAllSounds();
		void SetRTPCValue(const char* name, float value, AkGameObjectID game_obj_id = AK_INVALID_GAME_OBJECT);
	}

}

#endif
