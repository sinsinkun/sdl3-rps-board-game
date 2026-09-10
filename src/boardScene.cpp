#include "app.hpp"

using namespace App;

void addBoardToRendererObjects(Gfx::BasicRenderer *renderer) {
  std::vector<glm::vec3> positions = {
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
  Gfx::Primitive tile = Gfx::rect2d(90.0f, 90.0f, 0.0f);

  for (int i=0; i<positions.size(); i++) {
    int objId = renderer->addObject(tile);
    Gfx::RenderObject& obj = renderer->getMutableObject(objId);
    obj.pos = positions.at(i);
    obj.albedo = Gfx::rgb(14, 71, 124);
  }
}

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat) : Scene() {
  // initialize gpu pipeline
  renderer = new Gfx::BasicRenderer(targetFormat, gpu, Gfx::PT_Triangle, SDL_GPU_CULLMODE_BACK, 800, 600);
  renderer->cam = Gfx::RenderCamera {
    .perspective = false
  };

  addBoardToRendererObjects(renderer);
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