#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform GlobalUbo {
    mat4 vp;
} uGlobal;

layout(location = 0) in vec3 aPosition;

layout(location = 0) out vec3 vTexCoord;

out gl_PerVertex{
    vec4 gl_Position;
};

void main() {
    vTexCoord = aPosition;
    vec4 position = uGlobal.vp * vec4(aPosition, 1.0);
    gl_Position = position.xyww;
}
