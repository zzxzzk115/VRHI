# Backend Scoring System Design

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/design/backend_scoring.md)

## Overview

One of VRHI's core innovations is its smart backend selection system. During initialization, VRHI detects all available backends, queries hardware-supported features, scores each backend based on application requirements, and selects the highest-scoring backend.

## Design Goals

1. **Automation**: No manual backend selection needed; system automatically chooses optimal solution
2. **Intelligence**: Considers features, performance, stability, and other factors
3. **Customizable**: Allows applications to specify required features and preferences
4. **Transparent**: Provides scoring details for debugging and optimization

## Scoring Dimensions

### 1. Feature Support Score (Weight: 40%)

Scores based on quantity and quality of features supported by the backend.

**Required Features (Must-Have)**:
- If any required feature is not supported, backend score is 0 and is excluded
- All backends must support all required features to be considered

**Optional Features (Nice-to-Have)**:
- Each supported optional feature adds bonus points
- More supported optional features = higher score

### 2. Performance Score (Weight: 30%)

Based on backend's theoretical performance and known benchmark results.

### 3. Stability Score (Weight: 20%)

Based on backend's maturity and known issues.

### 4. Compatibility Score (Weight: 10%)

Device and driver support for the backend.

## Scoring Algorithm

```
Total Score = (Feature Score × 0.4) + 
              (Performance Score × 0.3) + 
              (Stability Score × 0.2) + 
              (Compatibility Score × 0.1)
```

## Required Feature Validation

### Design Principles

For applications that explicitly require specific features, VRHI provides strict required feature validation:

1. **Strict Validation**: All features in the `required` list must be supported by the selected backend
2. **Fail Fast**: If no backend can satisfy all required features, immediately return an error without initialization
3. **Clear Error Messages**: Clearly indicate which required features are not supported by any backend
4. **Application Control**: Application developers decide which features are required, not VRHI

### Validation Flow

```
1. Application specifies required feature list
   ↓
2. VRHI enumerates all available backends
   ↓
3. For each backend, check:
   Does it support all required features?
   ├─ Yes → Keep backend, continue scoring
   └─ No → Exclude backend
   ↓
4. Check candidate backend list:
   ├─ Empty → Return Error::NoCompatibleBackend
   │          with details of missing features
   └─ Not empty → Continue backend scoring and selection
```

### Error Handling

When no backend satisfies all required features, `CreateDevice` returns an error:

**Error Code**: `Error::Code::NoCompatibleBackend`

**Error message includes**:
- List of missing required features
- Details of unsupported features for each backend
- Suggested alternatives (e.g., downgrade certain feature requirements)

### Code Example

```cpp
// Define strict feature requirements
FeatureRequirements requirements;
requirements.required = {
    Feature::Compute,           // Required: Compute shaders
    Feature::Texture3D,         // Required: 3D textures
    Feature::MultiDrawIndirect, // Required: Multi-draw indirect
};

auto deviceResult = VRHI::CreateDevice(requirements);

if (!deviceResult) {
    // Handle required features not satisfied
    const auto& error = deviceResult.error();
    
    if (error.code == Error::Code::NoCompatibleBackend) {
        std::cerr << "Error: No backend supports all required features\n";
        std::cerr << error.message << "\n";
        
        // Application should:
        // 1. Notify user hardware doesn't meet minimum requirements
        // 2. Provide alternatives (e.g., disable certain features)
        // 3. Or exit gracefully
        
        return EXIT_FAILURE;
    }
}

auto device = std::move(*deviceResult);
// Device is guaranteed to support all required features
```

### Validation Guarantees

Once `CreateDevice` returns successfully, application can be confident that:

1. **All required features available**: All features in `required` list are supported by selected backend
2. **Runtime safety**: Can directly use these features without additional checks
3. **Consistency**: These features remain available throughout device lifetime

### Best Practices

1. **Specify only truly required features**: Only list features that are absolutely necessary
   ```cpp
   // ✅ Good - only truly required
   requirements.required = {
       Feature::Texture2D,
       Feature::VertexBuffers,
   };
   
   // ❌ Avoid - listing all desired features
   requirements.required = {
       Feature::RayTracing,        // May be too strict
       Feature::MeshShading,       // Most devices don't support
       Feature::BindlessResources,
   };
   ```

2. **Use optional features**: For non-critical functionality, use `optional`
   ```cpp
   requirements.required = {
       Feature::Texture2D,  // Must have
   };
   requirements.optional = {
       Feature::Compute,    // Nice to have, can work without
       Feature::Geometry,
   };
   ```

3. **Provide fallback path**: Prepare alternative implementation
   ```cpp
   auto device = VRHI::CreateDevice(highEndRequirements);
   
   if (!device) {
       // Try downgraded requirements
       auto fallbackDevice = VRHI::CreateDevice(basicRequirements);
       if (fallbackDevice) {
           // Use basic feature mode
       }
   }
   ```

For detailed documentation including complete code examples, platform-specific considerations, and debugging support, please refer to the [Chinese version](../zh-CN/design/backend_scoring.md).
