R"(
#version 330 core

in vec2 vPos;

out vec4 fragColor;

uniform float uAlpha;

void main() {
   	fragColor.r = 0;
   	fragColor.g = 0;
   	fragColor.b = 0;
   	fragColor.a = uAlpha;
}
)"




