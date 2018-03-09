#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoord;
in vec3 Color;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform bool showTexture;

void main()
{
vec3 tex_diffuse;
vec3 tex_specular;

    if(Color.y != 0.0f){
            tex_diffuse = Color;
            tex_specular = Color;
    }else{
        if(showTexture) {
            tex_diffuse = vec3(texture(material.diffuse, TexCoord));
            tex_specular = vec3(texture(material.specular, TexCoord));
        } else {
            tex_diffuse = vec3(0.75,0.75,0.75);
            tex_specular = vec3(0.75,0.75,0.75);
        }

    }
		
                 // Ambient
    vec3 ambient = light.ambient * tex_diffuse;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * tex_diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * tex_specular;

    // Attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
	
}
