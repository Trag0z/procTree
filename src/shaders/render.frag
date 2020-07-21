#version 330 core

in vec3 pos;
in vec3 normal;

out vec4 color;

uniform vec3 light_pos;

void main()
{
    vec4 object_color=vec4(.4863,.3647,.1059,1.);
    
    // ambient light
    vec3 ambient=vec3(.2);;
    
    vec3 light_dir=normalize(light_pos-pos);
    
    // float cos_theta=clamp(dot(normal,light_dir),0.,1.);
    
    float diffuse=max(dot(normal,light_dir),0.);
    
    color=vec4(object_color.rgb*(ambient+diffuse),1.);
}