varying vec3 vPosition;
varying vec3 vNormal;

uniform vec4 uColor;
uniform vec3 uCameraPosition;

vec3 pointLightPositions[] = {
	vec3( 5.0,  0.2,  2.0),
	vec3( 2.3, -12.3, -4.0),
	vec3(-4.0,  2.0, -12.0),
	vec3( 0.0,  0.0, -3.0)
};  


void main() {

	
	vec3 result = vec3(0.0, 0.0, 0.0);
	// Ambitent light
	vec3 uAmbitentLightColor = vec3(0.031373, 0.156863, 0.278431);
	float ambientStrength = 0.5;
    vec3 ambient = vec3(ambientStrength) * uAmbitentLightColor;

    for(int i = 0; i < 4; i++) {
    	vec3 uLightColor = vec3(1.0, 1.0, 1.0);
		float lightBrightness = 0.3;
	    vec3 norm = normalize(vNormal);
		vec3 lightDir = normalize(pointLightPositions[i] - vPosition);  

	    // diffuse light
		float attenuation = 1.0;
		float distance = distance(pointLightPositions[i], vPosition);
		float diff = max(dot(norm, lightDir), 0.0);

		vec3 diffuse = diff * uLightColor * lightBrightness;

		// spectular light 
		float specularStrength = 1.0;
		vec3 viewDir = normalize(uCameraPosition - vPosition);
		vec3 reflectDir = reflect(-lightDir, norm);  
		
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
		vec3 specular = specularStrength * spec * uLightColor  * lightBrightness;

		result += diffuse;
		result += specular;  

    }
    // light propertices 
    //vec3 uLightPosition = vec3(-7.0, 7.0, 5.0);
	
	// result

	result = (result + ambient) * uColor.xyz;

    gl_FragColor = vec4(result, 1.0);
}




