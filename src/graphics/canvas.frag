#version 330 core
in vec2 TexCoords;
out vec4 color;

// Color
uniform sampler3D cyan;
uniform sampler3D magenta;
uniform sampler3D yellow;
uniform float saturation;

// Layer blurring
uniform int width;
uniform int height;
uniform int blur;

// Layer blending
uniform float visibility;
uniform int scatterDepth;

void main()
{
    int numLayers = 4;
    vec3 cmyColor = vec3(0.0, 0.0, 0.0);
    for (int layer = scatterDepth - 1; layer >= 0; layer--) {
        float layerDepth = layer * 1.0 / (numLayers - 1);
        vec3 layerCoords = vec3(TexCoords, layerDepth);
        vec3 layerColor = vec3(0.0, 0.0, 0.0);
        int neighboringSamples = 0;
        for (int i = -layer * blur; i <= layer * blur; i++) {
            for (int j = -layer * blur; j <= layer * blur; j++) {
                vec3 neighborCoords = layerCoords;
                neighborCoords.s += 1.0 * i / width;
                neighborCoords.t += 1.0 * j / height;
                layerColor += vec3(texture(cyan, neighborCoords).r,
                                   texture(magenta, neighborCoords).r,
                                   texture(yellow, neighborCoords).r);
                neighboringSamples++;
            }
        }
        layerColor /= float(neighboringSamples);
        cmyColor += layerColor;
        cmyColor *= visibility;
    }
    color = vec4(1 - cmyColor * saturation, 1);
}
