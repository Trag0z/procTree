#pragma once
#include "pch.h"
#include "Vertex.h"

ArrayBuffer::ArrayBuffer(GLenum binding_target, GLenum usage, GLuint size)
    : binding_target_(binding_target), usage_(usage), size_(size) {
    glGenBuffers(1, &id_);
    glBindBuffer(binding_target_, id_);
    glBufferData(binding_target_, size_, nullptr, usage_);
}

GLuint ArrayBuffer::id() const { return id_; }

void ArrayBuffer::write_data(GLuint size, const void* src) const {
    SDL_assert(size <= size_);
    glBindBuffer(binding_target_, id_);
    glBufferSubData(binding_target_, 0, size, src);
}

void ArrayBuffer::read_data(GLuint size, void* dst) const {
    SDL_assert(size <= size_);
    glBindBuffer(binding_target_, id_);
    glGetBufferSubData(binding_target_, 0, size, dst);
}

void ArrayBuffer::set_as_feedback_target() const {
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, id_);
}

VertexArray::VertexArray(ArrayBuffer vertex_buffer, ArrayBuffer index_buffer) {
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    // Vetex array
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id());

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    // length attribute
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, length)));
    glEnableVertexAttribArray(1);

    // Element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.id());

    // Unbind array for error safety
    glBindVertexArray(0);
}

void VertexArray::bind() const { glBindVertexArray(id); }