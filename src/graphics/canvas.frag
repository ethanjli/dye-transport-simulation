#version 330 core
in vec2 TexCoords;
out vec4 color;

// Color
uniform sampler3D cyan;
uniform sampler3D magenta;
uniform sampler3D yellow;
uniform float saturation;

// Layer blending
uniform float visibility;
uniform int depth;

// Blurring
const float xPixelDistance = 1.0f / 96;
const float yPixelDistance = 1.0f / 96;
const int boxRange = 2;

void main()
{
    int numLayers = 4;
    vec3 cmyColor = vec3(0.0, 0.0, 0.0);
    for (int layer = depth - 1; layer >= 0; layer--) {
        float layerDepth = layer * 1.0 / (numLayers - 1);
        vec3 layerCoords = vec3(TexCoords, layerDepth);
        vec3 layerColor = vec3(0.0, 0.0, 0.0);
        /*
        vec3 layerColor = vec3(texture(cyan, layerCoords).r,
                               texture(magenta, layerCoords).r,
                               texture(yellow, layerCoords).r);
                               */
        int neighboringSamples = 0;
        for (int i = -boxRange; i <= boxRange; i++) {
            for (int j = -boxRange; j <= boxRange; j++) {
                vec3 neighborCoords = layerCoords;
                neighborCoords.s += i * layer * xPixelDistance;
                neighborCoords.t += j * layer * yPixelDistance;
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
