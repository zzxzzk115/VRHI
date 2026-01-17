# RAII Design Principles

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/design/raii_principles.md)

## Overview

VRHI strictly follows RAII (Resource Acquisition Is Initialization) principles to ensure proper resource management and automatic release. This is a core design pattern of modern C++ that effectively prevents memory and resource leaks.

## RAII Core Concepts

**RAII** core ideas:
- **Acquisition is Initialization**: Resources acquired during object construction
- **Release is Destruction**: Resources released during object destruction  
- **Scope-Bound**: Resource lifetime bound to object scope
- **Exception-Safe**: Resources released correctly even when exceptions occur

## VRHI RAII Practices

### 1. Smart Pointer Management

VRHI uses smart pointers to automatically manage object lifecycles:

```cpp
// Use unique_ptr for exclusive resources
std::unique_ptr<Device> device = VRHI::CreateDevice();
// device automatically destroyed when it goes out of scope

// Use shared_ptr for shared resources
std::shared_ptr<Texture> texture = device->CreateTexture(desc);
// Automatically destroyed when last reference disappears
```

### 2. Resource Wrapper Classes

All GPU resources are encapsulated in RAII classes with proper copy/move semantics.

### 3. C++23 Features

- `std::expected` for error handling
- Move semantics and perfect forwarding
- Concepts and constraints
- Ranges and views
- Coroutine support

For detailed documentation including code examples, scope management patterns, and best practices, please refer to the [Chinese version](../zh-CN/design/raii_principles.md).
