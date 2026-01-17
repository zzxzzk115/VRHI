// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include "Resources.hpp"
#include <cstdint>

namespace VRHI {

// Forward declarations
class Texture;

// ============================================================================
// Fence Interface
// ============================================================================

/// Fence for CPU-GPU synchronization
class Fence {
public:
    virtual ~Fence() = default;
    
    // Fence cannot be copied, only moved
    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;
    Fence(Fence&&) noexcept = default;
    Fence& operator=(Fence&&) noexcept = default;
    
    // ========================================================================
    // Synchronization
    // ========================================================================
    
    /// Wait for fence to be signaled
    /// @param timeout Timeout in nanoseconds (0 = don't wait, UINT64_MAX = wait forever)
    /// @return true if signaled, false if timeout
    virtual bool Wait(uint64_t timeout = UINT64_MAX) = 0;
    
    /// Reset fence to unsignaled state
    virtual void Reset() = 0;
    
    /// Check if fence is signaled
    virtual bool IsSignaled() const noexcept = 0;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native fence handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    Fence() = default;
};

// ============================================================================
// Semaphore Interface
// ============================================================================

/// Semaphore for GPU-GPU synchronization
class Semaphore {
public:
    virtual ~Semaphore() = default;
    
    // Semaphore cannot be copied, only moved
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore(Semaphore&&) noexcept = default;
    Semaphore& operator=(Semaphore&&) noexcept = default;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native semaphore handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    Semaphore() = default;
};

// ============================================================================
// Swap Chain Descriptor
// ============================================================================

struct SwapChainDesc {
    void* windowHandle = nullptr;
    uint32_t width = 1280;
    uint32_t height = 720;
    TextureFormat format = TextureFormat::RGBA8_SRGB;
    uint32_t bufferCount = 2;
    bool vsync = true;
    
    const char* debugName = nullptr;
};

// ============================================================================
// Swap Chain Interface
// ============================================================================

/// Swap chain for presenting images to the screen
class SwapChain {
public:
    virtual ~SwapChain() = default;
    
    // SwapChain cannot be copied, only moved
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
    SwapChain(SwapChain&&) noexcept = default;
    SwapChain& operator=(SwapChain&&) noexcept = default;
    
    // ========================================================================
    // Swap Chain Operations
    // ========================================================================
    
    /// Acquire next image from swap chain
    /// @param timeout Timeout in nanoseconds
    /// @param semaphore Optional semaphore to signal when image is ready
    /// @param fence Optional fence to signal when image is ready
    /// @return Image index on success, error on failure
    virtual uint32_t AcquireNextImage(uint64_t timeout = UINT64_MAX,
                                     Semaphore* semaphore = nullptr,
                                     Fence* fence = nullptr) = 0;
    
    /// Present current image to screen
    /// @param waitSemaphores Optional semaphores to wait on before presenting
    virtual void Present(Semaphore* const* waitSemaphores = nullptr,
                        uint32_t waitSemaphoreCount = 0) = 0;
    
    /// Resize swap chain
    /// @param width New width
    /// @param height New height
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    
    // ========================================================================
    // Swap Chain Information
    // ========================================================================
    
    /// Get current swap chain width
    virtual uint32_t GetWidth() const noexcept = 0;
    
    /// Get current swap chain height
    virtual uint32_t GetHeight() const noexcept = 0;
    
    /// Get swap chain format
    virtual TextureFormat GetFormat() const noexcept = 0;
    
    /// Get number of swap chain images
    virtual uint32_t GetImageCount() const noexcept = 0;
    
    /// Get swap chain image at index
    /// @param index Image index
    /// @return Texture representing the swap chain image
    virtual Texture* GetImage(uint32_t index) = 0;
    
    /// Get current image index
    virtual uint32_t GetCurrentImageIndex() const noexcept = 0;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native swap chain handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    SwapChain() = default;
};

// Convenience aliases for the public API




} // namespace VRHI
