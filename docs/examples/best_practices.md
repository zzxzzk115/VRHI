# VRHI Best Practices

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/examples/best_practices.md)

## Resource Management

### 1. Use RAII and Smart Pointers

✅ **Recommended**:
```cpp
{
    auto buffer = device->CreateBuffer(desc).value();
    // Use buffer
} // Automatic release
```

### 2. Batch Create Resources

Create resources in batches to reduce overhead.

### 3. Reuse Resources

Use resource pools to avoid frequent allocation/deallocation.

## Performance Optimization

### 1. Minimize State Changes

Group draw calls by pipeline and material to reduce state switching.

### 2. Use Correct Memory Access Pattern

Choose appropriate memory access modes based on usage patterns.

### 3. Batch Draw Calls

Use instancing and indirect drawing to reduce draw call overhead.

### 4. Async Resource Loading

Load resources asynchronously on background threads.

## Error Handling

Always check return values and provide graceful degradation.

## Multi-Threading

Use independent command buffers per thread; resource creation is thread-safe.

For detailed documentation including code examples, debugging tips, and platform-specific best practices, please refer to the [Chinese version](../zh-CN/examples/best_practices.md).
