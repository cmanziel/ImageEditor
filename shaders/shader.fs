#version 330 core

layout(location = 0) out vec4 color;

//uniform vec3 point_color;

in vec3 out_point_color; // input from vertex shader

void main() {
	color = vec4(out_point_color, 1.0);
}