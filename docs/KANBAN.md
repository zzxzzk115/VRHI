# VRHI v1.0 Kanban Board

This document tracks all work items for VRHI v1.0 development. Tasks can be assigned to team members and tracked through completion.

## Legend

- **Status**: Backlog → In Progress → Review → Done
- **Priority**: P0 (Critical) → P1 (High) → P2 (Medium) → P3 (Low)
- **Assignee**: @username or Unassigned
- **Effort**: S (Small, <1 day) → M (Medium, 1-3 days) → L (Large, 4-7 days) → XL (Extra Large, >7 days)

---

## Phase 1-2: Foundation & Core Interface (Weeks 1-4)

### Build System & Project Structure

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| F1.1 | Create root CMakeLists.txt with C++23 config | P0 | S | Unassigned | Backlog |
| F1.2 | Set up `external/` directory structure | P0 | S | Unassigned | Backlog |
| F1.3 | Add Vulkan detection (FindVulkan.cmake) | P0 | S | Unassigned | Backlog |
| F1.4 | Add OpenGL detection | P0 | S | Unassigned | Backlog |
| F1.5 | Configure compile_commands.json export | P0 | S | Unassigned | Backlog |
| F1.6 | Create modular CMakeLists for src/ subdirs | P1 | M | Unassigned | Backlog |
| F1.7 | Set up build options (VRHI_BACKEND_*, etc.) | P1 | S | Unassigned | Backlog |
| F1.8 | Add platform detection (Windows/Linux/macOS) | P1 | S | Unassigned | Backlog |
| F1.9 | Configure install targets and exports | P2 | M | Unassigned | Backlog |

### Core Types & Error Handling

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| F2.1 | Implement `Error` class with error codes | P0 | S | Unassigned | Backlog |
| F2.2 | Create `BackendType` enum | P0 | S | Unassigned | Backlog |
| F2.3 | Create `Feature` enum (all features) | P0 | M | Unassigned | Backlog |
| F2.4 | Implement `DeviceConfig` struct | P0 | S | Unassigned | Backlog |
| F2.5 | Implement `FeatureRequirements` struct | P0 | S | Unassigned | Backlog |
| F2.6 | Create base types (Handle, Version, etc.) | P1 | S | Unassigned | Backlog |
| F2.7 | Add std::expected type aliases | P1 | S | Unassigned | Backlog |
| F2.8 | Write unit tests for error handling | P1 | M | Unassigned | Backlog |

### Device Abstraction (Interfaces)

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| F3.1 | Define `IBackend` interface | P0 | M | Unassigned | Backlog |
| F3.2 | Define `IDevice` interface | P0 | M | Unassigned | Backlog |
| F3.3 | Define `ICommandBuffer` interface | P0 | M | Unassigned | Backlog |
| F3.4 | Define `IBuffer` interface | P1 | S | Unassigned | Backlog |
| F3.5 | Define `ITexture` interface | P1 | S | Unassigned | Backlog |
| F3.6 | Define `IPipeline` interface | P1 | M | Unassigned | Backlog |
| F3.7 | Define `IShader` interface | P1 | S | Unassigned | Backlog |
| F3.8 | Create mock backend for testing | P1 | M | Unassigned | Backlog |

### Core API Functions

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| F4.1 | Implement `EnumerateBackends()` signature | P0 | S | Unassigned | Backlog |
| F4.2 | Implement `CreateDevice()` signature | P0 | S | Unassigned | Backlog |
| F4.3 | Add backend registry system | P0 | M | Unassigned | Backlog |
| F4.4 | Implement backend factory pattern | P0 | M | Unassigned | Backlog |
| F4.5 | Add initialization/shutdown functions | P1 | S | Unassigned | Backlog |

---

## Phase 3-4: Backend Implementation (Weeks 5-9)

### Backend Scoring & Selection

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| B1.1 | Implement backend scoring algorithm | P0 | M | Unassigned | Backlog |
| B1.2 | Add platform-native API bonus logic | P0 | S | Unassigned | Backlog |
| B1.3 | Implement feature support scoring (40%) | P0 | M | Unassigned | Backlog |
| B1.4 | Implement performance scoring (30%) | P1 | M | Unassigned | Backlog |
| B1.5 | Implement stability scoring (20%) | P1 | S | Unassigned | Backlog |
| B1.6 | Implement compatibility scoring (10%) | P1 | S | Unassigned | Backlog |
| B1.7 | Add backend priority lists per platform | P1 | S | Unassigned | Backlog |
| B1.8 | Write unit tests for scoring system | P1 | M | Unassigned | Backlog |

### Feature Detection

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| B2.1 | Implement `IsFeatureSupported()` interface | P0 | S | Unassigned | Backlog |
| B2.2 | Add required feature validation logic | P0 | M | Unassigned | Backlog |
| B2.3 | Implement feature query for OpenGL | P0 | L | Unassigned | Backlog |
| B2.4 | Implement feature query for Vulkan | P1 | L | Unassigned | Backlog |
| B2.5 | Add texture format capability queries | P1 | M | Unassigned | Backlog |
| B2.6 | Add compute shader detection | P1 | S | Unassigned | Backlog |
| B2.7 | Write tests for feature detection | P1 | M | Unassigned | Backlog |

### OpenGL 3.3 Backend (Full Implementation)

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| B3.1 | Integrate GLAD (glad_gl33) | P0 | M | Unassigned | Backlog |
| B3.2 | Implement OpenGL33Backend class | P0 | L | Unassigned | Backlog |
| B3.3 | Implement OpenGL33Device class | P0 | L | Unassigned | Backlog |
| B3.4 | Implement context creation | P0 | M | Unassigned | Backlog |
| B3.5 | Implement basic state management | P0 | M | Unassigned | Backlog |
| B3.6 | Add error checking/debugging | P1 | M | Unassigned | Backlog |
| B3.7 | Implement backend registration | P0 | S | Unassigned | Backlog |
| B3.8 | Write integration tests | P1 | L | Unassigned | Backlog |

### Basic Rendering (Triangle Example)

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| B4.1 | Create basic vertex buffer | P0 | M | Unassigned | Backlog |
| B4.2 | Create basic shader program | P0 | M | Unassigned | Backlog |
| B4.3 | Implement draw call | P0 | M | Unassigned | Backlog |
| B4.4 | Add clear/present operations | P0 | S | Unassigned | Backlog |
| B4.5 | Create HelloTriangle example | P0 | M | Unassigned | Backlog |
| B4.6 | Verify triangle renders correctly | P0 | S | Unassigned | Backlog |

---

## Phase 5-6: Resource Management & Multi-Backend (Weeks 10-13)

### RAII Resource Management

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| R1.1 | Implement Buffer class with RAII | P0 | M | Unassigned | Backlog |
| R1.2 | Implement Texture class with RAII | P0 | M | Unassigned | Backlog |
| R1.3 | Implement Shader class with RAII | P0 | M | Unassigned | Backlog |
| R1.4 | Implement Pipeline class with RAII | P0 | M | Unassigned | Backlog |
| R1.5 | Add resource descriptor structs | P0 | M | Unassigned | Backlog |
| R1.6 | Implement resource creation via Device | P0 | M | Unassigned | Backlog |
| R1.7 | Add proper cleanup/destruction | P0 | M | Unassigned | Backlog |
| R1.8 | Write resource lifecycle tests | P1 | L | Unassigned | Backlog |

### Buffer Implementation

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| R2.1 | Define BufferUsage flags | P0 | S | Unassigned | Backlog |
| R2.2 | Define BufferDesc struct | P0 | S | Unassigned | Backlog |
| R2.3 | Implement CreateBuffer() for OpenGL | P0 | M | Unassigned | Backlog |
| R2.4 | Implement buffer mapping/unmapping | P0 | M | Unassigned | Backlog |
| R2.5 | Add buffer update operations | P0 | M | Unassigned | Backlog |
| R2.6 | Implement vertex/index buffer binding | P0 | M | Unassigned | Backlog |
| R2.7 | Add uniform buffer support | P1 | M | Unassigned | Backlog |

### Texture Implementation

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| R3.1 | Define TextureFormat enum | P0 | S | Unassigned | Backlog |
| R3.2 | Define TextureUsage flags | P0 | S | Unassigned | Backlog |
| R3.3 | Define TextureDesc struct | P0 | M | Unassigned | Backlog |
| R3.4 | Implement CreateTexture() for OpenGL | P0 | M | Unassigned | Backlog |
| R3.5 | Add texture upload (1D/2D/3D) | P0 | L | Unassigned | Backlog |
| R3.6 | Implement sampler creation | P1 | M | Unassigned | Backlog |
| R3.7 | Add texture binding | P0 | S | Unassigned | Backlog |

### Shader Compilation

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| R4.1 | Integrate glslang library | P0 | M | Unassigned | Backlog |
| R4.2 | Integrate SPIRV-Cross library | P0 | M | Unassigned | Backlog |
| R4.3 | Implement GLSL → SPIR-V compilation | P0 | M | Unassigned | Backlog |
| R4.4 | Implement SPIR-V → GLSL conversion (GL) | P0 | M | Unassigned | Backlog |
| R4.5 | Implement shader reflection | P1 | L | Unassigned | Backlog |
| R4.6 | Add shader caching | P2 | M | Unassigned | Backlog |
| R4.7 | Create shader compilation example | P1 | M | Unassigned | Backlog |

### Pipeline State

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| R5.1 | Define PipelineDesc struct | P0 | M | Unassigned | Backlog |
| R5.2 | Define blend/raster/depth state | P0 | M | Unassigned | Backlog |
| R5.3 | Implement CreatePipeline() for OpenGL | P0 | L | Unassigned | Backlog |
| R5.4 | Add pipeline binding | P0 | M | Unassigned | Backlog |
| R5.5 | Implement state caching | P2 | M | Unassigned | Backlog |

### Vulkan Backend (Basic Skeleton)

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| R6.1 | Link Vulkan SDK | P0 | S | Unassigned | Backlog |
| R6.2 | Implement VulkanBackend class | P0 | M | Unassigned | Backlog |
| R6.3 | Implement VulkanDevice class | P0 | M | Unassigned | Backlog |
| R6.4 | Create Vulkan instance | P0 | M | Unassigned | Backlog |
| R6.5 | Enumerate physical devices | P0 | M | Unassigned | Backlog |
| R6.6 | Create logical device | P0 | M | Unassigned | Backlog |
| R6.7 | Implement basic buffer creation | P1 | M | Unassigned | Backlog |
| R6.8 | Verify Vulkan triangle renders | P1 | M | Unassigned | Backlog |

---

## Phase 7-8: Window System & Examples (Weeks 14-16)

### Window System Abstraction

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| W1.1 | Define IWindow interface | P0 | M | Unassigned | Backlog |
| W1.2 | Define WindowConfig struct | P0 | S | Unassigned | Backlog |
| W1.3 | Define WindowSystemType enum | P0 | S | Unassigned | Backlog |
| W1.4 | Implement WindowFactory | P0 | M | Unassigned | Backlog |
| W1.5 | Add platform-specific selection logic | P0 | S | Unassigned | Backlog |

### GLFW Integration

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| W2.1 | Integrate GLFW library (external/) | P0 | S | Unassigned | Backlog |
| W2.2 | Implement GLFWWindow class | P0 | M | Unassigned | Backlog |
| W2.3 | Add window creation/destruction | P0 | M | Unassigned | Backlog |
| W2.4 | Implement event polling | P0 | M | Unassigned | Backlog |
| W2.5 | Add callback system | P0 | M | Unassigned | Backlog |
| W2.6 | Implement CreateSurface() for OpenGL | P0 | M | Unassigned | Backlog |
| W2.7 | Implement CreateSurface() for Vulkan | P0 | M | Unassigned | Backlog |

### KeyCode Abstraction

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| W3.1 | Define KeyCode enum (platform-independent) | P0 | M | Unassigned | Backlog |
| W3.2 | Define KeyAction enum | P0 | S | Unassigned | Backlog |
| W3.3 | Implement GLFW → KeyCode mapping | P0 | M | Unassigned | Backlog |
| W3.4 | Add keyboard event callbacks | P0 | S | Unassigned | Backlog |
| W3.5 | Add mouse event callbacks | P1 | M | Unassigned | Backlog |
| W3.6 | Write event handling tests | P1 | M | Unassigned | Backlog |

### Example Applications

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| W4.1 | Create HelloTriangle example | P0 | M | Unassigned | Backlog |
| W4.2 | Create TexturedCube example | P0 | L | Unassigned | Backlog |
| W4.3 | Create FeatureDetection example | P1 | M | Unassigned | Backlog |
| W4.4 | Create MultiBackend example | P1 | M | Unassigned | Backlog |
| W4.5 | Add example READMEs | P1 | S | Unassigned | Backlog |
| W4.6 | Create example assets (shaders, textures) | P1 | M | Unassigned | Backlog |

---

## Phase 9-10: Testing, Polish & Release (Weeks 17-20)

### Unit Testing

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| T1.1 | Set up testing framework (Google Test) | P0 | S | Unassigned | Backlog |
| T1.2 | Write tests for core types | P0 | M | Unassigned | Backlog |
| T1.3 | Write tests for error handling | P0 | M | Unassigned | Backlog |
| T1.4 | Write tests for backend scoring | P0 | M | Unassigned | Backlog |
| T1.5 | Write tests for feature detection | P0 | M | Unassigned | Backlog |
| T1.6 | Write tests for resource management | P0 | L | Unassigned | Backlog |
| T1.7 | Achieve >80% code coverage | P0 | XL | Unassigned | Backlog |

### Integration Testing

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| T2.1 | Test OpenGL backend end-to-end | P0 | L | Unassigned | Backlog |
| T2.2 | Test Vulkan backend end-to-end | P0 | L | Unassigned | Backlog |
| T2.3 | Test window system integration | P0 | M | Unassigned | Backlog |
| T2.4 | Test multi-backend switching | P1 | M | Unassigned | Backlog |
| T2.5 | Test required feature validation | P1 | M | Unassigned | Backlog |
| T2.6 | Run all examples on all platforms | P0 | L | Unassigned | Backlog |

### Performance Optimization

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| T3.1 | Profile OpenGL backend overhead | P1 | M | Unassigned | Backlog |
| T3.2 | Profile Vulkan backend overhead | P1 | M | Unassigned | Backlog |
| T3.3 | Optimize hot paths (<5% overhead) | P1 | L | Unassigned | Backlog |
| T3.4 | Add performance benchmarks | P2 | M | Unassigned | Backlog |
| T3.5 | Document performance characteristics | P2 | S | Unassigned | Backlog |

### Quality Assurance

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| T4.1 | Fix all compiler warnings | P0 | M | Unassigned | Backlog |
| T4.2 | Run clang-tidy on all sources | P0 | M | Unassigned | Backlog |
| T4.3 | Run AddressSanitizer | P0 | M | Unassigned | Backlog |
| T4.4 | Run UndefinedBehaviorSanitizer | P0 | M | Unassigned | Backlog |
| T4.5 | Run memory leak detection (Valgrind) | P1 | M | Unassigned | Backlog |
| T4.6 | Fix all detected issues | P0 | L | Unassigned | Backlog |

### Documentation Completion

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| T5.1 | Review and update API documentation | P0 | M | Unassigned | Backlog |
| T5.2 | Add code comments to all public APIs | P0 | L | Unassigned | Backlog |
| T5.3 | Create API reference (Doxygen) | P1 | M | Unassigned | Backlog |
| T5.4 | Update README with build instructions | P0 | S | Unassigned | Backlog |
| T5.5 | Add troubleshooting guide | P1 | M | Unassigned | Backlog |
| T5.6 | Create migration guide (for future) | P2 | S | Unassigned | Backlog |

### Release Preparation

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| T6.1 | Create CHANGELOG.md | P0 | S | Unassigned | Backlog |
| T6.2 | Tag v1.0.0 release | P0 | S | Unassigned | Backlog |
| T6.3 | Create release notes | P0 | S | Unassigned | Backlog |
| T6.4 | Package source distribution | P1 | S | Unassigned | Backlog |
| T6.5 | Update project status in README | P0 | S | Unassigned | Backlog |

---

## CI/CD Enhancements (Progressive - Phase 2)

### Extended CI Pipeline

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| C1.1 | Add ARM64 cross-compilation (Linux) | P2 | M | Unassigned | Backlog |
| C1.2 | Add ARM64 cross-compilation (Windows) | P2 | M | Unassigned | Backlog |
| C1.3 | Add ARM64 native build (macOS) | P2 | S | Unassigned | Backlog |
| C1.4 | Add format checking job | P2 | S | Unassigned | Backlog |
| C1.5 | Add clang-tidy job | P2 | M | Unassigned | Backlog |
| C1.6 | Add test execution job | P1 | M | Unassigned | Backlog |
| C1.7 | Add code coverage job | P2 | M | Unassigned | Backlog |
| C1.8 | Add Codecov integration | P2 | S | Unassigned | Backlog |

---

## Backlog (Future/Optional)

### Additional Backends (v1.x)

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| B5.1 | Implement OpenGL 4.1 backend (macOS) | P3 | L | Unassigned | Backlog |
| B5.2 | Implement OpenGL 4.6 backend | P3 | L | Unassigned | Backlog |
| B5.3 | Implement OpenGL ES 3.0 backend | P3 | L | Unassigned | Backlog |
| B5.4 | Implement OpenGL ES 3.1 backend | P3 | L | Unassigned | Backlog |

### Additional Window Systems (v1.x)

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| W5.1 | Implement SDL2 window system | P3 | M | Unassigned | Backlog |
| W5.2 | Implement SDL3 window system | P3 | M | Unassigned | Backlog |
| W5.3 | Implement EGL window system | P3 | M | Unassigned | Backlog |

### Advanced Features (v2.0+)

| ID | Task | Priority | Effort | Assignee | Status |
|----|------|----------|--------|----------|--------|
| A1.1 | Implement Direct3D 12 backend | P3 | XL | Unassigned | Backlog |
| A1.2 | Implement Metal backend | P3 | XL | Unassigned | Backlog |
| A1.3 | Add MoltenVK integration (macOS/iOS) | P3 | L | Unassigned | Backlog |
| A1.4 | Implement compute shader support | P3 | XL | Unassigned | Backlog |
| A1.5 | Implement ray tracing support | P3 | XL | Unassigned | Backlog |
| A1.6 | Migrate to Slang shader language | P3 | XL | Unassigned | Backlog |

---

## Summary Statistics

**Total Tasks**: 196 tasks across 10 phases

**By Phase**:
- Phase 1-2 (Foundation): 27 tasks
- Phase 3-4 (Backend): 29 tasks
- Phase 5-6 (Resources): 39 tasks
- Phase 7-8 (Window/Examples): 24 tasks
- Phase 9-10 (Testing/Release): 39 tasks
- CI/CD Enhancements: 8 tasks
- Future/Optional: 30 tasks

**By Priority**:
- P0 (Critical): 109 tasks
- P1 (High): 51 tasks
- P2 (Medium): 20 tasks
- P3 (Low): 16 tasks

**By Effort**:
- S (Small): 57 tasks
- M (Medium): 97 tasks
- L (Large): 34 tasks
- XL (Extra Large): 8 tasks

---

## Usage Instructions

### Assigning Tasks

To assign a task to yourself or another team member:

1. Find the task by ID (e.g., `F1.1`)
2. Update the **Assignee** column from `Unassigned` to `@username`
3. Update the **Status** to `In Progress`
4. Commit the change with message: `Assign task F1.1 to @username`

### Updating Status

Task status flow:
1. **Backlog**: Task is defined but not started
2. **In Progress**: Task is actively being worked on
3. **Review**: Task is complete and awaiting code review
4. **Done**: Task is reviewed, tested, and merged

### Creating GitHub Issues

Each task can optionally be tracked as a GitHub issue:

```bash
# Example for task F1.1
gh issue create --title "F1.1: Create root CMakeLists.txt with C++23 config" \
  --label "phase-1,priority-P0,effort-S" \
  --body "Create the root CMakeLists.txt with C++23 standard configuration..."
```

### Weekly Planning

Recommended workflow:
1. Review backlog every Monday
2. Assign tasks for the week based on priority and capacity
3. Update status daily
4. Review completed tasks every Friday
5. Adjust priorities based on progress

---

Last Updated: 2026-01-17
