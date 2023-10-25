#include "World04.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"
#include "Renderer/VertexBuffer.h"

#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/color_space.hpp>


namespace nc
{
	bool World04::Initialize()

	{
		// materials 
		//auto material = GET_RESOURCE(Material, "Materials/skull.mtrl"); 
		//auto material = GET_RESOURCE(Material, "Materials/quad.mtrl"); 
		//auto material = GET_RESOURCE(Material, "Materials/grid.mtrl"); 
		//auto material = GET_RESOURCE(Material, "Materials/multi.mtrl"); 
		auto material = GET_RESOURCE(Material, "materials/squirrel.mtrl");
		m_model = std::make_shared<Model>();
		m_model->SetMaterial(material);
		m_model->Load("models/squirrel.glb", glm::vec3{ 0, -0.7f, 0 }, glm::vec3{ 0 }, glm::vec3{ 0.4f });

		if (!material) {
			std::cerr << "Error: Failed to load material" << std::endl;
			return false;
		}

		// initialize new Model object, wrap in smart pointer and set material 
		//m_model = std::make_shared<Model>(); // create new instance of Model class and returnd shared_ptr pointing to it. (smart pointer) Assigns to m_model
		//m_model->SetMaterial(material); // m_model is smart pointer pointing to Model object so can use -> to call methods on the pointed to object. Calls SetMaterial method on Model object, passing it 'material'
		//m_model->Load("Models/Craniu.blend");// {m_transform.position.x = 0, m_transform.position.y = 0, m_transform.position.z = 2; }
		//m_model->Load("Models/buddha.obj", glm::vec3{ 0 }, glm::vec3{ -90, 0, 0 });
		//m_model->Load("Models/dragon.obj"); {m_transform.position.x = 0, m_transform.position.y = -6, m_transform.position.z = -15; }
		//m_model->Load("Models/sphere.obj", glm::vec3{ 0 }, glm::vec3{-90, 0, 0});
		//m_model->Load("Models/plane.obj");
		//m_transform.position.y = -1;
		//m_model->Load("Models/teapot.obj"), glm::vec3{ 0, 0, -10 }, glm::vec3{-90, 0, 0};

		// overrides - Light
		m_ambientColor = glm::vec3(.093f, .073f, .123f);

		// light variables 
		for (int i = 0; i < 3; i++)
		{
			m_lights[i].type = light_t::eType::Point;
			m_lights[i].position = glm::vec3{ randomf(-5, 5), randomf(1, 8), randomf(-5, 5)};
			m_lights[i].direction = glm::vec3{ 0, -1, 0 };
			m_lights[i].color = glm::rgbColor(glm::vec3{ randomf() * 360, 1, 1 });
			m_lights[i].intensity = 1;
			m_lights[i].range = 5;
			m_lights[i].innerAngle = 10.0f;
			m_lights[i].outerAngle = 30.0f;
		}

		return true;
	};

		
	void World04::Shutdown()
	{
	}

	void World04::Update(float dt)
	{
		ENGINE.GetSystem<Gui>()->BeginFrame();

		

		ImGui::Begin("Scene");
		ImGui::ColorEdit3("Ambient Color", glm::value_ptr(m_ambientColor));
		ImGui::Separator();

		for (int i = 0; i < 3; i++)
		{
			std::string name = "light" + std::to_string(i);
			if (ImGui::Selectable(name.c_str(), m_selected == i)) m_selected = i;
		}
		ImGui::End();

		ImGui::Begin("Light");
		const char* types[] = {"Point", "Directional", "Spot"};
		ImGui::Combo("Type", (int*)(&m_lights[m_selected].type), types, 3); // light type, address to light type, types, 3 light types 

		if (m_lights[m_selected].type != light_t::Directional)  ImGui::DragFloat3("Position", glm::value_ptr(m_lights[m_selected].position), 0.1f);
		if (m_lights[m_selected].type != light_t::Point)		ImGui::DragFloat3("Direction", glm::value_ptr(m_lights[m_selected].direction), 0.1f);
		if (m_lights[m_selected].type == light_t::Spot)
		{
			ImGui::DragFloat("Inner Angle", &m_lights[m_selected].innerAngle, 1, 0, m_lights[m_selected].outerAngle);
			ImGui::DragFloat("Outer Angle", &m_lights[m_selected].outerAngle, 1, m_lights[m_selected].innerAngle, 90);
		}
		ImGui::ColorEdit3("Color", glm::value_ptr(m_lights[m_selected].color));
		ImGui::DragFloat("Intensity", &m_lights[m_selected].intensity, 0, 0.1f, 0);
		if (m_lights[m_selected].type != light_t::Directional)
		{
			ImGui::DragFloat("Range", &m_lights[m_selected].range, 0.1f, 0.1f, 50);
		}

		/*ImGui::DragFloat3("Light Position", glm::value_ptr(m_lightsPosition));
		ImGui::DragFloat("Light Cutoff", glm::value_ptr(m_lightsPosition));*/

		ImGui::End();

		//m_transform.rotation.z += 0 * dt;

		m_transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_A) ? m_speed * -dt : 0;
		m_transform.position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_D) ? m_speed * +dt : 0;
		m_transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_W) ? m_speed * -dt : 0; // going into screen 
		m_transform.position.z += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_S) ? m_speed * +dt : 0; // coming out of screen

		m_time += dt;

		auto material = m_model->GetMaterial();

		auto program = material->GetProgram();
		if (!material) {
			std::cerr << "Error: Material is null." << std::endl;
			return;
		}

		material->ProcessGui();
		material->Bind();

		for (int i = 0; i < 3; i++)
		{
			std::string name = "lights[" + std::to_string(i) + "]";
			// should these be [m_selected] or [i]? (10.24)
			material->GetProgram()->SetUniform(name + ".type", m_lights[i].type);
			material->GetProgram()->SetUniform(name + ".position", m_lights[i].position);
			material->GetProgram()->SetUniform(name + ".direction", glm::normalize(m_lights[i].direction));
			material->GetProgram()->SetUniform(name + ".color", m_lights[i].color);
			material->GetProgram()->SetUniform(name + ".intensity", m_lights[i].intensity);
			material->GetProgram()->SetUniform(name + ".range", m_lights[i].range);
			material->GetProgram()->SetUniform(name + ".innerAngle", glm::radians(m_lights[i].innerAngle));
			material->GetProgram()->SetUniform(name + ".outerAngle", glm::radians(m_lights[i].outerAngle));

		}
		// added but not sure if we still need this here (10.24)
		material->GetProgram()->SetUniform("ambientLight", m_ambientColor);

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
