#version 330 core

layout (location = 0) in vec3 position;
layout (location = 3) in mat4 instanceModel;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceModel * vec4(position,1.0f);

}
