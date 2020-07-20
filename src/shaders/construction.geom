#version 330 core
layout(triangles)in;
layout(triangle_strip,max_vertices=30)out;

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
    
    if(ext_length[0]!=0.){
        // Create extruded triangle
        vec4 surface_normal=vec4(normalize(cross(vec3(gl_in[0].gl_Position-gl_in[1].gl_Position),vec3(gl_in[2].gl_Position-gl_in[0].gl_Position))),0.f);
        float shrink_factor=.2;
        
        vec4 new_positions[4];
        vec4 new_normals[4];
        float new_ext_lengths[4];
        
        for(int i=0;i<3;i++){
            new_positions[i]=gl_in[i].gl_Position+(center-gl_in[i].gl_Position)*shrink_factor+surface_normal*ext_length[0];
            new_normals[i]=normalize(gl_in[i].gl_Position-center);
            new_ext_lengths[i]=0.;
        }
        
        float tip_distance=ext_length[0]*1.25;
        
        new_positions[3]=center+surface_normal*tip_distance;
        new_normals[3]=surface_normal;
        new_ext_lengths[3]=ext_length[0]*.8;
        
        out_position=new_positions[2];
        out_normal=new_normals[2];
        out_ext_length=0.;
        EmitVertex();
        
        out_position=gl_in[0].gl_Position;
        out_normal=normalize(gl_in[0].gl_Position-center);
        out_ext_length=0.;
        EmitVertex();
        
        out_position=new_positions[0];
        out_normal=new_normals[0];
        out_ext_length=0.;
        EmitVertex();
        
        out_position=gl_in[1].gl_Position;
        out_normal=normalize(gl_in[1].gl_Position-center);
        out_ext_length=0.;
        EmitVertex();
        
        out_position=new_positions[1];
        out_normal=new_normals[1];
        out_ext_length=0.;
        EmitVertex();
        
        out_position=new_positions[2];
        out_normal=new_normals[2];
        out_ext_length=ext_length[0]*.8;
        EmitVertex();
        
        out_position=new_positions[3];
        out_normal=new_normals[3];
        out_ext_length=ext_length[0]*.8;
        EmitVertex();
        
        out_position=new_positions[0];
        out_normal=new_normals[0];
        out_ext_length=0.;
        EmitVertex();
        
        out_position=new_positions[1];
        out_normal=new_normals[1];
        out_ext_length=ext_length[0]*.8;
        EmitVertex();
    }
    
    EndPrimitive();
}