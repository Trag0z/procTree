#pragma once
#include "pch.h"
#include "Shaders.h"

static bool checkCompileErrors(GLuint object, bool program) {
    GLint success;
    GLchar infoLog[1024];

    if (program) {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            printf("ERROR: Program link-time error:\n%s", infoLog);
            return false;
        }
    } else {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            printf("ERROR: Shader compile-time error:\n%s", infoLog);
            return false;
        }
    }
    return true;
}

static std::string read_text_file(const char* path) {
    std::string string;
    try {
        std::ifstream input_file(path);

        std::stringstream stream;

        stream << input_file.rdbuf();

        input_file.close();

        string = stream.str();
    } catch (std::exception e) {
        printf("ERROR: Failed to read shader file/n");
        SDL_TriggerBreakpoint();
    }
    return string;
}

GLuint load_and_compile_shader_from_file(const char* vert_path,
                                         const char* geom_path,
                                         const char* frag_path) {
    GLuint program_id = glCreateProgram();

    // Vertex shader
    const std::string vert_shader_source = read_text_file(vert_path);
    auto vert_shader_c_str = vert_shader_source.c_str();

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_shader_c_str, NULL);
    glCompileShader(vert_shader);
    if (!checkCompileErrors(vert_shader, false))
        SDL_TriggerBreakpoint();
    glAttachShader(program_id, vert_shader);

    // Fragment shader
    GLuint frag_shader = 0;
    if (frag_path) {
        const std::string frag_shader_source = read_text_file(frag_path);
        auto frag_shader_c_str = frag_shader_source.c_str();

        frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_shader, 1, &frag_shader_c_str, NULL);
        glCompileShader(frag_shader);
        if (!checkCompileErrors(frag_shader, false))
            SDL_TriggerBreakpoint();
        glAttachShader(program_id, frag_shader);
    }

    // Geometry shader
    GLuint geom_shader = 0;
    if (geom_path) {
        const std::string geom_shader_source = read_text_file(geom_path);
        auto gem_shader_c_str = geom_shader_source.c_str();

        geom_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom_shader, 1, &gem_shader_c_str, NULL);
        glCompileShader(geom_shader);
        if (!checkCompileErrors(geom_shader, false))
            SDL_TriggerBreakpoint();
        glAttachShader(program_id, geom_shader);

        const GLchar* feedbackVaryings[] = {
            "position"}; //, "normal", "ext_length"};
        glTransformFeedbackVaryings(program_id, 1, feedbackVaryings,
                                    GL_INTERLEAVED_ATTRIBS);
    }

    glLinkProgram(program_id);
    if (!checkCompileErrors(program_id, true))
        SDL_TriggerBreakpoint();

    glDeleteShader(vert_shader);
    if (frag_path)
        glDeleteShader(frag_shader);
    if (geom_path)
        glDeleteShader(geom_shader);

    return program_id;
}