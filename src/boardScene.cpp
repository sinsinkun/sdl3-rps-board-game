#include "app.hpp"

using namespace App;

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat) : Scene() {
  // todo: initialize gpu pipeline
}

SDL_AppResult BoardScene::update(SystemUpdates const &sys) {
  // todo: handle inputs

  return SDL_APP_CONTINUE;
}

SDL_AppResult BoardScene::render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screen) {
  // todo: handle render update logic
  // todo: render gpu pipeline

  return SDL_APP_CONTINUE;
}

void BoardScene::destroy() {
  // todo: cleanup resources
}