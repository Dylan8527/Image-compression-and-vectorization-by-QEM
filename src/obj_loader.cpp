/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-10 01:41:55
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-10 13:29:38
 * @FilePath: \CS271assignment3\src\obj_loader.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#include <obj_loader.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>

bool loadObj(const std::string &path, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float scale, vec3 translation){
  std::clog << "-- Loading model " << path << std::endl;
  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = "./";  // Path to material files

  tinyobj::ObjReader reader;
  std::string input = path;
  for (int dep = 0; dep < 5; ++dep) {
    if (!reader.ParseFromFile(input, readerConfig)) {
      input = "../" + input;
      if (dep < 4) {
        continue;
      }
      if (!reader.Error().empty()) {
        std::clog << "TinyObjReader: " << reader.Error();
      }
      exit(1);
    }
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  vertices.resize(attrib.vertices.size()/3);
  for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
    vertices[int(i/3)].position = vec3(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
    vertices[int(i/3)].color = vec3(attrib.colors[i], attrib.colors[i + 1], attrib.colors[i + 2]);
  }
  // shifting and scaling 
  for (auto &v : vertices) v.position = v.position * scale + translation;

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        indices.push_back(idx.vertex_index);
        vertices[idx.vertex_index].normal =  normalize(vec3(
                attrib.normals[idx.normal_index * 3 + 0],
                attrib.normals[idx.normal_index * 3 + 1],
                attrib.normals[idx.normal_index * 3 + 2]));
      }
      index_offset += fv;
    }
  }

  std::clog << "  # vertices: " << attrib.vertices.size() / 3<< std::endl;
  std::clog << "  # faces: " << indices.size() / 3 << std::endl;
  return true;
}