varying vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    vec3 envColor = texture2D(environmentMap, WorldPos).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    gl_FragColor = vec4(envColor, 1.0);
}
