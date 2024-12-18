#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform vec3 sunColor;

void main()
{
	vec4 texColor = texture(uTex, chTex);
    outCol = vec4(sunColor, 1.0) * texColor;
}