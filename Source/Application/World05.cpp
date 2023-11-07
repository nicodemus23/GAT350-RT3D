#include "World05.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/color_space.hpp>


namespace nc
{
	bool World05::Initialize()

	{
		m_scene = std::make_unique<Scene>();
		m_scene->Load("Scenes/scene.json");
		m_scene->Initialize();

		{
			auto actor = CREATE_CLASS(Actor);
			actor->name = "light1";
			actor->transform.position = glm::vec3{ 3, 3, 3 };
			auto lightComponent = CREATE_CLASS(LightComponent);
			lightComponent->type = LightComponent::eType::Point;
			//lightComponent->color = glm::rgbColor(glm::vec3{ randomf() * 360, 1, 1 });
			lightComponent->color = (glm::vec3 { 1, 1, 1 });
			lightComponent->intensity = 1;
			lightComponent->range = 20;
			lightComponent->innerAngle = 10.0f;
			lightComponent->outerAngle = 30.0f;
			actor->AddComponent(std::move(lightComponent));
			m_scene->Add(std::move(actor));
		}

		{
			auto actor = CREATE_CLASS(Actor);
			actor->name = "camera1";
			actor->transform.position = glm::vec3{ 0, 0, 18 };
			actor->transform.rotation = glm::radians(glm::vec3{ 0, 180, 0 });

			auto cameraComponent = CREATE_CLASS(CameraComponent);
			cameraComponent->SetPerspective(70.0f, ENGINE.GetSystem<Renderer>()->GetWidth() / (float)ENGINE.GetSystem<Renderer>()->GetHeight(), 0.1f, 100.0f);
			actor->AddComponent(std::move(cameraComponent));

			auto cameraController = CREATE_CLASS(CameraController);
			cameraController->speed = 5;
			cameraController->sensitivity = 0.5f;
			cameraController->m_owner = actor.get();
			cameraController->Initialize();
			actor->AddComponent(std::move(cameraController));

			m_scene->Add(std::move(actor));
		}

		return true;
	};

		
	void World05::Shutdown()
	{
	}

	void World05::Update(float dt)
	{
		ENGINE.GetSystem<Gui>()->BeginFrame();

		m_scene->Update(dt);
		m_scene->ProcessGui();

		//m_transform.rotation.z += 0 * dt;

		auto actor = m_scene->GetActorByName<Actor>("actor1");
		//actor = m_scene->GetActorByName<Actor>("actor2");
		//actor = m_scene->GetActorByName<Actor>("actor3");

		//actor->transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_A) ? m_speed * -dt : 0;
		//actor->transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_D) ? m_speed * +dt : 0;
		//actor->transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_W) ? m_speed * -dt : 0; // going into screen 
		//actor->transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_S) ? m_speed * +dt : 0; // coming out of screen

		// Get material for Actor1
		auto material = actor->GetComponent<ModelComponent>()->material;
		material->ProcessGui();
		material->Bind();

		auto program = material->GetProgram();
		if (!material) {
			std::cerr << "Error: Material is null." << std::endl;
			return;
		}


		material = GET_RESOURCE(Material, "materials/refraction.mtrl");
		if (material)
		{
			ImGui::Begin("Refraction");

			//m_refraction = 1.0 + std::fabs(std::sin(m_time));
			m_refraction = 1.9;
			ImGui::DragFloat("IOR", &m_refraction, 0.01f, 1, 3); 
			auto program = material->GetProgram();
			program->SetUniform("ior", m_refraction);
			ImGui::End();
		}

		material->GetProgram()->SetUniform("ambientLight", m_ambientColor);
		material->GetProgram()->SetUniform("ambientIntensity", m_ambientIntensity);

		m_time += dt;

		ENGINE.GetSystem<Gui>()->EndFrame();

	}

	void World05::Draw(Renderer& renderer)
	{
		// pre-render
		renderer.BeginFrame();

		// render
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_scene->Draw(renderer); 

		ENGINE.GetSystem<Gui>()->Draw();

		// post-render
		renderer.EndFrame();
	}
}
