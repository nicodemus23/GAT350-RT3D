#pragma once
#include "Framework/World.h"
#include "Renderer/Renderer.h"
#include "Core/Math/Transform.h"
#include <vector>


namespace nc
{
	class World03 : public World
	{
	public:
		bool Initialize() override;
		void Shutdown() override;
		void Update(float dt) override;
		void Draw(Renderer& renderer) override;

	private:
		float m_time = 0;
		float m_speed = 5;

		Transform m_transform;

		res_t<Program> m_program; // resource type is shared program that can be shared among multiple objects 
		res_t<Texture> m_texture;
		res_t<Material> m_material;
		res_t<VertexBuffer> m_vertexBuffer;

	};
}