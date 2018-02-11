//This is meant to be an editor of shaders
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 color;
out vec3 ourColor;
out vec2 TexCoord;
out vec3 pos;
out vec3 Position;
out vec3 Normals;
out vec3 Vposition;
uniform mat4 mat_model;
uniform mat4 projection_view;
uniform float time;

vec3 rotate(vec3 v, vec3 axis, float angle);
mat4 rotationMatrix(vec3 axis, float angle);
float pi = 3.1415926535;

void main()
{
Position = position;


pos = vec3(position);


pos.z = position.z + (sin(time+pos.x-pos.y)+0.8*sin(time+pos.x+pos.y));

 gl_Position = projection_view * mat_model* vec4(pos, 1.0f);

 
 Normals = normals;
 TexCoord = texCoord;
  Vposition = position;
 Vposition.z = position.z + 0.8*(sin(time+pos.x-pos.y));

Normals = rotate(Normals,vec3(0,1,0),radians(abs(sin(time/2*pi))))+position.z + (cos(time+pos.x-pos.y))*0.5;


}


mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float sc = 1.0 - c;
    
    return mat4(sc * axis.x * axis.x + c,           sc * axis.x * axis.y - axis.z * s,  sc * axis.z * axis.x + axis.y * s,  0.0,
                sc * axis.x * axis.y + axis.z * s,  sc * axis.y * axis.y + c,           sc * axis.y * axis.z - axis.x * s,  0.0,
                sc * axis.z * axis.x - axis.y * s,  sc * axis.y * axis.z + axis.x * s,  sc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}
vec3 rotate(vec3 v, vec3 axis, float angle) {
	mat4 m = rotationMatrix(axis, angle);
	return (m * vec4(v, 1.0)).xyz;
}