#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 influences;
layout(location = 4) in vec4 bones;

out float ourTime;
out vec4 ourColor;
out vec3 ourNormal;
out vec2 TexCoord;

uniform float _time;
uniform vec4 _color;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view;

uniform samplerBuffer _skinning_text;
uniform int _num_pixels;

void main()
{
    vec3 skinned_pos = vec3(0.0f, 0.0f, 0.0f);
    vec3 skinned_normal = vec3(0.0f, 0.0f, 0.0f);
    
    float total_weight = 0.0f;

bool test = false;

    for(int i = 0; i < 4; i++)
    {
        int bone_id = int (bones[i]);

        if(bone_id == -1)
            break;

        int start_buffer_pos = bone_id * 4 * 4;

        mat4 skinning_mat = mat4(
        //Column 0
        texelFetch(_skinning_text, start_buffer_pos).r,
        texelFetch(_skinning_text, start_buffer_pos + 1).r,
        texelFetch(_skinning_text, start_buffer_pos + 2).r,
        texelFetch(_skinning_text, start_buffer_pos + 3).r,
        //Column 1
        texelFetch(_skinning_text, start_buffer_pos + 4).r,
        texelFetch(_skinning_text, start_buffer_pos + 5).r,
        texelFetch(_skinning_text, start_buffer_pos + 6).r,
        texelFetch(_skinning_text, start_buffer_pos + 7).r,
        //Column 2
        texelFetch(_skinning_text, start_buffer_pos + 8).r,
        texelFetch(_skinning_text, start_buffer_pos + 9).r,
        texelFetch(_skinning_text, start_buffer_pos + 10).r,
        texelFetch(_skinning_text, start_buffer_pos + 11).r,
        //Column 3
        texelFetch(_skinning_text, start_buffer_pos + 12).r,
        texelFetch(_skinning_text, start_buffer_pos + 13).r,
        texelFetch(_skinning_text, start_buffer_pos + 14).r,
        texelFetch(_skinning_text, start_buffer_pos + 15).r
        );

        vec4 moved_pos = skinning_mat * vec4(position, 1.0f);
        vec3 normalized_pos = moved_pos.xyz / moved_pos.w;
        skinned_pos = normalized_pos * influences[i] + skinned_pos;

        vec4 moved_normals = skinning_mat * vec4(normal, 0.0f);
        skinned_normal = moved_normals.xyz * influences[i] + skinned_normal;

        total_weight += influences[i];
        
        if(total_weight >= 1.0f)
            break;
    }
    
	gl_Position = viewproj *  model * vec4(skinned_pos, 1.0f);
	ourColor = _color;
	TexCoord = texCoord;
	ourTime = _time;
	ourNormal = mat3(model) * normalize(skinned_normal);
}