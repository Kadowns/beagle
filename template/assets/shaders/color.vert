#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0, set = 0) uniform CameraUniform {
    mat4 vp;
} uCamera;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 vColor;

out gl_PerVertex{
    vec4 gl_Position;
};

void main() {
    vColor = aColor;
    gl_Position =  uCamera.vp * vec4(aPosition, 1.0f);
}