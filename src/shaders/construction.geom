#version 330 core
layout(triangles)in;
layout(points,max_vertices=30)out;

in vec4 normal[];
in float ext_length[];

out vec4 out_position;
out vec4 out_normal;
out float out_ext_length;

vec4 find_surface_normal(vec4 p1,vec4 p2,vec4 p3){
    return vec4(normalize(cross(vec3(p1-p2),vec3(p1-p3))),0.f);
}

void add_triangle(vec4 p1,vec4 p2,vec4 p3,float ext_length){
    vec4 new_normal=vec4(0.);
    
    if(ext_length>0.){
        new_normal=find_surface_normal(p1,p2,p3);
    }
    
    out_position=p1;
    out_normal=new_normal;
    out_ext_length=ext_length;
    EmitVertex();
    
    out_position=p2;
    out_normal=new_normal;
    out_ext_length=ext_length;
    EmitVertex();
    
    out_position=p3;
    out_normal=new_normal;
    out_ext_length=ext_length;
    EmitVertex();
}

void main(){
    // Find the center of the triangle
    vec4 center=gl_in[0].gl_Position+(gl_in[1].gl_Position-gl_in[0].gl_Position)*.5;
    center+=(gl_in[2].gl_Position-center)*.5;
    
    // Ground triangle
    for(int i=0;i<3;i++){
        out_position=gl_in[i].gl_Position;
        out_normal=vec4(0.);
        out_ext_length=0.;
        EmitVertex();
    }
    
    if(ext_length[0]!=0.){
        
        // Create new points
        float shrink_factor=.2;
        
        vec4 new_position[4];
        
        for(int i=0;i<3;i++){
            new_position[i]=gl_in[i].gl_Position+(center-gl_in[i].gl_Position)*shrink_factor+normal[0]*ext_length[0];
            new_position[i].w=1.;
        }
        
        float tip_distance=ext_length[0]*1.25;
        
        new_position[3]=center+normal[0]*tip_distance;
        new_position[3].w=1.;
        
        // Add all the new triangles
        
        // Sides
        add_triangle(gl_in[0].gl_Position,new_position[0],gl_in[1].gl_Position,0.);
        add_triangle(gl_in[1].gl_Position,new_position[0],new_position[1],0.);
        
        add_triangle(gl_in[1].gl_Position,new_position[1],gl_in[2].gl_Position,0.);
        add_triangle(gl_in[2].gl_Position,new_position[1],new_position[2],0.);
        
        add_triangle(gl_in[2].gl_Position,new_position[2],gl_in[0].gl_Position,0.);
        add_triangle(gl_in[0].gl_Position,new_position[2],new_position[0],0.);
        
        // Tip
        float new_ext_length=ext_length[0]*.8;
        add_triangle(new_position[0],new_position[3],new_position[1],new_ext_length);
        add_triangle(new_position[1],new_position[3],new_position[2],new_ext_length);
        add_triangle(new_position[2],new_position[3],new_position[0],new_ext_length);
    }
    EndPrimitive();
    
}