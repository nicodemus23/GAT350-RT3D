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

		// create depth texture 
		auto texture = std::make_shared<Texture>();
		texture->CreateDepthTexture(1024, 1024);
		ADD_RESOURCE("depth_texture", texture);

		// create depth buffer 
		auto framebuffer = std::make_shared<Framebuffer>();
		framebuffer->CreateDepthBuffer(texture);
		ADD_RESOURCE("depth_buffer", framebuffer);

		//framebuffer = GET_RESOURCE(Framebuffer, "depth_buffer");

		// set depth texture to debug
		auto material = GET_RESOURCE(Material, "Materials/sprite.mtrl"); 
		if (material)
		{
			material->albedoTexture = texture;
		}

		/*{
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
		}*/

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

		auto framebuffer = GET_RESOURCE(Framebuffer, "depth_buffer");
		renderer.SetViewport(framebuffer->GetSize().x, framebuffer->GetSize().y);
		framebuffer->Bind();

		renderer.ClearDepth();
		auto program = GET_RESOURCE(Program, "shaders/shadow_depth.prog");
		program->Use(); // use shadow_depth as the current program 

		auto lights = m_scene->GetComponents<LightComponent>();
		for (auto light : lights)
		{	//
			if (light->castShadow)
			{
				glm::mat4 shadowMatrix = light->GetShadowMatrix();
				program->SetUniform("shadowVP", shadowMatrix);
			}
		}
		// get all models in the scene
		auto models = m_scene->GetComponents<ModelComponent>();
		for (auto model : models)
		{
			program->SetUniform("model", model->m_owner->transform.GetMatrix());
			model->model->Draw(); // model component->actual model with vertices->Draw();
		}

		framebuffer->Unbind();

		// *** PASS 2 ***
		renderer.ResetViewport();
		renderer.BeginFrame();
		m_scene->Draw(renderer);

		// post-render
		ENGINE.GetSystem<Gui>()->Draw();
		renderer.EndFrame();
	}
}
