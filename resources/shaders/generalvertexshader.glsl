uniform mat4 uVPMatrix;
attribute vec3 vCol;
attribute vec3 vPos;
varying vec3 color;

void main() {
    gl_Position = vec4(vPos, 1.0);
    color = vec3(1.0, 1.0, 1.0);
};