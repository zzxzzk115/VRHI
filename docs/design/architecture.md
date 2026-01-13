# VRHI Architecture Design

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/design/architecture.md)

## Overview

VRHI (Versatile Render Hardware Interface) uses a layered architecture design, providing a unified rendering API abstraction while supporting multiple graphics backends.

## Architecture Layers

```
┌─────────────────────────────────────────┐
│         Application Layer                │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│      VRHI Core Interface Layer           │
│  - Device                                │
│  - CommandBuffer                         │
│  - Resources (Buffer, Texture, etc.)     │
│  - Pipeline                              │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│     Backend Abstraction Layer            │
│  - Backend Factory                       │
│  - Feature Detection                     │
│  - Scoring System                        │
└─────────────────────────────────────────┘
                    ↓
┌──────────┬──────────┬──────────┬─────────┐
│ Vulkan   │ OpenGL   │ OpenGL   │ OpenGL  │
│ Backend  │ 4.x      │ 3.3      │ ES      │
│          │ Backend  │ Backend  │ Backend │
└──────────┴──────────┴──────────┴─────────┘
                    ↓
┌─────────────────────────────────────────┐
│            Hardware Drivers              │
└─────────────────────────────────────────┘
```

## Core Components

### 1. Device

The Device is the core component of VRHI, representing a graphics hardware device.

**Main Responsibilities:**
- Resource creation and management
- Command queue management
- Feature queries
- Backend lifecycle management

### 2. CommandBuffer

Command buffers are used to record and submit rendering commands.

### 3. Resources

Includes various GPU resources: buffers, textures, samplers, etc.

### 4. Pipeline

Graphics and compute pipeline configuration.

## Backend System

All backends implement a unified interface, created through a factory pattern.

### Supported Backends

1. **Vulkan Backend** - High-performance PC and modern mobile devices
2. **OpenGL 4.6 Backend** - High-end PC
3. **OpenGL 4.3 Backend** - Mid to high-end PC
4. **OpenGL 3.3 Backend** - Older PCs and laptops
5. **OpenGL 2.0 Backend** - Legacy hardware
6. **OpenGL ES 3.1 Backend** - Modern mobile devices
7. **OpenGL ES 3.0 Backend** - Mainstream mobile devices
8. **OpenGL ES 2.0 Backend** - Older mobile devices, Raspberry Pi

For detailed documentation, please refer to the [Chinese version](../zh-CN/design/architecture.md).
