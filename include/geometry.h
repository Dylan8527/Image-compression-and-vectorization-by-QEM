#ifndef _geometry_
#define _geometry_

#include <shader.h>
#include <opengl_object.h>
#include <algorithm>

class Geometry
{
protected:
    OpenGLObject m_opengl_object; // OpenGL object for rendering.

public:
    Geometry(){}; // read from obj file

    ~Geometry(){};

    // Draw the geometry based on the OpenGLObject.
    virtual void Draw(Shader &shader, ShaderParam &shader_param) = 0;
    std::vector<Vertex> GetVertices() {return m_opengl_object.vertices;};
    std::vector<unsigned int> GetIndices() {return m_opengl_object.indices;};
};


class Faces: public Geometry
{   
public:
    Faces(
        const std::string &path,
        float scale,
        vec3 translation
    ); // read from obj file

    Faces(
        std::vector<Vertex> vertices,
        std::vector<unsigned int> indices
    ); 
    ~Faces() = default;

    void Draw(Shader &shader, ShaderParam &shader_param);
};

class Lines: public Geometry
{   
public:
    Lines(
        std::vector<Vertex> &vertices,
        std::vector<unsigned int> &indices
    );
    ~Lines() = default;

    void Draw(Shader &shader, ShaderParam &shader_param);
};
#endif 