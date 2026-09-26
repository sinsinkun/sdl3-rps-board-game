#include "app.hpp"

using namespace App;

void addBoardTiles(BoardTile boardTiles[5][5], Gfx::BasicRenderer *renderer) {
  glm::vec3 positions[5][5] = {
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

  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      int objId = renderer->addObject(tile);
      Gfx::RenderObject& obj = renderer->getMutableObject(objId);
      obj.pos = positions[i][j];
      obj.albedo = Gfx::rgb(14, 71, 124);
      boardTiles[i][j].objectId = objId;
      boardTiles[i][j].position = positions[i][j];
    }
  }
}

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat, TTF_TextEngine *textEngine) : Scene() {
  // initialize gpu pipeline
  renderer = new Gfx::BasicRenderer(targetFormat, gpu, Gfx::PT_Triangle, SDL_GPU_CULLMODE_BACK, 800, 600);
  renderer->cam = Gfx::RenderCamera {
    .perspective = false
  };
  renderer->enableTextGeneration(targetFormat, textEngine, "assets/font.ttf", 24);

  addBoardTiles(boardTiles, renderer);
  SDL_GPUTexture *txt = renderer->createTextTexture(
    "Rock", glm::vec3(10.0, 10.0, 0.0), Gfx::WHITE, Gfx::TRANSPARENT,
    targetFormat, 100, 100
  );
  renderer->addTextureToObject(boardTiles[0][2].objectId, txt, glm::vec2(100.0, 100.0));
}

SDL_AppResult BoardScene::update(SystemUpdates const &sys) {
  // resize if necessary
  if (sys.winSize.x != screenSize.x || sys.winSize.y != screenSize.y) {
    renderer->resizeScreen((Uint32)sys.winSize.x, (Uint32)sys.winSize.y);
    screenSize = sys.winSize;
  }
  // handle inputs
  if (sys.keysPressed.find(SDLK_A) != sys.keysPressed.end()) {
    renderer->swapTexturesOnObjects(boardTiles[0][2].objectId, boardTiles[1][2].objectId);
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult BoardScene::render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screen) {
  // render gpu pipeline
  renderer->render(cmdBuf, screen);

  return SDL_APP_CONTINUE;
}

void BoardScene::destroy() {
  // cleanup resources
  renderer->destroy();
  SDL_free(renderer);
}
