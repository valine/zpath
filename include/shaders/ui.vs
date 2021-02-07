R"(
#version 330 core
uniform mat4 uVPMatrix;
in vec3 vCol;
in vec4 vPosUi;

out vec3 uiColor;
out vec2 TexCoords;
void main() {
    gl_Position = uVPMatrix * vec4(vPosUi.xyz, 1.0);
    TexCoords = vPosUi.zw;
}
)"