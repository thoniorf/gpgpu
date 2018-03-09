#ifndef LAMP_H
#define LAMP_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "model.h"

class Lamp{
public:
    const float LAMP_ZOOM_STEP = 0.05f;
    const float ZOOM = 1.5f;
    const float ZOOM_MAX = 3.5f;
    const float ZOOM_MIN = 1.0f;

    Lamp(const char* model_path)
    {
        zoom = ZOOM;
        model = new Model(model_path);
        position = glm::vec3(0.0f,0.0f,zoom);
    }
    ~Lamp()
    {
        delete model;
    }
    void Draw(Shader shader)
    {
        model->Draw(shader);
    }
    glm::vec3 getPosition()
    {
        return position;
    }

    void updatePosition(float delta, glm::vec3 rot_axis)
    {
        glm::mat4 transformations = glm::mat4();

        transformations = glm::rotate(transformations, delta, rot_axis);
        glm::vec4 postion4 = (glm::vec4(position,1.0f)) * transformations;
        position = glm::vec3(postion4.x,postion4.y,postion4.z);
    }
    void setPosition(const glm::vec3 &value)
    {
        position = value;
    }

    glm::vec3 getReversePosition()
    {
        return glm::vec3(-position.x,-position.y,-position.z);
    }
    float getX_rotation()
    {
        return x_rotation;
    }

    void setX_rotation(float value)
    {
        x_rotation += value;
    }

    float getY_rotation()
    {
        return y_rotation;
    }

    void setY_rotation(float value)
    {
        y_rotation += value;
    }

    float getZoom()
    {
    return zoom;
    }

    void setZoom(float value)
    {
    zoom += value;
    position.z = zoom;
    }

private:
    glm::vec3 position;
    Model* model;
    float x_rotation, y_rotation;
    float zoom;
};

#endif // LAMP_H






