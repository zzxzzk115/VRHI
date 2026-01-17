# VRHI Examples

This directory contains example applications demonstrating how to use VRHI.

## Planned Examples

### HelloTriangle
A minimal example that renders a single triangle using VRHI. Demonstrates:
- Device creation and backend selection
- Buffer creation (vertex buffer)
- Shader compilation
- Pipeline setup
- Command recording and submission
- Frame presentation

### TextureMapping
Demonstrates texture creation, loading, and sampling:
- Texture creation and upload
- Sampler configuration
- Texture binding in shaders
- Mipmapping

### ComputeShader
Shows how to use compute shaders:
- Compute pipeline setup
- Storage buffer usage
- GPU computation
- Result readback

### MultiBackend
Demonstrates backend enumeration and selection:
- Listing available backends
- Backend scoring and selection
- Feature detection
- Manual backend selection

### AdvancedRendering
Advanced rendering techniques:
- Multiple render targets
- Post-processing effects
- Instancing
- Indirect drawing

## Building Examples

Examples are excluded from the default build target. To build them:

```bash
# Configure with examples enabled
cmake -B build -DVRHI_BUILD_EXAMPLES=ON

# Build all examples
cmake --build build

# Or build a specific example
cmake --build build --target HelloTriangle
```

## Current Status

🚧 **Examples will be added during implementation phase**

Examples will be implemented once the core API and at least one backend are functional.
