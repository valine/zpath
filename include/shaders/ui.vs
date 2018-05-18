R"(
uniform mat4 uVPMatrix;
attribute vec3 vCol;
attribute vec3 vPos;
varying vec3 color;

void main() {
    gl_Position = uVPMatrix * vec4(vPos, 1.0);
    color = vCol;
}
)"