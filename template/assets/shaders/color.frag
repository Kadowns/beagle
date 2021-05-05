#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vColor;

layout(location = 0) out vec4 outColor;

layout (binding = 0, set = 1) uniform MaterialUniform {
    vec3 color;
} uMaterial;

void main() {
    outColor = vColor * vec4(uMaterial.color, 1);
}
