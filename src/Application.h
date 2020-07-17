#pragma once
#include "pch.h"
#include "Shaders.h"

struct Vertex;

namespace {
constexpr size_t MAX_GEOMETRY_ITERATIONS = 10;
constexpr size_t MAX_BRANCHES() {
    size_t ret = 1;
    for (size_t i = 0; i < MAX_GEOMETRY_ITERATIONS - 1; ++i) {
        ret *= 3;
    }
    return ret + 1;
};
} // namespace

class Application {
    SDL_Window* window;
    SDL_Renderer* sdl_renderer;
    SDL_GLContext gl_context;

    GLuint construction_shader_id;
    GLuint rendering_shader_id;

    GLuint input_uniform;
    GLuint output_uniform;

    GLuint input_vao;
    GLuint input_ebo;
    GLuint input_vbo;

    GLuint feedback_vbo;

    size_t next_vertex = 3;
    Vertex* vertices;
    const size_t max_vertices = 3 + MAX_BRANCHES() * 4;
    // std::array<Vertex, 3 + MAX_BRANCHES() * 4> vertices;

    size_t next_index = 3;
    GLuint* indices;
    const size_t max_indices = 3 + MAX_BRANCHES() * 3 * 9;
    // std::array<GLfloat, 3 + MAX_BRANCHES() * 3 * 9> indices;

    const size_t max_geometry_iterations = 10;

  public:
    bool running = false;

    void init();
    void run();
};