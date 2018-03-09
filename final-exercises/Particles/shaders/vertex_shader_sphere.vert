#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(position,1.0f);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    FragPos = vec3(instanceMatrix * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(instanceMatrix))) * normal;
}
