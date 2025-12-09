#include <Viewer.hpp>
#include <ShaderProgram.hpp>
#include <FrameRenderable.hpp>
#include <HierarchicalRenderable.hpp>
#include <CylinderMeshRenderable.hpp>
#include <MeshRenderable.hpp>
#include <SphereMeshRenderable.hpp>

void initialize_scene( Viewer& viewer )
{
	ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");

	viewer.addShaderProgram( flatShader );
	
	//Frame
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);
	
	// TODO: Add an hierarchical structure of 3D objects.
	ShaderProgramPtr parentProg = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
	ShaderProgramPtr childProg = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
	viewer.addShaderProgram(parentProg);
	viewer.addShaderProgram(childProg);

	//std::shared_ptr<>
	// const std::string cat_path = "../../sfmlGraphicsPipeline/meshes/cat.obj";
    // MeshRenderablePtr cat = std::make_shared<MeshRenderable>(flatShader, cat_path);
	// const std::string pillar_path = "../../sfmlGraphicsPipeline/meshes/pillar.obj";
    // MeshRenderablePtr pillar = std::make_shared<MeshRenderable>(flatShader, pillar_path);
	// cat ->setGlobalTransform(getRotationMatrix(1.57f,glm::vec3(0,0,-1))*getTranslationMatrix(0.25,3.23,0));
	// pillar->setGlobalTransform(getTranslationMatrix(-5,0,0)*getRotationMatrix(1.57f,glm::vec3(0,0,1)));

	// HierarchicalRenderable::addChild(pillar,cat);
	// viewer.addRenderable(pillar);

	bool indexed = false; // indexed version already implemented
    unsigned int slices = 20u; // number of slices
    bool vertex_normals = true; // use vertex normals ? else triangle normals
    // See CylinderMeshRenderable.cpp
	//étage 0
    CylinderMeshRenderablePtr cylinder0 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	//étage 1
	CylinderMeshRenderablePtr cylinder1_1 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder1_2 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder1_3 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder1_4 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	//étage 2
	CylinderMeshRenderablePtr cylinder2_1 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_2 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_3 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_4 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_5 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_6 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_7 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_8 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_9 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_10 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_11 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_12 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_13 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_14 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_15 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	CylinderMeshRenderablePtr cylinder2_16 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);


	SphereMeshRenderablePtr sp2_1 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_2 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_3 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_4 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_5 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_6 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_7 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_8 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_9 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_10 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_11 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_12 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_13 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_14 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_15 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	SphereMeshRenderablePtr sp2_16 = std::make_shared<SphereMeshRenderable>(flatShader, indexed, 10u,slices, vertex_normals);
	//étage 3
	// CylinderMeshRenderablePtr cylinder3_1 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	// CylinderMeshRenderablePtr cylinder3_2 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	// CylinderMeshRenderablePtr cylinder3_3 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	// CylinderMeshRenderablePtr cylinder3_4 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	// CylinderMeshRenderablePtr cylinder3_5 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	// CylinderMeshRenderablePtr cylinder3_6 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	// CylinderMeshRenderablePtr cylinder3_7 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	// CylinderMeshRenderablePtr cylinder3_8 = std::make_shared<CylinderMeshRenderable>(flatShader, indexed, slices, vertex_normals);
	
	HierarchicalRenderable::addChild(cylinder0,cylinder1_1);
	HierarchicalRenderable::addChild(cylinder0,cylinder1_2);
	HierarchicalRenderable::addChild(cylinder0,cylinder1_3);
	HierarchicalRenderable::addChild(cylinder0,cylinder1_4);
	HierarchicalRenderable::addChild(cylinder1_1,cylinder2_1);
	HierarchicalRenderable::addChild(cylinder1_1,cylinder2_2);
	HierarchicalRenderable::addChild(cylinder1_1,cylinder2_5);
	HierarchicalRenderable::addChild(cylinder1_1,cylinder2_6);
	HierarchicalRenderable::addChild(cylinder1_2,cylinder2_3);
	HierarchicalRenderable::addChild(cylinder1_2,cylinder2_4);
	HierarchicalRenderable::addChild(cylinder1_2,cylinder2_7);
	HierarchicalRenderable::addChild(cylinder1_2,cylinder2_8);
	HierarchicalRenderable::addChild(cylinder1_3,cylinder2_9);
	HierarchicalRenderable::addChild(cylinder1_3,cylinder2_10);
	HierarchicalRenderable::addChild(cylinder1_3,cylinder2_11);
	HierarchicalRenderable::addChild(cylinder1_3,cylinder2_12);
	HierarchicalRenderable::addChild(cylinder1_4,cylinder2_13);
	HierarchicalRenderable::addChild(cylinder1_4,cylinder2_14);
	HierarchicalRenderable::addChild(cylinder1_4,cylinder2_15);
	HierarchicalRenderable::addChild(cylinder1_4,cylinder2_16);
	HierarchicalRenderable::addChild(cylinder2_1,sp2_1);
	HierarchicalRenderable::addChild(cylinder2_2,sp2_2);
	HierarchicalRenderable::addChild(cylinder2_3,sp2_3);
	HierarchicalRenderable::addChild(cylinder2_4,sp2_4);
	HierarchicalRenderable::addChild(cylinder2_5,sp2_5);
	HierarchicalRenderable::addChild(cylinder2_6,sp2_6);
	HierarchicalRenderable::addChild(cylinder2_7,sp2_7);
	HierarchicalRenderable::addChild(cylinder2_8,sp2_8);
	HierarchicalRenderable::addChild(cylinder2_9,sp2_9);
	HierarchicalRenderable::addChild(cylinder2_10,sp2_10);
	HierarchicalRenderable::addChild(cylinder2_11,sp2_11);
	HierarchicalRenderable::addChild(cylinder2_12,sp2_12);
	HierarchicalRenderable::addChild(cylinder2_13,sp2_13);
	HierarchicalRenderable::addChild(cylinder2_14,sp2_14);
	HierarchicalRenderable::addChild(cylinder2_15,sp2_15);
	HierarchicalRenderable::addChild(cylinder2_16,sp2_16);
	
	
	
	// HierarchicalRenderable::addChild(cylinder2_1,cylinder3_1);
	// HierarchicalRenderable::addChild(cylinder2_1,cylinder3_2);
	// HierarchicalRenderable::addChild(cylinder2_2,cylinder3_3);
	// HierarchicalRenderable::addChild(cylinder2_2,cylinder3_4);
	// HierarchicalRenderable::addChild(cylinder2_3,cylinder3_5);
	// HierarchicalRenderable::addChild(cylinder2_3,cylinder3_6);
	// HierarchicalRenderable::addChild(cylinder2_4,cylinder3_7);
	// HierarchicalRenderable::addChild(cylinder2_4,cylinder3_8);

	cylinder0->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder1_1->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder1_2->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder1_3->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder1_4->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_1->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_2->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_3->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_4->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_5->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_6->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_7->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_8->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_9->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_10->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_11->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_12->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_13->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_14->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_15->setLocalTransform(getScaleMatrix(0.3,0.3,4));
	cylinder2_16->setLocalTransform(getScaleMatrix(0.3,0.3,4));

	cylinder0->setGlobalTransform(getTranslationMatrix(0,0,0));

	cylinder1_1->setGlobalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
	cylinder1_2->setGlobalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
	cylinder1_3->setGlobalTransform(getTranslationMatrix(0,0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
	cylinder1_4->setGlobalTransform(getTranslationMatrix(0,-0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));

	cylinder2_1->setGlobalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
	cylinder2_2->setGlobalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
	cylinder2_3->setGlobalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
	cylinder2_4->setGlobalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
	cylinder2_5->setGlobalTransform(getTranslationMatrix(0,0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
	cylinder2_6->setGlobalTransform(getTranslationMatrix(0,-0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));
	cylinder2_7->setGlobalTransform(getTranslationMatrix(0,0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
	cylinder2_8->setGlobalTransform(getTranslationMatrix(0,-0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));
	cylinder2_9->setGlobalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
	cylinder2_10->setGlobalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
	cylinder2_13->setGlobalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0)));
	cylinder2_14->setGlobalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0)));
	cylinder2_11->setGlobalTransform(getTranslationMatrix(0,0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
	cylinder2_12->setGlobalTransform(getTranslationMatrix(0,-0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));
	cylinder2_15->setGlobalTransform(getTranslationMatrix(0,0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(-5/2,0,0)));
	cylinder2_16->setGlobalTransform(getTranslationMatrix(0,-0.0,3.5)*getRotationMatrix(3.14/5,glm::vec3(5/2,0,0)));
	sp2_1->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_2->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_3->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_4->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_5->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_6->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_7->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_8->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_9->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_10->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_11->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_12->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_13->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_14->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_15->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));
	sp2_16->setLocalTransform(getTranslationMatrix(0,0,4)*getScaleMatrix(0.3,0.3,0.3));


	// cylinder3_1->setLocalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0))*getScaleMatrix(0.3,0.3,2));
	// cylinder3_2->setLocalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0))*getScaleMatrix(0.3,0.3,2));
	// cylinder3_3->setLocalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0))*getScaleMatrix(0.3,0.3,2));
	// cylinder3_4->setLocalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0))*getScaleMatrix(0.3,0.3,2));
	// cylinder3_5->setLocalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0))*getScaleMatrix(0.3,0.3,2));
	// cylinder3_6->setLocalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0))*getScaleMatrix(0.3,0.3,2));
	// cylinder3_7->setLocalTransform(getTranslationMatrix(0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,1,0))*getScaleMatrix(0.3,0.3,2));
	// cylinder3_8->setLocalTransform(getTranslationMatrix(-0.0,0,3.5)*getRotationMatrix(3.14/5,glm::vec3(0,-1,0))*getScaleMatrix(0.3,0.3,2));
	

	viewer.addRenderable(cylinder0);

	


}

int main() 
{
	glm::vec4 background_color(0.73,0.83,0.90,1);
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
