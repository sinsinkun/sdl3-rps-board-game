#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL_main.h>

#include "app.hpp"
#include "boardScene.hpp"

using namespace App;

// helper to wrap default SDL3 system initialization
SDL_AppResult initSDLSystems(AppState& state) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log(
    "SDL3 v%d.%d.%d initialized\n",
    SDL_VERSIONNUM_MAJOR(SDL_VERSION),
    SDL_VERSIONNUM_MINOR(SDL_VERSION),
    SDL_VERSIONNUM_MICRO(SDL_VERSION)
  );

  state.window = SDL_CreateWindow("SDL3 Vulkan", 800, 600, SDL_WINDOW_RESIZABLE);
  if (!state.window) {
    SDL_Log("SDL_CreateWindow() failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("Window initialized");
  SDL_SetWindowMinimumSize(state.window, 400, 300);

  // can add other shader formats: SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL
  state.gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
  if (!state.gpu) {
    SDL_Log("SDL_CreateGPUDevice() failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("GPU device initialized: %s", SDL_GetGPUDeviceDriver(state.gpu));

  bool claimed = SDL_ClaimWindowForGPUDevice(state.gpu, state.window);
  if (!claimed) {
    SDL_Log("SDL_ClaimWindowForGPUDevice() failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("Claimed window for GPU device");

  // load icon
  state.winIcon = IMG_Load("assets/icon.png");
  SDL_SetWindowIcon(state.window, state.winIcon);

  // initialize text engine
  if (!TTF_Init()) {
    SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  };
  state.textEngine = TTF_CreateGPUTextEngine(state.gpu);
  if (state.textEngine == NULL) {
    SDL_Log("Failed to create text engine: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("Started text engine");

  SDL_Log("-- Successfully initialized SDL3 systems --");
  return SDL_APP_CONTINUE;
}

// initialization of app
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("RPS Board Game", "0.1", "com.example.rps-board");
  *appstate = new AppState;
  AppState& state = *static_cast<AppState*>(*appstate);
  if (initSDLSystems(state) != SDL_APP_CONTINUE) {
    return SDL_APP_FAILURE;
  }

  // load scene into memory
  SDL_GPUTextureFormat scFormat = SDL_GetGPUSwapchainTextureFormat(state.gpu, state.window);
  BoardScene *scene1 = new BoardScene(state.gpu, scFormat, state.textEngine);
  state.scenes.push_back(scene1);

  return SDL_APP_CONTINUE;
}

// handle events (note: these happen asynchronously from the iterate loop)
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState& state = *static_cast<AppState*>(appstate);
  switch (event->type) {
    // triggers on last window close and other things. End the program.
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_RESIZED:
      state.sys.winSize.x = event->window.data1;
      state.sys.winSize.y = event->window.data2;
      break;
    case SDL_EVENT_KEY_DOWN:
      if (state.sys.keysHeld.find(event->key.key) == state.sys.keysHeld.end()) {
        state.sys.keysPressed.emplace(event->key.key);
        state.sys.keysHeld.emplace(event->key.key);
      }
      break;
    case SDL_EVENT_KEY_UP:
      state.sys.keysHeld.erase(event->key.key);
      break;
    case SDL_EVENT_MOUSE_MOTION:
      state.sys.mousePosScreenSpace = glm::vec2(event->motion.x, event->motion.y);
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      state.sys.mouseClickState = MouseClickState::DOWN;
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      state.sys.mouseClickState = MouseClickState::UP;
      break;
    default:
      break;
  }
  return SDL_APP_CONTINUE;
}

// update/render loop
SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState& state = *static_cast<AppState*>(appstate);

  // ---------------------------------------------
  // Update logic
  // ---------------------------------------------

  Uint64 newTime = SDL_GetTicksNS();
  Uint64 delta = newTime - state.sys.lifetime;

  // forced frame cap
  int maxDelta = 1000000000 / App::MAX_FPS;
  if (delta < maxDelta) return SDL_APP_CONTINUE;

  // calculate FPS
  state.sys.lifetime = newTime;
  state.sys.deltaTime = (float)delta / (float)SDL_NS_PER_SECOND;
  state.sys.timeSinceLastFps += delta;
  if (state.sys.timeSinceLastFps >= SDL_NS_PER_SECOND) {
    float fps = (state.sys.deltaTime > 0.0f) ? (1.0f / state.sys.deltaTime) : 0.0f;
    // subtract to preserve overflow and prevent time drift
    state.sys.timeSinceLastFps -= SDL_NS_PER_SECOND;
    SDL_Log("FPS: %.2f (Scene %d)", fps, state.currentScene);
  }

  // update scene
  if (state.scenes.size() > 0 && state.currentScene > -1) {
    if (state.currentScene > state.scenes.size()) {
      SDL_Log("Tried to access scene index greater than scenes length");
      return SDL_APP_CONTINUE;
    }
    SDL_AppResult res = state.scenes.at(state.currentScene)->update(state.sys);
    if (res != SDL_APP_CONTINUE) return res;
  }

  // events are asynchronous, so we have to clean up keysPressed per frame
  for (SDL_Keycode key : state.sys.keysPressed) {
    if (state.sys.keysHeld.find(key) != state.sys.keysHeld.end()) {
      state.sys.keysPressed.erase(key);
    }
  }
  if (state.sys.mouseClickState == MouseClickState::UP) {
    state.sys.mouseClickState = MouseClickState::NONE;
  }

  // ---------------------------------------------
  // Render logic
  // ---------------------------------------------

  // acquire command buffer
	SDL_GPUCommandBuffer *cmdBuf = SDL_AcquireGPUCommandBuffer(state.gpu);
  SDL_InsertGPUDebugLabel(cmdBuf, "Screen Render");
	// acquire swapchain
	SDL_GPUTexture* swapchain = NULL;
	SDL_AcquireGPUSwapchainTexture(cmdBuf, state.window, &swapchain, NULL, NULL);
	if (swapchain == NULL) {
		// if swapchain == NULL, its not ready yet - skip render
		SDL_CancelGPUCommandBuffer(cmdBuf);
		return SDL_APP_CONTINUE;
	}

  // render scene
  if (state.scenes.size() > 0 && state.currentScene > -1) {
    if (state.currentScene > state.scenes.size()) {
      SDL_Log("Tried to access scene index greater than scenes length");
      return SDL_APP_CONTINUE;
    }
    SDL_AppResult res = state.scenes.at(state.currentScene)->render(cmdBuf, swapchain);
    if (res != SDL_APP_CONTINUE) {
      SDL_CancelGPUCommandBuffer(cmdBuf);
      return res;
    }
  }

  // end render chain
	if (!SDL_SubmitGPUCommandBuffer(cmdBuf)) {
		SDL_Log("Failed to submit GPU command %s", SDL_GetError());
		return SDL_APP_FAILURE;
	};

  return SDL_APP_CONTINUE;
}

// clean up on exit
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState& state = *static_cast<AppState*>(appstate);
  SDL_Log("Closing SDL3");

  // destroy scenes
  for (Scene* &scene : state.scenes) {
    scene->destroy();
    SDL_free(scene);
  }
  state.scenes.clear();

  // destroy text engine
  TTF_DestroyGPUTextEngine(state.textEngine);
  TTF_Quit();

  SDL_ReleaseWindowFromGPUDevice(state.gpu, state.window);
  SDL_DestroyGPUDevice(state.gpu);
  SDL_DestroySurface(state.winIcon);
  SDL_DestroyWindow(state.window);

  SDL_Quit();
}
