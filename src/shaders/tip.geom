#version 330 core
layout (triangles) in;
layout (points, max_vertices = 3) out;

in block {
    vec4 position;
    vec4 normal;
    float ext_length;
} in_vertices[];

out feedback {
    vec4 position;
    vec4 normal;
    float ext_length;
} out_feedback;

void main() {
    vec4 center = in_vertices[0] + (in_vertices[1] - in_vertices[0]) * 0.5f;
    center += (in_vertices[2] - center) * 0.5f;
    out_feedback.position = center + in_vertices[0].normal * in_vertices[0].ext_length;
    for (int i = 0; i < 3; ++i) {
        out_feedback.normal = normalize(cross())

    }
    
}