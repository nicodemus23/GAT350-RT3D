#pragma once
#include "Component.h"

namespace nc
{
	class CameraController : public Component
	{
	public:
		CLASS_DECLARATION(CameraController)

		bool Initialize() override;
		void Update(float dt) override;
		void ProcessGui();

	public:
		glm::vec3 rotation{ 1 };

		float sensitivity = 0.1f;
		float speed = 0;
		float yaw = 0;
		float pitch = 0;
	};
}