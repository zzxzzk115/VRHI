# Device Management API

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/api/device.md)

## Overview

The Device is VRHI's core object, representing a graphics hardware device. This document describes device creation, management, and usage.

## Device Creation

### CreateDevice

```cpp
std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config = {});
```

Creates a rendering device instance.

## Device Interface

The Device interface provides:

- **Information Queries**: GetBackendType, GetBackendInfo, GetProperties
- **Feature Queries**: GetFeatures, IsFeatureSupported
- **Resource Creation**: CreateBuffer, CreateTexture, CreateShader, etc.
- **Command Execution**: CreateCommandBuffer, Submit, WaitIdle
- **Synchronization**: CreateFence, CreateSemaphore, Flush
- **Swap Chain Management**: GetSwapChain, Present, Resize

For detailed documentation including configuration options, usage examples, and best practices, please refer to the [Chinese version](../zh-CN/api/device.md).
