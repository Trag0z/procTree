#pragma once
#include "pch.h"

class Application {
    SDL_Window* window;
    SDL_Renderer* sdl_renderer;
    SDL_GLContext gl_context;

  public:
    void init();
    bool run();
};