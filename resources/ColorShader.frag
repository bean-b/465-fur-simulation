#version 330

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform sampler2D furTex;
uniform sampler2D skinTex;
in vec4 interpSurfPosition;
uniform float CurrentLayer;

uniform vec3 eye_world;

in vec3 position_world;
in vec3 normal_world;



uniform float shadowIntensity;
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
        fragColor = texture(skinTex, texture_coordinates) * 0.3f;
        fragColor.a = 1.0;
    }else{
        fragColor.a = furVisibility;
        float shadow = mix(shadowIntensity, 1, CurrentLayer);
        fragColor *= shadow;
    }







    float aStr = 0.58f;
    float dStr = 1.1f;
    float sStr = 0.04f;


    // Ambient
    vec3 ambient = aStr * fragColor.rgb;
    // Diffuse
    vec3 lightDir = normalize(lightPos - position_world);
    vec3 normal = normalize(normal_world);
    float diff = max(dot(lightDir, normal), 0.0);


    vec3 diffuse = diff * fragColor.rgb;

    diffuse = diffuse * dStr;

    float spec = 0.0;
    vec3 viewDir = normalize(eye_world - position_world);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(sStr) * spec;

    fragColor = vec4(ambient + diffuse + specular, fragColor.a);

}
