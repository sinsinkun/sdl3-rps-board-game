#pragma once

#include "app.hpp"

namespace App {
  enum Rps {
    RPS_NONE, ROCK, PAPER, SCISSORS
  };
  // note: this is in [y][x] format due to how arrays work
  const glm::vec3 tilePositions[5][5] = {
    glm::vec3 {-200.0, 200.0, 0.0},
    glm::vec3 {-100.0, 200.0, 0.0},
    glm::vec3 {   0.0, 200.0, 0.0},
    glm::vec3 { 100.0, 200.0, 0.0},
    glm::vec3 { 200.0, 200.0, 0.0},

    glm::vec3 {-200.0, 100.0, 0.0},
    glm::vec3 {-100.0, 100.0, 0.0},
    glm::vec3 {   0.0, 100.0, 0.0},
    glm::vec3 { 100.0, 100.0, 0.0},
    glm::vec3 { 200.0, 100.0, 0.0},

    glm::vec3 {-200.0, 0.0, 0.0},
    glm::vec3 {-100.0, 0.0, 0.0},
    glm::vec3 {   0.0, 0.0, 0.0},
    glm::vec3 { 100.0, 0.0, 0.0},
    glm::vec3 { 200.0, 0.0, 0.0},

    glm::vec3 {-200.0, -100.0, 0.0},
    glm::vec3 {-100.0, -100.0, 0.0},
    glm::vec3 {   0.0, -100.0, 0.0},
    glm::vec3 { 100.0, -100.0, 0.0},
    glm::vec3 { 200.0, -100.0, 0.0},
    
    glm::vec3 {-200.0, -200.0, 0.0},
    glm::vec3 {-100.0, -200.0, 0.0},
    glm::vec3 {   0.0, -200.0, 0.0},
    glm::vec3 { 100.0, -200.0, 0.0},
    glm::vec3 { 200.0, -200.0, 0.0},
  };
  struct BoardTile {
    int objectId = -1;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec2 size = glm::vec2(0.0f);
    bool isCoordInsideTile(glm::vec2 coord) {
      float minX = position.x - (size.x / 2.0f);
      float maxX = position.x + (size.x / 2.0f);
      float minY = position.y - (size.y / 2.0f);
      float maxY = position.y + (size.y / 2.0f);
      bool isCoordInside = false;
      if (coord.x >= minX && coord.x <= maxX && coord.y >= minY && coord.y <= maxY) {
        isCoordInside = true;
      }
      return isCoordInside;
    }
  };
  struct PlayerTiles {
    BoardTile rock;
    BoardTile paper;
    BoardTile scissors;
  };
  class BoardScene: public Scene {
  public:
    BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat, TTF_TextEngine *textEngine);
    SDL_AppResult update(SystemUpdates const &sys) override;
    SDL_AppResult render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screenTx) override;
    void resetGameState();
    void destroy() override;
    // render pipeline
    Gfx::BasicRenderer *renderer;
  private:
    BoardTile boardTiles[5][5];
    PlayerTiles players[2];
    int activePlayer = 0;
    BoardTile* activeTile = NULL;
    glm::vec3 activeTileStartingPos = glm::vec3(0.0f);
    // UI
    BoardTile resetBtn;
    BoardTile msgDisplay;
  };
}