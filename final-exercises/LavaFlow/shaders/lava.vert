#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 textCoord;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Color;
out vec3 Normal;
out vec3 FragPos;

void main()
{
 	gl_Position = projection * view * 1 * vec4(aPos, 1.0f);
 	TexCoord = textCoord; 	
        Color = color;
 	FragPos = vec3(model * vec4(aPos, 1.0f));
 	Normal = mat3(transpose(inverse(model))) * normal;
 	//  Normal = mat3(transpose(inverse(model))) * vec3(0.0f, 1.0f, 0.0f);
	


}
