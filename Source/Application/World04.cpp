#include "World04.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

#include <glm/glm/gtc/type_ptr.hpp>


namespace nc
{
	bool World04::Initialize()

	{
		// materials 
		//auto material = GET_RESOURCE(Material, "Materials/skull.mtrl"); 
		//auto material = GET_RESOURCE(Material, "Materials/quad.mtrl"); 
		auto material = GET_RESOURCE(Material, "Materials/grid.mtrl"); 

		// initialize new Model object, wrap in smart pointer and set material 
		m_model = std::make_shared<Model>(); // create new instance of Model class and returnd shared_ptr pointing to it. (smart pointer) Assigns to m_model
		m_model->SetMaterial(material); // m_model is smart pointer pointing to Model object so can use -> to call methods on the pointed to object. Calls SetMaterial method on Model object, passing it 'material'
		m_model->Load("Models/Craniu.blend");//, glm::vec3{ 0, 0, 2.5f }, glm::vec3{-90, 0, 90});
		//m_model->Load("Models/buddha.obj", glm::vec3{ 0 }, glm::vec3{ -90, 0, 0 });
		m_model->Load("Models/dragon.obj"); m_transform.position.z = -2;
		//m_model->Load("Models/sphere.obj", glm::vec3{ 0 }, glm::vec3{-90, 0, 0});
		//m_model->Load("Models/plane.obj");
		//m_transform.position.y = -1;
		//m_model->Load("Models/teapot.obj"), glm::vec3{ 0, 0, -10 }, glm::vec3{-90, 0, 0};

		// overrides - Light
		m_ambientColor = glm::vec3(.093f, .073f, .123f);

		// light variables 
		m_light.type = light_t::eType::Point;
		m_light.position = glm::vec3{0, 5, 0};
		m_light.direction = glm::vec3{0, -1, 0};
		m_light.color = glm::vec3{ 1, 1, 1 };
		m_light.cutoff = 30.0f;

		return true;
	};

		
	void World04::Shutdown()
	{
	}

	void World04::Update(float dt)
	{
		ENGINE.GetSystem<Gui>()->BeginFrame();

		

		ImGui::Begin("Transform");
		ImGui::DragFloat3("Position", &m_transform.position[0], 0.1f);
		ImGui::DragFloat3("Rotation", &m_transform.rotation[0]);
		ImGui::DragFloat3("Scale", &m_transform.scale[0], 0.1f);
		ImGui::End();

		ImGui::Begin("Light");
		const char* types[] = {"Point", "Directional", "Spot"};
		ImGui::Combo("Type", (int*)(&m_light.type), types, 3); // light type, address to light type, types, 3 light types 

		if (m_light.type != light_t::Directional) ImGui::DragFloat3("Position", glm::value_ptr(m_light.position), 0.1f);
		if (m_light.type != light_t::Point) ImGui::DragFloat3("Direction", glm::value_ptr(m_light.direction), 0.1f);
		if (m_light.type == light_t::Spot) ImGui::DragFloat("Cutoff", &m_light.cutoff, 1, 0, 90);

		ImGui::ColorEdit3("Color", glm::value_ptr(m_light.color));

		ImGui::ColorEdit3("Ambient Color", glm::value_ptr(m_ambientColor));


		/*ImGui::DragFloat3("Light Position", glm::value_ptr(m_lightPosition));
		ImGui::DragFloat("Light Cutoff", glm::value_ptr(m_lightPosition));*/

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

		material->GetProgram()->SetUniform("light.type", m_light.type);
		material->GetProgram()->SetUniform("light.position", m_light.position);
		material->GetProgram()->SetUniform("light.direction", m_light.direction);
		material->GetProgram()->SetUniform("light.color", m_light.color);
		material->GetProgram()->SetUniform("light.cutoff", glm::radians(m_light.cutoff));
	

		// model matrix // translate from object space to world space 
		material->GetProgram()->SetUniform("model", m_transform.GetMatrix()); // SetUniform from Program.cpp


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
