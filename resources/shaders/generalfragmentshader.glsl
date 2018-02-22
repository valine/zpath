varying vec3 vPosition;
varying vec3 vNormal;

//uniform vec4 uColor;
uniform vec3 uCameraPosition;
void main() {
	vec4 uColor = vec4(1.0, 0.9, 0.9, 1.0);
	
	// Ambitent light
	vec3 uAmbitentLightColor = vec3(0.031373, 0.156863, 0.278431);
	float ambientStrength = 0.5;
    vec3 ambient = vec3(ambientStrength) * uAmbitentLightColor;

    // light propertices 
    vec3 uLightPosition = vec3(-7.0, 7.0, 5.0);
	vec3 uLightColor = vec3(1.0, 1.0, 1.0);
	float lightBrightness = 10.0;
    vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(uLightPosition - vPosition);  

    // diffuse light
	float attenuation = 1;
	float distance = distance(uLightPosition, vPosition);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = diff * uLightColor;

	// spectular light 
	float specularStrength = 1;
	vec3 viewDir = normalize(uCameraPosition - vPosition);
	vec3 reflectDir = reflect(-lightDir, norm);  
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
	vec3 specular = specularStrength * spec * uLightColor;  

	// result

	vec3 result = (diffuse + ambient + specular) * uColor.xyz;

    gl_FragColor = vec4(result, 1.0);
}




