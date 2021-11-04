R"(
#version 330 core

uniform sampler2D imageTexture;

uniform vec4 uColor;
uniform vec4 uTint;
uniform float uTexScale;
in vec2 TexCoords;

out vec4 fragColor;
void main() {
	vec4 texColor = texture(imageTexture, TexCoords * vec2(1.0 / uTexScale)) * uTint;
	float gamma = 1.0 / 2.2;
	vec4 gCorrect = pow(texColor, vec4(vec3(1.0/2.2), 1.0));
   	fragColor = gCorrect;
}
)"




