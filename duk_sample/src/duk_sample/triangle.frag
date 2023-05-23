#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vColor;

layout(location = 0) out vec4 oColor;


void main() {
    oColor = vec4(vColor, 1.0f);
}