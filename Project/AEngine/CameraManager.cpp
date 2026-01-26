#include "CameraManager.hpp"
#include "pch.hpp"

namespace CameraController
{
    namespace
    {
        AEVec2 position = { 0.0f, 0.0f };
        float moveSpeed = 300.0f;

        // Boundary limits
        float boundMinX = -FLT_MAX;
        float boundMaxX = FLT_MAX;
        float boundMinY = -FLT_MAX;
        float boundMaxY = FLT_MAX;
        bool useBounds = false;
    }

    void Init()
    {
        
        position = { 0.0f, 0.0f };
        moveSpeed = 300.0f;
        useBounds = false;

        AEGfxSetCamPosition(position.x, position.y);
    }

    void Update(float dt)
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

    AEVec2 GetPosition()
    {
        return position;
    }

    void SetMoveSpeed(float speed)
    {
        moveSpeed = speed;
    }
}