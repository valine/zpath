R"(
#version 330 core

in vec4 vPos;

out vec4 fragColor;

uniform vec4 uRadius;
uniform vec4 uColor;
uniform float uWidth;
uniform float uHeight;
uniform float uLineWidth;
uniform vec4 uOutlineColor;

float sigmoid(float x) {
    return 1.0 / (1.0 + exp(-x));
}

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

    float falloff = 80.0;
    float lineWidth = uLineWidth / uWidth;
    float width = 1.0 - pow(lineWidth, 1.0 / (uRadius.x));
    float isOn = float((1.0 - sigmoid((abs(val) - 1.0) * falloff)));
    float outline = float((1.0 - sigmoid((abs(val) - width) * falloff)));

    vec4 color = mix(uOutlineColor, uColor, outline);
    float intensity = 1.0;
   	fragColor.r = color.r;
   	fragColor.g = color.g;
   	fragColor.b = color.b;
   	fragColor.a = isOn;
}
)"




