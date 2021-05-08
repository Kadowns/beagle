#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "light.glsl"
#include "phong.glsl"

layout(binding = 1, set = 0) uniform GlobalUniform {
    Illumination illumination;
    vec3 viewPosition;
} uGlobal;

layout(binding = 0, set = 1) uniform MaterialUniform {
    vec3 color;
} uMaterial;

layout (binding = 1, set = 1) uniform sampler2D uImage;

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vPosition;

layout(location = 0) out vec4 outColor;


void main() {
    vec3 illumination = calculate_illumination_phong(uGlobal.illumination, vPosition, normalize(vNormal), uGlobal.viewPosition);
    vec3 result = uMaterial.color * illumination * texture(uImage, vTexCoord).xyz;
    outColor = vec4(result, 1);
}
