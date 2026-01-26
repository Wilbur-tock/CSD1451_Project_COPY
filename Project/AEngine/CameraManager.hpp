#pragma once
#include "AEEngine.h"

namespace CameraController
{
    enum class CameraState
    {
        STATIC,      // Camera doesn't move
        INPUT,       // Camera controlled by WASD input
        FOLLOW       // Camera follows a target
    };

    void Init();
    void Update(float dt);
    void SetPosition(const AEVec2& pos);
    void SetBounds(float minX, float maxX, float minY, float maxY);
    void SetState(CameraState newState);
    void SetFollowTarget(const AEVec2* target);
    void SetFollowSpeed(float speed);

    AEVec2 GetPosition();
    CameraState GetState();

    void SetMoveSpeed(float speed);
}