#pragma once
#include "Framework/World.h"
#include "Renderer/Renderer.h"
#include "Core/Math/Transform.h"

#include <vector>


namespace nc
{
	
	class World05 : public World
	{
		

	public:
		bool Initialize() override;
		void Shutdown() override;
		void Update(float dt) override;
		void Draw(Renderer& renderer) override;

	private:
		float m_time = 0;
		float m_speed = 5;
		float m_refraction{ 1 };
		glm::vec3 m_ambientColor{ 0.2f };
		glm::vec3 m_ambientIntensity{ 1.0f };
		glm::vec4 m_reflectedColor{ 1 };

	};
}