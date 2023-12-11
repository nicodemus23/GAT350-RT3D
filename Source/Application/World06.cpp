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
			(effect) ? m_params |= INVERT_MASK : m_params &= ~INVERT_MASK; // ^= (exclusive or) since there is no 
		}
		effect = m_params & GRAYSCALE_MASK;
		if (ImGui::Checkbox("Grayscale", &effect))
		{	// bit operations:
			// 0001 - mask
			// 0000 - params
			// 0001
			// |= is "OR ="
			(effect) ? m_params |= GRAYSCALE_MASK :  m_params &= ~GRAYSCALE_MASK; // ^= (exclusive or) since there is no 
		}

		effect = m_params & COLORTINT_MASK;
		if (ImGui::Checkbox("Tint Or Not to Tint", &effect))
		{
			// toggle bit mask checkbox
			if (effect) m_params |= COLORTINT_MASK; // if effect is true set bit in m_params to on: turn on color tint (mask for tint set to 1 and other bits 0)
			else m_params &= ~COLORTINT_MASK; // if effect is false set bit in m_params to off: turn off color tint (inverse mask color tint mask 0, other bits 1)


		}
		// always display color picker if enabled 
		if (effect)
		{
			(ImGui::ColorEdit3("Tint", &m_colorTint[0]));
		}

		effect = m_params & SCANLINE_MASK;
		if (ImGui::Checkbox("Scanline or ScanlineLESS", &effect))
		{
			(effect) ? m_params |= SCANLINE_MASK : m_params &= ~SCANLINE_MASK;
		}

		if (effect)
		{
			ImGui::SliderFloat("Scanline Frequency", &m_scanlineFrequency, 50.0f, 200.0f);
			ImGui::SliderFloat("Scanline Intensity", &m_scanlineIntensity, 0.0f, 1.0f);
		}

		effect = m_params & GRAIN_MASK;
		if (ImGui::Checkbox("Grain Effect", &effect))
		{
			(effect) ? m_params |= GRAIN_MASK : m_params &= ~GRAIN_MASK;
		}
		if (effect)
		{
			ImGui::SliderFloat("Grain Amount", &m_grainIntensity, 0.0f, 1.0f);
		}

		effect = m_params & BLUR_MASK;
		if (ImGui::Checkbox("Blur 3x3 Effect", &effect))
		{
			(effect) ? m_params |= BLUR_MASK : m_params &= ~BLUR_MASK;
		}

		if (effect)
		{
			ImGui::SliderFloat("Blur Amount", &m_blurIntensity, 0.0f, 1.0f);
		}

		effect = m_params & BLUR5x5_MASK;
		if (ImGui::Checkbox("Blur 5x5 Effect", &effect))
		{
			(effect) ? m_params |= BLUR5x5_MASK : m_params &= ~BLUR5x5_MASK;
		}

		if (effect)
		{
			ImGui::SliderFloat("Blur Amount", &m_blur5x5Intensity, 0.0f, 1.0f);
		}

		effect = m_params & RADIAL_BLUR_MASK;
		if (ImGui::Checkbox("Radial Blur Effect", &effect))
		{
			(effect) ? m_params |= RADIAL_BLUR_MASK : m_params &= ~RADIAL_BLUR_MASK;
		}
		if (effect)
		{
			ImGui::SliderFloat("Radial Blur Multiplier", &m_radialBlurIntensity, 0.0f, 1.0f);
		}

		effect = m_params & EDGE_DETECTION_MASK;
		if (ImGui::Checkbox("Edge Detection Effect", &effect))
		{
			(effect) ? m_params |= EDGE_DETECTION_MASK : m_params &= ~EDGE_DETECTION_MASK;
		}
		if (effect)
		{
			ImGui::SliderFloat("Edge Detection Multiplier", &m_edgeIntensity, 0.0f, 1.0f);
		}

		

		

		ImGui::End();

		// set post process shader
		auto program = GET_RESOURCE(Program, "shaders/postprocess.prog");
		if (program)
		{
			program->Use(); // this is the shader we're gonna use...
			program->SetUniform("blend", m_blend);
			program->SetUniform("params", m_params);

			int textureWidth = ENGINE.GetSystem<Renderer>()->GetWidth();
			int textureHeight = ENGINE.GetSystem<Renderer>()->GetHeight();
			glm::vec2 texelSize = glm::vec2(1.0f / textureWidth, 1.0f / textureHeight);

			// update tintRGB uniform only if the COLORTINT_MASK is set:
			if (m_params & COLORTINT_MASK)
			{
				program->SetUniform("tintRGB", m_colorTint);
			}

			if (m_params & SCANLINE_MASK)
			{
				program->SetUniform("scanlineFrequency", m_scanlineFrequency);
				program->SetUniform("scanlineIntensity", m_scanlineIntensity);
			}

			if (m_params & GRAIN_MASK)
			{
				program->SetUniform("grainIntensity", m_grainIntensity);
				program->SetUniform("time", m_time);
			}

			if (m_params & BLUR_MASK)
			{
				program->SetUniform("texelSize", texelSize);
				program->SetUniform("blurIntensity", m_blurIntensity);
			}

			if (m_params & BLUR5x5_MASK)
			{
				program->SetUniform("texelSize", texelSize);
				program->SetUniform("blurIntensity", m_blur5x5Intensity);
			}

			if (m_params & RADIAL_BLUR_MASK)
			{
				program->SetUniform("texelSize", texelSize);
				program->SetUniform("textureWidth", m_textureWidth);
				program->SetUniform("textureHeight", m_textureHeight);
				program->SetUniform("radialBlurIntensity", m_radialBlurIntensity);
				program->SetUniform("uBlurQuality", m_blurQuality);
				program->SetUniform("uBlurSize", m_blurSize);
				program->SetUniform("uBlurDirections", m_blurDirections);
			}

			if (m_params & EDGE_DETECTION_MASK)
			{
				program->SetUniform("texelSize", texelSize);
				program->SetUniform("textureWidth", m_textureWidth);
				program->SetUniform("textureHeight", m_textureHeight);
				program->SetUniform("edgeIntensity", m_edgeIntensity);
			}
			
			
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
		m_scene->GetActorByName("postprocess")->active = true;

		auto framebuffer = GET_RESOURCE(Framebuffer, "fb");
		renderer.SetViewport(framebuffer->GetSize().x, framebuffer->GetSize().y);
		framebuffer->Bind();

		renderer.BeginFrame(glm::vec3{ 0.0f, 0, 0.0f });
		m_scene->Draw(renderer);

		framebuffer->Unbind();

		m_scene->GetActorByName("postprocess")->active = false;
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
