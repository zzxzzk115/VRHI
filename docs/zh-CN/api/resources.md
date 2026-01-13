# 资源管理 API

## 概述

VRHI 提供统一的资源管理接口，包括缓冲区、纹理、采样器等 GPU 资源。所有资源都遵循 RAII 原则，自动管理生命周期。

## Buffer (缓冲区)

### BufferDesc

```cpp
enum class BufferUsage : uint32_t {
    Vertex       = 1 << 0,  // 顶点缓冲
    Index        = 1 << 1,  // 索引缓冲
    Uniform      = 1 << 2,  // Uniform 缓冲
    Storage      = 1 << 3,  // 存储缓冲 (SSBO)
    Indirect     = 1 << 4,  // 间接绘制缓冲
    TransferSrc  = 1 << 5,  // 传输源
    TransferDst  = 1 << 6,  // 传输目标
};

enum class MemoryAccess {
    GpuOnly,      // 仅 GPU 访问
    CpuToGpu,     // CPU 写入，GPU 读取
    GpuToCpu,     // GPU 写入，CPU 读取
    CpuOnly,      // 仅 CPU 访问
};

struct BufferDesc {
    size_t size;                          // 缓冲区大小（字节）
    BufferUsage usage;                    // 使用标志
    MemoryAccess memoryAccess = MemoryAccess::GpuOnly;
    const void* initialData = nullptr;    // 初始数据
    const char* debugName = nullptr;      // 调试名称
};
```

### Buffer 类

```cpp
class Buffer {
public:
    // 获取大小
    size_t GetSize() const;
    
    // 获取使用标志
    BufferUsage GetUsage() const;
    
    // 映射内存
    void* Map();
    void* Map(size_t offset, size_t size);
    void Unmap();
    
    // 更新数据
    void Update(const void* data, size_t size, size_t offset = 0);
    
    // 读取数据（仅支持 CpuToGpu 或 GpuToCpu）
    void Read(void* data, size_t size, size_t offset = 0);
    
    // 拷贝缓冲区（需要在命令缓冲中执行）
    static void Copy(CommandBuffer* cmd, Buffer* src, Buffer* dst,
                     size_t srcOffset, size_t dstOffset, size_t size);
};
```

### 使用示例

```cpp
// 创建顶点缓冲
VRHI::BufferDesc vertexDesc;
vertexDesc.size = sizeof(vertices);
vertexDesc.usage = VRHI::BufferUsage::Vertex;
vertexDesc.memoryAccess = VRHI::MemoryAccess::CpuToGpu;
vertexDesc.initialData = vertices.data();
vertexDesc.debugName = "Vertex Buffer";

auto vertexBuffer = device->CreateBuffer(vertexDesc).value();

// 动态更新
std::vector<Vertex> newVertices = GenerateVertices();
vertexBuffer->Update(newVertices.data(), newVertices.size() * sizeof(Vertex));

// 映射内存
{
    void* mapped = vertexBuffer->Map();
    memcpy(mapped, data, size);
    vertexBuffer->Unmap();
}
```

## Texture (纹理)

### TextureDesc

```cpp
enum class TextureType {
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture1DArray,
    Texture2DArray,
    TextureCubeArray,
};

enum class TextureFormat {
    // 8-bit 格式
    R8_UNorm,
    RG8_UNorm,
    RGBA8_UNorm,
    RGBA8_SRGB,
    
    // 16-bit 格式
    R16_Float,
    RG16_Float,
    RGBA16_Float,
    
    // 32-bit 格式
    R32_Float,
    RG32_Float,
    RGBA32_Float,
    
    // 深度/模板
    Depth16,
    Depth24Stencil8,
    Depth32F,
    
    // 压缩格式
    BC1_UNorm,      // DXT1
    BC3_UNorm,      // DXT5
    BC7_UNorm,      // BC7
    ETC2_RGB8,      // ETC2
    ASTC_4x4,       // ASTC
};

enum class TextureUsage : uint32_t {
    Sampled          = 1 << 0,  // 着色器采样
    Storage          = 1 << 1,  // 存储图像
    RenderTarget     = 1 << 2,  // 渲染目标
    DepthStencil     = 1 << 3,  // 深度/模板
    TransferSrc      = 1 << 4,  // 传输源
    TransferDst      = 1 << 5,  // 传输目标
};

struct TextureDesc {
    TextureType type = TextureType::Texture2D;
    TextureFormat format = TextureFormat::RGBA8_UNorm;
    TextureUsage usage = TextureUsage::Sampled;
    
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    uint32_t arrayLayers = 1;
    uint32_t sampleCount = 1;  // MSAA 采样数
    
    const void* initialData = nullptr;
    const char* debugName = nullptr;
};
```

### Texture 类

```cpp
class Texture {
public:
    // 获取属性
    TextureType GetType() const;
    TextureFormat GetFormat() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetDepth() const;
    uint32_t GetMipLevels() const;
    uint32_t GetArrayLayers() const;
    
    // 更新纹理数据
    void Update(const void* data, size_t size,
                uint32_t mipLevel = 0, uint32_t arrayLayer = 0);
    
    // 更新子区域
    void UpdateRegion(const void* data, 
                      uint32_t x, uint32_t y, uint32_t z,
                      uint32_t width, uint32_t height, uint32_t depth,
                      uint32_t mipLevel = 0, uint32_t arrayLayer = 0);
    
    // 生成 Mipmap
    void GenerateMipmaps(CommandBuffer* cmd);
    
    // 读取纹理数据
    void Read(void* data, size_t size,
              uint32_t mipLevel = 0, uint32_t arrayLayer = 0);
};
```

### 使用示例

```cpp
// 创建 2D 纹理
VRHI::TextureDesc texDesc;
texDesc.type = VRHI::TextureType::Texture2D;
texDesc.format = VRHI::TextureFormat::RGBA8_SRGB;
texDesc.usage = VRHI::TextureUsage::Sampled | VRHI::TextureUsage::TransferDst;
texDesc.width = 1024;
texDesc.height = 1024;
texDesc.mipLevels = std::log2(1024) + 1;  // 完整 mipmap 链
texDesc.debugName = "Diffuse Texture";

auto texture = device->CreateTexture(texDesc).value();

// 加载图像数据
auto imageData = LoadImage("texture.png");
texture->Update(imageData.pixels, imageData.size);

// 生成 mipmaps
auto cmd = device->CreateCommandBuffer();
texture->GenerateMipmaps(cmd.get());
device->Submit(std::move(cmd));

// 创建立方体贴图
VRHI::TextureDesc cubeDesc;
cubeDesc.type = VRHI::TextureType::TextureCube;
cubeDesc.format = VRHI::TextureFormat::RGBA16_Float;
cubeDesc.usage = VRHI::TextureUsage::Sampled;
cubeDesc.width = 512;
cubeDesc.height = 512;
cubeDesc.arrayLayers = 6;  // 6 个面

auto cubemap = device->CreateTexture(cubeDesc).value();

// 更新每个面
for (uint32_t face = 0; face < 6; ++face) {
    auto faceData = LoadCubeFace(face);
    cubemap->Update(faceData.data(), faceData.size(), 0, face);
}
```

## Sampler (采样器)

### SamplerDesc

```cpp
enum class FilterMode {
    Nearest,
    Linear,
};

enum class AddressMode {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class CompareOp {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,
};

struct SamplerDesc {
    FilterMode minFilter = FilterMode::Linear;
    FilterMode magFilter = FilterMode::Linear;
    FilterMode mipmapMode = FilterMode::Linear;
    
    AddressMode addressModeU = AddressMode::Repeat;
    AddressMode addressModeV = AddressMode::Repeat;
    AddressMode addressModeW = AddressMode::Repeat;
    
    float mipLodBias = 0.0f;
    float minLod = 0.0f;
    float maxLod = 1000.0f;
    
    bool anisotropyEnable = false;
    float maxAnisotropy = 1.0f;
    
    bool compareEnable = false;
    CompareOp compareOp = CompareOp::Never;
    
    float borderColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    const char* debugName = nullptr;
};
```

### 使用示例

```cpp
// 线性过滤采样器
VRHI::SamplerDesc linearDesc;
linearDesc.minFilter = VRHI::FilterMode::Linear;
linearDesc.magFilter = VRHI::FilterMode::Linear;
linearDesc.mipmapMode = VRHI::FilterMode::Linear;
linearDesc.addressModeU = VRHI::AddressMode::Repeat;
linearDesc.addressModeV = VRHI::AddressMode::Repeat;

auto linearSampler = device->CreateSampler(linearDesc).value();

// 各向异性过滤
VRHI::SamplerDesc anisoDesc;
anisoDesc.anisotropyEnable = true;
anisoDesc.maxAnisotropy = 16.0f;

auto anisoSampler = device->CreateSampler(anisoDesc).value();

// 阴影采样器
VRHI::SamplerDesc shadowDesc;
shadowDesc.compareEnable = true;
shadowDesc.compareOp = VRHI::CompareOp::LessOrEqual;
shadowDesc.addressModeU = VRHI::AddressMode::ClampToBorder;
shadowDesc.addressModeV = VRHI::AddressMode::ClampToBorder;
shadowDesc.borderColor[0] = 1.0f;  // 白色边界

auto shadowSampler = device->CreateSampler(shadowDesc).value();
```

## Shader (着色器)

### ShaderDesc

```cpp
enum class ShaderStage {
    Vertex,
    Fragment,
    Geometry,
    TessControl,
    TessEvaluation,
    Compute,
    Mesh,
    Task,
};

enum class ShaderLanguage {
    GLSL,       // GLSL 源码
    HLSL,       // HLSL 源码
    SPIRV,      // SPIR-V 字节码
};

struct ShaderDesc {
    ShaderStage stage;
    ShaderLanguage language = ShaderLanguage::GLSL;
    
    // 源码或字节码
    const void* code;
    size_t codeSize;
    
    // 入口点（HLSL 需要）
    const char* entryPoint = "main";
    
    // 宏定义
    struct MacroDefine {
        std::string name;
        std::string value;
    };
    std::vector<MacroDefine> defines;
    
    const char* debugName = nullptr;
};
```

### 使用示例

```cpp
// GLSL 顶点着色器
const char* vertexSource = R"(
    #version 450
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec2 texCoord;
    
    layout(location = 0) out vec2 outTexCoord;
    
    void main() {
        gl_Position = vec4(position, 1.0);
        outTexCoord = texCoord;
    }
)";

VRHI::ShaderDesc vertexDesc;
vertexDesc.stage = VRHI::ShaderStage::Vertex;
vertexDesc.language = VRHI::ShaderLanguage::GLSL;
vertexDesc.code = vertexSource;
vertexDesc.codeSize = strlen(vertexSource);
vertexDesc.debugName = "Vertex Shader";

auto vertexShader = device->CreateShader(vertexDesc).value();

// 从文件加载 SPIR-V
auto spirvCode = ReadFile("shader.vert.spv");

VRHI::ShaderDesc spirvDesc;
spirvDesc.stage = VRHI::ShaderStage::Vertex;
spirvDesc.language = VRHI::ShaderLanguage::SPIRV;
spirvDesc.code = spirvCode.data();
spirvDesc.codeSize = spirvCode.size();

auto spirvShader = device->CreateShader(spirvDesc).value();
```

## 资源视图

### TextureView

```cpp
struct TextureViewDesc {
    Texture* texture;
    TextureType viewType;
    TextureFormat format;
    uint32_t baseMipLevel = 0;
    uint32_t mipLevelCount = 1;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayerCount = 1;
};

// 创建纹理视图
auto view = device->CreateTextureView(viewDesc).value();
```

### BufferView

```cpp
struct BufferViewDesc {
    Buffer* buffer;
    TextureFormat format;  // 用于纹理缓冲
    size_t offset = 0;
    size_t size = 0;  // 0 = 整个缓冲区
};

// 创建缓冲区视图
auto view = device->CreateBufferView(viewDesc).value();
```

## 资源绑定

资源绑定通过描述符集或绑定组完成，详见 [命令记录](commands.md)。

## 最佳实践

### 缓冲区

1. **选择正确的内存访问模式**: 
   - 频繁更新使用 `CpuToGpu`
   - 静态数据使用 `GpuOnly`
   
2. **使用临时缓冲上传大数据**:
```cpp
// 创建临时缓冲
auto staging = device->CreateBuffer({
    .size = dataSize,
    .usage = BufferUsage::TransferSrc,
    .memoryAccess = MemoryAccess::CpuToGpu,
}).value();

staging->Update(data, dataSize);

// 拷贝到 GPU
auto cmd = device->CreateCommandBuffer();
Buffer::Copy(cmd.get(), staging.get(), gpuBuffer.get(), 0, 0, dataSize);
device->Submit(std::move(cmd));
```

### 纹理

1. **使用正确的格式**:
   - 颜色纹理使用 SRGB 格式
   - HDR 使用浮点格式
   - 深度使用深度格式

2. **生成 Mipmaps**: 提高采样质量和性能

3. **压缩纹理**: 节省内存和带宽

### 采样器

1. **重用采样器**: 采样器对象可以共享
2. **合理使用各向异性**: 平衡质量和性能
3. **选择正确的寻址模式**: 根据纹理用途选择

## 线程安全

- 资源创建: 线程安全
- 资源更新: 需要外部同步
- 资源访问: 读操作线程安全，写操作需同步
