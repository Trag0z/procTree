#version 330 core
layout(triangles)in;
layout(points,max_vertices=7)out;

in float ext_length[];

out vec4 out_position;
out vec4 out_normal;
out float out_ext_length;

void main(){
    // Find the center of the triangle
    vec4 center=gl_in[0].gl_Position+(gl_in[1].gl_Position-gl_in[0].gl_Position)*.5;
    center+=(gl_in[2].gl_Position-center)*.5;
    
    // Ground triangle
    for(int i=0;i<3;i++){
        out_position=gl_in[i].gl_Position;
        out_normal=normalize(gl_in[i].gl_Position-center);
        out_ext_length=0.;
        EmitVertex();
    }
    
    // Create extruded triangle
    vec4 surface_normal=vec4(normalize(cross(vec3(gl_in[1].gl_Position-gl_in[0].gl_Position),vec3(gl_in[2].gl_Position-gl_in[0].gl_Position))),0.f);
    float shrink_factor=.2;
    
    for(int i=0;i<3;i++){
        out_position=gl_in[i].gl_Position+(center-gl_in[i].gl_Position)*shrink_factor+surface_normal*ext_length[0];
        out_normal=normalize(gl_in[i].gl_Position-center);
        out_ext_length=ext_length[0]*.8;
        EmitVertex();
    }
    
    float tip_distance=ext_length[0]*1.25;
    
    out_position=center+surface_normal*tip_distance;
    out_normal=surface_normal;
    out_ext_length=0.f;
    EmitVertex();
    
    EndPrimitive();
}