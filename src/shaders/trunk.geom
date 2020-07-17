#version 330 core
layout (triangles) in;
layout (points, max_vertices = 3) out;

// in in_block {
   in vec4 position[];
    in vec4 normal[];
    in float ext_length[];
// } in_vertices[];

// out out_block {
//     vec4 position;
//     vec4 normal;
//     float ext_length;
// } out_vertex;

out vec4 out_position;
out vec4 out_normal;
out float out_ext_length;

void main() {
    for (int i = 0; i < 3; i++) {
        out_position = position[i]; //gl_in[i].gl_Position; // + normal * ext_length;
        out_normal = normal[i];
        out_ext_length = ext_length[i];
        EmitVertex();
    }
    EndPrimitive();
}