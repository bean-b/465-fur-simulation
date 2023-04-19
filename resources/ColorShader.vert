#version 330


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;
uniform mat4 projection_mat, view_mat, model_mat;
uniform mat3 normal_mat;
out vec3 position_world;
out vec2 texture_coordinates;

out vec4 interpSurfPosition;
out vec3 interpSurfNormal;



uniform sampler2D furTex;

uniform float CurrentLayer; //value between 0 and 1
uniform float MaxHairLength; //maximum hair length



void main(void)
{
	
	interpSurfNormal = mat3(transpose(inverse(model_mat)))*vertex_normal;
	interpSurfPosition = model_mat * vec4(vertex_position, 1.0) + vec4(normalize(interpSurfNormal) * MaxHairLength *CurrentLayer, 1);
	texture_coordinates = vertex_texcoord;
	gl_Position = projection_mat * view_mat * interpSurfPosition;

}




