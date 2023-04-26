#version 330


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;
uniform mat4 projection_mat, view_mat, model_mat;
uniform mat3 normal_mat;
out vec3 position_world;
out vec3 normal_world;
out vec2 texture_coordinates;

out vec4 interpSurfPosition;
out vec3 interpSurfNormal;



uniform sampler2D furTex;

uniform float CurrentLayer; //value between 0 and 1
uniform float MaxHairLength; //maximum hair length

uniform float gravPower;



void main(void)
{
	


	interpSurfNormal = mat3(transpose(inverse(model_mat)))*vertex_normal;
	vec4 pos = vec4(vertex_position, 1.0) + vec4(normalize(interpSurfNormal) * MaxHairLength *CurrentLayer, 1);
	interpSurfPosition = model_mat * pos;
	
//	vec3 grav = vec3(0, gravPower, 0.);
//	interpSurfPosition.xyz += grav*pow(CurrentLayer, 3);
	
	texture_coordinates = vertex_texcoord;
	gl_Position = projection_mat * view_mat * interpSurfPosition;


	position_world = vec3 (model_mat * vec4 (vertex_position, 1.0));
	normal_world = normalize(normal_mat * vertex_normal);

}




