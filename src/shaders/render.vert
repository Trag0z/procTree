#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 in_normal;
layout (location = 2) in float in_length;

uniform mat4 projection;

out vec4 normal;

void main()
{
    gl_Position = projection * position;
    normal = in_normal;
}