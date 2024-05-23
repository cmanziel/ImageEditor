#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in float draw;
layout(location = 2) in vec3 point_color;

uniform float point_size;

//uniform vec3 point_color; // color for points drawn by the brush
//uniform vec3 image_color; // the image's pixel color onto which the brush draws

out vec3 out_point_color;

void main() {
	gl_PointSize = point_size;
	gl_Position = vec4(pos, 0.0, 1.0);

	out_point_color = point_color;

	/*
	if(draw == 1.0)
		out_point_color = point_color;
	else
		out_point_color = image_color;
		*/
}