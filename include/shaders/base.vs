R"(
#version 330 core
uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoords;

out vec3 vColor;
out vec3 vNormal;
out vec3 vPosition;
out vec2 vTextureCoords;

void main() {
    gl_Position = (uProjectionMatrix * uViewMatrix * uModelMatrix) * vec4(aPos, 1.0);
    vPosition = (uModelMatrix * vec4(aPos, 1.0)).xyz;
    vNormal = (uModelMatrix * vec4(aNormal, 0.0)).xyz;;
    vTextureCoords = aTextureCoords;
}
)"