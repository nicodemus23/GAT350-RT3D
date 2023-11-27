#pragma once

namespace nc
{
	class Editor
	{



	public:
		void Update();

		void ProcessGui(class Scene* scene); // forward declaration

	private:
		class Object* m_selected = nullptr;
		bool m_active = true;
	};
}