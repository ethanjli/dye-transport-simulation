#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D cyan;
uniform sampler2D magenta;
uniform sampler2D yellow;

void main()
{
    color = vec4(1 - texture(cyan, TexCoords).r,
                 1 - texture(magenta, TexCoords).r,
                 1 - texture(yellow, TexCoords).r, 1);
}
