#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;

void main()
{
    vec2 texCoords = chTex;
    texCoords.x = 1.0 - texCoords.x;
	vec4 texColor = texture(uTex, texCoords);
    outCol = texColor;
}