//
// Created by Jake M O'Neill on 18/03/2023.
//

#include "mesh.h"

/**
 *
 * @return unit cube mesh
 */
const discrete::Mesh& discrete::cube_mesh() {
    static Mesh unit_cube{
            {
                    {{-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}},
                    {{0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}},
                    {{0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}},
                    {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}}
            },

            {
                    0, 2, 3,
                    4, 2, 3,
                    2, 5, 4,
                    4, 7, 5,
                    1, 7, 5,
                    1, 7, 6,
                    0, 6, 3,
                    0, 6, 1,
                    0, 5, 2,
                    0, 5, 1,
                    3, 7, 6,
                    3, 7, 4
            }
    };

    return unit_cube;
}