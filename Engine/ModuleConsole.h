#ifndef __ModuleConsole_H__
#define __ModuleConsole_H__

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"


class Console : public Module
{
public:

	Console(bool start_enabled = false);
	virtual ~Console();
	//bool Init(JSON_Object* node);
	//bool Start();
	//update_status Predate(float dt);
	update_status Update(float dt);
	//update_status Postdate(float dt);
	bool CleanUp();

	void OpenClose();
	bool IsOpen();

	void ClearLog();
	void AddLog(const char*, ...) IM_PRINTFARGS(2);

	void Draw(const char* title);

	void ExecCommand(const char* command_line);

	static int TextEditCallbackStub(ImGuiTextEditCallbackData* data); // In C++11 you are better off using lambdas for this sort of forwarding callbacks
	int TextEditCallback(ImGuiTextEditCallbackData* data); // In C++11 you are better off using lambdas for this sort of forwarding callbacks

public:
	bool console_activated = false;

private:

	char                  InputBuf[256];
	ImVector<char*>       Items;
	bool                  ScrollToBottom;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImVector<const char*> Commands;

	// Portable helpers
	static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
	static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
	static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }

};


#endif