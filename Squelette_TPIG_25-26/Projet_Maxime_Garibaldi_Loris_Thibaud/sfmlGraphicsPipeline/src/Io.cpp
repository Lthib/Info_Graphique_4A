// #include "./../include/Io.hpp"
// #include <iostream>

// #define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
// #include "tiny_obj_loader.h"

// bool read_obj(const std::string& filename,
//         std::vector<glm::vec3>& positions,
//         std::vector<unsigned int>& triangles,
//         std::vector<glm::vec3>& normals,
//         std::vector<glm::vec2>& texcoords
//         )
// {
//     std::vector<tinyobj::shape_t> shapes;
//     std::vector<tinyobj::material_t> materials;
//     std::string err;

//     bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str());

//     if (!err.empty()) 
//     {
//         std::cerr << err << std::endl;
//     }

//     if (!ret) 
//     {
//         return ret;
//     }

//     positions.clear();
//     triangles.clear();
//     normals.clear();
//     texcoords.clear();

//     for (size_t i = 0; i < shapes.size(); i++) 
//     {
//         assert((shapes[i].mesh.indices.size() % 3) == 0);
//         for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) 
//         {
//             triangles.push_back( shapes[i].mesh.indices[f] );
//         }
//         assert((shapes[i].mesh.positions.size() % 3) == 0);
//         for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) 
//         {
//             positions.push_back( glm::vec3(shapes[i].mesh.positions[3*v+0], shapes[i].mesh.positions[3*v+1],shapes[i].mesh.positions[3*v+2]) );
//         }

//         assert((shapes[i].mesh.normals.size() % 3) == 0);
//         for (size_t n = 0; n < shapes[i].mesh.normals.size() / 3; n++)
//         {
//             normals.push_back( glm::vec3(shapes[i].mesh.normals[3*n+0], shapes[i].mesh.normals[3*n+1],shapes[i].mesh.normals[3*n+2]) );
//         }

//         assert((shapes[i].mesh.texcoords.size() % 2) == 0);
//         for (size_t t = 0; t < shapes[i].mesh.texcoords.size() / 2; t++)
//         {
//             texcoords.push_back( glm::vec2(shapes[i].mesh.texcoords[2*t+0], shapes[i].mesh.texcoords[2*t+1]) );
//         }
//     }

//     return ret;
// }

#include "./../include/Io.hpp"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

bool read_obj(const std::string& filename,
        std::vector<glm::vec3>& positions,
        std::vector<unsigned int>& triangles,
        std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& texcoords
        )
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str());

    if (!err.empty()) 
    {
        std::cerr << err << std::endl;
    }

    if (!ret) 
    {
        return ret;
    }

    positions.clear();
    triangles.clear();
    normals.clear();
    texcoords.clear();

    for (size_t i = 0; i < shapes.size(); i++) 
    {
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) 
        {
            triangles.push_back( shapes[i].mesh.indices[f] );
        }
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) 
        {
            positions.push_back( glm::vec3(shapes[i].mesh.positions[3*v+0], shapes[i].mesh.positions[3*v+1],shapes[i].mesh.positions[3*v+2]) );
        }

        assert((shapes[i].mesh.normals.size() % 3) == 0);
        for (size_t n = 0; n < shapes[i].mesh.normals.size() / 3; n++)
        {
            normals.push_back( glm::vec3(shapes[i].mesh.normals[3*n+0], shapes[i].mesh.normals[3*n+1],shapes[i].mesh.normals[3*n+2]) );
        }

        assert((shapes[i].mesh.texcoords.size() % 2) == 0);
        for (size_t t = 0; t < shapes[i].mesh.texcoords.size() / 2; t++)
        {
            texcoords.push_back( glm::vec2(shapes[i].mesh.texcoords[2*t+0], shapes[i].mesh.texcoords[2*t+1]) );
        }
    }

    return ret;
}

bool read_obj_with_materials(
        const std::string& obj_path,
        const std::string& mtl_basepath,
        std::vector<std::vector<glm::vec3>>& all_positions,
        std::vector<std::vector<glm::vec3>>& all_normals,
        std::vector<std::vector<glm::vec2>>& all_texcoords,
        std::vector<MaterialPtr>& materials
        )
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> tobj_materials;
    std::string err;

    bool ret = tinyobj::LoadObj(shapes, tobj_materials, err, obj_path.c_str(), mtl_basepath.c_str());
    if (!err.empty()) 
    {
        std::cerr << err << std::endl;
    }

    if (!ret) 
    {
        return ret;
    }

    all_positions.clear();
    all_normals.clear();
    all_texcoords.clear();
    materials.clear();

    int N = tobj_materials.size();


    all_positions.resize(N);
    all_normals.resize(N);
    all_texcoords.resize(N);


    for (size_t i = 0; i < N; i++)
    {
        tinyobj::material_t tobj_mat = tobj_materials[i];
        glm::vec3 ambient(tobj_mat.ambient[0], tobj_mat.ambient[1], tobj_mat.ambient[2]);
        glm::vec3 diffuse(tobj_mat.diffuse[0], tobj_mat.diffuse[1], tobj_mat.diffuse[2]);
        glm::vec3 specular(tobj_mat.specular[0], tobj_mat.specular[1], tobj_mat.specular[2]);
        float shininess = tobj_mat.shininess;
        MaterialPtr mat = std::make_shared<Material>(ambient, diffuse, specular, shininess);
        materials.push_back(mat);
    }


    for (size_t i = 0; i < shapes.size(); i++) 
    {   
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        assert((shapes[i].mesh.normals.size() % 3) == 0);
        assert((shapes[i].mesh.texcoords.size() % 2) == 0);

        size_t n_positions = shapes[i].mesh.positions.size() / 3;
        size_t n_normals = shapes[i].mesh.normals.size() / 3;
        size_t n_texcoords = shapes[i].mesh.texcoords.size() / 2;

        for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) 
        {   
            int iv0 = shapes[i].mesh.indices[ 3 * f + 0];
            int iv1 = shapes[i].mesh.indices[ 3 * f + 1];
            int iv2 = shapes[i].mesh.indices[ 3 * f + 2];

            int mat_id = shapes[i].mesh.material_ids[f];

            all_positions[mat_id].push_back(glm::vec3(
                shapes[i].mesh.positions[3 * iv0 + 0],
                shapes[i].mesh.positions[3 * iv0 + 1],
                shapes[i].mesh.positions[3 * iv0 + 2]));
            all_positions[mat_id].push_back(glm::vec3(
                shapes[i].mesh.positions[3 * iv1 + 0],
                shapes[i].mesh.positions[3 * iv1 + 1],
                shapes[i].mesh.positions[3 * iv1 + 2]));
            all_positions[mat_id].push_back(glm::vec3(
                shapes[i].mesh.positions[3 * iv2 + 0],
                shapes[i].mesh.positions[3 * iv2 + 1],
                shapes[i].mesh.positions[3 * iv2 + 2]));
            if (n_normals == n_positions){
                all_normals[mat_id].push_back(glm::vec3(
                    shapes[i].mesh.normals[3 * iv0 + 0],
                    shapes[i].mesh.normals[3 * iv0 + 1],
                    shapes[i].mesh.normals[3 * iv0 + 2]));
                all_normals[mat_id].push_back(glm::vec3(
                    shapes[i].mesh.normals[3 * iv1 + 0],
                    shapes[i].mesh.normals[3 * iv1 + 1],
                    shapes[i].mesh.normals[3 * iv1 + 2]));
                all_normals[mat_id].push_back(glm::vec3(
                    shapes[i].mesh.normals[3 * iv2 + 0],
                    shapes[i].mesh.normals[3 * iv2 + 1],
                    shapes[i].mesh.normals[3 * iv2 + 2]));
            }
            if (n_texcoords == n_positions){
                all_texcoords[mat_id].push_back(glm::vec2(
                    shapes[i].mesh.texcoords[2 * iv0 + 0],
                    shapes[i].mesh.texcoords[2 * iv0 + 1]));
                all_texcoords[mat_id].push_back(glm::vec2(
                    shapes[i].mesh.texcoords[2 * iv1 + 0],
                    shapes[i].mesh.texcoords[2 * iv1 + 1]));
                all_texcoords[mat_id].push_back(glm::vec2(
                    shapes[i].mesh.texcoords[2 * iv2 + 0],
                    shapes[i].mesh.texcoords[2 * iv2 + 1]));
            }
        }
    }

    for (size_t i = 0; i < materials.size() ; ++i)
    {
        size_t n_positions = all_positions[i].size() / 3;
        size_t n_normals = all_normals[i].size() / 3;
        size_t n_texcoords = all_texcoords[i].size() / 2;
        if (n_normals != n_positions){
            all_normals[i].clear();
            all_normals[i].resize(n_positions, glm::vec3(0,1,0));
        }
        if (n_texcoords != n_positions){
            all_texcoords[i].clear();
            all_texcoords[i].resize(n_positions, glm::vec2(0,0));
        }
    }

    return ret;
}


bool read_obj_with_materials_indexed(
        const std::string& obj_path,
        const std::string& mtl_basepath,
        std::vector<std::vector<glm::vec3>>& all_positions,
        std::vector<std::vector<glm::vec3>>& all_normals,
        std::vector<std::vector<glm::vec2>>& all_texcoords,
        std::vector<std::vector<unsigned int>>& all_indices,
        std::vector<MaterialPtr>& materials
        )
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> tobj_materials;
    std::string err;

    bool ret = tinyobj::LoadObj(shapes, tobj_materials, err, obj_path.c_str(), mtl_basepath.c_str());
    if (!err.empty()) 
    {
        std::cerr << err << std::endl;
    }

    if (!ret) 
    {
        return ret;
    }

    all_positions.clear();
    all_normals.clear();
    all_texcoords.clear();
    all_indices.clear();
    materials.clear();

    int N = tobj_materials.size();

    all_positions.resize(N);
    all_normals.resize(N);
    all_texcoords.resize(N);
    all_indices.resize(N);

    for (size_t i = 0; i < N; i++) {
        tinyobj::material_t tobj_mat = tobj_materials[i];
        glm::vec3 ambient(tobj_mat.ambient[0], tobj_mat.ambient[1], tobj_mat.ambient[2]);
        glm::vec3 diffuse(tobj_mat.diffuse[0], tobj_mat.diffuse[1], tobj_mat.diffuse[2]);
        glm::vec3 specular(tobj_mat.specular[0], tobj_mat.specular[1], tobj_mat.specular[2]);
        float shininess = tobj_mat.shininess;
        MaterialPtr mat = std::make_shared<Material>(ambient, diffuse, specular, shininess);
        materials.push_back(mat);
    }

    for (size_t i = 0; i < shapes.size(); i++) 
    {   
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        assert((shapes[i].mesh.normals.size() % 3) == 0);
        assert((shapes[i].mesh.texcoords.size() % 2) == 0);

        size_t n_positions = shapes[i].mesh.positions.size() / 3;
        size_t n_normals = shapes[i].mesh.normals.size() / 3;
        size_t n_texcoords = shapes[i].mesh.texcoords.size() / 2;

        std::vector<int> unique_indices(n_positions, -1); // -1 means the vertex is not yet added.

        for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) 
        {   
            int mat_id = shapes[i].mesh.material_ids[f];

            for (int v = 0; v < 3; v++) 
            {
                int iv = shapes[i].mesh.indices[3 * f + v];

                if (unique_indices[iv] == -1) // Vertex not yet added
                {
                    unique_indices[iv] = all_positions[mat_id].size();

                    all_positions[mat_id].push_back(glm::vec3(
                        shapes[i].mesh.positions[3 * iv + 0],
                        shapes[i].mesh.positions[3 * iv + 1],
                        shapes[i].mesh.positions[3 * iv + 2]));

                    if (n_normals == n_positions) 
                    {
                        all_normals[mat_id].push_back(glm::vec3(
                            shapes[i].mesh.normals[3 * iv + 0],
                            shapes[i].mesh.normals[3 * iv + 1],
                            shapes[i].mesh.normals[3 * iv + 2]));
                    }

                    if (n_texcoords == n_positions) 
                    {
                        all_texcoords[mat_id].push_back(glm::vec2(
                            shapes[i].mesh.texcoords[2 * iv + 0],
                            shapes[i].mesh.texcoords[2 * iv + 1]));
                    }
                }

                all_indices[mat_id].push_back(unique_indices[iv]);
            }
        }
    }

    for (int i = 0 ; i < N ; ++i){
        int n_positions = all_positions[i].size();
        int n_normals = all_normals[i].size();
        int n_texcoords = all_texcoords[i].size();
        if (n_normals != n_positions){
            all_normals[i].clear(),
            all_normals[i].resize(n_positions, glm::vec3(0,1,0));
        }
        if (n_texcoords != n_positions){
            all_texcoords[i].clear(),
            all_texcoords[i].resize(n_positions, glm::vec2(0,0));
        }
    }

    return ret;
}

