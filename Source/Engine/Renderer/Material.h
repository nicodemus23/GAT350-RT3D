#pragma once
#include "Framework/Resource/Resource.h"
#include "GUI.h"
#include <glm/glm/glm.hpp>
#include <vector>
#include <memory>

namespace nc
{
	class Texture;
	class Program;

	class Material : public Resource
	{
	public:
		// texture checks // #define preprocessor directives in lit_phong.frag
		const uint32_t ALBEDO_TEXTURE_MASK		= (1 << 0); // 00001 (bit value 1)
		const uint32_t SPECULAR_TEXTURE_MASK	= (1 << 1); // 00010 (bit value 2)
		const uint32_t NORMAL_TEXTURE_MASK		= (1 << 2); // 00100 (bit value 4)
		const uint32_t EMISSIVE_TEXTURE_MASK	= (1 << 3); // 01000 (bit value 8)
		const uint32_t CUBEMAP_TEXTURE_MASK		= (1 << 4); // 10000 (bit value 16)

		// bit shifting:
		// 0001 = ALBEDO
		// 0010 = SPECULAR
		// 0100 = NORMAL
		// 1000 = EMISSIVE

		// if have all 5 = 11111

	public:
		virtual bool Create(std::string filename, ...) override;

		void Bind();

		res_t<Program> GetProgram() { return m_program; }
		void ProcessGui();
	public:
		uint32_t params{ 0 };
		glm::vec3 albedo{ 1 }; 
		glm::vec3 specular{ 1 }; 
		glm::vec3 emissive{ 0 };
		float reflectionIntensity = 1;
		float shininess = 2;

		glm::vec2 tiling{ 1, 1 }; // uv - st
		glm::vec2 offset{ 0, 0 };

		res_t<Texture> albedoTexture;
		res_t<Texture> specularTexture;
		res_t<Texture> normalTexture;
		res_t<Texture> emissiveTexture;
		res_t<Texture> cubemapTexture;
		res_t<Texture> depthTexture;

	private:
		res_t<Program> m_program;
		//std::vector<res_t<Texture>> m_textures;
	};
}