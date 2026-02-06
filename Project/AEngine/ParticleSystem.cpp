#include "ParticleSystem.hpp"
#include "GameStateManager.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>

namespace ParticleSystem {

	// Constants
	// const float PI = 3.14159265f;
	const float DEFAULT_GRAVITY = 50.0f;

	// Helper function to generate random float in range
	static float RandomFloat(float min, float max) {
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		return min + random * (max - min);
	}

	// Helper function to generate random angle
	static float RandomAngle() {
		return RandomFloat(0.0f, 2.0f * PI);
	}

	ParticleEmitter::ParticleEmitter(AEVec2 position, int maxParticles) 
		: emitterPosition(position), 
		  maxParticles(maxParticles), 
		  emissionRate(10.0f),
		  emissionTimer(0.0f),
		  particleMesh(nullptr),
		  gravity(DEFAULT_GRAVITY),
		  minLifetime(1.0f), maxLifetime(2.0f),
		  minSpeed(50.0f), maxSpeed(100.0f),
		  minSize(5.0f), maxSize(10.0f),
		  startColor(Color(1.0f, 1.0f, 1.0f, 1.0f)),
		  fadeOut(true) {
		
		particles.resize(maxParticles);
		CreateParticleMesh();
		
		// Seed random number generator once per process
		static bool seeded = []() {
			srand(static_cast<unsigned int>(time(nullptr)));
			return true;
		}();
		(void)seeded; // Suppress unused variable warning
	}

	ParticleEmitter::~ParticleEmitter() {
		if (particleMesh) {
			AEGfxMeshFree(particleMesh);
			particleMesh = nullptr;
		}
	}

	void ParticleEmitter::CreateParticleMesh() {
		// Create a simple square mesh for particles
		AEGfxMeshStart();

		// Create a centered square
		AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
					 0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
					-0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

		AEGfxTriAdd( 0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
					 0.5f,  0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
					-0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

		particleMesh = AEGfxMeshEnd();
	}

	void ParticleEmitter::SetEmissionRate(float particlesPerSecond) {
		emissionRate = particlesPerSecond;
	}

	void ParticleEmitter::SetLifetimeRange(float minLife, float maxLife) {
		minLifetime = minLife;
		maxLifetime = maxLife;
	}

	void ParticleEmitter::SetSpeedRange(float minSpd, float maxSpd) {
		minSpeed = minSpd;
		maxSpeed = maxSpd;
	}

	void ParticleEmitter::SetSizeRange(float minSz, float maxSz) {
		minSize = minSz;
		maxSize = maxSz;
	}

	void ParticleEmitter::SetColor(Color col) {
		startColor = col;
	}

	void ParticleEmitter::SetFadeOut(bool fade) {
		fadeOut = fade;
	}

	void ParticleEmitter::SetPosition(AEVec2 pos) {
		emitterPosition = pos;
	}

	void ParticleEmitter::SetGravity(float gravityAccel) {
		gravity = gravityAccel;
	}

	void ParticleEmitter::EmitParticle() {
		// Find an inactive particle
		for (int i = 0; i < maxParticles; ++i) {
			if (!particles[i].active) {
				Particle& p = particles[i];
				
				// Initialize particle
				p.active = true;
				p.position = emitterPosition;
				
				// Random velocity direction and speed
				float angle = RandomAngle();
				float speed = RandomFloat(minSpeed, maxSpeed);
				AEVec2Set(&p.velocity, cosf(angle) * speed, sinf(angle) * speed);
				
				// Random lifetime and size
				p.lifetime = RandomFloat(minLifetime, maxLifetime);
				p.maxLifetime = p.lifetime;
				p.size = RandomFloat(minSize, maxSize);
				p.color = startColor;
				
				break;
			}
		}
	}

	void ParticleEmitter::Update(float deltaTime) {
		// Update emission timer and emit particles
		emissionTimer += deltaTime;
		float emissionInterval = 1.0f / emissionRate;
		
		while (emissionTimer >= emissionInterval && emissionRate > 0.0f) {
			EmitParticle();
			emissionTimer -= emissionInterval;
		}

		// Update all active particles
		for (int i = 0; i < maxParticles; ++i) {
			Particle& p = particles[i];
			
			if (p.active) {
				// Update lifetime
				p.lifetime -= deltaTime;
				
				// Deactivate if dead
				if (p.lifetime <= 0.0f) {
					p.active = false;
					continue;
				}
				
				// Update position
				AEVec2 displacement;
				AEVec2Scale(&displacement, &p.velocity, deltaTime);
				AEVec2Add(&p.position, &p.position, &displacement);
				
				// Apply gravity (simple physics)
				p.velocity.y -= gravity * deltaTime;
				
				// Update alpha for fade out effect
				if (fadeOut) {
					float lifeRatio = p.lifetime / p.maxLifetime;
					p.color.SetAlpha(startColor.GetAlpha() * lifeRatio);
				}
			}
		}
	}

	void ParticleEmitter::Render() {
		// Set render mode for colored particles
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);

		// Render all active particles
		for (int i = 0; i < maxParticles; ++i) {
			const Particle& p = particles[i];
			
			if (p.active) {
				// Create transform matrix for this particle
				AEMtx33 transform, scale, trans;
				AEMtx33Scale(&scale, p.size, p.size);
				AEMtx33Trans(&trans, p.position.x, p.position.y);
				AEMtx33Concat(&transform, &trans, &scale);
				
				// Set particle color and transparency
				AEGfxSetTransparency(p.color.GetAlpha());
				AEGfxSetColorToMultiply(p.color.GetRed(), p.color.GetGreen(), 
										 p.color.GetBlue(), p.color.GetAlpha());
				AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
				
				// Set transform and draw
				AEGfxSetTransform(transform.m);
				AEGfxMeshDraw(particleMesh, AE_GFX_MDM_TRIANGLES);
			}
		}
	}

	int ParticleEmitter::GetActiveParticleCount() const {
		int count = 0;
		for (int i = 0; i < maxParticles; ++i) {
			if (particles[i].active) {
				count++;
			}
		}
		return count;
	}
}
