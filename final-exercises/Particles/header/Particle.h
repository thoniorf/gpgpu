 
#ifndef PARTICLE_H
#define PARTICLE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particle{

private:
    const GLfloat * interval;
    const GLint sign [2] = {-1, 1};
    const GLfloat max_interval;
    const GLfloat increasing_step = 0.012;
 	 
public:
    glm::vec3 position;
    
    Particle (const GLfloat * interval, const GLfloat max_interval) : interval(interval), max_interval(max_interval)
    {
        for(int i=0; i<3; i++)
        {
            //define random GLfloat number as follow.
            float r = (float)rand() / (float)RAND_MAX;
            //set initial position for each axis, inside the interval passed as parameter.
            position[i] = interval[0] + r *( interval[1] - interval[0]);
        }
    }

    void updatePosition ()
    {
		 for(int i=0; i<3; i++)
		 {
		 	int randNum = rand() % 2; 
		 	int multiplier = sign[randNum];
		 	
		 	GLfloat increasing_value = increasing_step * multiplier;
		 	if(abs(position[i] + increasing_value) < max_interval)
				position[i] +=increasing_value;
			else
				position[i] -=increasing_value;
		 }	
    }

    glm::vec3& getPosition ()
    {
        return position;
    }
};

#endif
