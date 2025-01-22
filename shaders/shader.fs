#version 330 core
#define NR_POINT_LIGHTS 1

out vec4 FragColor;

//struct Material
//{
//    sampler2D diffuse;
//    sampler2D specular;   
//    float shininess;
//};

struct DirLight
{
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
};

struct PointLight
{
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Fog
{
    bool IsOn;
    float ExpDensity;
    float End;
    vec3 Color;
};

in vec2 TextCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos[NR_POINT_LIGHTS];
in vec3 DirLightDirection;

//uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform Fog fog;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 lightDirection);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos);
float CalcFogFactor();

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);

   vec3 res = CalcDirLight(dirLight, norm, viewDir, DirLightDirection);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        res += CalcPointLight(pointLights[i], norm, FragPos, viewDir, LightPos[i]);

    if (fog.IsOn)
    {
        float FogFactor = CalcFogFactor();
        res = vec3(mix(vec4(fog.Color, 1.0), vec4(res, 1.0), FogFactor));
    }

    FragColor = vec4(res, 1.0);
};

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 lightDirection)
{
    vec3 lightDir = normalize(-lightDirection);

    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TextCoord));
    vec3 diffuse = light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(texture_diffuse1, TextCoord));
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), 32.0f) * vec3(texture(texture_specular1, TextCoord));

    return (ambient + specular + diffuse);
};

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TextCoord));
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.diffuse * vec3(texture(texture_diffuse1, TextCoord));
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f) * vec3(texture(texture_specular1,TextCoord)) * light.specular;

    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return ((ambient + specular + diffuse) * attenuation);
};

float CalcFogFactor()
{
    float CameraToPixelDist = FragPos.z;
    float DistRatio = 4.0 * CameraToPixelDist / fog.End;
    return exp(- DistRatio * fog.ExpDensity * DistRatio * fog.ExpDensity);
}

//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 lightDirection)
//{
//    vec3 lightDir = normalize(-lightDirection);
//
//    vec3 reflectDir = reflect(-lightDir, normal);
//
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TextCoord));
//    vec3 diffuse = light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(material.diffuse, TextCoord));
//    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.specular, TextCoord));
//
//    return (ambient + specular + diffuse);
//};
//
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos)
//{
//    vec3 lightDir = normalize(lightPos - fragPos);
//    vec3 reflectDir = reflect(-lightDir, normal);
//
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TextCoord));
//    vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.diffuse * vec3(texture(material.diffuse, TextCoord));
//    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.specular,TextCoord)) * light.specular;
//
//    float distance = length(lightPos - FragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//
//    return ((ambient + specular + diffuse) * attenuation);
//};