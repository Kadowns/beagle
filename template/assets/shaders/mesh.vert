#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0, set = 0) uniform CameraUniform {
    mat4 vp;
} uCamera;

//vertex
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

//instance
layout(location = 3) in mat4 aTransform;
layout(location = 7) in mat4 aInverseTransform;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec3 vPosition;

out gl_PerVertex{
    vec4 gl_Position;
};

void main() {
    vTexCoord = aTexCoord;
    vNormal = mat3(transpose(aInverseTransform)) * aNormal;
    vPosition = vec3(aTransform * vec4(aPosition, 1.0f));
    gl_Position =  uCamera.vp * vec4(vPosition, 1.0f);
}