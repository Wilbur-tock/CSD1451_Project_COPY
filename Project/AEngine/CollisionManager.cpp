#include "CollisionManager.hpp"

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
        //float mouseX = 0, mouseY = 0;
        //Utils::GetWorldMousePos(mouseX, mouseY);
        //Utils::GetWorldPos(x, y);
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);

        // Add offset to the mouse position
        mouseX -= static_cast<s32>(Config::SCREEN_WIDTH / 2.0f);
        mouseY -= static_cast<s32>(Config::SCREEN_HEIGHT / 2.0f);

        // Calculate Bounding Box
        float halfW = w/ 2.0f;
        float halfH = h/ 2.0f;

        x += halfW;
        y += halfH;

        float left = x - halfW;
        float right = x + halfW;
        float top = y + halfH;
        float bottom = y - halfH;

        //std::cout << mouseX << " " << mouseY << " | " << x << " " << y << " | " << left << " " << right << " " << top << " " << bottom << std::endl;

        return mouseX >= left && mouseX <= right && mouseY >= bottom && mouseY <= top;
    }
}