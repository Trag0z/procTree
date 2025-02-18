#version 330 core
layout(triangles)in;
layout(points,max_vertices=30)out;

in vec3 normal[];
in float ext_length[];

out vec4 out_position;
out vec3 out_normal;
out float out_ext_length;

void add_triangle(vec4 p1,vec4 p2,vec4 p3,float ext_length){
    vec3 new_normal=normalize(cross(vec3(p1-p2),vec3(p1-p3)));
    
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
    // Ground triangle
    for(int i=0;i<3;i++){
        out_position=gl_in[i].gl_Position;
        out_normal=normal[i];
        out_ext_length=0.;
        EmitVertex();
    }
    
    if(ext_length[0]!=0.){
        // Find the center of the triangle
        vec4 center=gl_in[0].gl_Position+(gl_in[1].gl_Position-gl_in[0].gl_Position)*.5;
        center+=(gl_in[2].gl_Position-center)*.333;
        
        // Create new points
        float shrink_factor=.5;
        
        vec4 new_position[4];
        
        for(int i=0;i<3;i++){
            new_position[i]=gl_in[i].gl_Position+(center-gl_in[i].gl_Position)*shrink_factor+vec4(normal[0]*ext_length[0],0.);
        }
        
        float tip_distance=ext_length[0]*1.2;
        
        new_position[3]=center+vec4(normal[0]*tip_distance,0.);
        
        // Add all the new triangles
        
        // Sides
        add_triangle(gl_in[0].gl_Position,gl_in[1].gl_Position,new_position[0],0.);
        add_triangle(gl_in[1].gl_Position,new_position[1],new_position[0],0.);
        
        add_triangle(gl_in[1].gl_Position,gl_in[2].gl_Position,new_position[1],0.);
        add_triangle(gl_in[2].gl_Position,new_position[2],new_position[1],0.);
        
        add_triangle(gl_in[2].gl_Position,gl_in[0].gl_Position,new_position[2],0.);
        add_triangle(gl_in[0].gl_Position,new_position[0],new_position[2],0.);
        
        // Tip
        float new_ext_length=ext_length[0]*.4;
        add_triangle(new_position[0],new_position[1],new_position[3],new_ext_length);
        add_triangle(new_position[1],new_position[2],new_position[3],new_ext_length);
        add_triangle(new_position[2],new_position[0],new_position[3],new_ext_length);
    }
    EndPrimitive();
}