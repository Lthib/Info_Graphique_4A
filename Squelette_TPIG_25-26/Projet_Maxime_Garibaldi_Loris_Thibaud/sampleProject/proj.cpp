#include <ShaderProgram.hpp>
#include <Viewer.hpp>
#include <thread>
#include <iostream>
#include <ShaderProgram.hpp>

#include <QuadMeshRenderable.hpp>
#include <FrameRenderable.hpp>

#include <dynamics/DynamicSystemRenderable.hpp>
#include <dynamics/DampingForceField.hpp>
#include <dynamics/ConstantForceField.hpp>
#include <dynamics/SpringForceField.hpp>
#include <dynamics/EulerExplicitSolver.hpp>

#include <dynamics/ParticleRenderable.hpp>
#include <dynamics/ParticleListRenderable.hpp>
#include <dynamics/ConstantForceFieldRenderable.hpp>
#include <dynamics/SpringForceFieldRenderable.hpp>
#include <dynamics/SpringListRenderable.hpp>
#include <dynamics/ControlledForceFieldRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>
#include <Io.hpp>
#include <lighting/PointLightRenderable.hpp>
#include <texturing/TexturedLightedMeshRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/SpotLightRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>

bool collisionLaunched = false;  // indique si la collision a été déclenchée
float animcol = 16.05f; // en secondes
float animfovdeb = 12.5f;
float animfovfin = 16.0f;
float mvcam1deb = 5.0f; // en secondes
float mvcam1fin = 12.5f;
float cam2deb = 16.0f; // en secondes

//Authors : Maxime Garibaldi/Loris Thibaud

glm::vec3 p1(1.0,0.0,1.0),p2(1.0,0.0,-1.0), p3(-1.0,0.0,-1.0), p4(-1.0,0.0,1.0);


void collisions(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable,QuadMeshRenderablePtr& planeRenderable);

void loadObjWithMaterials(
    const std::string& obj_path,
    const std::string& basepath,
    ShaderProgramPtr shader,
    std::vector<LightedMeshRenderablePtr>& result_parts)
{
    std::vector<std::vector<glm::vec3>> positions, normals;
    std::vector<std::vector<glm::vec2>> texcoords;
    std::vector<MaterialPtr> materials;


    //On lit l'objet avec ses matériaux, et on passe si la fonction retourne true.
    bool ok = read_obj_with_materials(obj_path, basepath, positions, normals, texcoords, materials);
    if (!ok) {
        std::cerr << "Erreur loadObjWithMaterials : la fonction a pas marché" << obj_path << std::endl;
        return;
    }



    //On vérifie qu'il y a au moins un matériau sinon on mets une erreur
    size_t n_parts = materials.size();
    if (n_parts == 0) {
        std::cerr << "Erreur loadObjWithMaterials : pas de matériaux" << obj_path << std::endl;
        return;
    }

    //On souhaite éviter les accès en dehors des limites de la taille de nos matériaux
    positions.resize(n_parts);
    normals.resize(n_parts);
    texcoords.resize(n_parts);


    for (size_t i = 0; i < n_parts; ++i)
    {

        //on regarde les meshs qui ont bien des sommets (j'ai eu une erreur je ne sais pas pourquoi)
        if (positions[i].empty()) {
            std::cerr << "Erreur loadObjWithMaterials : la partie" << i <<"n'a pas de positions." << std::endl;
            continue;
        }


        //On construit notre lightedMeshRenderable
        LightedMeshRenderablePtr part = std::make_shared<LightedMeshRenderable>(
            shader, positions[i], normals[i],
            std::vector<glm::vec4>(),
            materials[i]
        );
        result_parts.push_back(part);
    }

    //si il y a aucune partie, on continue
    if (result_parts.empty()) {
        std::cerr << "Erreur loadObjWithMaterials : pas de partie" << obj_path << std::endl;
    }

}

void initialize_scene( Viewer& viewer ,DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable,ShaderProgramPtr& flatShader)
{
    //On met en place les différents shaders que nous allons utilisés dans notre projet.
    ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
    viewer.addShaderProgram(phongShader);

    ShaderProgramPtr phongShader1 = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/phongEdge1Fragment.glsl");
    viewer.addShaderProgram(phongShader1);

    ShaderProgramPtr phongShader2 = std::make_shared<ShaderProgram>(
    "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
    "../../sfmlGraphicsPipeline/shaders/phongFragment1.glsl");
    viewer.addShaderProgram(phongShader2);
    
    ShaderProgramPtr weirdShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/weirdFragment.glsl");
    viewer.addShaderProgram(weirdShader);

    viewer.addShaderProgram( flatShader );
    // FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    // viewer.addRenderable(frame);

    const std::string basepath = "../../sfmlGraphicsPipeline/meshes/";
    //Initialize a dynamic system (Solver, Time step, Restitution coefficient)
    
    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.01);
    system->computeSimulationStep();
    //Create a renderable associated to the dynamic system
    //This renderable is responsible for calling DynamicSystem::computeSimulationStep() in the animate() function
    //It is also responsible for some of the key/mouse events
    viewer.addRenderable(systemRenderable);

    
    {//Notre première scène celle de la plaine
        std::vector<LightedMeshRenderablePtr> plaine_parts;//On commence par charger l'ensemble de notre décors
        printf("marche1");
        loadObjWithMaterials(basepath + "PlaineNT.obj", basepath, phongShader, plaine_parts);
        printf("marche2");

            if (plaine_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }
        LightedMeshRenderablePtr plaine_root = plaine_parts[0];
        viewer.addRenderable(plaine_root);
        for (int i = 1; i < plaine_parts.size(); ++i)
            HierarchicalRenderable::addChild(plaine_root, plaine_parts[i]);
        //On positionne la scène au bonne endroit et la faisons apparaître dans les bons timings
        plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 28.0f);
        plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 28.0002f);
        plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        // Ici on charge l'élément de pomme qui apparait lors de cette scène
        std::vector<LightedMeshRenderablePtr> p_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "pomme.obj", basepath, phongShader, p_parts);
        printf("marche2");

            if (p_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr p_root = p_parts[0];
        viewer.addRenderable(p_root);
        for (int i = 1; i < p_parts.size(); ++i)
            HierarchicalRenderable::addChild(p_root, p_parts[i]);
        //On la positionne ensuite dans notre scène
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,7.6,9.4), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0.3)), 0.0f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,7.6,9.4), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0.3)), 16.0f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0.2,9.4), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 16.0001f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0.2,9.4), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 19.5f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0.2,9.4), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0.365)), 19.5001f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0.2,9.4), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0.365)), 21.25f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0.2,9.4), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0.365)), 22.25f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-7.6,0.2,26), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0.365)), 23.25f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-7.6,0.2,26), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 23.2501f);
        p_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-7.6,0.2,26), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        //On charge cette fois-ci la souris
        std::vector<LightedMeshRenderablePtr> sou_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "souris.obj", basepath, phongShader1, sou_parts);
        printf("marche2");

            if (sou_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr sou_root = sou_parts[0];
        viewer.addRenderable(sou_root);
        for (int i = 1; i < sou_parts.size(); ++i)
            HierarchicalRenderable::addChild(sou_root, sou_parts[i]);
        //On postionne la souris et on créer sou mouvement durant lequel elle va chercher la pomme
        sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-14,0.25,0.5), glm::angleAxis(glm::radians(220.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 16.0f);//19.75
        sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-5.6,0,9.4), glm::angleAxis(glm::radians(220.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 21.75f);
        sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-5.6,0,9.4), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 22.25f);
        sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,26), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 23.25f);
        sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,26), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 23.251f);
        sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,26), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
    }


    //Notre deuxième scène le puits
    {   
        //On charge notre scène
        std::vector<LightedMeshRenderablePtr> puit_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "SPuitNT.obj", basepath, phongShader, puit_parts);
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
        //on fait apparaître la scène au bon moment
        puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 28.0f);
        puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 34.0f);
        puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 34.0001f);
        puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
    }
    
    
    //scène forêt
    {
        std::vector<LightedMeshRenderablePtr> f_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "ForetNonTexture.obj", basepath, phongShader, f_parts);
        printf("marche2");

            if (f_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr f_root = f_parts[0];
        viewer.addRenderable(f_root);
        for (int i = 1; i < f_parts.size(); ++i)
            HierarchicalRenderable::addChild(f_root, f_parts[i]);
        f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 34.0f);
        f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 40.0f);
        f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 40.0001f);
        f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
    }
    
    
    
    //scène arctique
    {
        std::vector<LightedMeshRenderablePtr> plat_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "PlateformeNT.obj", basepath, phongShader, plat_parts);
        printf("marche2");

            if (plat_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr plat = plat_parts[0];
        viewer.addRenderable(plat);
        for (int i = 1; i < plat_parts.size(); ++i)
            HierarchicalRenderable::addChild(plat, plat_parts[i]);
        plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,150), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 40.0f);
        plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,152.2), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 46.0f);
        plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 46.0001f);
        plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        std::vector<LightedMeshRenderablePtr> arc_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "ArctiqueNT.obj", basepath, phongShader, arc_parts);
        printf("marche2");

            if (arc_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr arc_root = arc_parts[0];
        viewer.addRenderable(arc_root);
        for (int i = 1; i < arc_parts.size(); ++i)
            HierarchicalRenderable::addChild(arc_root, arc_parts[i]);
        arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 40.0f);
        arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 46.0f);
        arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 46.0001f);
        arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        // Pingouin
        std::vector<LightedMeshRenderablePtr> pingouin_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "pingouinNT.obj", basepath, phongShader, pingouin_parts);
        printf("marche2");

            if (pingouin_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide aprÃ¨s chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr pingouin_root = pingouin_parts[0];
        viewer.addRenderable(pingouin_root);
        for (int i = 1; i < pingouin_parts.size(); ++i)
            HierarchicalRenderable::addChild(pingouin_root, pingouin_parts[i]);

        printf("marche3");
        
        pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-31,-1,18), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 40.0f);
        pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-31,-1,18), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 46.0f);
        pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 46.0001f);
        pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        pingouin_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 0.0f);
        pingouin_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.5,0,0), glm::angleAxis(0.3f,glm::vec3(0,0,1)), glm::vec3(1)), 1.0f);
        pingouin_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 2.0f);
        pingouin_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0.5,0,0), glm::angleAxis(0.3f,glm::vec3(0,0,1)), glm::vec3(1)), 3.0f);
        pingouin_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 4.0f);

        pingouin_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 0.0f);
        pingouin_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(-0.5,0,0), glm::angleAxis(-0.3f,glm::vec3(0,0,1)), glm::vec3(1)), 1.0f);
        pingouin_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 2.0f);
        pingouin_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(-0.5,0,0), glm::angleAxis(-0.3f,glm::vec3(0,0,1)), glm::vec3(1)), 3.0f);
        pingouin_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 4.0f);
    }

    
    //scène ville
    {
        std::vector<LightedMeshRenderablePtr> city_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "CityNT.obj", basepath, phongShader, city_parts);
        printf("marche2");

            if (city_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr city_root = city_parts[0];
        viewer.addRenderable(city_root);
        for (int i = 1; i < city_parts.size(); ++i)
            HierarchicalRenderable::addChild(city_root, city_parts[i]);
        city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 46.0f);
        city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 52.0f);
        city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 52.0001f);
        city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
        
        //Light
        {
            glm::mat4 globalTransformation, localTransformation;

            //Define a spot light
            glm::vec3 s_position(0.0,5.0,-8.0), s_spotDirection = glm::normalize(glm::vec3(0.0,-1.0,1.0));
            //glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
            glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.9764,0.921,0), s_specular(1.8);
            float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
            float s_innerCutOff=std::cos(glm::radians(50.0f)), s_outerCutOff=std::cos(glm::radians(70.0f));
            SpotLightPtr spotLight = std::make_shared<SpotLight>(s_position, s_spotDirection,
                                                                s_ambient, s_diffuse, s_specular,
                                                                s_constant, s_linear, s_quadratic,
                                                                s_innerCutOff, s_outerCutOff);
            viewer.addSpotLight(spotLight);
            spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 0.0f);
            spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 46.0f);
            spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-7,7,0), glm::vec3(-7,0,0), Light::base_forward), 46.001f);
            spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-7,7,0), glm::vec3(-7,0,0), Light::base_forward), 52);
            spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 52.001f);
            spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 83.0f);
            SpotLightRenderablePtr spotLightRenderable = std::make_shared<SpotLightRenderable>(flatShader, spotLight);
            localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0,0,0));
            spotLightRenderable->setLocalTransform(localTransformation);
            viewer.addRenderable(spotLightRenderable);
        }
    }

    //scène volcan
    {
        std::vector<LightedMeshRenderablePtr> vol_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "volcan.obj", basepath, phongShader, vol_parts);
        printf("marche2");

            if (vol_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr vol_root = vol_parts[0];
        viewer.addRenderable(vol_root);
        for (int i = 1; i < vol_parts.size(); ++i)
            HierarchicalRenderable::addChild(vol_root, vol_parts[i]);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 0.0f);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 41.9f);
        vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 52.0f);
        vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 58.0f);
        vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 58.0001f);
        vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
    }
    
    //scène INSA
    {
        std::vector<LightedMeshRenderablePtr> insa_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "INSA.obj", basepath, phongShader, insa_parts);
        printf("marche2");

            if (insa_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr insa_root = insa_parts[0];
        viewer.addRenderable(insa_root);
        for (int i = 1; i < insa_parts.size(); ++i)
            HierarchicalRenderable::addChild(insa_root, insa_parts[i]);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 0.0f);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 41.9f);
        insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 58.0f);
        insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 64.0f);
        insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 64.0001f);
        insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
    }
    
    //scène WTF
    {
        std::vector<LightedMeshRenderablePtr> wtf_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "WTF.obj", basepath, weirdShader, wtf_parts);
        printf("marche2");

            if (wtf_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr wtf_root = wtf_parts[0];
        viewer.addRenderable(wtf_root);
        for (int i = 1; i < wtf_parts.size(); ++i)
            HierarchicalRenderable::addChild(wtf_root, wtf_parts[i]);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 0.0f);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 41.9f);
        wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 64.0f);
        wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 70.0f);
        wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 70.0001f);
        wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
    }
    
    //scène égout
    {
        std::vector<LightedMeshRenderablePtr> eg_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "egout.obj", basepath, phongShader2, eg_parts);
        printf("marche2");

            if (eg_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr eg_root = eg_parts[0];
        viewer.addRenderable(eg_root);
        for (int i = 1; i < eg_parts.size(); ++i)
            HierarchicalRenderable::addChild(eg_root, eg_parts[i]);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 0.0f);
        // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 41.9f);
        eg_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,-2,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 70.0f);
        eg_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,-2,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 83.0f);

        {//élement de la scène rajouter
            //Tortue eg
            std::vector<LightedMeshRenderablePtr> t_parts;
            printf("marche1");
            loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader1, t_parts);
            printf("marche2");

                if (t_parts.empty()) {
                std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
                // on peut revenir sans crash ou afficher un placeholder
                return;
            }

            LightedMeshRenderablePtr t_root = t_parts[0];
            viewer.addRenderable(t_root);
            for (int i = 1; i < t_parts.size(); ++i)
                HierarchicalRenderable::addChild(t_root, t_parts[i]);

            t_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-9.25,0.5,7), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 77.0f);
            t_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-8.25,0.5,10), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 80.0f);
            t_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-8.25,0.5,10), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 83.0f);

            t_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
            t_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
            t_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
            t_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            t_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
            t_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
            t_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
            t_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            t_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
            t_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
            t_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
            t_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            t_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
            t_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
            t_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
            t_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            //Tortue2 eg
            std::vector<LightedMeshRenderablePtr> t2_parts;
            printf("marche1");
            loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader1, t2_parts);
            printf("marche2");

                if (t2_parts.empty()) {
                std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
                // on peut revenir sans crash ou afficher un placeholder
                return;
            }

            LightedMeshRenderablePtr t2_root = t2_parts[0];
            viewer.addRenderable(t2_root);
            for (int i = 1; i < t2_parts.size(); ++i)
                HierarchicalRenderable::addChild(t2_root, t2_parts[i]);

            t2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-11.75,0.5,5.5), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 77.0f);
            t2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-10.75,0.5,8.5), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 80.0f);
            t2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-10.75,0.5,8.5), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 83.0f);

            t2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
            t2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
            t2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
            t2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            t2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
            t2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
            t2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
            t2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            t2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
            t2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
            t2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
            t2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            t2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            t2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
            t2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
            t2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
            t2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);
            
            //pomme egout
            std::vector<LightedMeshRenderablePtr> ap_parts;
            printf("marche1");
            loadObjWithMaterials(basepath + "pommeCroq.obj", basepath, phongShader, ap_parts);//chargemnt du modèle 3D
            printf("marche2");

                if (ap_parts.empty()) {
                std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
                // on peut revenir sans crash ou afficher un placeholder
                return;
            }

            LightedMeshRenderablePtr ap_root = ap_parts[0];
            viewer.addRenderable(ap_root);
            for (int i = 1; i < ap_parts.size(); ++i)
                HierarchicalRenderable::addChild(ap_root, ap_parts[i]);
            //on le positione dans la scène de façon à ce que l'on vois que la pomme est croquer 
            ap_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3.25,0.5,20.5), glm::angleAxis(glm::radians(-20.0f), glm::vec3(0,1,0)), glm::vec3(0.3)), 70.0f);
            ap_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3.25,0.5,20.5), glm::angleAxis(glm::radians(-20.0f), glm::vec3(0,1,0)), glm::vec3(0.3)), 83.0f);


            //ZZZ eg
            std::vector<LightedMeshRenderablePtr> z_parts;
            printf("marche1");
            loadObjWithMaterials(basepath + "Zzz.obj", basepath, phongShader1, z_parts);
            printf("marche2");

                if (z_parts.empty()) {
                std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
                // on peut revenir sans crash ou afficher un placeholder
                return;
            }

            LightedMeshRenderablePtr z_root = z_parts[0];
            viewer.addRenderable(z_root);
            for (int i = 1; i < z_parts.size(); ++i)
                HierarchicalRenderable::addChild(z_root, z_parts[i]);


            z_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3.25,0.5,19.5), glm::angleAxis(glm::radians(-60.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 70.0f);
            z_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-3.25,0.5,19.5), glm::angleAxis(glm::radians(-60.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 83.0f);

            z_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            z_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(-0.55,0.75,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
        }
        {// Les trois spotlight disposer dans la scène
            {
                glm::mat4 globalTransformation, localTransformation;

                //Define a spot light
                glm::vec3 s_position(0.0,5.0,-8.0), s_spotDirection = glm::normalize(glm::vec3(0.0,-1.0,1.0));
                //glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
                glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(1), s_specular(1.2);
                float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
                float s_innerCutOff=std::cos(glm::radians(20.0f)), s_outerCutOff=std::cos(glm::radians(40.0f));
                SpotLightPtr spotLight2 = std::make_shared<SpotLight>(s_position, s_spotDirection,
                                                                    s_ambient, s_diffuse, s_specular,
                                                                    s_constant, s_linear, s_quadratic,
                                                                    s_innerCutOff, s_outerCutOff);
                viewer.addSpotLight(spotLight2);
                spotLight2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 0.0f);
                spotLight2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 70.0f);
                spotLight2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-2,10,-2), glm::vec3(0,0,0), Light::base_forward), 70.001f);
                spotLight2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-2,10,-2), glm::vec3(0,0,0), Light::base_forward), 83.0f);
                SpotLightRenderablePtr spotLightRenderable2 = std::make_shared<SpotLightRenderable>(flatShader, spotLight2);
                localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0,0,0));
                spotLightRenderable2->setLocalTransform(localTransformation);
                viewer.addRenderable(spotLightRenderable2);
            }
            {
                glm::mat4 globalTransformation, localTransformation;

                //Define a spot light
                glm::vec3 s_position(0.0,5.0,-8.0), s_spotDirection = glm::normalize(glm::vec3(0.0,-1.0,1.0));
                //glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
                glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(1), s_specular(1.2);
                float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
                float s_innerCutOff=std::cos(glm::radians(60.0f)), s_outerCutOff=std::cos(glm::radians(75.0f));
                SpotLightPtr spotLight3 = std::make_shared<SpotLight>(s_position, s_spotDirection,
                                                                    s_ambient, s_diffuse, s_specular,
                                                                    s_constant, s_linear, s_quadratic,
                                                                    s_innerCutOff, s_outerCutOff);
                viewer.addSpotLight(spotLight3);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 0.0f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 70.0f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 70.001f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 71.000f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 71.001f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 71.20f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 71.201f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 72.201f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 72.202f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 72.402f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 72.403f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 73.403f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 73.404f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 73.604f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 73.605f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 74.605f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(2,10,2), glm::vec3(2,-101,2), Light::base_forward), 83.0f);
                SpotLightRenderablePtr spotLightRenderable3 = std::make_shared<SpotLightRenderable>(flatShader, spotLight3);
                localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0,0,0));
                spotLightRenderable3->setLocalTransform(localTransformation);
                viewer.addRenderable(spotLightRenderable3);
            }

            {
                glm::mat4 globalTransformation, localTransformation;

                //Define a spot light
                glm::vec3 s_position(0.0,5.0,-8.0), s_spotDirection = glm::normalize(glm::vec3(0.0,-1.0,1.0));
                //glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
                glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(2), s_specular(1.7);
                float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
                float s_innerCutOff=std::cos(glm::radians(0.0f)), s_outerCutOff=std::cos(glm::radians(20.0f));
                SpotLightPtr spotLight3 = std::make_shared<SpotLight>(s_position, s_spotDirection,
                                                                    s_ambient, s_diffuse, s_specular,
                                                                    s_constant, s_linear, s_quadratic,
                                                                    s_innerCutOff, s_outerCutOff);
                viewer.addSpotLight(spotLight3);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 0.0f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-100,0), glm::vec3(0,-101,0), Light::base_forward), 70.0f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 70.001f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 71.000f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 71.001f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 71.20f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 71.201f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 72.201f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 72.202f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 72.402f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 72.403f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 73.403f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 73.404f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 73.604f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 73.605f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 74.605f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 74.606f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 74.806f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 74.807f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 75.807f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 75.808f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 76.008f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 76.009f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 77.009f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 77.01f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 77.21f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 77.211f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 78.211f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 78.212f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 78.412f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 78.413f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 79.413f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 79.414f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 79.614f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 79.615f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 80.615f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 80.616f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 80.816f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 80.817f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 81.817f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 81.818f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,-10,0), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 82.018f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 82.019f);
                spotLight3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.25,0.5,12.5), glm::vec3(-3.25,0.5,19.5), Light::base_forward), 83.0f);
                SpotLightRenderablePtr spotLightRenderable3 = std::make_shared<SpotLightRenderable>(flatShader, spotLight3);
                localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0,0,0));
                spotLightRenderable3->setLocalTransform(localTransformation);
                viewer.addRenderable(spotLightRenderable3);
            }
        }
    }
    
    //La famille tortue que l'on voit tout au cours de l'animation sauf dans les égouts
    {//l'animation de tortue sera la même pour les 4 tortues
        //Tortue1
        {
            //On commence par charger notre modèle 3D de tortue
            std::vector<LightedMeshRenderablePtr> tort_parts;
            printf("marche1");
            loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader1, tort_parts);
            printf("marche2");

                if (tort_parts.empty()) {
                std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
                // on peut revenir sans crash ou afficher un placeholder
                return;
            }

            LightedMeshRenderablePtr tort_root = tort_parts[0];
            viewer.addRenderable(tort_root);
            for (int i = 1; i < tort_parts.size(); ++i)
                HierarchicalRenderable::addChild(tort_root, tort_parts[i]);
            //Ensuite, nous animons l'ensemble de la tortue pour créer le mouvement qui fait passer la tortue de la droite à la gauche de l'écran.
            tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-10.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 12.5f);
            tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-10.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 16.0f);
            //Ici, on a voulu que les tortues arrivent à un certains emplacement pour qu'elles soient sous le lampadaire.
            tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-1.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 46.0f);
            tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 70.0f);
            tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 70.001f);
            tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);
            
            //Ensuite, on anime chaque patte pour qu'à un moment, elle se lève puis se repose sur le sol tout en ayant fait un mouvement en avant pour simuler l'avancée d'une patte
            //et on fini par faire attendre les pattes opposés.
            tort_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            tort_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
            tort_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
            tort_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
            tort_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            tort_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            tort_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
            tort_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
            tort_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
            tort_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            tort_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            tort_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
            tort_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
            tort_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
            tort_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

            tort_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
            tort_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
            tort_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
            tort_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
            tort_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);
        }
        //Tortue2
        std::vector<LightedMeshRenderablePtr> tort2_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader1, tort2_parts);
        printf("marche2");

            if (tort2_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr tort2_root = tort2_parts[0];
        viewer.addRenderable(tort2_root);
        for (int i = 1; i < tort2_parts.size(); ++i)
            HierarchicalRenderable::addChild(tort2_root, tort2_parts[i]);

        tort2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-1.5,0.5,-18.5), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-1.5,0.5,-13.1), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 12.5f);
        tort2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-1.5,0.5,-13.1), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 16.0f);
        tort2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-1.5,0.5,-4.1), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 46.0f);
        tort2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-1.5,0.5,13.5), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 70.0f);
        tort2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-1.5,0.5,13.5), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 70.001f);
        tort2_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-1.5,0.5,13.5), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        tort2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
        tort2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
        tort2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
        tort2_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
        tort2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
        tort2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
        tort2_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
        tort2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
        tort2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
        tort2_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
        tort2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
        tort2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
        tort2_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        //Tortue3
        std::vector<LightedMeshRenderablePtr> tort3_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader1, tort3_parts);
        printf("marche2");

            if (tort3_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr tort3_root = tort3_parts[0];
        viewer.addRenderable(tort3_root);
        for (int i = 1; i < tort3_parts.size(); ++i)
            HierarchicalRenderable::addChild(tort3_root, tort3_parts[i]);

        tort3_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(1.5,0.5,-19), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 0.0f);
        tort3_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(1.5,0.5,-13.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 12.5f);
        tort3_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(1.5,0.5,-13.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 16.0f);
        tort3_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(1.5,0.5,-4.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 46.0f);
        tort3_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(1.5,0.5,13), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 70.0f);
        tort3_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(1.5,0.5,13), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 70.001f);
        tort3_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(1.5,0.5,13), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        tort3_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort3_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
        tort3_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
        tort3_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
        tort3_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort3_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort3_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
        tort3_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
        tort3_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
        tort3_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort3_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort3_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
        tort3_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
        tort3_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
        tort3_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort3_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort3_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
        tort3_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
        tort3_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
        tort3_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        //Tortue4
        std::vector<LightedMeshRenderablePtr> tort4_parts;
        printf("marche1");
        loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader1, tort4_parts);
        printf("marche2");

            if (tort4_parts.empty()) {
            std::cerr << "[movingCylinder] ERREUR : puit_parts est vide après chargement de puit.obj" << std::endl;
            // on peut revenir sans crash ou afficher un placeholder
            return;
        }

        LightedMeshRenderablePtr tort4_root = tort4_parts[0];
        viewer.addRenderable(tort4_root);
        for (int i = 1; i < tort4_parts.size(); ++i)
            HierarchicalRenderable::addChild(tort4_root, tort4_parts[i]);
        
        tort4_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-0,0.5,-21), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 0.0f);
        tort4_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-15.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 12.5f);
        tort4_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-15.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 16.0f);
        tort4_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-6.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 46.0f);
        tort4_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,11), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.6,0.6,0.6)), 70.0f);
        tort4_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,11), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 70.001f);
        tort4_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,11), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 83.0f);

        tort4_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort4_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
        tort4_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
        tort4_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
        tort4_parts[1]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort4_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort4_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
        tort4_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
        tort4_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
        tort4_parts[2]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort4_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort4_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
        tort4_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
        tort4_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
        tort4_parts[3]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);

        tort4_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
        tort4_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.5f);
        tort4_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.1,-0.025), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.0f);
        tort4_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-0.05), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 3.5f);
        tort4_parts[0]->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 4.0f);
    }

//Gestion de la camera
    {
        //Premier plan fixe sur notre première scène
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 0 ); 
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 8 );
        //Mouvement de caméra afin de se rapprocher de l'arbre avec la pomme dans la première scène
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 8, 10, 0), glm::vec3(-13,3.8,14.7)), 11 );
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 8, 10, 0), glm::vec3(-13,3.8,14.7)), 12.5 );
        //Changement instantané de plan sur la tête de la tortue puis mouvement de recul pour l'effet vertigo
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 0, 0.75, -8), glm::vec3(0,0.75,-10.6)), 12.501 );
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 0, 0.75, -5), glm::vec3(0,0.75,-10.6)), 16.0 );
        //Changement de plan de nouveau en regardant l'abre met avec un distance plus grand à l'arbre afin de voir la pomme tombé 
        //et la souris la récupérer.
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 20, 10, 0), glm::vec3(-13,3.8,14.7)), 16.001 );
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 20, 10, 0), glm::vec3(-13,3.8,14.7)), 22.0 );
        //Retour sur le plan fixe global qui dure jusqu'a la scène de l'égout.
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 22.001 );
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 70 );
        //Mouvement de caméra pour se rapprocher de la souris endormie et finir l'animation avec les tortues qui rentre dans
        //le champ de la caméra.
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 20, 9, 0), glm::vec3(0,5,0)), 70.0001 );
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 14, 6, 0), glm::vec3(-4.25,0.5,19.5)), 77.0 );
        viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3(  14, 6, 0), glm::vec3(-4.25,0.5,19.5)), 83 );

        //Ici, nous jouons avec la Fov de la camera en l'augmentant ou la diminuant.
        viewer.getCamera().addFovKeyframe(glm::radians(30.0f), 0.0f);
        viewer.getCamera().addFovKeyframe(glm::radians(30.0f), 12.5f);
        viewer.getCamera().addFovKeyframe(glm::radians(90.0f), 12.501f);
        viewer.getCamera().addFovKeyframe(glm::radians(35.0f), 16.0f);
        viewer.getCamera().addFovKeyframe(glm::radians(30.0f), 16.0001f); 
        viewer.getCamera().addFovKeyframe(glm::radians(30.0f), 83.0f); 
    }

    //Les différents fonds du projet
    {
        ShaderProgramPtr textureShader = std::make_shared<ShaderProgram>(
                "../../sfmlGraphicsPipeline/shaders/textureVertex.glsl",
                "../../sfmlGraphicsPipeline/shaders/textureFragment.glsl");
        viewer.addShaderProgram(textureShader);//on charge en premier le shader qui nous servira pour les fonds
        //ensuite nous récupérons l'objet qui représente notre fond
        const std::string fond_mesh_path = "../../sfmlGraphicsPipeline/meshes/Fond1.obj";
        //on  créer un materiau pour notre fond avec ces valeur ambient, diduse et specular
        MaterialPtr fond_material = std::make_shared<Material>();
        fond_material->setAmbient(glm::vec3(0.3,0.3,0.3));
        fond_material->setDiffuse(glm::vec3(1.0,1.0,1.0));
        fond_material->setSpecular(glm::vec3(0.2,0.2,0.2));
        fond_material->setShininess(32.0f);
        //on créer le fond avec son image, le shader et le materiau créer avant
        const std::string fond1_texture_path = "../../sfmlGraphicsPipeline/meshes/Fond1.png";
        TexturedLightedMeshRenderablePtr Fond1 =
        std::make_shared<TexturedLightedMeshRenderable>(
            textureShader,          
            fond_mesh_path,      
            fond_material,       
            fond1_texture_path    
        );
        //Pour finir ont utilise les keyframe pour le faire apparaître et disparaître quand on a besoin
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 40.0f);
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 46.0f);
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 46.0001f);
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 58.00f);
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 58.0001f);
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 64.0f);
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 64.0001f);
        Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 83.0f);
        viewer.addRenderable(Fond1);

        //on refait la même chose que pour le précédent
        const std::string fond2_texture_path = "../../sfmlGraphicsPipeline/meshes/fond2.png";
        TexturedLightedMeshRenderablePtr Fond2 =
            std::make_shared<TexturedLightedMeshRenderable>(
                textureShader,          
                fond_mesh_path,      
                fond_material,       
                fond2_texture_path    
            );
        Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 28.0f);
        Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 34.0f);
        Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 34.0001f);
        Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 83.0f);
        viewer.addRenderable(Fond2);
        //pareil
        const std::string fond3_texture_path = "../../sfmlGraphicsPipeline/meshes/Fond3.png";
        TexturedLightedMeshRenderablePtr Fond3 =
            std::make_shared<TexturedLightedMeshRenderable>(
                textureShader,          
                fond_mesh_path,      
                fond_material,       
                fond3_texture_path    
            );
        Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 34.0f);
        Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 40.0f);
        Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 40.0001f);
        Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 83.0f);
        viewer.addRenderable(Fond3);
        //Pareil
        const std::string fond4_texture_path = "../../sfmlGraphicsPipeline/meshes/fond4.png";
        TexturedLightedMeshRenderablePtr Fond4 =
            std::make_shared<TexturedLightedMeshRenderable>(
                textureShader,          
                fond_mesh_path,      
                fond_material,       
                fond4_texture_path    
            );
        Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 64.0f);
        Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 70.0f);
        Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 70.0001f);
        Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 83.0f);
        viewer.addRenderable(Fond4);
        //Pareil
        const std::string fond5_texture_path = "../../sfmlGraphicsPipeline/meshes/fond5.png";
        TexturedLightedMeshRenderablePtr Fond5 =
            std::make_shared<TexturedLightedMeshRenderable>(
                textureShader,          
                fond_mesh_path,      
                fond_material,       
                fond4_texture_path    
            );
        Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 52.0f);
        Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 58.0f);
        Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 58.0001f);
        Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 83.0f);
        viewer.addRenderable(Fond5);

    }
    
    {
        { // Directional light qui représente notre soleil dans nos scènes
            auto dir_light = std::make_shared<DirectionalLight>(glm::vec3(-1,-1,-1), glm::vec3(0), glm::vec3(1), glm::vec3(1));
            viewer.addDirectionalLight(dir_light);
            //On utilise les keyframes afin de positionner notre light à certaines positions pour créer différentes intensités du soleil.
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.290 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60)    , 0);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.290 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 28);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.375 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 28.001);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.375 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 34);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.380 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 34.001);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.380 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 40);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.410 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 46);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.425 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 46.001);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.425 * 2 * M_PI, 0, 1, 1) *getTranslationMatrix(0,3,60) , 52);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.375 * 2 * M_PI, 0, 1, 1) *getTranslationMatrix(0,3,60) , 52.001);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.400 * 2 * M_PI, 0, 1, 1) *getTranslationMatrix(0,3,60) , 58);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.290 * 2 * M_PI, 0, 1, 1) *getTranslationMatrix(0,3,60) , 58.001);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.290 * 2 * M_PI, 0, 1, 1) *getTranslationMatrix(0,3,60) , 64);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.425 * 2 * M_PI, 0, 1, 1) *getTranslationMatrix(0,3,60) , 64.001);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.425 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 70);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.375 * 2 * M_PI, 0, 1, 1)*getTranslationMatrix(0,3,60) , 70.001);
            dir_light->addGlobalTransformKeyframe(getRotationMatrix( 0.375 * 2 * M_PI, 0, 1, 1)  *getTranslationMatrix(0,3,60)  , 83);

            auto dir_light_renderable = std::make_shared<DirectionalLightRenderable>(flatShader, dir_light);
            dir_light_renderable->setLocalTransform(getTranslationMatrix(0, 1.5, 3) * getScaleMatrix(0.5));

            viewer.addRenderable(dir_light_renderable);
        }
        //Ici on créer une simple lumière très faible pour éviter que le dessous de nos modèles soit trop sombres
        glm::vec3 dir = glm::normalize(glm::vec3(0,1,0));
        glm::vec3 ambient = glm::vec3(0,0,0);
        glm::vec3 diffuse = glm::vec3(0.3);
        glm::vec3 specular = glm::vec3(0.5);
        DirectionalLightPtr light3 = std::make_shared<DirectionalLight>(dir, ambient, diffuse, specular);
        viewer.addDirectionalLight(light3);
    }

    { // cubemap
        //ici on créer notre décor cube map que l'on voit dans la première scène
        //viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(1, 1, 1 ), glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) ) );
        ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
        viewer.addShaderProgram(cubeMapShader);
        
        std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/skybox3";
        auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);

        viewer.addRenderable(cubemap);
        
    }

    

    //Populate the dynamic system with particles, forcefields and create renderables associated to them for visualization.
    //Uncomment only one of the following line
    
    viewer.getCamera().setFov(0.5);
    //Finally activate animation
    viewer.startAnimation();
    // Set up a shader and add a 3D frame.
}

int main() 
{
    glm::vec4 background_color(0.7,0.7,0.7,1);
	Viewer viewer(1280,720,background_color);
    DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl", 
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    QuadMeshRenderablePtr planeRenderable = std::make_shared<QuadMeshRenderable>(flatShader, p1, p2, p3, p4);
    float pr =0.3;
	initialize_scene(viewer,system,systemRenderable,flatShader);

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		viewer.animate();
        //On regarde continuellement le temps pour savoir quand on est au bon moment pour lancer l'animation de collision.
        if (!collisionLaunched && viewer.getTime() >= animcol) {
            collisions(viewer, system, systemRenderable,planeRenderable);
            collisionLaunched = true;
        }
        //Puis dès qu'un certain timing est passé, on supprime à collision pour faire disparaître la pomme de notre collision et on fait disparaître le plan.
        if(viewer.getTime() >= animcol + 3.5f){
        system->setCollisionsDetection(false);
        planeRenderable->setLocalTransform(getScaleMatrix(0,0,0));
        viewer.getCamera();
    }
        
        
		viewer.draw();
		viewer.display();
	}	

	return EXIT_SUCCESS;
}

void collisions(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable,QuadMeshRenderablePtr& planeRenderable)
{
    //Initialize a shader for the following renderables
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );
    

    
    

    //Activate collision detection
    system->setCollisionsDetection(true);

    //Initialize the restitution coefficient for collision
    //1.0 = full elastic response
    //0.0 = full absorption
    system->setRestitution(0.3f);

    //Initialize a plane from 3 points and add it to the system as an obstacle
    
    PlanePtr plane = std::make_shared<Plane>(p1, p2, p3);
    system->addPlaneObstacle(plane);

    

    //Create a plane renderable to display the obstacle
    
    HierarchicalRenderable::addChild( systemRenderable, planeRenderable );

    glm::vec3 px,pv;
    float pm, pr;
    //Particle vs Plane collision
    {

        //Initialize a particle with position, velocity, mass and radius and add it to the system
        px = glm::vec3(-6.6,7.3,9.4);
        pv = glm::vec3(0.0,-1.8,0.0);
        pr = 0.3;
        pm = 1.2;
        ParticlePtr particle = std::make_shared<Particle>( px, pv, pm, pr);
        system->addParticle( particle );

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
        const std::string pillar_path = "../../sfmlGraphicsPipeline/meshes/pomme.obj";
        ParticleRenderablePtr particleRenderable = std::make_shared<ParticleRenderable>(flatShader, particle,pillar_path);
        particleRenderable ->setLocalTransform(getScaleMatrix(0.0f,0.0f,0.0f));
        HierarchicalRenderable::addChild( systemRenderable, particleRenderable );
    }

    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), DynamicSystem::gravity );
    system->addForceField( gravityForceField );
}