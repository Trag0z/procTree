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
    shaders.construction =
        load_and_compile_shader_from_file("../src/shaders/construction.vert",
                                          "../src/shaders/trunk.geom", nullptr);

    shaders.line = load_and_compile_shader_from_file(
        "../src/shaders/line.vert", nullptr, "../src/shaders/line.frag");

    shaders.render = load_and_compile_shader_from_file(
        "../src/shaders/render.vert", nullptr, "../src/shaders/render.frag");

    //          Setup buffers               //
    render_vbo = ArrayBuffer(GL_ARRAY_BUFFER, GL_STREAM_DRAW,
                             sizeof(Vertex) * max_vertices);
    feedback_vbo = ArrayBuffer(GL_ARRAY_BUFFER, GL_STREAM_READ,
                               sizeof(Vertex) * max_vertices);
    ebo = ArrayBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STREAM_DRAW,
                      sizeof(Vertex) * max_indices);

    render_vao = VertexArray(render_vbo, ebo);
    feedback_vao = VertexArray(feedback_vbo, ebo);

    //          Initial data                //
    vertices = new Vertex[max_vertices];
    vertices[0].position = {-1.0f, 0.0f, 1.0f, 1.0f};
    vertices[1].position = {1.0f, 0.0f, 1.0f, 1.0f};
    vertices[2].position = {0.0f, 0.0f, -1.0f, 1.0f};

    vertices[0].normal = {0.0f, 1.0f, 0.0f, 1.0f};
    vertices[1].normal = {0.0f, 1.0f, 0.0f, 1.0f};
    vertices[2].normal = {0.0f, 1.0f, 0.0f, 1.0f};

    vertices[0].length = 5.0f;
    vertices[1].length = 5.0f;
    vertices[2].length = 5.0f;

    triangle_indices = new glm::uvec3[max_indices / 3];
    triangle_indices[0] = {0, 1, 2};

    render_vbo.write_data(sizeof(Vertex) * 3, vertices);
    ebo.write_data(sizeof(GLuint) * 3, triangle_indices);

    running = true;
}

static GLuint create_tree_indices(GLuint num_branches, glm::uvec3* triangles) {
    const glm::uvec3 start_indices[] = {{0, 1, 2}, {0, 3, 1}, {1, 3, 4},
                                        {1, 4, 2}, {2, 4, 5}, {2, 5, 0},
                                        {0, 5, 3}};

    const glm::uvec3 tip_indices[] = {{3, 6, 4}, {4, 6, 5}, {5, 6, 3}};

    const GLuint triangles_per_branch =
        sizeof(start_indices) / sizeof(glm::uvec3);

    GLuint num_triangles = 0;
    // GLuint num_tips = 1; // TODO: calculate
    for (GLuint current_branches = 0; current_branches < num_branches;
         ++current_branches) {
        for (GLuint i = 0; i < triangles_per_branch; ++i) {
            triangles[num_triangles++] =
                start_indices[i] + triangles_per_branch * current_branches;
        }
        // Add tip
        for (auto vec : tip_indices) {
            triangles[num_triangles++] =
                vec + triangles_per_branch * current_branches;
        }
    }

    return num_triangles;
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
        Checkbox("Render model", &render_model);
        Checkbox("Render wireframes", &render_wireframes);
        Checkbox("Render debug triangle", &render_debug_triangle);

        NewLine();
        Text("Debug triangle indices");
        InputScalarN("Indices", ImGuiDataType_U32, debug_triangle_indices, 3);

        End();
    }

    if (run_geometry_pass) {
        //          First Geometry pass          //
        glUseProgram(shaders.construction);

        render_vao.bind();
        feedback_vbo.set_as_feedback_target();

        glBeginTransformFeedback(GL_POINTS);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glEndTransformFeedback();

        // So that read_Data() and write_data() don't change the vao
        glBindVertexArray(0);

        glFlush();

        run_geometry_pass = false;

        // Load data into CPU memory
        feedback_vbo.read_data(sizeof(Vertex) * max_vertices, vertices);

        render_vbo.write_data(sizeof(Vertex) * max_vertices, vertices);

        num_triangles = create_tree_indices(1, triangle_indices);
        ebo.write_data(sizeof(glm::uvec3) * num_triangles, triangle_indices);
    }

    // Render
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update camera
    if (mouse.button_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        const float sensitivity = 0.1f;
        object_rotation += (mouse.x - mouse.last_x) * sensitivity;
    }

    ebo.write_data(sizeof(glm::uvec3) * num_triangles, triangle_indices);

    glm::mat4 projection =
        // glm::ortho(8.0f, -8.0f, -5.0f, 5.0f, 0.1f, 10.0f);
        glm::perspective(glm::radians(100.0f), 1920.0f / 1200.0f, 0.1f, 10.0f);

    // For some reason, the perspective matrix seems to switch the direction of
    // the x-axis, so we reverse it again here
    projection = glm::scale(projection, {-1.0, 1.0, 1.0});

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

        render_vao.bind();
        glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0);
    }

    // Debug rendering
    glUseProgram(shaders.line);

    GLuint projection_id = glGetUniformLocation(shaders.render, "projection");
    glUniformMatrix4fv(projection_id, 1, 0, value_ptr(projection));

    GLuint view_id = glGetUniformLocation(shaders.render, "view");
    glUniformMatrix4fv(view_id, 1, 0, value_ptr(view));

    GLuint model_id = glGetUniformLocation(shaders.render, "model");
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
