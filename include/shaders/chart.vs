R"(
#version 330 core
uniform mat4 uVPMatrix;
attribute vec4 vPosUi;


void main() {
    gl_Position = uVPMatrix * vec4(vPosUi.xyz, 1.0);
}
)"