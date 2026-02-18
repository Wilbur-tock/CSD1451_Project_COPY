#include "UIManager.hpp"

#include "GraphicsManager.hpp"
#include "AssetsManager.hpp"

namespace UIManager {
    Bar::Bar(AEVec2 _pos, AEVec2 _size, Color _barColor, FillType _type)
        : pos{ _pos }, size{ _size }, barColor{ _barColor }, type{ _type } {}

	void DrawBar(Bar bar, float currValue, float maxValue) {
        AEGfxTexture* tex = AssetsManager::squareTexture;

        float x = bar.pos.x, y = bar.pos.y;
        float w = bar.size.x, h = bar.size.y;

        float halfWidth = bar.size.x / 2.0f;
        x -= halfWidth;

        // Lighter background bar
        Color color = { bar.barColor.Red(), bar.barColor.Green(), bar.barColor.Blue() };
        color.EditColorPercentage(0.8f);
        GraphicsManager::DrawUI(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, w, h), color);

        float percentage = (currValue / maxValue);
        float newW = w, newH = h;
        if (bar.type == RIGHT_TO_LEFT) { newW *= percentage; }
        else if (bar.type == TOP_TO_BOTTOM) { 
            y -= h; // change origin point to BOTTOM LEFT of the bar
            newH *= -percentage; 
        }

        // Bar
        GraphicsManager::DrawUI(tex, GraphicsManager::CreateTransformMatrix(x, y, 0, newW, newH), bar.barColor);
	}
}