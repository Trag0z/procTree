#version 330 core
layout (triangles) in;
layout (points, max_vertices = 3) out;

in block {
    vec4 normal;
    float ext_length;
} In[];

// out out_block {
//     vec4 position;
//     vec4 normal;
//     float ext_length;
// } out_vertex;

out vec4 out_position;
out vec4 out_normal;
out float out_ext_length;

void main() {
    // Find the center of the triangle
    vec4 center = gl_in[0].gl_Position + (gl_in[1].gl_Position - gl_in[0].gl_Position);
    center += (gl_in[2].gl_Position - center) * 0.5;

    for (int i = 0; i < 3; i++) {
        // Move a bit towards the center and extrude along normal
        out_position = gl_in[i].gl_Position + (center - gl_in[i].gl_Position) * 0.2 + In[i].normal * In[i].ext_length;
        
        out_normal = In[i].normal;
        out_ext_length = 0.0;
        EmitVertex();
    }
    EndPrimitive();
}