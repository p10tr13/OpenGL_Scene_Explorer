#version 330 core

out vec4 FragColor;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TextCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform sampler2D texture1;
uniform vec3 lightColor;

uniform Light light;
uniform Material material;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

    vec3 ambient = light.ambient * material.ambient;

    vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * material.specular * light.specular * lightColor;

    FragColor = texture(texture1, TextCoord) * vec4(ambient + diffuse + specular, 1.0);
};