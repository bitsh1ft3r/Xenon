// Copyright 2025 Xenon Emulator Project

#pragma once

#include <thread>
#include <vector>
#include <fstream>

#include <SDL3/SDL.h>

#define GL_GLEXT_PROTOTYPES
extern "C" {
#include <KHR/khrplatform.h>
#include <glad/glad.h>
}

#include "Base/Types.h"
#include "Core/RAM/RAM.h"
#include "Core/RootBus/HostBridge/PCIe.h"
#include "Render/Abstractions/Texture.h"
   
// ARGB (Console is BGRA)
#define COLOR(r, g, b, a) ((a) << 24 | (r) << 16 | (g) << 8  | (b) << 0)
#define TILE(x) ((x + 31) >> 5) << 5


namespace Render {

class Renderer {
public:
  Renderer(RAM* ram);
  ~Renderer();
  void Start();
  void Shutdown();

  void Resize(int x, int y);

  void Thread();

  RAM* ramPointer{};
  u8 *fbPointer{};

  // Vali0004: This may need to be in XGPU
  // Initial Internal rendering width/height.
  u32 internalWidth = 1280;
  u32 internalHeight = 720;
private:
  // Thread handle
  std::thread thread;

  // Backbuffer texture
  std::unique_ptr<Texture> backbuffer;

  // Resolution
  u32 width = 720, height = 1280;
  // Vertical SYNC
  bool VSYNC = true;
  bool fullscreen = false;
  // Pixel buffer
  int pitch = 0;
  std::vector<u32> pixels{};
  // SDL Window data
  SDL_Window *mainWindow{};
  SDL_GLContext context;
  SDL_Event windowEvent;
  // GL Handles                                
  GLuint texture, dummyVAO, shaderProgram, pixelBuffer;
  GLuint renderShaderProgram;
};

// Shaders

inline constexpr const char* vertexShaderSource = R"(
#version 430 core

out vec2 o_texture_coord;

void main() {
  o_texture_coord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
  gl_Position = vec4(o_texture_coord * vec2(2.0f, -2.0f) + vec2(-1.0f, 1.0f), 0.0f, 1.0f);
}
)";

inline constexpr const char* fragmentShaderSource = R"(
#version 430 core

in vec2 o_texture_coord;

out vec4 o_color;

uniform usampler2D u_texture;
void main() {
  uint pixel = texture(u_texture, o_texture_coord).r;
  // Gotta love BE vs LE (X360 works in BGRA, so we work in ARGB)
  float a = float((pixel >> 24) & 0xFF) / 255.0;
  float r = float((pixel >> 16) & 0xFF) / 255.0;
  float g = float((pixel >> 8) & 0xFF) / 255.0;
  float b = float((pixel >> 0) & 0xFF) / 255.0;
  o_color = vec4(r, g, b, a);
}
)";

inline constexpr const char* computeShaderSource = R"(
#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (r32ui, binding = 0) uniform writeonly uimage2D o_texture;
layout (std430, binding = 1) buffer pixel_buffer
{
  uint pixel_data[];
};

uniform int internalWidth;
uniform int internalHeight;

uniform int resWidth;
uniform int resHeight;

// This is black magic to convert tiles to linear, just don't touch it
int xeFbConvert(int width, int addr) {
  int y = addr / (width * 4);
  int x = (addr % (width * 4)) / 4;
  return ((((y & ~31) * width) + (x & ~31) * 32) +
         (((x & 3) + ((y & 1) << 2) + ((x & 28) << 1) + ((y & 30) << 5)) ^ 
         ((y & 8) << 2)));
}

#define TILE(x) ((x + 31) >> 5) << 5

void main() {
  ivec2 texel_pos = ivec2(gl_GlobalInvocationID.xy);
  // OOB check, but shouldn't be needed
  if (texel_pos.x >= resWidth || texel_pos.y >= resHeight)
    return;

  // Precalc whatever it would be with extra sizing for 32x32 tiles
  int tiledWidth = TILE(internalWidth);
  int tiledHeight = TILE(internalHeight);

  // Scale accordingly
  float scaleX = tiledWidth / float(resWidth);
  float scaleY = tiledHeight / float(resHeight);

  // Map to source resolution
  int srcX = int(float(texel_pos.x) * scaleX);
  int srcY = int(float(texel_pos.y) * scaleY);

  // God only knows how this indexing works
  int stdIndex = (srcY * tiledWidth + srcX);
  int xeIndex = xeFbConvert(tiledWidth, stdIndex * 4);

  uint packedColor = pixel_data[xeIndex];
  imageStore(o_texture, texel_pos, uvec4(packedColor, 0, 0, 0));
}
)";

} // namespace Render