R"(
#version 330 core
uniform vec4 uColor;
in vec2 vTexCoords;
uniform sampler2D textureMap;
uniform sampler2D texLut;

uniform float zmin;
uniform float zmax;

out vec4 fragColor;

void main() {
    vec4 texColor = texture(textureMap, vTexCoords);
    float zValue = texColor.r;

    float span = zmax - zmin;
    float factor = (zValue - zmin) / span;
    vec4 heat = texture(texLut, vec2(factor, 0.0));

    float gamma = 2.0;
   	fragColor.r = pow(heat.r, 1.0/gamma);
   	fragColor.g = pow(heat.g, 1.0/gamma);
   	fragColor.b = pow(heat.b, 1.0/gamma);
   	fragColor.a = 1.0;
}
)"




