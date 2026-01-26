#pragma once
#include "pch.hpp"

namespace CameraController
{
    void Init();
    void Update(float dt);
    void SetPosition(const AEVec2& pos);
    void SetBounds(float minX, float maxX, float minY, float maxY);

    AEVec2 GetPosition();

    void SetMoveSpeed(float speed);
}