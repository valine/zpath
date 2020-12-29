R"(
#version 330 core
uniform vec4 uColor;
varying vec2 vTexCoords;
uniform sampler2D texture;


void main() {
    vec4 texColor = texture2D(texture, vTexCoords);
    gl_FragColor = texColor;
    float gamma = 2.0;
   	gl_FragColor.r = pow(texColor.r, 1.0/gamma);
   	gl_FragColor.g = pow(texColor.r, 1.0/gamma);
   	gl_FragColor.b = pow(texColor.r, 1.0/gamma);
   	gl_FragColor.a = 1.0;
}
)"




