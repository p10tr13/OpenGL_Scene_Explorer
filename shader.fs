#version 330 core

out vec4 FragColor;

in vec2 TextCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform sampler2D texture1;
uniform vec3 lightColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

    float ambientStrenght = 0.1;
    vec3 ambient = ambientStrenght * lightColor;

    float specularStrength = 0.5;
    vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 128) * specularStrength * lightColor;

    FragColor = texture(texture1, TextCoord) * vec4(ambient + diffuse + specular, 1.0);
};