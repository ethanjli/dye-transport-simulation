#include "fluidtexture.h"

FluidTexture::FluidTexture(const std::shared_ptr<FluidSystem> &fluidSystem) :
    fluidSystem(fluidSystem)
{
    glGenTextures(DyeField::coords, &ids[0]);
}

void FluidTexture::generate() {
    for (std::size_t i = 0; i < DyeField::coords; ++i) {
        const auto &d = fluidSystem->density[i].dimensions();

        glBindTexture(GL_TEXTURE_3D, ids[i]);
        glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, d[0], d[1], d[2] - 2, 0, format, GL_FLOAT,
                     fluidSystem->density[i].data() + 1 * d[0] * d[1]);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterMin);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filterMax);
        glBindTexture(GL_TEXTURE_3D, 0);
    }
}

void FluidTexture::update() {
    for (std::size_t i = 0; i < DyeField::coords; ++i) {
        const auto &d = fluidSystem->density[i].dimensions();

        glBindTexture(GL_TEXTURE_3D, ids[i]);
        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, d[0], d[1], d[2] - 2, format, GL_FLOAT,
                        fluidSystem->density[i].data() + 1 * d[0] * d[1]);
        glBindTexture(GL_TEXTURE_3D, 0);
    }
}

void FluidTexture::bind(size_t channel) const {
    glBindTexture(GL_TEXTURE_3D, ids[channel]);
}
