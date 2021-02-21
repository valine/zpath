R"(
#version 330

layout (location = 0) in vec3 aPos;

out vec4 vPos;

uniform mat4 uMatrix;

void main() {
    gl_Position = uMatrix * vec4(aPos.xyz, 1.0);
    vPos = vec4(aPos, 1.0);
}
)"