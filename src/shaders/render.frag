#version 330 core

in vec3 pos;
in vec3 normal;

out vec4 color;

uniform vec3 light_pos;
uniform vec3 camera_pos;

void main()
{
    vec4 object_color=vec4(.4863,.3647,.1059,1.);
    
    // phong shading
    
    // ambient light
    float ambient=.2;
    
    // diffuse light
    vec3 light_dir=normalize(light_pos-pos);
    float diffuse=max(dot(normal,light_dir),0.);
    
    // specular light
    float specular_strength=.5;
    vec3 view_dir=normalize(camera_pos-pos);
    vec3 reflect_dir=reflect(-light_dir,normal);
    
    float specular=pow(max(dot(view_dir,reflect_dir),0.),32)*specular_strength;
    
    color=vec4(object_color.rgb*(ambient+diffuse+specular),1.);
}