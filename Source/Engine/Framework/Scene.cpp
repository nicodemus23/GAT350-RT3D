#include "Scene.h"
#include "Framework/Components/CollisionComponent.h"
#include "Framework/Components/LightComponent.h"
#include "Framework/Components/CameraComponent.h"

namespace nc
{
	bool Scene::Initialize()
	{
		for (auto& actor : m_actors) actor->Initialize();

		return true;
	}

	void Scene::Update(float dt)
	{
		// update and remove destroyed actors
		auto iter = m_actors.begin();
		while (iter != m_actors.end())
		{
			if ((*iter)->active) (*iter)->Update(dt);
			((*iter)->destroyed) ? iter = m_actors.erase(iter) : iter++;
		}
	}

	void Scene::Draw(Renderer& renderer)
	{
		// get light components
		std::vector<LightComponent*> lights;
		for (auto& actor : m_actors) // go through all actors
		{
			if (!actor->active) continue; // skip the rest if not actor 

			auto component = actor->GetComponent<LightComponent>();
			if (component)
			{
				lights.push_back(component);
			}
		}

		// get camera component
		CameraComponent* camera = nullptr;
		for (auto& actor : m_actors)
		{
			if (!actor->active) continue;

			camera = actor->GetComponent<CameraComponent>();
			if (camera != nullptr) // if component is valid... 
			{
				break; // ... break out of for loop 
			}
		}
		// get all shader programs in the resource system
		auto programs = ResourceManager::Instance().GetAllOfType<Program>();
		// set all shader programs camera and lights uniforms
		for (auto& program : programs)
		{	// current program
			program->Use();

			// set camera in shader program 
			if (camera) camera->SetProgram(program);

			// set lights in shader program
			int index = 0;
			for (auto light : lights)
			{
				std::string name = "lights[" + std::to_string(index++) + "]";


				light->SetProgram(program, name);
			}



			program->SetUniform("numLights", index);
			program->SetUniform("ambientLight", ambientColor);
		}



		for (auto& actor : m_actors)
		{
			if (actor->active) actor->Draw(renderer);
		}
	}

	void Scene::Add(std::unique_ptr<Actor> actor)
	{
		actor->m_scene = this;
		actor->m_game = m_game;
		m_actors.push_back(std::move(actor));
	}

	void Scene::RemoveAll(bool force)
	{
		auto iter = m_actors.begin();
		while (iter != m_actors.end())
		{
			(force || !(*iter)->persistent) ? iter = m_actors.erase(iter) : iter++;
		}
	}

	bool Scene::Load(const std::string& filename)
	{
		rapidjson::Document document;
		if (!Json::Load(filename, document))
		{
			ERROR_LOG("Could not load scene file: " << filename);
			return false;
		}

		Read(document);

		return true;
	}

	void Scene::Read(const json_t& value)
	{
		if (HAS_DATA(value, actors) && GET_DATA(value, actors).IsArray())
		{
			for (auto& actorValue : GET_DATA(value, actors).GetArray())
			{
				std::string type;
				READ_DATA(actorValue, type);

				auto actor = CREATE_CLASS_BASE(Actor, type);
				actor->Read(actorValue);

				if (actor->prototype)
				{
					std::string name = actor->name;
					Factory::Instance().RegisterPrototype(name, std::move(actor));
				}
				else
				{
					Add(std::move(actor));
				}
			}
		}

	}

	void Scene::ProcessGui()
	{
		ImGui::Begin("Scene");
		ImGui::ColorEdit3("Ambient", glm::value_ptr(ambientColor));
		ImGui::Separator();



		for (auto& actor : m_actors)
		{
			if (ImGui::Selectable(actor->name.c_str(), actor->guiSelect))
			{	// lambda function - search all objects in m_actors list from beginning to end - auto& a is first actor - for each actor that isn't selected in gui set to "false" - set selected to "true"
				std::for_each(m_actors.begin(), m_actors.end(), [](auto& a) { a->guiSelect = false; });
				actor->guiSelect = true;
			}
		}
		ImGui::End();


		// uses Selectable lambda function result from above - iterator is pointer to the object 
		ImGui::Begin("Inspector");
		auto iter = std::find_if(m_actors.begin(), m_actors.end(), [](auto& a) { return a->guiSelect; });
		if (iter != m_actors.end())
		{
			(*iter)->ProcessGui();
		}
		ImGui::End();
	}

}