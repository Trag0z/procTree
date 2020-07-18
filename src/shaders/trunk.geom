#version 330 core
layout(triangles)in;
layout(points,max_vertices=4)out;

in float ext_length[];

out vec4 out_position;
out float out_ext_length;

void main(){
    // Return triangle as is
    for(int i=0;i<3;i++){
        out_position=gl_in[i].gl_Position;
        out_ext_length=ext_length[i];
        EmitVertex();
    }
    
    // if(In[0].ext_length==0.f){
        //     EndPrimitive();
    // }
    
    // for(int i=0;i<3;i++){
        //     out_position=gl_in[i].gl_Position;
        //     out_ext_length=In[i].ext_length;
        //     EmitVertex();
    // }
    
    // // Find the center of the triangle
    // vec4 center=gl_in[0].gl_Position+(gl_in[1].gl_Position-gl_in[0].gl_Position);
    // center+=(gl_in[2].gl_Position-center)*.5;
    
    // // Create extruded triangle
    // vec4 surface_normal=vec4(normalize(cross(vec3(gl_in[1].gl_Position-gl_in[0].gl_Position),vec3(gl_in[2].gl_Position-gl_in[0].gl_Position))),0.f);
    // float shrink_factor=.2;
    
    // for(int i=0;i<3;++i){
        //     out_position=gl_in[i].gl_Position+(center-gl_in[i].gl_Position)*shrink_factor+surface_normal*In[0].ext_length;
        //     out_ext_length=1.;
        //     EmitVertex();
    // }
    
    // float tip_distance=length(surface_normal)*1.25;
    
    // out_position=center+surface_normal*In[0].ext_length+tip_distance;
    // out_ext_length=0.f;
    // EmitVertex();
    
    EndPrimitive();
}