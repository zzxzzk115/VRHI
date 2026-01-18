# TexturedCube Example

This example demonstrates 3D rendering with texture mapping using VRHI. It renders a rotating textured cube with proper depth testing and basic directional lighting.

## Features Demonstrated

- **3D Geometry**: Cube mesh with position, normal, and UV coordinates
- **Index Buffers**: Efficient rendering using indexed geometry
- **Texture Loading**: Loading PNG textures using stb_image
- **Texture Sampling**: Creating and configuring samplers for texture filtering
- **Depth Testing**: Proper 3D depth buffering
- **MVP Transformations**: Model-View-Projection matrix calculations
- **Animation**: Smooth rotation based on elapsed time
- **Simple Lighting**: Basic directional lighting in fragment shader

## Building

The example is built automatically when you build VRHI with examples enabled:

```bash
cmake -B build -DVRHI_BUILD_EXAMPLES=ON
cmake --build build --target TexturedCube
```

## Running

From the VRHI root directory:

```bash
./build/bin/examples/TexturedCube
```

## Requirements

- OpenGL 3.3 or higher
- The texture asset must be present at: `assets/textures/awesomeface.png`

## Code Structure

### Vertex Data

Each vertex contains:
- Position (vec3): x, y, z coordinates
- Normal (vec3): surface normal for lighting
- UV coordinates (vec2): texture coordinates

### Shaders

**Vertex Shader**:
- Transforms vertices using MVP matrix
- Passes through normals and texture coordinates

**Fragment Shader**:
- Samples the texture at the given UV coordinates
- Applies simple directional lighting based on surface normal
- Outputs the final color

### Rendering Loop

1. Update MVP matrix based on elapsed time
2. Clear color and depth buffers
3. Set viewport and scissor rectangles
4. Bind pipeline with shaders
5. Bind vertex and index buffers
6. Draw the cube (36 indices)
7. Present the frame

## Controls

- **ESC** or close window to exit

## Learning Points

This example is useful for learning:
- How to set up 3D geometry with multiple vertex attributes
- How to load and use textures in VRHI
- How to create and configure samplers
- How to implement depth testing
- How to perform 3D transformations with matrices
- Basic lighting techniques in shaders
