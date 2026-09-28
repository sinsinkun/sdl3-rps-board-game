#pragma once

#include "app.hpp"

namespace App {
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
}