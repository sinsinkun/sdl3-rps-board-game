#include "boardScene.hpp"

using namespace App;

#pragma region helpers

void addBoardTiles(BoardTile boardTiles[5][5], Gfx::BasicRenderer *renderer) {
  glm::vec2 tileSize = glm::vec2(90.0f, 90.0f);
  Gfx::Primitive tile = Gfx::rect2d(tileSize.x, tileSize.y, 0.0f);

  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      int objId = renderer->addObject(tile);
      Gfx::RenderObject& obj = renderer->getMutableObject(objId);
      obj.pos = tilePositions[i][j];
      obj.albedo = Gfx::rgb(14, 71, 124);
      // mark goal points
      if (j == 0 || j == 4) {
        obj.albedo = Gfx::rgb(23, 112, 163);
      }
      boardTiles[i][j].objectId = objId;
      boardTiles[i][j].position = tilePositions[i][j];
      boardTiles[i][j].size = tileSize;
    }
  }
}

void addPlayerTiles(PlayerTiles& playerTiles, int variation, Gfx::BasicRenderer *renderer) {
  Gfx::Primitive tile = Gfx::rect2d(80.0f, 80.0f, 0.0f);
  glm::vec2 tileSize = glm::vec2(80.0f, 80.0f);
  // set positions
  glm::vec3 rockPos = playerPositions[0];
  glm::vec3 paperPos = playerPositions[1];
  glm::vec3 scissorsPos = playerPositions[2];
  SDL_FColor bgColor = Gfx::rgb(134, 86, 13);
  if (variation == 2) {
    rockPos = playerPositions[3];
    paperPos = playerPositions[4];
    scissorsPos = playerPositions[5];
    bgColor = Gfx::rgb(80, 16, 139);
  }

  SDL_GPUTexture *rockTxt = renderer->createTextTexture(
    "Rock", glm::vec3(20.0, 32.0, 0.0), Gfx::WHITE, bgColor,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, (1.2f * tileSize)
  );
  SDL_GPUTexture *paperTxt = renderer->createTextTexture(
    "Paper", glm::vec3(16.0, 32.0, 0.0), Gfx::WHITE, bgColor,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, (1.2f * tileSize)
  );
  SDL_GPUTexture *scissorsTxt = renderer->createTextTexture(
    "Scissors", glm::vec3(2.0, 32.0, 0.0), Gfx::WHITE, bgColor,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, (1.2f * tileSize)
  );

  int rockId = renderer->addObject(tile);
  renderer->updateObjectTexture(rockId, rockTxt, tileSize);
  Gfx::RenderObject& rock = renderer->getMutableObject(rockId);
  rock.pos = rockPos;
  playerTiles.rock.objectId = rockId;
  playerTiles.rock.position = rockPos;
  playerTiles.rock.size = tileSize;

  int paperId = renderer->addObject(tile);
  Gfx::RenderObject& paper = renderer->getMutableObject(paperId);
  renderer->updateObjectTexture(paperId, paperTxt, tileSize);
  paper.pos = paperPos;
  playerTiles.paper.objectId = paperId;
  playerTiles.paper.position = paperPos;
  playerTiles.paper.size = tileSize;

  int scissorsId = renderer->addObject(tile);
  renderer->updateObjectTexture(scissorsId, scissorsTxt, tileSize);
  Gfx::RenderObject& scissors = renderer->getMutableObject(scissorsId);
  scissors.pos = scissorsPos;
  playerTiles.scissors.objectId = scissorsId;
  playerTiles.scissors.position = scissorsPos;
  playerTiles.scissors.size = tileSize;
}

void addResetButton(BoardTile& resetBtn, Gfx::BasicRenderer *renderer) {
  glm::vec2 size = glm::vec2(120.0f, 40.0f);
  glm::vec3 pos = glm::vec3(0.0f, 275.0f, 10.0f);
  Gfx::Primitive tile = Gfx::rect2d(size.x, size.y, 0.0f);

  SDL_GPUTexture *txtx = renderer->createTextTexture(
    "Reset", glm::vec3(30.0, 8.0, 0.0), Gfx::WHITE, Gfx::rgb(145, 39, 39),
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, size
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
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, (1.5f * size)
  );

  int objId = renderer->addObject(tile);
  renderer->updateObjectTexture(objId, txtx, size);
  Gfx::RenderObject& obj = renderer->getMutableObject(objId);
  obj.pos = pos;

  msgDisplay.objectId = objId;
  msgDisplay.position = pos;
}

void updateDisplayText(BoardTile& msgDisplay, Gfx::BasicRenderer *renderer, std::string text) {
  SDL_GPUTexture *txtx = renderer->createTextTexture(
    text, glm::vec3(10.0, 10.0, 0.0), Gfx::WHITE, Gfx::BLACK,
    SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, glm::vec2(240.0f, 60.0f)
  );
  renderer->updateObjectTexture(msgDisplay.objectId, txtx, glm::vec2(160.0f, 40.0f));
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

void deactivateTile(BoardTile *tile, Gfx::BasicRenderer *renderer, int &tilesLeft) {
  if (tile == NULL) {
    SDL_Log("Tried to deactivate NULL tile");
    return;
  }
  static const glm::vec3 inactivePos = glm::vec3(9999.0f, 9999.0f, 0.0f);
  Gfx::RenderObject& obj = renderer->getMutableObject(tile->objectId);
  obj.visible = false;
  obj.pos = inactivePos;
  tile->position = inactivePos;
  tilesLeft -= 1;
}

Rps opponentAtTargetPos(glm::vec3 const &targetPos, PlayerTiles &opponent) {
  if (targetPos == opponent.rock.position) return Rps::ROCK;
  if (targetPos == opponent.paper.position) return Rps::PAPER;
  if (targetPos == opponent.scissors.position) return Rps::SCISSORS;
  return Rps::RPS_NONE;
}

EndState checkForEndState(glm::vec3 const &targetPos, int activePlayer, PlayerTiles const players[2], bool positionReset) {
  // pass on no active player
  if (activePlayer == 99) return EndState::CONTINUE;
  // condition 1: player 1 crossed into victory zone
  if (activePlayer == 0 && targetPos.x == 200.0f) return EndState::P1_WIN;
  // condition 2: player 2 is out of pieces
  if (players[1].tilesLeft < 1) return EndState::P1_WIN;
  // condition 3: player 2 crossed into victory zone
  if (activePlayer == 1 && targetPos.x == -200.0f) return EndState::P2_WIN;
  // condition 4: player 1 is out of pieces
  if (players[0].tilesLeft < 1) return EndState::P2_WIN;
  // if no one won:
  if (!positionReset && activePlayer == 0) return EndState::P2_TURN;
  if (!positionReset && activePlayer == 1) return EndState::P1_TURN;
  // retake turn
  return EndState::CONTINUE;
}

#pragma endregion helpers

BoardScene::BoardScene(SDL_GPUDevice *gpu, SDL_GPUTextureFormat targetFormat, TTF_TextEngine *textEngine) : Scene() {
  // initialize gpu pipeline
  renderer = new Gfx::BasicRenderer(targetFormat, gpu, Gfx::PT_Triangle, SDL_GPU_CULLMODE_BACK, glm::vec2(800.0f, 600.0f));
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

  SDL_Log("Game start");
}

UpdateResult BoardScene::update(SystemUpdates const &sys) {
  UpdateResult res;
  renderer->updateWindowSize(sys.winSize);
  // handle inputs
  glm::vec2 cursorPos = getCursorWorldSpace(sys.mousePosScreenSpace, sys.winSize);
  Rps rps = findMouseOverRps(cursorPos, players[activePlayer]);
  // change cursor on hover
  if (rps != Rps::RPS_NONE || resetBtn.isCoordInsideTile(cursorPos)) {
    res.cursorStyle = CursorType::C_POINTER;
  }
  // 1. find active file
  if (sys.mouseClickState == MouseClickState::M_DOWN && activeTile == NULL) {
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
  else if (sys.mouseClickState == MouseClickState::M_DOWN && activeTile != NULL) {
    glm::vec3 targetPos = glm::vec3(cursorPos.x, cursorPos.y, 1.0);
    activeTile->position = targetPos;
    Gfx::RenderObject& obj = renderer->getMutableObject(activeTile->objectId);
    // render the active object above other tiles
    obj.pos = glm::vec3(targetPos.x, targetPos.y, 2.0);
  }
  // 3. handle dropping of active tile
  else if (sys.mouseClickState == MouseClickState::M_UP && activeTile != NULL) {
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
      bool positionReset = false;
      // check if tile moved
      if (targetPos == activeTileStartingPos) {
        positionReset = true;
      }
      // prevent collision with own tiles
      else if (isSelfColliding(players[activePlayer], activeTile->objectId, targetPos)) {
        SDL_Log("Colliding with self - resetting position");
        targetPos = activeTileStartingPos;
        positionReset = true;
      }
      // prevent moving to invalid position
      else if (glm::length(targetPos - activeTileStartingPos) > 100.0f) {
        SDL_Log("Invalid target position (%.2f, %.2f, %.2f) -> (%.2f, %.2f, %.2f)",
          activeTileStartingPos.x, activeTileStartingPos.y, activeTileStartingPos.z,
          targetPos.x, targetPos.y, targetPos.z
        );
        targetPos = activeTileStartingPos;
        positionReset = true;
      }
      // resolve collision with opponent tiles
      Rps opp = opponentAtTargetPos(targetPos, players[inactivePlayer]);
      if (opp != Rps::RPS_NONE) {
        switch (rps) {
          case Rps::ROCK:
            if (opp == Rps::ROCK) {
              targetPos = activeTileStartingPos;
              positionReset = true;
            } else if (opp == Rps::PAPER) {
              deactivateTile(activeTile, renderer, players[activePlayer].tilesLeft);
            } else if (opp == Rps::SCISSORS) {
              deactivateTile(&players[inactivePlayer].scissors, renderer, players[inactivePlayer].tilesLeft);
            }
            break;
          case Rps::PAPER:
            if (opp == Rps::ROCK) {
              deactivateTile(&players[inactivePlayer].rock, renderer, players[inactivePlayer].tilesLeft);
            } else if (opp == Rps::PAPER) {
              targetPos = activeTileStartingPos;
              positionReset = true;
            } else if (opp == Rps::SCISSORS) {
              deactivateTile(activeTile, renderer, players[activePlayer].tilesLeft);
            }
            break;
          case Rps::SCISSORS:
            if (opp == Rps::ROCK) {
              deactivateTile(activeTile, renderer, players[activePlayer].tilesLeft);
            } else if (opp == Rps::PAPER) {
              deactivateTile(&players[inactivePlayer].paper, renderer, players[inactivePlayer].tilesLeft);
            } else if (opp == Rps::SCISSORS) {
              targetPos = activeTileStartingPos;
              positionReset = true;
            }
            break;
          default:
            break;
        }
      }
      // move activeTile to new tile
      activeTile->position = targetPos;
      Gfx::RenderObject& obj = renderer->getMutableObject(activeTile->objectId);
      obj.pos = targetPos;
      // check for end state
      switch (checkForEndState(targetPos, activePlayer, players, positionReset)) {
        case EndState::P1_TURN:
          updateDisplayText(msgDisplay, renderer, "Player 1's turn");
          activePlayer = 0;
          break;
        case EndState::P2_TURN:
          updateDisplayText(msgDisplay, renderer, "Player 2's turn");
          activePlayer = 1;
          break;
        case EndState::P1_WIN:
          SDL_Log("Player 1 wins");
          updateDisplayText(msgDisplay, renderer, "Player 1 WINS!");
          activePlayer = 99;
          break;
        case EndState::P2_WIN:
          SDL_Log("Player 2 wins");
          updateDisplayText(msgDisplay, renderer, "Player 2 WINS!");
          activePlayer = 99;
          break;
        case EndState::CONTINUE:
        default:
          break;
      }
    }
    activeTile = NULL;
  }
  // 4. handle UI interaction
  else if (sys.mouseClickState == MouseClickState::M_UP && activeTile == NULL) {
    // check for reset btn click
    if (resetBtn.isCoordInsideTile(cursorPos)) {
      updateDisplayText(msgDisplay, renderer, "Player 1's turn");
      resetGameState();
    }
  }

  return res;
}

SDL_AppResult BoardScene::render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* screen) {
  // render gpu pipeline
  renderer->render(cmdBuf, screen);

  return SDL_APP_CONTINUE;
}

void BoardScene::resetGameState() {
  // reset player 1
  players[0].rock.position = playerPositions[0];
  Gfx::RenderObject& rock1 = renderer->getMutableObject(players[0].rock.objectId);
  rock1.pos = playerPositions[0];
  rock1.visible = true;

  players[0].paper.position = playerPositions[1];
  Gfx::RenderObject& paper1 = renderer->getMutableObject(players[0].paper.objectId);
  paper1.pos = playerPositions[1];
  paper1.visible = true;

  players[0].scissors.position = playerPositions[2];
  Gfx::RenderObject& scissors1 = renderer->getMutableObject(players[0].scissors.objectId);
  scissors1.pos = playerPositions[2];
  scissors1.visible = true;

  // reset player 2
  players[1].rock.position = playerPositions[3];
  Gfx::RenderObject& rock2 = renderer->getMutableObject(players[1].rock.objectId);
  rock2.pos = playerPositions[3];
  rock2.visible = true;

  players[1].paper.position = playerPositions[4];
  Gfx::RenderObject& paper2 = renderer->getMutableObject(players[1].paper.objectId);
  paper2.pos = playerPositions[4];
  paper2.visible = true;

  players[1].scissors.position = playerPositions[5];
  Gfx::RenderObject& scissors2 = renderer->getMutableObject(players[1].scissors.objectId);
  scissors2.pos = playerPositions[5];
  scissors2.visible = true;

  // reset active player
  activePlayer = 0;
  activeTile = NULL;
  SDL_Log("Reset board state");
}

void BoardScene::destroy() {
  // cleanup resources
  renderer->destroy();
  SDL_free(renderer);
}
