#include "World01.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"

namespace nc
{
    bool World01::Initialize()
    {
        return true;
    }

    void World01::Shutdown()
    {
    }

    void World01::Update(float dt)
    {
    }

    void World01::Draw(Renderer& renderer)
    {
        // pre-render
        renderer.BeginFrame();

        // render
        glBegin(GL_TRIANGLES);

        glColor3f(1, 0, 0);
        glVertex2f(-0.5f, -0.5f); 

        glColor3f(0, 0, 1);
        glVertex2f(0, 0.5f);

        glColor3f(0, 1, 0);
        glVertex2f(0.5f, -0.5f);




        glEnd();
      

        // post-render
        renderer.EndFrame();
    }
}
