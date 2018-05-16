R"(
uniform mat4 uVPMatrix;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

attribute vec3 aPos;

void main() {
    gl_Position = (uProjectionMatrix * uViewMatrix) * vec4(aPos, 1.0);
}
)"