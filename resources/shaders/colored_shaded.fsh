#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float ambient;

uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{
    // ambient
    vec3 a = ambient * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 33);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (a + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}