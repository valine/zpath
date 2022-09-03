R"(
#version 330 core
uniform mat4 uVPMatrix;
in vec4 vPosUi;

uniform vec4 uBounds;

vec4 eval(vec4 xIn) {
    return vec4(sin(xIn.x), 0.0, 0.0, 0.0);
}
void main() {

    float posX = vPosUi.x; // Zero to one screen space

    float x = mix(uBounds.x, uBounds.z, posX);
    float fx = eval(vec4(x, 0.0, 0.0, 0.0)).x;
    float y = (fx - uBounds.w) / (uBounds.y - uBounds.w);

    vec3 point = vec3(posX, y, vPosUi.z);
    gl_Position = uVPMatrix * vec4(point, 1.0);
}
)"