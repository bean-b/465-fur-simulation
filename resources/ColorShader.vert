#version 330

// Vertex shader

// INPUT: from the C++ program


// These are "global" variables that are set with a specific value within the application code:
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

// These variables are automatically assigned the value of each vertex and cooresponding normal and texcoord
// as they pass through the rendering pipeline. The layout locations are based on how the VAO was organized
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;

// OUTPUT: to the fragment shader
// Normal of the current point on the surface, interpolated across the surface.
out vec3 interpSurfNormal;


out vec2 interpTexCoord;
//TODO: Make an output variable to output the texture coordinate


void main(void)
{
    
    // We need the normal to calculate lighting.  So, we will similarly pass it on to the fragment
    // program as an "out" variable, and we'll do a matrix multiplication.  However,
    // it turns out you have to use a slightly different matrix for normals because they transform a
    // bit differently than points.
    interpSurfNormal = vec3(normalMatrix * vec4(vertex_normal, 0.0));
    interpTexCoord = vertex_texcoord;
    
    
    //TODO: output the texture coordinate to the fragment shader

    
    // This is the last line of almost every vertex shader program.  We don't need this for our lighting
    // calculations, but it is required by OpenGl.  Whereas a fragment program must output a color
    // as its final result, a vertex program must output a vertex position that has been projected into the
    // 2D screen space as its final result.  The line below does this.  This is exactly what OpenGL
    // would do by default for us if we didn't write our own custom vertex program.
    gl_Position = modelViewProjectionMatrix * vec4 (vertex_position, 1.0);
}
