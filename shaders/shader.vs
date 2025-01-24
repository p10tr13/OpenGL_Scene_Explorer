#version 400 core
#define NR_POINT_LIGHTS 1
#define NR_FLASHLIGHTS 1

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoord;

out vec2 TextCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos[NR_POINT_LIGHTS];
out vec3 FlashlightPos[NR_FLASHLIGHTS];
out vec3 FlashlightDir[NR_FLASHLIGHTS];
out vec3 DirLightDirection;

uniform vec3 lightPos[NR_POINT_LIGHTS];
uniform vec3 dirLightDirection;
uniform vec3 flashlightPos[NR_FLASHLIGHTS];
uniform vec3 flashlightDir[NR_FLASHLIGHTS];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(view * model))) * aNormal;
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		LightPos[i] = vec3(view * vec4(lightPos[i], 1.0));
	for(int i = 0; i < NR_FLASHLIGHTS; i++)
		FlashlightPos[i] = vec3(view * vec4(flashlightPos[i], 1.0));
	for(int i = 0; i < NR_FLASHLIGHTS; i++)
		FlashlightDir[i] = normalize(mat3(view) * flashlightDir[i]);
	DirLightDirection = mat3(view) * dirLightDirection;
	TextCoord = aTextCoord;
};