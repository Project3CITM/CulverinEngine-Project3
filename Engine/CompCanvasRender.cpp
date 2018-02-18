#include "CompCanvasRender.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompImage.h"
#include "CompText.h"
#include "CompRectTransform.h"
#include "CompCanvas.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "ModuleRenderGui.h"
#include "SDL/include/SDL_opengl.h"
#include "GL3W/include/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

CompCanvasRender::CompCanvasRender(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Canvas Render";
	glGetError();
	my_canvas=(CompCanvas*)parent->FindParentComponentByType(Comp_Type::C_CANVAS);
	glGenBuffers(1, &vertices_id);
	CheckOpenGlError("glGenBuffers vertices");

	glGenBuffers(1, &indices_id);
	CheckOpenGlError("glGenBuffers indices_id");

}

CompCanvasRender::CompCanvasRender(const CompCanvasRender & copy, GameObject * parent) : Component(Comp_Type::C_CANVAS_RENDER, parent)
{
}

CompCanvasRender::~CompCanvasRender()
{
}

void CompCanvasRender::Update(float dt)
{
	AddToCanvas();
}

void CompCanvasRender::Clear()
{
	glDeleteBuffers(1, &vertices_id);
	vertices_id = 0;
	glDeleteBuffers(1, &indices_id);
	indices_id = 0;

}

void CompCanvasRender::ShowOptions()
{
	//ImGui::MenuItem("CREATE", NULL, false, false);
	if (ImGui::MenuItem("Reset"))
	{
		// Not implmented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_CAMERA)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompCanvasRender*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompCanvasRender::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionCanvasRender");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionCanvasRender"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	ImGui::TreePop();
}

void CompCanvasRender::CopyValues(const CompCanvasRender* component)
{
	//more...
}

void CompCanvasRender::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	//...
}

void CompCanvasRender::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	Enable();
}

void CompCanvasRender::AddToCanvas()
{
	if (my_canvas == nullptr)
	{
		LOG("ERROR:CanvasRender don't found");
		return;
	}
	
	my_canvas->AddCanvasRender(this);
}

void CompCanvasRender::RemoveFromCanvas()
{
	if (my_canvas == nullptr)
	{
		LOG("ERROR:CanvasRender with UID &i don't have Canvas", parent->GetUUID());
		return;
	}
	my_canvas->RemoveCanvasRender(this);

}

void CompCanvasRender::ProcessImage(CompImage * image)
{
	//UV Setup
	//-x+y	//0,1-------1,1 //+x+y
			//|	3     /	2|
			//|		/	 |
			//|	0 /		1|
	//-x-y	//0,0-------1,0 //+x-y

	//Clear All Vertices and indices
	vertices.clear();
	indices.clear();
	CanvasVertex ver;

	ver.position = parent->GetComponentRectTransform()->GetSouthWestPosition();
	ver.tex_coords = float2(0.0f, 0.0f);
	vertices.push_back(ver);

	ver.position = parent->GetComponentRectTransform()->GetSouthEastPosition();
	ver.tex_coords = float2(1.0f, 0.0f);
	vertices.push_back(ver);

	ver.position = parent->GetComponentRectTransform()->GetNorthEastPosition();
	ver.tex_coords = float2(1.0f, 1.0f);
	vertices.push_back(ver);

	ver.position = parent->GetComponentRectTransform()->GetNorthWestPosition();
	ver.tex_coords = float2(0.0f, 1.0f);
	vertices.push_back(ver);

	uint lastIndex = 0;

	indices.push_back(lastIndex);
	indices.push_back(lastIndex + 1);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 3);
	indices.push_back(lastIndex);

	//----
	glGetError();
	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	CheckOpenGlError("glBindBuffer vertices");
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CanvasVertex), &vertices[0], GL_STATIC_DRAW);
	CheckOpenGlError("glBufferData vertices");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CheckOpenGlError("glBindBuffer vertices 0");


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	CheckOpenGlError("glBindBuffer indices");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
	CheckOpenGlError("glBufferData indices");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CheckOpenGlError("glBindBuffer indices 0");

}

void CompCanvasRender::ProcessQuad(const std::vector<float2>& ver_quad, const std::vector<float2>& uv_coord)
{
	//UV Setup
	//-x+y	 //0,1-------1,1 //+x+y
			//|	3     /	2|
			//|		/	 |
			//|	0 /		1|
	//-x-y	//0,0-------1,0 //+x-y
	vertices.clear();
	indices.clear();
	CanvasVertex ver;

	ver.position = float3(ver_quad[0],0);
	ver.tex_coords = uv_coord[0];
	vertices.push_back(ver);

	ver.position = float3(ver_quad[1], 0);
	ver.tex_coords = uv_coord[1];
	vertices.push_back(ver);

	ver.position = float3(ver_quad[2], 0);
	ver.tex_coords = uv_coord[2];
	vertices.push_back(ver);

	ver.position = float3(ver_quad[3], 0);
	ver.tex_coords = uv_coord[3];
	vertices.push_back(ver);




	uint lastIndex = 0;

	indices.push_back(lastIndex);
	indices.push_back(lastIndex + 1);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 3);
	indices.push_back(lastIndex);

	//----
	glGetError();
	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	CheckOpenGlError("glBindBuffer vertices");
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CanvasVertex), &vertices[0], GL_STATIC_DRAW);
	CheckOpenGlError("glBufferData vertices");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CheckOpenGlError("glBindBuffer vertices 0");


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	CheckOpenGlError("glBindBuffer indices");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
	CheckOpenGlError("glBufferData indices");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CheckOpenGlError("glBindBuffer indices 0");
}

void CompCanvasRender::PorcessText(CompText * text)
{
	graphic = text;

}

void CompCanvasRender::DrawGraphic()
{
	if (graphic == nullptr)
	{
		return;
	}


	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	




	GLint g_AttribLocationColor = glGetUniformLocation(App->render_gui->default_ui_shader->programID, "Color_UI_ME");
	GLint modelLoc = glGetUniformLocation(App->render_gui->default_ui_shader->programID, "model");
	GLint g_AttribLocationTexture = glGetUniformLocation(App->render_gui->default_ui_shader->programID, "Texture");

	CompRectTransform* transform = (CompRectTransform*)parent->FindComponentByType(C_RECT_TRANSFORM);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)&transform->GetGlobalTransform().Transposed());
	//glUniform1i(g_AttribLocationTexture, graphic->GetTextureID());
	glUniform4f(g_AttribLocationColor,graphic->GetColor().x, graphic->GetColor().y, graphic->GetColor().z, graphic->GetColor().w);
	glGetError();

	glActiveTexture(GL_TEXTURE0);
	if (graphic->GetTextureID() != 0)
	{
		glBindTexture(GL_TEXTURE_2D, graphic->GetTextureID());
		CheckOpenGlError("glBindTexture color");
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->vertices_id);
	CheckOpenGlError("glBindBuffer vertices_id");

	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char *)NULL + (0 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char *)NULL + (3 * sizeof(float)));
	
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CheckOpenGlError("glBindBuffer vertices_id 0");

	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indices_id);
	CheckOpenGlError("glBindBuffer indices_id");
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	CheckOpenGlError("glDrawElements indices");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CheckOpenGlError("glBindBuffer indices_id 0 ");

	//Reset TextureColor
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);



}

void CompCanvasRender::SetGraphic(CompGraphic * set_graphic)
{
	graphic = set_graphic;
}

void CompCanvasRender::CheckOpenGlError(std::string info)
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		LOG("[error]Error %s, %s\n", info.c_str(),gluErrorString(error));
	}
}