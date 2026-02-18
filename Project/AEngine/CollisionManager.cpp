#include "CollisionManager.hpp"
#include "Utils.hpp"

namespace CollisionManager {
    bool AreCirclesIntersecting(float x1, float y1, float r1, float x2, float y2, float r2) {
        // Calculate the difference in coordinates
        float dx = x2 - x1;
        float dy = y2 - y1;

        float distanceSq = (dx * dx) + (dy * dy);

        float radiusSum = r1 + r2;
        float radiusSumSq = radiusSum * radiusSum;
        return distanceSq <= radiusSumSq;
    }

    bool AreCircleSquareIntersecting(float sX, float sY, float sWidth, float sHeight, float cX, float cY, float r) {
        // Calculate the bounds of the square
        float halfW = sWidth / 2.0f;
        float halfH = sHeight / 2.0f;

        float minX = sX - halfW;
        float maxX = sX + halfW;
        float minY = sY - halfH;
        float maxY = sY + halfH;

        // Find the closest point on the AABB to the circle center
        float closestX = cX;
        float closestY = cY;

        Utils::Limit(closestX, minX, maxX);
        Utils::Limit(closestY, minY, maxY);

        // Calculate the distance from the circle center to this closest point
        float distanceX = cX - closestX;
        float distanceY = cY - closestY;

        // Use the squared distance to check collision
        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

        return distanceSquared < (r * r);
    }

    bool IsPointInGameObject(GameObject const& obj) {
        if (obj.collider.type == COLLIDER_BOX) {
            float mouseX = 0, mouseY = 0;
            Utils::GetWorldMousePos(mouseX, mouseY);

            // Calculate the bounding box in WORLD COORDINATES ONLY
            float halfWidth = obj.size.x / 2.0f, halfHeight = obj.size.y / 2.0f;
            float left = obj.pos.x - halfWidth;
            float right = obj.pos.x + halfWidth;
            float top = obj.pos.y + halfHeight;
            float bottom = obj.pos.y - halfHeight;

            return (mouseX >= left && mouseX <= right && mouseY <= top && mouseY >= bottom);
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
}