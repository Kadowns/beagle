#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0, set = 0) uniform CameraUniform {
    mat4 vp;
} uCamera;

//vertex
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

//instance
layout(location = 3) in mat4 aTransform;

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vTexCoord;

out gl_PerVertex{
    vec4 gl_Position;
};

void main() {
    vColor = aColor;
    vTexCoord = aTexCoord;
    gl_Position =  uCamera.vp * aTransform * vec4(aPosition, 1.0f);
}