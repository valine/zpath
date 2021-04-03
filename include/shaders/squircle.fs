R"(
#version 330 core

in vec4 vPos;

out vec4 fragColor;

void main() {
    float val = pow(vPos.x, 4) + pow(vPos.y, 4.0);
    float isOn = float(val < 1.0);
   	fragColor.r = 1.0;
   	fragColor.g = 1.0;
   	fragColor.b = 1.0;
   	fragColor.a = isOn;
}
)"




