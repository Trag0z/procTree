#version 330 core
layout(location=0)in vec4 in_pos;
layout(location=1)in vec4 in_normal;
layout(location=2)in float in_length;

out float ext_length;

void main()
{
    gl_Position=in_pos;
    ext_length=in_length;
}