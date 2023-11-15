#pragma once
#include "Actor.h"
#include <list>

namespace nc
{
	class Renderer;

	// Manages Actors in the scene which can be loaded and read from a JSON file.
	class Scene
	{
	public:
		Scene() = default;

		bool Initialize();
		void Update(float dt);
		void Draw(Renderer& renderer);

		void Add(std::unique_ptr<Actor> actor);
		void RemoveAll(bool force = false);

		bool Load(const std::string& filename);
		void Read(const json_t& value);

		template<typename T>
		T* GetActor();
		template<typename T = Actor>
		T* GetActorByName(const std::string& name);
		template<typename T>
		std::vector<T*> GetComponents();

		void SetGame(World* game) { m_game = game; }
		void ProcessGui();
		friend class Actor;

	public:
		glm::vec3 ambientColor{ 0.2f };
		float ambientIntensity{ 1.0f };

	private:
		World* m_game = nullptr;
		std::list<std::unique_ptr<Actor>> m_actors;
	};

	template<typename T>
	inline T* Scene::GetActor()
	{
		for (auto& actor : m_actors)
		{
			T* result = dynamic_cast<T*>(actor.get());
			if (result) return result;
		}

		return nullptr;
	}

	template<typename T>
	inline T* Scene::GetActorByName(const std::string& name)
	{
		for (auto& actor : m_actors)
		{
			if (actor->name == name)
			{
				T* result = dynamic_cast<T*>(actor.get());
				if (result) return result;
			}
		}

		return nullptr;
	}

	template<typename T>
	inline std::vector<T*> Scene::GetComponents()
	{

		std::vector<T*> components;
		// go through actors in scene
		for (auto& actor : m_actors)
		{	// if actor isn't active, just continue to the next 
			if (!actor->active) continue;

			auto component = actor->GetComponent<T>();
			// if there's a component, add to vector of components
			if (component)
			{
				components.push_back(component);
			}
		}

		return components;
	}


}