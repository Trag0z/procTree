#version 330 core

in vec4 pos;
in vec4 normal;

out vec4 color;

void main()
{
    vec4 default_color=vec4(1.,0.,0.,1.);
    vec4 light_pos=vec4(10.,10.,-10.,1.);
    
    vec4 light_dir=normalize(light_pos-pos);
    
    // float cos_theta=clamp(dot(normal,light_dir),0.,1.);
    
    float diff=max(dot(normal,light_dir),0.);
    
    color=vec4(default_color.rgb*diff,1.);
}