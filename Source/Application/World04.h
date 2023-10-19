#pragma once
#include "Framework/World.h"
#include "Renderer/Renderer.h"
#include "Core/Math/Transform.h"
#include <vector>


namespace nc
{
	class World04 : public World
	{
	public:
		bool Initialize() override;
		void Shutdown() override;
		void Update(float dt) override;
		void Draw(Renderer& renderer) override;

	private:
		float m_time = 0;
		float m_speed = 1.1f;

		glm::vec3 m_ambientColor = { 0.5f, 0.5f, 0.5f };
		glm::vec3 m_lightColor = { 0.75f, 0.75f, 0.75f };
		glm::vec3 m_lightPosition = { 0.0f, 8.0f, 0.0f };

		Transform m_transform;
		res_t<Model> m_model;

	};
}