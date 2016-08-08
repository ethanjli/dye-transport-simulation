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
uniform float blurDiscreteness;

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
        // This box blur implementation was based (with heavy modifications) on
        // http://blog.trsquarelab.com/2015/12/a-simple-box-blur-implementation-in.html
        for (int i = -layer * blur; i <= layer * blur; i++) {
            for (int j = -layer * blur; j <= layer * blur; j++) {
                vec3 neighborCoords = layerCoords;
                neighborCoords.s += blurDiscreteness * i / width;
                neighborCoords.t += blurDiscreteness * j / height;
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
