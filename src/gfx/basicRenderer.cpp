#include "basicRenderer.hpp"

using namespace Gfx;

BasicRenderer::BasicRenderer(
  SDL_GPUTextureFormat targetFormat,
  SDL_GPUDevice *gpu,
  GPUPrimitiveType type,
  SDL_GPUCullMode cullMode,
  Uint32 sw,
  Uint32 sh
) {
  device = gpu;
  // create shaders
  SDL_GPUShader *vertShader = Gfx::loadShader(device, "obj.vert", 0, 1, 0, 0);
  SDL_GPUShader *fragShader = Gfx::loadShader(device, "obj-basic.frag", 1, 1, 0, 0);

  // change render type
  SDL_GPUPrimitiveType primType = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
  SDL_GPUFillMode fillMode = SDL_GPU_FILLMODE_FILL;
  if (type == PT_Point) {
    primType = SDL_GPU_PRIMITIVETYPE_POINTLIST;
  }
  if (type == PT_Line) {
    primType = SDL_GPU_PRIMITIVETYPE_LINELIST;
    fillMode = SDL_GPU_FILLMODE_LINE;
  }

  // create pipeline
	pipeline = SDL_CreateGPUGraphicsPipeline(device, new SDL_GPUGraphicsPipelineCreateInfo {
		.vertex_shader = vertShader,
		.fragment_shader = fragShader,
    .vertex_input_state = createVertexInputState(),
		.primitive_type = primType,
		.rasterizer_state = SDL_GPURasterizerState {
			.fill_mode = fillMode,
			.cull_mode = cullMode,
		},
    .depth_stencil_state = SDL_GPUDepthStencilState {
      .compare_op = SDL_GPU_COMPAREOP_LESS,
      .write_mask = 0xFF,
      .enable_depth_test = true,
      .enable_depth_write = true,
    },
		.target_info = SDL_GPUGraphicsPipelineTargetInfo {
			.color_target_descriptions = new SDL_GPUColorTargetDescription {
				.format = targetFormat,
				.blend_state = SDL_GPUColorTargetBlendState {
					.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
					.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
					.color_blend_op = SDL_GPU_BLENDOP_ADD,
					.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
					.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
					.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
					.enable_blend = true,
				},
			},
			.num_color_targets = 1,
      .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
      .has_depth_stencil_target = true,
		},
	});

  // create depth texture
  depthTx = SDL_CreateGPUTexture(device, new SDL_GPUTextureCreateInfo {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
    .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
    .width = sw,
    .height = sh,
    .layer_count_or_depth = 1,
    .num_levels = 1,
  });

  // release shaders
	SDL_ReleaseGPUShader(device, vertShader);
  SDL_ReleaseGPUShader(device, fragShader);
}

void BasicRenderer::enableTextGeneration(
  SDL_GPUTextureFormat targetFormat,
  TTF_TextEngine *textEngineInput,
  std::string fontPath,
  float fontSize
) {
  textEnabled = true;
  textEngine = textEngineInput;

  // TODO: replace with text specific shaders
  SDL_GPUShader *vertShader = Gfx::loadShader(device, "obj.vert", 0, 1, 0, 0);
  SDL_GPUShader *fragShader = Gfx::loadShader(device, "obj-basic.frag", 1, 1, 0, 0);

  // instantiate text pipeline
  textPipeline = SDL_CreateGPUGraphicsPipeline(device, new SDL_GPUGraphicsPipelineCreateInfo {
    .vertex_shader = vertShader,
    .fragment_shader = fragShader,
    .vertex_input_state = createVertexInputState(),
    .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
    .target_info = {
      .color_target_descriptions = new SDL_GPUColorTargetDescription {
        .format = targetFormat,
        .blend_state = SDL_GPUColorTargetBlendState {
          .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
          .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
          .color_blend_op = SDL_GPU_BLENDOP_ADD,
          .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
          .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
          .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
          .enable_blend = true,
        },
      },
      .num_color_targets = 1,
      .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID, /* Need to set this to avoid missing initializer for field error */
      .has_depth_stencil_target = false,
    },
  });

  // load font
  textFont = TTF_OpenFont(fontPath.c_str(), fontSize);
  if (textFont == NULL) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
  } else {
    SDL_Log("Opened font \"%s\"", fontPath.c_str());
  }

  SDL_Log("Enabled text generation in BasicRenderer");

  // release shaders
	SDL_ReleaseGPUShader(device, vertShader);
  SDL_ReleaseGPUShader(device, fragShader);
}

void BasicRenderer::resizeScreen(Uint32 w, Uint32 h) {
  SDL_ReleaseGPUTexture(device, depthTx);
  depthTx = SDL_CreateGPUTexture(device, new SDL_GPUTextureCreateInfo {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
    .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
    .width = w,
    .height = h,
    .layer_count_or_depth = 1,
    .num_levels = 1,
  });
  cam.viewWidth = (float)w;
  cam.viewHeight = (float)h;
}

int BasicRenderer::addObject(std::vector<RenderVertex> const &vertices) {
	// create vertex buffer
  Uint32 vSize = sizeof(RenderVertex) * vertices.size();
  SDL_GPUBuffer *vBuffer = SDL_CreateGPUBuffer(device, new SDL_GPUBufferCreateInfo {
    .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
    .size = vSize
  });
  // pump vertex data into transfer buffer
  SDL_GPUTransferBuffer *vertTransferBuf = SDL_CreateGPUTransferBuffer(
    device,
    new SDL_GPUTransferBufferCreateInfo {
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = vSize,
    }
  );
  RenderVertex* vertData = static_cast<RenderVertex*>(SDL_MapGPUTransferBuffer(
    device, vertTransferBuf, false
  ));
  for (int i=0; i < vertices.size(); i++) {
    vertData[i] = vertices.at(i);
  }
  SDL_UnmapGPUTransferBuffer(device, vertTransferBuf);

  // create cmd buffer + copy pass
	SDL_GPUCommandBuffer *cmdBuf = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmdBuf);
  // upload vertex buffer
  SDL_UploadToGPUBuffer(
    copyPass,
    new SDL_GPUTransferBufferLocation {
      .transfer_buffer = vertTransferBuf,
      .offset = 0,
    },
    new SDL_GPUBufferRegion {
      .buffer = vBuffer,
      .offset = 0,
      .size = vSize,
    },
    false
  );
  // clean up passes
	SDL_EndGPUCopyPass(copyPass);
	if (!SDL_SubmitGPUCommandBuffer(cmdBuf)) {
    SDL_Log("Failed to upload to buffer - %s", SDL_GetError());
  };
  // release transfer buffers
  SDL_ReleaseGPUTransferBuffer(device, vertTransferBuf);

  // create placeholder texture + sampler
  SDL_GPUTexture *tx = SDL_CreateGPUTexture(device, new SDL_GPUTextureCreateInfo {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
    .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    .width = 1,
    .height = 1,
    .layer_count_or_depth = 1,
    .num_levels = 1,
  });
  SDL_GPUSampler *sm = SDL_CreateGPUSampler(device, new SDL_GPUSamplerCreateInfo {
    .min_filter = SDL_GPU_FILTER_LINEAR,
    .mag_filter = SDL_GPU_FILTER_LINEAR,
    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
  });

  // create object
  int id = renderObjects.size();
  renderObjects.push_back(RenderObject {
    .id = id,
    .visible = true,
    .vertexBuffer = vBuffer,
    .vertexCount = (int)(vertices.size()),
    .indexCount = 0,
    .sampler = sm,
    .texture = tx,
  });
  return id;
}

int BasicRenderer::addObject(std::vector<RenderVertex> const &vertices, std::vector<Uint16> const &indices) {
  // create vertex buffer
  Uint32 vSize = sizeof(RenderVertex) * vertices.size();
  SDL_GPUBuffer *vBuffer = SDL_CreateGPUBuffer(device, new SDL_GPUBufferCreateInfo {
    .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
    .size = vSize
  });
  // create index buffer
  Uint32 iSize = sizeof(Uint16) * indices.size();
  SDL_GPUBuffer *iBuffer = SDL_CreateGPUBuffer(device, new SDL_GPUBufferCreateInfo {
    .usage = SDL_GPU_BUFFERUSAGE_INDEX,
    .size = iSize
  });

  // pump vertex data into transfer buffer
  SDL_GPUTransferBuffer *vertTransferBuf = SDL_CreateGPUTransferBuffer(
    device,
    new SDL_GPUTransferBufferCreateInfo {
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = vSize,
    }
  );
  RenderVertex* vertData = static_cast<RenderVertex*>(SDL_MapGPUTransferBuffer(
    device, vertTransferBuf, false
  ));
  for (int i=0; i < vertices.size(); i++) {
    vertData[i] = vertices.at(i);
  }
  SDL_UnmapGPUTransferBuffer(device, vertTransferBuf);

  // pump index data into transfer buffer
  SDL_GPUTransferBuffer *idxTransferBuf = SDL_CreateGPUTransferBuffer(
    device,
    new SDL_GPUTransferBufferCreateInfo {
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = iSize,
    }
  );
  Uint16* indexData = static_cast<Uint16*>(SDL_MapGPUTransferBuffer(
    device, idxTransferBuf, false
  ));
  for (int i=0; i < indices.size(); i++) {
    indexData[i] = indices.at(i);
  }
  SDL_UnmapGPUTransferBuffer(device, idxTransferBuf);

  // create cmd buffer + copy pass
	SDL_GPUCommandBuffer *cmdBuf = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmdBuf);

  // upload vertex buffer
  SDL_UploadToGPUBuffer(
    copyPass,
    new SDL_GPUTransferBufferLocation {
      .transfer_buffer = vertTransferBuf,
      .offset = 0,
    },
    new SDL_GPUBufferRegion {
      .buffer = vBuffer,
      .offset = 0,
      .size = vSize,
    },
    false
  );
  // upload index buffer
  SDL_UploadToGPUBuffer(
    copyPass,
    new SDL_GPUTransferBufferLocation {
      .transfer_buffer = idxTransferBuf,
      .offset = 0,
    },
    new SDL_GPUBufferRegion {
      .buffer = iBuffer,
      .offset = 0,
      .size = iSize,
    },
    false
  );

  // clean up passes
	SDL_EndGPUCopyPass(copyPass);
	if (!SDL_SubmitGPUCommandBuffer(cmdBuf)) {
    SDL_Log("Failed to upload to buffers - %s", SDL_GetError());
  };
  // release transfer buffers
  SDL_ReleaseGPUTransferBuffer(device, vertTransferBuf);
  SDL_ReleaseGPUTransferBuffer(device, idxTransferBuf);

  
  // create placeholder texture + sampler
  SDL_GPUTexture *tx = SDL_CreateGPUTexture(device, new SDL_GPUTextureCreateInfo {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
    .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    .width = 1,
    .height = 1,
    .layer_count_or_depth = 1,
    .num_levels = 1,
  });
  SDL_GPUSampler *sm = SDL_CreateGPUSampler(device, new SDL_GPUSamplerCreateInfo {
    .min_filter = SDL_GPU_FILTER_LINEAR,
    .mag_filter = SDL_GPU_FILTER_LINEAR,
    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
  });

  // create object
  int id = renderObjects.size();
  renderObjects.push_back(RenderObject {
    .id = id,
    .visible = true,
    .vertexBuffer = vBuffer,
    .indexBuffer = iBuffer,
    .vertexCount = (int)(vertices.size()),
    .indexCount = (int)(indices.size()),
    .sampler = sm,
    .texture = tx,
  });
  return id;
}

int BasicRenderer::addObject(Primitive const &shape) {
  if (shape.useIndices) {
    return addObject(shape.vertices, shape.indices);
  }
  return addObject(shape.vertices);
}

void BasicRenderer::addTextureToObject(int id, SDL_GPUTexture *texture, glm::vec2 textureSize) {
  if (id < 0 || id >= renderObjects.size()) {
    SDL_Log("ERR: Tried to access render object that doesn't exist %d", id);
    return;
  }
  SDL_ReleaseGPUTexture(device, renderObjects.at(id).texture);
  renderObjects.at(id).texture = texture;
  renderObjects.at(id).textureSize = textureSize;
}

void addGlyphToVertices(
	TTF_GPUAtlasDrawSequence *sequence,
	std::vector<RenderVertex> *vertices,
	std::vector<Uint16> *indices,
	SDL_FColor color,
	glm::vec3 origin
) {
  for (int i=0; i < sequence->num_vertices; i++) {
		RenderVertex vert;
		const SDL_FPoint pos = sequence->xy[i];
		const SDL_FPoint uv = sequence->uv[i];
		vert.pos.x = origin.x + pos.x;
    vert.pos.y = pos.y - origin.y;
    vert.pos.z = origin.z;
		vert.uv.x = uv.x;
    vert.uv.y = uv.y;
		vertices->push_back(vert);
	}
	for (int i=0; i < sequence->num_indices; i++) {
		indices->push_back(sequence->indices[i]);
	}
}

void BasicRenderer::addTextToObject(
  int id,
  std::string text,
  glm::vec3 pos,
  SDL_FColor color,
  SDL_GPUTextureFormat textureFormat, 
  Uint32 textureWidth,
  Uint32 textureHeight
) {
  if (textEngine == NULL || textFont == NULL) {
    SDL_Log("Missing requirement to create ttfText (%p, %p)", textEngine, textFont);
    return;
  }
  TTF_Text *ttfText = TTF_CreateText(textEngine, textFont, text.c_str(), 0);
  if (ttfText == NULL) {
    SDL_Log("Something went wrong while creating text: %s", SDL_GetError());
    return;
  } else {
    SDL_Log("Created TTF_Text \"%s\"", ttfText->text);
  }
  RenderText renderText = RenderText {
    .parentObjectId = id,
    .text = text,
    .pos = pos,
    .color = color,
    .ttfText = ttfText
  };
  
  // update texture size
  SDL_GPUTexture *textTexture = SDL_CreateGPUTexture(device, new SDL_GPUTextureCreateInfo {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = textureFormat,
    .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    .width = textureWidth,
    .height = textureHeight,
    .layer_count_or_depth = 1,
    .num_levels = 1,
  });
  addTextureToObject(id, textTexture, glm::vec2((float)textureWidth, (float)textureHeight));
  SDL_Log("Created texture for text");

  // generate vertex/index buffers
  TTF_GPUAtlasDrawSequence *sequence = TTF_GetGPUTextDrawData(ttfText); // -- BROKEN
  if (sequence == NULL) {
    SDL_Log("Something went wrong while acquiring the text sequence: %s", SDL_GetError());
    return;
  }
  std::vector<RenderVertex> vertices;
  std::vector<Uint16> indices;
  for (TTF_GPUAtlasDrawSequence *seq = sequence; seq != NULL; seq = seq->next) {
    addGlyphToVertices(seq, &vertices, &indices, renderText.color, renderText.pos);
  }
  renderText.vertexBuffer = SDL_CreateGPUBuffer(device, new SDL_GPUBufferCreateInfo {
    .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
    .size = (Uint32)(sizeof(RenderVertex) * vertices.size()),
  });
  renderText.indexBuffer = SDL_CreateGPUBuffer(device, new SDL_GPUBufferCreateInfo {
    .usage = SDL_GPU_BUFFERUSAGE_INDEX,
    .size = (Uint32)(sizeof(Uint16) * indices.size()),
  });
  SDL_Log("Added glyphs to vertices");
  copyVertexDataIntoBuffer(device, renderText.vertexBuffer, renderText.indexBuffer, &vertices, &indices);
  renderText.vertexCount = vertices.size();
  renderText.indexCount = indices.size();
  SDL_Log("Copied data into buffers");

  // add to list
  renderTexts.push_back(renderText);
}

RenderObject& BasicRenderer::getMutableObject(int id) {
  return renderObjects.at(id);
}

void BasicRenderer::renderTextsToObjTextures(SDL_GPUCommandBuffer *cmdBuf) {
  // each RenderText needs its own pass
  for (RenderText renderText : renderTexts) {
    // select target
    SDL_GPUTexture *target = NULL;
    if (renderText.parentObjectId < 0 || renderObjects.size() < renderText.parentObjectId) {
      SDL_Log("Trying to access non-existant parentObjectId (%i) - Skipping text render", renderText.parentObjectId);
      continue;
    }
    target = renderObjects.at(renderText.parentObjectId).texture;
    SDL_GPUSampler *sampler = renderObjects.at(renderText.parentObjectId).sampler;
    glm::vec2 targetSize = renderObjects.at(renderText.parentObjectId).textureSize;
    if (target == NULL) {
      SDL_Log("Texture target not found");
      continue;
    }
    // build render pass
    SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(cmdBuf, new SDL_GPUColorTargetInfo {
      .texture = target,
      .clear_color = Gfx::TRANSPARENT,
      .load_op = SDL_GPU_LOADOP_LOAD,
      .store_op = SDL_GPU_STOREOP_STORE,
    }, 1, NULL);

    // setup pipeline
    TTF_GPUAtlasDrawSequence *sequence = TTF_GetGPUTextDrawData(renderText.ttfText);
    SDL_GPUTexture *atlas = sequence->atlas_texture;
    SDL_BindGPUGraphicsPipeline(pass, textPipeline);
    SDL_BindGPUFragmentSamplers(pass, 0, new SDL_GPUTextureSamplerBinding {
      .texture = atlas,
      .sampler = sampler
    }, 1);
    SDL_BindGPUVertexBuffers(pass, 0, new SDL_GPUBufferBinding {
      .buffer = renderText.vertexBuffer,
      .offset = 0,
    }, 1);
    SDL_BindGPUIndexBuffer(pass, new SDL_GPUBufferBinding {
      .buffer = renderText.indexBuffer,
      .offset = 0,
    }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
  
    SDL_PushGPUVertexUniformData(cmdBuf, 0, &targetSize, sizeof(glm::vec2));
    // dynamically offset buffers for each glyph
    int index_offset = 0, vertex_offset = 0;
    SDL_PushGPUFragmentUniformData(cmdBuf, 0, &renderText.color, sizeof(SDL_FColor));
    for (TTF_GPUAtlasDrawSequence *seq = sequence; seq != NULL; seq = seq->next) {
      SDL_DrawGPUIndexedPrimitives(pass, seq->num_indices, 1, index_offset, vertex_offset, 0);
      index_offset += seq->num_indices;
      vertex_offset += seq->num_vertices;
    }

    SDL_EndGPURenderPass(pass);
  }
}

void BasicRenderer::render(SDL_GPUCommandBuffer *cmdBuf, SDL_GPUTexture* target) {
  SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(cmdBuf, new SDL_GPUColorTargetInfo {
		.texture = target,
		.clear_color = clearColor,
		.load_op = SDL_GPU_LOADOP_LOAD,
		.store_op = SDL_GPU_STOREOP_STORE,
	}, 1, new SDL_GPUDepthStencilTargetInfo {
    .texture = depthTx,
    .clear_depth = 1,
    .load_op = SDL_GPU_LOADOP_CLEAR,
    .store_op = SDL_GPU_STOREOP_STORE,
  });
  SDL_BindGPUGraphicsPipeline(pass, pipeline);
  // build view/proj matrices early
  glm::mat4x4 view = viewMatrix(cam);
  glm::mat4x4 proj = projMatrix(cam);

  // handle each object separately
  for (RenderObject const &obj : renderObjects) {
    if (!obj.visible) continue;
    if (obj.vertexBuffer == NULL) {
      SDL_Log("ERR: Missing vertex data for object %d", obj.id);
      continue;
    }
    SDL_BindGPUVertexBuffers(pass, 0, new SDL_GPUBufferBinding {
      .buffer = obj.vertexBuffer,
      .offset = 0,
    }, 1);
    // build matrices
    glm::mat4x4 matrices[3] = { modelMatrix(obj), view, proj };
    SDL_PushGPUVertexUniformData(cmdBuf, 0, &matrices, sizeof(matrices));
    // upload texture
    SDL_BindGPUFragmentSamplers(pass, 0, new SDL_GPUTextureSamplerBinding {
      .texture = obj.texture,
      .sampler = obj.sampler
    }, 1);
    // upload material
    SDL_PushGPUFragmentUniformData(cmdBuf, 0, &obj.albedo, sizeof(glm::vec4));
    // draw
    if (obj.indexCount > 0) {
      SDL_BindGPUIndexBuffer(pass, new SDL_GPUBufferBinding {
        .buffer = obj.indexBuffer,
        .offset = 0,
      }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
      SDL_DrawGPUIndexedPrimitives(pass, obj.indexCount, 1, 0, 0, 0);
    } else {
      SDL_DrawGPUPrimitives(pass, obj.vertexCount, 1, 0, 0);
    }
  }
  // end pass
  SDL_EndGPURenderPass(pass);
}

void BasicRenderer::clearAllObjectAssets() {
  for (int i=0; i<renderObjects.size(); i++) {
    if (renderObjects[i].vertexBuffer != NULL) SDL_ReleaseGPUBuffer(device, renderObjects[i].vertexBuffer);
    if (renderObjects[i].indexBuffer != NULL) SDL_ReleaseGPUBuffer(device, renderObjects[i].indexBuffer);
    if (renderObjects[i].texture != NULL) SDL_ReleaseGPUTexture(device, renderObjects[i].texture);
    if (renderObjects[i].sampler != NULL) SDL_ReleaseGPUSampler(device, renderObjects[i].sampler);
  }
  renderObjects.clear();
}

void BasicRenderer::clearAllTextAssets() {
  for (int i=0; i<renderTexts.size(); i++) {
    if (renderTexts[i].ttfText != NULL) TTF_DestroyText(renderTexts[i].ttfText);
    if (renderTexts[i].vertexBuffer != NULL) SDL_ReleaseGPUBuffer(device, renderTexts[i].vertexBuffer);
    if (renderTexts[i].indexBuffer != NULL) SDL_ReleaseGPUBuffer(device, renderTexts[i].indexBuffer);
  }
}

void BasicRenderer::destroy() {
  clearAllObjectAssets();
  SDL_ReleaseGPUTexture(device, depthTx);
  SDL_ReleaseGPUGraphicsPipeline(device, pipeline);

  if (textEnabled) {
    clearAllTextAssets();
    TTF_CloseFont(textFont);
    SDL_ReleaseGPUGraphicsPipeline(device, textPipeline);
  }
}
