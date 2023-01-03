/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-10 13:54:04
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-14 20:11:49
 * @FilePath: \CS271assignment3\src\solver.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#include "solver.h"
#include <cmath>
#include <set>
#include <omp.h>

struct face{
    unsigned int indices[3];
    float area;
    vec3 barycentric_coord;
    vec3 barycentric_color;
};

struct edge{
    unsigned int indices[2];
    float QEM;
};

std::vector<bool> at_boundary(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices){
    std::vector<bool> boundary(vertices->size(), false);
    
    float max_x = -1e9, max_y = -1e9;
    float min_x = 1e9, min_y = 1e9;
    for (int i = 0; i < vertices->size(); i++){
        max_x = std::max(max_x, (*vertices)[i].position.x);
        max_y = std::max(max_y, (*vertices)[i].position.y);
        min_x = std::min(min_x, (*vertices)[i].position.x);
        min_y = std::min(min_y, (*vertices)[i].position.y);
    }
    for (int i = 0; i < vertices->size(); i++){
        if ((*vertices)[i].position.x == max_x || (*vertices)[i].position.x == min_x ||
            (*vertices)[i].position.y == max_y || (*vertices)[i].position.y == min_y){
            boundary[i] = true;
        }
    }
    return boundary;
}

void NewQEM_sovler::Solve(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices,
    std::vector<Vertex>* simplified_vertices, std::vector<unsigned int>* simplified_indices)
{
   
    float lambda = 1.f; // weight of distance

    *simplified_vertices = *vertices;
    *simplified_indices  = *indices;
    
    std::vector<vec3> colors(simplified_vertices->size());
    for (int i = 0; i < simplified_vertices->size(); i++)
        colors[i] = (*simplified_vertices)[i].color;
    // convert sRGB to CIELAB
    DoRGBtoLABConversion(colors);
    // update color
    for (int i = 0; i < simplified_vertices->size(); i++)
        (*simplified_vertices)[i].color = colors[i];


    while (simplified_indices->size() / 3> target_number)
    {
        // QEM(e) = QEM(v1) + QEM(v2)
        // QEM(v) = sum of QEM of all the faces adjacent to v
        // QEM(v, f) = area(f) ( (p-p0)*(p-p0) + (c-c0)*(c-c0) )
        // p0, c0 are the barycentric coordinate of the face f
        // p, c are the barycentric coordinate of the vertex v
        // area(f) is the area of the face f
        // QEM(e) = QEM(v1) + QEM(v2) = sum of QEM of all the faces adjacent to v1 + sum of QEM of all the faces adjacent to v2
        // QEM(e) = sum of QEM of all the faces adjacent to v1 + sum of QEM of all the faces adjacent to v2
        
        // first construct the faces
        std::vector<face> faces(simplified_indices->size() / 3);
        // #pragma omp parallel for schedule(static) shared(faces)
        for(int i = 0; i < simplified_indices->size(); i+=3) {
            face f;
            f.indices[0] = (*simplified_indices)[i];
            f.indices[1] = (*simplified_indices)[i+1];
            f.indices[2] = (*simplified_indices)[i+2];
            f.area = 0.5 * glm::length(glm::cross((*simplified_vertices)[f.indices[1]].position - (*simplified_vertices)[f.indices[0]].position, (*simplified_vertices)[f.indices[2]].position - (*simplified_vertices)[f.indices[0]].position));
            f.barycentric_coord = (1.0f / 3.0f) * ((*simplified_vertices)[f.indices[0]].position + (*simplified_vertices)[f.indices[1]].position + (*simplified_vertices)[f.indices[2]].position);
            f.barycentric_color = (1.0f / 3.0f) * ((*simplified_vertices)[f.indices[0]].color + (*simplified_vertices)[f.indices[1]].color + (*simplified_vertices)[f.indices[2]].color);
            faces[i/3] = f;
        }

        // then construct the QEM of each vertex
        std::vector<float> QEMs(simplified_vertices->size(), 0);
        for(int i = 0; i < simplified_indices->size(); i+=3) {
            face f = faces[i/3];
            for(int j = 0; j < 3; j++){
                QEMs[f.indices[j]] += f.area * (lambda * glm::dot(f.barycentric_coord - (*simplified_vertices)[f.indices[j]].position, f.barycentric_coord - (*simplified_vertices)[f.indices[j]].position) + glm::dot(f.barycentric_color - (*simplified_vertices)[f.indices[j]].color, f.barycentric_color - (*simplified_vertices)[f.indices[j]].color));
            }
            //TODO: penalize sharp shape variation (e.g. sharp edges)
            
        }
        
        // then construct the QEM of each edge
        std::vector<edge> edge_QEMs(simplified_indices->size());
        // #pragma omp parallel for shared(QEMs) schedule(static) 
        for(int i = 0; i < simplified_indices->size(); i+=3) {
            face f = faces[i/3];
            for(int j = 0; j < 3; j++) {
                edge e;
                e.indices[0] = f.indices[j];
                e.indices[1] = f.indices[(j+1)%3];
                e.QEM = QEMs[e.indices[0]] + QEMs[e.indices[1]];
                edge_QEMs[i+j] = e;
            }
        }

        // then find the edge with the minimum QEM
        // boundary
        std::vector<bool> boundary = at_boundary(simplified_vertices, simplified_indices);
        float min_QEM = -1;
        int min_index = 0;
        for(int i = 1; i < edge_QEMs.size(); i++) {
            // don't delete the edge on the boundary
            if(boundary[edge_QEMs[i].indices[0]] || boundary[edge_QEMs[i].indices[1]])
                continue;
            if(edge_QEMs[i].QEM < min_QEM || min_QEM == -1) {
                min_QEM = edge_QEMs[i].QEM;
                min_index = i;
            }
        }
        edge min_edge = edge_QEMs[min_index];

        // then collapse the edge
        // first find the vertex with the larger index
        int v1 = min_edge.indices[0];
        int v2 = min_edge.indices[1];
        if(v1 > v2) {
            int temp = v1;
            v1 = v2;
            v2 = temp;
        }
        //update the v1
        // (*simplified_vertices)[v1].position = (1.0f / 2.0f) * ((*simplified_vertices)[v1].position + (*simplified_vertices)[v2].position);
        // (*simplified_vertices)[v1].color = (1.0f / 2.0f) * ((*simplified_vertices)[v1].color + (*simplified_vertices)[v2].color);
        for(auto it = simplified_indices->begin(); it != simplified_indices->end(); it+=3) {
            if((*it == v1 && *(it+1) == v2) || (*it == v2 && *(it+1) == v1) || (*it == v1 && *(it+2) == v2) || (*it == v2 && *(it+2) == v1) || (*(it+1) == v1 && *(it+2) == v2) || (*(it+1) == v2 && *(it+2) == v1)) {
                simplified_indices->erase(it, it+3);
                it -= 3;
            }
        }

        float area_sum = 0;
        glm::vec3 barycentric_coord_sum(0, 0, 0);
        glm::vec3 barycentric_color_sum(0, 0, 0);
        for(int i = 0; i < simplified_indices->size(); i+=3) {
            face f = faces[i/3];
            for(int j = 0; j < 3; j++) {
                if(f.indices[j] == v1 || f.indices[j] == v2) {
                    area_sum += f.area;
                    barycentric_coord_sum += f.area * f.barycentric_coord;
                    barycentric_color_sum += f.area * f.barycentric_color;
                    break;
                }
            }
        }
        (*simplified_vertices)[v1].position = barycentric_coord_sum / area_sum;
        (*simplified_vertices)[v1].color = barycentric_color_sum / area_sum;
        // then update the indices
        // also delete the face that contains the edge

        for(auto it = simplified_indices->begin(); it != simplified_indices->end(); it++) {
            if(*it == v2)
                *it = v1;
            else if(*it > v2)
                *it -= 1;
        }
        
        // then update the vertices
        simplified_vertices->erase(simplified_vertices->begin() + v2);
        printf("simplified_faces size: %lld\n", simplified_indices->size() / 3);
        static int ii = 0;
        printf("v1 = %d, v2 = %d\n", v1, v2);
        ii++;
        if(ii == 500){
            while(1);
        }

    }
    for(int i = 0; i < simplified_vertices->size(); i++) {
        colors[i] = (*simplified_vertices)[i].color;
    }
    // convert CIELAB to sRGB
    DoLABtoRGBConversion(colors);
    for (int i = 0; i < simplified_vertices->size(); i++)
        (*simplified_vertices)[i].color = colors[i];
        
    printf("simplified_faces size: %lld", simplified_indices->size() / 3);
}

void NewQEM_sovler::Simplify(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices,
    std::vector<Vertex>* simplified_vertices, std::vector<unsigned int>* simplified_indices)
{
    
}

void NewQEM_sovler::DoRGBtoLABConversion(std::vector<vec3> &colors)
{
    RGB2XYZ(colors);
    XYZ2LAB(colors);
}

void NewQEM_sovler::RGB2XYZ(std::vector<vec3> &colors)
{
    for (int i = 0; i < colors.size(); i++)
    {
        float r = colors[i].x;
        float g = colors[i].y;
        float b = colors[i].z;

        if(r <= 0.04045)	r = r/12.92;
        else				r = pow((r+0.055)/1.055,2.4);
        if(g <= 0.04045)	g = g/12.92;
        else				g = pow((g+0.055)/1.055,2.4);
        if(b <= 0.04045)	b = b/12.92;
        else				b = pow((b+0.055)/1.055,2.4);

        float x = 0.412453f * r + 0.357580f * g + 0.180423f * b;
        float y = 0.212671f * r + 0.715160f * g + 0.072169f * b;
        float z = 0.019334f * r + 0.119193f * g + 0.950227f * b;

        colors[i].x = x;
        colors[i].y = y;
        colors[i].z = z;
    }
}

void NewQEM_sovler::XYZ2LAB(std::vector<vec3> &colors)
{
    for (int i = 0; i < colors.size(); i++)
    {
        float x = colors[i].x;
        float y = colors[i].y;
        float z = colors[i].z;

        float epsilon = 0.008856f;	//actual CIE standard
        float kappa   = 903.3f;		//actual CIE standard

        float Xr = 0.950456f;	//reference white
        float Yr = 1.0f;		//reference white
        float Zr = 1.088754f;	//reference white

        float xr = x / Xr;
        float yr = y / Yr;
        float zr = z / Zr;

        float fx, fy, fz;
        if ( xr > epsilon )
            fx = pow(xr, 1.0f/3.0f);
        else
            fx = ( kappa * xr + 16.0f ) / 116.0f;

        if ( yr > epsilon )
            fy = pow(yr, 1.0f/3.0f);
        else
            fy = ( kappa * yr + 16.0f ) / 116.0f;

        if ( zr > epsilon )
            fz = pow(zr, 1.0f/3.0f);
        else
            fz = ( kappa * zr + 16.0f ) / 116.0f;

        float L = 116.0f * fy - 16.0f;
        float a = 500.0f * ( fx - fy );
        float b = 200.0f * ( fy - fz );

        colors[i].x = L;
        colors[i].y = a;
        colors[i].z = b;
    }
}

void NewQEM_sovler::DoLABtoRGBConversion(std::vector<vec3> &colors)
{
    LAB2XYZ(colors);
    XYZ2RGB(colors);
}

void NewQEM_sovler::LAB2XYZ(std::vector<vec3> &colors)
{
    for (int i = 0; i < colors.size(); i++)
    {
        float L = colors[i].x;
        float a = colors[i].y;
        float b = colors[i].z;

        float epsilon = 0.008856f;	//actual CIE standard
        float kappa   = 903.3f;		//actual CIE standard

        float Xr = 0.950456f;	//reference white
        float Yr = 1.0f;		//reference white
        float Zr = 1.088754f;	//reference white

        float fy = ( L + 16.0f ) / 116.0f;
        float fx = a / 500.0f + fy;
        float fz = fy - b / 200.0f;

        float xr, yr, zr;
        if ( pow(fx, 3.0f) > epsilon )
            xr = pow(fx, 3.0f);
        else
            xr = ( 116.0f * fx - 16.0f ) / kappa;

        if ( L > kappa * epsilon )
            yr = pow(( L + 16.0f ) / 116.0f, 3.0f);
        else
            yr = L / kappa;

        if ( pow(fz, 3.0f) > epsilon )
            zr = pow(fz, 3.0f);
        else
            zr = ( 116.0f * fz - 16.0f ) / kappa;

        float x = xr * Xr;
        float y = yr * Yr;
        float z = zr * Zr;

        colors[i].x = x;
        colors[i].y = y;
        colors[i].z = z;
    }
}   

void NewQEM_sovler::XYZ2RGB(std::vector<vec3> &colors)
{
    for (int i = 0; i < colors.size(); i++)
    {
        float x = colors[i].x;
        float y = colors[i].y;
        float z = colors[i].z;

        float r =  3.240479f * x - 1.537150f * y - 0.498535f * z;
        float g = -0.969256f * x + 1.875992f * y + 0.041556f * z;
        float b =  0.055648f * x - 0.204043f * y + 1.057311f * z;

        if(r <= 0.0031308)	r = 12.92f * r;
        else				r = 1.055f * pow(r,1.0f/2.4f) - 0.055f;
        if(g <= 0.0031308)	g = 12.92f * g;
        else				g = 1.055f * pow(g,1.0f/2.4f) - 0.055f;
        if(b <= 0.0031308)	b = 12.92f * b;
        else				b = 1.055f * pow(b,1.0f/2.4f) - 0.055f;

        colors[i].x = r;
        colors[i].y = g;
        colors[i].z = b;
    }
}
