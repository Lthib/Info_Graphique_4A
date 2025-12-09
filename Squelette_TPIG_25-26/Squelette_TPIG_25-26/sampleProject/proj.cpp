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

bool collisionLaunched = false;  // indique si la collision a été déclenchée
float animcol = 16.05f; // en secondes
float animfovdeb = 12.5f;
float animfovfin = 16.0f;
float mvcam1deb = 5.0f; // en secondes
float mvcam1fin = 12.5f;
float cam2deb = 16.0f; // en secondes

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

void initialize_scene( Viewer& viewer ,DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable,ShaderProgramPtr& flatShader)
{
    //Set up a shader and add a 3D frame.
    ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
    viewer.addShaderProgram(phongShader);

    viewer.addShaderProgram( flatShader );
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

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

    const std::string cat_path = "../../sfmlGraphicsPipeline/meshes/tree.obj";
    MeshRenderablePtr cat = std::make_shared<MeshRenderable>(flatShader, cat_path);
    cat->setLocalTransform(getScaleMatrix(0.2,0.2,0.2)*getTranslationMatrix(2,0,2));
    //viewer.addRenderable(cat);
    

    //scène plaine
    std::vector<LightedMeshRenderablePtr> plaine_parts;
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
    plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 25.0f);
    plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 25.0002f);
    plaine_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

    //scène puits
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
    puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 25.0f);
    puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 32.0f);
    puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 32.0001f);
    puit_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

    //scène forêt
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
    f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 32.0f);
    f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 42.0f);
    f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 42.0001f);
    f_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

    //scène artique
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
    plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,150), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 42.0f);
    plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-2,0,152.8), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 52.0f);
    plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 52.0001f);
    plat->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

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
    arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 42.0f);
    arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 52.0f);
    arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 52.0001f);
    arc_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);


    //scène ville
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
    // city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 0.0f);
    // city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 31.9f);
    city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 52.0f);
    city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 62.0f);
    city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 62.0001f);
    city_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);
    
    //scène volcan
    std::vector<LightedMeshRenderablePtr> vol_parts;
    printf("marche1");
    loadObjWithMaterials(basepath + "VolcanNT.obj", basepath, phongShader, vol_parts);
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
    vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 62.0f);
    vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 72.0f);
    vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 72.0001f);
    vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

    //scène INSA
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
    insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 72.0f);
    insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 82.0f);
    insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 82.0001f);
    insa_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

    //scène WTF
    std::vector<LightedMeshRenderablePtr> wtf_parts;
    printf("marche1");
    loadObjWithMaterials(basepath + "WTF.obj", basepath, phongShader, wtf_parts);
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
    wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 82.0f);
    wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 92.0f);
    wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 92.0001f);
    wtf_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

    //scène égout
    std::vector<LightedMeshRenderablePtr> eg_parts;
    printf("marche1");
    loadObjWithMaterials(basepath + "egoutNT.obj", basepath, phongShader, eg_parts);
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
    eg_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,-2,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 92.0f);
    eg_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,-2,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 102.0f);
    //angle : -16.4 pos : -9.8 , -2 , 7
    //Souris
    std::vector<LightedMeshRenderablePtr> sou_parts;
    printf("marche1");
    loadObjWithMaterials(basepath + "souris.obj", basepath, phongShader, sou_parts);
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
    sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-14,0,0.5), glm::angleAxis(glm::radians(220.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 19.75f);
    sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,9.4), glm::angleAxis(glm::radians(220.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 20.75f);
    sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,9.4), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 21.25f);
    sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,26), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0.3,0.3,0.3)), 22.25f);
    sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,26), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 22.251f);
    sou_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-6.6,0,26), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);
    //Tortue
    std::vector<LightedMeshRenderablePtr> tort_parts;
    printf("marche1");
    loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader, tort_parts);
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
    

    // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 0.0f);
    // vol_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 41.9f);


    tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-10.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 12.5f);
    tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-10.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 16.0f);
    tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-1.6), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 52.0f);
    tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 92.0f);
    tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 92.001f);
    tort_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,16), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);
    
    // tort_parts[0]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    // tort_parts[0]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.5f);
    // tort_parts[0]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    // tort_parts[0]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);


    // tort_parts[4]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    // tort_parts[4]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.5f);
    // tort_parts[4]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    // tort_parts[4]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);

    // tort_parts[3]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    // tort_parts[3]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    // tort_parts[3]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
    // tort_parts[3]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);

    // tort_parts[2]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    // tort_parts[2]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    // tort_parts[2]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
    // tort_parts[2]->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);


    //Tortue eg
    std::vector<LightedMeshRenderablePtr> t_parts;
    printf("marche1");
    loadObjWithMaterials(basepath + "tortue.obj", basepath, phongShader, t_parts);
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

    t_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-16.25,0.5,-16), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 92.0f);
    t_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-12.25,0.5,-5), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 98.0f);
    t_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-12.25,0.5,-5), glm::angleAxis(glm::radians(200.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 102.0f);

    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 0 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 5 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 8, 10, 0), glm::vec3(-13,3.8,14.7)), 11 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 8, 10, 0), glm::vec3(-13,3.8,14.7)), 12.5 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 0, 0.75, -5), glm::vec3(0,0.75,-6)), 12.501 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 0, 0.75, -5), glm::vec3(0,0.75,-6)), 16.0 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 20, 10, 0), glm::vec3(-13,3.8,14.7)), 16.001 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 20, 10, 0), glm::vec3(-13,3.8,14.7)), 24.0 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 24.001 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 30, 10, 0), glm::vec3(0,5,0)), 92 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 26, 10, 0), glm::vec3(0,5,0)), 92.0001 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 26, 10, 0), glm::vec3(0,5,0)), 102 );


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
    
    pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-31,-1,18), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 42.0f);
    pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-31,-1,18), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 52.0f);
    pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 52.0001f);
    pingouin_root->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0)), glm::vec3(0,0,0)), 102.0f);

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

//fonds
ShaderProgramPtr textureShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/textureVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/textureFragment.glsl");
viewer.addShaderProgram(textureShader);

const std::string fond_mesh_path = "../../sfmlGraphicsPipeline/meshes/Fond1.obj";

MaterialPtr fond_material = std::make_shared<Material>();
fond_material->setAmbient(glm::vec3(0.3,0.3,0.3));
fond_material->setDiffuse(glm::vec3(1.0,1.0,1.0)); // à modifier au feeling
fond_material->setSpecular(glm::vec3(0.2,0.2,0.2));
fond_material->setShininess(32.0f);

const std::string fond1_texture_path = "../../sfmlGraphicsPipeline/meshes/Fond1.png";
TexturedLightedMeshRenderablePtr Fond1 =
   std::make_shared<TexturedLightedMeshRenderable>(
       textureShader,          
       fond_mesh_path,      
       fond_material,       
       fond1_texture_path    
   );
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 0.0f);
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 25.0f);
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 25.0001f);
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 72.00f);
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 72.0001f);
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 82.0f);
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 82.0001f);
Fond1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 102.0f);
viewer.addRenderable(Fond1);

const std::string fond2_texture_path = "../../sfmlGraphicsPipeline/meshes/fond2.png";
TexturedLightedMeshRenderablePtr Fond2 =
    std::make_shared<TexturedLightedMeshRenderable>(
        textureShader,          
        fond_mesh_path,      
        fond_material,       
        fond2_texture_path    
    );
Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 25.0f);
Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 32.0f);
Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 32.0001f);
Fond2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 102.0f);
viewer.addRenderable(Fond2);

const std::string fond3_texture_path = "../../sfmlGraphicsPipeline/meshes/Fond3.png";
TexturedLightedMeshRenderablePtr Fond3 =
    std::make_shared<TexturedLightedMeshRenderable>(
        textureShader,          
        fond_mesh_path,      
        fond_material,       
        fond3_texture_path    
    );
Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 32.0f);
Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 42.0f);
Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 42.0001f);
Fond3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 102.0f);
viewer.addRenderable(Fond3);

const std::string fond4_texture_path = "../../sfmlGraphicsPipeline/meshes/fond4.png";
TexturedLightedMeshRenderablePtr Fond4 =
    std::make_shared<TexturedLightedMeshRenderable>(
        textureShader,          
        fond_mesh_path,      
        fond_material,       
        fond4_texture_path    
    );
Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 42.0f);
Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 52.0f);
Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 52.0001f);
Fond4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 102.0f);
viewer.addRenderable(Fond4);

const std::string fond5_texture_path = "../../sfmlGraphicsPipeline/meshes/fond5.png";
TexturedLightedMeshRenderablePtr Fond5 =
    std::make_shared<TexturedLightedMeshRenderable>(
        textureShader,          
        fond_mesh_path,      
        fond_material,       
        fond4_texture_path    
    );
Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 62.0f);
Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(1)), 72.0f);
Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 72.0001f);
Fond5->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(0.0f,glm::vec3(0,0,1)), glm::vec3(0)), 102.0f);
viewer.addRenderable(Fond5);


    // Light
    {

    }

    glm::vec3 lightPosition(0.0,5.0,8.0);
    glm::vec3 dir = glm::normalize(glm::vec3(-1,-1,-1));
    glm::vec3 ambient = glm::vec3(0,0,0);
    glm::vec3 diffuse = glm::vec3(1,1,1);
    glm::vec3 specular = glm::vec3(1,1,1);
    DirectionalLightPtr light1 = std::make_shared<DirectionalLight>(dir, ambient, diffuse, specular);
    light1->setGlobalTransform(getTranslationMatrix(lightPosition) * light1->getGlobalTransform());
    dir = glm::normalize(glm::vec3(1,-1,1));
    ambient = glm::vec3(0,0,0);
    diffuse = glm::vec3(1,0.9,0.9);
    specular = glm::vec3(1,0.9,0.9);
    DirectionalLightPtr light2 = std::make_shared<DirectionalLight>(dir, ambient, diffuse, specular);
    dir = glm::normalize(glm::vec3(0,3,0));
    ambient = glm::vec3(0,0,0);
    diffuse = glm::vec3(0.5,0.3,0.3);
    specular = glm::vec3(0.5,0.3,0.3);
    DirectionalLightPtr light3 = std::make_shared<DirectionalLight>(dir, ambient, diffuse, specular);

    viewer.addDirectionalLight(light1);
    viewer.addDirectionalLight(light2);
    viewer.addDirectionalLight(light3);
    

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
	initialize_scene(viewer,system,systemRenderable,flatShader);

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		viewer.animate();
        if (!collisionLaunched && viewer.getTime() >= animcol) {
            collisions(viewer, system, systemRenderable,planeRenderable);
            collisionLaunched = true;
        }

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
        pr = 0.2;
        pm = 1.2;
        ParticlePtr particle = std::make_shared<Particle>( px, pv, pm, pr);
        system->addParticle( particle );

        //Create a particleRenderable for each particle of the system
        //DynamicSystemRenderable act as a hierarchical renderable
        //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
        const std::string pillar_path = "../../sfmlGraphicsPipeline/meshes/pomme2.obj";
        ParticleRenderablePtr particleRenderable = std::make_shared<ParticleRenderable>(flatShader, particle,pillar_path);
        particleRenderable ->setLocalTransform(getScaleMatrix(0.002f,0.002f,0.002f));
        HierarchicalRenderable::addChild( systemRenderable, particleRenderable );
    }

    //Initialize a force field that apply to all the particles of the system to simulate gravity
    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), DynamicSystem::gravity );
    system->addForceField( gravityForceField );
}