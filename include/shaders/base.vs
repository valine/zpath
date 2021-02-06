R"(
#version 330 core
uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;


in vec3 aPos;
in vec3 aNormal;
in vec2 aTextureCoords;

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