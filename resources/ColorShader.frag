#version 330

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform sampler2D furTex;
in vec4 interpSurfPosition;
uniform float CurrentLayer;

uniform vec3 eye_world;

in vec3 position_world;
in vec3 normal_world;

uniform vec3 baseColor;


uniform float shadowIntensity = 0.6f;
uniform vec3 lightPos; 


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

    float shadow = mix(shadowIntensity, 1, CurrentLayer);
    fragColor *= shadow;

    if(CurrentLayer == 0){
        fragColor.a = 1.0;
    }




    // Ambient
    vec3 ambient = 0.2 * fragColor.rgb;
    // Diffuse
    vec3 lightDir = normalize(lightPos - position_world);
    vec3 normal = normalize(normal_world);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * fragColor.rgb;

    fragColor = vec4(ambient + diffuse, fragColor.a);

}
