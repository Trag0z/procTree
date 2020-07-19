#version 330 core

in vec4 pos;
in vec4 normal;

out vec4 color;

void main()
{
    vec4 object_color=vec4(1.,0.,0.,1.);
    
    // ambient light
    vec3 ambient=vec3(.15);;
    
    vec4 light_pos=vec4(0.,-5.,4.,1.);
    
    vec4 light_dir=normalize(light_pos-pos);
    
    // float cos_theta=clamp(dot(normal,light_dir),0.,1.);
    
    float diffuse=max(dot(normal,light_dir),0.);
    
    color=vec4(object_color.rgb*(ambient+diffuse),object_color.a);
}