#pragma once
#include "pch.h"
#include "Shaders.h"
#include "Types.h"
#include "Vertex.h"

struct Vertex;

namespace {
constexpr GLuint MAX_GEOMETRY_ITERATIONS = 10;
constexpr GLuint MAX_BRANCHES() {
    GLuint ret = 1;
    for (GLuint i = 0; i < MAX_GEOMETRY_ITERATIONS - 1; ++i) {
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

    struct {
        GLuint construction, render, line;
    } shaders;

    ArrayBuffer start_vbo, render_vbo, feedback_vbo[2], ebo;
    VertexArray start_vao, render_vao, feedback_vao[2];

    Vertex* vertices;
    Triangle* triangles;
    const GLuint max_vertices = 3 + MAX_BRANCHES() * 4;

    GLuint num_triangles;
    glm::uvec3* triangle_indices;
    const GLuint max_indices = 3 + MAX_BRANCHES() * 3 * 9;

    const GLuint max_geometry_iterations = MAX_GEOMETRY_ITERATIONS; // @CLEANUP

    struct {
        int x, y;
        int last_x, last_y;
        u32 button_state;
    } mouse;

    float object_rotation = 0.0f;
    struct {
        glm::vec3 pos = {0.0f, 7.0f, 6.0f};
        glm::vec3 target = {0.0f, 5.0f, 0.0f};
    } camera;

    glm::vec3 light_position = {4.0f, 11.0f, 5.0f};
    glm::uvec3 debug_triangle_indices = glm::uvec3(0);

    GLuint geometry_iteration = 0;

    bool render_model = true;
    bool render_wireframes = false;
    bool render_debug_triangle = false;

    bool run_geometry_pass = true;

  public:
    bool running = false;

    void init();
    void run();
};