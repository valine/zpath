R"(
#version 330 core
#define lightCount 4
#define PI 3.14159265359

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTextureCoords;

uniform vec4 uColor;
uniform vec3 uCameraPosition;

uniform vec3 uLightPositions[lightCount];
uniform vec3 uLightColors[lightCount];

uniform float uMetallic;
uniform float uRoughness;

uniform sampler2D sColorTexture;
uniform sampler2D sNormalTexture;

out vec4 fragColor;

float ao = 1;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main() {
	
    vec3 N = normalize(vNormal); 
    vec3 V = normalize(uCameraPosition - vPosition);

    vec3 Lo = vec3(0.0);
    vec3 texColor = texture(sColorTexture, vTextureCoords).rgb;
    texColor = pow(texColor, vec3(2.2));

    for(int i = 0; i < lightCount; i++) {

    	vec3 L = normalize(uLightPositions[i] - vPosition);
    	vec3 H = normalize(V + L);
  
		float distance    = length(uLightPositions[i] - vPosition);
    	float attenuation = 1.0 / (distance * distance);
    	vec3 radiance     = uLightColors[i] * attenuation; 

    	vec3 F0 = vec3(0.04); 
		F0      = mix(F0, uColor.rgb, uMetallic);
		vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

		float NDF = DistributionGGX(N, H, uRoughness);       
		float G   = GeometrySmith(N, V, L, uRoughness);     

		vec3 nominator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular     = nominator / max(denominator, 0.001);    

    	 vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - uMetallic;
	 	// add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * texColor / PI + specular) * radiance * NdotL; 

    }

  
 	vec3 ambient = vec3(0.002428, 0.021219, 0.063010) * texColor * ao;
    vec3 color = ambient + Lo;

    
    fragColor = vec4(color, 1.0);
}
)"


