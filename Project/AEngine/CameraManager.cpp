
#pragma once
#include "cameraManager.hpp"
#include "pch.hpp"

namespace CameraController
{
    namespace
    {
        AEVec2 position = { 0.0f, 0.0f };
        float moveSpeed = 300.0f;
        float followSpeed = 5.0f;

        // Boundary limits
        float boundMinX = -FLT_MAX;
        float boundMaxX = FLT_MAX;
        float boundMinY = -FLT_MAX;
        float boundMaxY = FLT_MAX;
        bool useBounds = false;

        // Camera state
        CameraState currentState = CameraState::INPUT;
        const AEVec2* followTarget = nullptr;
    }

    void Init()
    {
        position = { 0.0f, 0.0f };
        moveSpeed = 300.0f;
        followSpeed = 5.0f;
        useBounds = false;
        currentState = CameraState::INPUT;
        followTarget = nullptr;

        AEGfxSetCamPosition(position.x, position.y);
    }

    void Update(float dt)
    {
        switch (currentState)
        {
        case CameraState::STATIC:
            // Camera doesn't move, do nothing
            break;

        case CameraState::INPUT:
        {
            AEVec2 movement = { 0.0f, 0.0f };

            // WASD movement
            if (AEInputCheckCurr(AEVK_W))
                movement.y += 1.0f;
            if (AEInputCheckCurr(AEVK_S))
                movement.y -= 1.0f;
            if (AEInputCheckCurr(AEVK_A))
                movement.x -= 1.0f;
            if (AEInputCheckCurr(AEVK_D))
                movement.x += 1.0f;

            // Normalize diagonal movement
            float length = AEVec2Length(&movement);
            if (length > 0.0f)
            {
                AEVec2Scale(&movement, &movement, 1.0f / length);
                AEVec2Scale(&movement, &movement, moveSpeed * dt);
                AEVec2Add(&position, &position, &movement);
            }
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

    void SetPosition(const AEVec2& pos)
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

    void SetBounds(float minX, float maxX, float minY, float maxY)
    {
        boundMinX = minX;
        boundMaxX = maxX;
        boundMinY = minY;
        boundMaxY = maxY;
        useBounds = true;
    }

    void SetState(CameraState newState)
    {
        currentState = newState;
    }

    void SetFollowTarget(const AEVec2* target)
    {
        followTarget = target;
    }

    void SetFollowSpeed(float speed)
    {
        followSpeed = speed;
    }

    AEVec2 GetPosition()
    {
        return position;
    }

    CameraState GetState()
    {
        return currentState;
    }

    void SetMoveSpeed(float speed)
    {
        moveSpeed = speed;
    }
}