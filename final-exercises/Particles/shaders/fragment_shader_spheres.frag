#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal; 
in vec2 TexCoord;
 
  
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);
    
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction)); 
    
    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
        
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
       vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
        
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
           vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
        
        // attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;   
            
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);  
    }
    else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        FragColor = vec4(light.ambient, 1.0);
    }
} 


