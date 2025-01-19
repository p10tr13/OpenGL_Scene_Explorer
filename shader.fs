#version 330 core

out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;   
    float shininess;
}; 

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 TextCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform vec3 lightColor;

uniform Light light;
uniform Material material;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * light.diffuse * vec3(texture(material.diffuse, TextCoord));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TextCoord));

    vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.specular,TextCoord)) * light.specular;

    float distance = length(LightPos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.0);
};