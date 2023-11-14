#include "World07.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/color_space.hpp>


namespace nc
{
	bool World07::Initialize()

	{
		m_scene = std::make_unique<Scene>();
		m_scene->Load("Scenes/scene_shadow.json");
		m_scene->Initialize();

		auto texture = std::make_shared<Texture>();
		texture->CreateTexture(1024, 1024);
		ADD_RESOURCE("fb_texture", texture);

		auto framebuffer = std::make_shared<Framebuffer>();
		framebuffer->CreateFramebuffer(texture);
		ADD_RESOURCE("fb", framebuffer);

		auto material = GET_RESOURCE(Material, "Materials/postprocess.mtrl"); // use postprocess material to hold ...
		if (material)
		{
			material->albedoTexture = texture;
		}


		//{
		//	auto actor = CREATE_CLASS(Actor);
		//	actor->name = "light1";
		//	actor->transform.position = glm::vec3{ 3, 3, 3 };
		//	auto lightComponent = CREATE_CLASS(LightComponent);
		//	lightComponent->type = LightComponent::eType::Point;
		//	//lightComponent->color = glm::rgbColor(glm::vec3{ randomf() * 360, 1, 1 });
		//	lightComponent->color = (glm::vec3 { 1, 1, 1 });
		//	lightComponent->intensity = 1;
		//	lightComponent->range = 20;
		//	lightComponent->innerAngle = 10.0f;
		//	lightComponent->outerAngle = 30.0f;
		//	actor->AddComponent(std::move(lightComponent));
		//	m_scene->Add(std::move(actor));
		//}


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


	void World07::Shutdown()
	{
	}

	void World07::Update(float dt)
	{
		m_time += dt;

		ENGINE.GetSystem<Gui>()->BeginFrame();

		m_scene->Update(dt);
		m_scene->ProcessGui();		

		ENGINE.GetSystem<Gui>()->EndFrame();

	}

	void World07::Draw(Renderer& renderer)
	{
		//// *** PASS 1 *** 
		//m_scene->GetActorByName("postprocess")->active = false;

		//auto framebuffer = GET_RESOURCE(Framebuffer, "fb");
		//renderer.SetViewport(framebuffer->GetSize().x, framebuffer->GetSize().y);
		//framebuffer->Bind();

		//renderer.BeginFrame(glm::vec3{ 0.0f, 0, 0.0f });
		//m_scene->Draw(renderer);

		//framebuffer->Unbind();

		//m_scene->GetActorByName("postprocess")->active = true;
		//auto actor = m_scene->GetActorByName("cube");
		//actor->active = true;

		// *** PASS 2 ***
		renderer.ResetViewport();
		renderer.BeginFrame();
		m_scene->Draw(renderer);

		// post-render
		ENGINE.GetSystem<Gui>()->Draw();
		renderer.EndFrame();
	}
}
