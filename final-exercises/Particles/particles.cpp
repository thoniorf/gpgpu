#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "header/camera.h"
#include "header/Shader.h"
#include "header/model.h"
#include "header/Particle.h"
#include "header/lamp.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void update_particles(std::vector <Particle>& particles, glm::mat4* models_matrices_for_particles, GLuint & buffer, Model & sphere_model);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* wired_cube_vertex = "shaders/vertex_shader.vert";
const char* wired_cube_frag = "shaders/fragment_shader_wired.frag";
const char* little_cube_vertex = "shaders/vertex_shader.vert";
const char* little_cube_frag = "shaders/fragment_shader_little.frag";
const char* sphere_vertex = "shaders/vertex_shader_sphere.vert";
const char* sphere_frag = "shaders/fragment_shader_spheres.frag";
const char* lamp_vert = "shaders/vertex_shader_lamp.vert";
const char* lamp_frag = "shaders/fragment_shader_lamp.frag";

const unsigned int NUMBER_OF_PARTICLES = 2000;

const float INTERVAL_LITTLE_CUBE [2] = {-0.33, 0.33}; // little less than little cube size
const float INTERVAL_MAX_WIRED_CUBE = 0.99f; // little less than big cube size

Lamp* lamp;

std::vector <Particle> particles;
glm::mat4* models_matrices_for_particles;

Camera camera(glm::vec3(0.0f, 0.0f, 5.5f));

bool movingParticles = false;

float cubes_rotation_x_axis = 0.0f;
float cubes_rotation_y_axis = 0.0f;

float lamp_rotation_x_axis = 0.0f;
float lamp_rotation_y_axis = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct WiredCube{
    float vertex[24] = {
        1.0f, 	1.0f, 	1.0f,
        1.0f,  -1.0f, 	1.0f,
        -1.0f,  -1.0f,	1.0f,
        -1.0f, 	1.0f, 	1.0f,
        1.0f, 	1.0f,  -1.0f,
        1.0f,  -1.0f,  -1.0f,
        -1.0f,  -1.0f,  -1.0f,
        -1.0f, 	1.0f,  -1.0f,
    };
    unsigned int indexes[32] = {
        //back
        0,1,
        1,2,
        2,3,
        3,0,
        //front
        4,5,
        5,6,
        6,7,
        7,4,
        //left
        3,7,
        6,7,
        2,3,
        6,2,
        //right
        0,4,
        1,5,
        0,1,
        4,5
    };

}WiredCube;

struct LittleCube{
    float vertex[24] = {
        0.35f, 	0.35f, 0.35f,
        0.35f,  -0.35f, 0.35f,
        -0.35f,  -0.35f, 0.35f,
        -0.35f, 	0.35f, 0.35f,
        0.35f, 	0.35f, -0.35f,
        0.35f,  -0.35f, -0.35f,
        -0.35f,  -0.35f, -0.35f,
        -0.35f, 	0.35f, -0.35f
    };
    unsigned int indexes[36] = {
        //back
        0,1,2,
        2,0,3,
        //front
        4,5,6,
        6,4,7,
        //left
        2,6,3,
        3,7,6,
        //right
        0,4,5,
        5,0,1,
        //up
        0,4,7,
        7,0,3,
        //down
        6,2,1,
        1,6,5
    };

}LittleCube;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particles", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    Shader wired_cube_shader(wired_cube_vertex, wired_cube_frag);
    Shader little_cube_shader(little_cube_vertex, little_cube_frag);
    Shader spheres_shader(sphere_vertex,sphere_frag);
    Shader lamp_shader(lamp_vert,lamp_frag);

    Model sphere_model("models/sphere.obj");
    Model lamp_model("models/lamp/lamp.obj");

    lamp = new Lamp("models/lamp/lamp.obj");


	models_matrices_for_particles = new glm::mat4[NUMBER_OF_PARTICLES];
    unsigned int buffer;
	glGenBuffers(1, &buffer);

    for (unsigned int i = 0; i< NUMBER_OF_PARTICLES; i++)
    {
       Particle p (INTERVAL_LITTLE_CUBE, INTERVAL_MAX_WIRED_CUBE);
       particles.push_back(p);
    }

    unsigned int VBOs[2], VAOs[2], EBOs[2];
	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);
	glGenBuffers(2, EBOs);

	//WIRED CUBE
	glBindVertexArray(VAOs[0]);			
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(WiredCube.vertex), WiredCube.vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(WiredCube.indexes), WiredCube.indexes, GL_STATIC_DRAW);
	
    glEnableVertexAttribArray(0);

	//LITTLE CUBE
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LittleCube.vertex), LittleCube.vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);	
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(LittleCube.indexes), LittleCube.indexes, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	
	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        update_particles (particles, models_matrices_for_particles, buffer, sphere_model);

        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(0);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 model;
        model = glm::rotate(model, cubes_rotation_y_axis, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, cubes_rotation_x_axis, glm::vec3(1.0f, 0.0f, 0.0f));

        //wired cube
        wired_cube_shader.use();
        wired_cube_shader.setMat4("view",view);
        wired_cube_shader.setMat4("model",model);
        wired_cube_shader.setMat4("projection", projection);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_LINES, 32, GL_UNSIGNED_INT, 0);

         //little cube
        little_cube_shader.use();
        little_cube_shader.setMat4("view",view);
        little_cube_shader.setMat4("model",model);
        little_cube_shader.setMat4("projection", projection);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
        glDepthMask(1);

        //LAMP RENDERING
        lamp_shader.use();
        lamp_shader.setMat4("projection", projection);
        lamp_shader.setMat4("view", view);

        glm::mat4 lamp_matrix = glm::mat4();
        lamp_matrix = glm::translate(lamp_matrix, lamp->getPosition());
        lamp_matrix = glm::rotate(lamp_matrix, lamp->getY_rotation(), glm::vec3(0.0f, 1.0f, 0.0f));
        lamp_matrix = glm::rotate(lamp_matrix, lamp->getX_rotation(), glm::vec3(1.0f, 0.0f, 0.0f));
        lamp_matrix = glm::scale(lamp_matrix, glm::vec3(0.09f, 0.09f, 0.09f));
        lamp_shader.setMat4("model", lamp_matrix);

        lamp_model.Draw(lamp_shader);

        spheres_shader.use();

        spheres_shader.setMat4("projection", projection);
        spheres_shader.setMat4("view", view);

        spheres_shader.setFloat("material.shininess", 32.0f);
        spheres_shader.setInt("material.diffuse", 0); //material.diffuse (se lo portiamo ad uno renderizzerà rispetto all'utlima texture renderizzata nel loop)
        spheres_shader.setInt("material.specular", 1);

        spheres_shader.setVec3("light.position", lamp->getPosition());
        spheres_shader.setVec3("light.direction", lamp->getReversePosition()); //direzione della luce; avrà le stesse coordinate della posizione dell'oggetto che modella la lampada, ma saranno negate. Questo perchè se puntassimo la direzione nello stesso modo della posizione, in relazione al sistema di riferimento, si otterrebbe una luce puntante verso l'asse z positivo anzichè quello negativo
        spheres_shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f))); //raggio di taglio del cono, più lo si aumenta più il cono di luce sarà grande e viceversa
        spheres_shader.setVec3("viewPos", camera.Position);
        spheres_shader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
        spheres_shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        spheres_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        spheres_shader.setFloat("light.constant", 1.0f);
        spheres_shader.setFloat("light.linear", 0.09f);
        spheres_shader.setFloat("light.quadratic", 0.032f);

        glBindTexture(GL_TEXTURE_2D, sphere_model.textures_loaded[0].id);
        for (unsigned int i = 0; i < sphere_model.meshes.size(); i++)
        {
           glBindVertexArray(sphere_model.meshes[i].VAO);
           glDrawElementsInstanced(GL_TRIANGLES, sphere_model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, NUMBER_OF_PARTICLES);
           glBindVertexArray(0);
        }


        glfwPollEvents();

        glfwSwapBuffers(window);
        }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(2, EBOs);
    delete lamp;
    glfwTerminate();
    return 0;
}

void update_particles(std::vector <Particle>& particles, glm::mat4* models_matrices_for_particles, GLuint & buffer, Model & sphere_model)
{

	for (unsigned int i = 0; i < NUMBER_OF_PARTICLES; i++)
	{
		glm::mat4 particle_model;
	
        if(movingParticles)
			particles[i].updatePosition();

		particle_model = glm::rotate(particle_model, cubes_rotation_y_axis, glm::vec3(0.0f, 1.0f, 0.0f));
		particle_model = glm::rotate(particle_model, cubes_rotation_x_axis, glm::vec3(1.0f, 0.0f, 0.0f));		
		particle_model = glm::translate(particle_model, particles[i].getPosition());
		particle_model = glm::scale(particle_model, glm::vec3(0.018f, 0.018f, 0.018f));
		
		models_matrices_for_particles[i] = particle_model;
	}
	//configure Instanced Array
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, NUMBER_OF_PARTICLES * sizeof(glm::mat4), &models_matrices_for_particles[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < sphere_model.meshes.size(); i++)
	{
		GLuint VAO = sphere_model.meshes[i].VAO;
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

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//CUBES KEYBOARD HANDLING
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cubes_rotation_x_axis -= deltaTime;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cubes_rotation_x_axis += deltaTime;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cubes_rotation_y_axis += deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cubes_rotation_y_axis -= deltaTime;

	//LAMP KEYBOARD HANDLING
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
            lamp->setX_rotation(-deltaTime);
            lamp->updatePosition(deltaTime, glm::vec3(1.0, 0.0, 0.0));
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
            lamp->setX_rotation(deltaTime);
            lamp->updatePosition(-deltaTime, glm::vec3(1.0, 0.0, 0.0));
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
            lamp->setY_rotation(deltaTime);
            lamp->updatePosition(-deltaTime, glm::vec3(0.0, 1.0, 0.0));
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
            lamp->setY_rotation(-deltaTime);
            lamp->updatePosition(deltaTime, glm::vec3(0.0, 1.0, 0.0));
	}	
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
        if(glm::abs(lamp->getZoom()) >= lamp->ZOOM_MIN) {
            lamp->setZoom(-lamp->LAMP_ZOOM_STEP);
        }

	}
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS){
        if(glm::abs(lamp->getZoom()) <= lamp->ZOOM_MAX) {
            lamp->setZoom(lamp->LAMP_ZOOM_STEP);
        }

	}	
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        movingParticles = !movingParticles;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
