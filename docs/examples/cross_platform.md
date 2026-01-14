# Cross-Platform Development Guide

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/examples/cross_platform.md)

## Overview

VRHI is designed from the ground up with cross-platform support. This guide helps developers build applications across multiple platforms.

## Supported Platforms

| Platform | Architecture | Recommended Backend | Alternative Backends |
|----------|--------------|---------------------|----------------------|
| **Windows 10/11** | x64, ARM64 | Vulkan | OpenGL 4.6/4.1/3.3 |
| **Linux** | x64, ARM64 | Vulkan | OpenGL 4.6/4.1/3.3 |
| **macOS** | x64, ARM64 | OpenGL 4.1 | OpenGL 3.3 |
| **Android** | ARM64, ARM | Vulkan | OpenGL ES 3.1/3.0 |
| **iOS** | ARM64 | OpenGL ES 3.1 | OpenGL ES 3.0 |
| **Raspberry Pi 4+** | ARM64 | OpenGL ES 3.0 | OpenGL ES 3.1, Vulkan (experimental) |
| **Raspberry Pi 3** | ARM | OpenGL ES 3.0 | - |

## Platform-Specific Configuration

Examples for Windows, Linux, macOS, Android, iOS, and Raspberry Pi are provided.

## Conditional Compilation

Use platform detection macros and runtime feature detection for cross-platform code.

## Performance Optimization

Platform-specific optimizations for mobile vs. desktop platforms.

## Texture Format Compatibility

Cross-platform texture strategy with format fallbacks.

For detailed documentation including platform-specific code examples, build configurations, and common issues, please refer to the [Chinese version](../zh-CN/examples/cross_platform.md).
