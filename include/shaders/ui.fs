R"(
#version 330 core
uniform vec4 uColor;
//varying vec2 TexCoords;

out vec4 fragColor;
void main() {

    fragColor = uColor;

    float gamma = 2.0;
   	fragColor.r = pow(fragColor.r, 1.0/gamma);
   	fragColor.g = pow(fragColor.g, 1.0/gamma);
   	fragColor.b = pow(fragColor.b, 1.0/gamma);
}
)"




