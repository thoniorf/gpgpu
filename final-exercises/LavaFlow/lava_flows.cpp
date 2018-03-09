#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "header/stb_image.h"
#include "header/shader.h"
#include "header/camera.h"
#include "header/surface.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void load_texture (unsigned int &texture, string &path);
void readFile(const char* path);
void readThickness();
void readTemperatures();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//texture
unsigned int texture;
string path_texture = "dataset/texture.png";

//datasets paths
const char* data_path ="dataset/altitudes.dat";
const char* thick_path ="dataset/lava.dat";
const char* temp_path ="dataset/temperature.dat";

unsigned int data_cols;
unsigned int data_rows;
unsigned int cell_size;

float temperature_min = 0.0f;
float temperature_max = 0.0f;
float thickness_min = 0.0f;
float thickness_max = 0.0f;
float altitude_min = 0.0f;
float altitude_max = 0.0f;

Camera camera(glm::vec3(1800.0f, 3000.0f, -3500.0f));
//shaders path
const char* vertex_shader = "shaders/lava.vert";
const char* fragment_shader = "shaders/lava.frag";
//surface
Surface surface;
// texture display
bool showTexture = true;

int main()
{

	camera.Up = glm::vec3(0.0f, 3.0f, 0.0f);

	 // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LavaFlow", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
 

    // GLEW: initialize
    glewExperimental = GL_TRUE;
    glewInit();

    // configure global opengl state
    // ------------------------
    glEnable(GL_DEPTH_TEST);

    Shader shader(vertex_shader, fragment_shader);


    //DATA READING
    readFile(data_path);
    readThickness();
    readTemperatures();


    surface.createGreenGrid();
    surface.generateGreenCoord();
    surface.setAltitudeFromDataToGreen();
    surface.setTemperatureFromDataToGreen();
    surface.setVertexFromGreen();
    surface.setIndicesFromGreen();
    surface.setTextureCoordsFromGreen();
    surface.setColorsFromGreen();
    surface.calculateDirtyNormals();
    surface.calculateNormals();
    surface.DeleteGridsAndDirtyNormals();

    // BUFFERS
    GLuint VBO, VAO, EBO, TEXT, COL, NORM;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &TEXT);
    glGenBuffers(1, &COL);
    glGenBuffers(1, &NORM);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*surface.getGreenGridElementsNumber()*3, &surface.vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, TEXT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*surface.getGreenGridElementsNumber()*2, &surface.textCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, COL);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*surface.getGreenGridElementsNumber()*3, &surface.colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, NORM);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*surface.getGreenGridElementsNumber()*3, &surface.normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*surface.getGreenGridElementsNumber()*6, &surface.indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    load_texture(texture,path_texture);
  
    //GAME LOOP
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        
        //INPUT
        processInput(window);


						  
        glm::mat4 view;
        view = camera.GetViewMatrix();

        glm::mat4 projection;
        projection = glm::perspective(45.0f, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 15000.0f);

        glm::mat4 model;
        model = glm::mat4();

        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        
        shader.setVec3("viewPos", camera.Front);
        shader.setFloat("material.shininess", 64.0f);
        shader.setInt("material.diffuse", 0); //material.diffuse (se lo portiamo ad uno renderizzerà rispetto all'utlima texture renderizzata nel loop)
        shader.setInt("material.specular", 1);

        // point light settings 
        shader.setVec3("light.position", camera.Position);
        shader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
        shader.setVec3("light.diffuse",  1.0f, 1.0f, 1.0f);
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("light.constant",0.6f);
        shader.setFloat("light.linear",0.000014);
        shader.setFloat("light.quadratic", 0.00000007);
     
        shader.setBool("showTexture",showTexture);
        glBindVertexArray(VAO);
		
        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawElements(GL_TRIANGLES, surface.getGreenGridElementsNumber()*6, GL_UNSIGNED_INT, 0);
		  
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
 
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_T && action == GLFW_RELEASE)
        showTexture = !showTexture;
}
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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}

void load_texture (unsigned int &texture, string &path){
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //PIXXELATO
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //INTERPOLATO

    int width, height, nrChannels;

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
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

void readFile(const char* path)
{
	string line;
    ifstream myfile (path);
	int line_number = 0;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			line_number++;
			istringstream iss(line);
			string value;
			if (line_number == 1 || line_number == 2 || line_number == 5)
			{
				int value_number = 0;
				while (std::getline(iss, value, '\t'))
				{
					if (value != "")
					{
                        if (value_number == 1)
                        {
                            if (line_number == 1)
                            {
								data_cols = atoi(value.c_str()); 
							}
                            else if (line_number == 2)
                            {
                                data_rows = atoi(value.c_str());
                                surface.createDataGrid(data_rows,data_cols);
							}
							else if (line_number == 5)
							{
								std::string::size_type sz;     // alias of size_t
                                cell_size = stod(value,&sz);

                               surface.setCellSize(cell_size);

							}
						}
						value_number++;
					}
				}
			}

			else if(line_number > 6)
			{
				int column = 0;
				while (std::getline(iss, value, ' '))
				{
					if (value != "" && column < data_cols)
					{
						std::string::size_type sz;     // alias of size_t
                        double altitudine = stod(value,&sz);
						if(line_number-7 == 0 && column == 0){
							altitude_min = altitudine;
							altitude_max = altitudine;
						}
						if(altitudine > altitude_max)
							altitude_max = altitudine;
						if(altitudine < altitude_min)
							altitude_min = altitudine;

                        surface.setDataCoordAltitiudeAtPosition((float) ((line_number-7)*cell_size),(float) (column*cell_size),(float) (altitudine),line_number-7,column);

                        column++;
					}
				}
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}
void readThickness() {
    string line;
    ifstream myfile (thick_path);
    int nLine = 0;
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            nLine++;
            istringstream iss(line);
            string value;

            if(nLine > 6)
            {
                int column = 0;

                while (std::getline(iss, value, ' '))
                {
                    if (value != "" && column < data_cols)
                    {
                        std::string::size_type sz;     // alias of size_t
                        double thickness = stod(value,&sz);

                        surface.addDataThicknessAtIndex((float)(thickness),nLine-7,column);

                        column++;

                    if(nLine == 0 && column == 0)
                    {
                        thickness_max = thickness;
                        thickness_min = thickness;
                    }
                    if(thickness > thickness_max)
                        thickness_max = thickness;
                    if(thickness < thickness_min)
                        thickness_min = thickness;
                    }
                }
            }
        }
        myfile.close();
    }
    else cout << "Unable to open file";

    // LAVA THICKNESS NORMALIZATION BETWEEN 0 and 1
    int index = 0;
    for (int i = 0; i < data_rows; i++){
        for(int j = 0; j < data_cols; j++)
        {
            float temperature = surface.getDataTemperatureAtIndex(i,j);

            if(temperature != 0.0f)
            {
                surface.setDataTemperatureAtIndex((temperature - temperature_min ) / (temperature_max - temperature_min),i,j);
                index++;
            }
        }
    }
}
void readTemperatures() {
	string line;
    ifstream myfile (temp_path);
	int nLine = 0;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			nLine++;
			istringstream iss(line);
			string value;
	
			if(nLine > 6)
			{
				int column = 0;
		
				while (std::getline(iss, value, ' '))
				{
					if (value != "" && column < data_cols)
					{
						std::string::size_type sz;     // alias of size_t
						double temperatura = stod(value,&sz);

                        surface.setDataTemperatureAtIndex((float)(temperatura),nLine-7,column);

                        column++;
						
                    if(nLine == 0 && column == 0)
					{
						temperature_max = temperatura;
						temperature_min = temperatura; 	
					}
					if(temperatura > temperature_max)
						temperature_max = temperatura;
					if(temperatura < temperature_min)
						temperature_min = temperatura;
					}
				}
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";
	
    // TEMPERATURES NORMALIZATION BETWEEN 0 and 1
    int index = 0;
	for (int i = 0; i < data_rows; i++){
		for(int j = 0; j < data_cols; j++)
		{
            float temperature = surface.getDataTemperatureAtIndex(i,j);

            if(temperature != 0.0f)
            {
                surface.setDataTemperatureAtIndex((temperature - temperature_min ) / (temperature_max - temperature_min),i,j);
                index++;
			}
		}
	}
}
