# discrete::vec3 position;
#     discrete::vec3 velocity;
#     ufixed32_mid diameter;
#     ufixed32_high frictionCoefficient;
#     ufixed32_high density;

import struct

# Define a function to create a binary format for Voxel
def pack_voxel(position, velocity, diameter, friction_coefficient, density):
    voxel_format = 'fff' + 'fff' + 'f' + 'f' + 'f'
    packed_data = struct.pack(voxel_format,
                              position[0], position[1], position[2],
                              velocity[0], velocity[1], velocity[2],
                              diameter, friction_coefficient, density)
    return packed_data

# Function to pack an array of voxels (world)
def pack_world(voxels):
    # Collect all packed voxels into a single byte string
    packed_voxels = b''.join(pack_voxel(*voxel) for voxel in voxels)
    return packed_voxels

def single_cube():

    voxels = [
        ((0.0, 0.0, 0.0), (0.0, 0.0, 0.0), 1.0, 0.0, 0.0),
    ]
    with open('../world.dat', 'wb') as file:
            file.write(pack_world(voxels))

if __name__ == '__main__':
    single_cube()
