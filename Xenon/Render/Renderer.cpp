// Copyright 2025 Xenon Emulator Project

#include "Renderer.h"

#include "Render/Implementations/OGLTexture.h" 
#include "GUI/Implementations/OpenGL.h"
#include "Base/Config.h"
#include "Base/Path_util.h"
#include "Base/Version.h"
#include "Base/Logging/Log.h"

#include "Core/XGPU/XGPU.h"


// Shaders
void compileShaders(GLuint shader, const char* source) {
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  // Ensure the shader built
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    LOG_ERROR(System, "Failed to initialize SDL video subsystem: {}", infoLog);
  }
}

GLuint createShaderPrograms(const char* vertex, const char* fragment) {
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  compileShaders(vertexShader, vertex);
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  compileShaders(fragmentShader, fragment);
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return program;
}

Render::Renderer::Renderer(RAM *ram) :
  ramPointer(ram),
  internalWidth(Config::internalWindowWidth()),
  internalHeight(Config::internalWindowHeight()),
  width(TILE(Config::windowWidth())),
  height(TILE(Config::windowHeight())),
  VSYNC(Config::vsync()),
  fullscreen(Config::fullscreenMode())
{
  thread = std::thread(&Render::Renderer::Thread, this);
  thread.detach();
}       

Render::Renderer::~Renderer() {
  Shutdown();
}

// Vali0004:
// Why did I do this, you may ask?
// Well, it's because OpenGL fucking sucks.
// Both SDL and OpenGL use per-thread states.
// It is not possible to create all of the OpenGL state in a different thread, then pass it over.
// Just why...

void Render::Renderer::Start() {
  // Init SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_ERROR(System, "Failed to initialize SDL video subsystem: {}", SDL_GetError());
  }

  std::string title = std::format("Xenon {}", Base::VERSION);
  // SDL3 window properties.
  SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title.c_str());
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
  // For a new Vulkan support, don't forget to change 'SDL_WINDOW_OPENGL' by 'SDL_WINDOW_VULKAN'.
  SDL_SetNumberProperty(props, "flags", SDL_WINDOW_OPENGL);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
  // Create window
  mainWindow = SDL_CreateWindowWithProperties(props);
  // Destroy (no longer used) properties
  SDL_DestroyProperties(props);
  // Set min size
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
    LOG_ERROR(System, "Failed to create OpenGL context: {}", SDL_GetError());
  }
  // Init GLAD
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    LOG_ERROR(System, "Failed to initialize OpenGL Loader");
  }
  // Set VSYNC
  SDL_GL_SetSwapInterval((int)VSYNC);
  // Set if we are in fullscreen mode or not
  SDL_SetWindowFullscreen(mainWindow, fullscreen);
  // Get current window ID
  windowID = SDL_GetWindowID(mainWindow);

  // TODO(Vali0004): Pull shaders from a file
  // Init shader handles
  GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
  compileShaders(computeShader, computeShaderSource);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, computeShader);
  glLinkProgram(shaderProgram);
  glDeleteShader(computeShader);
  renderShaderProgram = createShaderPrograms(vertexShaderSource, fragmentShaderSource);

  // Create our backbuffer
  backbuffer = std::make_unique<OGLTexture>();

  // Init GL texture
  backbuffer->CreateTextureHandle(width, height,
    // Set our texture flags
    Render::eCreationFlags::glTextureWrapS_GL_CLAMP_TO_EDGE | Render::eCreationFlags::glTextureWrapT_GL_CLAMP_TO_EDGE |
    Render::eCreationFlags::glTextureMinFilter_GL_NEAREST | Render::eCreationFlags::glTextureMagFilter_GL_NEAREST |
    // Set our texture depth
    Render::eTextureDepth::R32U
  );

  // TODO(Vali0004): Setup a buffer implementation, abstract this away 
  // Init pixel buffer
  pitch = width * height * sizeof(u32);
  pixels.resize(pitch, COLOR(30, 30, 30, 255)); // Init with dark grey
  glGenBuffers(1, &pixelBuffer);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pixelBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, pixels.size(), pixels.data(), GL_DYNAMIC_DRAW);

  // Create a dummy VAO
  glGenVertexArrays(1, &dummyVAO);

  // Set clear color
  glClearColor(0.f, 0.f, 0.f, 1.f);
  // Setup viewport
  glViewport(0, 0, width, height);
  // Disable unneeded things
  glDisable(GL_BLEND); // Xenos does not have alpha, and blending breaks anyways
  glDisable(GL_DEPTH_TEST);

  // Create our GUI
  gui = std::make_unique<OpenGLGUI>();
  gui->Init(mainWindow, reinterpret_cast<void*>(context));
}

void Render::Renderer::Shutdown() {
  gui->Shutdown();
  gui.reset();
  glDeleteVertexArrays(1, &dummyVAO);
  glDeleteBuffers(1, &pixelBuffer);
  glDeleteProgram(shaderProgram);
  glDeleteProgram(renderShaderProgram);
  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(mainWindow);
  SDL_Quit();
}

void Render::Renderer::Resize(int x, int y, bool resizeViewport) {
  // Normalize our x and y for tiling
  width = TILE(x);
  height = TILE(y);
  // Resize viewport
  if (resizeViewport)
    glViewport(0, 0, width, height);
  // Recreate our texture with the new size
  backbuffer->ResizeTexture(width, height);
  // Set our new pitch
  pitch = width * height * sizeof(u32);
  // Resize our pixel buffer
  pixels.resize(pitch);
  // Recreate the buffer
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pixelBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, pixels.size(), pixels.data(), GL_DYNAMIC_DRAW);
  LOG_DEBUG(Xenos, "Resized window to {}x{}", width, height);
}

void Render::Renderer::Thread() {
  Start();

  // Framebuffer pointer from main memory.
  fbPointer = ramPointer->getPointerToAddress(XE_FB_BASE);
  // Should we render?
  bool rendering = Config::gpuThreadEnabled();
  while (rendering) {
    // Process events.
    while (SDL_PollEvent(&windowEvent)) {
      if (gui.get())
        ImGui_ImplSDL3_ProcessEvent(&windowEvent);
      switch (windowEvent.type) {
      case SDL_EVENT_WINDOW_RESIZED:
        if (windowEvent.window.windowID == windowID) {
          LOG_DEBUG(Xenos, "Resizing window...");
          Resize(windowEvent.window.data1, windowEvent.window.data2);
        }
        break;
      case SDL_EVENT_QUIT:
        Shutdown();
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
        if (windowEvent.key.key == SDLK_F6) {
          LOG_INFO(Xenos, "RenderWindow: Resize without changing viewport");
          Resize(1280, 720, false);
        }
        if (windowEvent.key.key == SDLK_F9) {
          LOG_INFO(Xenos, "RenderWindow: Taking a XenosFB snapshot");
          const auto UserDir = Base::FS::GetUserPath(Base::FS::PathType::UserDir);
          std::ofstream f(UserDir / "fbmem.bin", std::ios::out | std::ios::binary | std::ios::trunc);
          if (!f) {
            LOG_ERROR(Xenos, "Failed to open fbmem.bin for writing");
          }
          else {
            f.write(reinterpret_cast<const char*>(fbPointer), pitch);
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
    if (fbPointer) {
      const u32* ui_fbPointer = reinterpret_cast<u32*>(fbPointer);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, pitch, ui_fbPointer);

      if (!imguiRender) {
        // Use the compute shader
        glUseProgram(shaderProgram);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pixelBuffer);
        glUniform1i(glGetUniformLocation(shaderProgram, "internalWidth"), internalWidth);
        glUniform1i(glGetUniformLocation(shaderProgram, "internalHeight"), internalHeight);
        glUniform1i(glGetUniformLocation(shaderProgram, "resWidth"), width);
        glUniform1i(glGetUniformLocation(shaderProgram, "resHeight"), height);
        glDispatchCompute(width / 16, height / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
      }
    }

    // Render the texture
    if (!imguiRender) {
      glUseProgram(renderShaderProgram);
      backbuffer->Bind();
      glBindVertexArray(dummyVAO);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
    }
    gui->Render(backbuffer.get());

    SDL_GL_SwapWindow(mainWindow);
  }
}