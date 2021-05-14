#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0, set = 0) uniform CameraUniform {
    mat4 vp;
} uCamera;

//vertex
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

//instance
layout(location = 5) in mat4 aTransform;
layout(location = 9) in mat4 aInverseTransform;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vPosition;
layout(location = 2) out mat3 vTBN;

out gl_PerVertex{
    vec4 gl_Position;
};

void main() {
    vTexCoord = aTexCoord;
    vec3 T = normalize(vec3(aTransform * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(aTransform * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(aTransform * vec4(aNormal,    0.0)));
    vTBN = mat3(T, B, N);
    vPosition = vec3(aTransform * vec4(aPosition, 1.0f));
    gl_Position =  uCamera.vp * vec4(vPosition, 1.0f);
}