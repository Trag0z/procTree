#version 330 core
layout(location=0)in vec4 in_pos;
layout(location=1)in vec4 in_normal;
layout(location=2)in float in_length;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position=projection*view*model*in_pos;
}