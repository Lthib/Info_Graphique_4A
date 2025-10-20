#include <Viewer.hpp>
#include <glm/glm.hpp>
#include "./../include/ShaderProgram.hpp"
#include "./../include/FrameRenderable.hpp"
#include "./../include/IndexedCubeRenderable.hpp"
#include "./../include/CubeRenderable.hpp"
#include "./../include/Renderable.hpp"

#define SCR_WIDTH 1024
#define SCR_HEIGHT 768

int main( int argc, char* argv[] )
{
	// Stage 1: Create the window and its OpenGL context
    glm::vec4 background_color = glm::vec4(0.8,0.8,0.8,1.0);
    Viewer viewer(SCR_WIDTH, SCR_HEIGHT, background_color);

    // Stage 2: Load resources like shaders, meshes... and make them part of the virtual scene
    // Path to the vertex shader glsl code
     std::string vShader = "./../../sfmlGraphicsPipeline/shaders/defaultVertex.glsl";
     // Path to the fragment shader glsl code
     std::string fShader = "./../../sfmlGraphicsPipeline/shaders/defaultFragment.glsl";
     // Compile and link the shaders into a program
     ShaderProgramPtr defaultShader = std::make_shared<ShaderProgram>(vShader, fShader);
     // Add the shader program to the Viewer
     viewer.addShaderProgram(defaultShader);

    vShader = "./../../sfmlGraphicsPipeline/shaders/flatVertex.glsl";
    fShader = "./../../sfmlGraphicsPipeline/shaders/flatFragment.glsl";

     // Compile and link the shaders into a program
     ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(vShader, fShader);

     // Add the shader program to the Viewer
     viewer.addShaderProgram(flatShader);

    // Shader program instantiation
     // ...
     // When instantiating a renderable ,
     // you must specify the shader program used to draw it .
     FrameRenderablePtr frame = std::make_shared<FrameRenderable>(defaultShader);
     viewer.addRenderable(frame);

     // Instantiate a CubeRenderable while specifying its shader program
     IndexedCubeRenderablePtr Indexedcube = std::make_shared<IndexedCubeRenderable>(flatShader);
    // Add the renderable to the Viewer
	 CubeRenderablePtr cube = std::make_shared<CubeRenderable>(flatShader);
     
     glm::mat4 t = glm::translate(glm::mat4(),glm::vec3(2.0f,0.0f,0.0f));
     glm::mat4 r = glm::rotate(glm::mat4(), -45.0f,glm::vec3(0.0f,0.0f,2.0f));
     glm::mat4 s = glm::scale(glm::mat4(),glm::vec3(1.0f,1.2f,1.4f));

	 Indexedcube-> setModelMatrix(t * r *s);
     
	 
	 viewer.addRenderable ( Indexedcube );
	 viewer.addRenderable(cube);

    // Stage 3: Our program loop
    while( viewer.isRunning() )
    {
        viewer.handleEvent();     // user interactivity (keyboard/mouse)
        viewer.draw();        // rasterization (write in framebuffer)
        viewer.display();        // refresh window
    }

    return EXIT_SUCCESS;
}

