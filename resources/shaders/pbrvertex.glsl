#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;


void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    Normal = mat3(uModelMatrix) * aNormal;   

    gl_Position =  uProjectionMatrix * uViewMatrix * vec4(WorldPos, 1.0);
}