#include "Texture.h"
#include "Renderer.h"
#include "Core/Logger.h"
#define STB_IMAGE_IMPLEMENTATION // builds code for stb_image
#include <stb/stb_image.h>

namespace nc
{
	Texture::~Texture()
	{
		if (m_texture) glDeleteTextures(1,  &m_texture); // delete 1 texture, address of texture
	}
	 
	bool Texture::Create(std::string filename, ...)
	{
		va_list args;

		va_start(args, filename);

		Renderer& renderer = va_arg(args, Renderer);

		va_end(args);

		return Load(filename, renderer);
	}

	bool Texture::Load(const std::string& filename, Renderer& renderer)
	{
		int channels = 0;
		// Steven Barrett's image loader:
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename.c_str(), &m_size.x, &m_size.y, &channels, 0); // channels (rgb), 0 reads image for what channels exist 
		if (!data)
		{
			WARNING_LOG("No data. Could not create surface: " << filename);
			return false;
		}
		// 1 texture, address of texture
		glGenTextures(1, &m_texture); 
		// bind texture using handle
		glBindTexture(m_target, m_texture);

		GLenum internalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8;
		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

		glTexStorage2D(m_target, 1, internalFormat, m_size.x, m_size.y);
		glTexSubImage2D(m_target, 0, 0, 0, m_size.x, m_size.y, format, GL_UNSIGNED_BYTE, data); // data is pointer to pixels declared above

		// texture filtering // types: point, linear, anisotropic filtering (defaults to linear)
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		stbi_image_free(data);


		return true;
	}
}