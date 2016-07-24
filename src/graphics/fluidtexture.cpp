#include "fluidtexture.h"

#include <iostream>

FluidTexture::FluidTexture(const std::shared_ptr<FluidSystem> &fluidSystem) :
    fluidSystem(fluidSystem)
{
    glGenTextures(1, &id);
}

void FluidTexture::generate() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, fluidSystem->density.cols(),
                 fluidSystem->density.rows(), 0, imageFormat, GL_FLOAT,
                 fluidSystem->density.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FluidTexture::update() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fluidSystem->density.cols(),
                    fluidSystem->density.rows(), GL_RED, GL_FLOAT,
                    fluidSystem->density.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FluidTexture::bind() const {
    glBindTexture(GL_TEXTURE_2D, id);
}
