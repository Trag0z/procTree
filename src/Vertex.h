#pragma once
#include "pch.h"

struct Vertex {
    glm::vec4 position;
    glm::vec3 normal;
    float length;
};

class ArrayBuffer {
    GLuint id_;
    GLenum binding_target_;
    GLenum usage_;
    GLuint size_;

  public:
    ArrayBuffer() {}
    ArrayBuffer(GLenum binding_target, GLenum usage, GLuint size);
    GLuint id() const;

    void write_data(GLuint size, const void* src) const;

    void bind_as_feedback_target() const;
};

class VertexArray {
    GLuint id;

  public:
    VertexArray() {}
    VertexArray(ArrayBuffer vbo);

    void bind() const;
};