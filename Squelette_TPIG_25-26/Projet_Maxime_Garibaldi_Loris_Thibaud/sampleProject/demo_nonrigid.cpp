#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <texturing/TexturedMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <Utils.hpp>

#include <iostream>

void initialize_scene( Viewer& viewer )
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(1, 2, 2 ), glm::vec3(1,1, 1), glm::vec3( 0, 1, 0 ) ) );
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    ShaderProgramPtr nonRigidShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/nonRigidVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/nonRigidFragment.glsl");
    viewer.addShaderProgram(flatShader);
    viewer.addShaderProgram(nonRigidShader);
    
    auto frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    std::string fish_mesh_path = "../../sfmlGraphicsPipeline/meshes/fish.obj";
    std::string fish_texture_path = "../../sfmlGraphicsPipeline/textures/fish_texture.png";
    auto fish = std::make_shared<TexturedMeshRenderable>(nonRigidShader, fish_mesh_path, fish_texture_path);
    viewer.addRenderable(fish);
}

int main() 
{
	Viewer viewer(1280,720, glm::vec4(0.8,0.8,0.8,1));
	initialize_scene(viewer);
    viewer.startAnimation();

	while( viewer.isRunning())
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();        
	}	

	return EXIT_SUCCESS;
}
