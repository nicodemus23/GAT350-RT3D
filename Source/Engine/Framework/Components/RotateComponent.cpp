#include "RotateComponent.h"
#include "Core/Math/MathUtils.h"

namespace nc
{
	CLASS_DEFINITION(RotateComponent)

		bool RotateComponent::Initialize()
	{
		return true;
	}
	void RotateComponent::Update(float dt)
	{
		glm::quat rotation = EulerToQuaternion(euler * dt);
		m_owner->transform.rotation = m_owner->transform.rotation * rotation;
	}

	void RotateComponent::ProcessGui()
	{
		ImGui::DragFloat3("rotate", glm::value_ptr(euler));
	}

	void RotateComponent::Read(const json_t& value)
	{
		if (!Json::Read(value, "euler", euler, true)) {
			ERROR_LOG("Failed to read euler angles for RotateComponent");
		}
		else {
			INFO_LOG("euler angles parsed from json successfully");
		}

		READ_DATA(value, euler);

		
	}
}