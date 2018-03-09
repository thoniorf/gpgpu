#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "include/camera.h"
#include "include/shader.h"
#include "include/model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void instanceSpheres(glm::mat4* modelMatrices,Model sphere, int amount);
void rotateSpheres(glm::mat4* modelMatrices, int amount );

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//shaders path consts
const char* wiredCubeVertex = "./shaders/vertex_shader.vert";
const char* wiredCubeFragment = "./shaders/fragment_shader_wired.frag";
const char* littleCubeVertex = "./shaders/vertex_shader.vert";
const char* littleCubeFragment = "./shaders/fragment_shader_little.frag";
const char* sphereVertex = "./shaders/vertex_shader_sphere.vert";
const char* sphereFragment = "./shaders/fragment_shader_sphere.frag";

//model path const
const char* sphereModel = "./models/sphere.obj";

const int spheresAmount = 10000;

//Camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool keys[1024];
float rotation_x_axis = 0.0f;
float rotation_y_axis = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particles", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // shaders definition
    Shader shaderWiredCube(wiredCubeVertex,wiredCubeFragment);
    Shader shaderLittleCube(littleCubeVertex,littleCubeFragment);
    Shader shaderSpheres(sphereVertex,sphereFragment);

//    // verticies and indexes
//    float vertices_wired_cube[] = {
//        //WIRED CUBE
//        0.65f, 0.65f, 0.65f,
//        0.65f, -0.65f, 0.65f,
//       -0.65f, -0.65f,0.65f,
//       -0.65f, 0.65f, 0.65f,
//        0.65f, 0.65f, -0.65f,
//        0.65f, -0.65f, -0.65f,
//       -0.65f, -0.65f, -0.65f,
//       -0.65f, 0.65f, -0.65f,
//    };

//    unsigned int indices_wired_cube[] = {
//        //WIRED CUBE
//        //back_face
//        0,1,
//        1,2,
//        2,3,
//        3,0,

//        //front_face
//        4,5,
//        5,6,
//        6,7,
//        7,4,

//        //left face
//        3,7,
//        6,7,
//        2,3,
//        6,2,

//        //right face
//        0,4,
//        1,5,
//        0,1,
//        4,5
//    };

//    float vertices_little_cube[] = {
//        //LITTLE CUBE
//        0.15f, 0.15f, 0.15f,
//        0.15f, -0.15f, 0.15f,

//        -0.15f, -0.15f, 0.15f,
//        -0.15f, 0.15f, 0.15f,

//        0.15f, 0.15f, -0.15f,
//        0.15f, -0.15f, -0.15f,

//        -0.15f, -0.15f, -0.15f,
//        -0.15f, 0.15f, -0.15f
//    };

//    unsigned int indices_little_cube[] = {
//        //LITTLE CUBE
//        //back
//        0,1,2,
//        2,0,3,

//        //front_face
//        4,5,6,
//        6,4,7,

//        //left face
//        2,6,3,
//        3,7,6,

//        //right face
//        0,4,5,
//        5,0,1,

//        //up_face
//        0,4,7,
//        7,0,3,

//        //down_face
//        6,2,1,
//        1,6,5

//    };

//    unsigned int VBOs[2], VAOs[2], EBOs[2];
//    glGenVertexArrays(2, VAOs);
//    glGenBuffers(2, VBOs);
//    glGenBuffers(2, EBOs);

//    glBindVertexArray(VAOs[0]);
//    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_wired_cube), vertices_wired_cube, GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_wired_cube), indices_wired_cube, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);

//    glBindVertexArray(VAOs[1]);
//    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_little_cube), vertices_little_cube, GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_little_cube), indices_little_cube, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);

////    Model sphere(sphereModel);
////    glm::mat4* modelMatrices;
////    modelMatrices = new glm::mat4[spheresAmount];
////    instanceSpheres(modelMatrices,sphere,spheresAmount);

    Model planet(sphereModel);


    glm::mat4 planet_origin;
    planet_origin = glm::translate(planet_origin,glm::vec3(0.0f,0.0f,0.0f));
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(0);

//        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 planet_model;

        planet_model = glm::rotate(planet_model, rotation_y_axis, glm::vec3(0.0f, 1.0f, 0.0f));
        planet_model = glm::rotate(planet_model, rotation_x_axis, glm::vec3(1.0f, 0.0f, 0.0f));
        planet_model = glm::translate(planet_model,glm::vec3(0.0f,0.0f,-10.0f));
        planet_model = glm::scale(planet_model, glm::vec3(0.5f, 0.5f, 0.5f));
        view = camera.GetViewMatrix();
        projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

//        shaderWiredCube.use();
//        shaderWiredCube.setMat4("model",model);
//        shaderWiredCube.setMat4("view",view);
//        shaderWiredCube.setMat4("projection", projection);

//        glBindVertexArray(VAOs[0]);
//        glDrawElements(GL_LINES, 32, GL_UNSIGNED_INT, 0);


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        shaderWiredCube.use();
        shaderWiredCube.setMat4("model",planet_model);
        shaderWiredCube.setMat4("view", view);
        shaderWiredCube.setMat4("projection",projection);

         planet.Draw(shaderWiredCube);

//        shaderLittleCube.use();
//        shaderLittleCube.setMat4("model",model);
//        shaderLittleCube.setMat4("view",view);
//        shaderLittleCube.setMat4("projection", projection);

//        glBindVertexArray(VAOs[1]);
//        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


//    shaderSpheres.use();
//    shaderSpheres.setMat4("view", view);
//    shaderSpheres.setMat4("projection", projection);
//    for(int i = 0; i < sphere.meshes.size(); i++) {
//        glBindVertexArray(sphere.meshes[i].VAO);
//        glDrawElementsInstanced(GL_TRIANGLES, sphere.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, spheresAmount);
//        glBindVertexArray(0);
//    }





      glfwSwapBuffers(window);
      glfwPollEvents();
    }



//    glDeleteVertexArrays(2, VAOs);
//    glDeleteBuffers(2, VBOs);
//    glDeleteBuffers(2, EBOs);

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
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotation_x_axis += -deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotation_x_axis += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotation_y_axis += -deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotation_y_axis += deltaTime;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
void instanceSpheres(glm::mat4 *modelMatrices,Model sphere, int amount)
{
    float radius = 0.01;
    float offset = 0.15f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model;

        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 1.0f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(0.0005f, 0.0005f, 0.0005f));
//        model = glm::rotate(model, rotation_y_axis, glm::vec3(0.0f, 1.0f, 0.0f));
//        model = glm::rotate(model, rotation_x_axis, glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrices[i] = model;
    }

    // vertex Buffer Object
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0],
    GL_STATIC_DRAW);

    for (unsigned int i = 0; i < sphere.meshes.size(); i++)
        {
            unsigned int VAO = sphere.meshes[i].VAO;
            glBindVertexArray(VAO);
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }
}
void rotateSpheres(glm::mat4* modelMatrices,int amount)
{

    for (unsigned int i = 0; i < spheresAmount; i++)
    {
        glm::mat4 model;

        modelMatrices[i] = model;
    }
}
