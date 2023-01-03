/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-10 01:30:11
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-10 01:35:12
 * @FilePath: \CS271assignment3\include\obj_loader.h
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_
#include <geometry.h>
#include <iostream>
#include <memory>

bool loadObj(const std::string &path, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float scale, vec3 translation);

#endif  // CS271_INCLUDE_OBJ_LOADER_H_