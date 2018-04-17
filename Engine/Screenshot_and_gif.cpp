#include "Screenshot_and_gif.h"

#include "Application.h"
#include "ModuleInput.h"

#include "gif-h\gif.h"
#include "Devil\include\ilut.h"

void Culverin_Screenshot::TakeFullScreen()
{
	if (App->input->GetKey(FullScreenKey) == KEY_STATE::KEY_DOWN)
	{
		ILuint imageID = ilGenImage();
		ilBindImage(imageID);
		ilutGLScreen();
		ilEnable(IL_FILE_OVERWRITE);

		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);
		static char tmp_string[1024];
		sprintf_s(tmp_string, 1024, "Screenshots/ScreenFull/ScreenFull_%i_%i_%i_%i_%i_%i__%i.png", ltm.tm_mday, 1 + ltm.tm_mon, 1900 + ltm.tm_year, ltm.tm_hour, ltm.tm_min, ltm.tm_sec, ScreenshotsNum);

		ilSave(IL_PNG, tmp_string);
		ilDisable(IL_FILE_OVERWRITE);
		ilDeleteImage(imageID);
		ScreenshotsNum++;
	}
}

void Culverin_Screenshot::TakePartScreen()
{
	if (App->input->GetKey(PartScreenKey) == KEY_STATE::KEY_DOWN)
	{
		ImGuiIO GuiIO = ImGui::GetIO();
		switch (state)
		{
		case Culverin_PartScreenshot_STATE::Culverin_PartScreenshot_STATE_WAITING_FOR_POINT1:
			Point1.x = GuiIO.MousePos.x;
			Point1.y = GuiIO.DisplaySize.y - GuiIO.MousePos.y;
			state = Culverin_PartScreenshot_STATE::Culverin_PartScreenshot_STATE_WAITING_FOR_POINT2;
			break;
		case Culverin_PartScreenshot_STATE::Culverin_PartScreenshot_STATE_WAITING_FOR_POINT2:
		{
			Point2.x = GuiIO.MousePos.x;
			Point2.y = GuiIO.DisplaySize.y - GuiIO.MousePos.y;

			float2 MinPoint = float2((Point1.x <= Point2.x) ? Point1.x : Point2.x, (Point1.y <= Point2.y) ? Point1.y : Point2.y);
			float2 MaxPoint = float2((Point1.x >= Point2.x) ? Point1.x : Point2.x, (Point1.y >= Point2.y) ? Point1.y : Point2.y);

			uint width = abs(MaxPoint.x - MinPoint.x);
			uint height = abs(MaxPoint.y - MinPoint.y);
			unsigned char* s_pixels = new unsigned char[width * height * sizeof(unsigned char) * 3];
			glReadPixels(MinPoint.x, MinPoint.y, width, height, GL_RGB, GL_UNSIGNED_BYTE, s_pixels);
			ILuint imageID = ilGenImage();
			ilBindImage(imageID);
			ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, s_pixels);
			ilEnable(IL_FILE_OVERWRITE);

			time_t now = time(0);
			tm ltm;
			localtime_s(&ltm, &now);
			static char tmp_string[1024];
			sprintf_s(tmp_string, 1024, "Screenshots/ScreenPortion/ScreenPortion_%i_%i_%i_%i_%i_%i__%i.png", ltm.tm_mday, 1 + ltm.tm_mon, 1900 + ltm.tm_year, ltm.tm_hour, ltm.tm_min, ltm.tm_sec, ScreenshotsNum);

			ilSave(IL_PNG, tmp_string);
			ilDisable(IL_FILE_OVERWRITE);
			ilDeleteImage(imageID);
			RELEASE_ARRAY(s_pixels);

			ScreenshotsNum++;
			state = Culverin_PartScreenshot_STATE::Culverin_PartScreenshot_STATE_WAITING_FOR_POINT1;
			break;
		}
		}
	}
}

GifWriter gif_writer;

void Culverin_Gif::TakeFullScreen(float dt)
{
	switch (Gif_State)
	{
	case Culverin_PartGif_STATE::Culverin_PartGif_STATE_CAN_START:
		if (App->input->GetKey(FullScreenKey) == KEY_DOWN)
		{
			time_t now = time(0);
			tm ltm;
			localtime_s(&ltm, &now);
			static char tmp_string[1024];
			sprintf_s(tmp_string, 1024, "Screenshots/GifFull/GifFull_%i_%i_%i_%i_%i_%i__%i.gif", ltm.tm_mday, 1 + ltm.tm_mon, 1900 + ltm.tm_year, ltm.tm_hour, ltm.tm_min, ltm.tm_sec, ScreenshotsNum);
			ImGuiIO GuiIO = ImGui::GetIO();
			bool gif_in_progress = GifBegin(&gif_writer, tmp_string, GuiIO.DisplaySize.x, GuiIO.DisplaySize.y, (uint32_t)(dt * 100.0f), 8, false);
			if (gif_in_progress)
			{
				pixels = new unsigned char[GuiIO.DisplaySize.x *  GuiIO.DisplaySize.y * sizeof(unsigned char) * 4];
				Gif_State = Culverin_PartGif_STATE::Culverin_PartGif_STATE_RUNNING;
			}
		}
		break;
	case Culverin_PartGif_STATE::Culverin_PartGif_STATE_RUNNING:
		ImGuiIO GuiIO = ImGui::GetIO();
		glReadPixels(0, 0, GuiIO.DisplaySize.x, GuiIO.DisplaySize.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		GifWriteFrame(&gif_writer, pixels, GuiIO.DisplaySize.x, GuiIO.DisplaySize.y, (uint32_t)(dt * 100.0f), 8, false);
		if (App->input->GetKey(FullScreenKey) == KEY_DOWN)
		{
			GifEnd(&gif_writer);
			RELEASE_ARRAY(pixels);
			ScreenshotsNum++;
			Gif_State = Culverin_PartGif_STATE::Culverin_PartGif_STATE_CAN_START;
		}
		break;
	}
}

void Culverin_Gif::TakePartScreen(float dt)
{

}