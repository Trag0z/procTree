#version 330 core
layout(triangles)in;
layout(points,max_vertices=7)out;

in block{
    // vec4 normal;
    float ext_length;
    int vertex_id;
}In[];

// out out_block {
    //     vec4 position;
    //     vec4 normal;
    //     float ext_length;
// } out_vertex;

out vec4 out_position;
out float out_ext_length;

void main(){
    // Return triangle as is
    for(int i=0;i<3;i++){
        out_position=In[i].position
        out_ext_length=In[i].ext_length;
        EmitVertex();
    }
    
    if(In[0].length==0.f){
        EndPrimitive();
    }
    
    // Find the center of the triangle
    vec4 center=gl_in[0].gl_Position+(gl_in[1].gl_Position-gl_in[0].gl_Position)*.5;
    center+=(gl_in[2].gl_Position-center)*.5;
    
    // Create extruded triangle
    vec4 surface_normal=normalize(In[0].normal+In[1].normal+In[2].normal);
    float shrink_factor=.2;
    
    for(int i=0;i<3;++i){
        out_position=gl_in[i].gl_Position+(center-gl_in[i].gl_Position)*.shrink_factor+surface_normal*In[0].ext_length;
        out_ext_length=1.;
        EmitVertex();
    }
    
    float tip_distance=In[0].normal*1.25;
    out_position=center+surface_normal*In[0].ext_length+tip_distance;
    out_ext_length=0.f;
    EmitVertex();
    
    EndPrimitive();
}