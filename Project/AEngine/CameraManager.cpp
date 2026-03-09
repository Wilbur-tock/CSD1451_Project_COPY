#include "pch.hpp"
#include "CameraManager.hpp"
#include "GameStateManager.hpp"

namespace CameraManager
{
    // Define the global camera instance
    CameraController camera;

    CameraController::CameraController()
        : position{ 0.0f, 0.0f }
        , moveSpeed(300.0f)
        , followSpeed(5.0f)
        , boundMinX(-FLT_MAX)
        , boundMaxX(FLT_MAX)
        , boundMinY(-FLT_MAX)
        , boundMaxY(FLT_MAX)
        , useBounds(false)
        , currentState(CameraState::INPUT)
        , followTarget(nullptr)
        , zoomLevel(1.0f)
        , zoomSpeed(2.5f)
        , zoomMin(0.5f)
        , zoomMax(2.0f)
        , isZoomAllow(true)
    {
    }

    void CameraController::ClampPosition()
    {
        if (!useBounds) return;

        // Half screen in world units depends on current zoom.
        // As zoom decreases (zoomed out), halfW/halfH grow, shrinking the
        // allowed camera center range and pushing the camera inward so that
        // screen edges never exceed the world bounds.
        float halfW = (Config::SCREEN_WIDTH * 0.5f) / zoomLevel;
        float halfH = (Config::SCREEN_HEIGHT * 0.5f) / zoomLevel;

        position.x = std::clamp(position.x, boundMinX + halfW, boundMaxX - halfW);
        position.y = std::clamp(position.y, boundMinY + halfH, boundMaxY - halfH);
    }

    void CameraController::Init()
    {
        position = { 0.0f, 0.0f };
        moveSpeed = 300.0f;
        followSpeed = 5.0f;
        useBounds = false;
        currentState = CameraState::INPUT;
        followTarget = nullptr;

        AEGfxSetCamPosition(position.x, position.y);

        camera.SetMoveSpeed(1500.0f); // Set how fast the camera scrolls in world units per second

        // Set world boundariesm, prevents player from scolling outside the playable area
        camera.SetBounds(
            Config::WORLD_MIN_X, // Left boundary
            Config::WORLD_MAX_X, // Right boundary
            Config::WORLD_MIN_Y, // Bottom boundary
            Config::WORLD_MAX_Y  // Top boundary
        );
    }

    void CameraController::Update(float dt)
    {
        switch (currentState)
        {
        case CameraState::STATIC:
            zoomLevel = 1.0f;
            break;

        case CameraState::INPUT:
        {
            AEVec2 movement = { 0.0f, 0.0f };

            if (AEInputCheckCurr(AEVK_W)) movement.y += 1.0f;
            if (AEInputCheckCurr(AEVK_S)) movement.y -= 1.0f;
            if (AEInputCheckCurr(AEVK_A)) movement.x -= 1.0f;
            if (AEInputCheckCurr(AEVK_D)) movement.x += 1.0f;

            float length = AEVec2Length(&movement);
            if (length > 0.0f)
            {
                AEVec2Scale(&movement, &movement, 1.0f / length);
                AEVec2Scale(&movement, &movement, (moveSpeed / zoomLevel) * dt);
                AEVec2Add(&position, &position, &movement);
            }

            if (!isZoomAllow) break;

            s32 scrollDelta = 0;
            AEInputMouseWheelDelta(&scrollDelta);
            if (scrollDelta > 0)        ZoomIn(dt);
            else if (scrollDelta < 0)   ZoomOut(dt);
            break;
        }
        case CameraState::FOLLOW:
        {
            if (followTarget != nullptr)
            {
                AEVec2 targetPos = *followTarget;
                AEVec2 direction;
                AEVec2Sub(&direction, &targetPos, &position);

                float lerpFactor = 1.0f - powf(0.001f, dt * followSpeed);
                AEVec2Scale(&direction, &direction, lerpFactor);
                AEVec2Add(&position, &position, &direction);
            }
            break;
        }
        }

        // Clamp position after ALL movement (WASD, follow lerp) using zoom-aware bounds
        ClampPosition();

        // Floor position for pixel-perfect rendering
        position.x = floorf(position.x);
        position.y = floorf(position.y);

        AEGfxSetCamPosition(position.x, position.y);
    }

    void CameraController::SetPosition(const AEVec2& pos)
    {
        position = pos;

        // Apply boundary clamping if enabled
        if (useBounds)
        {
            if (position.x < boundMinX) position.x = boundMinX;
            if (position.x > boundMaxX) position.x = boundMaxX;
            if (position.y < boundMinY) position.y = boundMinY;
            if (position.y > boundMaxY) position.y = boundMaxY;
        }

        // Floor position for pixel-perfect rendering
        position.x = floorf(position.x);
        position.y = floorf(position.y);

        AEGfxSetCamPosition(position.x, position.y);
    }

    void CameraController::SetBounds(float minX, float maxX, float minY, float maxY)
    {
        boundMinX = minX;
        boundMaxX = maxX;
        boundMinY = minY;
        boundMaxY = maxY;
        useBounds = true;
    }

    void CameraController::SetState(CameraState newState)
    {
        currentState = newState;
    }

    void CameraController::SetFollowTarget(const AEVec2* target)
    {
        followTarget = target;
    }

    void CameraController::SetFollowSpeed(float speed)
    {
        followSpeed = speed;
    }

    AEVec2 CameraController::GetPosition() const
    {
        return position;
    }

    CameraController::CameraState CameraController::GetState() const
    {
        return currentState;
    }

    std::string CameraController::PrintState() const
    {
        switch (currentState)
        {
        case CameraState::STATIC: return "STATIC";
        case CameraState::INPUT: return "INPUT";
        case CameraState::FOLLOW: return "FOLLOW";
        default: return "UNKNOWN";
        }
	}

    std::string CameraController::PrintVisibleTiles() const
    {
        float visibleW = static_cast<float>(Config::SCREEN_WIDTH) / (static_cast<float>(Config::TILE_SIZE) * zoomLevel);
        float visibleH = static_cast<float>(Config::SCREEN_HEIGHT) / (static_cast<float>(Config::TILE_SIZE) * zoomLevel);

        return "Visible Tiles Width: " + Utils::FormatFloat(visibleW, 2)
            + " Height: " + Utils::FormatFloat(visibleH, 2);
	}

    void CameraController::SetMoveSpeed(float speed)
    {
        moveSpeed = speed;
    }

    // ===== ZOOM FUNCTION =====

    float CameraController::GetZoom() const
    {
        return zoomLevel;
    }

    void CameraController::SetZoom(float zoom)
    {
        zoomLevel = std::clamp(zoom, zoomMin, zoomMax);
    }

    void CameraController::SetZoomSpeed(float speed)
    {
        zoomSpeed = speed;
    }

    void CameraController::SetZoomBounds(float minZoom, float maxZoom)
    {
        zoomMin = minZoom;
        zoomMax = maxZoom;
    }

    void CameraController::ZoomIn(float dt)
    {
        zoomLevel += zoomSpeed * dt;
        if (zoomLevel > zoomMax) zoomLevel = zoomMax;
        ClampPosition();
    }

    void CameraController::ZoomOut(float dt)
    {
        zoomLevel -= zoomSpeed * dt;
        if (zoomLevel < zoomMin) zoomLevel = zoomMin;
        ClampPosition();
    }

    void CameraController::SetZoomAllow(bool isAllow) {
        isZoomAllow = isAllow;
    }

}