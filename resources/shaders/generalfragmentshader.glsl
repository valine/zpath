varying vec3 vPosition;
varying vec3 vNormal;

//uniform vec4 uColor;

void main() {


	vec4 uColor = vec4(1.0, 0.9, 0.9, 1.0);
	
	vec3 uAmbitentLightColor = vec3(0.031373, 0.156863, 0.278431);
	float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uAmbitentLightColor;

    vec3 uLightPosition = vec3(4.0, 7.0, 0.0);
	vec3 uLightColor = vec3(1.0, 1.0, 1.0);
	float lightBrightness = 10.0;

	vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(uLightPosition - vPosition);  
	
	float attenuation = 2;
	float distance = distance(uLightPosition, vPosition);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = diff * uLightColor;

	vec3 result = (diffuse + ambient) * uColor.xyz;

    gl_FragColor = vec4(result, 1.0);
}




