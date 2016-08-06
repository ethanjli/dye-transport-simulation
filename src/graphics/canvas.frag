#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform float saturation;
uniform float opacity;
uniform int depth;

uniform sampler3D cyan;
uniform sampler3D magenta;
uniform sampler3D yellow;

void main()
{
    int numLayers = 4;
    vec3 rawColor = vec3(0.0, 0.0, 0.0);
    for (int layer = depth - 1; layer >= 0; layer--) {
        vec3 coords = vec3(TexCoords, layer * (1.0 / (numLayers - 1)));
        vec3 newColor = vec3(texture(cyan, coords).r, texture(magenta, coords).r, texture(yellow, coords).r);
        rawColor += newColor;
        rawColor *= opacity;
    }
    color = vec4(1 - rawColor * saturation, 1);
}
