#version 330

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform sampler2D furTex;
uniform float CurrentLayer;


in vec2 texture_coordinates;
void main() 
{

    
    fragColor = texture(furTex, texture_coordinates);

    if(CurrentLayer == 0){
        fragColor.rgb = vec3(166.0/255.0, 75.0/255.0,  42.0/255.0);
        fragColor.a = 1.0;
    };

    float shadow = mix(0.4,1,CurrentLayer);
    fragColor *= shadow;

}
