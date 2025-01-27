#version 400 core
#define NR_POINT_LIGHTS 1
#define NR_FLASHLIGHTS 1

out vec4 FragColor;

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

struct Flashlight
{
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;
};

struct Fog
{
    bool IsOn;
    float ExpDensity;
    float End;
    vec3 Color;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TextCoord;
in vec3 pointLightPos[NR_POINT_LIGHTS];
in vec3 DirLightDirection;
in vec3 FlashlightPos[NR_FLASHLIGHTS];
in vec3 FlashlightDir[NR_FLASHLIGHTS];

uniform sampler2D albedoMap;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Fog fog;
uniform Flashlight flashlights[NR_FLASHLIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 lightDirection);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos);
vec3 CalcFlashlight(Flashlight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos, vec3 direction);
float CalcFogFactor();

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);

    vec3 res = CalcDirLight(dirLight, norm, viewDir, DirLightDirection);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        res += CalcPointLight(pointLights[i], norm, FragPos, viewDir, pointLightPos[i]);

    for(int i = 0; i < NR_FLASHLIGHTS; i++)
		res += CalcFlashlight(flashlights[i], norm, FragPos, viewDir, FlashlightPos[i], FlashlightDir[i]);

    if (fog.IsOn)
    {
        float FogFactor = CalcFogFactor();
        res = vec3(mix(vec4(fog.Color, 1.0), vec4(res, 1.0), FogFactor));
    }

    FragColor = vec4(res, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 lightDirection)
{
    vec3 lightDir = normalize(-lightDirection);

    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = light.ambient * vec3(texture(albedoMap, TextCoord));
    vec3 diffuse = light.diffuse * max(dot(normal, lightDir), 0.0) * vec3(texture(albedoMap, TextCoord));

    return (ambient + diffuse);
};

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = light.ambient * vec3(texture(albedoMap, TextCoord));
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.diffuse * vec3(texture(albedoMap, TextCoord));

    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    return ((ambient + diffuse) * attenuation);
};

vec3 CalcFlashlight(Flashlight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos, vec3 direction)
{
    vec3 lightDir = normalize(lightPos - fragPos);

    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = light.ambient * vec3(texture(albedoMap, TextCoord));
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.diffuse * vec3(texture(albedoMap, TextCoord));

    float theta = dot(lightDir, normalize(-direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return ((ambient + diffuse) * attenuation * intensity);
};

float CalcFogFactor()
{
    float CameraToPixelDist = FragPos.z;
    float DistRatio = 4.0 * CameraToPixelDist / fog.End;
    return exp(- DistRatio * fog.ExpDensity * DistRatio * fog.ExpDensity);
};