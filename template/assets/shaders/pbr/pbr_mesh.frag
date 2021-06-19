#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "pbr.glsl"

//Global set
layout(binding = 1, set = 0) uniform GlobalUniform {
    Illumination illumination;
    vec3 viewPosition;
} uGlobal;

layout (binding = 2, set = 0) uniform samplerCube uIrradianceMap;
layout (binding = 3, set = 0) uniform samplerCube uPrefilteredMap;
layout (binding = 4, set = 0) uniform sampler2D uBrdfLUT;

//Material set
layout(binding = 0, set = 1) uniform Material {
    vec4  albedo;
    float metallic;
    float roughness;
    float ao;
} uMaterial;

layout (binding = 1, set = 1) uniform sampler2D uAlbedoMap;
layout (binding = 2, set = 1) uniform sampler2D uMetallicMap;
layout (binding = 3, set = 1) uniform sampler2D uRoughnessMap;
layout (binding = 4, set = 1) uniform sampler2D uAmbientOcclusionMap;
layout (binding = 5, set = 1) uniform sampler2D uNormalMap;

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vPosition;
layout(location = 2) in mat3 vTBN;

layout(location = 0) out vec4 outColor;


vec3 unpack_normal(in mat3 tbn, in vec2 texCoord, in sampler2D normalMap){
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, texCoord).rgb;
//    vec3 normal = vec3(0.0, 0.0, 1.0);

    // transform normal vector to range [-1,1]
    normal = normal * 2.0 - 1.0;

    //transform from tanget space to world space
    normal = normalize(tbn * normal);
    return normal;
}

void main() {

    PBRInput pbr;
    pbr.albedo = uMaterial.albedo.rgb * pow(texture(uAlbedoMap, vTexCoord).rgb, vec3(2.2));
    pbr.metallic = uMaterial.metallic * texture(uMetallicMap, vTexCoord).r;
    pbr.roughness = uMaterial.roughness * texture(uRoughnessMap, vTexCoord).r;
    pbr.ao = uMaterial.ao * texture(uAmbientOcclusionMap, vTexCoord).r;
    pbr.normal = unpack_normal(vTBN, vTexCoord, uNormalMap);
    pbr.F0 = mix(vec3(0.04), uMaterial.albedo.xyz, uMaterial.metallic);
    pbr.irradiance = texture(uIrradianceMap, pbr.normal).rgb;

    vec3 viewDirection = normalize(uGlobal.viewPosition - vPosition);
    vec3 reflected = reflect(-viewDirection, pbr.normal);

    const float MAX_REFLECTION_LOD = 4.0;
    pbr.prefilteredColor = textureLod(uPrefilteredMap, reflected, pbr.roughness * MAX_REFLECTION_LOD).rgb;
    pbr.brdf = texture(uBrdfLUT, vec2(max(dot(pbr.normal, viewDirection), 0.0), pbr.roughness)).rg;

    vec3 color = calculate_illumination_pbr(uGlobal.illumination, vPosition, viewDirection, pbr);
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    outColor = vec4(color, 1.0f);
}
