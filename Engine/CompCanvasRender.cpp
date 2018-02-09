#include "CompCanvasRender.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompImage.h"
#include "CompText.h"
#include "CompRectTransform.h"
CompCanvasRender::CompCanvasRender(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Canvas Render";

	glGenBuffers(1, &vertices_id);
	glGenBuffers(1, &indices_id);
}

CompCanvasRender::CompCanvasRender(const CompCanvasRender & copy, GameObject * parent) : Component(Comp_Type::C_CANVAS_RENDER, parent)
{
}

CompCanvasRender::~CompCanvasRender()
{
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
		ImGui::OpenPopup("OptionsMesh");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsMesh"))
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

void CompCanvasRender::ProcessImage(CompImage * image)
{
	graphic = image;
	//UV Setup
	//0,1-------1,1
	//|	3     /	2|
	//|		/	 |
	//|	0 /		1|
	//0,0-------1,0
	vertices.clear();
	indices.clear();


		CanvasVertex ver;
		ver.position = parent->GetComponentRectTransform()->GetSouthWestPosition();
		ver.tex_coords=float2(0.0f,0.0f);
		vertices.push_back(ver);

		ver.position = parent->GetComponentRectTransform()->GetSouthWestPosition();
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
		indices.push_back(lastIndex );

		//----
		glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CanvasVertex), &vertices[0], GL_STATIC_DRAW);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	glPushMatrix();
	glMultMatrixf((float*)&graphic->GetRectTrasnform()->GetGlobalTransform().Transposed());

	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (graphic->GetTextureID() != -1)
	{
		glBindTexture(GL_TEXTURE_2D, graphic->GetTextureID());
		//	glColor4f(image->GetImage()->GetRGBA().x, image->GetImage()->GetRGBA().y, image->GetImage()->GetRGBA().z, image->GetImage()->GetRGBA().w);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	glVertexPointer(3, GL_FLOAT, sizeof(CanvasVertex), NULL);
	//glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normals));
	glTexCoordPointer(2, GL_FLOAT, sizeof(CanvasVertex), (void*)offsetof(CanvasVertex, tex_coords));
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

}
