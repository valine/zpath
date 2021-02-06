R"(
#version 330 core
uniform vec4 uColor;
in vec2 TexCoords;
uniform sampler2D imageTexture;
in vec3 color;

out vec4 fragColor;
void main() {
	vec4 texColor = texture(imageTexture, TexCoords);
	float gamma = 1.0 / 2.2;
   	fragColor = pow(texColor, vec4(vec3(1.0/2.2), 1.0)) * pow(uColor, vec4(vec3(gamma),1.0));
}
)"




