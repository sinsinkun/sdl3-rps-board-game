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
  class SystemUpdates {
  public:
    glm::vec2 winSize = glm::vec2(800.0f, 600.0f);
    // fps calculation variables
    Uint64 lifetime = 0;
    float deltaTime = 0.0f;
    Uint64 timeSinceLastFps = 0;
    // inputs passthrough
    glm::vec2 mousePosScreenSpace = glm::vec2(0.0f);
    std::set<SDL_Keycode> keysPressed;
    std::set<SDL_Keycode> keysHeld;
    bool isKeyPressed(SDL_Keycode key) const {
      return keysPressed.find(key) != keysPressed.end();
    }
    bool isKeyHeld(SDL_Keycode key) const {
      return keysHeld.find(key) != keysHeld.end();
    }
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

  enum BoardTileState {
    DEFAULT,
    ACCESSIBLE,
    ROCK,
    PAPER,
    SCISSORS,
  };

  struct BoardTile {
    int objectId = -1;
    glm::vec3 position = glm::vec3(0.0f);
    BoardTileState state = BoardTileState::DEFAULT;
  };

  class BoardScene: public Scene {
  public:
    BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat, TTF_TextEngine *textEngine);
    SDL_AppResult update(SystemUpdates const &sys) override;
    SDL_AppResult render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screenTx) override;
    void destroy() override;
    // render pipeline
    Gfx::BasicRenderer *renderer;
    glm::vec2 screenSize = glm::vec2(0.0f);
  private:
    BoardTile boardTiles[5][5];
  };

  struct AppState {
    SDL_Window *window = NULL;
    SDL_GPUDevice *gpu = NULL;
    SDL_Surface *winIcon = NULL;
    SystemUpdates sys;
    std::vector<Scene*> scenes;
    TTF_TextEngine *textEngine = NULL;
    int currentScene = 0;
  };
}