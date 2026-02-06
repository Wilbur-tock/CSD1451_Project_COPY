#ifndef PARTICLEEFFECTS_HPP
#define PARTICLEEFFECTS_HPP

#include "ParticleSystem.hpp"

// Collection of pre-configured particle effects for easy use
namespace ParticleEffects {
	
	// Configure emitter for fire effect
	inline void ConfigureFire(ParticleSystem::ParticleEmitter* emitter) {
		emitter->SetEmissionRate(50.0f);
		emitter->SetLifetimeRange(0.5f, 1.5f);
		emitter->SetSpeedRange(50.0f, 100.0f);
		emitter->SetSizeRange(10.0f, 20.0f);
		emitter->SetColor(Color(1.0f, 0.4f, 0.1f, 1.0f));  // Orange-red
		emitter->SetFadeOut(true);
	}
	
	// Configure emitter for smoke effect
	inline void ConfigureSmoke(ParticleSystem::ParticleEmitter* emitter) {
		emitter->SetEmissionRate(20.0f);
		emitter->SetLifetimeRange(2.0f, 4.0f);
		emitter->SetSpeedRange(20.0f, 40.0f);
		emitter->SetSizeRange(15.0f, 30.0f);
		emitter->SetColor(Color(0.3f, 0.3f, 0.3f, 0.5f));  // Gray
		emitter->SetFadeOut(true);
	}
	
	// Configure emitter for sparkle effect
	inline void ConfigureSparkle(ParticleSystem::ParticleEmitter* emitter) {
		emitter->SetEmissionRate(30.0f);
		emitter->SetLifetimeRange(0.3f, 0.8f);
		emitter->SetSpeedRange(100.0f, 200.0f);
		emitter->SetSizeRange(3.0f, 6.0f);
		emitter->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));  // White
		emitter->SetFadeOut(true);
	}
	
	// Configure emitter for explosion effect (one-time burst)
	inline void ConfigureExplosion(ParticleSystem::ParticleEmitter* emitter) {
		emitter->SetEmissionRate(0.0f);  // Disable automatic emission
		emitter->SetLifetimeRange(0.5f, 1.5f);
		emitter->SetSpeedRange(150.0f, 300.0f);
		emitter->SetSizeRange(5.0f, 15.0f);
		emitter->SetColor(Color(1.0f, 0.5f, 0.0f, 1.0f));  // Orange
		emitter->SetFadeOut(true);
		
		// Emit burst of particles
		for (int i = 0; i < 50; ++i) {
			emitter->EmitParticle();
		}
	}
	
	// Configure emitter for rain effect
	inline void ConfigureRain(ParticleSystem::ParticleEmitter* emitter) {
		emitter->SetEmissionRate(100.0f);
		emitter->SetLifetimeRange(1.0f, 2.0f);
		emitter->SetSpeedRange(200.0f, 300.0f);  // Fast downward
		emitter->SetSizeRange(2.0f, 4.0f);
		emitter->SetColor(Color(0.5f, 0.5f, 1.0f, 0.6f));  // Light blue
		emitter->SetGravity(100.0f);  // Strong gravity for rain
		emitter->SetFadeOut(false);
	}
	
	// Configure emitter for magic/energy effect
	inline void ConfigureMagic(ParticleSystem::ParticleEmitter* emitter) {
		emitter->SetEmissionRate(40.0f);
		emitter->SetLifetimeRange(1.0f, 2.0f);
		emitter->SetSpeedRange(60.0f, 120.0f);
		emitter->SetSizeRange(8.0f, 12.0f);
		emitter->SetColor(Color(0.5f, 0.2f, 1.0f, 1.0f));  // Purple
		emitter->SetGravity(0.0f);  // No gravity for floating magic particles
		emitter->SetFadeOut(true);
	}
	
	// Configure emitter for dust/debris effect
	inline void ConfigureDust(ParticleSystem::ParticleEmitter* emitter) {
		emitter->SetEmissionRate(25.0f);
		emitter->SetLifetimeRange(1.5f, 3.0f);
		emitter->SetSpeedRange(30.0f, 60.0f);
		emitter->SetSizeRange(5.0f, 10.0f);
		emitter->SetColor(Color(0.6f, 0.5f, 0.4f, 0.7f));  // Brown/tan
		emitter->SetFadeOut(true);
	}
}

#endif // !PARTICLEEFFECTS_HPP
