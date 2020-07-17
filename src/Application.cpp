#pragma once
#include "pch.h"
#include "Application.h"
#include "DebugCallback.h"

void Application::init() {
#ifndef NDEBUG
    printf("Running in debug mode.");
#endif

    //          Initialize SDL              //
    SDL_assert_always(SDL_Init(SDL_INIT_EVERYTHING) == 0);
    // SDL_assert_always(IMG_Init(IMG_INIT_PNG) != 0);

    window =
        SDL_CreateWindow("procTree", 3840, 956, 0, 0,
                         SDL_WINDOW_ALLOW_HIGHDPI |
                             SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    SDL_assert_always(window);

    sdl_renderer = SDL_CreateRenderer(window, -1, 0);

    // Use OpenGL 3.3 core
    // const char* glsl_version = "#version 330 core";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    //          Create openGL context       //
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        printf("OpenGL context could not be created! SDL Error: %s\n",
               SDL_GetError());
        SDL_assert(false);
    }

    //          Initialize GLEW             //
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
    }

    //          OpenGL configuration        //
    if (SDL_GL_SetSwapInterval(1) < 0) {
        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    glm::ivec2 window_size;
    SDL_GetWindowSize(window, &window_size.x, &window_size.y);

    glViewport(0, 0, window_size.x, window_size.y);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(handle_gl_debug_output, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
#endif

    //          Setup ImGui context         //
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGui::StyleColorsDark();

    // ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    // ImGui_ImplOpenGL3_Init(glsl_version);

    //          Create shaders              //
    construction_shader_id =
        load_and_compile_shader_from_file("../src/shaders/construction.vert",
                                          "../src/shaders/trunk.geom", nullptr);

    rendering_shader_id = load_and_compile_shader_from_file(
        "../src/shaders/render.vert", nullptr, "../src/shaders/render.frag");

    //      Setup input vertex array        //
    glGenVertexArrays(1, &input_vao);
    glBindVertexArray(input_vao);

    // EBO
    glGenBuffers(1, &input_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, input_ebo);

    indices = new GLuint[max_indices];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3, indices,
                 GL_STREAM_DRAW);

    // VBO
    glGenBuffers(1, &input_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, input_vbo);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    // length attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, length)));
    glEnableVertexAttribArray(2);

    vertices = new Vertex[max_vertices];
    vertices[0].position = {-1.0f, 0.0f, 1.0f, 1.0f};
    vertices[1].position = {1.0f, 0.0f, 1.0f, 1.0f};
    vertices[2].position = {0.0f, 0.0f, -1.0f, 1.0f};

    vertices[0].normal = {0.0f, 1.0f, 0.0f, 0.0f};
    vertices[1].normal = {0.0f, 1.0f, 0.0f, 0.0f};
    vertices[2].normal = {0.0f, 1.0f, 0.0f, 0.0f};

    vertices[0].length = 5.0f;
    vertices[1].length = 5.0f;
    vertices[2].length = 5.0f;

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 3, vertices);

    glBindVertexArray(0);

    //      Setup feedback vertex buffer    //
    glGenBuffers(1, &feedback_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, feedback_vbo);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    // length attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, length)));
    glEnableVertexAttribArray(2);

    // Allocate
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3, nullptr, GL_STREAM_READ);
    // glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedback_vbo);

    //      Setup render vertex array       //
    glGenVertexArrays(1, &render_vao.id);
    glBindVertexArray(render_vao.id);

    // VBO
    glGenBuffers(1, &render_vao.vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, render_vao.vbo_id);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    // length attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, length)));
    glEnableVertexAttribArray(2);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * max_vertices, nullptr,
                 GL_STREAM_DRAW);

    // EBO
    glGenBuffers(1, &render_vao.ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_vao.ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * max_indices, nullptr,
                 GL_STREAM_DRAW);

    glBindVertexArray(0);

    running = true;
}

void Application::run() {
    // Geometry passes
    glUseProgram(construction_shader_id);
    glBindVertexArray(input_vao);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedback_vbo);

    glBeginTransformFeedback(GL_POINTS);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glEndTransformFeedback();

    glFlush();
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(Vertex) * 3,
                       &vertices[next_vertex]);
    next_vertex += 3;

    SDL_assert(indices[next_index - 3] + 1 == indices[next_index - 2] &&
               indices[next_index - 3] + 2 == indices[next_index - 1]);

    GLuint last_triangle_start = indices[next_index - 3];
    indices[next_index++] = last_triangle_start + 0;
    indices[next_index++] = last_triangle_start + 3;
    indices[next_index++] = last_triangle_start + 1;

    indices[next_index++] = last_triangle_start + 1;
    indices[next_index++] = last_triangle_start + 3;
    indices[next_index++] = last_triangle_start + 4;

    indices[next_index++] = last_triangle_start + 1;
    indices[next_index++] = last_triangle_start + 4;
    indices[next_index++] = last_triangle_start + 2;

    indices[next_index++] = last_triangle_start + 2;
    indices[next_index++] = last_triangle_start + 4;
    indices[next_index++] = last_triangle_start + 5;

    indices[next_index++] = last_triangle_start + 2;
    indices[next_index++] = last_triangle_start + 5;
    indices[next_index++] = last_triangle_start + 0;

    indices[next_index++] = last_triangle_start + 0;
    indices[next_index++] = last_triangle_start + 5;
    indices[next_index++] = last_triangle_start + 3;

    indices[next_index++] = last_triangle_start + 3;
    indices[next_index++] = last_triangle_start + 4;
    indices[next_index++] = last_triangle_start + 5;

    // Render
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rendering_shader_id);

    // Update camera
    glm::vec3 camera_pos = {0.0f, 3.0f, -6.0f};
    glm::vec3 camera_target = {0.0f, 4.0f, 0.0f};
    glm::mat4 projection =
        glm::perspective(glm::radians(100.0f), 1920.0f / 1200.0f, -3.0f, 3.0f) * glm::lookAt(camera_pos, camera_target, { 0.0f, 1.0f, 0.0f });

    GLuint uniform_id = glGetUniformLocation(rendering_shader_id, "projection");
    glUniformMatrix4fv(uniform_id, 1, 0, value_ptr(projection));

    // Update VAO data
    glBindVertexArray(render_vao.id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_vao.ebo_id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLfloat) * (next_index),
                    indices);

    glBindBuffer(GL_ARRAY_BUFFER, render_vao.vbo_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * (next_vertex),
                    vertices);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(next_index),
                   GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);

    SDL_TriggerBreakpoint();
}