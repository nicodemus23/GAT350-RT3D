#pragma once
#include "Framework/World.h"
#include "Renderer/Renderer.h"
#include "Core/Math/Transform.h"

#include <vector>


namespace nc
{
	
	class World08 : public World
	{

	public:	
	const uint32_t INVERT_MASK			= (1 << 0);
	const uint32_t GRAYSCALE_MASK		= (1 << 1);
	const uint32_t COLORTINT_MASK		= (1 << 2);
	const uint32_t SCANLINE_MASK		= (1 << 3);
	const uint32_t GRAIN_MASK			= (1 << 4);
	const uint32_t BLUR_MASK			= (1 << 5);
	const uint32_t BLUR5x5_MASK			= (1 << 6);
	const uint32_t RADIAL_BLUR_MASK		= (1 << 7);
	const uint32_t EDGE_DETECTION_MASK	= (1 << 8);

		

	public:
		bool Initialize() override;
		void Shutdown() override;
		void Update(float dt) override;
		void Draw(Renderer& renderer) override;

		uint32_t m_params{ 0 };

	private:
		float m_time = 0;
		float m_speed = 5;
		float m_blend{ 1 };
		glm::vec3 m_colorTint{ 1 }; // instance of glm::vec3
		float m_scanlineFrequency = 100.0f;
		float m_scanlineIntensity = 0.5f;
		float m_blurIntensity = 0.0f;
		float m_blur5x5Intensity = 0.0f;

		//shader uniforms
		float m_normalMapIntensity = 1.0f;
		float m_normalMapContrast = 1.0f;

		int m_stepCount = 5;

		int m_textureWidth;
		int m_textureHeight;

		float m_grainIntensity = 0.5f;

		float m_refraction{ 1 };
		glm::vec3 m_ambientColor{ 0.2f };
		glm::vec3 m_ambientIntensity{ 1.0f };
		glm::vec4 m_reflectedColor{ 1 };

	};
}