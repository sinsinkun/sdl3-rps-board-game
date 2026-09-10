#pragma once

#include <vector>
#include <SDL3/SDL.h>

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
      Uint32 sw,
      Uint32 sh
    );
    void resizeScreen(Uint32 w, Uint32 h);
    int addObject(std::vector<Gfx::RenderVertex> const &vertices);
    int addObject(std::vector<Gfx::RenderVertex> const &vertices, std::vector<Uint16> const &indices);
    int addObject(Gfx::Primitive const &shape);
    void addTextureToObject(int id, SDL_GPUTexture *texture);
    Gfx::RenderObject& getMutableObject(int id);
    void render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* target);
    void clearAllObjectAssets();
    void destroy();
    Gfx::RenderCamera cam;
    SDL_FColor clearColor = SDL_FColor{ 0.02f, 0.02f, 0.08f, 1.0f };
  private:
    std::vector<Gfx::RenderObject> renderObjects;
    SDL_GPUDevice *device = NULL;
    SDL_GPUGraphicsPipeline *pipeline = NULL;
    SDL_GPUTexture *depthTx = NULL;
  };
}