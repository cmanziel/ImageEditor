#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 2) in vec3 point_color;

uniform float point_size;

out vec3 out_point_color;

void main() {
	gl_PointSize = point_size;
	gl_Position = vec4(pos, 0.0, 1.0);

	out_point_color = point_color;
}