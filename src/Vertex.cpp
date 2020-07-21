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
    SDL_assert(usage_ == GL_STATIC_DRAW || usage_ == GL_DYNAMIC_DRAW ||
               usage_ == GL_STREAM_DRAW);
    glBindBuffer(binding_target_, id_);
    glBufferSubData(binding_target_, 0, size, src);
}

void ArrayBuffer::read_data(GLuint size, void* dst) const {
    SDL_assert(size <= size_);
    SDL_assert(usage_ == GL_STATIC_READ || usage_ == GL_DYNAMIC_READ ||
               usage_ == GL_STREAM_READ);
    glBindBuffer(binding_target_, id_);
    glGetBufferSubData(binding_target_, 0, size, dst);
}

void ArrayBuffer::bind_as_feedback_target() const {
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, id_);
    SDL_assert(usage_ == GL_STATIC_READ || usage_ == GL_DYNAMIC_READ ||
               usage_ == GL_STREAM_READ || usage_ == GL_STATIC_COPY ||
               usage_ == GL_DYNAMIC_COPY || usage_ == GL_STREAM_COPY);
}

void ArrayBuffer::bind_as_copy_source() const {
    glBindBuffer(GL_COPY_READ_BUFFER, id_);
}

void ArrayBuffer::bind_as_copy_target() const {
    glBindBuffer(GL_COPY_WRITE_BUFFER, id_);
}

VertexArray::VertexArray(ArrayBuffer vbo) {
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    // Vetex array
    glBindBuffer(GL_ARRAY_BUFFER, vbo.id());

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    // length attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, length)));
    glEnableVertexAttribArray(2);

    // Unbind array for error safety
    glBindVertexArray(0);
}

void VertexArray::bind() const { glBindVertexArray(id); }