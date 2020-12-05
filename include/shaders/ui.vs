R"(
#version 330 core
uniform mat4 uVPMatrix;
attribute vec3 vCol;
attribute vec4 vPosUi;
attribute vec2 aTexCoords;

varying vec3 color;
varying vec2 TexCoords;
void main() {
    gl_Position = uVPMatrix * vec4(vPosUi.xyz, 1.0);
    color = vCol;
    TexCoords = vPosUi.zw;
}
)"