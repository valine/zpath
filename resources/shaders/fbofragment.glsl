

varying vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec4 hdrColor = texture(hdrBuffer, TexCoords);
    if(hdr)
    {
        // exposure
        vec4 result = vec4(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
      
        result = pow(result, vec4(1.0 / gamma));
        gl_FragColor = vec4(result.rgb, hdrColor.a);
    }
    else
    {
        vec4 result = pow(hdrColor, vec4(1.0 / gamma));
        gl_FragColor = result;
    }
}