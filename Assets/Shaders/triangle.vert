#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;

layout(binding = 0) uniform UBO {
    mat4 projectionMatrix;
    mat4 viewMatrix;
} ubo;

layout(location = 0) out vec3 outColor;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    outColor = inColor.xyz;
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(inPos.xyz, 1.0);
}