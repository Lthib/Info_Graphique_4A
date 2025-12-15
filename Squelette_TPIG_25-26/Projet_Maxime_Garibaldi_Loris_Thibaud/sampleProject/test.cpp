#include <ShaderProgram.hpp>
#include <Viewer.hpp>

#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <GeometricTransformation.hpp>
#include <lighting/Light.hpp>
#include <lighting/LightedMeshRenderable.hpp>
#include <Io.hpp>

#include <iostream>

void movingCylinder(Viewer& viewer);

void initialize_scene(Viewer& viewer)
{
    movingCylinder(viewer);
}

int main()
{
    glm::vec4 background_color(0.7,0.7,0.7,1);
    Viewer viewer(1280,720, background_color);
    initialize_scene(viewer);

    while(viewer.isRunning())
    {
        viewer.handleEvent();
        viewer.animate();
        viewer.draw();
        viewer.display();
    }

    return EXIT_SUCCESS;
}

void loadObjWithMaterials(
    const std::string& obj_path,
    const std::string& basepath,
    ShaderProgramPtr shader,
    std::vector<LightedMeshRenderablePtr>& result_parts)
{
    std::vector<std::vector<glm::vec3>> positions, normals;
    std::vector<std::vector<glm::vec2>> texcoords;
    std::vector<MaterialPtr> materials;


    bool ok = read_obj_with_materials(obj_path, basepath, positions, normals, texcoords, materials);
    if (!ok) {
        std::cerr << "[loadObjWithMaterials] ERREUR: impossible de charger " << obj_path << std::endl;
        return;
    }



    // sécurité : vérifier cohérence des tailles
    size_t n_parts = materials.size();
    if (n_parts == 0) {
        std::cerr << "[loadObjWithMaterials] Aucun matériau trouvé dans " << obj_path << std::endl;
        return;
    }

    // si positions/normals/texcoords n'ont pas la même taille que materials,
    // on les considère comme vides pour les indices manquants (on protège l'accès)
    positions.resize(n_parts);
    normals.resize(n_parts);
    texcoords.resize(n_parts);


    for (size_t i = 0; i < n_parts; ++i)
    {

        // ignorer les parties sans positions : création d'un mesh sans vertex conduit à des comportements indéfinis
        if (positions[i].empty()) {
            std::cerr << "[loadObjWithMaterials] Skip material " << i << " : pas de positions (vertex) associées." << std::endl;
            continue;
        }


        // si normals/texcoords sont vides on laisse le constructeur gérer (il peut générer des normals par défaut)
        LightedMeshRenderablePtr part = std::make_shared<LightedMeshRenderable>(
            shader, positions[i], normals[i],
            std::vector<glm::vec4>(), // no vertex colors
            materials[i]
        );
        result_parts.push_back(part);
    }


    if (result_parts.empty()) {
        std::cerr << "[loadObjWithMaterials] ATTENTION: aucune partie valide chargée depuis " << obj_path << std::endl;
    }

}


void movingCylinder(Viewer& viewer)
{
    // ---------- SHADERS ----------
    ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
    viewer.addShaderProgram(phongShader);

    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram(flatShader);

    // ---------- FRAME ----------
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    const std::string basepath = "../../sfmlGraphicsPipeline/meshes/";

    //SOURIS
    //std::vector<LightedMeshRenderablePtr> souris_parts;
    //loadObjWithMaterials(basepath + "souris2.obj", basepath, phongShader, souris_parts);

    //LightedMeshRenderablePtr souris_root = souris_parts[0];
    //viewer.addRenderable(souris_root);
    //for (int i = 1; i < souris_parts.size(); ++i)
    //    HierarchicalRenderable::addChild(souris_root, souris_parts[i]);

    //souris_root->setLocalTransform(getScaleMatrix(0.3,0.3,0.3)*getTranslationMatrix(4,-0.1,0));

    //PLAINE
    //std::vector<LightedMeshRenderablePtr> plaine_parts;
    //loadObjWithMaterials(basepath + "plaine.obj", basepath, phongShader, plaine_parts);

    //LightedMeshRenderablePtr plaine_root = plaine_parts[0];
    //viewer.addRenderable(plaine_root);
    //for (int i = 1; i < plaine_parts.size(); ++i)
    //    HierarchicalRenderable::addChild(plaine_root, plaine_parts[i]);


    //PUITS
    std::vector<LightedMeshRenderablePtr> puit_parts;
    printf("marche1");
    loadObjWithMaterials(basepath + "puit.obj", basepath, phongShader, puit_parts);
    printf("marche2");

        if (puit_parts.empty()) {
        std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
        // on peut revenir sans crash ou afficher un placeholder
        return;
    }

    LightedMeshRenderablePtr puit_root = puit_parts[0];
    viewer.addRenderable(puit_root);
    for (int i = 1; i < puit_parts.size(); ++i)
        HierarchicalRenderable::addChild(puit_root, puit_parts[i]);

    puit_root->setGlobalTransform(getScaleMatrix(0.3,0.3,0.3)*getTranslationMatrix(100,0,100));

    printf("marche3");
    // //TORTUE
    // std::vector<LightedMeshRenderablePtr> carapace_parts;
    // loadObjWithMaterials(basepath + "carapaceT.obj", basepath, phongShader, carapace_parts);
    // LightedMeshRenderablePtr carapace_root = carapace_parts[0];
    // viewer.addRenderable(carapace_root);

    // for(int i=1;i<carapace_parts.size();++i)
    //     HierarchicalRenderable::addChild(carapace_root, carapace_parts[i]);

    // auto loadOne = [&](std::string filename){
    //     std::vector<LightedMeshRenderablePtr> parts;
    //     loadObjWithMaterials(basepath + filename, basepath, phongShader, parts);
    //     LightedMeshRenderablePtr root = parts[0];
    //     for (int i = 1; i < parts.size(); ++i)
    //         HierarchicalRenderable::addChild(root, parts[i]);
    //     return root;
    // };

    // LightedMeshRenderablePtr teteT  = loadOne("teteT.obj");
    // LightedMeshRenderablePtr pieds1 = loadOne("piedsT.obj");
    // LightedMeshRenderablePtr pieds2 = loadOne("piedsT.obj");
    // LightedMeshRenderablePtr pieds3 = loadOne("piedsT.obj");
    // LightedMeshRenderablePtr pieds4 = loadOne("piedsT.obj");

    // // Attach
    // HierarchicalRenderable::addChild(carapace_root, teteT);
    // HierarchicalRenderable::addChild(carapace_root, pieds1);
    // HierarchicalRenderable::addChild(carapace_root, pieds2);
    // HierarchicalRenderable::addChild(carapace_root, pieds3);
    // HierarchicalRenderable::addChild(carapace_root, pieds4);

    // // Pieds placement
    // pieds2->setLocalTransform(getTranslationMatrix(0,0,0.85));
    // pieds3->setLocalTransform(getTranslationMatrix(0.85,0,0));
    // pieds4->setLocalTransform(getTranslationMatrix(0.85,0,0.85));

    // carapace_root->setGlobalTransform(getTranslationMatrix(0,2,0));

    // //souris_root->addGlobalTransformKeyframe(
    // //    GeometricTransformation(glm::vec3(4,-0.1,0), glm::angleAxis(0.0f,glm::vec3(0,1,0)), glm::vec3(1)), 2.0f);
    // //souris_root->addGlobalTransformKeyframe(
    // //    GeometricTransformation(glm::vec3(4,-0.1,-25), glm::angleAxis(0.0f,glm::vec3(0,1,0)), glm::vec3(1)), 10.0f);


    // carapace_root->addGlobalTransformKeyframe(
    //     GeometricTransformation(glm::vec3(0,0.5,0), glm::quat(), glm::vec3(1)), 0.0f);
    // carapace_root->addGlobalTransformKeyframe(
    //     GeometricTransformation(glm::vec3(0,0.5,-3), glm::quat(), glm::vec3(1)), 10.0f);

    // pieds1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),0.0f);
    // pieds1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0),glm::quat(),glm::vec3(1)),0.5f);
    // pieds1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),1.0f);
    // pieds1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),2.0f);

    // pieds4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),0.0f);
    // pieds4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0),glm::quat(),glm::vec3(1)),0.5f);
    // pieds4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),1.0f);
    // pieds4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),2.0f);

    // pieds3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),0.0f);
    // pieds3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),1.0f);
    // pieds3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0),glm::quat(),glm::vec3(1)),1.5f);
    // pieds3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),2.0f);

    // pieds2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),0.0f);
    // pieds2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),1.0f);
    // pieds2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0),glm::quat(),glm::vec3(1)),1.5f);
    // pieds2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat(),glm::vec3(1)),2.0f);
   
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
    printf("marche4");

    // Lancer animation
    viewer.startAnimation();
    printf("marche5");
}