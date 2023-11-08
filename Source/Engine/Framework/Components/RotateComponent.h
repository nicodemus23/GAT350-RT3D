#pragma once
#include "Component.h"
#include "Core/Core.h"
#include "Renderer/Program.h"
#include "Framework/Framework.h"

namespace nc
{
	class RotateComponent : public Component
	{
	public:
		CLASS_DECLARATION(RotateComponent)
		
		bool Initialize();
		virtual void Update(float dt);
		void ProcessGui() override;

	public:
		glm::vec3 euler{ 1 };
		glm::vec3 rotation{ 1 };


	};

}
