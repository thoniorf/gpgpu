#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 offset;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec2 textCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position +offset,1.0f);
    FragPos = vec3(1 * vec4(position + offset, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normals;
    TexCoords = vec2(textCoords.x, 1.0 - textCoords.y);
}
