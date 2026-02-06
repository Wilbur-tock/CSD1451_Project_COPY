#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include "pch.hpp"
#include "Color.hpp"

namespace ParticleSystem {
	// Single particle structure
	struct Particle {
		AEVec2 position;        // Current position
		AEVec2 velocity;        // Movement velocity
		float lifetime;         // Current remaining lifetime
		float maxLifetime;      // Initial lifetime for fade calculations
		Color color;            // Particle color with alpha
		float size;             // Particle size
		bool active;            // Is particle currently active

		Particle() : lifetime(0.0f), maxLifetime(0.0f), size(1.0f), active(false) {
			AEVec2Zero(&position);
			AEVec2Zero(&velocity);
		}
	};

	// Particle emitter class
	class ParticleEmitter {
	private:
		std::vector<Particle> particles;
		AEVec2 emitterPosition;
		int maxParticles;
		float emissionRate;         // Particles per second
		float emissionTimer;        // Internal timer for emission
		AEGfxVertexList* particleMesh;

		// Particle spawning parameters
		float minLifetime, maxLifetime;
		float minSpeed, maxSpeed;
		float minSize, maxSize;
		Color startColor;
		bool fadeOut;               // Should particles fade as they age

		// Create the mesh for rendering particles
		void CreateParticleMesh();

	public:
		ParticleEmitter(AEVec2 position, int maxParticles = 100);
		~ParticleEmitter();

		// Configuration methods
		void SetEmissionRate(float particlesPerSecond);
		void SetLifetimeRange(float minLife, float maxLife);
		void SetSpeedRange(float minSpd, float maxSpd);
		void SetSizeRange(float minSz, float maxSz);
		void SetColor(Color col);
		void SetFadeOut(bool fade);
		void SetPosition(AEVec2 pos);

		// Update and render
		void Update(float deltaTime);
		void Render();

		// Emit a single particle immediately
		void EmitParticle();

		// Get particle count
		int GetActiveParticleCount() const;
	};
}

#endif // !PARTICLESYSTEM_HPP
