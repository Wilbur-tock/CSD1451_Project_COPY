#include "pch.hpp"
#include "CameraManager.hpp"

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
        , zoomSpeed(1.0f)
        , zoomMin(0.25f)
        , zoomMax(3.0f)
        , isZoomAllow(true)
    {
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
    }

    void CameraController::Update(float dt)
    {
        switch (currentState)
        {
        case CameraState::STATIC:
            // Camera doesn't move, do nothing

			// Set camera zoom to default when in static mode
            zoomLevel = 1.0f;

            break;

        case CameraState::INPUT:
        {
            AEVec2 movement = { 0.0f, 0.0f };

            // WASD movement
            if (AEInputCheckCurr(AEVK_W)) movement.y += 1.0f;
            if (AEInputCheckCurr(AEVK_S)) movement.y -= 1.0f;
            if (AEInputCheckCurr(AEVK_A)) movement.x -= 1.0f;
            if (AEInputCheckCurr(AEVK_D)) movement.x += 1.0f;

            // Normalize diagonal movement
            float length = AEVec2Length(&movement);
            if (length > 0.0f)
            {
                AEVec2Scale(&movement, &movement, 1.0f / length);
                // AEVec2Scale(&movement, &movement, moveSpeed * dt);
				AEVec2Scale(&movement, &movement, (moveSpeed / zoomLevel) * dt); // Scale movement by zoom level 
                AEVec2Add(&position, &position, &movement);
            }

            if (!isZoomAllow) break;

            //if (AEInputCheckCurr(AEVK_Q))  // Zoom out
            //    ZoomOut(dt);
            //if (AEInputCheckCurr(AEVK_E))  // Zoom in
            //    ZoomIn(dt);

            // Zoom with mouse scroll wheel
            s32 scrollDelta = 0;
            AEInputMouseWheelDelta(&scrollDelta);
            if (scrollDelta > 0)          // Scroll away from user = zoom in
                ZoomIn(dt * static_cast<float>(scrollDelta));
            else if (scrollDelta < 0)     // Scroll towards user = zoom out
                ZoomOut(dt * static_cast<float>(-scrollDelta));

            break;
        }

        case CameraState::FOLLOW:
        {
            if (followTarget != nullptr)
            {
                // Smooth lerp towards target
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

        // Apply camera transformation to Alpha Engine
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
    }

    void CameraController::ZoomOut(float dt)
    {
        zoomLevel -= zoomSpeed * dt;
        if (zoomLevel < zoomMin) zoomLevel = zoomMin;
    }

    void CameraController::SetZoomAllow(bool isAllow) {
        isZoomAllow = isAllow;
    }
}