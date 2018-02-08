varying vec3 vColor;

uniform vec4 uColor;

void main() {
    gl_FragColor = vec4(vColor, 1.0);


    float gamma = 2.0;
   	gl_FragColor.r = pow(gl_FragColor.r, 1.0/gamma);
   	gl_FragColor.g = pow(gl_FragColor.g, 1.0/gamma);
   	gl_FragColor.b = pow(gl_FragColor.b, 1.0/gamma);
};




