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
  glm::vec2 tileSize = glm::vec2(90.0f, 90.0f);
  Gfx::Primitive tile = Gfx::rect2d(tileSize.x, tileSize.y, 0.0f);

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
      boardTiles[i][j].size = tileSize;
    }
  }
}

void addPlayerTiles(PlayerTiles& playerTiles, int variation, Gfx::BasicRenderer *renderer) {
  Gfx::Primitive tile = Gfx::rect2d(80.0f, 80.0f, 0.0f);
  glm::vec2 tileSize = glm::vec2(80.0f, 80.0f);
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
  playerTiles.rock.size = tileSize;

  int paperId = renderer->addObject(tile);
  Gfx::RenderObject& paper = renderer->getMutableObject(paperId);
  renderer->updateObjectTexture(paperId, paperTxt, glm::vec2(100.0, 100.0));
  paper.pos = paperPos;
  playerTiles.paper.objectId = paperId;
  playerTiles.paper.position = paperPos;
  playerTiles.paper.size = tileSize;

  int scissorsId = renderer->addObject(tile);
  renderer->updateObjectTexture(scissorsId, scissorsTxt, glm::vec2(100.0, 100.0));
  Gfx::RenderObject& scissors = renderer->getMutableObject(scissorsId);
  scissors.pos = scissorsPos;
  playerTiles.scissors.objectId = scissorsId;
  playerTiles.scissors.position = scissorsPos;
  playerTiles.scissors.size = tileSize;
}

void addResetButton(BoardTile& resetBtn, Gfx::BasicRenderer *renderer) {
  glm::vec2 size = glm::vec2(120.0f, 40.0f);
  glm::vec3 pos = glm::vec3(0.0f, 280.0f, 10.0f);
  Gfx::Primitive tile = Gfx::rect2d(size.x, size.y, 0.0f);

  SDL_GPUTexture *txtx = renderer->createTextTexture(
    "Reset", glm::vec3(30.0, 10.0, 0.0), Gfx::WHITE, Gfx::BLUE,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 120, 40
  );

  int objId = renderer->addObject(tile);
  renderer->updateObjectTexture(objId, txtx, size);
  Gfx::RenderObject& obj = renderer->getMutableObject(objId);
  obj.pos = pos;

  resetBtn.objectId = objId;
  resetBtn.position = pos;
  resetBtn.size = size;
}

void addMsgDisplay(BoardTile& msgDisplay, Gfx::BasicRenderer *renderer) {
  glm::vec2 size = glm::vec2(160.0f, 40.0f);
  glm::vec3 pos = glm::vec3(-300.0f, 280.0f, 10.0f);
  Gfx::Primitive tile = Gfx::rect2d(size.x, size.y, 0.0f);

  SDL_GPUTexture *txtx = renderer->createTextTexture(
    "Player 1's turn", glm::vec3(10.0, 10.0, 0.0), Gfx::WHITE, Gfx::BLACK,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 240, 60
  );

  int objId = renderer->addObject(tile);
  renderer->updateObjectTexture(objId, txtx, size);
  Gfx::RenderObject& obj = renderer->getMutableObject(objId);
  obj.pos = pos;

  msgDisplay.objectId = objId;
  msgDisplay.position = pos;
}

glm::vec2 getCursorWorldSpace(glm::vec2 mousePos, glm::vec2 screenSize) {
  // position translates 1:1, but camera is centered on the screen
  float xPos = mousePos.x - (screenSize.x / 2.0);
  float yPos = (screenSize.y / 2.0) - mousePos.y;
  return glm::vec2(xPos, yPos);
}

// 0: none, 1: rock, 2: paper, 3: scissors
Rps findMouseOverRps(glm::vec2 cursorPos, PlayerTiles& activePlayer) {
  if (activePlayer.rock.isCoordInsideTile(cursorPos)) return Rps::ROCK;
  if (activePlayer.paper.isCoordInsideTile(cursorPos)) return Rps::PAPER;
  if (activePlayer.scissors.isCoordInsideTile(cursorPos)) return Rps::SCISSORS;
  return Rps::RPS_NONE;
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

void updateDisplayText(BoardTile& msgDisplay, Gfx::BasicRenderer *renderer, std::string text) {
  SDL_GPUTexture *txtx = renderer->createTextTexture(
    text, glm::vec3(10.0, 10.0, 0.0), Gfx::WHITE, Gfx::BLACK,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 240, 60
  );
  renderer->updateObjectTexture(msgDisplay.objectId, txtx, glm::vec2(160.0f, 40.0f));
}

#pragma endregion helpers

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat, TTF_TextEngine *textEngine) : Scene() {
  // initialize gpu pipeline
  renderer = new Gfx::BasicRenderer(targetFormat, gpu, Gfx::PT_Triangle, SDL_GPU_CULLMODE_BACK, 800, 600);
  renderer->cam = Gfx::RenderCamera {
    .perspective = false
  };
  renderer->enableTextGeneration(SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, textEngine, "assets/font.ttf", 24);

  // add render assets
  addBoardTiles(boardTiles, renderer);
  addPlayerTiles(players[0], 1, renderer);
  addPlayerTiles(players[1], 2, renderer);
  addResetButton(resetBtn, renderer);
  addMsgDisplay(msgDisplay, renderer);
}

SDL_AppResult BoardScene::update(SystemUpdates const &sys) {
  // resize if necessary
  if (sys.winSize.x != screenSize.x || sys.winSize.y != screenSize.y) {
    renderer->resizeScreen((Uint32)sys.winSize.x, (Uint32)sys.winSize.y);
    screenSize = sys.winSize;
  }
  // handle inputs
  glm::vec2 cursorPos = getCursorWorldSpace(sys.mousePosScreenSpace, screenSize);
  Rps rps = findMouseOverRps(cursorPos, players[activePlayer]);
  // 1. find active file
  if (sys.mouseClickState == MouseClickState::DOWN && activeTile == NULL) {
    if (activePlayer == 0 || activePlayer == 1) {
      switch (rps) {
        case Rps::ROCK:
          activeTile = &players[activePlayer].rock;
          activeTileStartingPos = activeTile->position;
          break;
        case Rps::PAPER:
          activeTile = &players[activePlayer].paper;
          activeTileStartingPos = activeTile->position;
          break;
        case Rps::SCISSORS:
          activeTile = &players[activePlayer].scissors;
          activeTileStartingPos = activeTile->position;
          break;
        default:
          break;
      }
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
        SDL_Log("Invalid target position (%f, %f, %f) -> (%f, %f, %f)",
          activeTileStartingPos.x, activeTileStartingPos.y, activeTileStartingPos.z,
          targetPos.x, targetPos.y, targetPos.z
        );
        targetPos = activeTileStartingPos;
      }
      // resolve collision with opponent tiles
      glm::vec3 inactivePos = glm::vec3(9999.0f, 9999.0f, 0.0f);
      switch (rps) {
        case Rps::ROCK:
          if (targetPos == players[inactivePlayer].rock.position) {
            targetPos = activeTileStartingPos;
          }
          else if (targetPos == players[inactivePlayer].paper.position) {
            Gfx::RenderObject& activeObj = renderer->getMutableObject(activeTile->objectId);
            activeObj.visible = false;
            activeObj.pos = inactivePos;
            activeTile->position = inactivePos;
          }
          else if (targetPos == players[inactivePlayer].scissors.position) {
            Gfx::RenderObject& inactiveObj = renderer->getMutableObject(players[inactivePlayer].scissors.objectId);
            inactiveObj.visible = false;
            inactiveObj.pos = inactivePos;
            players[inactivePlayer].scissors.position = inactivePos;
          }
          break;
        case Rps::PAPER:
          if (targetPos == players[inactivePlayer].rock.position) {
            Gfx::RenderObject& inactiveObj = renderer->getMutableObject(players[inactivePlayer].rock.objectId);
            inactiveObj.visible = false;
            inactiveObj.pos = inactivePos;
            players[inactivePlayer].rock.position = inactivePos;
          }
          else if (targetPos == players[inactivePlayer].paper.position) {
            targetPos = activeTileStartingPos;
          }
          else if (targetPos == players[inactivePlayer].scissors.position) {
            Gfx::RenderObject& activeObj = renderer->getMutableObject(activeTile->objectId);
            activeObj.visible = false;
            activeObj.pos = inactivePos;
            activeTile->position = inactivePos;
          }
          break;
        case Rps::SCISSORS:
          if (targetPos == players[inactivePlayer].rock.position) {
            Gfx::RenderObject& activeObj = renderer->getMutableObject(activeTile->objectId);
            activeObj.visible = false;
            activeObj.pos = inactivePos;
            activeTile->position = inactivePos;
          }
          else if (targetPos == players[inactivePlayer].paper.position) {
            Gfx::RenderObject& inactiveObj = renderer->getMutableObject(players[inactivePlayer].paper.objectId);
            inactiveObj.visible = false;
            inactiveObj.pos = inactivePos;
            players[inactivePlayer].paper.position = inactivePos;
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
      // check for win condition
      bool gameEnded = false;
      if (activePlayer == 0 && targetPos.x == 200.0f) {
        SDL_Log("Player 1 wins");
        updateDisplayText(msgDisplay, renderer, "Player 1 WINS!");
        gameEnded = true;
      } else if (activePlayer == 1 && targetPos.x == -200.0f) {
        SDL_Log("Player 2 wins");
        updateDisplayText(msgDisplay, renderer, "Player 2 WINS!");
        gameEnded = true;
      }
      // check that inactive player still has tiles
      if (!gameEnded) {
        Gfx::RenderObject& rock1 = renderer->getMutableObject(players[0].rock.objectId);
        Gfx::RenderObject& paper1 = renderer->getMutableObject(players[0].paper.objectId);
        Gfx::RenderObject& scissors1 = renderer->getMutableObject(players[0].scissors.objectId);
        if (!rock1.visible && !paper1.visible && !scissors1.visible) {
          SDL_Log("Player 2 win by wipeout");
          updateDisplayText(msgDisplay, renderer, "Player 2 WINS!");
          gameEnded = true;
        }
        Gfx::RenderObject& rock2 = renderer->getMutableObject(players[1].rock.objectId);
        Gfx::RenderObject& paper2 = renderer->getMutableObject(players[1].paper.objectId);
        Gfx::RenderObject& scissors2 = renderer->getMutableObject(players[1].scissors.objectId);
        if (!rock2.visible && !paper2.visible && !scissors2.visible) {
          SDL_Log("Player 1 win by wipeout");
          updateDisplayText(msgDisplay, renderer, "Player 1 WINS!");
          gameEnded = true;
        }
      }
      // switch active player
      if (targetPos != activeTileStartingPos) {
        SDL_Log("Switching active player");
        if (!gameEnded && inactivePlayer == 0) {
          updateDisplayText(msgDisplay, renderer, "Player 1's turn");
        }
        if (!gameEnded && inactivePlayer == 1) {
          updateDisplayText(msgDisplay, renderer, "Player 2's turn");
        }
        activePlayer = gameEnded ? 99 : inactivePlayer;
      }
    }
    activeTile = NULL;
  }
  // 4. handle UI interaction
  else if (sys.mouseClickState == MouseClickState::UP && activeTile == NULL) {
    // check for reset btn click
    if (resetBtn.isCoordInsideTile(cursorPos)) {
      updateDisplayText(msgDisplay, renderer, "Player 1's turn");
      resetGameState();
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult BoardScene::render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screen) {
  // render gpu pipeline
  renderer->render(cmdBuf, screen);

  return SDL_APP_CONTINUE;
}

void BoardScene::resetGameState() {
  // reset player 1
  players[0].rock.position = glm::vec3 {-200.0, -100.0, 1.0};
  Gfx::RenderObject& rock1 = renderer->getMutableObject(players[0].rock.objectId);
  rock1.pos = glm::vec3 {-200.0, -100.0, 1.0};
  rock1.visible = true;

  players[0].paper.position = glm::vec3 {-200.0, 0.0, 1.0};
  Gfx::RenderObject& paper1 = renderer->getMutableObject(players[0].paper.objectId);
  paper1.pos = glm::vec3 {-200.0, 0.0, 1.0};
  paper1.visible = true;

  players[0].scissors.position = glm::vec3 {-200.0, 100.0, 1.0};
  Gfx::RenderObject& scissors1 = renderer->getMutableObject(players[0].scissors.objectId);
  scissors1.pos = glm::vec3 {-200.0, 100.0, 1.0};
  scissors1.visible = true;

  // reset player 2
  players[1].rock.position = glm::vec3 {200.0, 100.0, 1.0};
  Gfx::RenderObject& rock2 = renderer->getMutableObject(players[1].rock.objectId);
  rock2.pos = glm::vec3 {200.0, 100.0, 1.0};
  rock2.visible = true;

  players[1].paper.position = glm::vec3 {200.0, 0.0, 1.0};
  Gfx::RenderObject& paper2 = renderer->getMutableObject(players[1].paper.objectId);
  paper2.pos = glm::vec3 {200.0, 0.0, 1.0};
  paper2.visible = true;

  players[1].scissors.position = glm::vec3 {200.0,-100.0, 1.0};
  Gfx::RenderObject& scissors2 = renderer->getMutableObject(players[1].scissors.objectId);
  scissors2.pos = glm::vec3 {200.0, -100.0, 1.0};
  scissors2.visible = true;

  // reset active player
  activePlayer = 0;
  activeTile = NULL;
}

void BoardScene::destroy() {
  // cleanup resources
  renderer->destroy();
  SDL_free(renderer);
}
