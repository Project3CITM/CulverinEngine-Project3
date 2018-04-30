#version 330 core


 
out vec4 color;

 

in vec3 camera_to_vertex;
in vec3 vertex_normal;

void main()
{


vec4 highlight = vec4(0.0, 0.5, 1.0, 1.0);

 color = highlight;

	color = vec4((highlight * (1 - (dot(vertex_normal, camera_to_vertex) * 2.5))).xyz, 1.0);


}