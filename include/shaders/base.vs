R"(
uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;


attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTextureCoords;

varying vec3 vColor;
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoords;

void main() {
    gl_Position = (uProjectionMatrix * uViewMatrix) * vec4(aPos, 1.0);
    vPosition = aPos;
    vNormal = aNormal;
    vTextureCoords = aTextureCoords;
}
)"