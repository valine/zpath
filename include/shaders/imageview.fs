R"(
#version 330 core
uniform vec4 uColor;
varying vec2 TexCoords;

uniform sampler2D texture;

varying vec3 color;
void main() {
	vec4 texColor = texture2D(texture, TexCoords);
   	gl_FragColor = pow(texColor, vec4(vec3(1.0/2.2), 1.0));
}
)"



