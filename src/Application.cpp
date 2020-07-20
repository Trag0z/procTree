#pragma once
#include "pch.h"
#include "Application.h"
#include "DebugCallback.h"

static GLuint uint_pow(GLuint base, GLuint exp) {
    GLuint result = 1;
    while (exp) {
        if (exp % 2) {
            result *= base;
        }
        exp /= 2;
        base *= base;
    }
    return result;
}

static GLuint create_tree_indices(GLuint geometry_iteration,
                                  glm::uvec3* triangles) {
    // const glm::uvec3 start_indices[] = {{0, 1, 2}, {0, 3, 1}, {1, 3, 4},
    //                                     {1, 4, 2}, {2, 4, 5}, {2, 5, 0},
    //                                     {0, 5, 3}};

    // const glm::uvec3 tip_indices[] = {{6, 4, 3}, {6, 5, 4}, {6, 3, 5}};

    // const GLuint triangles_per_branch =
    //     sizeof(start_indices) / sizeof(glm::uvec3);

    GLuint num_branches = 0;
    for (GLuint i = 0; i < geometry_iteration; ++i) {
        num_branches += uint_pow(3, i);
    }

    // GLuint num_triangles = 0;
    // // GLuint num_tips = uint_pow(3, geometry_iteration);

    // GLuint branches_to_leaf = geometry_iteration;
    // GLuint branch_depth = 1;
    // GLuint leaves_added = 0;

    // for (GLuint current_branches = 0; current_branches < num_branches;
    //      ++current_branches) {

    //     for (GLuint i = 0; i < triangles_per_branch; ++i) {
    //         triangles[num_triangles++] =
    //             start_indices[i] + triangles_per_branch * current_branches;
    //     }

    //     if (branch_depth == branches_to_leaf) {
    //         // Add tip
    //         for (auto vec : tip_indices) {
    //             triangles[num_triangles++] =
    //                 vec + triangles_per_branch * current_branches;
    //         }
    //         ++leaves_added;

    //         if (leaves_added % 3) {
    //             leaves_added = 0;
    //             --branch_depth;
    //         }
    //     } else {
    //         ++branch_depth;
    //     }
    // }

    GLuint num_triangles = num_branches * 10;
    for (GLuint i = 0; i < num_triangles; ++i) {
        triangles[i] = {i * 3, i * 3 + 1, i * 3 + 2};
    }

    return num_triangles;
}

void Application::init() {
#ifndef NDEBUG
    printf("DEBUG MODE");
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
    const char* glsl_version = "#version 330 core";
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
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

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
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //          Create shaders              //
    shaders.construction = load_and_compile_shader_from_file(
        "../src/shaders/construction.vert", "../src/shaders/construction.geom",
        nullptr);

    shaders.line = load_and_compile_shader_from_file(
        "../src/shaders/line.vert", nullptr, "../src/shaders/line.frag");

    shaders.render = load_and_compile_shader_from_file(
        "../src/shaders/render.vert", nullptr, "../src/shaders/render.frag");

    //          Setup buffers               //
    start_vbo =
        ArrayBuffer(GL_ARRAY_BUFFER, GL_STREAM_DRAW, sizeof(Vertex) * 3);

    render_vbo = ArrayBuffer(GL_ARRAY_BUFFER, GL_STREAM_READ,
                             sizeof(Vertex) * max_vertices);

    feedback_vbo[0] = ArrayBuffer(GL_ARRAY_BUFFER, GL_STREAM_COPY,
                                  sizeof(Vertex) * max_vertices);
    feedback_vbo[1] = ArrayBuffer(GL_ARRAY_BUFFER, GL_STREAM_COPY,
                                  sizeof(Vertex) * max_vertices);

    ebo = ArrayBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STREAM_DRAW,
                      sizeof(Vertex) * max_indices);

    start_vao = VertexArray(start_vbo, ebo);
    render_vao = VertexArray(render_vbo, ebo);
    feedback_vao[0] = VertexArray(feedback_vbo[0], ebo);
    feedback_vao[1] = VertexArray(feedback_vbo[1], ebo);

    //          Initial data                //
    vertices = new Vertex[max_vertices];

    // vertices[0].position = {0.0f, 0.0f, 0.0f, 1.0f};
    // vertices[1].position = {1.0f, 1.0f, 1.0f, 1.0f};
    // vertices[2].position = {2.0f, 2.0f, 2.0f, 1.0f};

    vertices[0].position = {-1.0f, 0.0f, -1.0f, 1.0f};
    vertices[1].position = {1.0f, 0.0f, -1.0f, 1.0f};
    vertices[2].position = {0.0f, 0.0f, 1.0f, 1.0f};

    vertices[0].normal = {0.0f, 1.0f, 0.0f, 1.0f};
    vertices[1].normal = {0.0f, 1.0f, 0.0f, 1.0f};
    vertices[2].normal = {0.0f, 1.0f, 0.0f, 1.0f};

    vertices[0].length = 5.0f;
    vertices[1].length = 5.0f;
    vertices[2].length = 5.0f;

    triangle_indices = new glm::uvec3[max_indices / 3];
    triangle_indices[0] = {0, 1, 2};

    start_vbo.write_data(sizeof(Vertex) * 3, vertices);
    ebo.write_data(sizeof(GLuint) * 3, triangle_indices);

    //          First Geometry pass          //
    glUseProgram(shaders.construction);

    start_vao.bind();
    feedback_vbo[0].set_as_feedback_target();

    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glEndTransformFeedback();

    // So that read_Data() and write_data() don't change the vao
    glBindVertexArray(0);

    glFlush();

    run_geometry_pass = false;

    // Copy data to render buffer
    feedback_vbo[0].bind_as_copy_source();
    render_vbo.bind_as_copy_target();

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                        sizeof(Vertex) * max_vertices);
    render_vbo.read_data(sizeof(Vertex) * max_vertices, vertices);

    num_triangles = create_tree_indices(++geometry_iteration, triangle_indices);
    ebo.write_data(sizeof(glm::uvec3) * num_triangles, triangle_indices);

    running = true;
}

void Application::run() {
    last_frame_start = frame_start;
    frame_start = SDL_GetTicks();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
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

    { // Update gui
        using namespace ImGui;

        Begin("Debug control", NULL, ImGuiWindowFlags_NoTitleBar);
        run_geometry_pass = Button("Run geometry pass");

        Checkbox("Render model", &render_model);
        Checkbox("Render wireframes", &render_wireframes);
        Checkbox("Render debug triangle", &render_debug_triangle);

        NewLine();
        Text("Debug triangle indices");
        InputScalarN("Indices", ImGuiDataType_U32, &debug_triangle_indices, 3);

        NewLine();
        DragFloat3("Light position", value_ptr(light_position), 0.2f);

        End();
    }

    if (run_geometry_pass && ++geometry_iteration < max_geometry_iterations) {
        const GLuint read_buffer_index = geometry_iteration % 2;
        const GLuint write_buffer_index = read_buffer_index ^ 1;

        glUseProgram(shaders.construction);

        feedback_vao[read_buffer_index].bind();
        feedback_vbo[write_buffer_index].set_as_feedback_target();

        glBeginTransformFeedback(GL_TRIANGLES);
        glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0);
        glEndTransformFeedback();

        // So that read_Data() and write_data() don't change the vao
        glBindVertexArray(0);

        glFlush();

        // Copy data to render buffer
        feedback_vbo[write_buffer_index].bind_as_copy_source();
        render_vbo.bind_as_copy_target();

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                            sizeof(Vertex) * max_vertices);
        render_vbo.read_data(sizeof(Vertex) * max_vertices, vertices);

        num_triangles =
            create_tree_indices(geometry_iteration, triangle_indices);
        ebo.write_data(sizeof(glm::uvec3) * num_triangles, triangle_indices);
    }

    // Render
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update camera
    if (mouse.button_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        const float sensitivity = 0.03f;
        object_rotation += (mouse.x - mouse.last_x) * sensitivity;
    }

    ebo.write_data(sizeof(glm::uvec3) * num_triangles, triangle_indices);

    glm::mat4 projection =
        glm::perspective(glm::radians(100.0f), 1920.0f / 1200.0f, 0.1f, 100.0f);

    const glm::mat4 view =
        glm::lookAt(camera.pos, camera.target, {0.0f, 1.0f, 0.0f});

    glm::mat4 model =
        glm::rotate(glm::mat4(1.0f), object_rotation, {0.0f, 1.0f, 0.0f});

    if (render_model) {
        glUseProgram(shaders.render);

        GLuint projection_id =
            glGetUniformLocation(shaders.render, "projection");
        glUniformMatrix4fv(projection_id, 1, 0, value_ptr(projection));

        GLuint view_id = glGetUniformLocation(shaders.render, "view");
        glUniformMatrix4fv(view_id, 1, 0, value_ptr(view));

        GLuint model_id = glGetUniformLocation(shaders.render, "model");
        glUniformMatrix4fv(model_id, 1, 0, value_ptr(model));

        GLuint light_pos_id = glGetUniformLocation(shaders.render, "light_pos");
        glUniform3fv(light_pos_id, 1, value_ptr(light_position));

        render_vao.bind();
        glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0);
    }

    // Debug rendering
    glUseProgram(shaders.line);

    GLuint projection_id = glGetUniformLocation(shaders.line, "projection");
    glUniformMatrix4fv(projection_id, 1, 0, value_ptr(projection));

    GLuint view_id = glGetUniformLocation(shaders.line, "view");
    glUniformMatrix4fv(view_id, 1, 0, value_ptr(view));

    GLuint model_id = glGetUniformLocation(shaders.line, "model");
    glUniformMatrix4fv(model_id, 1, 0, value_ptr(model));

    if (render_wireframes) {
        glDrawElements(GL_LINE_LOOP, num_triangles * 3, GL_UNSIGNED_INT, 0);
    }

    if (render_debug_triangle) {
        ebo.write_data(sizeof(debug_triangle_indices), &debug_triangle_indices);
        glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);

    // Wait for next frame
    u32 last_frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > last_frame_time)
        SDL_Delay(frame_delay - last_frame_time);
}
