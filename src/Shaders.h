#pragma once
#include "pch.h"

GLuint load_and_compile_shader_from_file(const char* vert_path,
                                         const char* geom_path,
                                         const char* frag_path);
