R"(
#version 330 core
in vec3 aPos;
in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
	gl_Position = (vec4(aPos, 1.0) - vec4(0.5,0.5, 0.0, 0.0)) * vec4(2.0, 2.0, 1.0, 1.0);
}
)"