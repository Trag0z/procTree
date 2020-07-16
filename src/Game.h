#pragma once
#include "pch.h"

class Game {
  public:
    bool running = false;
    U32 frame_start, last_frame_start;

    SDL_Window* window;
    SDL_Renderer* sdl_renderer;
    SDL_GLContext gl_context;

    GameConfig game_config;
    Renderer renderer;

    MouseKeyboardInput mouse_keyboard_input;
    std::array<Gamepad, Gamepad::NUM_PADS> gamepads;

    Player player;
    Background background;

    Level level;
    LevelEditor level_editor;

    enum GameMode { PLAY = 0, SPLINE_EDITOR = 1, LEVEL_EDITOR = 2 } game_mode;

    void init();
    bool run();

  private:
    void update_gui();
};