#include "boardScene.hpp"

using namespace App;

#pragma region helpers

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
      // mark goal points
      if (j == 0 || j == 4) {
        obj.albedo = Gfx::rgb(23, 112, 163);
      }
      boardTiles[i][j].objectId = objId;
      boardTiles[i][j].position = positions[i][j];
    }
  }
}

void addPlayerTiles(PlayerTiles& playerTiles, int variation, Gfx::BasicRenderer *renderer) {
  Gfx::Primitive tile = Gfx::rect2d(80.0f, 80.0f, 0.0f);
  // set positions
  glm::vec3 rockPos = glm::vec3 {-200.0, -100.0, 1.0};
  glm::vec3 paperPos = glm::vec3 {-200.0, 0.0, 1.0};
  glm::vec3 scissorsPos = glm::vec3 {-200.0, 100.0, 1.0};
  SDL_FColor bgColor = Gfx::PURPLE;
  if (variation == 2) {
    rockPos = glm::vec3 {200.0, 100.0, 1.0};
    paperPos = glm::vec3 {200.0, 0.0, 1.0};
    scissorsPos = glm::vec3 {200.0, -100.0, 1.0};
    bgColor = Gfx::ORANGE;
  }

  SDL_GPUTexture *rockTxt = renderer->createTextTexture(
    "Rock", glm::vec3(22.0, 38.0, 0.0), Gfx::WHITE, bgColor,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 100, 100
  );
  SDL_GPUTexture *paperTxt = renderer->createTextTexture(
    "Paper", glm::vec3(18.0, 38.0, 0.0), Gfx::WHITE, bgColor,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 100, 100
  );
  SDL_GPUTexture *scissorsTxt = renderer->createTextTexture(
    "Scissors", glm::vec3(4.0, 38.0, 0.0), Gfx::WHITE, bgColor,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 100, 100
  );

  int rockId = renderer->addObject(tile);
  renderer->updateObjectTexture(rockId, rockTxt, glm::vec2(100.0, 100.0));
  Gfx::RenderObject& rock = renderer->getMutableObject(rockId);
  rock.pos = rockPos;
  playerTiles.rock.objectId = rockId;
  playerTiles.rock.position = rockPos;

  int paperId = renderer->addObject(tile);
  Gfx::RenderObject& paper = renderer->getMutableObject(paperId);
  renderer->updateObjectTexture(paperId, paperTxt, glm::vec2(100.0, 100.0));
  paper.pos = paperPos;
  playerTiles.paper.objectId = paperId;
  playerTiles.paper.position = paperPos;

  int scissorsId = renderer->addObject(tile);
  renderer->updateObjectTexture(scissorsId, scissorsTxt, glm::vec2(100.0, 100.0));
  Gfx::RenderObject& scissors = renderer->getMutableObject(scissorsId);
  scissors.pos = scissorsPos;
  playerTiles.scissors.objectId = scissorsId;
  playerTiles.scissors.position = scissorsPos;
}

glm::vec2 getCursorWorldSpace(glm::vec2 mousePos, glm::vec2 screenSize) {
  // position translates 1:1, but camera is centered on the screen
  float xPos = mousePos.x - (screenSize.x / 2.0);
  float yPos = (screenSize.y / 2.0) - mousePos.y;
  return glm::vec2(xPos, yPos);
}

// 0: none, 1: rock, 2: paper, 3: scissors
int findMouseOverRps(glm::vec2 cursorPos, PlayerTiles activePlayer) {
  glm::vec3 cPos = glm::vec3(cursorPos.x, cursorPos.y, 0.0);
  if (glm::distance(cPos, activePlayer.rock.position) < 40.0f) return 1;
  if (glm::distance(cPos, activePlayer.paper.position) < 40.0f) return 2;
  if (glm::distance(cPos, activePlayer.scissors.position) < 40.0f) return 3;
  return 0;
}

BoardTile* findNearestBoardTile(BoardTile* tileToMove, BoardTile boardTiles[5][5]) {
  // when tiles are dropped, they need to snap to the nearest boardTile
  BoardTile* nearestTile = NULL;
  float shortestDistance = 100000.0f;
  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      glm::vec3 a = tileToMove->position;
      glm::vec3 b = boardTiles[i][j].position;
      float dist = glm::distance(a, b);
      if (dist < shortestDistance) {
        shortestDistance = dist;
        nearestTile = &boardTiles[i][j];
      }
    }
  }
  return nearestTile;
}

bool isSelfColliding(PlayerTiles const &player, int activeId, glm::vec3 const &targetPos) {
  if (player.rock.objectId != activeId &&targetPos == player.rock.position) {
    return true;
  }
  if (player.paper.objectId != activeId &&targetPos == player.paper.position) {
    return true;
  }
  if (player.scissors.objectId != activeId &&targetPos == player.scissors.position) {
    return true;
  }
  return false;
}

#pragma endregion helpers

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat, TTF_TextEngine *textEngine) : Scene() {
  // initialize gpu pipeline
  renderer = new Gfx::BasicRenderer(targetFormat, gpu, Gfx::PT_Triangle, SDL_GPU_CULLMODE_BACK, 800, 600);
  renderer->cam = Gfx::RenderCamera {
    .perspective = false
  };
  renderer->enableTextGeneration(SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, textEngine, "assets/font.ttf", 24);

  addBoardTiles(boardTiles, renderer);
  addPlayerTiles(players[0], 1, renderer);
  addPlayerTiles(players[1], 2, renderer);
}

SDL_AppResult BoardScene::update(SystemUpdates const &sys) {
  // resize if necessary
  if (sys.winSize.x != screenSize.x || sys.winSize.y != screenSize.y) {
    renderer->resizeScreen((Uint32)sys.winSize.x, (Uint32)sys.winSize.y);
    screenSize = sys.winSize;
  }
  // handle inputs
  glm::vec2 cursorPos = getCursorWorldSpace(sys.mousePosScreenSpace, screenSize);
  int rpsNum = findMouseOverRps(cursorPos, players[activePlayer]);
  // 1. find active file
  if (sys.mouseClickState == MouseClickState::DOWN && activeTile == NULL) {
    switch (rpsNum) {
      case 1: // rock
        activeTile = &players[activePlayer].rock;
        activeTileStartingPos = activeTile->position;
        break;
      case 2: // paper
        activeTile = &players[activePlayer].paper;
        activeTileStartingPos = activeTile->position;
        break;
      case 3: // scissors
        activeTile = &players[activePlayer].scissors;
        activeTileStartingPos = activeTile->position;
        break;
      default:
        break;
    }
  }
  // 2. handle movement of active tile
  else if (sys.mouseClickState == MouseClickState::DOWN && activeTile != NULL) {
    // restrict movement to 100px radius around starting position
    glm::vec3 targetPos = glm::vec3(cursorPos.x, cursorPos.y, 1.0);
    activeTile->position = targetPos;
    Gfx::RenderObject& obj = renderer->getMutableObject(activeTile->objectId);
    // render the active object above other tiles
    obj.pos = glm::vec3(targetPos.x, targetPos.y, 2.0);
  }
  // 3. handle dropping of active tile
  else if (sys.mouseClickState == MouseClickState::UP && activeTile != NULL) {
    BoardTile* nearestTile = findNearestBoardTile(activeTile, boardTiles);
    // reset to original position if nearestTile not found
    if (nearestTile == NULL) {
      SDL_Log(
        "Could not find nearest tile - something went wrong. activeTile pos: (%f, %f, %f)", 
        activeTile->position.x, activeTile->position.y, activeTile->position.z
      );
      activeTile->position = activeTileStartingPos;
      Gfx::RenderObject& obj = renderer->getMutableObject(activeTile->objectId);
      obj.pos = activeTileStartingPos;
    } else {
      glm::vec3 targetPos = glm::vec3(nearestTile->position.x, nearestTile->position.y, 1.0);
      int inactivePlayer = activePlayer == 0 ? 1 : 0;
      // prevent collision with own tiles
      if (isSelfColliding(players[activePlayer], activeTile->objectId, targetPos)) {
        SDL_Log("Colliding with self - resetting position");
        targetPos = activeTileStartingPos;
      }
      // prevent moving to invalid position
      if (glm::length(targetPos - activeTileStartingPos) > 100.0f) {
        SDL_Log("Invalid target position");
        targetPos = activeTileStartingPos;
      }
      // resolve collision with opponent tiles
      switch (rpsNum) {
        case 1: // rock
          if (targetPos == players[inactivePlayer].rock.position) {
            targetPos = activeTileStartingPos;
          }
          else if (targetPos == players[inactivePlayer].paper.position) {
            Gfx::RenderObject& activeObj = renderer->getMutableObject(activeTile->objectId);
            activeObj.visible = false;
          }
          else if (targetPos == players[inactivePlayer].scissors.position) {
            Gfx::RenderObject& inactiveObj = renderer->getMutableObject(players[inactivePlayer].scissors.objectId);
            inactiveObj.visible = false;
          }
          break;
        case 2: // paper
          if (targetPos == players[inactivePlayer].rock.position) {
            Gfx::RenderObject& inactiveObj = renderer->getMutableObject(players[inactivePlayer].rock.objectId);
            inactiveObj.visible = false;
          }
          else if (targetPos == players[inactivePlayer].paper.position) {
            targetPos = activeTileStartingPos;
          }
          else if (targetPos == players[inactivePlayer].scissors.position) {
            Gfx::RenderObject& activeObj = renderer->getMutableObject(activeTile->objectId);
            activeObj.visible = false;
          }
          break;
        case 3: // scissors
          if (targetPos == players[inactivePlayer].rock.position) {
            Gfx::RenderObject& activeObj = renderer->getMutableObject(activeTile->objectId);
            activeObj.visible = false;
          }
          else if (targetPos == players[inactivePlayer].paper.position) {
            Gfx::RenderObject& inactiveObj = renderer->getMutableObject(players[inactivePlayer].paper.objectId);
            inactiveObj.visible = false;
          }
          else if (targetPos == players[inactivePlayer].scissors.position) {
            targetPos = activeTileStartingPos;
          }
          break;
        default:
          break;
      }
      // move activeTile to new tile
      activeTile->position = targetPos;
      Gfx::RenderObject& obj = renderer->getMutableObject(activeTile->objectId);
      obj.pos = targetPos;
      // todo: check for win condition
      // switch active player
      if (targetPos != activeTileStartingPos) {
        SDL_Log("Switching active player");
        activePlayer = inactivePlayer;
      }
    }
    activeTile = NULL;
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
