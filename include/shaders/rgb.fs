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

    float gamma = 2.0;
   	fragColor.r = pow(texColor.r, 1.0/gamma);
   	fragColor.g = pow(texColor.g, 1.0/gamma);
   	fragColor.b = pow(texColor.b, 1.0/gamma);
   	fragColor.a = 1.0;
}
)"




