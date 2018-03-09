#define lightCount 4

varying vec3 vPosition;
varying vec3 vNormal;

uniform vec4 uColor;
uniform vec3 uCameraPosition;

uniform vec3 uLightPositions[lightCount];
uniform vec3 uLightColors[lightCount];


void main() {

	//uLightColors[0] = vec3(1.0,1.0,1.0);
	// uLightColors[1] = vec3(1.0,1.0,1.0);
	// uLightColors[2] = vec3(1.0,1.0,1.0);
	// uLightColors[3] = vec3(1.0,1.0,1.0);

	// uLightPositions[0] = vec3( 5.0,  0.2,  2.0);
	// uLightPositions[1] =  vec3( 2.3, -12.3, -4.0);
	// uLightPositions[2] =vec3(-4.0,  2.0, -12.0);
	// uLightPositions[3] = vec3( 0.0,  0.0, -3.0);
	
	vec3 result = vec3(0.0, 0.0, 0.0);
	// Ambitent light

	vec3 uAmbitentLightColor = vec3(0.002428, 0.021219, 0.063010);
	float ambientStrength = 0.5;
    vec3 ambient = vec3(ambientStrength) * uAmbitentLightColor;

    for(int i = 0; i < lightCount; i++) {
		float lightBrightness = 0.3;
	    vec3 norm = normalize(vNormal);
		vec3 lightDir = normalize(uLightPositions[i] - vPosition);  

	    // diffuse light
		float attenuation = 1.0;
		float distance = distance(uLightPositions[i], vPosition);
		float diff = max(dot(norm, lightDir), 0.0);

		vec3 diffuse = diff * uLightColors[i] * lightBrightness;

		// spectular light 
		float specularStrength = 2.0;
		vec3 viewDir = normalize(uCameraPosition - vPosition);
		vec3 reflectDir = reflect(-lightDir, norm);  
		
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
		vec3 specular = specularStrength * spec * uLightColors[i]  * lightBrightness;

		result += diffuse;
		result += specular;  

    }

	
	// result

	result = (result + ambient) * uColor.xyz;

    gl_FragColor = vec4(result, 1.0);
}



