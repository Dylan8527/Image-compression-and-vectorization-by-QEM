/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-10 01:39:55
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-10 14:19:56
 * @FilePath: \CS271assignment3\include\opengl_object.h
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#ifndef _OPENGL_OBJECT_
#define _OPENGL_OBJECT_

#include <shader.h>

struct Vertex
{
  glm::vec3 position; // Vertex position.
  glm::vec3 normal;   // Vertex normal.
  glm::vec3 color;    // Vertex color.
};

struct DrawOption
{
  bool draw_element;
  GLenum primitive_mode;

  DrawOption() : draw_element(true),
                 primitive_mode(GL_TRIANGLES)
  {
  }
};

// Class to define an object for rendering with OpenGL.
class OpenGLObject
{
private:
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

public:
  glm::vec3 color; // Material color.

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  glm::mat4 M; // Model transformation matrix.

  DrawOption draw_option;

  OpenGLObject();
  ~OpenGLObject() = default;

  void InitData();
  // Draw the current object.
  void Draw(Shader &shader, ShaderParam &shader_param);
  void UpdateIndices();
};

#endif // _OPENGL_OBJECT_