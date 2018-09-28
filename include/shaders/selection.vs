R"(
#version 330 core
uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

attribute vec3 aPos;
attribute vec3 aNormal;

void main() {

    gl_Position = (uProjectionMatrix * uViewMatrix * uModelMatrix) * vec4(aPos, 1.0);
}
)"

