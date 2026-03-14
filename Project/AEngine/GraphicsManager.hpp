#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include "pch.hpp"

#include "Color.hpp"
#include "GameObject.hpp"

// Contains all the drawing of graphics/UI functions in here
namespace GraphicsManager {
	extern AEGfxVertexList *pMeshCenter, *pMeshTopLeftCorner, *pSpriteMesh;

	// Load all resources required and generate mesh
	void Load();
	void Unload();

	// Generate the mesh to draw on e.g.paper to draw on. There are TWO type of mesh, 
	// center origin and top left corner origin, two commonly used origin mode in the game
	void GenerateMesh(AEGfxVertexList*& pMesh, bool isOriginCenter = true);

	// Create the transform matrix to be used to draw out the texture on the mesh,
	// x,y - WORLD COORDINATES position of WHERE the texture is going to be render at
	// rot - rotation, 
	// scaleX, scaleY, size of the texture
	AEMtx33 CreateTransformMatrix(float x, float y, float rot = 0.f, float scaleX = 100.f, float scaleY = 100.f);

	// Draw game object on the map using pMeshMiddle rect mode middle
	void DrawGameObject(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color);
	// Draw game object on the map using pMeshMiddle rect mode middle
	void DrawGameObject(GameObject const& gameObject);
	void DrawCharacter(GameObject const& gameObject);

	// Draw UI texture using pMeshCorner (rect mode corner origin) using WORLD COORDINATES
	void DrawWorldUI(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color);
	// Draw UI texture using pMeshCorner (rect mode corner origin) using WORLD COORDINATES
	void DrawScreenOverlay(AEGfxTexture* pTex, AEMtx33 const& transform, Color const& color);

	// Call once during Load() to build the sprite frame mesh
	void BuildSpriteMesh();
	// Draw a single frame from a horizontal sprite sheet in screen space
	// frame     – 0-based frame index
	// screenX/Y – center position of the sprite (screen coordinates)
	// w, h      – render size in pixels
	void DrawSpriteFrame(AEGfxTexture* tex, int frame, float screenX, float screenY, float w, float h);
	void DrawLine(AEVec2 pos, AEVec2 size);
	void DrawCircle(AEVec2 center, float radius);
}

#endif // !GRAPHICSMANAGER_HPP
