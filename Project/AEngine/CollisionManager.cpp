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

    bool IsPointInRect(float x, float y, float w, float h) {
        s32 mouseX = 0, mouseY = 0;
        AEInputGetCursorPosition(&mouseX, &mouseY);

        // Convert mouse position to world position but without the camera movement adjustment
        mouseX -= static_cast<s32>(Config::SCREEN_WIDTH / 2.0f);
        mouseY = static_cast<s32>(Config::SCREEN_HEIGHT / 2.0f - mouseY);

        // Calculate Bounding Box
        float left = x;
        float right = x + w;
        float top = y;        // Y is the top
        float bottom = y - h;    // It grows down, so bottom is Y minus height

        //std::cout << mouseX << " " << mouseY << " | " << x << " " << y << " | " << left << " " << right << " " << top << " " << bottom << std::endl;

        return (mouseX >= left && mouseX <= right && mouseY <= top && mouseY >= bottom);
    }
}