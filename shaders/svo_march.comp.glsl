#version 430

struct voxel {
    vec3 position;
    vec3 color;
    float size;
};

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(std430, binding = 0) buffer SVOBuffer {
    voxel voxels[];
};

void main()
{
    // Calculate the global position in the 3D grid
    uint globalX = gl_GlobalInvocationID.x;
    uint globalY = gl_GlobalInvocationID.y;
    uint globalZ = gl_GlobalInvocationID.z;

    // Calculate the index in the flattened SVO buffer
    uint index = globalX + globalY * 128 + globalZ * 128 * 128;

    // Perform some operation on the voxel data
    voxels[index].color += vec3(0.1, 0.1, 0.1);
}
