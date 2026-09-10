#include "app.hpp"

using namespace App;

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat) : Scene() {
  // initialize gpu pipeline
  renderer = new Gfx::BasicRenderer(targetFormat, gpu, Gfx::PT_Tri, SDL_GPU_CULLMODE_BACK, 800, 600);
  renderer->cam = Gfx::RenderCamera {
    .perspective = false
  };

  // add objects
  int obj1id = renderer->addObject(Gfx::rect2d(80.0f, 80.0f, 0.0f));
  Gfx::RenderObject& obj1 = renderer->getMutableObject(obj1id);
  obj1.pos = glm::vec3(-100.0, 0.0, 0.0);
  obj1.albedo = Gfx::RED;

  int obj2id = renderer->addObject(Gfx::rect2d(80.0f, 80.0f, 0.0f));
  Gfx::RenderObject& obj2 = renderer->getMutableObject(obj2id);
  obj2.pos = glm::vec3(0.0, 0.0, 0.0);
  obj2.albedo = Gfx::GREEN;

  int obj3id = renderer->addObject(Gfx::rect2d(80.0f, 80.0f, 0.0f));
  Gfx::RenderObject& obj3 = renderer->getMutableObject(obj3id);
  obj3.pos = glm::vec3(100.0, 0.0, 0.0);
  obj3.albedo = Gfx::BLUE;
}

SDL_AppResult BoardScene::update(SystemUpdates const &sys) {
  // resize if necessary
  if (sys.winSize.x != screenSize.x || sys.winSize.y != screenSize.y) {
    renderer->resizeScreen((Uint32)sys.winSize.x, (Uint32)sys.winSize.y);
    screenSize = sys.winSize;
  }
  // todo: handle inputs

  return SDL_APP_CONTINUE;
}

SDL_AppResult BoardScene::render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screen) {
  // todo: handle render update logic
  // render gpu pipeline
  renderer->render(cmdBuf, screen);

  return SDL_APP_CONTINUE;
}

void BoardScene::destroy() {
  // cleanup resources
  renderer->destroy();
  SDL_free(renderer);
}