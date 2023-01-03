/*
 * @Author: Dylan8527 vvm8933@gmail.com
 * @Date: 2022-11-09 21:43:11
 * @LastEditors: Dylan8527 vvm8933@gmail.com
 * @LastEditTime: 2022-11-12 02:30:53
 * @FilePath: \CS271assignment3\src\main.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
 */
#include <utils.h>
#include <shader.h>
#include <camera.h>
#include <iostream>
#include <geometry.h>
#include <solver.h>
#include <chrono>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#define INIT_FOV 22.0f
float fov = INIT_FOV;

bool see_line = false;
bool mode = false;

GLFWwindow*  window;
Camera mycamera;

int QEM_target_number = -1;

int main(int argc, char* argv[])
{
    // read 'QEM_target_number' from command line
    // -----------------------------------------
    if(argc > 1)
        QEM_target_number = std::stoi(argv[1]);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CS271-ASSIGNMENT3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // Init glew.
    if (glewInit() != GLEW_OK)
        LOG_ERR("failed to init glew");
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // shader
    // --------------------
    Shader shader1("src/shaders/projective.vs", "src/shaders/direct_lighting.fs");
    Shader shader2("src/shaders/projective.vs", "src/shaders/only_black.fs");
    
    ShaderParam shader_param;
    shader_param.projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH/(float)SCR_HEIGHT,  0.1f, 100.0f);

    Faces faces(getPath("data/panic.obj"), 1.0f, vec3(0.0f, 0.0f, 0.0f));
    Lines lines(faces.GetVertices(), faces.GetIndices());
    //TODO: Use NewQEM algorithm to simplify the mesh.
    
    std::vector<Vertex> simplified_vertices;
    std::vector<unsigned int> simplified_indices;
    if(QEM_target_number < 0){
        printf("\n Please input the target number of simplified faces in command line: \n");
        scanf("%d", &QEM_target_number);
        while(QEM_target_number < 0){
            printf("Invalid target number, please input again: \n");
            scanf("%d", &QEM_target_number);
        }
    }
    
    NewQEM_sovler mysolver(QEM_target_number);
    // record solve time 
    auto start = std::chrono::steady_clock::now();
    mysolver.Solve(&faces.GetVertices(), &faces.GetIndices(), &simplified_vertices, &simplified_indices);
    auto end = std::chrono::steady_clock::now();
    auto diff = (end - start) / std::chrono::milliseconds(1);
    printf("Solve time: %lld ms\n", diff);

    Faces simplified_faces(simplified_vertices, simplified_indices);
    Lines simplified_lines(simplified_vertices, simplified_indices);
    
    // render loop
    // -----------
    while(!glfwWindowShouldClose(window))
    {
        // input 
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if(!mode){
            faces.Draw(shader1, shader_param);
            if(see_line)
                lines.Draw(shader2, shader_param);
        }
        else{
            simplified_faces.Draw(shader1, shader_param);
            if(see_line)
                simplified_lines.Draw(shader2, shader_param);
        }
            
        // shader
        // --------------------
        shader_param.view = mycamera.getViewMatrix();
        shader_param.cameraPos = mycamera.Position;
        shader_param.projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH/(float)SCR_HEIGHT,  0.1f, 100.0f);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mycamera.processWalkAround(Upward);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mycamera.processWalkAround(Downward);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mycamera.processWalkAround(Leftward);   
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mycamera.processWalkAround(Rightward);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        mycamera.reset();
        fov = INIT_FOV;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        see_line = !see_line;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
        std::cout << "camera position: " << mycamera.Position.x << " " << mycamera.Position.y << " " << mycamera.Position.z << std::endl;
        std::cout << "camera fov: " << fov << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        mode = !mode;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if(fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 45.0f)
        fov = 45.0f;
}