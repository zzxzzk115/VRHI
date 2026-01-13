# Feature Detection System

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/design/feature_detection.md)

## Overview

VRHI's feature detection system queries hardware and driver support for various rendering features during initialization. This forms the foundation for backend scoring and selection.

## Design Principles

1. **Comprehensive**: Detect all features that may affect rendering
2. **Accurate**: Not only query claimed support, but verify actual functionality
3. **Efficient**: Detection process is fast and doesn't impact startup time
4. **Extensible**: Easy to add new feature detection
5. **Cross-Platform**: Unified detection interface with platform-specific details encapsulated in backends

## Feature Categories

- Core Rendering Features
- Texture Features
- Rendering Features
- Compute Features
- Advanced Features
- Memory and Performance Features

For detailed documentation including feature structures, detection flow, and implementation examples, please refer to the [Chinese version](../zh-CN/design/feature_detection.md).
