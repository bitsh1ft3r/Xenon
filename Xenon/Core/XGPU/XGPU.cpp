// Copyright 2025 Xenon Emulator Project

#include "XGPU.h"
#include "XGPUConfig.h"
#include "XenosRegisters.h"
#include "Core/Xe_Main.h"

#include "Base/Config.h"
#include "Base/Path_util.h"
#include "Base/Version.h"
#include "Base/Logging/Log.h"
#ifdef IMGUI
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#endif

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
    
    if (regIndex == 0x00000a07) { 
      *data = 0x2000000;
    }

    if (regIndex == 0x00001928) {
      *data = 0x2000000;
    }

    if (regIndex == 0x00001e54) {
      *data = 0;
    }

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
  resWidth = TILE(x);
  resHeight = TILE(y);
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

#ifdef IMGUI
void XenosGUIStyle() {
  ImGuiStyle &style = ImGui::GetStyle();
  ImVec4 *colors = style.Colors;
  // Colors
  colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
  colors[ImGuiCol_TextDisabled] = ImColor(255, 230, 49, 255);
  colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 248);
  colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 0);
  colors[ImGuiCol_PopupBg] = ImColor(20, 20, 20, 240);
  colors[ImGuiCol_Border] = ImColor(76, 76, 76, 127);
  colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
  colors[ImGuiCol_FrameBg] = ImColor(10, 10, 10, 138);
  colors[ImGuiCol_FrameBgHovered] = ImColor(10, 10, 10, 199);
  colors[ImGuiCol_FrameBgActive] = ImColor(71, 69, 69, 138);
  colors[ImGuiCol_TitleBg] = ImColor(111, 210, 50, 255);
  colors[ImGuiCol_TitleBgActive] = ImColor(108, 232, 0, 255);
  colors[ImGuiCol_TitleBgCollapsed] = ImColor(41, 41, 41, 191);
  colors[ImGuiCol_MenuBarBg] = ImColor(36, 36, 36, 255);
  colors[ImGuiCol_ScrollbarBg] = ImColor(5, 5, 5, 135);
  colors[ImGuiCol_ScrollbarGrab] = ImColor(79, 79, 79, 255);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(104, 104, 104, 255);
  colors[ImGuiCol_ScrollbarGrabActive] = ImColor(130, 130, 130, 255);
  colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
  colors[ImGuiCol_SliderGrab] = ImColor(87, 87, 87, 255);
  colors[ImGuiCol_SliderGrabActive] = ImColor(99, 97, 97, 255);
  colors[ImGuiCol_Button] = ImColor(108, 232, 0, 255);
  colors[ImGuiCol_ButtonHovered] = ImColor(110, 210, 50, 208);
  colors[ImGuiCol_ButtonActive] = ImColor(110, 210, 50, 240);
  colors[ImGuiCol_Header] = ImColor(94, 94, 94, 79);
  colors[ImGuiCol_HeaderHovered] = ImColor(97, 97, 97, 94);
  colors[ImGuiCol_HeaderActive] = ImColor(94, 94, 94, 130);
  colors[ImGuiCol_Separator] = ImColor(97, 97, 97, 127);
  colors[ImGuiCol_SeparatorHovered] = ImColor(117, 117, 117, 127);
  colors[ImGuiCol_SeparatorActive] = ImColor(117, 117, 117, 163);
  colors[ImGuiCol_ResizeGrip] = ImColor(0, 0, 0, 0);
  colors[ImGuiCol_ResizeGripHovered] = ImColor(108, 232, 0, 255);
  colors[ImGuiCol_ResizeGripActive] = ImColor(111, 210, 50, 255);
  colors[ImGuiCol_Tab] = ImColor(110, 210, 50, 208);
  colors[ImGuiCol_TabHovered] = ImColor(109, 232, 0, 240);
  colors[ImGuiCol_TabSelected] = ImColor(108, 232, 0, 255);
  colors[ImGuiCol_TabSelectedOverline] = ImColor(0, 0, 0, 0);
  colors[ImGuiCol_PlotLines] = ImColor(155, 155, 155, 255);
  colors[ImGuiCol_PlotLinesHovered] = ImColor(255, 110, 89, 255);
  colors[ImGuiCol_PlotHistogram] = ImColor(229, 179, 0, 255);
  colors[ImGuiCol_PlotHistogramHovered] = ImColor(255, 153, 0, 255);
  colors[ImGuiCol_TextSelectedBg] = ImColor(66, 150, 250, 89);
  colors[ImGuiCol_DragDropTarget] = ImColor(255, 255, 0, 230);
  colors[ImGuiCol_NavHighlight] = ImColor(66, 150, 250, 255);
  colors[ImGuiCol_NavWindowingHighlight] = ImColor(255, 255, 255, 179);
  colors[ImGuiCol_NavWindowingDimBg] = ImColor(204, 204, 204, 51);
  colors[ImGuiCol_ModalWindowDimBg] = ImColor(204, 204, 204, 89);

  // Style config
  style.Alpha = 1.f;
  style.DisabledAlpha = 0.95f;
  style.WindowPadding = { 10.f, 10.f };
  style.WindowRounding = 0.f;
  style.WindowBorderSize = 1.f;
  style.WindowMinSize = { 0.f, 0.f };
  style.WindowTitleAlign = { 0.f, 0.5f };
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 6.f;
  style.ChildBorderSize = 0.f;
  style.PopupRounding = 0.f;
  style.PopupBorderSize = 1.f;
  style.FramePadding = { 8.f, 4.f };
  style.FrameRounding = 4.f;
  style.FrameBorderSize = 1.f;
  style.ItemSpacing = { 10.f, 8.f };
  style.ItemInnerSpacing = { 6.f, 6.f };
  style.TouchExtraPadding = { 0.f, 0.f };
  style.IndentSpacing = 21.f;
  style.ScrollbarSize = 15.f;
  style.ScrollbarRounding = 0.f;
  style.GrabMinSize = 8.f;
  style.GrabRounding = 3.f;
  style.TabRounding = 5.f;
  style.TabBorderSize = 1.f;
  style.TabBarBorderSize = 0.5f;
  style.TabBarOverlineSize = 0.f;
  style.ButtonTextAlign = { 0.5f, 0.5f };
  style.DisplaySafeAreaPadding = { 3.f, 22.f };
  style.MouseCursorScale = 0.7f;
}
std::string InputText(const std::string& title, std::string initValue = {}, size_t maxCharacters = 256, const std::string& textHint = {}, ImGuiInputTextFlags flags = 0) {
  std::vector<char> buf(maxCharacters, '\0');
  if (buf[0] == '\0' && !initValue.empty()) {
    memcpy(buf.data(), initValue.data(), initValue.size());
  }

  if (textHint.empty()) {
    ImGui::InputText(title.c_str(), buf.data(), maxCharacters, flags);
  }
  else {
    ImGui::InputTextWithHint(title.c_str(), textHint.c_str(), buf.data(), maxCharacters, flags);
  }

  return buf.data();
}
bool ContainsHex(const std::string& str) {
  if (str.find("0x") != std::string::npos) {
    return true;
  }
  for (auto& c : str) {
    char uC = static_cast<char>(toupper(c)) - 65;
    if (uC >= 0 && uC <= 5) {
      return true;
    }
  }
  return false;
}
template <typename T>
void InputInt(const std::string& title, T& output, const std::string& textHint = {}, int baseRadix = 16) {
  int radix = baseRadix; 
  std::string initValue;
  if (radix == 16) {
    initValue = std::format("0x{:X}", (T)output);
  } else {
    initValue = std::format("{}", (T)output);
  }
  std::string buf = InputText(
    title,
    initValue,
    32,
    textHint,
    ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_AutoSelectAll
  );
  if (radix == 16 && !ContainsHex(buf)) {
    radix = 10;
  }
  if constexpr (std::is_unsigned_v<T>) {
    // This is cursed, but hey, better than checking for every type
    output = static_cast<T>(strtoull(buf.data(), NULL, radix));
  } else if constexpr (std::is_signed_v<T>) {
    output = static_cast<T>(strtoll(buf.data(), NULL, radix));
  } else {
    // ??????????
    LOG_INFO(Xenos, "How the fuck did you pass a int that is neither signed or unsigned?");
  }
}
void Xe::Xenos::XGPU::XenosGUIInit() {
  const char* glsl_version = "#version 430";
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = NULL; // We don't want to create a ini that stores pos as we init with a 1280x720 window, and reizing will keep pos, thus breaking things.

  XenosGUIStyle();

  ImGui_ImplSDL3_InitForOpenGL(mainWindow, context);
  ImGui_ImplOpenGL3_Init(glsl_version);
  // It might not be a bad idea to take the Xbox 360 font and convert it to TTF
  robotRegular19 = io.Fonts->AddFontFromFileTTF("fonts/Roboto/Roboto-Regular.ttf", 14.f);
  defaultFont13 = io.Fonts->AddFontDefault();
}
#endif

#ifdef IMGUI
struct ImGuiLog {
  ImGuiLog() {
    clear();
  }

  void pullFromFile(const std::filesystem::path& path, bool forcePull = false) {
    std::ifstream file(path, std::ios_base::in);
    if (!file.is_open()) {
      return;
    }
    size_t fileSize = std::filesystem::file_size(path);
    if (!forcePull && lastSize == fileSize) {
      file.close();
      return;
    }
    lastSize = buffer.size();
    clear();
    buffer.resize(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();
  }
  void clear() {
    buffer.clear();
    lineOffsets.clear();
    lineOffsets.push_back(0);
  }
  void send(const char data) {
    buffer.push_back(data);
  }
  void send(const std::string& data) {
    size_t size = buffer.size();
    buffer.resize(size + data.size());
    memcpy(&buffer[size], data.data(), data.size());
  }
  void draw() {
    if (ImGui::BeginPopup("Options")) {
      ImGui::Checkbox("Auto-scroll", &autoScroll);
      ImGui::EndPopup();
    }
    if (ImGui::Button("Options"))
      ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clearLog = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copyLog = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);
    ImGui::Separator();
    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
      if (clearLog)
        clear();
      if (copyLog)
        ImGui::LogToClipboard();

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
      const char *buf = buffer.data();
      const char *bufEnd = buffer.data() + buffer.size();
      if (filter.IsActive()) {
        for (int i = 0; i < lineOffsets.size(); i++) {
          const char* lineStart = buf + lineOffsets[i];
          const char* lineEnd = (i + 1 < lineOffsets.size()) ? (buf + lineOffsets[i + 1] - 1) : bufEnd;
          if (filter.PassFilter(lineStart, lineEnd))
            ImGui::TextUnformatted(lineStart, lineEnd);
        }
      }
      else {
        ImGuiListClipper clipper;
        clipper.Begin(lineOffsets.size());
        while (clipper.Step()) {
          for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
            const char* lineStart = buf + lineOffsets[line_no];
            const char* lineEnd = (line_no + 1 < lineOffsets.size()) ? (buf + lineOffsets[line_no + 1] - 1) : bufEnd;
            ImGui::TextUnformatted(lineStart, lineEnd);
          }
        }
        clipper.End();
      }
      ImGui::PopStyleVar();

      if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
  }
private:
  std::vector<char> buffer;
  size_t lastSize;
  ImGuiTextFilter filter;
  std::vector<int> lineOffsets;
  bool autoScroll = true;
};
#endif

void Xe::Xenos::XGPU::XenosThread() {
  if (!xenosWidth || !xenosHeight) {
    xenosWidth = 1280;
    xenosHeight = 720;
  }
  resWidth = TILE(Config::windowWidth());
  resHeight = TILE(Config::windowHeight());

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
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
  mainWindow = SDL_CreateWindowWithProperties(props);
  SDL_DestroyProperties(props);

  SDL_SetWindowMinimumSize(mainWindow, 640, 480);

  // Set OpenGL SDL Properties
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
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

#ifdef IMGUI
  XenosGUIInit();
#endif

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
  useVsync = Config::vsync();
  // Set VSYNC mode to default.
  SDL_GL_SetSwapInterval((int)useVsync);
  // Fullscreen Mode.
  isFullscreen = Config::fullscreenMode();
  SDL_SetWindowFullscreen(mainWindow, isFullscreen);

  while (rendering) {
    // Process events.
    while (SDL_PollEvent(&windowEvent)) {
#ifdef IMGUI  
      ImGui_ImplSDL3_ProcessEvent(&windowEvent);
#endif
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
          useVsync ^= true;
          SDL_GL_SetSwapInterval((int)useVsync);
          LOG_INFO(Xenos, "RenderWindow: Setting Vsync to: {0:#b}", useVsync);
        }
        if (windowEvent.key.key == SDLK_F11) {
          SDL_WindowFlags flag = SDL_GetWindowFlags(mainWindow);
          bool fullscreenMode = flag & SDL_WINDOW_FULLSCREEN;
          SDL_SetWindowFullscreen(mainWindow, !fullscreenMode);
          isFullscreen = fullscreenMode;
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
    glUniform1i(glGetUniformLocation(shaderProgram, "internalWidth"), xenosWidth);
    glUniform1i(glGetUniformLocation(shaderProgram, "internalHeight"), xenosHeight);
    glUniform1i(glGetUniformLocation(shaderProgram, "resWidth"), resWidth);
    glUniform1i(glGetUniformLocation(shaderProgram, "resHeight"), resHeight);
    glDispatchCompute(resWidth / 16, resHeight / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    // Render the texture 
    glClear(GL_COLOR_BUFFER_BIT); 
    glUseProgram(renderShaderProgram);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(dummyVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
#ifdef IMGUI
    // Render the XenosGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame(); {
      if (styleEditor) {
        ImGui::SetNextWindowPos({ 600.f, 60.f }, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ 1000.f, 900.f }, ImGuiCond_Once);
        ImGui::Begin("Style Editor", nullptr, ImGuiWindowFlags_NoCollapse); {
          ImGui::ShowStyleEditor();
        } ImGui::End();
      }
      if (demoWindow) {
        ImGui::ShowDemoWindow(&demoWindow);
      }
      ImGui::SetNextWindowSize({ 500.f, 300.f }, ImGuiCond_Once);
      ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse); {
        if (ImGui::BeginTabBar("#main_bar")) {
          if (ImGui::BeginTabItem("Log")) {
            static ImGuiLog log{};
            log.pullFromFile(Base::FS::GetUserPath(Base::FS::PathType::LogDir) / Base::FS::LOG_FILE);
            log.draw();
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("UART")) {
            static ImGuiLog uart{};
            if (Xe_Main && Xe_Main->smcCore) {
              for (char c = Xe_Main->smcCore->ReadUART(); c != '\0'; c = Xe_Main->smcCore->ReadUART()) {
                uart.send(c);
              }
            }
            uart.draw();
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Settings")) {
            if (ImGui::BeginTabBar("#settings_bar")) {
              if (ImGui::BeginTabItem("General")) {
                static int logLevel = static_cast<int>(Config::getCurrentLogLevel());
                InputInt("Log Level", logLevel, {}, 10);
                ImGui::Checkbox("Exit on window close", &Config::shouldQuitOnWindowClosure);
                ImGui::EndTabItem();
              }
              if (ImGui::BeginTabItem("PowerPC")) {
                static u64 initSkip1 = Config::HW_INIT_SKIP1();
                static u64 initSkip2 = Config::HW_INIT_SKIP2();
                InputInt("Hardware Init Skip 1", initSkip1);
                InputInt("Hardware Init Skip 2", initSkip2);
                ImGui::EndTabItem();
              }
              if (ImGui::BeginTabItem("SMC")) {
                static int smcPowerOnType = Config::smcPowerOnType();
                InputInt("Power On Signal", smcPowerOnType, {}, 10);
                ImGui::EndTabItem();
              }
              if (ImGui::BeginTabItem("GPU")) {
                InputInt("Width", Config::screenWidth, {}, 10);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone))
                  ImGui::SetTooltip("Note: This overrides the width on config level, not global");
                InputInt("Height", Config::screenHeight, {}, 10);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone))
                  ImGui::SetTooltip("Note: This overrides the height on config level, not global");
                ImGui::Checkbox("VSYNC", &useVsync);
                ImGui::Checkbox("Fullscreen", &isFullscreen);
                ImGui::Separator();
                if (ImGui::Button("Dump XenosFB")) {
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
                ImGui::EndTabItem();
              }
              if (ImGui::BeginTabItem("Paths")) {
                Config::fusesTxtPath = InputText("Fuses", Config::fusesPath());
                Config::nandBinPath = InputText("NAND", Config::nandPath());
                Config::oneBlBinPath = InputText("1BL", Config::oneBlPath());
                Config::oddDiscImagePath = InputText("ODD Image", Config::oddImagePath(), 256, "The ODD image is typically a ISO9960 file");
                ImGui::EndTabItem();
              }
              if (ImGui::BeginTabItem("Highly Experimental")) {
                InputInt("TPI", Config::ticksPerInstruction, {}, 10); 
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone))
                  ImGui::SetTooltip("Note: TPI is Ticks Per Instruction, this can mess with timing and crash Linux");
                ImGui::EndTabItem();
              }
              if (ImGui::BeginTabItem("ImGui")) {
                ImGui::Checkbox("Demo", &demoWindow);
                ImGui::Checkbox("Style Editor", &styleEditor);
                ImGui::EndTabItem();
              }
              ImGui::EndTabBar();
            }
            ImGui::Separator();
            if (ImGui::Button("Load")) {
              const auto userDir = Base::FS::GetUserPath(Base::FS::PathType::UserDir);
              Config::loadConfig(userDir / "xenon_config.toml");
              // We are not going to touch isFullscreen or display size while the CPU is active
              // Maybe later?
              useVsync = Config::vsync();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save")) {
              Config::vsyncEnabled = useVsync;
              Config::isFullscreen = isFullscreen;
              Config::screenWidth = resWidth;
              Config::screenHeight = resHeight;
              const auto userDir = Base::FS::GetUserPath(Base::FS::PathType::UserDir);
              Config::saveConfig(userDir / "xenon_config.toml");
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
              ImGui::SetTooltip("Note: This saves all settings to the config. This behaviour may be changed at some point");
            ImGui::EndTabItem();
          }
          ImGui::EndTabBar();
        }
      } ImGui::End();
    } ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

    SDL_GL_SwapWindow(mainWindow);
  }
}
