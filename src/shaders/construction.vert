#version 330 core
layout(location=0)in vec4 in_position;
layout(location=1)in float in_length;

out float ext_length;

void main()
{
    gl_Position=in_position;
    ext_length=in_length;
}