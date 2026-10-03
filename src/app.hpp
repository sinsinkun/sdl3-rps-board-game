#pragma once

#include <vector>
#include <set>
#include <iterator>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <glm/vec2.hpp>

#include "gfx/basicRenderer.hpp"

namespace App {
  const int MAX_FPS = 1000;
  enum MouseClickState { M_NONE, M_DOWN, M_UP };
  enum CursorType { C_DEFAULT, C_POINTER, C_TEXT, C_MOVE, C_PROGRESS };
  class SystemUpdates {
  public:
    glm::vec2 winSize = glm::vec2(800.0f, 600.0f);
    // fps calculation variables
    Uint64 lifetime = 0;
    float deltaTime = 0.0f;
    Uint64 timeSinceLastFps = 0;
    // inputs passthrough
    glm::vec2 mousePosScreenSpace = glm::vec2(0.0f);
    MouseClickState mouseClickState = MouseClickState::M_NONE;
    std::set<SDL_Keycode> keysPressed;
    std::set<SDL_Keycode> keysHeld;
    bool isKeyPressed(SDL_Keycode key) const {
      return keysPressed.find(key) != keysPressed.end();
    }
    bool isKeyHeld(SDL_Keycode key) const {
      return keysHeld.find(key) != keysHeld.end();
    }
  };
  struct UpdateResult {
    SDL_AppResult appResult = SDL_APP_CONTINUE;
    CursorType cursorStyle = C_DEFAULT;
  };
  class Scene {
  public:
    virtual UpdateResult update(SystemUpdates const &sys) {
      SDL_Log("ERR: scene update method not overwritten");
      UpdateResult res;
      return res;
    };
    virtual SDL_AppResult render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screenTx) {
      SDL_Log("ERR: scene render method not overwritten");
      return SDL_APP_CONTINUE;
    };
    virtual void destroy() {
      SDL_Log("ERR: scene destroy method not overwritten");
    };
  protected:
    Scene() {};
  };
  struct AppState {
    SDL_Window *window = NULL;
    SDL_GPUDevice *gpu = NULL;
    SDL_Surface *winIcon = NULL;
    SystemUpdates sys;
    std::vector<SDL_Cursor*> cursors;
    CursorType currentCursor = CursorType::C_DEFAULT;
    std::vector<Scene*> scenes;
    TTF_TextEngine *textEngine = NULL;
    int currentScene = 0;
  };
}