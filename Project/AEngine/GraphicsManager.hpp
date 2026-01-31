#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Button.hpp"
#include "TextObject.hpp"

namespace GraphicsManager {
	extern AEGfxVertexList *pMeshMiddle, *pMeshCorner;

	// Load all resources required and generate mesh
	void Load();

	// Create the transform matrix to be used to draw out the texture on the mesh,
	// x,y - position of where the texture is going to be render at
	// rot - rotation, 
	// scaleX, scaleY, scale of the texture no ranges
	AEMtx33 CreateTransformMatrix(float x, float y, float rot = 0.f, float scaleX = 100.f, float scaleY = 100.f);

	// Draw game object on the map using pMeshMiddle rect mode middle
	void DrawGameObject(AEGfxTexture* pTex, AEMtx33 transform, Color color);

	// Draw UI texture using pMeshCorner rect mode corner
	void DrawUI(AEGfxTexture* pTex, AEMtx33 transform, Color color);

	void DrawUIFollowCam(AEGfxTexture* pTex, AEMtx33 transform, Color color);

	// Draw text UI using noramlized coordinates 
	// x,y, use range from -1.0f to 1.0f
	// textScale, use range from 0.0 to 1.0f
	void DrawTextUI(TextObject textObj);

	// Button class to store all the button information (string text, position, texture, color)
	void DrawButton(Button btn);
}

#endif // !GRAPHICSMANAGER_HPP
