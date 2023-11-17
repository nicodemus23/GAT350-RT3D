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
		m_scene->Load("Scenes/scene_lpn_shadow.json");
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
		// get all of the materials 
		auto materials = GET_RESOURCES(Material);
		for (auto material : materials)
		{	
			material->depthTexture = texture;
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
		{	// if this modelcomponent has a shadow...
			if (model->castShadow)
			{	
				//glCullFace(GL_FRONT); // using the backface to use for sampling for the zbuffer 
				//... go ahead and draw it
				program->SetUniform("model", model->m_owner->transform.GetMatrix());
				model->model->Draw(); // model component->actual model with vertices->Draw();
			}
			
		}

		// unbind when you want to render to the screen
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
