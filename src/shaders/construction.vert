#version 330 core
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec4 in_normal;
layout (location = 2) in float in_length;

out block {
   out vec4 normal;
   out float ext_length;
} Out;


void main()
{
    gl_Position = in_position;
    Out.normal = in_normal;
    Out.ext_length = in_length;
}