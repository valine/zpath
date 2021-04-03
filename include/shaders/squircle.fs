R"(
#version 330 core

in vec4 vPos;

out vec4 fragColor;

uniform float uRadius;
uniform float uWidth;
uniform float uHeight;

void main() {
    float xRadius = uWidth / (uRadius);
    float yRadius = uHeight / (uRadius);

    float val = pow(abs(vPos.x), xRadius) + pow(abs(vPos.y), yRadius);
    float isOn = float(val < 1.0);
   	fragColor.r = 1.0;
   	fragColor.g = 1.0;
   	fragColor.b = 1.0;
   	fragColor.a = isOn;
}
)"




