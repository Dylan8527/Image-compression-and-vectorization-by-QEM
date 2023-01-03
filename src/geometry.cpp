/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-10 13:14:57
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-11 21:53:01
 * @FilePath: \CS271assignment3\src\geometry.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#include <geometry.h>
#include <obj_loader.h>

Faces::Faces(const std::string &path, float scale, vec3 translation)
{
    std::cout << "Generating Geometry from obj file " << std::endl;
    auto& vertices = m_opengl_object.vertices;
    auto& indices = m_opengl_object.indices;
    if (!loadObj(path, vertices, indices, scale, translation))
    {
        LOG_ERR("failed to load obj file: " + path);
    }

    m_opengl_object.M = glm::mat4(1.0f);
    m_opengl_object.InitData();
}

Faces::Faces(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    m_opengl_object.vertices = vertices;
    m_opengl_object.indices = indices;
    m_opengl_object.M = glm::mat4(1.0f);
    m_opengl_object.InitData();
}

void Faces::Draw(Shader &shader, ShaderParam &shader_param)
{
    m_opengl_object.draw_option.primitive_mode = GL_TRIANGLES;
    m_opengl_object.Draw(shader, shader_param);
}

Lines::Lines(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices)
{
    m_opengl_object.vertices = vertices;
    // m_opengl_object.indices = indices;
    std::vector<unsigned int> new_indices(indices.size()*2);
    int num_points = 0;
    for (int i = 0; i < indices.size(); i+=3)
    {
        // only add half of the indices
        if(vertices[indices[i]].position.x > 0.0f ||
        vertices[indices[i+1]].position.x > 0.0f  ||
        vertices[indices[i+2]].position.x > 0.0f) continue;
        
        new_indices[i*2] = indices[i];
        new_indices[i*2+1] = indices[i+1];

        new_indices[i*2+2] = indices[i+1];
        new_indices[i*2+3] = indices[i+2];

        new_indices[i*2+4] = indices[i+2];
        new_indices[i*2+5] = indices[i];
        num_points += 6;
    }
    // int num_points = new_indices.size() / 2;
    // if(num_points & 1) num_points++;
    // new_indices.resize(num_points);
    new_indices.resize(num_points);
    printf("# lines: %lld", new_indices.size()/2);
    m_opengl_object.indices = new_indices;
    m_opengl_object.M = glm::mat4(1.0f);
    m_opengl_object.InitData();
}

void Lines::Draw(Shader &shader, ShaderParam &shader_param)
{
    m_opengl_object.draw_option.primitive_mode = GL_LINES;
    m_opengl_object.Draw(shader, shader_param);
}

