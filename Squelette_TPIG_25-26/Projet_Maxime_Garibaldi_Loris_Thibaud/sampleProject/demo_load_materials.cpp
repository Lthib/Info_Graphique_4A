#include <Viewer.hpp>
#include <ShaderProgram.hpp>
#include <FrameRenderable.hpp>
#include <Io.hpp>
#include <lighting/LightedMeshRenderable.hpp>
#include <lighting/Light.hpp>


void initialize_scene( Viewer& viewer )
{
    // Create a shader program
	ShaderProgramPtr phong_shader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
	ShaderProgramPtr flat_shader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");

    // Add the shader program to the viewer
    viewer.addShaderProgram( phong_shader );

    const std::string obj_path = "../../sfmlGraphicsPipeline/meshes/musclecar.obj";
    const std::string mtl_basepath = "../../sfmlGraphicsPipeline/meshes/";

    std::vector<std::vector<glm::vec3>> all_positions;
    std::vector<std::vector<glm::vec3>> all_normals;
    std::vector<std::vector<glm::vec2>> all_texcoords;
    std::vector<std::vector<unsigned int>> all_indices;
    std::vector<MaterialPtr> materials;
    
    bool indexed = false;

    if (indexed)
        read_obj_with_materials_indexed(obj_path, mtl_basepath, all_positions, all_normals, all_texcoords, all_indices, materials);
    else
        read_obj_with_materials(obj_path, mtl_basepath, all_positions, all_normals, all_texcoords, materials);

    int n_object = materials.size();
    std::vector<glm::vec4> colors;
    
    LightedMeshRenderablePtr root;

    if (indexed)
        root = std::make_shared<LightedMeshRenderable>(
            phong_shader, all_positions[0], all_indices[0], all_normals[0], colors, materials[0]);
    else
        root = std::make_shared<LightedMeshRenderable>(
            phong_shader, all_positions[0], all_normals[0], colors, materials[0]);
    for (int i = 1 ; i < n_object ; ++i){
        if (indexed){
            LightedMeshRenderablePtr part = std::make_shared<LightedMeshRenderable>(
            phong_shader, all_positions[i], all_indices[i], all_normals[i], colors, materials[i]);
            HierarchicalRenderable::addChild(root, part);
        }else{
            LightedMeshRenderablePtr part = std::make_shared<LightedMeshRenderable>(
            phong_shader, all_positions[i], all_normals[i], colors, materials[i]);
            HierarchicalRenderable::addChild(root, part);
        }
    }

    root->addGlobalTransformKeyframe(getRotationMatrix(0.00 * 2 * M_PI, 0, 1, 0), 0);
    root->addGlobalTransformKeyframe(getRotationMatrix(0.25 * 2 * M_PI, 0, 1, 0), 10);
    root->addGlobalTransformKeyframe(getRotationMatrix(0.50 * 2 * M_PI, 0, 1, 0), 20);
    root->addGlobalTransformKeyframe(getRotationMatrix(0.75 * 2 * M_PI, 0, 1, 0), 30);
    root->addGlobalTransformKeyframe(getRotationMatrix(1.00 * 2 * M_PI, 0, 1, 0), 40);

    viewer.addRenderable(root);

    glm::vec3 dir = glm::normalize(glm::vec3(-1,-1,-1));
    glm::vec3 ambient = glm::vec3(0,0,0);
    glm::vec3 diffuse = glm::vec3(1,1,1);
    glm::vec3 specular = glm::vec3(1,1,1);
    DirectionalLightPtr light1 = std::make_shared<DirectionalLight>(dir, ambient, diffuse, specular);
    dir = glm::normalize(glm::vec3(1,-1,1));
    ambient = glm::vec3(0,0,0);
    diffuse = glm::vec3(1,0.9,0.9);
    specular = glm::vec3(1,0.9,0.9);
    DirectionalLightPtr light2 = std::make_shared<DirectionalLight>(dir, ambient, diffuse, specular);
    dir = glm::normalize(glm::vec3(0,1,0));
    ambient = glm::vec3(0,0,0);
    diffuse = glm::vec3(0.5,0.3,0.3);
    specular = glm::vec3(0.5,0.3,0.3);
    DirectionalLightPtr light3 = std::make_shared<DirectionalLight>(dir, ambient, diffuse, specular);

    viewer.addDirectionalLight(light1);
    viewer.addDirectionalLight(light2);
    viewer.addDirectionalLight(light3);

    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flat_shader);
    viewer.addRenderable(frame);

    viewer.startAnimation();
}

int main() 
{
    glm::vec4 background_color(0.8,0.8,0.8,1);
	Viewer viewer(1280,720, background_color);
	initialize_scene(viewer);

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}	

	return EXIT_SUCCESS;
}