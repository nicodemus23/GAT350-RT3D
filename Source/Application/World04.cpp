#include "World04.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

#include <glm/glm/gtc/type_ptr.hpp>


namespace nc
{
	bool World04::Initialize()

	{
		auto material = GET_RESOURCE(Material, "Materials/spot.mtrl"); 
		m_model = std::make_shared<Model>();
		m_model->SetMaterial(material);
		m_model->Load("Models/spot.obj");

		return true;
	};

		
	void World04::Shutdown()
	{
	}

	void World04::Update(float dt)
	{
		ENGINE.GetSystem<Gui>()->BeginFrame();

		ImGui::Begin("Transform");
		ImGui::DragFloat3("Position", &m_transform.position[0]);
		ImGui::DragFloat3("Rotation", &m_transform.rotation[0]);
		ImGui::DragFloat3("Scale", &m_transform.scale[0]);
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

		// view matrix
		glm::mat4 view = glm::lookAt(glm::vec3{ 0, 0, 3 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });
		material->GetProgram()->SetUniform("view", view); // SetUniform from Program.cpp

		// projection matrix - 3D
		glm::mat4 projection = glm::perspective(glm::radians(70.0f), 800.0f / 600.0f, 0.01f, 100.0f);
		material->GetProgram()->SetUniform("projection", projection); // SetUniform from Program.cpp

		ENGINE.GetSystem<Gui>()->EndFrame();


	}

	void World04::Draw(Renderer& renderer)
	{
		// pre-render
		renderer.BeginFrame();

		// render
		m_model->Draw(GL_TRIANGLES); 
		ENGINE.GetSystem<Gui>()->Draw();

		// post-render
		renderer.EndFrame();
	}
}
