# Command Recording API

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/api/commands.md)

## Overview

CommandBuffer is used to record rendering and compute commands, then submit them to the GPU for execution. VRHI's command buffer design is concise and efficient, supporting command recording and replay.

## CommandBuffer Class

The CommandBuffer interface provides methods for:

- **Render Passes**: BeginRenderPass, EndRenderPass
- **Pipeline Binding**: BindPipeline
- **Resource Binding**: BindVertexBuffer, BindIndexBuffer, BindTexture, etc.
- **Viewport and Scissor**: SetViewport, SetScissor
- **Draw Commands**: Draw, DrawIndexed, DrawIndirect
- **Compute Commands**: Dispatch, DispatchIndirect
- **Resource Operations**: CopyBuffer, CopyTexture, etc.
- **Clear Operations**: ClearColor, ClearDepth, ClearStencil
- **Synchronization**: Barrier
- **Debug Markers**: BeginDebugMarker, EndDebugMarker

For detailed documentation including enumeration types, usage examples, and best practices, please refer to the [Chinese version](../zh-CN/api/commands.md).
