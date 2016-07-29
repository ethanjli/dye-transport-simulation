#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;

void main()
{
    vec4 pigment = texture(image, TexCoords); // Texture is actually CMYA
    //color = vec4(1 - pigment.r, 1 - pigment.g, 1 - pigment.b, pigment.a);
    color = vec4(1, 1, 1 - pigment.r, 1);
}
