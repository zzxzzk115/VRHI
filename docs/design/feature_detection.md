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

## Required Feature Validation

### Validation Mechanism

Feature detection is not only used for backend scoring but also for validating application's required features:

```cpp
// Application defines required features
FeatureRequirements requirements;
requirements.required = {
    Feature::Compute,
    Feature::Texture3D,
};

// VRHI validates during initialization
auto deviceResult = VRHI::CreateDevice(requirements);

if (!deviceResult) {
    // Error: No backend supports all required features
    std::cerr << deviceResult.error().message << "\n";
    // Application should exit or downgrade functionality
}
```

### Handling Validation Failures

When required features are not satisfied, applications should:

1. **Identify failure reason**: Check error message to see which features are unsupported
2. **Notify user**: Explain hardware doesn't meet requirements
3. **Provide alternatives**: If possible, downgrade to basic functionality
4. **Log diagnostics**: Help with troubleshooting

### Runtime Feature Checks

While required features are validated at initialization, optional features can be checked at runtime:

```cpp
auto device = VRHI::CreateDevice(requirements).value();

// Runtime check for optional features
if (device->IsFeatureSupported(Feature::RayTracing)) {
    EnableRayTracedShadows();
} else {
    UseTraditionalShadows();
}
```

For detailed documentation including feature structures, detection flow, implementation examples, and best practices, please refer to the [Chinese version](../zh-CN/design/feature_detection.md).
