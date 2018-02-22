




varying vec3 vPosition;
varying vec3 vNormal;

//uniform vec4 uColor;

void main() {

	vec3 uLightPosition = vec3(4.0, 7.0, 0);
	vec3 uLightColor = vec3(1.0, 1.0, 1.0);
	float lightBrightness = 3.0;
	vec4 uColor = vec4(1.0, 0.9, 0.9, 1.0);

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor;

	vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(uLightPosition - vPosition);  
	
	float attenuation = 2.2;
	float distance = distance(uLightPosition, vPosition) / attenuation;
	float diff = dot(norm, lightDir);

	vec3 diffuse = diff * uLightColor * lightBrightness / pow(distance, attenuation);

	vec3 result = diffuse * uColor.xyz;

    gl_FragColor = vec4(result, 1.0);


    //float gamma = 2.0;
   	//gl_FragColor.r = pow(gl_FragColor.r, 1.0/gamma);
   	//gl_FragColor.g = pow(gl_FragColor.g, 1.0/gamma);
   	//gl_FragColor.b = pow(gl_FragColor.b, 1.0/gamma);
}




