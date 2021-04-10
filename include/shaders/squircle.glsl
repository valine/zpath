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

    float minDimen = min(uWidth, uHeight);
    float spanX = max(0.0, (uWidth - uHeight)) / uWidth;
    float spanY = max(0.0, (uHeight - uWidth)) / uHeight;

    float vposx = max(0.0, abs(vPos.x) - spanX) / (1.0 - spanX);
    //float vposx = abs(vPos.x);
    float vposy =  max(0.0, abs(vPos.y) - spanY) / (1.0 - spanY);


    float topLeft = pow(vposx, minDimen / uRadius.x) +
                    pow(vposy, minDimen / uRadius.x);

    float topRight = pow(vposx, minDimen / uRadius.y) +
                     pow(vposy, minDimen / uRadius.y);

    float bottomRight = pow(vposx, minDimen / uRadius.z) +
                        pow(vposy, minDimen / uRadius.z);

    float bottomLeft = pow(vposx, minDimen / uRadius.w) +
                       pow(vposy, minDimen / uRadius.w);

    float cornerFalloff = 0.5;

    float isTop = float(vPos.y > 0.0);
    float isLeft = float(vPos.x < 0.0);
    float isBottom = 1.0 - isTop;
    float isRight = 1.0 - isLeft;

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




