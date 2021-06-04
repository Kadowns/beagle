#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 1) uniform samplerCube uSkybox;

layout(location = 0) in vec3 vTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 color = texture(uSkybox, vTexCoord).rgb;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    outColor = vec4(color, 1.0);
}
