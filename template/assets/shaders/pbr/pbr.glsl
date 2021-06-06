#if !PBR_GLSL
#define PBR_GLSL 1

#include "../light.glsl"

const float PI = 3.14159265359;

struct PBRInput {
    vec3 albedo;
    vec3 F0;
    vec3 normal;
    vec3 irradiance;
    float metallic;
    float roughness;
    float ao;
    vec3 prefilteredColor;
    vec2 brdf;
};

vec3 fresnel_schlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float distribution_ggx(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float geometry_schlick_ggx(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometry_schlick_ggx(NdotV, roughness);
    float ggx1  = geometry_schlick_ggx(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 calculate_light_pbr(in vec3 L, in vec3 H, in vec3 V, in vec3 radiance, in PBRInput pbr){

    vec3 F  = fresnel_schlick(max(dot(H, V), 0.0), pbr.F0);
    float NDF = distribution_ggx(pbr.normal, H, pbr.roughness);
    float G   = geometry_smith(pbr.normal, V, L, pbr.roughness);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(pbr.normal, V), 0.0) * max(dot(pbr.normal, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - pbr.metallic;

    float NdotL = max(dot(pbr.normal, L), 0.0);
    return (kD * pbr.albedo / PI + specular) * radiance * NdotL;
}

vec3 calculate_point_light_pbr(in PointLight light, in vec3 P, in vec3 V, in PBRInput pbr){
    vec3 L = normalize(light.position - P);
    vec3 H = normalize(V + L);

    float distance    = length(light.position - P);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = (light.color.xyz * light.color.w) * attenuation;

    return calculate_light_pbr(L, H, V, radiance, pbr);
}

vec3 calculate_directional_light_pbr(in DirectionalLight light, in vec3 P, in vec3 V, in PBRInput pbr){
    vec3 L = normalize(light.direction);
    vec3 H = normalize(V + L);

    vec3 radiance = light.color.xyz * light.color.w;

    return calculate_light_pbr(L, H, V, radiance, pbr);
}

vec3 calculate_illumination_pbr(in Illumination illumination, in vec3 position, in vec3 viewDirection, in PBRInput pbr){

    vec3 directLighting = vec3(0);

    for (uint i = 0; i < illumination.directionalLightCount; i++){
        directLighting += calculate_directional_light_pbr(illumination.directionalLights[i], position, viewDirection, pbr);
    }
    for (uint i = 0; i < illumination.pointLightCount; i++){
        directLighting += calculate_point_light_pbr(illumination.pointLights[i], position, viewDirection, pbr);
    }

    vec3 kS = fresnel_schlick_roughness(max(dot(pbr.normal, viewDirection), 0.0), pbr.F0, pbr.roughness);

    //diffuse
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - pbr.metallic;
    vec3 diffuse = pbr.irradiance * pbr.albedo;

    //specular
    vec3 specular = pbr.prefilteredColor * (kS * pbr.brdf.x + pbr.brdf.y);

    vec3 ambient = (kD * diffuse + specular) * pbr.ao;

    return directLighting + ambient;
}

#endif