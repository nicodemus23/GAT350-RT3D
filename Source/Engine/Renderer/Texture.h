#pragma once
#include "Framework/Resource/Resource.h"
#include "glm/glm/glm.hpp"
#include "glad/include/glad/glad.h"


namespace nc
{
	class Texture : public Resource
	{
	public:
		Texture() = default;
		~Texture();

		virtual bool Create(std::string filename, ...) override;
		bool CreateTexture(int width, int height);
		bool CreateDepthTexture(int width, int height);

		bool Load(const std::string& filename, class Renderer& renderer);
		const glm::ivec2& GetSize() const { return m_size; }

		void SetActive(GLuint unit) { glActiveTexture(unit); }
		void Bind() { glBindTexture(m_target, m_texture); }

		// friends can access protected data 
		friend class Renderer;
		friend class Framebuffer;

	protected:
		// texture handle
		GLuint m_texture = 0;
		// default
		GLenum m_target = GL_TEXTURE_2D;
		// store texture as a glm vec2
		glm::ivec2 m_size{0};
	};
}
