#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "Button.hpp"
#include "TextObject.hpp"
#include "GameObject.hpp"

// Contains all the drawing of graphics/UI functions in here
namespace GraphicsManager {
	extern AEGfxVertexList *pMeshMiddle, *pMeshCorner;

	// Load all resources required and generate mesh
	void Load();
	void Unload();

	// Create the transform matrix to be used to draw out the texture on the mesh,
	// x,y - position of where the texture is going to be render at
	// rot - rotation, 
	// scaleX, scaleY, scale of the texture no ranges
	AEMtx33 CreateTransformMatrix(float x, float y, float rot = 0.f, float scaleX = 100.f, float scaleY = 100.f);

	// Draw game object on the map using pMeshMiddle rect mode middle
	void DrawGameObject(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color);
	// Draw game object on the map using pMeshMiddle rect mode middle
	void DrawGameObject(GameObject const& gameObject);

	// Draw UI texture using pMeshCorner (rect mode corner origin) using WORLD COORDINATES
	void DrawUI(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color);
	// Draw UI texture using pMeshCorner (rect mode corner origin) using WORLD COORDINATES
	void DrawUIFollowCam(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color);
}

#endif // !GRAPHICSMANAGER_HPP
