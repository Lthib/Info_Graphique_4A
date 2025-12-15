// #include <ShaderProgram.hpp>
// #include <Viewer.hpp>

// #include <CylinderMeshRenderable.hpp>
// #include <FrameRenderable.hpp>
// #include <GeometricTransformation.hpp>
// #include <iostream>
// #include <iomanip>

// void movingTree(Viewer& viewer);
// void movingCylinder(Viewer& viewer);

// void initialize_scene( Viewer& viewer )
// {
//     //movingCylinder(viewer);
//     movingTree(viewer);
// }

// int main() 
// {
//     glm::vec4 background_color(0.7,0.7,0.7,1);
// 	Viewer viewer(1280,720, background_color);
// 	initialize_scene(viewer);

// 	while( viewer.isRunning() )
// 	{
// 		viewer.handleEvent();
// 		viewer.animate();
// 		viewer.draw();
// 		viewer.display();
// 	}	

// 	return EXIT_SUCCESS;
// }

// void movingCylinder( Viewer& viewer )
// {
//     //Add shader
//     ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl", 
//                                                                     "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
//     viewer.addShaderProgram( flatShader );

//     //Frame
//     FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
//     viewer.addRenderable(frame);

//     //Animated cylinder
//     auto cylinder = std::make_shared<CylinderMeshRenderable>(flatShader, false);
//     cylinder->setGlobalTransform(glm::mat4(1.0));

//     // TODO: Keyframes on parent transformation
//     cylinder->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
//     cylinder->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,2),glm::quat{0,1,0,0},glm::vec3(1,1,1)),3.0);
//     cylinder->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{0,0,0,1},glm::vec3(1,1,1)),6.0);
//     //...

//     // TODO: Keyframes on local transformation
//     //cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
//     //cylinder->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1.5,1.5,1.5)),6.0);
//     //...
//     viewer.addRenderable(cylinder);
//     viewer.startAnimation();
// }

// void movingTree( Viewer& viewer )
// {
//     //Add shader
//     ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl", 
//                                                                     "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
//     viewer.addShaderProgram( flatShader );

//     //Frame
//     FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
//     viewer.addRenderable(frame);
//     bool indexed = false; // indexed version already implemented
//     unsigned int slices = 20u; // number of slices
//     bool vertex_normals = true; // use vertex normals ? else triangle normals

//     CylinderMeshRenderablePtr cylinder0 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	//étage 1
// 	CylinderMeshRenderablePtr cylinder1_1 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	CylinderMeshRenderablePtr cylinder1_2 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	CylinderMeshRenderablePtr cylinder1_3 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	CylinderMeshRenderablePtr cylinder1_4 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	//étage 2
// 	// CylinderMeshRenderablePtr cylinder2_1 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_2 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_3 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_4 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_5 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_6 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_7 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_8 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_9 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_10 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_11 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_12 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_13 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_14 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_15 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
// 	// CylinderMeshRenderablePtr cylinder2_16 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);

//     HierarchicalRenderable::addChild(cylinder0,cylinder1_1);
// 	HierarchicalRenderable::addChild(cylinder0,cylinder1_2);
// 	//HierarchicalRenderable::addChild(cylinder0,cylinder1_3);
// 	//HierarchicalRenderable::addChild(cylinder0,cylinder1_4);
// 	// HierarchicalRenderable::addChild(cylinder1_1,cylinder2_1);
// 	// HierarchicalRenderable::addChild(cylinder1_1,cylinder2_2);
// 	// HierarchicalRenderable::addChild(cylinder1_1,cylinder2_5);
// 	// HierarchicalRenderable::addChild(cylinder1_1,cylinder2_6);
// 	// HierarchicalRenderable::addChild(cylinder1_2,cylinder2_3);
// 	// HierarchicalRenderable::addChild(cylinder1_2,cylinder2_4);
// 	// HierarchicalRenderable::addChild(cylinder1_2,cylinder2_7);
// 	// HierarchicalRenderable::addChild(cylinder1_2,cylinder2_8);
// 	// HierarchicalRenderable::addChild(cylinder1_3,cylinder2_9);
// 	// HierarchicalRenderable::addChild(cylinder1_3,cylinder2_10);
// 	// HierarchicalRenderable::addChild(cylinder1_3,cylinder2_11);
// 	// HierarchicalRenderable::addChild(cylinder1_3,cylinder2_12);
// 	// HierarchicalRenderable::addChild(cylinder1_4,cylinder2_13);
// 	// HierarchicalRenderable::addChild(cylinder1_4,cylinder2_14);
// 	// HierarchicalRenderable::addChild(cylinder1_4,cylinder2_15);
// 	// HierarchicalRenderable::addChild(cylinder1_4,cylinder2_16);
//     //Tree modeling:
//     //The modeling is hierarchical (setLocalTransform, setGlobalTransform)
//     //The animation is hierarchical too (addGlobalTransformKeyframe, addLocalTransformKeyframe)

//     // TODO: Create and animate the main branch 
//     cylinder0->setLocalTransform( GeometricTransformation( glm::vec3{}, glm::quat(), glm::vec3{0.5,0.5,4.0}).toMatrix() );
//     cylinder1_1->setLocalTransform( GeometricTransformation( glm::vec3{}, glm::quat(), glm::vec3{0.5,0.5,4.0}).toMatrix() );
//     cylinder1_2->setLocalTransform( GeometricTransformation( glm::vec3{}, glm::quat(), glm::vec3{0.5,0.5,4.0}).toMatrix() );
//     //cylinder1_3->setLocalTransform( GeometricTransformation( glm::vec3{}, glm::quat(), glm::vec3{0.5,0.5,2.0}).toMatrix() );
//     //cylinder1_4->setLocalTransform( GeometricTransformation( glm::vec3{}, glm::quat(), glm::vec3{0.5,0.5,2.0}).toMatrix() );
//     // TODO: root->addGlobalTransformKeyframe(...)
//     cylinder0->setGlobalTransform(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)).toMatrix());

// 	cylinder1_1->setGlobalTransform(getTranslationMatrix(0,0,4)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
// 	cylinder1_2->setGlobalTransform(getTranslationMatrix(0.0,0,4)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
// 	//cylinder1_3->setGlobalTransform(getTranslationMatrix(0,1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
// 	//cylinder1_4->setGlobalTransform(getTranslationMatrix(0,-1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));

//     cylinder0->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
//     cylinder0->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::angleAxis(glm::radians(-10.0f), glm::vec3(0,1,0)),glm::vec3(1,1,1)),2.0);

//     // cylinder1_1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
//     // cylinder1_1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,2),glm::quat{0,0,1,0},glm::vec3(1,1,1)),3.0);

//     // cylinder1_2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
//     // cylinder1_2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,2),glm::quat{0,0,0,1},glm::vec3(1,1,1)),3.0);

//     // cylinder1_3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
//     // cylinder1_3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,2),glm::quat{0,0,1,0},glm::vec3(1,1,1)),3.0);


//     // cylinder1_4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
//     // cylinder1_4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,2),glm::quat{0,0,1,0},glm::vec3(1,1,1)),3.0);


// 	// cylinder2_1->setGlobalTransform(getTranslationMatrix(1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
// 	// cylinder2_2->setGlobalTransform(getTranslationMatrix(-1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
// 	// cylinder2_3->setGlobalTransform(getTranslationMatrix(1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
// 	// cylinder2_4->setGlobalTransform(getTranslationMatrix(-1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
// 	// cylinder2_5->setGlobalTransform(getTranslationMatrix(0,1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
// 	// cylinder2_6->setGlobalTransform(getTranslationMatrix(0,-1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));
// 	// cylinder2_7->setGlobalTransform(getTranslationMatrix(0,1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
// 	// cylinder2_8->setGlobalTransform(getTranslationMatrix(0,-1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));
// 	// cylinder2_9->setGlobalTransform(getTranslationMatrix(1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
// 	// cylinder2_10->setGlobalTransform(getTranslationMatrix(-1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
// 	// cylinder2_13->setGlobalTransform(getTranslationMatrix(1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
// 	// cylinder2_14->setGlobalTransform(getTranslationMatrix(-1.16,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
// 	// cylinder2_11->setGlobalTransform(getTranslationMatrix(0,1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
// 	// cylinder2_12->setGlobalTransform(getTranslationMatrix(0,-1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));
// 	// cylinder2_15->setGlobalTransform(getTranslationMatrix(0,1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
// 	// cylinder2_16->setGlobalTransform(getTranslationMatrix(0,-1.16,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));



//     //TODO: Add and animate a child branch
//     //r1->addGlobalTransformKeyframe(...);
//     //r1->addLocalTransformKeyframe(...);
//     // ...
//     //HierarchicalRenderable::addChild( root, r1 );
//     // ...

//     // TODO: Add and animate any other child branchs you want

//     viewer.addRenderable( cylinder0 );

//     viewer.startAnimation();
// }

#include <ShaderProgram.hpp>
#include <Viewer.hpp>

#include <CylinderMeshRenderable.hpp>
#include <FrameRenderable.hpp>
#include <GeometricTransformation.hpp>
#include <iostream>
#include <iomanip>

void movingTree(Viewer& viewer);
void movingCylinder(Viewer& viewer);

void initialize_scene( Viewer& viewer )
{
    movingCylinder(viewer);
    //movingTree(viewer);
}

int main() 
{
    glm::vec4 background_color(0.7,0.7,0.7,1);
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

void movingCylinder( Viewer& viewer )
{
    //Add shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl", 
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Frame
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    const std::string souris_path = "../../sfmlGraphicsPipeline/meshes/souris2.obj";
    MeshRenderablePtr souris = std::make_shared<MeshRenderable>(flatShader, souris_path);
    viewer.addRenderable(souris);

    const std::string plaine_path = "../../sfmlGraphicsPipeline/meshes/plaine2.obj";
    MeshRenderablePtr plaine = std::make_shared<MeshRenderable>(flatShader, plaine_path);
    viewer.addRenderable(plaine);

    const std::string puit_path = "../../sfmlGraphicsPipeline/meshes/puit2.obj";
    MeshRenderablePtr puit = std::make_shared<MeshRenderable>(flatShader, puit_path);
    viewer.addRenderable(puit);


    const std::string teteT_path = "../../sfmlGraphicsPipeline/meshes/teteT2.obj";
    MeshRenderablePtr teteT = std::make_shared<MeshRenderable>(flatShader, teteT_path);

    const std::string piedsT_path = "../../sfmlGraphicsPipeline/meshes/piedsT.obj";
    //viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(0, 2, -4 ), glm::vec3(0,0.5, 0), glm::vec3( 0, 0, 1 ) ) );
	//viewer.getCamera().addGlobalTransformKeyframe(glm::lookAt( glm::vec3(0,2,-4), glm::vec3(0, 0.5, 0), glm::vec3( 0, 1, 0 ) ), 0);
	//viewer.getCamera().addGlobalTransformKeyframe(glm::lookAt( glm::vec3(0,2,-4), glm::vec3(0, 0.5, -3), glm::vec3( 0, 1, 0 ) ), 10);
	//viewer.getCamera().addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,3,-4), glm::vec3(0, 0.5, 0), glm::vec3( 0, 0, -1 )), 0);
	//viewer.getCamera().addGlobalTransformKeyframe(lookAtModel(glm::vec3(0,3,-4), glm::vec3(0, 0.5, -3), glm::vec3( 0, 0, -1 )), 10);
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 0, 4, -5), glm::vec3(0,0.5,0)), 0 );
    viewer.getCamera().addGlobalTransformKeyframe( lookAtUpModel(glm::vec3( 0, 4, -5), glm::vec3(0,0.5,-3)), 2 );
    MeshRenderablePtr piedsT_1 = std::make_shared<MeshRenderable>(flatShader, piedsT_path);
   
    
    MeshRenderablePtr piedsT_2 = std::make_shared<MeshRenderable>(flatShader, piedsT_path);

    
    MeshRenderablePtr piedsT_3 = std::make_shared<MeshRenderable>(flatShader, piedsT_path);

    MeshRenderablePtr piedsT_4 = std::make_shared<MeshRenderable>(flatShader, piedsT_path);

    souris->setLocalTransform(getScaleMatrix(0.3,0.3,0.3)*getTranslationMatrix(4,-0.1,0));

    piedsT_2->setLocalTransform(getTranslationMatrix(0,0,0.85));
    piedsT_3->setLocalTransform(getTranslationMatrix(0.85,0,0));
    piedsT_4->setLocalTransform(getTranslationMatrix(0.85,0,0.85));

    puit->setLocalTransform(getScaleMatrix(0.3,0.3,0.3)*getTranslationMatrix(100,0,100));

    const std::string carapaceT_path = "../../sfmlGraphicsPipeline/meshes/carapaceT2.obj";
    MeshRenderablePtr carapaceT = std::make_shared<MeshRenderable>(flatShader, carapaceT_path);
    viewer.addRenderable(carapaceT);

    HierarchicalRenderable::addChild(carapaceT,teteT);
    HierarchicalRenderable::addChild(carapaceT, piedsT_1);
    HierarchicalRenderable::addChild(carapaceT, piedsT_2);
    HierarchicalRenderable::addChild(carapaceT, piedsT_3);
    HierarchicalRenderable::addChild(carapaceT, piedsT_4);

    carapaceT->setGlobalTransform(getTranslationMatrix(0,2,0));

    // TODO: Keyframes on parent transformation

    souris->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(4,-0.1,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);
    souris->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(4,-0.1,-25), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 10.0f);

    carapaceT->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    carapaceT->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.5,-3), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 10.0f);
    
    piedsT_1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    piedsT_1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.5f);
    piedsT_1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    piedsT_1->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);


    piedsT_4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    piedsT_4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.5f);
    piedsT_4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    piedsT_4->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);

    piedsT_3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    piedsT_3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    piedsT_3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
    piedsT_3->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);

    piedsT_2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 0.0f);
    piedsT_2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.0f);
    piedsT_2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.07,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 1.5f);
    piedsT_2->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0), glm::angleAxis(glm::radians(0.0f), glm::vec3(0,1,0)), glm::vec3(1,1,1)), 2.0f);

    viewer.startAnimation();
}

void movingTree( Viewer& viewer )
{
    //Add shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl", 
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Frame
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    //Tree modeling:
    //The modeling is hierarchical (setLocalTransform, setGlobalTransform)
    //The animation is hierarchical too (addGlobalTransformKeyframe, addLocalTransformKeyframe)

    // TODO: Create and animate the main branch 
    CylinderMeshRenderablePtr root = std::make_shared<CylinderMeshRenderable>(flatShader, false);
    root->setLocalTransform( GeometricTransformation( glm::vec3{}, glm::quat(), glm::vec3{0.1,0.1,2.0}).toMatrix() );
    // TODO: root->addGlobalTransformKeyframe(...)
    // ...

    //TODO: Add and animate a child branch
    CylinderMeshRenderablePtr r1 = std::make_shared<CylinderMeshRenderable>(flatShader, false);
    //r1->addGlobalTransformKeyframe(...);
    //r1->addLocalTransformKeyframe(...);
    // ...
    //HierarchicalRenderable::addChild( root, r1 );
    // ...

    // TODO: Add and animate any other child branchs you want

    viewer.addRenderable( root );

    viewer.startAnimation();
}