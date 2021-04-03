R"(
#version 330 core

in vec4 vPos;

out vec4 fragColor;

uniform vec4 uRadius;
uniform float uWidth;
uniform float uHeight;

void main() {
    float vposx = abs(vPos.x);
    float vposy = abs(vPos.y);

    float topLeft = pow(vposx, uWidth / uRadius.x) +
                    pow(vposy, uHeight / uRadius.x);

    float topRight = pow(vposx, uWidth / uRadius.y) +
                     pow(vposy, uHeight / uRadius.y);

    float bottomRight = pow(vposx, uWidth / uRadius.z) +
                     pow(vposy, uHeight / uRadius.z);

    float bottomLeft = pow(vposx, uWidth / uRadius.w) +
                        pow(vposy, uHeight / uRadius.w);

    float isTop = (vPos.y + 1.0) / 2.0;
    float isLeft = (-vPos.x + 1.0) / 2.0;
    float isBottom = (-vPos.y + 1.0) / 2.0;
    float isRight = (vPos.x + 1.0) / 2.0;

    float val = (topLeft * isTop * isLeft) +
                (topRight * isTop * isRight) +
                (bottomRight * isBottom * isRight) +
                (bottomLeft * isBottom * isLeft);

    float isOn = float(val <= 1);
    float outline = float(val <= 1 && val >= 0.5);
   	fragColor.r = mix(0.5, 1.0, outline);
   	fragColor.g = 1.0;
   	fragColor.b = 1.0;
   	fragColor.a = isOn;
}
)"




