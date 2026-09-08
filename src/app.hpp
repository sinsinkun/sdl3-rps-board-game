#pragma once

#include <SDL3/SDL.h>

namespace App {
  struct AppState {
    SDL_Window *window = NULL;
    SDL_GPUDevice *gpu = NULL;
  };
}