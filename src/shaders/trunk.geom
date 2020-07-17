#version 330 core
layout (triangles) in;
layout (points, max_vertices = 3) out;

in in_block {
    vec4 position;
    vec4 normal;
    float ext_length;
} in_vertices[];

// out out_block {
//     vec4 position;
//     vec4 normal;
//     float ext_length;
// } out_vertex;

out vec4 position;
// out vec4 normal;
// out float ext_length;

void main() {
    for (int i = 0; i < 3; i++) {
        position = vec4(1.0, 2.0, 3.0, 4.0); // gl_Position[0] + in_vertices[i].normal * in_vertices[i].ext_length;
        // normal = in_vertices[i].normal;
        // ext_length = 0;
        EmitVertex();
    }
    EndPrimitive();
}