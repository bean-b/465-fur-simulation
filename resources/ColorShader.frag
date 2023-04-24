#version 330

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform sampler2D furTex;
in vec4 interpSurfPosition;
uniform float CurrentLayer;


uniform vec3 baseColor;

in vec2 texture_coordinates;
void main() 
{
    
    
    fragColor = texture(furTex, texture_coordinates);

    float furVisibility = 1;

    if(CurrentLayer > fragColor.a){
        furVisibility = 0;
    }

    if(CurrentLayer == 0){
        fragColor.rgb = baseColor;
        fragColor.a = 1.0;
    }else{
    fragColor.a = furVisibility;
   }

    float shadow = mix(0.6, 1, CurrentLayer);
    fragColor *= shadow;

}
