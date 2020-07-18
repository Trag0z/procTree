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

    window = SDL_CreateWindow("procTree", 3840, 956, 1920, 1200,
                              SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS |
                                  SDL_WINDOW_OPENGL);
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

    // tip_construction_shader_id = load_and_compile_shader_from_file(
    //     "../src/shaders/construction.vert", "../src/shaders/tip.geom",
    //     nullptr);

    rendering_shader_id = load_and_compile_shader_from_file(
        "../src/shaders/render.vert", nullptr, "../src/shaders/render.frag");

    //      Setup input vertex buffer        //
    glGenVertexArrays(1, &input_buffer.vao);
    glBindVertexArray(input_buffer.vao);

    // EBO
    glGenBuffers(1, &input_buffer.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, input_buffer.ebo);

    indices = new GLuint[max_indices];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3, indices,
                 GL_STATIC_DRAW);

    // VBO
    glGenBuffers(1, &input_buffer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, input_buffer.vbo);

    // position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

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

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 3, vertices);

    //      Setup feedback buffers         //
    for (size_t i = 0; i < 2; ++i) {
        glGenVertexArrays(1, &feedback_buffer[i].vao);
        glBindVertexArray(feedback_buffer[i].vao);

        // VBO
        glGenBuffers(1, &feedback_buffer[i].vbo);
        glBindBuffer(GL_ARRAY_BUFFER, feedback_buffer[i].vbo);

        // position attribute
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                              reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // length attribute
        glVertexAttribPointer(
            2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, length)));
        glEnableVertexAttribArray(2);

        // Allocate
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * max_vertices, nullptr,
                     GL_STREAM_COPY);

        // EBO
        glGenBuffers(1, &render_buffer.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_buffer.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * max_indices,
                     nullptr, GL_STREAM_COPY);
    }

    glBindVertexArray(0);

    running = true;
}

void Application::run() {
    last_frame_start = frame_start;
    frame_start = SDL_GetTicks();

    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT ||
            (event.type == SDL_KEYDOWN &&
             event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) ||
            (event.type == SDL_WINDOWEVENT &&
             event.window.event == SDL_WINDOWEVENT_CLOSE &&
             event.window.windowID == SDL_GetWindowID(window))) {
            running = false;
        }
    }

    // Update mouse
    mouse.last_x = mouse.x;
    mouse.last_y = mouse.y;
    mouse.button_state = SDL_GetMouseState(&mouse.x, &mouse.y);

    //          First Geometry pass          //
    glUseProgram(construction_shader_id);

    glBindVertexArray(input_buffer.vao);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedback_buffer[0].vbo);

    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glEndTransformFeedback();

    glFlush();

    glBindVertexArray(feedback_buffer[0].vao);
    create_tree_indices(1);

    // Render
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update camera
    if (mouse.button_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        const float sensitivity = 0.1f;
        camera.rotation += (mouse.x - mouse.last_x) * sensitivity;
    }

    glm::mat4 projection =
        glm::perspective(glm::radians(100.0f), 1920.0f / 1200.0f, -3.0f, 3.0f) *
        glm::rotate(glm::lookAt(camera.pos, camera.target, {0.0f, 1.0f, 0.0f}),
                    camera.rotation, {0.0f, 1.0f, 0.0f});

    glUseProgram(rendering_shader_id);
    GLuint uniform_id = glGetUniformLocation(rendering_shader_id, "projection");
    glUniformMatrix4fv(uniform_id, 1, 0, value_ptr(projection));

    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(7), GL_UNSIGNED_INT,
                   0);

    SDL_GL_SwapWindow(window);

    // Wait for next frame
    u32 last_frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > last_frame_time)
        SDL_Delay(frame_delay - last_frame_time);
}

void Application::create_tree_indices(GLuint num_branches) {
    const GLuint start_indices[] = {0, 1, 2, 5, 0, 3, 1, 4, 5, 6, 3, 4};
    const GLuint indices_per_branch = sizeof(start_indices) / sizeof(GLfloat);

    GLuint num_written = 0;
    for (GLuint current_branches = 0; current_branches < num_branches;
         ++current_branches) {
        for (GLuint i = 0; i < indices_per_branch; ++i) {
            indices[num_written++] =
                start_indices[i] + indices_per_branch * current_branches;
        }
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLfloat) * num_written,
                    indices);
}