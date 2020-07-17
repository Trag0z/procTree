#version 330 core
layout (triangles) in;
layout (points, max_vertices = 4) out;

in block {
    vec4 normal;
    float ext_length;
} In[];

out vec4 out_position;
out vec4 out_normal;
out float out_ext_length;

void main() {
    vec4 surface_normal = vec4(normalize(cross(vec3(gl_in[1].gl_Position - gl_in[0].gl_Position), vec3(gl_in[2].gl_Position - gl_in[0].gl_Position))), 0.0);

    for (int i = 0; i < 3; ++i) {
        out_position = gl_in[i].gl_Position;
        out_normal = surface_normal;
        out_ext_length = In[i].ext_length * 0.3;
        EmitVertex();
    }

    // Find the center of the triangle
    vec4 center = gl_in[0].gl_Position + (gl_in[1].gl_Position - gl_in[0].gl_Position);
    center += (gl_in[2].gl_Position - center) * 0.5;

    out_position = center + In[0].normal * In[0].ext_length;
    out_normal = surface_normal;
    out_ext_length = In[0].ext_length * 0.4;
    EmitVertex();

    EndPrimitive();
}