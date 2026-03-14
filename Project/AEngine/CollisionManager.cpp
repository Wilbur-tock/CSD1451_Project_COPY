#include "CollisionManager.hpp"
#include "Utils.hpp"

#include "GameObject.hpp"
#include "BaseTypes.hpp"
#include "Collider.hpp"

namespace CollisionManager {
    bool AreCircleSquareIntersecting(Transform const& circle, Transform const& square) {
        // Calculate Bounds
        float halfW = square.size.x * 0.5f;
        float halfH = square.size.y * 0.5f;

        float minX = square.pos.x - halfW;
        float maxX = square.pos.x + halfW;
        float minY = square.pos.y - halfH;
        float maxY = square.pos.y + halfH;

        // Find closest point on box to circle center
        float closestX = std::clamp(circle.pos.x, minX, maxX);
        float closestY = std::clamp(circle.pos.y, minY, maxY);

        // Distance math
        float dx = circle.pos.x - closestX;
        float dy = circle.pos.y - closestY;

        float distanceSq = (dx * dx) + (dy * dy);
        float radius = circle.size.x * 0.5f;

        return distanceSq - 0.01f <= (radius * radius);
    }

    bool AreCircleSquareIntersecting(float cX, float cY, float r, float sX, float sY, float sWidth, float sHeight) {
        // Calculate the bounds of the square
        float halfW = sWidth / 2.0f;
        float halfH = sHeight / 2.0f;

        float minX = sX - halfW;
        float maxX = sX + halfW;
        float minY = sY - halfH;
        float maxY = sY + halfH;

        // Find the closest point on the AABB to the circle center
        float closestX = std::clamp(cX, minX, maxX);;
        float closestY = std::clamp(cY, minY, maxY);;

        // Calculate the distance from the circle center to this closest point
        float distanceX = cX - closestX;
        float distanceY = cY - closestY;

        // Use the squared distance to check collision
        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

        return distanceSquared < (r * r);
    }

    bool IsPointInGameObject(GameObject const& obj) {
        float mouseX = 0, mouseY = 0;
        Utils::GetWorldMousePos(mouseX, mouseY);

        if (obj.collider.type == CollisionType::COLLIDER_BOX) {
            float halfW = obj.transform.size.x * 0.5f;
            float halfH = obj.transform.size.y * 0.5f;

            // Calculate the bounding box in WORLD COORDINATES ONLY
            float left      = obj.transform.pos.x - halfW;
            float right     = obj.transform.pos.x + halfW;
            float top       = obj.transform.pos.y + halfH;
            float bottom    = obj.transform.pos.y - halfH;

            return (mouseX >= left && mouseX <= right && mouseY <= top && mouseY >= bottom);
        }
        else if (obj.collider.type == CollisionType::COLLIDER_CIRCLE) {
            // Calculate the distance on each axis
            float dx = mouseX - obj.transform.pos.x;
            float dy = mouseY - obj.transform.pos.y;

            // Use the squared distance to avoid a slow square root (sqrt) operation
            float distanceSquared = (dx * dx) + (dy * dy);

            // Compare against the squared radius
            float radius = obj.transform.size.x * 0.5f;
            float radiusSquared = radius * radius;

            return distanceSquared <= radiusSquared;
        }

        return false;
    }

    bool IsPointInRect(float x, float y, float w, float h) {
        s32 mouseX = 0, mouseY = 0;
        AEInputGetCursorPosition(&mouseX, &mouseY);

        // Convert mouse position to world position but without the camera movement adjustment
        mouseX -= static_cast<s32>(Config::SCREEN_WIDTH / 2.0f);
        mouseY = static_cast<s32>(Config::SCREEN_HEIGHT / 2.0f - mouseY);

        // Calculate Bounding Box
        float left = x;
        float right = x + w;
        float top = y;           // Y is the top
        float bottom = y - h;    // It grows down, so bottom is Y minus height

        return (mouseX >= left && mouseX <= right && mouseY <= top && mouseY >= bottom);
    }

    bool IsHorizontalCollision(Transform const& circle, Transform const& square) {
        float bias = 0.1f;
        float radius = circle.size.x * 0.5f;
        float overlapX = (radius + square.size.x / 2.0f) - std::abs(circle.pos.x - square.pos.x);
        float overlapY = (radius + square.size.y / 2.0f) - std::abs(circle.pos.y - square.pos.y);
        return overlapX < (overlapY + bias);
    }

    bool CheckContinuousCollision(AEVec2 start, AEVec2 end, Transform const& target) {
        AEVec2 path = { end.x - start.x, end.y - start.y };
        AEVec2 toTarget = { target.pos.x - start.x, target.pos.y - start.y };

        float pathLenSq = (path.x * path.x) + (path.y * path.y);
        float projRadius = 5.0f; 
        float targetRadius = target.size.x * 0.5f;
        float totalRadiusSq = (projRadius + targetRadius) * (projRadius + targetRadius);

        // If the projectile didn't move, just do a normal circle check
        if (pathLenSq == 0) {
            float dx = end.x - target.pos.x;
            float dy = end.y - target.pos.y;
            return (dx * dx + dy * dy) <= totalRadiusSq;
        }

        // Project toTarget onto path, clamped between 0 and 1
        float t = std::clamp((toTarget.x * path.x + toTarget.y * path.y) / pathLenSq, 0.0f, 1.0f);
        AEVec2 closestPoint = { start.x + t * path.x, start.y + t * path.y };

        float dx = target.pos.x - closestPoint.x;
        float dy = target.pos.y - closestPoint.y;
        float distSq = (dx * dx + dy * dy);

        //if (distSq < totalRadiusSq * 10.0f) {
        //    std::cout << "[Collision Debug]\n"
        //        << "  Proj Path: (" << start.x << "," << start.y << ") to (" << end.x << "," << end.y << ")\n"
        //        << "  Target Pos: (" << target.pos.x << "," << target.pos.y << ")\n"
        //        << "  DistSq: " << distSq << " | Threshold: " << totalRadiusSq << "\n"
        //        << "  Result: " << (distSq <= totalRadiusSq ? "!!! HIT !!!" : "MISS") << "\n"
        //        << "-----------------------" << std::endl;
        //}

        return distSq <= totalRadiusSq;
    }
}