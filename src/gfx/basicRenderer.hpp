#pragma once

#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "util.hpp"

namespace Gfx {
  // Basic vertex based renderer with flat coloring (no shadows)
  class BasicRenderer {
  public:
    BasicRenderer(
      SDL_GPUTextureFormat targetFormat,
      SDL_GPUDevice *gpu,
      Gfx::GPUPrimitiveType type,
      SDL_GPUCullMode cullMode,
      glm::vec2 winSize
    );
    // the targetFormat need to match the format of the text textures
    void enableTextGeneration(
      SDL_GPUTextureFormat targetFormat,
      TTF_TextEngine *textEngine,
      std::string fontPath,
      float fontSize
    );
    void resizeCanvas(glm::vec2 const &winSize);
    // this MUST be called in the update step - the internal renderer window size
    // needs to be kept consistent with the actual window size
    void updateWindowSize(glm::vec2 const &winSize);
    int addObject(std::vector<Gfx::RenderVertex> const &vertices);
    int addObject(std::vector<Gfx::RenderVertex> const &vertices, std::vector<Uint16> const &indices);
    int addObject(Gfx::Primitive const &shape);
    // do not run this within a render pass
    void updateObjectModel(int id, std::vector<Gfx::RenderVertex> const &vertices, std::vector<Uint16> const &indices);
    void updateObjectTexture(int id, SDL_GPUTexture *texture, glm::vec2 textureSize);
    void clearObjectTexture(int id);
    void swapObjectTextures(int id1, int id2);
    // remember to dispose of this texture after use
    // (attaching a texture to a RenderObject will automatically dispose)
    // do not run this within a render pass
    SDL_GPUTexture* createTextTexture(
      std::string text,
      glm::vec3 pos,
      SDL_FColor textColor,
      SDL_FColor backgroundColor,
      SDL_GPUTextureFormat textureFormat,
      glm::vec2 textureSize
    );
    Gfx::RenderObject& getMutableObject(int id);
    void render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* target);
    void clearAllObjectAssets();
    void destroy();
    Gfx::RenderCamera cam;
    SDL_FColor clearColor = SDL_FColor{ 0.02f, 0.02f, 0.08f, 1.0f };
  private:
    glm::vec2 winSizeCache = glm::vec2(0.0f);
    std::vector<Gfx::RenderObject> renderObjects;
    SDL_GPUDevice *device = NULL;
    SDL_GPUGraphicsPipeline *pipeline = NULL;
    SDL_GPUTexture *depthTx = NULL;
    // text assets
    bool textEnabled = false;
    TTF_TextEngine *textEngine = NULL; // reference only: do not close from here
    TTF_Font *textFont = NULL;
    SDL_GPUSampler *textSampler = NULL;
    SDL_GPUGraphicsPipeline *textPipeline = NULL;
  };
}