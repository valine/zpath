R"(
#version 330 core

in vec4 vPos;
out vec4 fragColor;

uniform sampler2D textureMap;
uniform float uRadiusX;
uniform float uRadiusY;

float gauss(float x, float height, float width) {


    float value = height * exp(-pow(x, 2.0) / pow(2.0 * width, 2.0));
  //  float value = 0.4 * exp(-pow(x, 2.0) / 4.0);

    return value;
}


void main() {


    int steps = 5;

    vec4 sum = vec4(0);

    vec2 coord = max(min(vPos.xy, vec2(1.0)), vec2(0.0));
    for (int x = 0; x < steps; x++) {
        float factorX = float(x) / float(steps);
        float guassX = gauss(factorX * 2.0 - 1.0, 1.5, 0.5);


        for (int y = 0; y < steps; y++) {
            float factorY = float(y) / float(steps);

            float guassY = gauss(factorY * 2.0 - 1.0, 1.5, 0.5);

            float guassFinal = guassX * guassY;
            vec2 stepCoord = coord + vec2(mix(-uRadiusX, uRadiusX, factorX), mix(-uRadiusY, uRadiusY, factorY));

            vec4 texColor = texture(textureMap, stepCoord) * guassFinal;
            sum += texColor / vec4(float(steps * steps));
        }
    }


   	fragColor = sum;
}
)"




