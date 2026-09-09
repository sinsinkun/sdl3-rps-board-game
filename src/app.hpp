#pragma once

#include <vector>
#include <SDL3/SDL.h>
#include <glm/vec2.hpp>

#include "gfx/basicObjectPipeline.hpp"

namespace App {
  struct SystemUpdates {
    glm::vec2 winSize = glm::vec2(800.0f, 600.0f);
    // fps calculation variables
    Uint64 lifetime = 0;
    float deltaTime = 0.0f;
    Uint64 timeSinceLastFps = 0;
    // inputs passthrough
    glm::vec2 mousePosScreenSpace = glm::vec2(0.0f);
    const bool *kbStates = NULL;
  };
  class Scene {
  public:
    virtual SDL_AppResult update(SystemUpdates const &sys) {
      SDL_Log("ERR: scene update method not overwritten");
      return SDL_APP_CONTINUE;
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
  class BoardScene: public Scene {
  public:
    BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat);
    SDL_AppResult update(SystemUpdates const &sys) override;
    SDL_AppResult render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screenTx) override;
    void destroy() override;
    // render pipeline
    Gfx::BasicObjectPipeline *gfxPipeline;
    glm::vec2 screenSize = glm::vec2(0.0f);
  };
  struct AppState {
    SDL_Window *window = NULL;
    SDL_GPUDevice *gpu = NULL;
    SystemUpdates sys;
    std::vector<Scene*> scenes;
    int currentScene = 0;
  };
}