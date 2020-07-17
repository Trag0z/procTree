#version 330 core
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec4 in_normal;
layout (location = 2) in float in_length;

// out block {
   out vec4 position;
   out vec4 normal;
   out float ext_length;
// } out_vertices;


void main()
{
    gl_Position = in_position;
    position = in_position;
    normal = in_normal;
    ext_length = in_length;
}