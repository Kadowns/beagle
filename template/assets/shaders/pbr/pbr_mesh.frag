#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "pbr.glsl"

layout(binding = 1, set = 0) uniform GlobalUniform {
    Illumination illumination;
    vec3 viewPosition;
} uGlobal;

layout(binding = 0, set = 1) uniform Material {
    PBRMaterial pbr;
} uMaterial;

layout (binding = 1, set = 1) uniform sampler2D uAlbedoMap;
layout (binding = 2, set = 1) uniform sampler2D uMetallicMap;
layout (binding = 3, set = 1) uniform sampler2D uRoughnessMap;
layout (binding = 4, set = 1) uniform sampler2D uAmbientOcclusionMap;

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vPosition;

layout(location = 0) out vec4 outColor;


void main() {

    PBRInput pbr;
    pbr.albedo = uMaterial.pbr.albedo.rgb * pow(texture(uAlbedoMap, vTexCoord).rgb, vec3(2.2));
    pbr.metallic = uMaterial.pbr.metallic * texture(uMetallicMap, vTexCoord).r;
    pbr.roughness = uMaterial.pbr.roughness * texture(uRoughnessMap, vTexCoord).r;
    pbr.ao = uMaterial.pbr.ao * texture(uAmbientOcclusionMap, vTexCoord).r;
    pbr.normal = normalize(vNormal);
    pbr.F0 = mix(vec3(0.04), uMaterial.pbr.albedo.xyz, uMaterial.pbr.metallic);

    vec3 color = calculate_illumination_pbr(uGlobal.illumination, vPosition, uGlobal.viewPosition, pbr);
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    outColor = vec4(color, 1);
}
