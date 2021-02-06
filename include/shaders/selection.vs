R"(
#version 330 core
uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

in vec3 aPos;
in vec3 aNormal;

void main() {

    gl_Position = (uProjectionMatrix * uViewMatrix * uModelMatrix) * vec4(aPos, 1.0);
}
)"

