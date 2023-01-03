/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-10 01:41:07
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-10 14:51:02
 * @FilePath: \CS271assignment3\src\opengl_object.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#include <opengl_object.h>

OpenGLObject::OpenGLObject()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void OpenGLObject::InitData()
{
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // bind buffers
    // configue vertex attributes
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2); // color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
    
    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLObject::UpdateIndices()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void OpenGLObject::Draw(Shader& shader, ShaderParam& shader_param)
{
    shader.use();
    shader.setMat4("model", M);
    shader.setMat4("view", shader_param.view);
    shader.setMat4("projection", shader_param.projection);

    glBindVertexArray(VAO);
    glDrawElements(draw_option.primitive_mode, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}