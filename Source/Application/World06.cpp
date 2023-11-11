#include "World06.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/color_space.hpp>


namespace nc
{
	bool World06::Initialize()

	{
		m_scene = std::make_unique<Scene>();
		m_scene->Load("Scenes/scene_framebuffer.json");
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

		
	void World06::Shutdown()
	{
	}

	void World06::Update(float dt)
	{
		m_time += dt;

		ENGINE.GetSystem<Gui>()->BeginFrame();

		m_scene->Update(dt);
		m_scene->ProcessGui();

		// set postprocess gui
		ImGui::Begin("Post-Process");
		ImGui::SliderFloat("Blend", &m_blend, 0, 1);
		bool effect = m_params & INVERT_MASK;
		if (ImGui::Checkbox("Invert", &effect))
		{	// bit operations:
			// 0001 - mask
			// 0000 - params
			// -------------
			// 0001
			// |= is "OR ="
			if (effect) m_params |= INVERT_MASK;
			else m_params ^= INVERT_MASK; // ^= (exclusive or) since there is no 
		}
		effect = m_params & GRAYSCALE_MASK;
		if (ImGui::Checkbox("Grayscale", &effect))
		{	// bit operations:
			// 0001 - mask
			// 0000 - params
			// 0001
			// |= is "OR ="
			if (effect) m_params |= GRAYSCALE_MASK;
			else m_params ^= GRAYSCALE_MASK; // ^= (exclusive or) since there is no 
		}

		ImGui::End();

		// set post process shader
		auto program = GET_RESOURCE(Program, "shaders/postprocess.prog");
		if (program)
		{
			program->Use(); // this is the shader we're gonna use...
			program->SetUniform("blend", m_blend);
			program->SetUniform("params", m_params);
		}

		//auto actor = m_scene->GetActorByName<Actor>("actor1");
		//
		//// Get material for Actor1
		//auto material = actor->GetComponent<ModelComponent>()->material;
		//material->ProcessGui();
		//material->Bind();

		//auto program = material->GetProgram();
		//if (!material) {
		//	std::cerr << "Error: Material is null." << std::endl;
		//	return;
		//}


		//material = GET_RESOURCE(Material, "materials/refraction.mtrl");
		//if (material)
		//{
		//	ImGui::Begin("Refraction");

		//	//m_refraction = 1.0 + std::fabs(std::sin(m_time));
		//	m_refraction = 1.9;
		//	ImGui::DragFloat("IOR", &m_refraction, 0.01f, 1, 3); 
		//	auto program = material->GetProgram();
		//	program->SetUniform("ior", m_refraction);
		//	ImGui::End();
		//}

		//material->GetProgram()->SetUniform("ambientLight", m_ambientColor);

		//material->GetProgram()->SetUniform("ambientIntensity", m_ambientIntensity);

		ENGINE.GetSystem<Gui>()->EndFrame();

	}

	void World06::Draw(Renderer& renderer)
	{
		// *** PASS 1 *** 
		m_scene->GetActorByName("postprocess")->active = false;

		auto framebuffer = GET_RESOURCE(Framebuffer, "fb");
		renderer.SetViewport(framebuffer->GetSize().x, framebuffer->GetSize().y); 
		framebuffer->Bind();

		renderer.BeginFrame(glm::vec3{ 0.0f, 0, 0.0f });
		m_scene->Draw(renderer); 

		framebuffer->Unbind();

		m_scene->GetActorByName("postprocess")->active = true;
		//auto actor = m_scene->GetActorByName("cube");
		//actor->active = true;

		// *** PASS 2 ***
		renderer.ResetViewport();
		renderer.BeginFrame();
		m_scene->GetActorByName("postprocess")->Draw(renderer);


		// post-render
		ENGINE.GetSystem<Gui>()->Draw();
		renderer.EndFrame();
	}
}
