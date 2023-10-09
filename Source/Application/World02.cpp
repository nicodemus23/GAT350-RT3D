#include "World02.h"
#include "Framework/Framework.h"
#include "Input/InputSystem.h"

//#define INTERLEAVE // preprocessor directive // interleaved buffers organize related data together {position, color} instead of {position} {color}
#define INDEX


namespace nc
{
    bool World02::Initialize()
    {   
        // shaders  // vertex = plural vertices // vertex attributes = position, color, normals, etc. 
        const char* vertexShader =
            "#version 430\n"
            "layout (location=0) in vec3 position;"
            "layout (location=1) in vec3 color;"
            "layout (location=0) out vec3 ocolor;" // out color - passed to fragmentShader 
            "void main() {"
            "  ocolor = color;"
            "  gl_Position = vec4(position, 1.0);"
            "}";


        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertexShader, NULL); // shader handle, 1 shader, address of shader, NULL) 
        glCompileShader(vs);

        const char* fragmentShader =
            "#version 430\n"
            "layout (location=0) in vec3 color;"
            "out vec4 ocolor;"
            "void main() {"
            "  ocolor = vec4(color, 1);"
            "}";

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragmentShader, NULL); // shader handle, 1 shader, address of shader, NULL) 
        glCompileShader(fs);

        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glUseProgram(program);

#ifdef INTERLEAVE
        // vertex data
        float vertexData[] = {
        -0.8f, -0.8f, 0.0f, 1.0f, 0.0f, 0.0f,
         -0.8f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.8f,  -0.8f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.8f,  0.8f, 0.0f, 1.0f, 1.0f, 1.0f

        };


        // stores vertex data in GPU memory
        GLuint vbo;

        // create buffer object
        glGenBuffers(1, &vbo); // pass back index/handle

        // bind buffer // ARRAY BUFFER is array of data 
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // set data
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
      
        // vao holds all the data for our object 
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glBindVertexBuffer(0, vbo, 0, 6 * sizeof(GLfloat)); // stride is how many bytes = 6 -> x,y,z,r,g,b

        // POSITION //
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
        glVertexAttribBinding(0, 0); // index 0, buffer 0

        // COLOR
        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
        glVertexAttribBinding(1, 0); // index 0, buffer 0
#elif defined(INDEX)
        const float vertexData[] = {
        -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top-left
         1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f  // bottom-left
        };

        GLuint indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        // vertex buffer object 
        GLuint vbo;

        // create buffer object
        glGenBuffers(1, &vbo); 
        // bind buffer // ARRAY BUFFER is array of data 
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

        // index buffer object
        GLuint ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


        // vertex array object (backpack)
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glBindVertexBuffer(0, vbo, 0, 6 * sizeof(GLfloat)); // stride is how many bytes = 6 -> x,y,z,r,g,b
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        // POSITION //
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
        glVertexAttribBinding(0, 0); // index 0, buffer 0

        // COLOR
        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
        glVertexAttribBinding(1, 0); // index 0, buffer 0

#else
        // vertex data
        float positionData[] = {
        -0.8f, -0.8f, 0.0f,
         -0.8f, 0.8f, 0.0f,
         0.8f,  -0.8f, 0.0f,
         0.8f,  0.8f, 0.0f
         
        };

        // color data
        float colorData[] =
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f
        };
   
        // stores vertex data in GPU memory
        GLuint vbo[2];

        // create buffer object
        glGenBuffers(2, vbo); // pass back index/handle

        // bind buffer // ARRAY BUFFER is array of data 
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

        // set data
        glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);

        // bind buffer // ARRAY BUFFER is array of data 
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

        // set data
        glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);


        // vao holds all the data for our object 
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        // POSITION //
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
        glBindVertexBuffer(0, vbo[0], 0, 3 * sizeof(GLfloat));

        // COLOR //
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  // every time we read in an object look for 3 floats for every position(vertex) - x, y, z
        glBindVertexBuffer(1, vbo[1], 0, 3 * sizeof(GLfloat));

#endif
         return true;

    }
    
     
    void World02::Shutdown()
    {
    }

    void World02::Update(float dt)
    {
        for (int i = 0; i < 10; i++)
        {
            m_angle += 180 * dt;
            m_position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_A) ? -dt : 0;
            m_position.x += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_D) ? +dt : 0;
            m_position.y += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_W) ? -dt : 0;
            m_position.y += ENGINE.GetSystem<InputSystem>()->GetKeyDown(SDL_SCANCODE_S) ? +dt : 0;
            m_time += dt;
        }
     
    }

    void World02::Draw(Renderer& renderer)
    {
        // pre-render
        renderer.BeginFrame();

        // render
        glBindVertexArray(m_vao);

#ifdef INDEX
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // no pointer 0
#else 
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 4 verts 
#endif
       

        // post-render
        renderer.EndFrame();
    }
}
