R"(
#version 330 core
uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoords;

void main() {

    gl_Position = (uProjectionMatrix * uViewMatrix * uModelMatrix) * vec4(aPos, 1.0);
}
)"

