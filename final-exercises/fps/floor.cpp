#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "include/camera.h"
#include "include/shader.h"
#include "include/model.h"
#include "include/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void processInput(GLFWwindow *window);
bool collisionDetection(glm::vec3 position);

void initGLFW() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}
void initCallback(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
}
void initGlew() {
    glewExperimental = GL_TRUE;
    glewInit();
}

// settings
const unsigned int SCR_WIDTH   = 800;
const unsigned int SCR_HEIGHT  = 600;
// FRAME vars
float deltaTime                = 0.0f;
float lastFrame                = 0.0f;
// FLOOR consts
const int floor_row_amount     = 100;
const int floor_columns_amount = 100;
const char* floor_vertex       = "shaders/floor_vertex.vert";
const char* floor_fragment     = "shaders/floor_fragment.frag";
const char* floor_texture      = "textures/floor.jpg";
// CUBE consts
const int cube_amount          = 9;
const float cube_size          = 1.0f;
const char* cube_vertex        = "shaders/cube_vertex.vert";
const char* cube_fragment      = "shaders/cube_fragment.frag";
const char* cube_texture       = "textures/cube.jpg";
glm::vec3 cube_transforms[cube_amount] = {
    glm::vec3(54.0f,  0.5f,  -35.0f),
    glm::vec3(50.0f,  0.5f, -30.0f),
    glm::vec3(55.0f, 0.5f, -55.0f),
    glm::vec3(55.0f, 0.5f, -50.0f),

    glm::vec3(50.0f, 0.5f, -50.0f),

    glm::vec3(44.0f,  0.5f,  -35.0f),
    glm::vec3(40.0f,  0.5f, -30.0f),
    glm::vec3(45.0f, 0.5f, -55.0f),
    glm::vec3(45.0f, 0.5f, -50.0f)

};
// SPHERE const
const int sphere_amount        = 6;
const float sphere_radius      = 0.3f;
const char* sphere_model       = "models/earth/earth.obj";
const char* sphere_vertex      = "shaders/sphere_vertex.vert";
const char* sphere_fragment    = "shaders/sphere_fragment.frag";
const char* sphere_texture     = "models/earth/earth.jpg";
glm::vec3 sphere_transforms[sphere_amount] ={
    glm::vec3(52.0f, 1.0f, -58.0f),
    glm::vec3(45.0f, 1.0f, -65.0f),

    glm::vec3(48.0f, 1.0f, -50.0f),

    glm::vec3(62.0f, 1.0f, -48.0f),
    glm::vec3(55.0f, 1.0f, -40.0f)
};
// LIGHT consts
const char* light_vertex        = "shaders/light_vertex.vert";
const char* light_fragment      = "shaders/light_fragment.frag";
bool light_on                   = true;
// CUBEMAPS consts
const char* cubemaps_vertex     ="shaders/cubemaps.vert";
const char* cubemaps_frag     ="shaders/cubemaps.frag";
// CAMERA settings
float lastX                    = SCR_WIDTH / 2.0f;
float lastY                    = SCR_HEIGHT / 2.0f;
bool firstMouse                = true;

Camera camera(glm::vec3(52.0f, 1.0f, -45.0f));

class Floor {
public:

    int row;
    int column;
    float x_offset;
    float z_offset;
    float square_size;
    Shader* shader;
    unsigned int texture;
    glm::vec3* transforms;
    unsigned int VAO,VBO[2],EBO;

    /* Triangles vertex definition
    -0.5,0.5--------0.5,0.5
        |              |
        |              |
        |              |
        |              |
        |              |
    -0.5,-0-5-------0.5,-0.5
    */

    float verts[48] = {
        //coords            //normals           //texCoords
        // top right
        0.5f,0.0f,0.5f,     0.0f,1.0f,0.0f,     1.0f,1.0f,// 0
        // bottom right
        0.5f,0.0f,-0.5f,    0.0f,1.0f,0.0f,     1.0f,0.0f,// 1
        // bottom left
        -0.5f,0.0f,-0.5f,    0.0f,1.0f,0.0f,    0.0f,0.0f,// 2
        // top left
        -0.5f,0.0f,0.5f,     0.0f,1.0f,0.0f,    0.0f,1.0f // 3
    };
    unsigned int index[6]= {
        0,1,3, //first
        1,2,3  //second
    };

    Floor(int row, int cols,float square_size, const char* vertex, const char* fragment) {
        this->row = row;
        this->column = cols;
        this->x_offset = 0.0f;
        this->z_offset = 0.0f;
        this->square_size = square_size;
        this->shader = new Shader(vertex,fragment);
        this->transforms = new glm::vec3[row*cols];
        glGenBuffers(2,VBO);
        glGenBuffers(1,&EBO);
        glGenVertexArrays(1,&VAO);
    }

    ~Floor() {
        glDeleteVertexArrays(1,&VAO);
        glDeleteVertexArrays(1,&EBO);
        glDeleteBuffers(2,VBO);
    }

    void generateTransformation() {
        int index = 0;
        for(int i = 0; i < floor_row_amount; i++){
            for(int j = 0; j < floor_columns_amount; j++) {
                glm::vec3 transform = glm::vec3(x_offset,0.0f,z_offset);
                transforms[index] = transform;
                z_offset -= square_size;
                index++;
            }
            x_offset += square_size;
            z_offset = 0.0f;
        }
    }
    void bind() {
        //floor offset binding
        glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*row*column,&transforms[0],GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        //floor vertex binding
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);
        // vertex
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)0);
        // normals
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(3 * sizeof(float)));
        // texCoords
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(6 * sizeof(float)));

        // offsets
        glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glVertexAttribDivisor(1, 1);
    }
    void loadTexture() {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        unsigned char *data = stbi_load(floor_texture, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
    void bindTexture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);

        shader->use();
        shader->setInt("material.diffuse",0);
        shader->setInt("material.specular",1);
        shader->setFloat("material.shininess",36.0f);
    }
    void unbindTexture() {
       glBindTexture(GL_TEXTURE_2D,0);

    }
};
class Cube{
public:
    float x_offset, z_offset, square_size;
    unsigned int texture;
    unsigned int VAO,VBO[2];
    Shader* shader;
    float verts[288] = {
        // coords             //Norm               // textcoords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    Cube(float square_size, const char* vertex, const char* fragment) {
        this->x_offset = 0.0f;
        this->z_offset = 0.0f;
        this->shader = new Shader(vertex,fragment);
        glGenBuffers(2,VBO);
        glGenVertexArrays(1,&VAO);
    }
    ~Cube() {
        glDeleteVertexArrays(1,&VAO);
        glDeleteBuffers(2,VBO);
    }

    void bind() {
        //floor offset binding
        glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*cube_amount,&cube_transforms[0],GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        //floor vertex binding
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
        // vertex
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)0);
        // normals
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(3 * sizeof(float)));
        // texCoords
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(6 * sizeof(float)));
        // offsets
        glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glVertexAttribDivisor(1, 1);
    }
    void loadTexture() {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        unsigned char *data = stbi_load(cube_texture, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
    void bindTexture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);

        shader->use();
        shader->setInt("material.diffuse",0);
        shader->setInt("material.specular",1);
        shader->setFloat("material.shininess",64.0f);
    }
    void unbindTexture() {
       glBindTexture(GL_TEXTURE_2D,0);

    }
};

class Light {
public:
    const glm::vec3 lightColor_on = glm::vec3(0.85,0.85,0.85);
    const glm::vec3 lightColor_off = glm::vec3(0.2,0.2,0.2);

    glm::vec3 pos;
    Shader* shader;
    unsigned int VAO,VBO; 
    float verts[108] = {
        // coords
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };
    Light(glm::vec3 pos,const char* vertex,const char* fragment) {
        this->pos = pos;
        this->shader = new Shader(vertex,fragment);
        glGenBuffers(1,&VBO);
        glGenVertexArrays(1,&VAO);
    }
    ~Light() {
       glDeleteVertexArrays(1,&VAO);
       glDeleteBuffers(1,&VBO);
    }
    void bind() {
        //floor vertex binding
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
        // vertex
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
    }
};

class Sphere {
  public:
    Model* model;
    Shader* shader;
    unsigned int texture;
    Sphere(const char* model,const char* vertex, const char* fragment) {
        this->model = new Model(model);
        this->shader = new Shader(vertex,fragment);
    }
    ~Sphere() {
        delete model;
        delete shader;
    }

    void Draw() {
        model->Draw(*shader);
    }
    void loadTexture() {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        unsigned char *data = stbi_load(cube_texture, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
    void bindTexture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        glBindTexture(GL_TEXTURE_2D, model->textures_loaded[0].id);

        shader->use();
        shader->setInt("texture_diffuse1",0);
        shader->setInt("material.diffuse",0);
        shader->setInt("material.specular",1);
        shader->setFloat("material.shininess",64.0f);


    }
    void unbindTexture() {
       glBindTexture(GL_TEXTURE_2D,0);

    }

};

class CubeMaps {
  public:

    const char* texture_faces[6]  = {
        "textures/skybox/right.tga",
        "textures/skybox/left.tga",
        "textures/skybox/top.tga",
        "textures/skybox/bottom.tga",
        "textures/skybox/front.tga",
        "textures/skybox/back.tga"
    };

    float vertices[108] = {
        -110.0f,  110.0f, -110.0f,
        -110.0f, -110.0f, -110.0f,
         110.0f, -110.0f, -110.0f,
         110.0f, -110.0f, -110.0f,
         110.0f,  110.0f, -110.0f,
        -110.0f,  110.0f, -110.0f,

        -110.0f, -110.0f,  110.0f,
        -110.0f, -110.0f, -110.0f,
        -110.0f,  110.0f, -110.0f,
        -110.0f,  110.0f, -110.0f,
        -110.0f,  110.0f,  110.0f,
        -110.0f, -110.0f,  110.0f,

         110.0f, -110.0f, -110.0f,
         110.0f, -110.0f,  110.0f,
         110.0f,  110.0f,  110.0f,
         110.0f,  110.0f,  110.0f,
         110.0f,  110.0f, -110.0f,
         110.0f, -110.0f, -110.0f,

        -110.0f, -110.0f,  110.0f,
        -110.0f,  110.0f,  110.0f,
         110.0f,  110.0f,  110.0f,
         110.0f,  110.0f,  110.0f,
         110.0f, -110.0f,  110.0f,
        -110.0f, -110.0f,  110.0f,

        -110.0f,  110.0f, -110.0f,
         110.0f,  110.0f, -110.0f,
         110.0f,  110.0f,  110.0f,
         110.0f,  110.0f,  110.0f,
        -110.0f,  110.0f,  110.0f,
        -110.0f,  110.0f, -110.0f,

        -110.0f, -110.0f, -110.0f,
        -110.0f, -110.0f,  110.0f,
         110.0f, -110.0f, -110.0f,
         110.0f, -110.0f, -110.0f,
        -110.0f, -110.0f,  110.0f,
         110.0f, -110.0f,  110.0f
    };

    unsigned int texture;
    unsigned int VAO,VBO;

    Shader* shader;

    CubeMaps(const char* vertex, const char* fragment) {
        shader = new Shader(vertex,fragment);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    ~CubeMaps()
    {
        glDeleteVertexArrays(1,&VAO);
        glDeleteBuffers(1,&VBO);
        delete shader;
    }

    void bind() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);

    }

    void loadTexture() {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < 6; i++)
        {
            unsigned char *data = stbi_load(texture_faces[i], &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << texture_faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    }

    void bindTexture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    }
    void unbindTexture() {
        glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    }
};
void setupLight(Shader* shader, Light &light);

int main()
{
    initGLFW();
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FLOOR", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    initCallback(window);
    initGlew();

    glEnable(GL_DEPTH_TEST);

    Floor floor(floor_row_amount,floor_columns_amount,1.0f,floor_vertex,floor_fragment);
    floor.generateTransformation();
    floor.bind();
    floor.loadTexture();
    floor.bindTexture();

    Cube cubes(0.5,cube_vertex,cube_fragment);
    cubes.bind();
    cubes.loadTexture();
    cubes.bindTexture();

    Light light(glm::vec3(50.0f,2.0f,-50.0f),light_vertex,light_fragment);
    light.bind();

    Sphere sphere(sphere_model,sphere_vertex,sphere_fragment);
    sphere.loadTexture();

    CubeMaps cubeMaps(cubemaps_vertex,cubemaps_frag);
    cubeMaps.bind();
    cubeMaps.loadTexture();
    cubeMaps.bindTexture();
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
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        model = glm::translate(model,glm::vec3(0.0f,0.0f,0.0f));
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 light_model;
        light_model = glm::translate(model,light.pos);
        light_model = glm::scale(light_model,glm::vec3(0.2f,0.2f,0.2f));
        light.shader->use();
        light.shader->setMat4("model",light_model);
        light.shader->setMat4("view",view);
        light.shader->setMat4("projection",projection);
        if(light_on) {
            light.shader ->setVec3("color",light.lightColor_on);
        } else {
            light.shader ->setVec3("color",light.lightColor_off);
        }
        glBindVertexArray(light.VAO);
        glDrawArrays(GL_TRIANGLES,0,36);
        glBindVertexArray(0);

        floor.bindTexture();
        floor.shader->use();
        setupLight(floor.shader,light);
        floor.shader->setMat4("model",model);
        floor.shader->setMat4("view",view);
        floor.shader->setMat4("projection",projection);
        glBindVertexArray(floor.VAO);
        glDrawElementsInstanced(GL_TRIANGLES,6,GL_UNSIGNED_INT,0,floor.row*floor.column);
        glBindVertexArray(0);
        floor.unbindTexture();

        cubes.bindTexture();
        cubes.shader->use();
        setupLight(cubes.shader,light);
        cubes.shader->setMat4("model",model);
        cubes.shader->setMat4("view",view);
        cubes.shader->setMat4("projection",projection);
        glBindVertexArray(cubes.VAO);
        glDrawArraysInstanced(GL_TRIANGLES,0,36,cube_amount);
        glBindVertexArray(0);
        cubes.unbindTexture();

        sphere.shader->use();
        setupLight(sphere.shader,light);
        sphere.shader->setMat4("view",view);
        sphere.shader->setMat4("projection",projection);
        for(int i = 0; i < sphere_amount; i++) {
           model = glm::mat4();
           model = glm::translate(model,sphere_transforms[i]);
           sphere.shader->setMat4("model",model);
           sphere.bindTexture();
           sphere.Draw();
        }

       glDepthFunc(GL_EQUAL);
       cubeMaps.shader->use();
       view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix)
       cubeMaps.shader->setMat4("view",view);
       cubeMaps.shader->setMat4("projection",projection);
       cubeMaps.bindTexture();
       glBindVertexArray(cubeMaps.VAO);
       glDrawArrays(GL_TRIANGLES,0,36);
       glBindVertexArray(0);
       glDepthFunc(GL_LESS);
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
void setupLight(Shader *shader, Light& light)
{
    shader->setVec3("light.position", light.pos);
    shader->setVec3("viewPos", camera.Position);
    if(light_on) {
        shader->setVec3("light.diffuse", light.lightColor_on);
    } else {
        shader->setVec3("light.diffuse", light.lightColor_off);
    }
    shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("material.shininess", 64.0f);
    shader->setFloat("light.constant",1.0f);
    shader->setFloat("light.linear", 0.027);
    shader->setFloat("light.quadratic", 0.028);
}
// process all input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float velocity = camera.MovementSpeed * deltaTime;
    glm::vec3 position;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        position =camera.Position + camera.Front * velocity;
        if(!collisionDetection(position))
            camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position = camera.Position - camera.Front * velocity;
        if(!collisionDetection(position))
            camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position = camera.Position - camera.Front * velocity;
        if(!collisionDetection(position))
            camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position = camera.Position + camera.Front * velocity;
        if(!collisionDetection(position))
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_L && action == GLFW_RELEASE)
            light_on = !light_on;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset,0.0f);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
bool collisionDetection(glm::vec3 position)
{
    for(int i = 0; i < cube_amount; i++)
    {
        float distance = sqrt(pow(position.x - cube_transforms[i].x, 2) + pow(position.y - cube_transforms[i].y, 2) + pow(position.z - cube_transforms[i].z, 2));
        if( distance <= cube_size)
            return true;
    }

    for(int i = 0; i < sphere_amount; i++)
    {
        float distance = sqrt(pow(position.x - sphere_transforms[i].x, 2) + pow(position.y - sphere_transforms[i].y, 2) + pow(position.z - sphere_transforms[i].z, 2));

        if( distance <= sphere_radius)
            return true;
    }

    if(position.x <=0 || position.x >= floor_row_amount || abs(position.z) <= 0 || abs(position.z) >= floor_columns_amount)
        return true;

    return false;

}
