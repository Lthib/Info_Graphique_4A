#ifndef CUBE_RENDERABLE_HPP
#define CUBE_RENDERABLE_HPP

#include "Renderable.hpp"
#include <vector>
#include <glm/glm.hpp>

class CubeRenderable : public Renderable
{
    public:
        ~CubeRenderable();
        CubeRenderable( ShaderProgramPtr program );

    private:
        void do_draw();
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> tcoords;

        std::vector< glm::vec3 > m_positions;
        unsigned int m_vBuffer;
        std::vector< glm::vec4 > m_colors;
        unsigned int m_cBuffer;

        //glm::mat4 m_model;
};

typedef std::shared_ptr<CubeRenderable> CubeRenderablePtr;

#endif
