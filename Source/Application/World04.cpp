#include "World04.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

#include <glm/glm/gtc/type_ptr.hpp>


namespace nc
{
	bool World04::Initialize()

	{
		auto material = GET_RESOURCE(Material, "Materials/skull.mtrl"); 
		//auto material = GET_RESOURCE(Material, "Materials/grid.mtrl"); 
		m_model = std::make_shared<Model>();
		m_model->SetMaterial(material);
		m_model->Load("Models/Craniu.blend");//, glm::vec3{ 0, 0, 2.5f }, glm::vec3{-90, 0, 90});
		//m_model->Load("Models/buddha.obj", glm::vec3{ 0 }, glm::vec3{ -90, 0, 0 });
		//m_model->Load("Models/sphere.obj", glm::vec3{ 0 }, glm::vec3{-90, 0, 0});
		//m_model->Load("Models/plane.obj"), glm::vec3{ 0 }, glm::vec3{-90, 0, 0};
		//m_model->Load("Models/teapot.obj"), glm::vec3{ 0, 0, -10 }, glm::vec3{-90, 0, 0};

		// overrides - Light
		m_ambientColor = glm::vec3(.093f, .073f, .123f);
		m_lightColor = glm::vec3(1.0f, .894f, .808f);

		return true;
	};

		
	void World04::Shutdown()
	{
	}

	void World04::Update(float dt)
	{
		ENGINE.GetSystem<Gui>()->BeginFrame();

		ImGui::Begin("Light");
		ImGui::ColorEdit3("Ambient Color", glm::value_ptr(m_ambientColor));
		ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(m_lightColor));
		ImGui::DragFloat3("Light Position", glm::value_ptr(m_lightPosition));
		ImGui::End(); 

		ImGui::Begin("Transform");
		ImGui::DragFloat3("Position", &m_transform.position[0], 0.1f);
		ImGui::DragFloat3("Rotation", &m_transform.rotation[0]);
		ImGui::DragFloat3("Scale", &m_transform.scale[0], 0.1f);
		ImGui::End();

		m_transform.rotation.z += 0 * dt;

		m_transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_A) ? m_speed * -dt : 0;
		m_transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_D) ? m_speed * +dt : 0;
		m_transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_W) ? m_speed * -dt : 0; // going into screen 
		m_transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_S) ? m_speed * +dt : 0; // coming out of screen

		m_time += dt;

		auto material = m_model->GetMaterial();

		material->ProcessGui();
		material->Bind();

		// model matrix // translate from object space to world space 
		material->GetProgram()->SetUniform("model", m_transform.GetMatrix()); // SetUniform from Program.cpp
		material->GetProgram()->SetUniform("ambientLight", m_ambientColor);
		material->GetProgram()->SetUniform("light.color", m_lightColor);
		material->GetProgram()->SetUniform("light.position", m_lightPosition);

		// view matrix
		glm::mat4 view = glm::lookAt(glm::vec3{ 0, 0, 3 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });
		material->GetProgram()->SetUniform("view", view); // SetUniform from Program.cpp

		// projection matrix - 3D
		glm::mat4 projection = glm::perspective(glm::radians(70.0f), ENGINE.GetSystem<Renderer>()->GetWidth() / (float) ENGINE.GetSystem<Renderer>()->GetHeight(), 0.01f, 100.0f);
		material->GetProgram()->SetUniform("projection", projection); // SetUniform from Program.cpp

		ENGINE.GetSystem<Gui>()->EndFrame();


	}

	void World04::Draw(Renderer& renderer)
	{
		// pre-render
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderer.BeginFrame();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// render
		m_model->Draw(GL_TRIANGLES); 
		ENGINE.GetSystem<Gui>()->Draw();

		// post-render
		renderer.EndFrame();
	}
}
