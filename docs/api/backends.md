# Backend Interfaces

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/api/backends.md)

## Overview

VRHI supports multiple graphics backends, each implementing a unified interface. This document describes the backend system design and features of each backend.

## Backend Architecture

All backends implement the `IBackend` interface.

## Supported Backends

### 1. Vulkan Backend
- **Platforms**: Windows, Linux, Android
- **Features**: Best performance, full modern feature support

### 2. OpenGL 4.6 Backend
- **Platforms**: Windows, Linux
- **Features**: Modern OpenGL features, Direct State Access (DSA)

### 3. OpenGL 4.3 Backend
- **Platforms**: Windows, Linux
- **Features**: Compute shader support

### 4. OpenGL 3.3 Backend
- **Platforms**: Windows, Linux, macOS
- **Features**: Wide hardware compatibility

### 5-8. OpenGL ES Backends (3.1, 3.0, 2.0)
- **Platforms**: Android, iOS, Raspberry Pi
- **Features**: Mobile and embedded device support

## Backend Selection Strategy

VRHI automatically selects the best backend based on platform and hardware capabilities, with manual override available.

For detailed documentation including backend features, selection strategies, and platform-specific configurations, please refer to the [Chinese version](../zh-CN/api/backends.md).
