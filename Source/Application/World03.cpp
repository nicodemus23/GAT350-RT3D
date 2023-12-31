#include "World03.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

#include <glm/glm/gtc/type_ptr.hpp>

#define INTERLEAVE // preprocessor directive // interleaved buffers organize related data together {position, color} instead of {position} {color}

namespace nc
{
	bool World03::Initialize()

	{
		m_material = GET_RESOURCE(Material, "Materials/quad.mtrl"); 


#ifdef INTERLEAVE
		// vertex data
		float vertexData[] = {
		-0.8f, -0.8f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 -0.8f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.8f,  -0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 0.8f,  0.8f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f

		};

		m_vertexBuffer = std::make_shared<VertexBuffer>();
		m_vertexBuffer->CreateVertexBuffer(sizeof(vertexData), 4, vertexData);
		m_vertexBuffer->SetAttribute(0, 3, 8 * sizeof(GLfloat), 0);                  // position 
		m_vertexBuffer->SetAttribute(1, 3, 8 * sizeof(GLfloat), 3 * sizeof(float));  // color 
		m_vertexBuffer->SetAttribute(2, 2, 8 * sizeof(GLfloat), 6 * sizeof(float));  // texcoord


#else
		// vertex data
		float positionData[] = {
		-0.8f, -0.8f, 0.0f,
		 -0.8f, 0.8f, 0.0f,
		 0.8f,  -0.8f, 0.0f,
		 0.8f,  0.8f, 0.0f

		};

		// color data
		float colorData[] =
		{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f
		};

		// stores vertex data in GPU memory
		GLuint vbo[2];

		// create buffer object
		glGenBuffers(2, vbo); // pass back index/handle

		// bind buffer // ARRAY BUFFER is array of data 
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

		// set data
		glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);

		// bind buffer // ARRAY BUFFER is array of data 
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

		// set data
		glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);


		// vao holds all the data for our object 
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// POSITION //
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
		glBindVertexBuffer(0, vbo[0], 0, 3 * sizeof(GLfloat));

		// COLOR //
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
		glBindVertexBuffer(1, vbo[1], 0, 3 * sizeof(GLfloat));

#endif
		//  m_position.z = -10.0f;
		return true;

	}


	void World03::Shutdown()
	{
	}

	void World03::Update(float dt)
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

		m_material->ProcessGui();
		m_material->Bind();

		// model matrix // translate from object space to world space 
		m_material->GetProgram()->SetUniform("model", m_transform.GetMatrix()); // SetUniform from Program.cpp

		// view matrix
		glm::mat4 view = glm::lookAt(glm::vec3{ 0, 0, 3 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });
		m_material->GetProgram()->SetUniform("view", view); // SetUniform from Program.cpp

		// projection matrix - 3D
		glm::mat4 projection = glm::perspective(glm::radians(70.0f), 800.0f / 600.0f, 0.01f, 100.0f);
		m_material->GetProgram()->SetUniform("projection", projection); // SetUniform from Program.cpp

		ENGINE.GetSystem<Gui>()->EndFrame();


	}

	void World03::Draw(Renderer& renderer)
	{
		// pre-render
		renderer.BeginFrame();

		// render
		m_vertexBuffer->Draw(GL_TRIANGLE_STRIP); 
		ENGINE.GetSystem<Gui>()->Draw();

		// post-render
		renderer.EndFrame();
	}
}
