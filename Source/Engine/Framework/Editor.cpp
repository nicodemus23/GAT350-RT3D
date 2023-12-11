#include "Editor.h"
#include "Scene.h";
#include "Framework/Components/CameraComponent.h";
#include "Framework/Components/LightComponent.h";


namespace nc
{
	void Editor::Update()
	{
		if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) m_active = !m_active;

	}
	
	void nc::Editor::ProcessGui(Scene* scene)
	{
		if (!m_active) return;

		// show resources
		ImGui::Begin("Resources");



		// models
		if (ImGui::CollapsingHeader("Models"))
		{
			auto models = GET_RESOURCES(Model); // grab all of the resources in the scene 
			for (auto& model : models) // & so it doesn't make a copy 
			{	// if selected ...
				if (ImGui::Selectable(model->name.c_str(), model.get() == m_selected)) // get raw pointer to resource (.get) and if it's equal to m_selected
				{
					m_selected = model.get();
				}
			}
		}

		// textures
		if (ImGui::CollapsingHeader("Textures"))
		{
			auto textures = GET_RESOURCES(Texture); // grab all of the resources in the scene 
			for (auto& texture : textures) // & so it doesn't make a copy 
			{	// if selected ...
				if (ImGui::Selectable(texture->name.c_str(), texture.get() == m_selected)) // get raw pointer to resource (.get) and if it's equal to m_selected
				{
					m_selected = texture.get();
				}
			}
		}

		// materials
		if (ImGui::CollapsingHeader("Materials"))
		{
			auto materials = GET_RESOURCES(Material); // grab all of the resources in the scene 
			for (auto& material : materials) // & so it doesn't make a copy 
			{	// if selected ...
				if (ImGui::Selectable(material->name.c_str(), material.get() == m_selected)) // get raw pointer to resource (.get) and if it's equal to m_selected
				{
					m_selected = material.get();
				}
			}
		}

		// shaders
		if (ImGui::CollapsingHeader("Shaders"))
		{
			auto shaders = GET_RESOURCES(Shader); // grab all of the resources in the scene 
			for (auto& shader : shaders) // & so it doesn't make a copy 
			{	// if selected ...
				if (ImGui::Selectable(shader->name.c_str(), shader.get() == m_selected)) // get raw pointer to resource (.get) and if it's equal to m_selected
				{
					m_selected = shader.get();
				}
			}
		}

		// lights
		if (ImGui::CollapsingHeader("Lights"))
		{
			for (auto& actor : scene->m_actors)
			{
				// Check if the actor has a LightComponent
				LightComponent* lightComp = actor->GetComponent<LightComponent>();
				if (lightComp)
				{
					// Use the actor's name for listing
					if (ImGui::Selectable(actor->name.c_str(), actor.get() == m_selected))
					{
						m_selected = actor.get(); // Select the actor
					}
				}
			}
		}

		


		

		ImGui::End();
		// show scene
		ImGui::Begin("Scene");

		scene->ProcessGui();
		ImGui::Separator();


		// show actors 
		ImGui::BeginChild("Actors");
		if (ImGui::BeginPopupContextWindow())
		{
			auto cameras = scene->GetComponents<CameraComponent>();
			auto camera = (!cameras.empty()) ? cameras[0] : nullptr;



			if (ImGui::MenuItem("Create empty")) {}
			{	// creates empty actor
				auto actor = CREATE_CLASS(Actor);
				actor->name = CreateUnique(actor->GetClassName());
				actor->Initialize();
				m_selected = actor.get(); 
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Sphere")) {}
			{	// creates empty actor
				auto actor = CREATE_CLASS_BASE(Actor, "Sphere");
				actor->name = CreateUnique(actor->name);
				if (camera)
				{
					actor->transform.position = camera->m_owner->transform.position + camera->m_owner->transform.Forward() * 3.0f;
				}
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Cube")) {}
			{	// creates empty actor
				auto actor = CREATE_CLASS_BASE(Actor, "Cube");
				actor->name = CreateUnique(actor->name);
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Camera")) {}
			{	// creates empty actor
				auto actor = CREATE_CLASS_BASE(Actor, "Camera");
				actor->name = CreateUnique(actor->name);
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
			if (ImGui::MenuItem("Light")) {}
			{	// creates empty actor
				auto actor = CREATE_CLASS_BASE(Actor, "Light");
				actor->name = CreateUnique(actor->name);
				actor->Initialize();
				m_selected = actor.get();
				scene->Add(std::move(actor));
			}
		}
		for (auto& actor : scene->m_actors)
		{
			if (ImGui::Selectable(actor->name.c_str(), actor.get() == m_selected))
			{
				m_selected = actor.get();
			}
		}
		ImGui::EndChild();
		ImGui::End();

		// show inspector
		ImGui::Begin("Inspector");
		if (m_selected)
		{
			m_selected->ProcessGui();
			// delete selected actor
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				// see if something is an actor
				auto actor = dynamic_cast<Actor*>(m_selected);
				if (actor)
				{
					scene->Remove(actor);
					m_selected = nullptr;
				}
			}
		}
		ImGui::End();

	}
}
