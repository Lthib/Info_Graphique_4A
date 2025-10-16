#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <lighting/Material.hpp>
#include <lighting/LightedMeshRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/SpotLightRenderable.hpp>
#include <lighting/LightedCubeRenderable.hpp>
#include <lighting/PointLightRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <FrameRenderable.hpp>
#include <GeometricTransformation.hpp>
#include <Utils.hpp>

#include <iostream>

void initialize_scene( Viewer& viewer )
{
    //Position the camera
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(5, 5, 5 ), glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) ) );
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
    ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
    viewer.addShaderProgram(flatShader);
    viewer.addShaderProgram(cubeMapShader);
    viewer.addShaderProgram(phongShader);
    
    std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/skybox2";
    auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);

    viewer.addRenderable(cubemap);

    auto frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);
    frame->setLocalTransform(getScaleMatrix(10,10,10));

    auto mat = std::make_shared<Material>(glm::vec3(0), glm::vec3(1), glm::vec3(0), 100.0f);

    auto lighted_cube = std::make_shared<LightedCubeRenderable>(phongShader, false, mat);
    lighted_cube->setLocalTransform(getScaleMatrix(5,.2,5));   
    viewer.addRenderable(lighted_cube);
    
    std::string bunny_obj_path = "../../sfmlGraphicsPipeline/meshes/bunny.obj";
    LightedMeshRenderablePtr lighted_bunny = std::make_shared<LightedMeshRenderable>(phongShader, bunny_obj_path, mat);
    lighted_bunny->setGlobalTransform(getTranslationMatrix(0,1,0));
    lighted_bunny->setLocalTransform(getScaleMatrix(2,2,2));
    viewer.addRenderable(lighted_bunny);
    
    glm::vec3 red(0.9,0.3,0.4), green(0.3,0.9,0.4), blue(0.4,0.3,0.9);
    { // Moving PointLight
        auto point_light = std::make_shared<PointLight>(glm::vec3(3,5,3), glm::vec3(0), red, glm::vec3(0), 1, 0, 0);
        viewer.addPointLight(point_light);

        point_light->addGlobalTransformKeyframe(getTranslationMatrix(3,1,3), 0);
        point_light->addGlobalTransformKeyframe(getTranslationMatrix(3,1,-3), 3);
        point_light->addGlobalTransformKeyframe(getTranslationMatrix(-3,1,-3), 6);
        point_light->addGlobalTransformKeyframe(getTranslationMatrix(-3,1,3), 9);
        point_light->addGlobalTransformKeyframe(getTranslationMatrix(3,1,3), 12);

        auto point_light_renderable = std::make_shared<PointLightRenderable>(flatShader, point_light);
        point_light_renderable->setLocalTransform(getScaleMatrix(0.3));
        viewer.addRenderable(point_light_renderable);
    }
    
    { // Moving DirectionalLight
        auto dir_light = std::make_shared<DirectionalLight>(glm::vec3(1,0,0), glm::vec3(0), green, glm::vec3(0));
        viewer.addDirectionalLight(dir_light);
        
        dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0 * 2 * M_PI, 0, 1, 0)    , 0);
        dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.25 * 2 * M_PI, 0, 1, 0) , 6);
        dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.5 * 2 * M_PI, 0, 1, 0)  , 12);
        dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.75 * 2 * M_PI, 0, 1, 0) , 18);
        dir_light->addGlobalTransformKeyframe(getRotationMatrix( 1 * 2 * M_PI, 0, 1, 0)    , 24);

        auto dir_light_renderable = std::make_shared<DirectionalLightRenderable>(flatShader, dir_light);
        dir_light_renderable->setLocalTransform(getTranslationMatrix(0, 1.5, 3) * getScaleMatrix(0.5));

        viewer.addRenderable(dir_light_renderable);
    }
    
    { // Moving SpotLight
        auto spot_light = std::make_shared<SpotLight>(glm::vec3(3,5,3), glm::vec3(-1,-1,-1), glm::vec3(0), blue, glm::vec3(0), 1, 0, 0, 0.98, 0.92);
        viewer.addSpotLight(spot_light);

        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3,5,3), glm::vec3(0), Light::base_forward), 0);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-3,5,3), glm::vec3(0), Light::base_forward), 3);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-3,5,-3), glm::vec3(0), Light::base_forward), 6);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3,5,-3), glm::vec3(0), Light::base_forward), 9);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3,5,3), glm::vec3(0), Light::base_forward), 12);

        auto spot_light_renderable = std::make_shared<SpotLightRenderable>(flatShader, spot_light);
        viewer.addRenderable(spot_light_renderable);
    }
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
