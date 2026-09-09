#include "app.hpp"

using namespace App;

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat) : Scene() {
  // initialize gpu pipeline
  gfxPipeline = new Gfx::BasicObjectPipeline(targetFormat, gpu, Gfx::PT_Tri, SDL_GPU_CULLMODE_BACK, 800, 600);
  gfxPipeline->cam = Gfx::RenderCamera {
    .perspective = false
  };

  int obj1id = gfxPipeline->addObject(Gfx::tube(80.0f, 40.0f, 100.0f, 18));
}

SDL_AppResult BoardScene::update(SystemUpdates const &sys) {
  // resize if necessary
  if (sys.winSize.x != screenSize.x || sys.winSize.y != screenSize.y) {
    gfxPipeline->resizeScreen((Uint32)sys.winSize.x, (Uint32)sys.winSize.y);
    screenSize = sys.winSize;
  }
  // todo: handle inputs

  return SDL_APP_CONTINUE;
}

SDL_AppResult BoardScene::render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screen) {
  // todo: handle render update logic
  // render gpu pipeline
  gfxPipeline->render(cmdBuf, screen);

  return SDL_APP_CONTINUE;
}

void BoardScene::destroy() {
  // cleanup resources
  gfxPipeline->destroy();
  SDL_free(gfxPipeline);
}