#version 430

struct voxel {
    vec3 position;
    vec3 color;
    float size;
};

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(std430, binding = 0) buffer svo_buffer {
    voxel voxels[];
};

uniform vec3 voxel_resolution;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 ambient_color;

#define MAX_MARCHING_STEPS 256
#define MAX_DISTANCE 100.0

uint traverse_octree(in const uvec3 voxel_position, inout bool is_leaf)
{
    uint level_dim = uint(voxel_resolution.x);
    uint level_pos = voxel_position.x + voxel_position.y * level_dim + voxel_position.z * level_dim * level_dim;
    uint index = 0u;
    uint current = 0u;

    do {
        bvec3 cmp = greaterThanEqual(uvec3(level_pos), uvec3(level_dim >>= 1u));
        index = (index << 3u) | (uint(cmp.x) << 2u) | (uint(cmp.y) << 1u) | uint(cmp.z);
        current = is_leaf ? uint(voxels[index].color.a) : uint(voxels[index].size);
        level_pos -= uint(cmp) * level_dim;
    } while (current != 0u && level_dim > 1u);

    is_leaf = current != 0u;
    return index;
}

vec3 get_normal(vec3 position)
{
    const float epsilon = 0.001;
    vec3 normal;
    bool is_leaf = false;
    normal.x = float(traverse_octree(uvec3(position + vec3(epsilon, 0.0, 0.0)), is_leaf)) - float(traverse_octree(uvec3(position - vec3(epsilon, 0.0, 0.0)), is_leaf));
    normal.y = float(traverse_octree(uvec3(position + vec3(0.0, epsilon, 0.0)), is_leaf)) - float(traverse_octree(uvec3(position - vec3(0.0, epsilon, 0.0)), is_leaf));
    normal.z = float(traverse_octree(uvec3(position + vec3(0.0, 0.0, epsilon)), is_leaf)) - float(traverse_octree(uvec3(position - vec3(0.0, 0.0, epsilon)), is_leaf));
    return normalize(normal);
}

vec3 get_shaded_color(vec3 position, vec3 color, vec3 normal)
{
    vec3 light_dir = normalize(light_position - position);
    float diffuse = max(dot(normal, light_dir), 0.0);
    vec3 ambient = ambient_color * color;
    vec3 diffuse_color = diffuse * light_color * color;
    return ambient + diffuse_color;
}

void main()
{
    ivec3 voxel_index = ivec3(gl_GlobalInvocationID.xyz);
    int index = voxel_index.x + voxel_index.y * int(voxel_resolution.x) + voxel_index.z * int(voxel_resolution.x) * int(voxel_resolution.x);
    voxel current_voxel = voxels[index];

    if (current_voxel.size > 0.0)
    {
        vec3 voxel_position = current_voxel.position;
        vec3 ray_direction = normalize(voxel_position - camera_position);
        vec3 ray_origin = camera_position;
        float t = 0.0;
        bool is_leaf_current = false;
        traverse_octree(uvec3(voxel_position), is_leaf_current);

        vec3 accumulated_color = vec3(0.0);

        for (int i = 0; i < MAX_MARCHING_STEPS; i++)
        {
            vec3 current_position = ray_origin + ray_direction * t;
            bool is_leaf = false;
            uint level = traverse_octree(uvec3(current_position), is_leaf);

            if (level == 0u || t > MAX_DISTANCE)
            {
                break;
            }

            vec3 voxel_color = vec3(1.0);

            if (is_leaf)
            {
                int leaf_index = int(level);
                voxel_color = voxels[leaf_index].color;
            }

            vec3 normal = get_normal(current_position);
            vec3 shaded_color = get_shaded_color(current_position, voxel_color, normal);

            accumulated_color += shaded_color;

            float step_size = 0.1;
            t += step_size * current_voxel.size;

            float opacity_threshold = 0.95;

            if (shaded_color.a > opacity_threshold)
            {
                break;
            }
        }

        vec4 final_color = vec4(accumulated_color, 1.0);
    }
}

