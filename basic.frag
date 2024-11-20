#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform vec3 sunColor;
uniform bool isSun;
uniform bool mirror;

void main()
{
    vec2 texCoords = chTex;
    if (mirror) {
        texCoords.x = 1.0 - texCoords.x;
    }

	vec4 texColor = texture(uTex, texCoords);

	if (isSun) {
        outCol = vec4(sunColor, 1.0) * texColor;
    } else {
        outCol = texColor;
    }
}