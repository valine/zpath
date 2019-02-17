R"(
#version 330 core
uniform mat4 uVPMatrix;
attribute vec3 vCol;
attribute vec4 vPos;
attribute vec2 aTexCoords;

varying vec3 color;
varying vec2 TexCoords;
void main() {
    gl_Position = uVPMatrix * vec4(vPos.xy, 0.0, 1.0);
    color = vCol;
    TexCoords = vPos.zw;
}
)"