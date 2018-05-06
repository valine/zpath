

varying vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main() {             
    const float gamma = 2.2;
    vec4 hdrColor = texture(hdrBuffer, TexCoords);
    if(hdr){
        // exposure
        // vec4 result = vec4(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
      
        hdrColor *= exposure;
        
        // Reinhard
        vec4 result = hdrColor / (vec4(1) + hdrColor);
        result = pow(result, vec4(1.0 / gamma));

        // haarm-peter
        // vec3 ld = vec3(0.002);
        // float linReference = 0.18;
        // float logReference = 444;
        // float logGamma = 0.45;


        // vec3 LogColor;
        // LogColor.rgb = (log10(0.4*hdrColor.rgb/linReference)/ld*logGamma + logReference)/1023.0;
        // LogColor.rgb = saturate(LogColor.rgb);

        // float FilmLutWidth = 256;
        // float Padding = 0.5/FilmLutWidth;

        // vec3 retColor;
        // retColor.r = tex2D(FilmLut, vec2( lerp(Padding,1-Padding,LogColor.r), 0.5)).r;
        // retColor.g = tex2D(FilmLut, vec2( lerp(Padding,1-Padding,LogColor.g), 0.5)).r;
        // retColor.b = tex2D(FilmLut, vec2( lerp(Padding,1-Padding,LogColor.b), 0.5)).r;


        // Jim Hejl


        vec3 x = max(vec3(0.0),hdrColor.rgb-0.004);
        vec3 retColor = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);



        gl_FragColor = vec4(retColor.rgb, 1.0);
    }
    else
    {
        vec4 result = pow(hdrColor, vec4(1.0 / gamma));
        gl_FragColor = result;
    }
}