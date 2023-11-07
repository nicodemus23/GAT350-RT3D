#pragma once
#include <glad/include/glad/glad.h>

namespace nc {

	const char* GetGLErrorString(GLenum error);
	void CheckGLError();

}