R"(
#version 330
in vec4 vPosUi;

uniform mat4 uVPMatrix;

out vec3 uiColor;
out vec2 TexCoords;
void main() {
    gl_Position = uVPMatrix * vec4(vPosUi.xyz, 1.0);
    TexCoords = vPosUi.zw;
}
)"