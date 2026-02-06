# Particle System Implementation

## Overview
This document describes the particle system implementation for the CSD1451_Project using the Alpha Engine framework.

## Implementation Files
- **ParticleSystem.hpp** - Header file containing particle structure and emitter class declarations
- **ParticleSystem.cpp** - Implementation file with particle logic and rendering
- **Level1.cpp** - Integration example showing particle system in action

## Architecture

### Particle Structure
Each particle contains:
- **Position** (AEVec2): Current x, y coordinates in world space
- **Velocity** (AEVec2): Movement speed and direction vector
- **Lifetime** (float): Remaining time before particle expires
- **MaxLifetime** (float): Initial lifetime used for fade calculations
- **Color** (Color): RGBA color with alpha channel for transparency
- **Size** (float): Particle render size
- **Active** (bool): Flag indicating if particle is currently in use

### ParticleEmitter Class
The `ParticleEmitter` manages a pool of particles with the following features:

#### Constructor
```cpp
ParticleEmitter(AEVec2 position, int maxParticles = 100)
```
- `position`: Initial emitter location in world space
- `maxParticles`: Maximum number of particles (default: 100)

#### Configuration Methods
- `SetEmissionRate(float particlesPerSecond)` - Controls spawn rate
- `SetLifetimeRange(float minLife, float maxLife)` - Random lifetime range
- `SetSpeedRange(float minSpd, float maxSpd)` - Random speed range
- `SetSizeRange(float minSz, float maxSz)` - Random size range
- `SetColor(Color col)` - Base particle color
- `SetFadeOut(bool fade)` - Enable/disable alpha fading
- `SetPosition(AEVec2 pos)` - Update emitter position

#### Core Methods
- `Update(float deltaTime)` - Updates all particles and spawns new ones
- `Render()` - Renders all active particles
- `EmitParticle()` - Manually spawn a single particle
- `GetActiveParticleCount()` - Returns number of active particles

## Features

### 1. Object Pooling
- Uses a pre-allocated vector of particles for efficiency
- Inactive particles are reused rather than allocated/deallocated
- Prevents memory fragmentation and improves performance

### 2. Automatic Emission
- Configurable emission rate (particles per second)
- Internal timer manages automatic particle spawning
- Can be set to 0 to disable automatic emission

### 3. Physics Simulation
- Particles move based on velocity vectors
- Simple gravity applied to all particles (50 units/sec²)
- Position updated each frame: `position += velocity * deltaTime`

### 4. Lifetime Management
- Each particle has a lifetime countdown
- Particles automatically deactivate when lifetime reaches 0
- Lifetime is randomized within configured range

### 5. Alpha Blending and Fade Out
- Particles fade out as they age (optional)
- Alpha calculated as: `alpha = (lifetime / maxLifetime) * startAlpha`
- Uses Alpha Engine's blend mode: `AE_GFX_BM_BLEND`

### 6. Randomization
- Random direction: 360° circular distribution
- Random speed: Within configured min/max range
- Random lifetime: Within configured min/max range
- Random size: Within configured min/max range

## Integration Example

### In Level1.cpp:
```cpp
#include "ParticleSystem.hpp"

// Global emitter pointer
ParticleSystem::ParticleEmitter* particleEmitter = nullptr;

void Init() {
    // Create emitter at specific position
    AEVec2 emitterPos;
    AEVec2Set(&emitterPos, 100.0f, 200.0f);
    particleEmitter = new ParticleSystem::ParticleEmitter(emitterPos, 200);
    
    // Configure properties
    particleEmitter->SetEmissionRate(30.0f);
    particleEmitter->SetLifetimeRange(1.0f, 3.0f);
    particleEmitter->SetSpeedRange(80.0f, 150.0f);
    particleEmitter->SetSizeRange(8.0f, 15.0f);
    particleEmitter->SetColor(Color(1.0f, 0.7f, 0.2f, 1.0f));
    particleEmitter->SetFadeOut(true);
}

void Update() {
    // Update particles each frame
    if (particleEmitter) {
        particleEmitter->Update(GameStateManager::dt);
    }
}

void Draw() {
    // Render particles
    if (particleEmitter) {
        particleEmitter->Render();
    }
}

void Free() {
    // Clean up
    if (particleEmitter) {
        delete particleEmitter;
        particleEmitter = nullptr;
    }
}
```

## Alpha Engine API Usage

### Rendering Functions
- `AEGfxSetRenderMode(AE_GFX_RM_COLOR)` - Color-based rendering
- `AEGfxSetBlendMode(AE_GFX_BM_BLEND)` - Alpha blending
- `AEGfxSetTransparency(float)` - Set alpha transparency
- `AEGfxSetColorToMultiply(r, g, b, a)` - Set particle color
- `AEGfxSetTransform(matrix)` - Apply transform matrix
- `AEGfxMeshDraw(mesh, mode)` - Draw particle mesh

### Math Functions
- `AEVec2Set(vec, x, y)` - Set vector components
- `AEVec2Add(result, vec1, vec2)` - Vector addition
- `AEVec2Scale(result, vec, scalar)` - Vector scaling
- `AEMtx33Scale(matrix, scaleX, scaleY)` - Create scale matrix
- `AEMtx33Trans(matrix, x, y)` - Create translation matrix
- `AEMtx33Concat(result, mat1, mat2)` - Concatenate matrices

## Performance Considerations

### Efficiency Optimizations
1. **Object Pooling**: Pre-allocated particle array prevents runtime allocations
2. **Early Exit**: Inactive particles skip processing
3. **Batch Rendering**: All particles rendered in single render pass
4. **Fixed Array Size**: Vector sized once at initialization

### Scalability
- Default max particles: 100
- Can be increased for denser effects
- Recommended max: 500 particles per emitter for 60 FPS
- Multiple emitters can be used for different effects

## Extending the System

### Adding New Features
1. **Multiple Particle Shapes**: Modify `CreateParticleMesh()` to support different shapes
2. **Texture Support**: Change render mode to `AE_GFX_RM_TEXTURE` and load textures
3. **Force Fields**: Add external forces in `Update()` (wind, attraction, etc.)
4. **Collision**: Check particle positions against level geometry
5. **Particle Types**: Add enum to differentiate particle behaviors
6. **Emitter Shapes**: Support line, circle, or area emission patterns

### Custom Effects Examples
```cpp
// Fire effect
emitter->SetColor(Color(1.0f, 0.4f, 0.1f, 1.0f));
emitter->SetLifetimeRange(0.5f, 1.5f);
emitter->SetSpeedRange(50.0f, 100.0f);
emitter->SetEmissionRate(50.0f);

// Smoke effect
emitter->SetColor(Color(0.3f, 0.3f, 0.3f, 0.5f));
emitter->SetLifetimeRange(2.0f, 4.0f);
emitter->SetSpeedRange(20.0f, 40.0f);
emitter->SetSizeRange(15.0f, 30.0f);

// Sparkles effect
emitter->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
emitter->SetLifetimeRange(0.3f, 0.8f);
emitter->SetSpeedRange(100.0f, 200.0f);
emitter->SetSizeRange(3.0f, 6.0f);
```

## Testing

### Verification Steps
1. Build the project in Visual Studio (Debug or Release configuration)
2. Run the application
3. Navigate to Level 1
4. Observe orange particles spawning and falling with gravity
5. Enable debug mode to see active particle count
6. Particles should fade out as they age

### Expected Behavior
- Particles spawn continuously at the emitter position
- Each particle moves in a random direction
- Gravity pulls particles downward
- Particles fade out and disappear after 1-3 seconds
- Active particle count displayed in debug text
- Smooth 60 FPS performance with 200 particles

## Build Configuration
The particle system files have been added to `AEngine.vcxproj`:
```xml
<ClCompile Include="ParticleSystem.cpp" />
<ClInclude Include="ParticleSystem.hpp" />
```

## Code Style
The implementation follows the existing project conventions:
- Namespace organization (`ParticleSystem::`)
- Alpha Engine API usage patterns
- Resource management (RAII with destructor cleanup)
- Consistent naming conventions (camelCase for methods, parameters)
- Comments explaining key functionality
