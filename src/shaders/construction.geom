#version 330 core
layout(triangles)in;
layout(triangle_strip,max_vertices=30)out;

in vec4 normal[];
in float ext_length[];

out vec4 out_position;
out vec4 out_normal;
out float out_ext_length;

vec4 find_surface_normal(vec4 p1,vec4 p2,vec4 p3){
    return vec4(normalize(cross(vec3(p1-p2),vec3(p1-p3))),0.f);
}

void main(){
    // Find the center of the triangle
    vec4 center=gl_in[0].gl_Position+(gl_in[1].gl_Position-gl_in[0].gl_Position)*.5;
    center+=(gl_in[2].gl_Position-center)*.5;
    
    // Ground triangle
    for(int i=0;i<3;i++){
        out_position=gl_in[i].gl_Position;
        out_normal=vec4(0.);//normal[i];//normalize(gl_in[i].gl_Position-center);
        out_ext_length=0.;
        EmitVertex();
    }
    
    if(ext_length[1]!=0.){
        // Create extruded triangle
        // vec4 surface_normal=vec4(normalize(cross(vec3(gl_in[0].gl_Position-gl_in[2].gl_Position),vec3(gl_in[0].gl_Position-gl_in[1].gl_Position))),0.f);
        float shrink_factor=.2;
        
        vec4 new_positions[4];
        
        for(int i=0;i<3;i++){
            new_positions[i]=gl_in[i].gl_Position+(center-gl_in[i].gl_Position)*shrink_factor+normal[1]*ext_length[1];
            new_positions[i].w=1.;
        }
        
        float tip_distance=ext_length[0]*1.25;
        
        new_positions[3]=center+normal[1]*tip_distance;// surface_normal*tip_distance;
        new_positions[3].w=1.;
        
        out_position=new_positions[2];
        out_normal=vec4(0.);
        out_ext_length=0.;
        EmitVertex();
        
        out_position=gl_in[0].gl_Position;
        out_normal=vec4(0.);
        out_ext_length=0.;
        EmitVertex();
        
        out_position=new_positions[0];
        out_normal=vec4(0.);
        out_ext_length=0.;
        EmitVertex();
        
        out_position=gl_in[1].gl_Position;
        out_normal=vec4(0.);
        out_ext_length=0.;
        EmitVertex();
        
        // Will be 2nd vertex of first triangle to extrude on next run
        out_position=new_positions[1];
        out_normal=find_surface_normal(new_positions[2],new_positions[1],new_positions[3]);
        out_ext_length=ext_length[0]*.8;;
        EmitVertex();
        
        out_position=new_positions[2];
        out_normal=vec4(0.);// find_surface_normal(new_positions[2],new_positions[3],new_positions[0]);
        out_ext_length=0.;//ext_length[0]*.8;
        EmitVertex();
        
        out_position=new_positions[3];
        out_normal=find_surface_normal(new_positions[2],new_positions[3],new_positions[0]);
        out_ext_length=ext_length[0]*.8;
        EmitVertex();
        
        out_position=new_positions[0];
        out_normal=find_surface_normal(new_positions[0],new_positions[1],new_positions[31]);
        out_ext_length=ext_length[0]*.8;
        EmitVertex();
        
        out_position=new_positions[1];
        out_normal=vec4(0.);//find_surface_normal(new_positions[1],new_positions[3],new_positions[2]);
        out_ext_length=0.;//ext_length[0]*.8;
        EmitVertex();
    }
    
    EndPrimitive();
}