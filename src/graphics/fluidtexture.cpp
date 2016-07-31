#include "fluidtexture.h"

FluidTexture::FluidTexture(const std::shared_ptr<FluidSystem> &fluidSystem) :
    fluidSystem(fluidSystem)
{
    glGenTextures(DyeField::coords, &ids[0]);
}

void FluidTexture::generate() {
    for (std::size_t i = 0; i < DyeField::coords; ++i) {
        const auto &d = fluidSystem->density[i].dimensions();

        glBindTexture(GL_TEXTURE_2D, ids[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, d[0], d[1],
                     0, format, GL_FLOAT, fluidSystem->density[i].data() + d[0] * d[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void FluidTexture::update() {
    for (std::size_t i = 0; i < DyeField::coords; ++i) {
        const auto &d = fluidSystem->density[i].dimensions();

        glBindTexture(GL_TEXTURE_2D, ids[i]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, d[0], d[1], GL_RED, GL_FLOAT,
                        fluidSystem->density[i].data() + d[0] * d[1]);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void FluidTexture::bind(size_t channel) const {
    glBindTexture(GL_TEXTURE_2D, ids[channel]);
}
