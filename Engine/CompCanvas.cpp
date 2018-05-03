#include "CompCanvas.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "JSONSerialization.h"

#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompCanvasRender.h"
#include "EventDef.h"
#include "ModuleEventSystem.h"
#include "ModuleRenderGui.h"
#include "ModuleWindow.h"
#include "CompRectTransform.h"
#include "DefaultShaders.h"
#include"ModuleShaders.h"
#include "ModuleFramebuffers.h"
#include"ShadersLib.h"
CompCanvas::CompCanvas(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "CompCanvas";
	my_transform = (CompRectTransform*)parent->FindComponentByType(Comp_Type::C_RECT_TRANSFORM);
	//UpdateCanvasScale();

	/*
	draw_mode.type = EventType::EVENT_DRAW;
	draw_mode.draw.Dtype = draw_mode.draw.DRAW_2D;
	draw_mode.draw.ToDraw = this;
	draw_mode.draw.DistanceCamToObject = 0.0f;
	*/
}

CompCanvas::CompCanvas(const CompCanvas & copy, GameObject * parent) :Component(Comp_Type::C_CANVAS, parent)
{
}

CompCanvas::~CompCanvas()
{
}
void CompCanvas::PreUpdate(float dt)
{

	//UpdateCanvasScale();
}
void CompCanvas::Update(float dt)
{
	App->render_gui->screen_space_canvas.push_back(this);
	//PushEvent(draw_mode);
	if (my_transform != nullptr)
	{
		if (my_transform->GetUpdateRect())
		{
			Resize(App->window->GetWidth(), App->window->GetHeight());
			my_transform->SetUpdateRect(false);
		}
	}
}

void CompCanvas::Clear()
{
	for (int i = 0; i<App->render_gui->screen_space_canvas.size(); i++)
	{
		if (App->render_gui->screen_space_canvas[i] == this)
		{
			App->render_gui->screen_space_canvas.erase(App->render_gui->screen_space_canvas.begin() + i);
		}
	}
	//RELEASE(default_ui_shader);

}

void CompCanvas::ShowOptions()
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
			CopyValues(((CompCanvas*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompCanvas::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsCanvas");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsCanvas"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}
	if (ImGui::DragFloat("Canvas Alpha##canvas_image", &canvas_alpha,0.01f,0.0f,1.0f))
	{
		CorrectCanvasAlpha();
		bool down = false;
		if (canvas_alpha<=current_canvas_alpha)
		{
			down = true;
		}
		current_canvas_alpha = canvas_alpha;
		SetCanvasAlpha(down);
	}
	ImGui::TreePop();
}

void CompCanvas::CopyValues(const CompCanvas* component)
{
	//more...
}

void CompCanvas::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);


	json_object_dotset_number_with_std(object, name + "Canvas Alpha", current_canvas_alpha);
	//...
}

void CompCanvas::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	default_ui_shader=App->module_shaders->CreateDefaultShader("default shader", UIShaderFrag, UIShaderVert,nullptr,true);
	current_canvas_alpha = json_object_dotget_number_with_std(object, name + "Canvas Alpha");
	canvas_alpha = current_canvas_alpha;
	default_ui_shader = App->module_shaders->CreateDefaultShader("default shader", UIShaderFrag, UIShaderVert, nullptr, true);

	//...
	Enable();
}

void CompCanvas::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int);				//UID
	buffer_size += sizeof(float);			//current_canvas_alpha
}

void CompCanvas::SaveBinary(char ** cursor, int position) const
{
	Component::SaveBinary(cursor, position);
	App->json_seria->SaveIntBinary(cursor, uid);
	App->json_seria->SaveFloatBinary(cursor, current_canvas_alpha);

}

void CompCanvas::LoadBinary(char ** cursor)
{
	uid = App->json_seria->LoadIntBinary(cursor);
	default_ui_shader = App->module_shaders->CreateDefaultShader("default shader", UIShaderFrag, UIShaderVert, nullptr, true);
	current_canvas_alpha = App->json_seria->LoadFloatBinary(cursor);
	canvas_alpha = current_canvas_alpha;
	default_ui_shader = App->module_shaders->CreateDefaultShader("default shader", UIShaderFrag, UIShaderVert, nullptr, true);

	//...
	Enable();
}

void CompCanvas::Resize(int width, int height)
{
	float2 resize_factor = my_transform->GenerateResizeFactor(width, height);

	my_transform->ResizeRecursive(resize_factor, true);

	for (uint i = 0; i < graphic_vector.size(); i++)
	{
		graphic_vector[i]->ResizeGenerateMesh();
	}
}



void CompCanvas::SyncComponent(GameObject* sync_parent)
{
	my_transform = (CompRectTransform*)parent->FindComponentByType(Comp_Type::C_RECT_TRANSFORM);

	my_transform->GenerateResizeFactor(App->window->GetWidth(), App->window->GetHeight());
	my_transform->Resize(my_transform->GetResizeFactor(),true);

}
void CompCanvas::UpdateCanvasScale()
{
	ImGuiIO& io = ImGui::GetIO();

	my_transform->SetPos(float3(io.DisplaySize.x*0.5f, io.DisplaySize.y*0.5f, 0.0f));

	// SetUIPosition
	my_transform->SetWidth(io.DisplaySize.x);
	my_transform->SetHeight(io.DisplaySize.y);

}
void CompCanvas::AddGraphic(CompGraphic * to_add)
{
	graphic_vector.push_back(to_add);
}

void CompCanvas::RemoveGraphic(CompGraphic * to_remove)
{
	for (int i = 0; i < graphic_vector.size(); i++)
	{
		if (graphic_vector[i] == to_remove)
		{
			LOG("CanvasRender Removed from Canvas");
			graphic_vector.erase(graphic_vector.begin() + i);
			return;
		}
	}
	LOG("Error:CanvasRender not found on this Canvas");
}

void CompCanvas::DrawDebugRectTransform()
{
	if (!IsActive())
		return;
	if (App->engine_state == EngineState::STOP)
	{
		my_transform->DrawRectTransform();
		for (int i = 0; i < graphic_vector.size(); i++)
		{
			CompGraphic* graphic = graphic_vector[i];
			if (!graphic->IsActive() || !graphic->GetToRender() || !graphic->GetParentActive())
				continue;
			graphic->DrawRectTranforms();
		}
	}
}
void CompCanvas::DrawGraphic(bool debug)
{
	if (!IsActive())
		return;
	ImGuiIO& io = ImGui::GetIO();

	if (io.DisplaySize.x == 0 || io.DisplaySize.y == 0)
		return;

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers, polygon fill.
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_texture_2D = glIsEnabled(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);


	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x,	0.0f,						 0.0f, 0.0f },
	{ 0.0f,						2.0f / io.DisplaySize.y,	 0.0f, 0.0f },
	{ 0.0f,						0.0f,						-1.0f, 0.0f },
	{ -1.0f,						-1.0f,						 0.0f, 1.0f },
	};
	default_ui_shader->Bind();
	GLint g_AttribLocationProjMtx = glGetUniformLocation(default_ui_shader->programID, "ProjMtx");
	if (App->engine_state != EngineState::STOP || debug)
	{
		glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

	}
	else
	{
		Frustum camFrust = App->renderer3D->active_camera->frustum;// App->camera->GetFrustum();
		glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());

	}
	if (App->engine_state == EngineState::STOP)
	{
		for (int i = 0; i < graphic_vector.size(); i++)
		{
			CompGraphic* graphic = graphic_vector[i];
			if (!graphic->IsActive() || !graphic->GetToRender() || !graphic->GetParentActive())
				continue;
			graphic->DrawRectTranforms();
		}
	}
	for (int i = 0; i < graphic_vector.size(); i++)
	{

		CompGraphic* graphic = graphic_vector[i];
		if (!graphic->IsActive() || !graphic->GetToRender() || !graphic->GetParentActive())
			continue;
		GLint g_AttribLocationColor = glGetUniformLocation(default_ui_shader->programID, "Color_UI_ME");
		GLint modelLoc = glGetUniformLocation(default_ui_shader->programID, "model");
		GLint g_AttribLocationTexture = glGetUniformLocation(default_ui_shader->programID, "Texture");
		CompRectTransform* transform = (CompRectTransform*)graphic->GetParent()->FindComponentByType(C_RECT_TRANSFORM);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)&transform->GetGlobalTransform().Transposed());
		glUniform1i(g_AttribLocationTexture, 0);
		glUniform4f(g_AttribLocationColor, graphic->GetColor().x, graphic->GetColor().y, graphic->GetColor().z, graphic->GetColor().w);
		glGetError();
		graphic->DrawGraphic();
		graphic->SetToRender(false);
	}
	//End Draw
	// Restore modified state
	default_ui_shader->Unbind();
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_texture_2D) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT, last_polygon_mode[0]); glPolygonMode(GL_BACK, last_polygon_mode[1]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

void CompCanvas::ClearGraphicVector()
{
	graphic_vector.clear();
}

void CompCanvas::SetDefaultUIShader(ShaderProgram * shader)
{
	default_ui_shader = shader;
}

void CompCanvas::SetDefaultTexture(int texture)
{
	default_texture_id = texture;
}

int CompCanvas::GetDefaultTexture()const
{
	return default_texture_id;
}

void CompCanvas::SetNewCanvasAlphaValue(float value)
{
	canvas_alpha = value;
	CorrectCanvasAlpha();
	bool down = false;
	if (canvas_alpha <= current_canvas_alpha)
	{
		down = true;
	}
	current_canvas_alpha = canvas_alpha;
	SetCanvasAlpha(down);
}

void CompCanvas::CorrectCanvasAlpha()
{
	if (canvas_alpha < 0.0f)
		canvas_alpha = 0.0f;
	else if (canvas_alpha > 1.0f)
		canvas_alpha = 1.0f;
}

void CompCanvas::SetCanvasAlpha(bool alpha_down)
{
	if (!IsActive())
		return;
	for (int i = 0; i < graphic_vector.size(); i++)
	{
		CompGraphic* graphic = graphic_vector[i];
		if (!graphic->IsActive() || !graphic->GetToRender() || !graphic->GetParentActive())
			continue;

		if(alpha_down)
		{

			if (graphic->GetAlpha() > current_canvas_alpha)
				graphic->SetAlpha(current_canvas_alpha);

		}
		else
		{

			if (graphic->GetAlpha()< current_canvas_alpha)
				graphic->SetAlpha(current_canvas_alpha);

		}
	}
}

