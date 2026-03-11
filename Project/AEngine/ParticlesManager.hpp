#ifndef PARTICLEMANAGER_HPP
#define PARTICLEMANAGER_HPP

#include "pch.hpp"
#include "Particles.hpp"
#include "CameraManager.hpp"

// Usage (per game-state):
//     ParticleManager::ParticleController::Init(2048);  // GameState Load/Init
//     ParticleManager::particleController.Emit(params); // wherever effects spawn
//     ParticleManager::particleController.Update(dt);   // GameState Update
//     ParticleManager::particleController.Draw();       // GameState Draw
//     ParticleManager::particleController.Shutdown();   // GameState Unload/Free

namespace ParticleManager {

    class ParticleController {
    public:

        // Allocate the particle pool and build the shared mesh.
        // Call once per game-state load.
        // poolSize: max concurrent particles (recommend power-of-two, e.g. 2048)
        void Init(int poolSize = 2048);

        // Release pool memory and the shared mesh.
        // Call once per game-state free.
        void Shutdown();

        // Advance all living particles by dt seconds.
        void Update(float dt);

        // Render all living particles with alpha blending.
        // Call inside your game-state Draw function.
        void Draw();

        // Spawn params.count new particles using the configuration in params.
        // Silently drops any particles that exceed the pool capacity.
        void Emit(const ParticleEmitParams& params);

        // Kill all living particles immediately (no Shutdown/Init needed).
        void Clear();

        // Returns the number of currently living particles.
        int ActiveCount() const;

        // Returns the total pool capacity set at Init time.
        int PoolCapacity() const { return poolSize; }

    private:
        std::vector<Particle>   pool{};     // Flat particle pool
        int                     poolSize{ 0 };  // Capacity
        int                     nextFree{ 0 };  // Rover index for free slot search

        AEGfxVertexList* mesh{ nullptr }; // Unit quad shared by all particles

        // Rover search for a dead slot; returns -1 if pool is full.
        int  FindFreeSlot();

        // Build the unit-square mesh used for rendering.
        void CreateMesh();

        // Returns a random float in [min, max].
        static float RandRange(float min, float max);
    };

    extern ParticleController particleController;

} // namespace ParticleManager

#endif // !PARTICLEMANAGER_HPP