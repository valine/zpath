uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

attribute vec3 vCol;
attribute vec3 vPos;
attribute vec3 vNormal;

varying vec3 vColor;

void main() {
    gl_Position = (uProjectionMatrix * uViewMatrix) * vec4(vPos, 1.0);
    vColor = vNormal;
};