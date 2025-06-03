#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in mat4 instanceModel;

layout(set = 0, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;

void main()
{
    mat4 model = instanceModel;

    vec4 worldPosition = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPosition;

    FragPos = vec3(worldPosition);
    Normal = mat3(transpose(inverse(model))) * aNormal;
}
