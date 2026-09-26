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
      Uint32 screenWidth,
      Uint32 screenHeight
    );
    void enableTextGeneration(
      SDL_GPUTextureFormat targetFormat,
      TTF_TextEngine *textEngine,
      std::string fontPath,
      float fontSize
    );
    void resizeScreen(Uint32 w, Uint32 h);
    int addObject(std::vector<Gfx::RenderVertex> const &vertices);
    int addObject(std::vector<Gfx::RenderVertex> const &vertices, std::vector<Uint16> const &indices);
    int addObject(Gfx::Primitive const &shape);
    void addTextureToObject(int id, SDL_GPUTexture *texture, glm::vec2 textureSize);
    // this adds a texture to an existing render object on which the text exists
    void addTextToObject(
      int objectId,
      std::string text,
      glm::vec3 pos,
      SDL_FColor color,
      SDL_GPUTextureFormat textureFormat, 
      Uint32 textureWidth,
      Uint32 textureHeight
    );
    Gfx::RenderObject& getMutableObject(int id);
    // this only needs to run when text is updated, not on every render cycle
    void renderTextsToObjTextures();
    void render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* target);
    void clearAllObjectAssets();
    void clearAllTextAssets();
    void destroy();
    Gfx::RenderCamera cam;
    SDL_FColor clearColor = SDL_FColor{ 0.02f, 0.02f, 0.08f, 1.0f };
  private:
    std::vector<Gfx::RenderObject> renderObjects;
    SDL_GPUDevice *device = NULL;
    SDL_GPUGraphicsPipeline *pipeline = NULL;
    SDL_GPUTexture *depthTx = NULL;
    // text assets
    bool textEnabled = false;
    TTF_TextEngine *textEngine = NULL; // reference only: do not close from here
    TTF_Font *textFont = NULL;
    std::vector<Gfx::RenderText> renderTexts;
    SDL_GPUGraphicsPipeline *textPipeline = NULL;
  };
}