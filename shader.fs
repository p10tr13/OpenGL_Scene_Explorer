#version 330 core

out vec4 FragColor;

in vec2 TextCoord;

uniform sampler2D texture1;
uniform vec3 lightColor;

void main()
{
   FragColor = texture(texture1, TextCoord) * vec4(lightColor, 1.0);
};