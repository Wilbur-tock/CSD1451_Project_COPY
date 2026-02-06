# Particle System - Quick Start Guide

## Basic Usage

### 1. Include Headers
```cpp
#include "ParticleSystem.hpp"
#include "ParticleEffects.hpp"  // Optional: for pre-configured effects
```

### 2. Create Emitter
```cpp
// In your level/game state header or as a global
ParticleSystem::ParticleEmitter* myEmitter = nullptr;
```

### 3. Initialize in Init/Load Function
```cpp
void Init() {
    // Set emitter position
    AEVec2 position;
    AEVec2Set(&position, 100.0f, 200.0f);
    
    // Create emitter with max 200 particles
    myEmitter = new ParticleSystem::ParticleEmitter(position, 200);
    
    // Option A: Use pre-configured effect
    ParticleEffects::ConfigureFire(myEmitter);
    
    // Option B: Configure manually
    myEmitter->SetEmissionRate(30.0f);
    myEmitter->SetLifetimeRange(1.0f, 3.0f);
    myEmitter->SetSpeedRange(50.0f, 150.0f);
    myEmitter->SetSizeRange(8.0f, 15.0f);
    myEmitter->SetColor(Color(1.0f, 0.5f, 0.0f, 1.0f));
    myEmitter->SetGravity(50.0f);
    myEmitter->SetFadeOut(true);
}
```

### 4. Update Each Frame
```cpp
void Update() {
    if (myEmitter) {
        myEmitter->Update(GameStateManager::dt);
    }
}
```

### 5. Render Each Frame
```cpp
void Draw() {
    if (myEmitter) {
        myEmitter->Render();
    }
}
```

### 6. Clean Up
```cpp
void Free() {
    if (myEmitter) {
        delete myEmitter;
        myEmitter = nullptr;
    }
}
```

## Available Pre-configured Effects

```cpp
ParticleEffects::ConfigureFire(emitter);      // Orange fire particles
ParticleEffects::ConfigureSmoke(emitter);     // Gray smoke particles
ParticleEffects::ConfigureSparkle(emitter);   // White sparkle particles
ParticleEffects::ConfigureExplosion(emitter); // Orange explosion burst
ParticleEffects::ConfigureRain(emitter);      // Blue rain particles
ParticleEffects::ConfigureMagic(emitter);     // Purple magic particles
ParticleEffects::ConfigureDust(emitter);      // Brown dust particles
```

## Configuration Methods

| Method | Purpose | Example |
|--------|---------|---------|
| `SetEmissionRate(float)` | Particles spawned per second | `emitter->SetEmissionRate(30.0f);` |
| `SetLifetimeRange(min, max)` | Random lifetime in seconds | `emitter->SetLifetimeRange(1.0f, 3.0f);` |
| `SetSpeedRange(min, max)` | Random speed range | `emitter->SetSpeedRange(50.0f, 150.0f);` |
| `SetSizeRange(min, max)` | Random size range | `emitter->SetSizeRange(5.0f, 15.0f);` |
| `SetColor(Color)` | Particle color with alpha | `emitter->SetColor(Color(1.0f, 0.5f, 0.0f, 1.0f));` |
| `SetGravity(float)` | Gravity acceleration | `emitter->SetGravity(50.0f);` |
| `SetFadeOut(bool)` | Enable alpha fade | `emitter->SetFadeOut(true);` |
| `SetPosition(AEVec2)` | Move emitter | `emitter->SetPosition(newPos);` |
| `EmitParticle()` | Spawn one particle | `emitter->EmitParticle();` |
| `GetActiveParticleCount()` | Get active count | `int count = emitter->GetActiveParticleCount();` |

## Common Patterns

### One-Time Explosion
```cpp
emitter->SetEmissionRate(0.0f);  // Disable continuous emission
// Emit burst
for (int i = 0; i < 50; ++i) {
    emitter->EmitParticle();
}
```

### Floating Particles (No Gravity)
```cpp
emitter->SetGravity(0.0f);
```

### Rising Particles (Negative Gravity)
```cpp
emitter->SetGravity(-30.0f);
```

### Dynamic Position (Follow Player)
```cpp
void Update() {
    AEVec2 playerPos = GetPlayerPosition();
    emitter->SetPosition(playerPos);
    emitter->Update(GameStateManager::dt);
}
```

### Multiple Emitters
```cpp
ParticleSystem::ParticleEmitter* fireEmitter = nullptr;
ParticleSystem::ParticleEmitter* smokeEmitter = nullptr;

void Init() {
    AEVec2 pos;
    AEVec2Set(&pos, 100.0f, 200.0f);
    
    fireEmitter = new ParticleSystem::ParticleEmitter(pos, 150);
    ParticleEffects::ConfigureFire(fireEmitter);
    
    // Offset smoke slightly above fire
    AEVec2 smokePos;
    AEVec2Set(&smokePos, 100.0f, 250.0f);
    smokeEmitter = new ParticleSystem::ParticleEmitter(smokePos, 100);
    ParticleEffects::ConfigureSmoke(smokeEmitter);
}

void Update() {
    if (fireEmitter) fireEmitter->Update(GameStateManager::dt);
    if (smokeEmitter) smokeEmitter->Update(GameStateManager::dt);
}

void Draw() {
    if (fireEmitter) fireEmitter->Render();
    if (smokeEmitter) smokeEmitter->Render();
}

void Free() {
    delete fireEmitter;
    delete smokeEmitter;
    fireEmitter = nullptr;
    smokeEmitter = nullptr;
}
```

## Performance Tips

1. **Max Particles**: Start with 100-200, increase only if needed
2. **Emission Rate**: Lower rates (10-50/sec) are more efficient
3. **Multiple Emitters**: Use sparingly (3-5 max recommended)
4. **Update Only When Visible**: Check if emitter is in camera view
5. **Pool Reuse**: The system automatically reuses inactive particles

## Troubleshooting

**Particles not showing?**
- Check that `Render()` is called in Draw phase
- Verify emitter position is within camera view
- Ensure emission rate > 0 for continuous emission

**Too many particles?**
- Reduce emission rate
- Decrease max particles in constructor
- Reduce particle lifetime

**Particles look wrong?**
- Check color alpha value (should be > 0)
- Verify size range is reasonable (5-20 for most effects)
- Make sure gravity is set correctly

## Example: Cannon Fire Effect
```cpp
// On cannon fire
void OnCannonFire(AEVec2 cannonPos) {
    // Create temporary explosion at cannon
    ParticleSystem::ParticleEmitter* explosion = 
        new ParticleSystem::ParticleEmitter(cannonPos, 100);
    ParticleEffects::ConfigureExplosion(explosion);
    
    // Note: You'll need to manage this emitter's lifetime
    // Consider storing it and deleting after particles die
}
```

For complete documentation, see `PARTICLE_SYSTEM_README.md`.
