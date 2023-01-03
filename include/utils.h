/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-09 22:20:25
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-09 23:22:26
 * @FilePath: \CS271assignment3\include\utils.h
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#ifndef _UTILS_H_
#define _UTILS_H_
#include "defines.h"
#include <string>

// Try and search target file in parent directories with max depth limit
std::string getPath(const std::string &target, int depth = 5);


#endif