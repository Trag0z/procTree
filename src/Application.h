#pragma once
#include "pch.h"
#include "Shaders.h"
#include "Types.h"
#include "Vertex.h"

class Application {
    SDL_Window* window;
    SDL_Renderer* sdl_renderer;
    SDL_GLContext gl_context;

    u32 last_frame_start, frame_start;
    u32 frame_delay = 60 / 1000;

    struct {
        int x, y;
        int last_x, last_y;
        u32 button_state;
    } mouse;

    struct {
        GLuint construction, render, line;
    } shaders;

    // GPU buffers
    VertexArray start_vao, feedback_vao[2];
    ArrayBuffer start_vbo, feedback_vbo[2];
    GLuint read_buffer_index, write_buffer_index;

    GLuint geometry_iteration = 0;
    const GLuint MAX_GEOMETRY_ITERATIONS = 10;

    GLuint num_triangles;

    float object_rotation = 0.0f;
    struct {
        glm::vec3 pos = {0.0f, 4.5f, 5.0f};
        glm::vec3 target = {0.0f, 4.0f, 0.0f};
    } camera;

    glm::vec3 light_position = {4.0f, 11.0f, 5.0f};

    bool render_model = true;
    bool render_wireframes = false;

    bool run_geometry_pass = true;

  public:
    bool is_running = false;

    void init();
    void run();
};