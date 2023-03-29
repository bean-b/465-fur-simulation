#version 330

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform sampler2D furTex;


in vec2 texCoord;
void main() 
{
    
    fragColor = texture(furTex, texCoord);
}
