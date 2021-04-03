R"(
#version 330 core

in vec4 vPos;

out vec4 fragColor;

uniform float uRadius;

void main() {
    float val = pow(abs(vPos.x), uRadius) + pow(abs(vPos.y), uRadius);
    float isOn = float(val < 1.0);
   	fragColor.r = 1.0;
   	fragColor.g = 1.0;
   	fragColor.b = 1.0;
   	fragColor.a = isOn;
}
)"




