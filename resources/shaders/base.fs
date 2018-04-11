#define lightCount 4
#define PI 3.14159265359

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

varying vec3 vPosition;
varying vec3 vNormal;

uniform vec4 uColor;
uniform vec3 uCameraPosition;

uniform vec3 uLightPositions[lightCount];
uniform vec3 uLightColors[lightCount];

uniform float uMetallic;
uniform float uRoughness;

float ao = 1;


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
     return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}  

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
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

float luma(vec3 color) {
  return dot(color, vec3(0.299, 0.587, 0.114));
}

void main() {
    
    vec3 N = normalize(vNormal); 
    vec3 V = normalize(uCameraPosition - vPosition);
    vec3 R = reflect(-V,N); 

    vec3 Lo = vec3(0.0);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, uColor.rgb, uMetallic);

    for(int i = 0; i < lightCount; i++) {
        vec3 L = normalize(uLightPositions[i] - vPosition);
        vec3 H = normalize(V + L);
  
        float distance    = length(uLightPositions[i] - vPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = uLightColors[i] * attenuation; 
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
        Lo += (kD * uColor / PI + specular) * radiance * NdotL; 

    }

     // ambient lighting (we now use IBL as the ambient term)

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, uRoughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - uMetallic;     

    // vec3 iN = N;
    // iN.x = -iN.x;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    irradiance = pow(irradiance, vec4(2.2));
    vec3 diffuse      = irradiance * uColor;


      R.y = -R.y;
    R.x = -R.x;
 // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  uRoughness * MAX_REFLECTION_LOD).rgb;    
    prefilteredColor = pow(prefilteredColor, vec4(2.2));
    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), uRoughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);


    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;

    float alpha = uColor.a + luma(specular) + F * 3;

    if (alpha > 1.0) {
        alpha = 1.0;
    }

    gl_FragColor = vec4(color, alpha);
}
