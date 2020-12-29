R"(
#version 330 core
uniform vec4 uColor;
varying vec2 vTexCoords;
uniform sampler2D texture;
uniform sampler2D texLut;

uniform float zmin;
uniform float zmax;


void main() {
    vec4 texColor = texture2D(texture, vTexCoords);
    float zValue = texColor.r;

    float span = zmax - zmin;
    float factor = (zValue - zmin) / span;
    vec4 heat = texture2D(texLut, vec2(factor, 0.0));

    float gamma = 2.0;
   	gl_FragColor.r = pow(heat.r, 1.0/gamma);
   	gl_FragColor.g = pow(heat.g, 1.0/gamma);
   	gl_FragColor.b = pow(heat.b, 1.0/gamma);
   	gl_FragColor.a = 1.0;
}
)"




