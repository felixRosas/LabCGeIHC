#version 330 core	//version con que trabajaremos
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

out vec3 our_color;		//variable de salida
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){

	gl_Position = projection * view * model * vec4(in_position, 1.0);
	our_color = in_color;

}
