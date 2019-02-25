R"(
#version 330 core
attribute vec3 aPos;
attribute vec2 aTexCoords;

varying vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
	gl_Position = (vec4(aPos, 1.0) - vec4(0.5,0.5, 0.0, 0.0)) * vec4(2.0, 2.0, 1.0, 1.0);
}
)"