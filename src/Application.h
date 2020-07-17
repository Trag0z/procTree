#pragma once
#include "pch.h"
#include "Shaders.h"
#include "Types.h"

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

    u32 last_frame_start, frame_start;
    u32 frame_delay = 60 / 1000;

    GLuint construction_shader_id;
    GLuint rendering_shader_id;

    GLuint input_uniform;
    GLuint output_uniform;

    GLuint input_vao;
    GLuint input_ebo;
    GLuint input_vbo;

    GLuint feedback_vbo;

    struct {
        GLuint id, ebo_id, vbo_id;
    } render_vao;

    size_t next_vertex = 3;
    Vertex* vertices;
    const size_t max_vertices = 3 + MAX_BRANCHES() * 4;

    size_t next_index = 3;
    GLuint* indices;
    const size_t max_indices = 3 + MAX_BRANCHES() * 3 * 9;

    const size_t max_geometry_iterations = 10;

    struct {
        int x, y;
        int last_x, last_y;
        u32 button_state;
    } mouse;

    struct {
        float rotation = 0.0f;
        glm::vec3 pos = {0.0f, 3.0f, -6.0f};
        glm::vec3 target = {0.0f, 4.0f, 0.0f};
    } camera;

  public:
    bool running = false;

    void init();
    void run();
};