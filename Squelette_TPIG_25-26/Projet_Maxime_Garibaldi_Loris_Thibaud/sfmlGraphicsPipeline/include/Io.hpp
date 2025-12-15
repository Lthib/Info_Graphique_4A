// #ifndef IO_HPP
// #define IO_HPP

// /**@file
//  *@brief Input/Output functions.
//  *
//  * Currently, this file only contains I/O functions for OBJ meshes.*/

// #include <vector>
// #include <glm/glm.hpp>
// #include <string>

// /**@brief Collect mesh data from an OBJ file.
//  *
//  * This function opens an OBJ mesh file to collect information such
//  * as vertex position, vertex indices of a face, vertex normals and vertex
//  * texture coordinates.
//  *
//  * @param filename The path to the mesh file.
//  * @param positions The vertex positions.
//  * @param indices The vertex indices of faces.
//  * @param normals The vertex normals.
//  * @param texcoords The vertex texture coordinates.
//  * @return False if import failed, true otherwise.
//  */
// bool read_obj(
//         const std::string& filename, 
//         std::vector<glm::vec3>& positions, 
//         std::vector<unsigned int>& indices,
//         std::vector<glm::vec3>& normals,
//         std::vector<glm::vec2>& texcoords
//         );

// #endif //IO_HPP
#ifndef IO_HPP
#define IO_HPP

/**@file
 *@brief Input/Output functions.
 *
 * Currently, this file only contains I/O functions for OBJ meshes.*/

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <lighting/Material.hpp>

/**@brief Collect mesh data from an OBJ file.
 *
 * This function opens an OBJ mesh file to collect information such
 * as vertex position, vertex indices of a face, vertex normals and vertex
 * texture coordinates.
 *
 * @param filename The path to the mesh file.
 * @param positions The vertex positions.
 * @param indices The vertex indices of faces.
 * @param normals The vertex normals.
 * @param texcoords The vertex texture coordinates.
 * @return False if import failed, true otherwise.
 */
bool read_obj(
        const std::string& filename, 
        std::vector<glm::vec3>& positions, 
        std::vector<unsigned int>& indices,
        std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& texcoords
        );

bool read_obj_with_materials(
        const std::string& obj_path,
        const std::string& mtl_basepath,
        std::vector<std::vector<glm::vec3>>& all_positions,
        std::vector<std::vector<glm::vec3>>& all_normals,
        std::vector<std::vector<glm::vec2>>& all_texcoords,
        std::vector<MaterialPtr>& materials
        );

bool read_obj_with_materials_indexed(
        const std::string& obj_path,
        const std::string& mtl_basepath,
        std::vector<std::vector<glm::vec3>>& all_positions,
        std::vector<std::vector<glm::vec3>>& all_normals,
        std::vector<std::vector<glm::vec2>>& all_texcoords,
        std::vector<std::vector<unsigned int>>& all_indices,
        std::vector<MaterialPtr>& materials
        );


#endif //IO_HPP

