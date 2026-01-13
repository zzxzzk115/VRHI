# 跨平台开发指南

## 概述

VRHI 设计之初就考虑了跨平台支持。本指南帮助开发者在多个平台上开发应用程序。

## 支持的平台

| 平台 | 架构 | 推荐后端 | 备选后端 |
|------|------|----------|----------|
| **Windows 10/11** | x64, ARM64 | Vulkan | OpenGL 4.6/4.3/3.3 |
| **Linux** | x64, ARM64 | Vulkan | OpenGL 4.6/4.3/3.3 |
| **macOS** | x64, ARM64 | OpenGL 3.3 | - |
| **Android** | ARM64, ARM | Vulkan | OpenGL ES 3.1/3.0/2.0 |
| **iOS** | ARM64 | OpenGL ES 3.0 | OpenGL ES 2.0 |
| **树莓派 4** | ARM64 | OpenGL ES 3.0 | Vulkan (实验), GLES 2.0 |
| **树莓派 3/Zero** | ARM | OpenGL ES 2.0 | - |

## 平台特定配置

### Windows

```cpp
#ifdef _WIN32
#include <VRHI/VRHI.hpp>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    VRHI::DeviceConfig config;
    config.windowHandle = hwnd;  // Win32 窗口句柄
    config.preferredBackend = VRHI::BackendType::Vulkan;
    
    auto device = VRHI::CreateDevice(config).value();
    
    // Windows 消息循环
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            RenderFrame(device.get());
        }
    }
    
    return 0;
}
#endif
```

**注意事项**:
- Vulkan 需要安装 Vulkan SDK
- OpenGL 通过驱动提供
- 支持 DirectX 互操作（未来）

### Linux

```cpp
#ifdef __linux__
#include <VRHI/VRHI.hpp>
#include <X11/Xlib.h>

int main() {
    // X11 窗口创建
    Display* display = XOpenDisplay(nullptr);
    Window window = XCreateSimpleWindow(display, /*...*/);
    
    VRHI::DeviceConfig config;
    config.windowHandle = (void*)window;
    config.preferredBackend = VRHI::BackendType::Vulkan;
    
    auto device = VRHI::CreateDevice(config).value();
    
    // X11 事件循环
    XEvent event;
    bool running = true;
    while (running) {
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == ClientMessage) {
                running = false;
            }
        }
        
        RenderFrame(device.get());
    }
    
    XCloseDisplay(display);
    return 0;
}
#endif
```

**注意事项**:
- 支持 X11 和 Wayland
- Mesa 驱动支持良好
- 可使用系统包管理器安装 Vulkan

### macOS

```cpp
#ifdef __APPLE__
#include <VRHI/VRHI.hpp>

// macOS 限制
// - OpenGL 最高支持 4.1，但推荐使用 3.3 Core
// - 未来将添加 Metal 后端支持

int main() {
    VRHI::DeviceConfig config;
    config.preferredBackend = VRHI::BackendType::OpenGL33;
    
    auto device = VRHI::CreateDevice(config).value();
    
    // Cocoa 事件循环
    // ...
    
    return 0;
}
#endif
```

**注意事项**:
- OpenGL 在 macOS 已被弃用
- 强烈建议等待 Metal 后端
- 使用 OpenGL 3.3 Core Profile

### Android

```cpp
#ifdef __ANDROID__
#include <VRHI/VRHI.hpp>
#include <android/native_window.h>

void android_main(struct android_app* app) {
    // 等待窗口创建
    while (!app->window) {
        int events;
        struct android_poll_source* source;
        if (ALooper_pollAll(0, nullptr, &events, 
                           (void**)&source) >= 0) {
            if (source) source->process(app, source);
        }
    }
    
    VRHI::DeviceConfig config;
    config.windowHandle = app->window;
    
    // 优先 Vulkan，回退到 OpenGL ES
    auto device = VRHI::CreateDevice(config).value();
    
    auto info = device->GetBackendInfo();
    __android_log_print(ANDROID_LOG_INFO, "VRHI", 
                       "Using backend: %s", info.name.c_str());
    
    // 主循环
    bool running = true;
    while (running) {
        // 处理事件...
        RenderFrame(device.get());
    }
}
#endif
```

**AndroidManifest.xml**:
```xml
<uses-feature android:glEsVersion="0x00030000" android:required="false" />
<uses-feature android:name="android.hardware.vulkan.version" 
              android:version="0x400003" android:required="false" />
```

**CMakeLists.txt**:
```cmake
add_library(native-lib SHARED main.cpp)
target_link_libraries(native-lib
    android
    log
    VRHI::VRHI
)
```

### iOS

```cpp
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS

#include <VRHI/VRHI.hpp>

@interface GameViewController : UIViewController
@end

@implementation GameViewController {
    std::unique_ptr<VRHI::Device> device;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    GLKView* view = (GLKView*)self.view;
    view.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    VRHI::DeviceConfig config;
    config.windowHandle = (__bridge void*)view;
    
    auto result = VRHI::CreateDevice(config);
    if (result) {
        device = std::move(*result);
    }
}

- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect {
    RenderFrame(device.get());
}

@end

#endif
#endif
```

**Info.plist**:
```xml
<key>UIRequiredDeviceCapabilities</key>
<array>
    <string>opengles-3</string>
</array>
```

### 树莓派

```cpp
#ifdef __arm__
#include <VRHI/VRHI.hpp>

int main() {
    VRHI::DeviceConfig config;
    
    // 树莓派 4: 优先 OpenGL ES 3.0
    // 树莓派 3: 使用 OpenGL ES 2.0
    
    auto device = VRHI::CreateDevice(config).value();
    
    // 使用 EGL 创建窗口和上下文
    // ...
    
    while (running) {
        RenderFrame(device.get());
    }
    
    return 0;
}
#endif
```

## 条件编译

### 平台检测宏

```cpp
// 在 VRHI 代码中使用
#if defined(_WIN32) || defined(_WIN64)
    #define VRHI_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define VRHI_PLATFORM_LINUX
    #if defined(__ANDROID__)
        #define VRHI_PLATFORM_ANDROID
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define VRHI_PLATFORM_IOS
    #else
        #define VRHI_PLATFORM_MACOS
    #endif
#endif

// 架构检测
#if defined(__aarch64__) || defined(_M_ARM64)
    #define VRHI_ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #define VRHI_ARCH_ARM
#elif defined(__x86_64__) || defined(_M_X64)
    #define VRHI_ARCH_X64
#endif
```

### 特性检测

```cpp
// 运行时特性检测（推荐）
if (device->IsFeatureSupported(VRHI::Feature::Compute)) {
    // 使用计算着色器
} else {
    // 使用替代方案
}

// 编译时特性检测（仅必要时）
#ifdef VRHI_PLATFORM_MOBILE
    // 移动平台特定代码
    const int kTextureSize = 2048;
#else
    // 桌面平台
    const int kTextureSize = 4096;
#endif
```

## 性能优化

### 移动平台优化

```cpp
#ifdef VRHI_PLATFORM_MOBILE
void OptimizeForMobile(VRHI::Device* device) {
    // 使用更小的纹理
    const uint32_t maxTextureSize = 2048;
    
    // 使用压缩纹理
    auto format = device->IsFeatureSupported(VRHI::Feature::TextureCompression)
        ? VRHI::TextureFormat::ETC2_RGB8
        : VRHI::TextureFormat::RGBA8_UNorm;
    
    // 降低几何复杂度
    const uint32_t maxVertices = 50000;
    
    // 使用更简单的着色器
    const bool useSimpleShaders = true;
}
#endif
```

### 桌面平台优化

```cpp
#ifndef VRHI_PLATFORM_MOBILE
void OptimizeForDesktop(VRHI::Device* device) {
    // 使用高分辨率纹理
    const uint32_t maxTextureSize = 8192;
    
    // 启用高级特性
    if (device->IsFeatureSupported(VRHI::Feature::Compute)) {
        EnableComputeShaders();
    }
    
    // 使用更复杂的材质
    EnablePBRMaterials();
}
#endif
```

## 纹理格式兼容性

### 跨平台纹理策略

```cpp
VRHI::TextureFormat SelectTextureFormat(VRHI::Device* device) {
#ifdef VRHI_PLATFORM_ANDROID
    // Android 优先使用 ETC2
    if (device->GetFeatures().texture.etc2) {
        return VRHI::TextureFormat::ETC2_RGB8;
    }
#elif defined(VRHI_PLATFORM_IOS)
    // iOS 支持 PVRTC 和 ASTC
    if (device->GetFeatures().texture.astc) {
        return VRHI::TextureFormat::ASTC_4x4;
    }
#else
    // 桌面平台使用 BC (DXT)
    if (device->GetFeatures().texture.dxt) {
        return VRHI::TextureFormat::BC1_UNorm;
    }
#endif
    
    // 回退到未压缩格式
    return VRHI::TextureFormat::RGBA8_UNorm;
}
```

## 构建配置

### CMake 跨平台配置

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyVRHIApp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 平台特定设置
if(WIN32)
    # Windows
    add_definitions(-DVRHI_PLATFORM_WINDOWS)
elseif(APPLE)
    if(IOS)
        # iOS
        add_definitions(-DVRHI_PLATFORM_IOS)
    else()
        # macOS
        add_definitions(-DVRHI_PLATFORM_MACOS)
    endif()
elseif(ANDROID)
    # Android
    add_definitions(-DVRHI_PLATFORM_ANDROID)
elseif(UNIX)
    # Linux
    add_definitions(-DVRHI_PLATFORM_LINUX)
endif()

# 添加 VRHI
add_subdirectory(external/VRHI)

# 创建可执行文件
add_executable(myapp ${SOURCES})
target_link_libraries(myapp PRIVATE VRHI::VRHI)

# 平台特定链接
if(WIN32)
    target_link_libraries(myapp PRIVATE opengl32)
elseif(ANDROID)
    target_link_libraries(myapp PRIVATE android log GLESv3)
elseif(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(myapp PRIVATE ${OPENGL_LIBRARY})
else()
    target_link_libraries(myapp PRIVATE GL)
endif()
```

## 测试策略

### 跨平台测试

```cpp
// 自动化测试框架
void TestAllBackends() {
    auto backends = VRHI::EnumerateBackends();
    
    for (const auto& backendInfo : backends) {
        std::cout << "Testing " << backendInfo.name << "...\n";
        
        VRHI::DeviceConfig config;
        config.preferredBackend = backendInfo.type;
        
        auto result = VRHI::CreateDevice(config);
        if (!result) {
            std::cerr << "  Failed to create device\n";
            continue;
        }
        
        auto device = std::move(*result);
        
        // 运行测试套件
        RunRenderTests(device.get());
        RunResourceTests(device.get());
        
        std::cout << "  All tests passed!\n";
    }
}
```

## 常见问题

### Q: 如何处理不同平台的窗口系统？

A: 使用平台抽象层或第三方库（如 GLFW, SDL）：

```cpp
#include <GLFW/glfw3.h>

GLFWwindow* window = glfwCreateWindow(800, 600, "VRHI App", nullptr, nullptr);

VRHI::DeviceConfig config;
#ifdef _WIN32
config.windowHandle = glfwGetWin32Window(window);
#elif defined(__linux__)
config.windowHandle = (void*)(uintptr_t)glfwGetX11Window(window);
#endif

auto device = VRHI::CreateDevice(config).value();
```

### Q: 移动平台内存限制如何处理？

A: 监控内存使用并调整资源质量：

```cpp
auto props = device->GetProperties();
uint64_t availableMB = props.availableMemory / (1024 * 1024);

if (availableMB < 500) {
    // 低内存模式
    textureQuality = Low;
    geometryDetail = Low;
}
```

### Q: 如何处理着色器兼容性？

A: 使用着色器变体或运行时选择：

```cpp
const char* GetShaderSource(VRHI::BackendType backend) {
    switch (backend) {
    case VRHI::BackendType::Vulkan:
    case VRHI::BackendType::OpenGL46:
        return vertexShader450;  // GLSL 450
    case VRHI::BackendType::OpenGL33:
        return vertexShader330;  // GLSL 330
    case VRHI::BackendType::OpenGLES30:
        return vertexShaderES300; // GLSL ES 300
    default:
        return vertexShaderES100; // GLSL ES 100
    }
}
```

## 最佳实践总结

1. **使用 Auto 后端**: 让 VRHI 选择最适合平台的后端
2. **运行时特性检测**: 避免硬编码平台特定代码
3. **提供降级路径**: 为低端设备准备简化版本
4. **测试多平台**: 在目标平台上实际测试
5. **优化资源**: 根据平台调整纹理大小和格式
6. **监控性能**: 使用平台工具分析性能
7. **处理限制**: 了解每个平台的限制和最佳实践

## 参考资源

- [Vulkan 教程](https://vulkan-tutorial.com/)
- [OpenGL 文档](https://www.khronos.org/opengl/)
- [移动图形优化指南](https://developer.android.com/games/optimize)
