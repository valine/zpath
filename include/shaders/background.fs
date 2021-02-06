R"(
#version 330 core
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform vec3 uColorFactor;

out vec4 fragColor;
void main()
{		
    vec3 envColor = texture(environmentMap, WorldPos).rgb;
    
    // HDR tonemap and gamma correct
   // envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor * uColorFactor, vec3(2.2)); 
    fragColor = vec4(envColor, 1.0);
}

)"
