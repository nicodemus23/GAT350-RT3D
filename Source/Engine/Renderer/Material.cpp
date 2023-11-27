#include "Material.h"
#include "Program.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Core/Core.h"
#include <imgui/ImFileDialog.h>

namespace nc
{
	bool Material::Create(std::string filename, ...)
	{
		// load program json document
		rapidjson::Document document;
		bool success = Json::Load(filename, document);
		if (!success)
		{
			INFO_LOG("Could not load program file (%s)." << filename);
			return false;
		}

		// read the program name
		std::string program;
		READ_DATA(document, program);
		// get program resource
		m_program = GET_RESOURCE(Program, program);

		// read the textures name
		std::string albedoTextureName;
		if (READ_NAME_DATA(document, "albedoTexture", albedoTextureName))
		{
			albedoTexture = GET_RESOURCE(Texture, albedoTextureName);
		}

		std::string specularTextureName;
		if (READ_NAME_DATA(document, "specularTexture", specularTextureName))
		{
			specularTexture = GET_RESOURCE(Texture, specularTextureName);
		}

		std::string emissiveTextureName;
		if (READ_NAME_DATA(document, "emissiveTexture", emissiveTextureName))
		{

			emissiveTexture = GET_RESOURCE(Texture, emissiveTextureName);
		}

		std::string normalTextureName;
		if (READ_NAME_DATA(document, "normalTexture", normalTextureName))
		{

			normalTexture = GET_RESOURCE(Texture, normalTextureName);
		}

		std::string cubemapName;
		if (READ_NAME_DATA(document, "cubemap", cubemapName))
		{
			params |= CUBEMAP_TEXTURE_MASK;
			std::vector<std::string> cubemaps;
			READ_DATA(document, cubemaps);
			cubemapTexture = GET_RESOURCE(Cubemap, cubemapName, cubemaps);
		}

		READ_DATA(document, albedo);
		READ_DATA(document, specular);
		READ_DATA(document, shininess);
	//	READ_DATA(document, reflectionIntensity);
		READ_DATA(document, emissive);
		READ_DATA(document, tiling);
		READ_DATA(document, offset);

		return true;
	}

	void Material::Bind()
	{
		m_program->Use();
		m_program->SetUniform("material.params", params);
		m_program->SetUniform("material.albedo", albedo);
		m_program->SetUniform("material.specular", specular);
		m_program->SetUniform("material.shininess", shininess);
	//	m_program->SetUniform("material.reflectionIntensity", reflectionIntensity);
		m_program->SetUniform("material.emissive", emissive);
		m_program->SetUniform("material.tiling", tiling);
		m_program->SetUniform("material.offset", offset);
		m_program->SetUniform("material.normalMapIntensity", normalMapIntensity);
		m_program->SetUniform("material.normalMapContrast", normalMapContrast);

		if (albedoTexture)
		{
			params |= ALBEDO_TEXTURE_MASK;
			albedoTexture->SetActive(GL_TEXTURE0);
			albedoTexture->Bind();
		}

		if (specularTexture)
		{
			params |= SPECULAR_TEXTURE_MASK;
			specularTexture->SetActive(GL_TEXTURE1);
			specularTexture->Bind();
		}

		if (normalTexture)
		{
			params |= NORMAL_TEXTURE_MASK;
			normalTexture->SetActive(GL_TEXTURE2);
			normalTexture->Bind();
		}

		if (emissiveTexture)
		{
			params |= EMISSIVE_TEXTURE_MASK;
			emissiveTexture->SetActive(GL_TEXTURE3);
			emissiveTexture->Bind();
		}

		if (cubemapTexture)
		{			params |= NORMAL_TEXTURE_MASK;

			cubemapTexture->SetActive(GL_TEXTURE4);
			cubemapTexture->Bind();
		}
		if (depthTexture)
		{
			depthTexture->SetActive(GL_TEXTURE5);
			depthTexture->Bind();
		}
	}
	void Material::ProcessGui()
	{
		ImGui::TextColored(ImVec4{ 0, 1, 0, 1 }, "Name: %s", name.c_str());

		// shader
		ImGui::Text("Shader:");
		ImGui::SameLine();
		ImGui::Text("%s", m_program->name.c_str());
		Gui::GetDialogueResource<Program>(m_program, "ShaderTextureDialog", "Open Shader", "Shader file (*.prog){.prog},.*");

		// albedo
		ImGui::Text("Albedo  ");
		ImGui::SameLine();
		ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo), ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);

		(albedoTexture) ? ImGui::Text("%s", albedoTexture->name.c_str()) : ImGui::Text("None");
		Gui::GetDialogueResource<Texture>(albedoTexture, "AlbedoTextureDialog", "Open texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");

		// specular
		ImGui::Text("Specular  ");
		ImGui::SameLine();
		ImGui::ColorEdit3("Specular", glm::value_ptr(specular), ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
		(specularTexture) ? ImGui::Text("%s", specularTexture->name.c_str()) : ImGui::Text("None");
		Gui::GetDialogueResource<Texture>(specularTexture, "SpecularTextureDialog", "Open texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");



		ImGui::DragFloat("Shininess", &shininess, 0.1f, 2.0f, 200.0f);
		
		// KEEP FOR WHEN I BRING BACK PHONG REFL SHADER 
		//ImGui::DragFloat("Reflection Intensity", &reflectionIntensity, 0.1f, 0.0f, 1.0f);

		ImGui::Text("Emissive  ");
		ImGui::SameLine();
		ImGui::ColorEdit3("Emissive", glm::value_ptr(emissive), ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
		(emissiveTexture) ? ImGui::Text("%s", emissiveTexture->name.c_str()) : ImGui::Text("None");
		Gui::GetDialogueResource<Texture>(emissiveTexture, "EmissiveTextureDialog", "Open texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");



		ImGui::TextColored(ImVec4{ 0, 1, 0, 1 }, "Name: %s", name.c_str());
		// normal
		ImGui::Text("Normal  ");
		(normalTexture) ? ImGui::Text("%s", normalTexture->name.c_str()) : ImGui::Text("None");
		Gui::GetDialogueResource<Texture>(normalTexture, "NormalTextureDialog", "Open texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");


		ImGui::DragFloat("Normal Map Intensity Mult.", &normalMapIntensity, 1.0f, 0.0f,  2.0f);
		ImGui::DragFloat("Normal Map Contrast", &normalMapContrast, 1.0f, 0.0f,  2.0f);

		
		// uv
		ImGui::DragFloat2("Tiling", glm::value_ptr(tiling), 0.1f);
		ImGui::DragFloat2("Offset", glm::value_ptr(offset), 0.1f);



	}
}