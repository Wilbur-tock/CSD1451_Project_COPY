#include "ParticlesManager.hpp"
#include "pch.hpp"

namespace ParticleManager {

    ParticleController particleController;

    void ParticleController::Init(int _poolSize) {
        assert(_poolSize > 0 && "[ParticleManager] Pool size must be positive.");
        assert(pool.empty() && "[ParticleManager] Init called twice without Shutdown.");

        poolSize = _poolSize;
        nextFree = 0;

        pool.resize(poolSize);
        for (Particle& p : pool) {
            p = Particle{};
            p.alive = false;
        }

        CreateMesh();
        //std::cout << "[ParticleManager] Initialised with pool size: " << poolSize << '\n';
    }

    void ParticleController::Shutdown() {
        pool.clear();
        poolSize = 0;
        nextFree = 0;

        if (mesh) {
            AEGfxMeshFree(mesh);
            mesh = nullptr;
        }

        std::cout << "[ParticleManager] Shutdown complete.\n";
    }

    void ParticleController::Update(float dt) {
        if (pool.empty()) return;

        for (Particle& p : pool) {
            if (!p.alive) continue;

            p.lifetimeCurrent -= dt;
            if (p.lifetimeCurrent <= 0.f) {
                p.alive = false;
                continue;
            }

            // Integrate position
            p.position.x += p.velocity.x * dt;
            p.position.y += p.velocity.y * dt;

            // Integrate rotation
            p.rotation += p.rotationSpeed * dt;
        }
    }

    void ParticleController::Draw() {
        if (pool.empty() || !mesh) return;

        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        float  zoom = CameraManager::camera.GetZoom();
        AEVec2 camPos = CameraManager::camera.GetPosition();

        AEMtx33 matToOrigin, matScale, matBack, zoomMat;
        AEMtx33Trans(&matToOrigin, -camPos.x, -camPos.y);
        AEMtx33Scale(&matScale, zoom, zoom);
        AEMtx33Trans(&matBack, camPos.x, camPos.y);
        AEMtx33Concat(&zoomMat, &matScale, &matToOrigin);
        AEMtx33Concat(&zoomMat, &matBack, &zoomMat);

        for (const Particle& p : pool) {
            if (!p.alive) continue;

            float t = p.NormalizedAge();

            float cr = p.colorStart.Red() + (p.colorEnd.Red() - p.colorStart.Red()) * t;
            float cg = p.colorStart.Green() + (p.colorEnd.Green() - p.colorStart.Green()) * t;
            float cb = p.colorStart.Blue() + (p.colorEnd.Blue() - p.colorStart.Blue()) * t;
            float ca = p.colorStart.Alpha() + (p.colorEnd.Alpha() - p.colorStart.Alpha()) * t;
            Color color(cr, cg, cb, ca);

            // Lerp scale
            float scale = p.scaleStart + (p.scaleEnd - p.scaleStart) * t;

            // Build transform: scale -> rotate -> translate
            AEMtx33 mtxScale, mtxRot, mtxTrans, mtxResult;
            AEMtx33Scale(&mtxScale, scale, scale);
            AEMtx33Rot(&mtxRot, p.rotation);
            AEMtx33Trans(&mtxTrans, p.position.x, p.position.y);
            AEMtx33Concat(&mtxResult, &mtxRot, &mtxScale);
            AEMtx33Concat(&mtxResult, &mtxTrans, &mtxResult);
            AEMtx33Concat(&mtxResult, &zoomMat, &mtxResult);

            AEGfxSetColorToMultiply(color.Red(), color.Green(), color.Blue(), color.Alpha());
            AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
            AEGfxSetTransparency(color.Alpha());
            AEGfxSetTransform(mtxResult.m);
            AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
        }

        // Restore defaults
        AEGfxSetBlendMode(AE_GFX_BM_NONE);
        AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
        AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
        AEGfxSetTransparency(1.f);
    }

    void ParticleController::Emit(const EmitParams& params) {
        if (pool.empty()) {
            std::cerr << "[ParticleManager] Emit called before Init!\n";
            return;
        }

        for (int n = 0; n < params.count; ++n) {
            int slot = FindFreeSlot();
            if (slot < 0) {
                std::cerr << "[ParticleManager] Pool exhausted, dropping "
                    << (params.count - n) << " particle(s).\n";
                break;
            }

            Particle& p = pool[slot];
            p.alive = true;

            // ---- Position: random point within spawn radius ----------------
            float spawnAngle = RandRange(0.f, PI);
            float spawnRadius = RandRange(0.f, params.spawnRadius);
            p.position.x = params.spawnPosition.x + cosf(spawnAngle) * spawnRadius;
            p.position.y = params.spawnPosition.y + sinf(spawnAngle) * spawnRadius;

            // ---- Velocity --------------------------------------------------
            float dir = RandRange(params.directionMin, params.directionMax);
            float speed = RandRange(params.speedMin, params.speedMax);
            p.velocity.x = cosf(dir) * speed;
            p.velocity.y = sinf(dir) * speed;

            // ---- Rotation --------------------------------------------------
            p.rotation = RandRange(0.f, PI);
            p.rotationSpeed = RandRange(params.rotationSpeedMin, params.rotationSpeedMax);

            // ---- Scale -----------------------------------------------------
            p.scaleStart = params.scaleStart;
            p.scaleEnd = params.scaleEnd;

            // ---- Color -----------------------------------------------------
            p.colorStart = params.colorStart;
            p.colorEnd = params.colorEnd;

            // ---- Lifetime --------------------------------------------------
            float life = RandRange(params.lifetimeMin, params.lifetimeMax);
            p.lifetimeMax = life;
            p.lifetimeCurrent = life;
        }
    }

    void ParticleController::Clear() {
        for (Particle& p : pool) {
            p.alive = false;
        }
        nextFree = 0;
        std::cout << "[ParticleManager] All particles cleared.\n";
    }

    int ParticleController::ActiveCount() const {
        int count = 0;
        for (const Particle& p : pool) {
            if (p.alive) ++count;
        }
        return count;
    }

    int ParticleController::FindFreeSlot() {
        for (int checked = 0; checked < poolSize; ++checked) {
            int idx = (nextFree + checked) % poolSize;
            if (!pool[idx].alive) {
                nextFree = (idx + 1) % poolSize;
                return idx;
            }
        }
        return -1; // Pool is full
    }

    void ParticleController::CreateMesh() {
        // Unit square centred at origin (-0.5 to 0.5 on both axes)
        AEGfxMeshStart();

        AEGfxTriAdd(
            -0.5f, -0.5f, 0xFFFFFFFF, 0.f, 1.f,
            0.5f, -0.5f, 0xFFFFFFFF, 1.f, 1.f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.f, 0.f
        );
        AEGfxTriAdd(
            0.5f, -0.5f, 0xFFFFFFFF, 1.f, 1.f,
            0.5f, 0.5f, 0xFFFFFFFF, 1.f, 0.f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.f, 0.f
        );

        mesh = AEGfxMeshEnd();
        assert(mesh && "[ParticleManager] Failed to create particle mesh.");
    }

    float ParticleController::RandRange(float min, float max) {
        if (min >= max) return min;
        float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return min + t * (max - min);
    }

} // namespace ParticleManager