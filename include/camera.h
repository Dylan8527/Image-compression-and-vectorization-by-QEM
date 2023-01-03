/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-09 22:53:05
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-10 19:50:57
 * @FilePath: \CS271assignment3\include\camera.h
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "defines.h"

enum WalkDirection { Upward, Downward, Leftward, Rightward};

const glm::vec3 camera_origin = glm::vec3(0.0f, 0.0f, 0.0f);

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 WorldUp;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;
    float walkSpeed;
    float mouseSensitivity;
    const float yaw=-90.f;
    const float pitch=0.f;

    Camera()
        : Position(camera_origin),
          WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
          Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          walkSpeed(0.1f),
          mouseSensitivity(0.1f) {
            Right = glm::normalize(glm::cross(Front, WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }
    
    glm::mat4 getViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // set camera position to (0.0f, 0.0f, 15.0f)
    void reset() {
        Position = camera_origin;
    }
    
    void processWalkAround(WalkDirection direction) {
        switch (direction) {
            case Upward:
                Position += Up * walkSpeed;
                break;
            case Downward:
                Position -= Up * walkSpeed;
                break;
            case Leftward:
                Position -= Right * walkSpeed;
                break;
            case Rightward:
                Position += Right * walkSpeed;
                break;
            default:
                break;
        }
    }

};

#endif