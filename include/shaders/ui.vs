R"(
#version 330 core
uniform mat4 uVPMatrix;
in vec3 vCol;
in vec4 vPosUi;
in vec2 aTexCoords;

out vec3 color;
out vec2 TexCoords;
void main() {
    gl_Position = uVPMatrix * vec4(vPosUi.xyz, 1.0);
    color = vCol;
    TexCoords = vPosUi.zw;
}
)"