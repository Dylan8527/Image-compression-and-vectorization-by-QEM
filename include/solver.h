/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-10 13:47:39
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-10 21:06:23
 * @FilePath: \CS271assignment3\include\solver.h
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "defines.h"
#include <opengl_object.h>
#include <geometry.h>

class Solver {
public:
    Solver(){};
    ~Solver(){};

    virtual void Solve(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices,
    std::vector<Vertex>* simplified_vertices, std::vector<unsigned int>* simplified_indices) = 0;

};

class NewQEM_sovler : public Solver {
public:
    int target_number;
    NewQEM_sovler(int target_number):target_number(target_number) {};

    void Solve(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices,
    std::vector<Vertex>* simplified_vertices, std::vector<unsigned int>* simplified_indices);
    
    void Simplify(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices,
    std::vector<Vertex>* simplified_vertices, std::vector<unsigned int>* simplified_indices);

	void DoRGBtoLABConversion(std::vector<vec3> &colors);
    void RGB2XYZ(std::vector<vec3> &colors);
    void XYZ2LAB(std::vector<vec3> &colors);

    void DoLABtoRGBConversion(std::vector<vec3> &colors);
    void LAB2XYZ(std::vector<vec3> &colors);
    void XYZ2RGB(std::vector<vec3> &colors);

};
#endif
