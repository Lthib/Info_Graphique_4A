#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <lighting/Material.hpp>
#include <texturing/EnvMapMeshRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <FrameRenderable.hpp>
#include <Utils.hpp>

#include <iostream>

void initialize_scene( Viewer& viewer )
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(1, 2, 2 ), glm::vec3(1,1, 1), glm::vec3( 0, 1, 0 ) ) );
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
    ShaderProgramPtr envmapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/envmapVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/envmapFragment.glsl");
    viewer.addShaderProgram(flatShader);
    viewer.addShaderProgram(cubeMapShader);
    viewer.addShaderProgram(envmapShader);
    
    std::string skybox_dir = "../../sfmlGraphicsPipeline/textures/skybox";
    auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, skybox_dir);
    viewer.addRenderable(cubemap);

    // Blur the environnment -> a hack to increase the material roughness -> more glossy specular reflections

    std::string diffuse_skybox_dir = "../../sfmlGraphicsPipeline/textures/skybox/diffuse";
    std::string specular_skybox_dir = "../../sfmlGraphicsPipeline/textures/skybox/glossy";
    std::string bunny_obj_path = "../../sfmlGraphicsPipeline/meshes/bunny.obj";
    std::string bunny_texture_path = "../../sfmlGraphicsPipeline/textures/bunny_texture.png";
    
    // One can try multiple material with different diffuse and specular color
    auto mat = std::make_shared<Material>(glm::vec3(0), glm::vec3(1), glm::vec3(1), 100.0f);
    
    auto envmapBunny = std::make_shared<EnvMapMeshRenderable>(
        envmapShader,
        bunny_obj_path, 
        mat,
        bunny_texture_path,
        diffuse_skybox_dir,
        specular_skybox_dir
    );

    envmapBunny->setGlobalTransform(getTranslationMatrix(1));
    envmapBunny->addGlobalTransformKeyframe(getTranslationMatrix(1) * getRotationMatrix(0 * 2 * M_PI,0,1,0)   , 0 );
    envmapBunny->addGlobalTransformKeyframe(getTranslationMatrix(1) * getRotationMatrix(0.25 * 2 * M_PI,0,1,0), 5 );
    envmapBunny->addGlobalTransformKeyframe(getTranslationMatrix(1) * getRotationMatrix(0.5 * 2 * M_PI,0,1,0) , 10 );
    envmapBunny->addGlobalTransformKeyframe(getTranslationMatrix(1) * getRotationMatrix(0.75 * 2 * M_PI,0,1,0), 15 );
    envmapBunny->addGlobalTransformKeyframe(getTranslationMatrix(1) * getRotationMatrix(1 * 2 * M_PI,0,1,0)   , 20);

    viewer.addRenderable(envmapBunny);

    auto frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);
    frame->setLocalTransform(getScaleMatrix(2));
}

int main() 
{
	Viewer viewer(1280,720, glm::vec4(0,0,0,1));
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
