#ifndef DISCRETE_MATERIAL_H
#define DISCRETE_MATERIAL_H

#include "util/fixed_point.h"
#include "util/Colour.h"
#include <cinttypes>


struct Material {

        // mechanics
        fixed32_high frictionCoefficient;
        fixed32_high density;
        fixed32_high restitutionCoefficient;

        // lighting
        fixed32_high metallicness;
        fixed32_high roughness;
        Color albedo;

};

namespace Materials {

     const Material Gold {
        .frictionCoefficient = 0.47,
        .density = 19320,
        .restitutionCoefficient = 0.3,
        .metallicness = 1,
        .roughness = 0,
        .albedo = {
            .r = 255,
            .g = 215,
            .b = 0,
            .a = 1
        }
    };

}


#endif //DISCRETE_MATERIAL_H
