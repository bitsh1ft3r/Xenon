// Copyright 2025 Xenon Emulator Project

#include "XGPU.h"
#include "XGPUConfig.h"
#include "XenosRegisters.h"

#include "Base/Config.h"
#include "Base/Path_util.h"
#include "Base/Version.h"
#include "Base/Logging/Log.h"

Xe::Xenos::XGPU::XGPU(RAM *ram) {
  // Assign RAM Pointer
  ramPtr = ram;

  memset(&xgpuConfigSpace.data, 0xf, sizeof(GENRAL_PCI_DEVICE_CONFIG_SPACE));
  // Setup config space as per dump taken from a Jasper console.
  // Located at config address 0xD0010000.
  u8 i = 0;
  for (u16 idx = 0; idx < 256; idx += 4) {
    memcpy(&xgpuConfigSpace.data[idx], &xgpuConfigMap[i], 4);
    i++;
  }

  xenosState.Regs = new u8[0xFFFFF];
  memset(xenosState.Regs, 0, 0xFFFFF);

  // Set Clocks speeds.
  u32 reg = 0x09000000;
  memcpy(&xenosState.Regs[REG_GPU_CLK], &reg, 4);
  reg = 0x11000c00;
  memcpy(&xenosState.Regs[REG_EDRAM_CLK], &reg, 4);
  reg = 0x1a000001;
  memcpy(&xenosState.Regs[REG_FSB_CLK], &reg, 4);
  reg = 0x19100000;
  memcpy(&xenosState.Regs[REG_MEM_CLK], &reg, 4);

  if (Config::gpuThreadEnabled()) {
    renderThread = std::thread(&XGPU::XenosThread, this);
  }
  else{
      LOG_WARNING(Xenos, "Xenos Render thread disbaled in config.");
  }
}

bool Xe::Xenos::XGPU::Read(u64 readAddress, u64 *data, u8 byteCount) {
  if (isAddressMappedInBAR(static_cast<u32>(readAddress))) {

    const u32 regIndex = (readAddress & 0xFFFFF) / 4;

    LOG_TRACE(Xenos, "Read Addr = {:#x}, reg: {:#x}.", readAddress, regIndex);

    XeRegister reg = static_cast<XeRegister>(regIndex);

    u32 regData = 0;
    memcpy(&regData, &xenosState.Regs[regIndex * 4], 4);

    // Switch for properly return the requested amount of data.
    switch (byteCount) {
  case 2:
      regData = regData >> 16;
      break;
  case 1:
      regData = regData >> 24;
      break;
  default:
      break;
  }

*data = regData;
    
    if (regIndex == 0x00000a07)
      *data = 0x2000000;

    if (regIndex == 0x00001928)
      *data = 0x2000000;

    if (regIndex == 0x00001e54)
      *data = 0;

    return true;
  }

  return false;
}

bool Xe::Xenos::XGPU::Write(u64 writeAddress, u64 data, u8 byteCount) {
  if (isAddressMappedInBAR(static_cast<u32>(writeAddress))) {

    const u32 regIndex = (writeAddress & 0xFFFFF) / 4;

    LOG_TRACE(Xenos, "Write Addr = {:#x}, reg: {:#x}, data = {:#x}.", writeAddress, regIndex,
        std::byteswap(static_cast<u32>(data)));

    XeRegister reg = static_cast<XeRegister>(regIndex);

    memcpy(&xenosState.Regs[regIndex * 4], &data, byteCount);
    return true;
  }

  return false;
}

void Xe::Xenos::XGPU::ConfigRead(u64 readAddress, u64 *data, u8 byteCount) {
  memcpy(data, &xgpuConfigSpace.data[readAddress & 0xFF], byteCount);
  return;
}

void Xe::Xenos::XGPU::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
  memcpy(&xgpuConfigSpace.data[writeAddress & 0xFF], &data, byteCount);
  return;
}

bool Xe::Xenos::XGPU::isAddressMappedInBAR(u32 address) {
  u32 bar0 = xgpuConfigSpace.configSpaceHeader.BAR0;
  u32 bar1 = xgpuConfigSpace.configSpaceHeader.BAR1;
  u32 bar2 = xgpuConfigSpace.configSpaceHeader.BAR2;
  u32 bar3 = xgpuConfigSpace.configSpaceHeader.BAR3;
  u32 bar4 = xgpuConfigSpace.configSpaceHeader.BAR4;
  u32 bar5 = xgpuConfigSpace.configSpaceHeader.BAR5;

  if (address >= bar0 && address <= bar0 + XGPU_DEVICE_SIZE ||
      address >= bar1 && address <= bar1 + XGPU_DEVICE_SIZE ||
      address >= bar2 && address <= bar2 + XGPU_DEVICE_SIZE ||
      address >= bar3 && address <= bar3 + XGPU_DEVICE_SIZE ||
      address >= bar4 && address <= bar4 + XGPU_DEVICE_SIZE ||
      address >= bar5 && address <= bar5 + XGPU_DEVICE_SIZE) {
    return true;
  }

  return false;
}

// Shaders
constexpr const char* vertexShaderSource = R"(
#version 430 core

out vec2 o_texture_coord;

void main() {
  o_texture_coord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
  gl_Position = vec4(o_texture_coord * vec2(2.0f, -2.0f) + vec2(-1.0f, 1.0f), 0.0f, 1.0f);
}
)";
constexpr const char* fragmentShaderSource = R"(
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
constexpr const char* computeShaderSource = R"(
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

void compileShader(GLuint shader, const char* source) {
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      LOG_ERROR(System, "Failed to initialize SDL video subsystem: {}", infoLog);
    }
}

GLuint createShaderProgram(const char* vertex, const char* fragment) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    compileShader(vertexShader, vertex);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    compileShader(fragmentShader, fragment);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void Xe::Xenos::XGPU::XenosResize(int x, int y) {
  // Normalize our x and y for tiling
  u32 resWidth = TILE(x);
  u32 resHeight = TILE(y);
  // Resize viewport
  glViewport(0, 0, resWidth, y);
  // Recreate our texture with the new size
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, resWidth, resHeight);
  glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
  // Set our new pitch
  pitch = resWidth * resHeight * sizeof(uint32_t);
  // Resize our pixel buffer
  pixels.resize(pitch);
  // Recreate the buffer
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pixelBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, pixels.size(), pixels.data(), GL_DYNAMIC_DRAW);
  LOG_DEBUG(Xenos, "Resized window to {}x{}", resWidth, resHeight);
}

void Xe::Xenos::XGPU::XenosThreadShutdown() {
  // Ensure all objects are cleaned up
  glDeleteVertexArrays(1, &dummyVAO);
  glDeleteBuffers(1, &pixelBuffer);
  glDeleteTextures(1, &texture);
  glDeleteProgram(shaderProgram);
  glDeleteProgram(renderShaderProgram);
  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(mainWindow);
  SDL_Quit();
}

void Xe::Xenos::XGPU::XenosThread() {
  // TODO(Vali0004): Pull internal width/height from ANA init
  u32 internalWidth = 1280;
  u32 internalHeight = 720;
  u32 resWidth = TILE(Config::windowWidth());
  u32 resHeight = TILE(Config::windowHeight());

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_ERROR(System, "Failed to initialize SDL video subsystem: {:#x}", SDL_GetError());
  }

  // Set the title.
  std::string TITLE = "Xenon " + std::string(Base::VERSION);

  // SDL3 window properties.
  SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING,
                        std::string(TITLE).c_str());
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER,
                        SDL_WINDOWPOS_CENTERED);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER,
                        SDL_WINDOWPOS_CENTERED);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,
                        Config::windowWidth());
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER,
                        Config::windowHeight());
  // Only putting this back when a Vulkan implementation is done.
  //SDL_SetNumberProperty(props, "flags", SDL_WINDOW_VULKAN);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
  mainWindow = SDL_CreateWindowWithProperties(props);
  SDL_DestroyProperties(props);

  SDL_SetWindowMinimumSize(mainWindow, 640, 480);

  // Set OpenGL SDL Properties
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  // Set RGBA size (R8G8B8A8)
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  // Set OpenGL version to 4.3 (earliest with CS)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  // We aren't using compatibility profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // Create OpenGL handle for SDL
  context = SDL_GL_CreateContext(mainWindow);
  if (!context) {
    LOG_ERROR(System, "Failed to create OpenGL context: {:#x}", SDL_GetError());
  }

  // Init GLAD
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    LOG_ERROR(System, "Failed to initialize OpenGL Loader");
  }

  // Init shader handles
  GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
  compileShader(computeShader, computeShaderSource);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, computeShader);
  glLinkProgram(shaderProgram);
  glDeleteShader(computeShader);
  renderShaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

  // Init GL texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, resWidth, resHeight);
  glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

  // Init pixel buffer
  pitch = resWidth * resHeight * sizeof(uint32_t);
  pixels.resize(pitch, COLOR(30, 30, 30, 255)); // Init with dark grey
  glGenBuffers(1, &pixelBuffer);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pixelBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, pixels.size(), pixels.data(), GL_DYNAMIC_DRAW);

  // Create a dummy VAO
  glGenVertexArrays(1, &dummyVAO);

  // Set clear color
  glClearColor(0.f, 0.f, 0.f, 1.f);
  // Setup viewport
  glViewport(0, 0, resWidth, resHeight);
  // Disable unneeded things
  glDisable(GL_BLEND); // Xenos does not have alpha, and blending breaks anyways
  glDisable(GL_DEPTH_TEST);

  // Framebuffer pointer from main memory.
  u8 *fbPointer = ramPtr->getPointerToAddress(XE_FB_BASE);
  // Rendering Mode.
  bool rendering = true;
  // VSYNC Mode.
  bool VSYNC = true;
  // Set VSYNC mode to default.
  SDL_GL_SetSwapInterval((int)VSYNC);
  // Fullscreen Mode.
  SDL_SetWindowFullscreen(mainWindow, Config::fullscreenMode());

  while (rendering) {
    // Process events.
    while (SDL_PollEvent(&windowEvent)) {
      switch (windowEvent.type) {
      case SDL_EVENT_WINDOW_RESIZED:
        LOG_DEBUG(Xenos, "Resizing window...");
        XenosResize(windowEvent.window.data1, windowEvent.window.data2);
        break;
      case SDL_EVENT_QUIT:
        XenosThreadShutdown();
        if (Config::quitOnWindowClosure()) {
          exit(0);
        }
        rendering = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        if (windowEvent.key.key == SDLK_F5) {
          SDL_GL_SetSwapInterval((int)!VSYNC);
          LOG_INFO(Xenos, "RenderWindow: Setting Vsync to: {0:#b}", VSYNC);
          VSYNC = !VSYNC;
        }
        if (windowEvent.key.key == SDLK_F9) {
          LOG_INFO(Xenos, "RenderWindow: Taking a XenosFB snapshot");
          std::ofstream f(Base::FS::GetUserPath(Base::FS::PathType::UserDir) / "fbmem.bin", std::ios::out | std::ios::binary | std::ios::trunc);
          if (!f) {
            LOG_ERROR(Xenos, "Failed to open fbmem.bin for writing");
          }
          else {
            f.write(reinterpret_cast<const char*>(fbPointer), pitch * 4);
            LOG_INFO(Xenos, "Framebuffer dumped to Xenon/fbmem.bin");
          }
          f.close();
        }
        if (windowEvent.key.key == SDLK_F11) {
          SDL_WindowFlags flag = SDL_GetWindowFlags(mainWindow);
          bool fullscreenMode = flag & SDL_WINDOW_FULLSCREEN;
          SDL_SetWindowFullscreen(mainWindow, !fullscreenMode);
        }
        break;
      default:
        break;
      }
    }

    // Upload buffer
    u32* ui_fbPointer = reinterpret_cast<uint32_t*>(fbPointer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, pitch, ui_fbPointer);

    // Use the compute shader
    glUseProgram(shaderProgram);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pixelBuffer);
    glUniform1i(glGetUniformLocation(shaderProgram, "internalWidth"), internalWidth);
    glUniform1i(glGetUniformLocation(shaderProgram, "internalHeight"), internalHeight);
    glUniform1i(glGetUniformLocation(shaderProgram, "resWidth"), resWidth);
    glUniform1i(glGetUniformLocation(shaderProgram, "resHeight"), resHeight);
    glDispatchCompute(resWidth / 16, resHeight / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    // Render the texture
    glUseProgram(renderShaderProgram);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(dummyVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

    SDL_GL_SwapWindow(mainWindow);
  }
}
