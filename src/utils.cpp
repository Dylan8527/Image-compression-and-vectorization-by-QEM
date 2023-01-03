/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-09 23:22:38
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-09 23:55:02
 * @FilePath: \CS271assignment3\src\utils.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#include <utils.h>

#ifdef USE_FILESYSTEM
#include <filesystem>

std::string getPath(const std::string &target, int depth)
{
  std::string path = target;
  namespace fs = std::filesystem;
  for (int i = 0; i < depth; ++i)
  {
    if (fs::exists(path))
    {
      return path;
    }
    path = "../" + path;
  }
  LOG_ERR("failed to get file: " + target);
  return target;
}
#else
#include <cstdio>

std::string getPath(const std::string &target, int depth)
{
  std::string path = target;
  for (int i = 0; i < depth; ++i)
  {
    FILE *file = fopen(path.c_str(), "r");
    if (file)
    {
      fclose(file);
      printf("%s\n", path.c_str());
      return path;
    }
    path = "../" + path;
  }
  LOG_ERR("failed to get file: " + target);
  return target;
}
#endif