#version 330

// Vertex shader

// These variables are automatically assigned the value of each vertex and cooresponding normal and texcoord
// as they pass through the rendering pipeline. The layout locations are based on how the VAO was organized
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;

// These are "global" variables that are set with a specific value within the application code using:
// shader.setUniform("view_mat", view);
// shader.setUniform("projection_mat", projection);
// shader.setUniform("model_mat", model);

out vec2 texCoord;
uniform mat4 projection_mat, view_mat, model_mat;

float currentLayer;
uniform float maxHairLength;


void main(void) {
	
	texCoord = vertex_texcoord;

	currentLayer = 0;

	vec3 pos = vec3(vertex_position) + (vertex_normal * maxHairLength * currentLayer);
	vec4 viewPos = vec4(pos,1) + vec4(model_mat) - vec4(view_mat);

	gl_Position = viewPos;

}

